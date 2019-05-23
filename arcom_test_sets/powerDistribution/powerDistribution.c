/*************************************
 * Monitor and control software for: *
 * POWER DISTRIBUTION SUBSYSTEM      *
 *                                   *
 * avaccari@nrao.edu                 *
 *************************************

 Date           Version     Notes
 2009-08-13     04.01.02    Modified selection interface to use band number to
                            select a band instead of band number -1

 2009-06-22     04.01.01    Fixed to support check of FPGA ready for both
                            versions of the FPGA code.

 2007-04-16     04.01.00    Added feature to monitor the currents and the
                            voltages continuously to allow fine tuning.

 2006-09-26     04.00.00    Modified scaling and mapping of monitor point to
                            reflect hardware change
                            Guru meditation error if FPGA not found

 2006-04-28     03.00.02    Fixed problem with unsigned variable display.

 2006-04-14     03.00.01    Added features to stop the analog monitor process if
                            the ADC_READY times out and to stop accessing the
                            SSC bus if SSC_READY times out.

 2006-04-14     03.00.00    Switched the nibbles in BREG. Fixed error in the
                            channel select coding. This version is not backward
                            compatible.

 2006-04-14     02.00.02    Added revision level to the menu.

 2006-04-14     02.00.01    Removed the unneeded command to switch to 10 MHz.

 2006-04-13     02.00.00    Switched AREG with BREG. Now AREG is associated with
                            command 0x08 and BREG with 0x09. This version is not
                            backward compatible.

 2006-03-15     01.00.00    First Release


*/
// Version string global
char version[16]="Rev. 04.01.02";



// Includes
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
#define ADC_CONVERT_STROBE  0x03
#define ADC_DATA_READ       0x06
#define PARALLEL_WRITE_AREG 0x08
#define PARALLEL_WRITE_BREG 0x09
#define PARALLEL_READ       0x0A


// Other Interface definitions
#define POWER_DISTRIBUTION_CH       20
#define PARALLEL_READ_SIZE          4
#define AREG_PARALLEL_WRITE_SIZE    10
#define BREG_PARALLEL_WRITE_SIZE    8
#define ADC_BUSY_MASK               0x0008
#define SSC_BUSY_MASK               0x0001
#define ADC_DATA_SIZE               18
#define ADC_STROBE_SIZE             3
#define FPGA_READY                  0x5A5A


// Registers
// AREG
typedef struct {
    unsigned int    b1  :1;
    unsigned int    b2  :1;
    unsigned int    b3  :1;
    unsigned int    b4  :1;
    unsigned int    b5  :1;
    unsigned int    b6  :1;
    unsigned int    b7  :1;
    unsigned int    b8  :1;
    unsigned int    b9  :1;
    unsigned int    b10 :1;
    unsigned int        :6;
} AREG_BITF;

typedef union {
    unsigned int    uint;
    AREG_BITF       bitf;
} AREG_UNION;




// BREG
typedef struct {
    unsigned char   channel :4;
    unsigned char   band    :4;
} BREG_NIBL;

typedef union {
    unsigned char   uchar;
    BREG_NIBL       nibl;
} BREG_UNION;


// Globals
AREG_UNION      areg;
BREG_UNION      breg;
unsigned int    lastAdcUint;
float           lastAdcFloat;
char            channel[6], unit[3];



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
    outpw(WRITE_LENGTH,ADC_STROBE_SIZE); // Set the lenght of the write message
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



// Check for the busy status of the adc (1 -> busy; 0 -> idle)
int checkAdcBusy(void){
    unsigned char   cnt = 0xFF;
    int             tmp;

    do{
        tmp = !(parallelRead(PARALLEL_READ,PARALLEL_READ_SIZE) & ADC_BUSY_MASK); // Read adc busy status
        cnt--; // Counter to avoid hung-ups
    } while (tmp&&cnt);

    if(!cnt){
        errorState("ADC busy after 255 tries");
    }

    return tmp;
}




// Perform a parallel write operation
void parallelWrite(char command, int data, char size){
    if(checkSscBusy()){ // Wait if ssc is busy
        return; // If busy return
    }
    outpw(WRITE_LENGTH,size+1); // Set the write size + strobe
    outpw(DATA_10,data<<1); // Store the data to be sent
    outpw(COMMAND,command); // Perform the parallel write
}




// Get the ADC data
unsigned int getAdc(char command, char size){
    if(checkSscBusy()){ // Wait if ssc is busy
        return 0; // If busy return 0
    }
    outpw(READ_LENGTH,size); // Set the read back size
    outpw(COMMAND,ADC_DATA_READ); // Send the ADC read back command
    if(checkSscBusy()){ // Wait if ssc is busy
        return 0; // If busy return 0
    }

    return inpw(DATA_10); // Return the least significant 16 bits
}





// Perform an analog monitor and return the 16 bit data
unsigned int analogMonitor(unsigned char breg){
    parallelWrite(PARALLEL_WRITE_BREG,breg,BREG_PARALLEL_WRITE_SIZE); // Select the what to monitor with AREG
    if(checkAdcBusy()){ // Make sure ADC is ready
        return 0; // If busy return 0
    }
    sendStrobe(ADC_CONVERT_STROBE); // Send conversion strobe command
    if(checkAdcBusy()){ // Wait for conversion to be done
        return 0; // If busy return 0
    }

    return getAdc(ADC_DATA_READ,ADC_DATA_SIZE); // Read data from ADC and return value
}




// Scale to the appropriate value
float adcScaling(float scaling, unsigned int data){
    return (scaling*data)/0x10000; // Scale and return the data
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





// Select Channel and Units
void selectChannelUnit(void){

    switch(breg.nibl.channel){
        case 0:
            sprintf(channel," +6 I");
            sprintf(unit,"A");
            break;
        case 1:
            sprintf(channel," -6 I");
            sprintf(unit,"A");
            break;
        case 2:
            sprintf(channel,"+15 I");
            sprintf(unit,"A");
            break;
        case 3:
            sprintf(channel,"-15 I");
            sprintf(unit,"A");
            break;
        case 4:
            sprintf(channel,"+24 I");
            sprintf(unit,"A");
            break;
        case 5:
            sprintf(channel," +8 I");
            sprintf(unit,"A");
            break;
        case 8:
            sprintf(channel," +6 V");
            sprintf(unit,"V");
            break;
        case 9:
            sprintf(channel," -6 V");
            sprintf(unit,"V");
            break;
        case 10:
            sprintf(channel,"+15 V");
            sprintf(unit,"V");
            break;
        case 11:
            sprintf(channel,"-15 V");
            sprintf(unit,"V");
            break;
        case 12:
            sprintf(channel,"+24 V");
            sprintf(unit,"V");
            break;
        case 13:
            sprintf(channel," +8 V");
            sprintf(unit,"V");
            break;
        default:
            sprintf(channel,"Err");
            sprintf(unit,"  ");
            break;
    }
}

// Display current state
void currentState(void){

    selectChannelUnit();

    printfAt(0,0,"AREG: 0x%X (000000%d%d%d%d%d%d%d%d%d%d)",
     areg.
      uint,areg.
            bitf.
             b10,areg.
                  bitf.
                   b9,areg.
                       bitf.
                        b8,areg.
                            bitf.
                             b7,areg.
                                 bitf.
                                  b6,areg.
                                      bitf.
                                       b5,areg.
                                           bitf.
                                            b4,areg.
                                                bitf.
                                                 b3,areg.
                                                     bitf.
                                                      b2,areg.
                                                          bitf.
                                                           b1);
    printfAt(1,0,"BREG: 0x%X (Band: %d Channel: %s)",
     breg.
      uchar,breg.
             nibl.
              band+1,
               channel);

    printfAt(2,0,"ADC:  0x%X (%u) (%6.5f %s)",lastAdcUint,lastAdcUint,lastAdcFloat,unit);

    printfAt(3,0,"--------------------------------------------------------------------------------");

}










// Display menu
int displayMenu(int menuNo){

    int menuChoice;

    clearScreen(); // Clear screen

    currentState(); // Display current state

    errorState(NULL); // Print Error state if any

    printfAt(5,5,"--- POWER DISTRIBUTION (%s) ---",version);

    if(menuNo==1){
        printfAt(7,5,"q - Exit");
    } else {
        printfAt(7,5,"q - Back");
    }

    switch(menuNo){
        case 0:
            break;
        case 1: // Main menu
            printfAt(9,5,"1 - Switch bands ON/OFF");
            printfAt(10,5,"2 - Select band to monitor");
            printfAt(11,5,"3 - Select channel to monitor");
            printfAt(13,5,"4 - Perform monitor operation");
            printfAt(14,5,"5 - Perform continuous monitoring");
            break;
        case 2: // Bands On/Off menu
            printfAt(9,5,"1 - Band 1 (%s)", areg.bitf.b1?"ON":"OFF");
            printfAt(10,5,"2 - Band 2 (%s)", areg.bitf.b2?"ON":"OFF");
            printfAt(11,5,"3 - Band 3 (%s)", areg.bitf.b3?"ON":"OFF");
            printfAt(12,5,"4 - Band 4 (%s)", areg.bitf.b4?"ON":"OFF");
            printfAt(13,5,"5 - Band 5 (%s)", areg.bitf.b5?"ON":"OFF");
            printfAt(9,25,"6 - Band 6 (%s)", areg.bitf.b6?"ON":"OFF");
            printfAt(10,25,"7 - Band 7 (%s)", areg.bitf.b7?"ON":"OFF");
            printfAt(11,25,"8 - Band 8 (%s)", areg.bitf.b8?"ON":"OFF");
            printfAt(12,25,"9 - Band 9 (%s)", areg.bitf.b9?"ON":"OFF");
            printfAt(13,25,"0 - Band 10 (%s)", areg.bitf.b10?"ON":"OFF");
            break;
        case 3: // Band to monitor menu
            printfAt(9,5,"1 - Band 1 %s", (breg.nibl.band==0)?"<-":"");
            printfAt(10,5,"2 - Band 2 %s", (breg.nibl.band==1)?"<-":"");
            printfAt(11,5,"3 - Band 3 %s", (breg.nibl.band==2)?"<-":"");
            printfAt(12,5,"4 - Band 4 %s", (breg.nibl.band==3)?"<-":"");
            printfAt(13,5,"5 - Band 5 %s", (breg.nibl.band==4)?"<-":"");
            printfAt(9,25,"6 - Band 6 %s", (breg.nibl.band==5)?"<-":"");
            printfAt(10,25,"7 - Band 7 %s", (breg.nibl.band==6)?"<-":"");
            printfAt(11,25,"8 - Band 8 %s", (breg.nibl.band==7)?"<-":"");
            printfAt(12,25,"9 - Band 9 %s", (breg.nibl.band==8)?"<-":"");
            printfAt(13,25,"0 - Band 10 %s", (breg.nibl.band==9)?"<-":"");
            break;
        case 4: // Channel to monitor menu
            printfAt(9,5,"0 - +6 I %s", (breg.nibl.channel==0)?"<-":"");
            printfAt(10,5,"1 - -6 I %s", (breg.nibl.channel==1)?"<-":"");
            printfAt(11,5,"2 - +15 I %s", (breg.nibl.channel==2)?"<-":"");
            printfAt(12,5,"3 - -15 I %s", (breg.nibl.channel==3)?"<-":"");
            printfAt(13,5,"4 - +24 I %s", (breg.nibl.channel==4)?"<-":"");
            printfAt(14,5,"5 - +8 I %s", (breg.nibl.channel==5)?"<-":"");
            printfAt(9,25,"6 - +6 V %s", (breg.nibl.channel==8)?"<-":"");
            printfAt(10,25,"7 - -6 V %s", (breg.nibl.channel==9)?"<-":"");
            printfAt(11,25,"8 - +15 V %s", (breg.nibl.channel==10)?"<-":"");
            printfAt(12,25,"9 - -15 V %s", (breg.nibl.channel==11)?"<-":"");
            printfAt(13,25,"a - +24 V %s", (breg.nibl.channel==12)?"<-":"");
            printfAt(14,25,"b - +8 V %s", (breg.nibl.channel==13)?"<-":"");
            break;


        default:
            break;
    }

    cursorAt(24,79);

	menuChoice=waitKey(0);

	return menuChoice;
}



// Toggle Power
void togglePower(void){

    int menuChoice;

    do {
        switch(menuChoice=displayMenu(2)){
            case '0':
                areg.uint^=0x0001<<(menuChoice-39);
                break;
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                areg.uint^=0x0001<<(menuChoice-49);
                break;
            default:
                break;
        }

        parallelWrite(PARALLEL_WRITE_AREG, areg.uint, AREG_PARALLEL_WRITE_SIZE);

    } while (menuChoice!='q');


}




// Select band to monitor
void selectBand(void){

    int menuChoice;

    do {
        switch(menuChoice=displayMenu(3)){
            case '0':
                breg.nibl.band=menuChoice-39;
                break;
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                breg.nibl.band=menuChoice-49;
                break;
            default:
                break;
        }
    } while (menuChoice!='q');

}




// Select channel to monitor
void selectChannel(void){

    int menuChoice;

    do {
        switch(menuChoice=displayMenu(4)){
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
                breg.nibl.channel=menuChoice-48;
                break;
            case '6':
            case '7':
            case '8':
            case '9':
                breg.nibl.channel=menuChoice-46;
                break;
            case 'a':
            case 'b':
                breg.nibl.channel=menuChoice-85;
                break;

            default:
                break;
        }
    } while (menuChoice!='q');
}




// Check that the fpga is up and running (0->OK, 1->BAD)
int checkFpga(void){

    return (((inpw(FPGA_CHECK)!=FPGA_READY)&&(inpw(FPGA_CHECK+2)!=FPGA_READY))?1:0);
}




// Perform the required monitor operation
void performMonitor(void){

    float scaling;

    lastAdcUint = analogMonitor(breg.uchar);

    switch(breg.nibl.channel){
        case 0: // +6 I
            scaling = 10.0;
            break;
        case 1: // -6 I
            scaling = 1.0;
            break;
        case 2: // +15 I
            scaling = 4.0;
            break;
        case 3: // -15 I
            scaling = 2.0;
            break;
        case 4: // +24 I
            scaling = 2.0;
            break;
        case 5: // +8 I
            scaling = 2.0;
            break;
        case 8: // +6 V
            scaling = 8.0;
            break;
        case 9: // -6 V
            scaling = -8.0;
            break;
        case 10: // +15 V
            scaling = 20.0;
            break;
        case 11: // -15 V
            scaling = -20.0;
            break;
        case 12: // +24 V
            scaling = 30.0;
            break;
        case 13: // +8 V
            scaling = 10.0;
            break;
        default:
            break;
    }

    lastAdcFloat = adcScaling(scaling, lastAdcUint);
}



// Perform continuous monitoring operation
void performContinuousMonitoring(void){

    unsigned char current;
    unsigned char voltage;

    clearScreen();

    printfAt(0,0,"Press any key to stop logging data!");

    // Cycle through channels until stopped
    do{
        for(current=0;current<6;current++){
            breg.nibl.channel=current;
            performMonitor();
            selectChannelUnit();
            printfAt(5+current,0,"Channel: %s ADC:  0x%X (%u) (%6.5f %s)",channel,lastAdcUint,lastAdcUint,lastAdcFloat,unit);
        }
        for(voltage=8;voltage<14;voltage++){
            breg.nibl.channel=voltage;
            performMonitor();
            selectChannelUnit();
            printfAt(5+voltage,0,"Channel: %s ADC:  0x%X (%u) (%6.5f %s)",channel,lastAdcUint,lastAdcUint,lastAdcFloat,unit);
        }
    } while(!kbhit());

    waitKey(' ');
}

// Main
void main(void){

    int menuChoice;

    clearScreen();

    // Check if FPGA is up and running
    if(checkFpga()){
        errorState("FPGA not responding!");
        clearScreen();
        printf("\n\nGuru meditation error!\nSit down... meditate...\nAs an alternative: cry for help from a guru!\n");
        exit(1);
    }

    outpw(PORT_SELECT,POWER_DISTRIBUTION_CH); // Select the correct output port

    do {
        switch(menuChoice=displayMenu(1)){ // Display main menu and wait for input
            case 'q': // Exit
                break;
            case '1': // Switch power to bands on/off
                togglePower();
                break;
            case '2': // Select band to monitor
                selectBand();
                break;
            case '3': // Select channel to monitor
                selectChannel();
                break;
            case '4': // Perform the monitor operation
                performMonitor();
                break;
            case '5': // Perform continuous monitoring
                performContinuousMonitoring();
            default:
                break;
        }
    } while (menuChoice!='q');

}

