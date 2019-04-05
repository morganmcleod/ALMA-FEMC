/**************************************
 * Monitor and control software for:  *
 * FRONT END THERMAL INTERLOCK MODULE *
 *                                    *
 * avaccari@nrao.edu                  *
 **************************************
 @(#) $Id: FETIM.c,v 1.4 2013/04/29 17:10:30 jpisano Exp $
 Date           Version     Notes
 2011-05-09     01.00.00    First Release
 2011-09-20     01.00.01    - Modified to support different hardware for the
                              temperature sensors. The new scale goes 0=>60C
                              instead of -20=>60C.
                            - Corrected length of AREG_OUT data (from 2 to 3
                              bits)
                            - Added command line option for extra clock cycles
                              and left shift amount for output registers
                            - Changed default extra clocks for output register
                              to two with data shifted left by two
                            - Added command line option for extra clock cycles
                              for the parallel ADC strobe
                            - Added menu options to perform continuous monitor
                              operation just for the following one at the time:
                              - Everything
                              - Digital
                              - Parallel ADC Analog
                              - Serial ADC Analog
                            - Added command line option for extra clock cycles
                              for the serial ADC strobe
                            - Added one extra bit at the size of the serial ADC
                              size. This goes first to compensate for the CS
                            - Inverted behavior of the glitch counter status:
                              2V now corresponds to 0% while 0V corresponds to
                              100%
                            - Fixed a bug with unit display
 2011-09-20     01.00.02    - Modified temp scaling factors:
                              offset from 0.0 to -20
                              scaling from 30 to 40

*/
// Version string global
//char version[16]="01.00.02-A";//try: $Revision: 1.4 $ & let cvs do the work...
char version[16]="$Revision: 1.5 $";


// Includes
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <conio.h>
#include <i86.h>
#include <stdlib.h>


// ISA bus I/O mapped commands
#define PORT_SELECT     0x300
#define DATA_10         0x302
#define WRITE_LENGTH    0x308
#define READ_LENGTH     0x30A
#define COMMAND         0x30C
#define BUSY_STATUS     0x30C
#define FPGA_CHECK      0x30E

// Command Words
#define PARALLEL_WRITE_AREG_OUT 0x00
#define PARALLEL_WRITE_BREG_OUT 0x01
#define PARALLEL_WRITE_CREG_OUT 0x02
#define PARALLEL_WRITE_DREG_OUT 0x03
#define SERIAL_ADC_ACCESS       0x04
#define PARALLEL_ADC_STROBE     0x06
#define PARALLEL_READ_AREG_IN   0x08
#define PARALLEL_READ_BREG_IN   0x09
#define PARALLEL_READ_CREG_IN   0x0A

// Other Interface definitions
#define FETIM_CH                        24
#define AREG_OUT_PARALLEL_WRITE_SIZE    3
#define BREG_OUT_PARALLEL_WRITE_SIZE    2
#define CREG_OUT_PARALLEL_WRITE_SIZE    1
#define DREG_OUT_PARALLEL_WRITE_SIZE    1
#define AREG_IN_PARALLEL_READ_SIZE      8
#define BREG_IN_PARALLEL_READ_SIZE      12
#define CREG_IN_PARALLEL_READ_SIZE      5
#define SERIAL_ADC_ACCESS_SIZE          17
#define PARALLEL_ADC_STROBE_SIZE        6
#define PARALLEL_ADC                    0
#define SERIAL_ADC                      1
#define SSC_BUSY_MASK               0x0001
#define FPGA_READY                  0x5A5A

// Continuous monitor modes
#define CONT_MON_ALL        0
#define CONT_MON_DIG        1
#define CONT_MON_ANA_PAR    2
#define CONT_MON_ANA_SER    3


// Registers
// AREG_OUT
typedef struct {
    unsigned char   par_ch  :3;
    unsigned char           :5;
} AREG_OUT_BITF;

typedef union {
    unsigned char   uchr;
    AREG_OUT_BITF   bitf;
} AREG_OUT_UNION;




// BREG_OUT
typedef struct {
    unsigned char   ser_ch  :2;
    unsigned char           :6;
} BREG_OUT_BITF;

typedef union {
    unsigned char   uchr;
    BREG_OUT_BITF   bitf;
} BREG_OUT_UNION;




// CREG_OUT
typedef struct {
    unsigned char   n2fill  :1;
    unsigned char           :7;
} CREG_OUT_BITF;

typedef union {
    unsigned char   uchr;
    CREG_OUT_BITF   bitf;
} CREG_OUT_UNION;




// DREG_OUT
typedef struct {
    unsigned char   feState :1;
    unsigned char           :7;
} DREG_OUT_BITF;

typedef union {
    unsigned char   uchr;
    DREG_OUT_BITF   bitf;
} DREG_OUT_UNION;



// BREG_IN
typedef struct {
    unsigned int    aSensOR     :1;
    unsigned int    tSensOR     :1;
    unsigned int    glitchT     :1;
    unsigned int    shtdnT      :1;
    unsigned int    delayT      :1;
    unsigned int    sSensF      :1;
    unsigned int    mSensF      :1;
    unsigned int    cblItlk     :1;
    unsigned int    compLnk     :1;
    unsigned int    he2OR       :1;
    unsigned int    extT2OR     :1;
    unsigned int    extT1OR     :1;
    unsigned int                :4;
} BREG_IN_BITF;

typedef union {
    unsigned int    uint;
    BREG_IN_BITF    bitf;
} BREG_IN_UNION;




// Globals
AREG_OUT_UNION  aregOut;
BREG_OUT_UNION  bregOut;
CREG_OUT_UNION  cregOut;
DREG_OUT_UNION  dregOut;
BREG_IN_UNION   bregIn;

unsigned char   lastParAdcUint=0;
unsigned int    lastSerAdcUint=0;
float           lastParAdcFloat=0.0;
float           lastSerAdcFloat=0.0;

unsigned char   parUnit[3];
unsigned char   serUnit[4];

unsigned char   hardwareVersion=0;

unsigned char   extraClocks=2;      // Two extra clock cycles by default
unsigned char   leftShift=2;        // Left shift by two clocks cycle by default
unsigned char   extraParStrbClks=0; // Extra clock cycles for the par ADC strobe
unsigned char   extraSerStrbClks=0; // Extra clock cycles for the ser ADC strobe


// Position the cursor to the desired location (x -> row, y -> column)
void cursorAt(unsigned char x, unsigned char y){

    union REGS r;

    // Check for limits
    if((x>24)||(y>79)){
        return;
    }

    r.h.ah = 2;
    r.h.bh = 0;
    r.h.dl = y;
    r.h.dh = x;

    int86(0x10,&r,&r);
}



// Printf at a particular location
void printfAt(unsigned char x, unsigned char y, char * format, ... ){

    va_list arglist;

    cursorAt(x,y);

    va_start( arglist, format );
    vprintf( format, arglist );
    va_end( arglist );

    flushall(); // Make sure everything is printed
}



// Wait for a key to be pressed
int waitKey(char key){

	int choice;

	fflush(stdin); // Make sure the input buffer is empty

	switch(key){
		case ' ':
			printfAt(24,54,"Press ENTER to continue...");
			while(!getche());
			return 0;
			break;
		default:
			choice=getche();
			break;
	}

	return choice;
}



// Writes error on the screen
void errorState(char *message){
    if(message){
        printfAt(24,0,message);
        waitKey(' ');
    } else {
        printfAt(24,0,"No errors");
    }
}



// Check the busy status of the ssc (1 -> busy; 0 -> idle)
int checkSscBusy(void){
    unsigned char   cnt = 0xFF;
    int             tmp;

    do{
        tmp = inpw(BUSY_STATUS) & SSC_BUSY_MASK; // Read ssc busy status
        cnt--; // Counter to avoid hung-ups
    } while (tmp&&cnt);

    if(!cnt){
        errorState("SSC busy after 255 tries");
    }

    return tmp;
}




// Send strobe
void sendStrobe(unsigned char strobe){
    if(checkSscBusy()){ // Wait if ssc is busy
        return; // If busy return
    }
    outpw(WRITE_LENGTH,PARALLEL_ADC_STROBE_SIZE+extraParStrbClks); // Set the lenght of the write message
    outpw(COMMAND,strobe); // Send the strobe command
}



// Perform a parallel read operation and return value
int parallelRead(unsigned char command, unsigned char size){
    if(checkSscBusy()){ // Wait if ssc is busy
        return 0; // If busy return 0
    }
    outpw(READ_LENGTH,size); // Set the read back size
    outpw(COMMAND,command); // Perform the parallel read
    if(checkSscBusy()){ // Wait if ssc is busy
        return 0; // If busy return 0
    }
    return (inpw(DATA_10) & (0xFFFF>>(16-size))); // Return the read data
}





// Perform a parallel write operation
void parallelWrite(char command, int data, char size){
    if(checkSscBusy()){ // Wait if ssc is busy
        return; // If busy return
    }
    outpw(WRITE_LENGTH,size+extraClocks); // Set the write size + strobe
    outpw(DATA_10,data<<leftShift); // Store the data to be sent
    outpw(COMMAND,command); // Perform the parallel write
}




// Get the ADC data
unsigned int getAdc(char command, char size){
    if(checkSscBusy()){ // Wait if ssc is busy
        return 0; // If busy return 0
    }
    outpw(READ_LENGTH,size); // Set the read back size
    outpw(COMMAND, command); // Send the ADC read back command
    if(checkSscBusy()){ // Wait if ssc is busy
        return 0; // If busy return 0
    }

    return inpw(DATA_10); // Return the least significant 16 bits
}





// Perform an analog monitor
unsigned int analogMonitor(unsigned char adc, unsigned char reg){

    unsigned int tempAdcData=0;
/*

The right thing to do is to get to the bottom of the extra bit, seemingly being inserted by the serial bus logic in the Xilinx chip.     The expedient thing to do so that we can continue moving forward in our testing is to patch the test program FETIM.COM so that it accounts for the extra bit.

So please take the latest version which you built last week and make the following additional change to it.
The required patch is surrounded comments in the SERIAL_ADC case:


// Perform an analog monitor
unsigned int analogMonitor(unsigned char adc, unsigned char reg){

    unsigned int tempAdcData=0;

    switch(adc){
        case PARALLEL_ADC:
            parallelWrite(PARALLEL_WRITE_AREG_OUT,reg,AREG_OUT_PARALLEL_WRITE_SIZE); // Select the what to monitor with AREG
            sendStrobe(PARALLEL_ADC_STROBE); // Send conversion strobe command
            tempAdcData = getAdc(PARALLEL_READ_AREG_IN,AREG_IN_PARALLEL_READ_SIZE); // Read data from the parallel ADC and return value
            break;
        case SERIAL_ADC:
            parallelWrite(PARALLEL_WRITE_BREG_OUT,reg,BREG_OUT_PARALLEL_WRITE_SIZE); // Select the what to monitor with AREG
            tempAdcData = getAdc(SERIAL_ADC_ACCESS,SERIAL_ADC_ACCESS_SIZE+extraSerStrbClks); // Read data from the serial ADC and return value

// This bit-shift is a temporary patch to work around the FETIM serial interface apparently sending 13 bits instead of 12:
            tempAdcData >>= 1;
// End temporary patch

            break;
        default:
            break;
    }

    return tempAdcData;

}


Thanks
-MM 
*/
    switch(adc)
    {
        case PARALLEL_ADC:
            parallelWrite(PARALLEL_WRITE_AREG_OUT,reg,AREG_OUT_PARALLEL_WRITE_SIZE); // Select the what to monitor with AREG
            sendStrobe(PARALLEL_ADC_STROBE); // Send conversion strobe command
            tempAdcData = getAdc(PARALLEL_READ_AREG_IN,AREG_IN_PARALLEL_READ_SIZE); // Read data from the parallel ADC and return value
            break;
        case SERIAL_ADC:
            parallelWrite(PARALLEL_WRITE_BREG_OUT,reg,BREG_OUT_PARALLEL_WRITE_SIZE); // Select the what to monitor with AREG
            tempAdcData = getAdc(SERIAL_ADC_ACCESS,SERIAL_ADC_ACCESS_SIZE+extraSerStrbClks); // Read data from the serial ADC and return value
             // This bit-shift is a temporary patch to work around the FETIM serial interface apparently sending 13 bits instead of 12:
            tempAdcData >>= 1;
// End temporary patch
            break;
        default:
            break;
    }

    return tempAdcData;

}




// Scale to the appropriate value
float adcScaling(unsigned char adc, unsigned int data)
{

    float scaled=0.0;
    float scaling=0.0;
    float offset=0.0;

    switch(adc)
    {
        case PARALLEL_ADC:
            switch(aregOut.uchr)
            {
                case 0:
                case 1:
                case 2:
                case 3:
                case 4:
                   //JAP 4/13/2013: change requested by Morgan
                    //offset=0.0;
                    //scaling=30.0;
                    offset = -20.0;
                    scaling = 40.0;
                    //offset=0.0;
                    //scaling=30.0;
                    offset = -20.0;
                    scaling=40.0;
                    break;
                case 5:
                    offset=100.0;
                    scaling=-50.0;
                    break;
                case 6:
                case 7:

                    offset=0.0;
                    scaling=1.0;
                    break;
                default:
                    break;
            }
            scaled=offset+scaling*((2.0*data)/0x100);
            break;
        case SERIAL_ADC:
            switch(bregOut.uchr)
            {
                case 0:
                case 1:
                   //JAP 4/13/2013: change requested by Morgan
                    //offset=0.0;
                    //scaling=30.0

                    
                    offset = -20.0;
                    scaling  =40.0;

                    break;
                case 2:
                    offset=-1.0;
                    //MTM 5/14/2013: change to scaling for He buffer tank.
                    //scaling=1.25;
                    scaling=2.50;

                    break;
                default:
                    break;
            }
            scaled=offset+scaling*((4.0*data)/0x1000);
            break;
        default:
            break;
    }

    return scaled;
}





// Clear screen
void clearScreen(void){

    union REGS r;

    r.h.ah = 10;
    r.h.al = 32;
    r.h.bh = 0;
    r.x.cx = 2000;

    int86(0x10,&r,&r);
}





// Select Parallel ADC Units
void selectParUnit(void){
    switch(aregOut.bitf.par_ch){
        case 0:
            sprintf(parUnit,"C");
            break;
        case 1:
            sprintf(parUnit,"C");
            break;
        case 2:
            sprintf(parUnit,"C");
            break;
        case 3:
            sprintf(parUnit,"C");
            break;
        case 4:
            sprintf(parUnit,"C");
            break;
        case 5:
            sprintf(parUnit,"%%");
            break;
        case 6:
            sprintf(parUnit,"V");
            break;
        case 7:
            sprintf(parUnit,"V");
            break;
        default:
            sprintf(parUnit,"");
            break;
    }
}

// Select Parallel ADC Units
void selectSerUnit(void){
    switch(bregOut.bitf.ser_ch){
        case 0:
            sprintf(serUnit,"C");
            break;
        case 1:
            sprintf(serUnit,"C");
            break;
        case 2:
            sprintf(serUnit,"MPa");
            break;
        case 3:
        default:
            sprintf(serUnit,"");
            break;
    }
}

// Display current state
void currentState(void){

    selectParUnit();
    selectSerUnit();

    printfAt(0,0,"AREG: 0x%X (%04b) => PAR_ADC: 0x%03X (%012b) (%04u) (%04.3f %s)",
                 aregOut.uchr,aregOut.uchr,
                 lastParAdcUint,lastParAdcUint,lastParAdcUint,lastParAdcFloat,parUnit);
    printfAt(1,0,"BREG: 0x%X (%04b) => SAR_ADC: 0x%03X (%012b) (%04u) (%04.3f %s)",
                 bregOut.uchr,bregOut.uchr,
                 lastSerAdcUint,lastSerAdcUint,lastSerAdcUint,lastSerAdcFloat,serUnit);
    printfAt(2,0,"CREG: 0x%X (N2 Fill: %s)     DREG: 0x%X (FE Safe State: %s)",
                 cregOut.uchr,cregOut.uchr==0?"OFF":"ON",
                 dregOut.uchr,dregOut.uchr==0?"OFF":"ON");
    printfAt(3,0,"--------------------------------------------------------------------------------");

}





// Display digital operation result
//--------------------------------------------------------------------------
void displayDigital(void){
    printfAt(11,0,"--------------------------------------------------------------------------------");
    printfAt(13,2,"Airflow Sensor OOR:         %u    |    Multiple Sensor Failure:            %u",
                  bregIn.bitf.aSensOR, bregIn.bitf.mSensF);
    printfAt(14,2,"Temeprature Sensor OOR:     %u    |    Cable Interlock Status:             %u",
                  bregIn.bitf.tSensOR, bregIn.bitf.cblItlk);
    printfAt(15,2,"Glitch Counter Triggered:   %u    |    Compressor Link Interlock Status:   %u",
                  bregIn.bitf.glitchT, bregIn.bitf.compLnk);
    printfAt(16,2,"Shutdown Triggered:         %u    |    He Buffer Tank Pressure OOR:        %u",
                  bregIn.bitf.shtdnT, bregIn.bitf.he2OR);
    printfAt(17,2,"Interlock Delay Triggered:  %u    |    External Temperature Sensor 2 OOR:  %u",
                  bregIn.bitf.delayT, bregIn.bitf.extT2OR);
    printfAt(18,2,"Single Sensor Failure:      %u    |    External Temperature Sensor 1 OOR:  %u",
                  bregIn.bitf.sSensF, bregIn.bitf.extT1OR);
    printfAt(20,0,"--------------------------------------------------------------------------------");


}






// Display menu
int displayMenu(int menuNo){

    int menuChoice;

    clearScreen(); // Clear screen

    currentState(); // Display current state

    errorState(NULL); // Print Error state if any

    printfAt(5,5,"--- FETIM (Hardw: %d) (Softw: %s) ---", hardwareVersion, version);

    if(menuNo==1){
        printfAt(7,5,"q - Exit");
    } else {
        printfAt(7,5,"q - Back");
    }

    switch(menuNo){
        case 0:
            break;
        case 1: // Main menu
            printfAt(9,5,"1 - Perform single analog monitor operation");
            printfAt(10,5,"2 - Perform single digital monitor operation");
            printfAt(11,5,"3 - Perform continuous monitoring");
            printfAt(12,5,"4 - Perform continuous digital monitoring");
            printfAt(13,5,"5 - Perform continuous analog monitoring parallel");
            printfAt(14,5,"6 - Perform continuous analog monitoring serial");
            printfAt(15,5,"7 - Perform control operations");
            break;
        case 2: // Single Analog Monitor
            printfAt(9,5,"0 - Internal temperature sensor 1");
            printfAt(10,5,"1 - Internal temperature sensor 2");
            printfAt(11,5,"2 - Internal temperature sensor 3");
            printfAt(12,5,"3 - Internal temperature sensor 4");
            printfAt(13,5,"4 - Internal temperature sensor 5");
            printfAt(14,5,"5 - Glitch counter status");
            printfAt(15,5,"6 - Airflow sensor 1");
            printfAt(16,5,"7 - Airflow sensor 2");
            printfAt(17,5,"8 - External temperature sensor 1");
            printfAt(18,5,"9 - External temperature sensor 2");
            printfAt(19,5,"a - He buffer tank pressure");
            break;
        case 3: // Single Digital Monitor
            printfAt(9,5,"0 - Perform digital Monitor");
            displayDigital();
            break;
        case 4: // Control operation
            printfAt(9,5,"0 - Toggle the N2 fill system register");
            printfAt(10,5,"1 - Toggle the FE status fill system register");
            break;
        default:
            break;
    }

    cursorAt(24,79);

	menuChoice=waitKey(0);

	return menuChoice;
}





// Single Digital Monitor
//--------------------------------------------------------------------------
void singleDigitalMonitor(void){

    int menuChoice;

    do {
        switch(menuChoice=displayMenu(3)){
            case '0':
                bregIn.uint=parallelRead(PARALLEL_READ_BREG_IN,BREG_IN_PARALLEL_READ_SIZE);
                displayDigital();
                break;
            default:
                break;
        }

    } while (menuChoice!='q');
}






// Check that the fpga is up and running (0->OK, 1->BAD)
//--------------------------------------------------------------------------
int checkFpga(void){

    return (((inpw(FPGA_CHECK)!=FPGA_READY)&&(inpw(FPGA_CHECK+2)!=FPGA_READY))?1:0);
}




// Perform a single monitor operation
//--------------------------------------------------------------------------
void singleAnalogMonitor(void){


    int menuChoice;

    do {
        switch(menuChoice=displayMenu(2)){
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
                aregOut.uchr=menuChoice-48;
                lastParAdcUint=analogMonitor(PARALLEL_ADC,aregOut.bitf.par_ch);
                lastParAdcFloat=adcScaling(PARALLEL_ADC,lastParAdcUint);
                break;
            case '8':
            case '9':
                bregOut.uchr=menuChoice-48-8;
                lastSerAdcUint=analogMonitor(SERIAL_ADC,bregOut.bitf.ser_ch);
                lastSerAdcFloat=adcScaling(SERIAL_ADC,lastSerAdcUint);
                break;
            case 'a':
                bregOut.uchr=menuChoice-97+2;
                lastSerAdcUint=analogMonitor(SERIAL_ADC,bregOut.bitf.ser_ch);
                lastSerAdcFloat=adcScaling(SERIAL_ADC,lastSerAdcUint);
            default:
                break;
        }
    } while (menuChoice!='q');
}



// Perform continuous monitoring operation
//--------------------------------------------------------------------------
void continuousMonitoring(unsigned char mode){

    unsigned char item;
    unsigned char parallel[8][35]={"Internal temp sensor 1:",
                                   "Internal temp sensor 2:",
                                   "Internal temp sensor 3:",
                                   "Internal temp sensor 4:",
                                   "Internal temp sensor 5:",
                                   "Glitch counter status: ",
                                   "Airflow sensor 1:      ",
                                   "Airflow sensor 2:      "};
    unsigned char serial[3][35]={"External temp sensor 1:",
                                 "External temp sensor 2:",
                                 "He buffer tank press:  "};

    clearScreen();

    printfAt(0,0,"Press any key to stop logging data!");
    printfAt(1,0,"--------------------------------------------------------------------------------");


    do {
        switch(mode){
            case CONT_MON_ALL:
                /* Analog parallel data */
                for(item=0;item<8;item++){
                    aregOut.uchr=item;
                    lastParAdcUint=analogMonitor(PARALLEL_ADC,aregOut.bitf.par_ch);
                    lastParAdcFloat=adcScaling(PARALLEL_ADC,lastParAdcUint);
                    printfAt(2+item,2,"%s %6.5f",parallel[item],lastParAdcFloat);
                    printfAt(2+item,35,"|");
                }

                /* Analog serial data */
                for(item=0;item<3;item++){
                    bregOut.uchr=item;
                    lastSerAdcUint=analogMonitor(SERIAL_ADC,bregOut.bitf.ser_ch);
                    lastSerAdcFloat=adcScaling(SERIAL_ADC,lastSerAdcUint);
                    printfAt(2+item,36,"    %s %6.5f",serial[item],lastSerAdcFloat);
                }

                /* Digital data */
                bregIn.uint=parallelRead(PARALLEL_READ_BREG_IN,BREG_IN_PARALLEL_READ_SIZE);
                displayDigital();

                break;

            case CONT_MON_DIG:
                /* Digital data */
                bregIn.uint=parallelRead(PARALLEL_READ_BREG_IN,BREG_IN_PARALLEL_READ_SIZE);
                displayDigital();
                break;

            case CONT_MON_ANA_PAR:
                /* Analog parallel data */
                for(item=0;item<8;item++){
                    aregOut.uchr=item;
                    lastParAdcUint=analogMonitor(PARALLEL_ADC,aregOut.bitf.par_ch);
                    lastParAdcFloat=adcScaling(PARALLEL_ADC,lastParAdcUint);
                    printfAt(2+item,2,"%s %6.5f",parallel[item],lastParAdcFloat);
                    printfAt(2+item,35,"|");
                }
                break;

            case CONT_MON_ANA_SER:
                /* Analog serial data */
                for(item=0;item<3;item++){
                    bregOut.uchr=item;
                    lastSerAdcUint=analogMonitor(SERIAL_ADC,bregOut.bitf.ser_ch);
                    lastSerAdcFloat=adcScaling(SERIAL_ADC,lastSerAdcUint);
                    printfAt(2+item,36,"    %s %6.5f",serial[item],lastSerAdcFloat);
                }
                break;

            default:
                break;
        }

    } while(!kbhit());

    waitKey(' ');
}



// Perform the control operations
//--------------------------------------------------------------------------
void controlOperations(void){

    int menuChoice;

    do {
        switch(menuChoice=displayMenu(4)){
            case '0':
                cregOut.bitf.n2fill^=0x01;
                parallelWrite(PARALLEL_WRITE_CREG_OUT,cregOut.uchr,CREG_OUT_PARALLEL_WRITE_SIZE);
                break;
            case '1':
                dregOut.bitf.feState^=0x01;
                parallelWrite(PARALLEL_WRITE_DREG_OUT,dregOut.uchr,DREG_OUT_PARALLEL_WRITE_SIZE);
                break;
            default:
                break;
        }
    } while (menuChoice!='q');

}





// Main
//--------------------------------------------------------------------------
void main(int argc, char *argv[]){

    int arg;
    int menuChoice;
    char *str;

    clearScreen();

    // Check if FPGA is up and running
    if(checkFpga()){
        errorState("FPGA not responding!");
        clearScreen();
        printf("\n\nGuru meditation error!\nSit down... meditate...\nAs an alternative: cry for help from a guru!\n");
        exit(1);
    }

    // Check for command line argument
    // Check for "help". If help, print and exit.
    for(arg=1;arg<argc;arg++){
        str=strstr(argv[arg],"help");
        if(str){
            printf("Usage: FETIM [help] [ec=val] [ls=val] [ps=val] [ss=val]\n\n");

            printf("Where the optional arguments are:\n");

            printf("GENERAL:\n");
            printf(" help               print this messages and exits the program\n\n");

            printf("OUTPUT REGISTERS:\n");
            printf(" ec=[extra clocks]  is the extra clock cycles after the data\n");
            printf("                    Default: %d\n",extraClocks);
            printf(" ls=[left shift]    is the amount of left shift to apply to the data\n");
            printf("                    Default: %d\n\n",leftShift);

            printf("PARALLEL ADC:\n");
            printf(" ps=[extra clocks]  is the extra clock cycles to add to the parallel ADC strobe\n");
            printf("                    Default: %d\n\n",extraParStrbClks);

            printf("SERIAL ADC:\n");
            printf(" ss=[extra clocks]  is the extra clock cycles to add to the serial ADC strobe\n");
            printf("                    Default: %d\n\n",extraSerStrbClks);

            exit(0);
        }
    }

    // Check for extra clocks (ec=val)
    for(arg=1;arg<argc;arg++){
        str=strstr(argv[arg],"ec");
        if(str){
            extraClocks=atoi(strchr(str,'=')+1);
        }
    }

    // Check for left shift (ls=val)
    for(arg=1;arg<argc;arg++){
        str=strstr(argv[arg],"ls");
        if(str){
            leftShift=atoi(strchr(str,'=')+1);
        }
    }

    // Check for extra parallel strobe clocks(ps=val)
    for(arg=1;arg<argc;arg++){
        str=strstr(argv[arg],"ps");
        if(str){
            extraParStrbClks=atoi(strchr(str,'=')+1);
        }
    }

    // Check for extra serial strobe clocks(ss=val)
    for(arg=1;arg<argc;arg++){
        str=strstr(argv[arg],"ss");
        if(str){
            extraSerStrbClks=atoi(strchr(str,'=')+1);
        }
    }

    outpw(PORT_SELECT,FETIM_CH); // Select the correct output port

    do {
        // Check the hardware version
        hardwareVersion=parallelRead(PARALLEL_READ_CREG_IN,CREG_IN_PARALLEL_READ_SIZE);

        switch(menuChoice=displayMenu(1)){ // Display main menu and wait for input
            case 'q': // Exit
                break;
            case '1': // Single analog monitor operation
                singleAnalogMonitor();
                break;
            case '2': // Single digital monitor operation
                singleDigitalMonitor();
                break;
            case '3': // Perform continuous monitoring
                continuousMonitoring(CONT_MON_ALL);
                break;
            case '4': // Perform continuous monitoring digital
                continuousMonitoring(CONT_MON_DIG);
                break;
            case '5': // Perform continuous monitoring analog parallel
                continuousMonitoring(CONT_MON_ANA_PAR);
                break;
            case '6': // Perform continuous monitoring analog serial
                continuousMonitoring(CONT_MON_ANA_SER);
                break;
            case '7': // Control operations
                controlOperations();
                break;
            default:
                break;
        }
    } while (menuChoice!='q');

}

