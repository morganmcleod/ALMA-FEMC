/*************************************
 * Monitor and control software for: *
 * IF SWITCH SUBSYSTEM               *
 *                                   *
 * avaccari@nrao.edu                 *
 *************************************

 Date           Version     Notes
 2009-12-14     01.02.00    Fixed to support check of FPGA ready for both
                            version of the FPGA code.
                            Added support to identify different hardware version
                            of the IF switch. Depending on the hardware level,
                            temperatures are computed in different ways.

 2006-05-30     01.01.00    Added temperature logging ability to the software.
                            Added idle loop function which is called every time
                            the software is waiting for input by the user.
                            Added feature to automatically start z-modem upload
                            of last log file.

 2006-05-09     01.00.05    Guru meditation error if FPGA not found.
                            Corrected scaling formula for temperature
                            calculation.

 2006-04-28     01.00.04    Fixed display problem: signed -> unsigned

 2006-04-17     01.00.03    Added features to stop the analog monitor process if
                            the ADC_READY times out and to stop accessing the
                            SSC bus if SSC_READY times out.

 2006-04-14     01.00.02    Added revision level to the menu.

 2006-03-29     01.00.01    Corrected Port and Command numbers

 2006-03-29     01.00.00    First Release


*/
// Version string global
char version[9]="01.02.00";

// Includes
#include <stdio.h>
#include <stdarg.h>
#include <conio.h>
#include <i86.h>
#include <math.h>
#include <errno.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include <string.h>

// ISA bus I/O mapped commands
#define PORT_SELECT     0x300
#define DATA_10         0x302
#define WRITE_LENGTH    0x308
#define READ_LENGTH     0x30A
#define COMMAND         0x30C
#define BUSY_STATUS     0x30C
#define FPGA_CHECK      0x30E

// Command Words
#define PARALLEL_WRITE_AREG         0x00
#define PARALLEL_WRITE_ATT_REG(reg) (0x01+reg)
#define PARALLEL_WRITE_FREG         0x05
#define PARALLEL_WRITE_GREG         0x06
#define ADC_CONVERT_STROBE          0x08
#define ADC_DATA_READ               0x09
#define PARALLEL_READ               0x0A


// Other Interface definitions
#define FPGA_READY                  0x5A5A

// User interface defines
#define FALSE                   0
#define TRUE                    1
#define MS_DELAY                250
#define MS_MESSAGE_DURATION     2000
#define SCREEN_COLUMNS          80
#define SCREEN_ROWS             25
#define SCREEN_SIZE             (SCREEN_COLUMNS*SCREEN_ROWS)
#define LAST_MESSAGE_SIZE       SCREEN_COLUMNS
#define UPPER_WINDOW_START      0
#define UPPER_WINDOW_END        4
#define UPPER_WINDOW_SIZE       (SCREEN_COLUMNS*(UPPER_WINDOW_END-UPPER_WINDOW_START+1))
#define LOWER_WINDOW_START      (UPPER_WINDOW_END+1)
#define MENU_START_LINE         (LOWER_WINDOW_START+1)
#define MENU_BACK_LINE          (MENU_START_LINE+2)
#define MENU_OPTION_LINE        (MENU_BACK_LINE+2)
#define ERROR_REPORT_LINE       24
#define MENU_MAIN_MENU          1
#define MENU_SELECT_BAND        2
#define MENU_SET_ATTEN          3
#define MENU_ENABLE_TEMP_CTRL   4
#define MENU_RUN_SCRIPTS        5
#define MENU_SCRIPT_LOG_TEMP    6

// OS defines
#define FOPEN       0
#define FSEEK_END   1
#define FCLOSE      2

// Serial Mux General
#define SSC_BUSY_MASK   0x0001

// Hardware Specific
#define IF_SWITCH_CH                21
#define PARALLEL_READ_SIZE          4
#define AREG_PARALLEL_WRITE_SIZE    4
#define ATT_REG_PARALLEL_WRITE_SIZE 4
#define FREG_PARALLEL_WRITE_SIZE    4
#define GREG_PARALLEL_WRITE_SIZE    3
#define ADC_BUSY_MASK               0x0008
#define HARDWARE_VERSION_MASK       0x0003
#define ADC_DATA_SIZE               18
#define ADC_STROBE_SIZE             3
#define CHANNELS                    4
#define AREG_RANGE                  13
#define R1                          4020.0
#define VREF                        0.5
#define BETA                        3380.0
#define DISPLAY_RESULTS             1
#define LOG_TEMP_START              0
#define LOG_TEMP_RUN                1
#define LOG_TEMP_STOP               2
#define FILE_NAME_SIZE              26


// Registers
// FREG
typedef struct {
    unsigned char   tc1 :1;
    unsigned char   tc2 :1;
    unsigned char   tc3 :1;
    unsigned char   tc4 :1;
    unsigned char       :4;
} FREG_BITF;

typedef union {
    unsigned char   uchar;
    FREG_BITF       bitf;
} FREG_UNION;


// A structure to hold all the info about the temperature monitor
typedef struct {
    unsigned char   greg[2];
    unsigned int    lastAdcUint[2];
    float           lastAdcFloat[2];
    float           RTermist;
    float           temp;
} TEMP_SWITCH;





// Globals
unsigned char   areg;
unsigned char   band[AREG_RANGE]={1,2,3,4,5,0,0,0,6,7,8,9,10}; // Mapping between areg and selected band
unsigned char   attReg[CHANNELS];
unsigned char   channel;
FREG_UNION      freg;
TEMP_SWITCH     temp[CHANNELS];
unsigned char   lastMessage[LAST_MESSAGE_SIZE];
unsigned long   tempLogPeriod, tempLogInterval, logTimeLeft;
unsigned char   tempLogFileName[FILE_NAME_SIZE];
unsigned char   tempLogDone=TRUE;
int             hardwareVersion;


// ***** START COMMON SOFTWARE *****
// *** START IDLE LOOP HANDLING
// This function is called every time the software is waiting for input
void idleLoop(void){

    static unsigned char    counter=0, count;


    // Check the temperature logging state
    logTemperatures(LOG_TEMP_RUN);

    // Print last state message
    if(*lastMessage){
        printfAt(ERROR_REPORT_LINE,0,"                                         ");
        printfAt(ERROR_REPORT_LINE,0,"%s",lastMessage);
        cursorAt(ERROR_REPORT_LINE,SCREEN_COLUMNS-1);
        count = 1;
    }

    // If it is the case, clear the state message
    if(count){
        if(counter == MS_MESSAGE_DURATION/MS_DELAY){
            printfAt(ERROR_REPORT_LINE,0,"                                         ");
            cursorAt(ERROR_REPORT_LINE,SCREEN_COLUMNS-1);
            errorState(NULL);
            counter = 0;
            count = 0;
        } else {
            counter += count;
        }
    }

    delay(MS_DELAY); // 250 ms sleep time to be nice with the CPU

}
// *** END IDLE LOOP HANDLING





// *** START USER INTERFACE
// Writes error on the screen
void errorState(char *message){
    if(message){
        sprintf(lastMessage,"Error: %s",message);
    } else {
        sprintf(lastMessage,"");
    }
}

// Position the cursor to the desired location (x -> row, y -> column)
void cursorAt(unsigned char x, unsigned char y){

    union REGS r;

    // Check for limits
    if((x>SCREEN_ROWS-1)||(y>SCREEN_COLUMNS-1)){
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

    fflush(stdout); // Make sure everything is printed
}

// Clear screen
void clearScreen(void){

    union REGS r;

    cursorAt(0,0);

    r.h.ah = 10;
    r.h.al = 32;
    r.h.bh = 0;
    r.x.cx = SCREEN_SIZE;

    int86(0x10,&r,&r);
}

// Clear status
void clearStatus(void){

    union REGS r;

    cursorAt(0,0);

    r.h.ah = 10;
    r.h.al = 32;
    r.h.bh = 0;
    r.x.cx = UPPER_WINDOW_SIZE;

    int86(0x10,&r,&r);
}

// Wait for a key to be pressed
int waitKey(char key){

	int choice;

	fflush(stdin); // Make sure the input buffer is empty

    if(key!=0xFF){
        // Idle loop waiting for IO maybe some parallel processing? YEAH RIGHT!
        while(!kbhit()){
            idleLoop();
        }

        choice=getch();

		return choice;
    }

	printfAt(ERROR_REPORT_LINE,54,"Press ENTER to continue...");
    // Idle loop waiting for IO maybe some parallel processing? YEAH RIGHT!
    while(!kbhit()){
        idleLoop();
    }
	getch();

	return 0;
}
// *** END USER INTERFACE ***






// *** START SERIAL MUX INTERFACE ***
// Check that the fpga is up and running (0->OK, 1->BAD)
int checkFpga(void){

    return (((inpw(FPGA_CHECK)!=FPGA_READY)&&(inpw(FPGA_CHECK+2)!=FPGA_READY))?1:0);
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
// *** END SERIAL MUX INTERFACE ***







// *** START SSI COMMON INTERFACE ***
// Send strobe
void sendStrobe(unsigned char strobe){
    checkSscBusy(); // Check if ssc if busy
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
        return; // If busy return 0
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
unsigned int analogMonitor(char command, unsigned char areg, char size){
    parallelWrite(command,areg,size); // Select the what to monitor with AREG
    if(checkAdcBusy()){ // Make sure ADC is ready
        return 0; // If busy return 0
    }
    sendStrobe(ADC_CONVERT_STROBE); // Send conversion strobe command
    if(checkAdcBusy()){ // Make sure ADC is ready
        return 0; // If busy return 0
    }

    return getAdc(ADC_DATA_READ,ADC_DATA_SIZE); // Read data from ADC and return value
}
// *** START SSI COMMON INTERFACE ***







// *** START COMMON SCALING ***
// Scale to the appropriate value
float adcScaling(float scaling, unsigned int data){
    return (scaling*data)/0x10000; // Scale and return the data
}
// *** END COMMON SCALING ***
// ***** END COMMON SOFTWARE *****








// ***** START APPLICATION SPECIFIC SOFTWARE *****
// Display current state
void currentState(void){
    printfAt(0,0,"AREG: 0x%X (Band: %d)",areg,band[areg]);
    printfAt(0,22,"BREG: 0x%X (Att. Sw1: %2d dB)",attReg[0],attReg[0]);
    printfAt(1,22,"CREG: 0x%X (Att. Sw2: %2d dB)",attReg[1],attReg[1]);
    printfAt(0,50,"DREG: 0x%X (Att. Sw3: %2d dB)",attReg[2],attReg[2]);
    printfAt(1,50,"EREG: 0x%X (Att. Sw4: %2d dB)",attReg[3],attReg[3]);
    printfAt(2,0,"FREG: 0x%X (0000%d%d%d%d)",freg.
                                              uchar,freg.
                                                     bitf.
                                                      tc4,freg.
                                                           bitf.
                                                            tc3,freg.
                                                                 bitf.
                                                                  tc2,freg.
                                                                       bitf.
                                                                        tc1);
    printfAt(2,22,"Temp Sw1: %3.2f C",temp[0].temp);
    printfAt(3,22,"Temp Sw2: %3.2f C",temp[1].temp);
    printfAt(2,50,"Temp Sw3: %3.2f C",temp[2].temp);
    printfAt(3,50,"Temp Sw4: %3.2f C",temp[3].temp);
    printfAt(UPPER_WINDOW_END,0,"--------------------------------------------------------------------------------");

}

// Display menu
int displayMenu(int menuNo){

    int menuChoice;

    clearScreen(); // Clear screen

    currentState(); // Display current state

    printfAt(MENU_START_LINE,5,"--- IF SWITCH (Hardw: %d) (Softw: %s) ---", hardwareVersion, version);

    if(menuNo==1){
        printfAt(MENU_BACK_LINE,5,"q - Exit");
    } else {
        printfAt(MENU_BACK_LINE,5,"q - Back");
    }

    switch(menuNo){
        case 0:
            break;
        case MENU_MAIN_MENU: // Main menu
            printfAt(MENU_OPTION_LINE,5,"1 - Select Band");
            printfAt(MENU_OPTION_LINE+1,5,"2 - Set Attenuation");
            printfAt(MENU_OPTION_LINE+2,5,"3 - Enable Temperature Controllers");
            printfAt(MENU_OPTION_LINE+3,5,"4 - Monitor Temperatures");
            printfAt(MENU_OPTION_LINE+4,5,"5 - Start/Stop Scripts");
            break;
        case MENU_SELECT_BAND: // Select Bands menu
            printfAt(MENU_OPTION_LINE,5,"0 - Band 1 %s", (band[areg]==1)?"<-":"");
            printfAt(MENU_OPTION_LINE+1,5,"1 - Band 2 %s", (band[areg]==2)?"<-":"");
            printfAt(MENU_OPTION_LINE+2,5,"2 - Band 3 %s", (band[areg]==3)?"<-":"");
            printfAt(MENU_OPTION_LINE+3,5,"3 - Band 4 %s", (band[areg]==4)?"<-":"");
            printfAt(MENU_OPTION_LINE+4,5,"4 - Band 5 %s", (band[areg]==5)?"<-":"");
            printfAt(MENU_OPTION_LINE,25,"5 - Band 6 %s", (band[areg]==6)?"<-":"");
            printfAt(MENU_OPTION_LINE+1,25,"6 - Band 7 %s", (band[areg]==7)?"<-":"");
            printfAt(MENU_OPTION_LINE+2,25,"7 - Band 8 %s", (band[areg]==8)?"<-":"");
            printfAt(MENU_OPTION_LINE+3,25,"8 - Band 9 %s", (band[areg]==9)?"<-":"");
            printfAt(MENU_OPTION_LINE+4,25,"9 - Band 10 %s", (band[areg]==10)?"<-":"");
            break;
        case MENU_SET_ATTEN: // Set Attenuation menu
            printfAt(MENU_OPTION_LINE,5,"h - IF Switch 1 %s", (channel==0)?"<-":"");
            printfAt(MENU_OPTION_LINE+1,5,"i - IF Switch 2 %s", (channel==1)?"<-":"");
            printfAt(MENU_OPTION_LINE+2,5,"j - IF Switch 3 %s", (channel==2)?"<-":"");
            printfAt(MENU_OPTION_LINE+3,5,"k - IF Switch 4 %s", (channel==3)?"<-":"");
            printfAt(MENU_OPTION_LINE,25,"0 - Attenuation  0dB %s", (attReg[channel]==0)?"<-":"");
            printfAt(MENU_OPTION_LINE+1,25,"1 - Attenuation  1dB %s", (attReg[channel]==1)?"<-":"");
            printfAt(MENU_OPTION_LINE+2,25,"2 - Attenuation  2dB %s", (attReg[channel]==2)?"<-":"");
            printfAt(MENU_OPTION_LINE+3,25,"3 - Attenuation  3dB %s", (attReg[channel]==3)?"<-":"");
            printfAt(MENU_OPTION_LINE+4,25,"4 - Attenuation  4dB %s", (attReg[channel]==4)?"<-":"");
            printfAt(MENU_OPTION_LINE+5,25,"5 - Attenuation  5dB %s", (attReg[channel]==5)?"<-":"");
            printfAt(MENU_OPTION_LINE+6,25,"6 - Attenuation  6dB %s", (attReg[channel]==6)?"<-":"");
            printfAt(MENU_OPTION_LINE+7,25,"7 - Attenuation  7dB %s", (attReg[channel]==7)?"<-":"");
            printfAt(MENU_OPTION_LINE,52,"8 - Attenuation  8dB %s", (attReg[channel]==8)?"<-":"");
            printfAt(MENU_OPTION_LINE+1,52,"9 - Attenuation  9dB %s", (attReg[channel]==9)?"<-":"");
            printfAt(MENU_OPTION_LINE+2,52,"a - Attenuation 10dB %s", (attReg[channel]==10)?"<-":"");
            printfAt(MENU_OPTION_LINE+3,52,"b - Attenuation 11dB %s", (attReg[channel]==11)?"<-":"");
            printfAt(MENU_OPTION_LINE+4,52,"c - Attenuation 12dB %s", (attReg[channel]==12)?"<-":"");
            printfAt(MENU_OPTION_LINE+5,52,"d - Attenuation 13dB %s", (attReg[channel]==13)?"<-":"");
            printfAt(MENU_OPTION_LINE+6,52,"e - Attenuation 14dB %s", (attReg[channel]==14)?"<-":"");
            printfAt(MENU_OPTION_LINE+7,52,"f - Attenuation 15dB %s", (attReg[channel]==15)?"<-":"");
            break;
        case MENU_ENABLE_TEMP_CTRL: // Enable Temperature controller menu
            printfAt(MENU_OPTION_LINE,5,"0 - Temperature Controller IF Switch 1 (%s)", freg.bitf.tc1?"ON":"OFF");
            printfAt(MENU_OPTION_LINE+1,5,"1 - Temperature Controller IF Switch 2 (%s)", freg.bitf.tc2?"ON":"OFF");
            printfAt(MENU_OPTION_LINE+2,5,"2 - Temperature Controller IF Switch 3 (%s)", freg.bitf.tc3?"ON":"OFF");
            printfAt(MENU_OPTION_LINE+3,5,"3 - Temperature Controller IF Switch 4 (%s)", freg.bitf.tc4?"ON":"OFF");
            break;
        case MENU_RUN_SCRIPTS: // Run predefined scripts
            printfAt(MENU_OPTION_LINE,5,"0 - Log temperatures (Log: %s - %lu sec left) (ENTER to update)",tempLogDone?"Idle":"Running",logTimeLeft);
            break;
        case MENU_SCRIPT_LOG_TEMP: // Script "monitor temperature" menu
            printfAt(MENU_OPTION_LINE,5,"0 - Total monitoring period (%lu sec) (set this first)",tempLogPeriod);
            printfAt(MENU_OPTION_LINE+1,5,"1 - Monitoring time interval (%lu sec)",tempLogInterval);
            printfAt(MENU_OPTION_LINE+2,5,"2 - Log file name (%s)",tempLogFileName);
            printfAt(MENU_OPTION_LINE+3,5,"3 - Start monitoring");
            printfAt(MENU_OPTION_LINE+4,5,"4 - Stop monitoring");
            printfAt(MENU_OPTION_LINE+5,5,"5 - Upload log file with zmodem");
            break;
        default:
            break;
    }


    cursorAt(ERROR_REPORT_LINE,SCREEN_COLUMNS-1);

	menuChoice=waitKey(NULL);

	return menuChoice;
}

// Toggle Power
void setAttenuation(void){

    int menuChoice;

    do {
        switch(menuChoice=displayMenu(MENU_SET_ATTEN)){
            case 'h': // Channel 1
            case 'i': // Channel 2
            case 'j': // Channel 3
            case 'k': // Channel 4
                channel = menuChoice-104;
                break;
            case '0': // Attenuation  0dB
            case '1': // Attenuation  1dB
            case '2': // Attenuation  2dB
            case '3': // Attenuation  3dB
            case '4': // Attenuation  4dB
            case '5': // Attenuation  5dB
            case '6': // Attenuation  6dB
            case '7': // Attenuation  7dB
            case '8': // Attenuation  8dB
            case '9': // Attenuation  9dB
                attReg[channel] = menuChoice-48;
                parallelWrite(PARALLEL_WRITE_ATT_REG(channel),attReg[channel],ATT_REG_PARALLEL_WRITE_SIZE);
                break;
            case 'a': // Attenuation 10dB
            case 'b': // Attenuation 11dB
            case 'c': // Attenuation 12dB
            case 'd': // Attenuation 13dB
            case 'e': // Attenuation 14dB
            case 'f': // Attenuation 15dB
                attReg[channel] = menuChoice-87;
                parallelWrite(PARALLEL_WRITE_ATT_REG(channel),attReg[channel],ATT_REG_PARALLEL_WRITE_SIZE);
                break;
            default:
                break;
        }
    } while (menuChoice!='q');
}

// Select band
void selectBand(void){

    int menuChoice;

    do {
        switch(menuChoice=displayMenu(MENU_SELECT_BAND)){
            case '0': // Band 1
            case '1': // Band 2
            case '2': // Band 3
            case '3': // Band 4
            case '4': // Band 5
                areg = menuChoice-48;
                parallelWrite(PARALLEL_WRITE_AREG,areg,AREG_PARALLEL_WRITE_SIZE);
                break;
            case '5': // Band 6
            case '6': // Band 7
            case '7': // Band 8
            case '8': // Band 9
            case '9': // Band 10
                areg = menuChoice-45;
                parallelWrite(PARALLEL_WRITE_AREG,areg,AREG_PARALLEL_WRITE_SIZE);
                break;
            default:
                break;
        }
    } while (menuChoice!='q');
}

// Enable temperature controller
void enableTemperatureController(void){

    int menuChoice;

    do {
        switch(menuChoice=displayMenu(MENU_ENABLE_TEMP_CTRL)){
            case '0': // Temperature controller channel 1
            case '1': // Temperature controller channel 2
            case '2': // Temperature controller channel 3
            case '3': // Temperature controller channel 4
                freg.uchar^=0x01<<(menuChoice-48);
                parallelWrite(PARALLEL_WRITE_FREG,freg.uchar,FREG_PARALLEL_WRITE_SIZE);
                break;
            default:
                break;
        }
    } while (menuChoice!='q');
}

// Perform the required monitor operation
void monitorTemperatures(unsigned char display){

    unsigned char       cnt1, cnt2;
    struct _exception   *err_info;

    // Calculate the temperatures
    switch(hardwareVersion){
        case 0: // The old IF switch requires to read two voltages to compute temperatures
            for(cnt1=0;cnt1<4;cnt1++){
                for(cnt2=0;cnt2<2;cnt2++){
                    temp[cnt1].greg[cnt2]=2*cnt1+cnt2; // Update GREG
                    temp[cnt1].lastAdcUint[cnt2] = analogMonitor(PARALLEL_WRITE_GREG,temp[cnt1].greg[cnt2],GREG_PARALLEL_WRITE_SIZE); // Perform the monitor operation
                    temp[cnt1].lastAdcFloat[cnt2] = adcScaling(5.0,temp[cnt1].lastAdcUint[cnt2]); // Scale to Voltage
                }
                temp[cnt1].RTermist = R1*(temp[cnt1].lastAdcFloat[0]+temp[cnt1].lastAdcFloat[1]-2*VREF)/(VREF-temp[cnt1].lastAdcFloat[0]);
                temp[cnt1].temp = BETA*298.15/(298.15*log(temp[cnt1].RTermist/10000.0)+BETA)-273.15;
            }
            break;
        case 1: // The new one requires only one readout
            cnt2=0;
            for(cnt1=0;cnt1<4;cnt1++){
                    temp[cnt1].greg[cnt2]=cnt1; // Update GREG
                    temp[cnt1].lastAdcUint[cnt2] = analogMonitor(PARALLEL_WRITE_GREG,temp[cnt1].greg[cnt2],GREG_PARALLEL_WRITE_SIZE); // Perform the monitor operation
                    temp[cnt1].lastAdcFloat[cnt2] = adcScaling(5.0,temp[cnt1].lastAdcUint[cnt2]); // Scale to Voltage
                temp[cnt1].RTermist = 10000.0*(temp[cnt1].lastAdcFloat[0]/2.5);
                temp[cnt1].temp = (BETA*298.15)/(298.15*log(temp[cnt1].RTermist/10000.0)+BETA)-273.15;
            }
            break;
        default:
            break;
    }

    // Check if log reported an error
    if(errno==EDOM){
        errorState("Argument of log() out of domain!");
    }

    if(display){
        clearScreen(); // Clear screen
        for(cnt1=0;cnt1<4;cnt1++){
            printfAt(MENU_START_LINE+4*cnt1,30,"V: %6.5f V <- ADC: 0x%04X (%5u) <- GREG: 0x%X",temp[cnt1].lastAdcFloat[0],temp[cnt1].lastAdcUint[0],temp[cnt1].lastAdcUint[0],temp[cnt1].greg[0]);
            printfAt(MENU_START_LINE+4*cnt1+1,0,"Temp Sw%d: %3.2f C <- RTherm1: %3.2f ohm",cnt1+1,temp[cnt1].temp,temp[cnt1].RTermist);
            printfAt(MENU_START_LINE+4*cnt1+2,30,"V: %6.5f V <- ADC: 0x%04X (%5u) <- GREG: 0x%X",temp[cnt1].lastAdcFloat[1],temp[cnt1].lastAdcUint[1],temp[cnt1].lastAdcUint[1],temp[cnt1].greg[1]);
        }
        currentState(); // Display current state
        waitKey(0xFF);
    } else {
        clearStatus(); // Clear the current state window
        currentState(); // Display current state
    }


}




// Script: log temperatures
void logTemperatures(unsigned char operation){

    static clock_t  startInterval, startTime, currentTime, sampleTime;
    static FILE     *logFile;
    float           logTime;

    switch(operation){
        case LOG_TEMP_START:
            // Check log state
            if(!tempLogDone){
                errorState("Log already running");
                break;
            }
            // Open the file for update (do you want to put a check on the file existance?)
            if((logFile=fopen(tempLogFileName,"w"))==NULL){
                errorState("Cannot open/create file");
                break;
            }

            // Format the time field
            time(&sampleTime);

            // Write the start time stamp to the file
            fprintf(logFile,"Log started on: %s",asctime(localtime(&sampleTime)));
            fprintf(logFile,"Time since start (sec),Temp1,Temp2,Temp3,Temp4\n");

            startInterval = startTime = clock();
            tempLogDone = FALSE;
            break;
        case LOG_TEMP_RUN:
            // Check log state
            if(tempLogDone){
                return;
            }

            currentTime = clock();
            logTimeLeft = tempLogPeriod-((currentTime-startTime)/CLOCKS_PER_SEC);

            // Check if it's time to log
            if((currentTime-startInterval)/CLOCKS_PER_SEC==tempLogInterval){
                // Get Temperatures without displaying the results
                monitorTemperatures(FALSE);
                logTime=((float)currentTime-(float)startTime)/CLOCKS_PER_SEC;
                // Write CSV formatted data to the file and flush the file
                fprintf(logFile,"%3.2f,%3.2f,%3.2f,%3.2f,%3.2f\n",logTime,temp[0].temp,temp[1].temp,temp[2].temp,temp[3].temp);
                sprintf(lastMessage,"Temperature data logged");
                startInterval = currentTime; // Start counting for next interval
            }


            // Check if the logging is done
            if((currentTime-startTime)/CLOCKS_PER_SEC==tempLogPeriod){
                // Close the file
                if(fflush(logFile)){
                    errorState("Problem flushing the file");
                    // If flush fails report error and try to close anyway
                }
                if(fclose(logFile)){
                    errorState("Problem closing the file");
                }
                // Notify user
                sprintf(lastMessage,"Temperature logging complete");
                tempLogDone = TRUE;
                logTimeLeft = 0;
            }

            break;
        case LOG_TEMP_STOP:
            // Check log state
            if(tempLogDone){
                errorState("Log is not running");
                break;
            }

            // Close the file
            if(fflush(logFile)){
                errorState("Problem flushing the file");
                // If flush fails report error and try to close anyway
            }
            if(fclose(logFile)){
                errorState("Problem closing the file");
            }
            // Notify user
            sprintf(lastMessage,"Temperature logging complete");
            tempLogDone = TRUE;
            logTimeLeft = 0;
            break;
        default:
            break;
    }
}




// Script: log temperatures
void logTemperaturesSetup(void){

    int             menuChoice;
    unsigned int    days,hours,minutes,seconds;
    unsigned char   fileName[9]="\0\0\0\0\0\0\0\0\0";
    unsigned char   command[80];
    unsigned char   cnt,badFileName;

    do {
        switch(menuChoice=displayMenu(MENU_SCRIPT_LOG_TEMP)){
            case '0': // Temperature logging period
                clearScreen();
                printfAt(MENU_OPTION_LINE,5,"Enter the temperature logging period in the following format:");
                printfAt(MENU_OPTION_LINE+1,5,"days hours minutes seconds");
                do{
                    printfAt(MENU_OPTION_LINE+2,5,"                                                     ");
                    printfAt(MENU_OPTION_LINE+2,5,"");
                } while (scanf("%u %u %u %u",&days,&hours,&minutes,&seconds)==EOF);
                tempLogPeriod = 86400*days+3600*hours+60*minutes+seconds;
                if(tempLogInterval > tempLogPeriod){
                    sprintf(lastMessage,"Log Interval > Log Period! Only 1 sample!");
                    tempLogInterval = tempLogPeriod;
                }
                break;
            case '1': // Temperature logging time interval
                clearScreen();
                printfAt(MENU_OPTION_LINE,5,"Enter the temperature logging interval in the following format:");
                printfAt(MENU_OPTION_LINE+1,5,"days hours minutes seconds");
                do{
                    printfAt(MENU_OPTION_LINE+2,5,"                                                     ");
                    printfAt(MENU_OPTION_LINE+2,5,"");
                } while (scanf("%u %u %u %u",&days,&hours,&minutes,&seconds)==EOF);
                tempLogInterval = 86400*days+3600*hours+60*minutes+seconds;
                if(tempLogInterval > tempLogPeriod){
                    sprintf(lastMessage,"Log Interval > Log Period! Only 1 sample!");
                    tempLogInterval = tempLogPeriod;
                }
                break;
            case '2': // File name
                clearScreen();
                printfAt(MENU_OPTION_LINE,5,"The log file will be stored under the c:\alma\log directory.");
                printfAt(MENU_OPTION_LINE+1,5,"If the file already exists, it will be ovewritten.");
                printfAt(MENU_OPTION_LINE+2,5,"Enter the name of the log file 8 char max:");
                do {
                    do {
                        printfAt(MENU_OPTION_LINE+3,5,"                                                     ");
                        printfAt(MENU_OPTION_LINE+3,5,"");
                    } while (scanf("%8s",fileName)==EOF);
                    badFileName = 0;
                    for(cnt=0;cnt<strlen(fileName);cnt++){
                        if(!isalnum(fileName[cnt])){
                            badFileName = 1;
                        }
                    }
                } while (badFileName);
                sprintf(tempLogFileName,"c:\\alma\\log\\%s.csv",fileName);
                printfAt(MENU_OPTION_LINE+4,5,"Final File name: %s",tempLogFileName);
                waitKey(0xFF);
                break;
            case '3': // Start monitoring script
                logTemperatures(LOG_TEMP_START);
                break;
            case '4': // Stop monitoring script
                logTemperatures(LOG_TEMP_STOP);
                break;
            case '5': // Upload file with z-modem
                clearScreen();
                if(!tempLogDone){
                    errorState("Log is still running!");
                    break;
                }
                command[0]='\0';
                system(strcat(strcat(command,"rsz /p1 /i4 /b115200 /s "),tempLogFileName));
                break;
            default:
                break;
        }
    } while (menuChoice!='q');
}



// Run scripts
void runScripts(void){

    int menuChoice;

    do {
        switch(menuChoice=displayMenu(MENU_RUN_SCRIPTS)){
            case '0': // Monitor temperatures at defined intervals
                logTemperaturesSetup();
                break;
            default:
                break;
        }
    } while (menuChoice!='q');
}


// Main
void main(void){

    int menuChoice;

    freopen(NULL,"r",stderr); // Redirect stderr

    clearScreen();

    // Check if FPGA is up and running
    if(checkFpga()){
        errorState("FPGA not responding!");
        clearScreen();
        printf("\n\nGuru meditation error!\nSit down... meditate...\nAs an alternative: cry for help from a guru!\n");
        exit(1);
    }

    outpw(PORT_SELECT,IF_SWITCH_CH); // Select the correct output port

    // Set speed to 10 Mhz (This shouldn't be needed with the test sets)
    outpw(0x308,1);
    outpw(0x30C,0x07);
    outpw(0x30C,0x17);


    do {
        // Check the hardware version
        hardwareVersion=parallelRead(PARALLEL_READ,PARALLEL_READ_SIZE)&HARDWARE_VERSION_MASK;

        switch(menuChoice=displayMenu(MENU_MAIN_MENU)){ // Display main menu and wait for input
            case 'q': // Exit
                break;
            case '1': // Select band
                selectBand();
                break;
            case '2': // Set attenuation
                setAttenuation();
                break;
            case '3': // Enable temperature controller
                enableTemperatureController();
                break;
            case '4': // Monitor the temperatures
                monitorTemperatures(DISPLAY_RESULTS);
                break;
            case '5': // Run scripts
                runScripts();
                break;
            default:
                break;
        }
    } while (menuChoice!='q');

}
// ***** END APPLICATION SPECIFIC SOFTWARE *****

