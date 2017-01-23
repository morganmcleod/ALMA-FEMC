/*! \file   lo.c
    \brief  LO functions

    <b> File informations: </b><br>
    Created: 2004/08/24 16:24:39 by avaccari

    <b> CVS informations: </b><br>
    \$Id: lo.c,v 1.21 2010/11/02 14:36:29 avaccari Exp $

    This files contains all the functions necessary to handle LO events. */

/* Includes */
#include <stdlib.h>     /* malloc */
#include <stdio.h>      /* printf & sscanf */
#include <string.h>     /* memset & strtok */

#include "error.h"
#include "frontend.h"
#include "debug.h"
#include "database.h"
#include "serialInterface.h"
#include "loSerialInterface.h"
#include "iniWrapper.h"

/* Globals */
/* Externs */
unsigned char   currentLoModule=0;
/* Statics */
static HANDLER  loModulesHandler[LO_MODULES_NUMBER]={ytoHandler,
                                                     photomixerHandler,
                                                     pllHandler,
                                                     amcHandler,
                                                     paHandler};

/* LO init */
/*! This function performs the operations necessary to initialize a LO at
    runtime. These are executed everytime a cartridge is powered up.
    \return
        - \ref NO_ERROR -> if no error occurred
        - \ref ERROR    -> if something wrong happened */
int loInit(void){
    int ret;

    #ifdef DEBUG_INIT
        printf(" - Initializing LO...\n");
    #endif // DEBUG_INIT

    /* Set the 10MHz */
    #ifdef DEBUG_INIT
        printf("   - 10MHz...\n");
    #endif // DEBUG_INIT

    if(serialAccess(LO_10MHZ_MODE,
                    NULL,
                    LO_10MHZ_MODE_SIZE,
                    LO_10MHZ_MODE_SHIFT_SIZE,
                    LO_10MHZ_MODE_SHIFT_DIR,
                    SERIAL_WRITE)==ERROR){
        return ERROR;
    }
    frontend.
     cartridge[currentModule].
      lo.
       ssi10MHzEnable=ENABLE;

    #ifdef DEBUG_INIT
        printf("     done!\n"); // 10MHz
    #endif // DEBUG_INIT

    ret = loZeroPaDrainVoltage();
    if (ret!=NO_ERROR)
        return ret;

    ret = loZeroPAGateVoltage();
    if (ret!=NO_ERROR)
        return ret;

    ret = loZeroYtoCoarseTuning();
    if (ret!=NO_ERROR)
        return ret;

    /* Set correct loop bandwidth. If it is not defined return the undefined
       warning. */
    #ifdef DEBUG_INIT
       printf("   - PLL Loop Bandwidth...\n");
    #endif // DEBUG_INIT
    if(frontend.
        cartridge[currentModule].
         lo.
          pll.
           loopBandwidthSelect[DEFAULT_VALUE]==PLL_LOOP_BANDWIDTH_UNDEFINED){
       storeError(ERR_LO,
                  0x03); // Error 0x03 -> Warning: The addressed hardware is not properly defined yet
       #ifdef DEBUG_INIT
           printf("     done!\n"); // PLL loop bandwidth
           printf("   done!\n"); // LO
       #endif // DEBUG_INIT
       return NO_ERROR;
    }

    /* If the value is defined then set it to the default value. */
    setLoopBandwidthSelect(frontend.
                            cartridge[currentModule].
                             lo.
                              pll.
                               loopBandwidthSelect[DEFAULT_VALUE]);

    #ifdef DEBUG_INIT
        printf("     done!\n"); // PLL loop bandwidth
        printf("   done!\n"); // LO
    #endif // DEBUG_INIT

    return NO_ERROR;
}

/*! Private helper to set the LO PA drain voltages to zero.
    \return
        - \ref NO_ERROR -> if no error occurred
        - \ref ERROR    -> if something wrong happened */
int loZeroPaDrainVoltage(void) {
    unsigned char *lastCommandData;

    /* Set the PA's drain voltage to 0. The mapping from channel to actual
       polarization is not relevant since we are zeroing all of them. */
    #ifdef DEBUG_INIT
        printf("   - Setting PAs drain voltage to 0\n");
    #endif // DEBUG_INIT

    CONV_FLOAT=0.0;
    currentPaChannelModule=PA_CHANNEL_DRAIN_VOLTAGE;

    /* PA Channel A */
    #ifdef DEBUG_INIT
        printf("     - Channel A\n");
    #endif // DEBUG_INIT
    currentPaModule=PA_CHANNEL_A;
    /* Set Channel. If error, return error and abort initialization */
    if(setPaChannel()==ERROR){
        return ERROR;
    }

    // make a pointer to the data word in the last commanded PA drain voltage:
    lastCommandData = &(frontend.
                         cartridge[currentModule].
                          lo.
                           pa.
                            paChannel[currentPaModule].
                             lastDrainVoltage.
                              data);

    // save the zero we just sent as the last commanded value:
    changeEndian(lastCommandData, CONV_CHR_ADD);

    #ifdef DEBUG_INIT
        printf("       done!\n"); // Channel A
    #endif // DEBUG_INIT

    /* PA Channel B */
    #ifdef DEBUG_INIT
        printf("     - Channel B\n");
    #endif // DEBUG_INIT
    currentPaModule=PA_CHANNEL_B;
    /* Set Channel. If error, return error and abort initialization */
    if(setPaChannel()==ERROR){
        return ERROR;
    }

    // make a pointer to the data word in the last commanded PA drain voltage:
    lastCommandData = &(frontend.
                         cartridge[currentModule].
                          lo.
                           pa.
                            paChannel[currentPaModule].
                             lastDrainVoltage.
                              data);

    // save the zero we just sent as the last commanded value:
    changeEndian(lastCommandData, CONV_CHR_ADD);

    #ifdef DEBUG_INIT
        printf("       done!\n"); // Channel B
        printf("     done!\n"); // Set PA drain voltage to 0
    #endif // DEBUG_INIT
    return NO_ERROR;
}

/*! Private helper to set the LO PA gate voltages to zero.
    \return
        - \ref NO_ERROR -> if no error occurred
        - \ref ERROR    -> if something wrong happened */
int loZeroPAGateVoltage(void) {
    /* Set the PA's gate voltage to 0 */
    #ifdef DEBUG_INIT
        printf("   - Setting PAs gate voltage to 0\n");
    #endif // DEBUG_INIT
    CONV_FLOAT=0.0;
    currentPaChannelModule=PA_CHANNEL_GATE_VOLTAGE;

    /* PA Channel A */
    #ifdef DEBUG_INIT
        printf("     - Channel A\n");
    #endif // DEBUG_INIT
    currentPaModule=PA_CHANNEL_A;
    /* Set Channel. If error, return error and abort initialization */
    if(setPaChannel()==ERROR){
        return ERROR;
    }
    #ifdef DEBUG_INIT
        printf("       done!\n"); // Channel A
    #endif // DEBUG_INIT

    /* PA Channel B */
    #ifdef DEBUG_INIT
        printf("     - Channel B\n");
    #endif // DEBUG_INIT
    currentPaModule=PA_CHANNEL_B;
    /* Set Channel. If error, return error and abort initialization */
    if(setPaChannel()==ERROR){
        return ERROR;
    }
    #ifdef DEBUG_INIT
        printf("       done!\n"); // Channel B
        printf("     done!\n"); // Set PA get voltage to 0
    #endif // DEBUG_INIT
    return NO_ERROR;
}

/*! Private helper to set the YTO course tuning word to zero
    \return
        - \ref NO_ERROR -> if no error occurred
        - \ref ERROR    -> if something wrong happened */
int loZeroYtoCoarseTuning(void) {
    #ifdef DEBUG_INIT
        printf("   - Setting YTO coarse tuning to 0\n");
    #endif // DEBUG_INIT
    CONV_FLOAT=0.0;
    currentPaChannelModule=PA_CHANNEL_GATE_VOLTAGE;
    
    /* Set coarse tuning. If error, return error and abort initialization */
    CONV_UINT(0) = 0;
    if(setYtoCoarseTune()==ERROR){
        return ERROR;
    }
    #ifdef DEBUG_INIT
        printf("     done!\n");
    #endif // DEBUG_INIT
    return NO_ERROR;
}

/* LO startup init */
/*! This function performs the operations necessary to initialize a LO during
    startup. These operations are performed only once during the startup
    sequence. This function is called by the frontendInit function one time for
    every available cartrdige so the currentModule veriable is updated every
    single time with the currently addressed WCA.
    \return
        - \ref NO_ERROR -> if no error occurred
        - \ref ERROR    -> if something wrong happened */
int loStartup(void){

    /* Few variables to help load the data from the configuration file */
    CFG_STRUCT dataIn;
    unsigned char cnt;
    char *str;
    char entryName[15];
    char entryText[50];
    static char loopBandwidthDefaults[10] = {
        99,     // band 1: don't care. fixed 2.5 MHz/V
        1,      // band 2: 1 -> 15MHz/V (Band 2,3,5,6,7,10)
        1,      // band 3
        0,      // band 4: 0 -> 7.5MHz/V (Band 4,8,9)
        1,      // band 5
        1,      // band 6
        1,      // band 7
        0,      // band 8
        0,      // band 9
        1       // band 10
    };

    printf(" LO %d configuration file: %s\n",
           currentModule+1,
           frontend.
            cartridge[currentModule].
             lo.
              configFile);

    printf(" Initializing LO %d...\n", currentModule+1);

    /* Load the PLL Loop Bandwidth value in the frontend variable default
       value. */
    printf("  - PLL Loop Bandwidth...\n");

    // No longer loading from INI file:
    frontend.
     cartridge[currentModule].
      lo.
       pll.
        loopBandwidthSelect[DEFAULT_VALUE] = loopBandwidthDefaults[currentModule];

    #ifdef DEBUG_STARTUP
        printf("    - PLL loop bandwidth default value %d...done!\n",
               frontend.
                cartridge[currentModule].
                 lo.
                  pll.
                   loopBandwidthSelect[DEFAULT_VALUE]);
    #endif /* DEBUG_STARTUP */

    printf("    done!\n"); // PLL loop bandwidth
    
    /* Set the limits for control messages */
    printf("  - Setting limits for control messages\n");
    printf("    - YTO coarse tuning\n");
    frontend.
     cartridge[currentModule].
      lo.
      yto.
       ytoCoarseTune[MIN_SET_VALUE]=YTO_COARSE_SET_MIN;

    frontend.
     cartridge[currentModule].
      lo.
       yto.
        ytoCoarseTune[MAX_SET_VALUE]=YTO_COARSE_SET_MAX;

    printf("      done!\n"); // YTO course tuning
    printf("    - Loading max safe power limits\n");

    // maxSafeLoPaESN gets filled:
    //  A valid 8-byte ESN -> The ESN data item from the PA_LIMITS table.
    //  8 bytes 00 -> No PA_LIMITS table was found.
    //  8 bytes FF -> PA_LIMITS table was found but no ESN entry found.

    frontend.
      cartridge[currentModule].
        lo.
          maxSafeLoPaTableSize=0;
    
    frontend.
      cartridge[currentModule].
        lo.
          maxSafeLoPaTable=NULL;

    memset(frontend.
            cartridge[currentModule].
             lo.
              maxSafeLoPaESN, 0, SERIAL_NUMBER_SIZE);

    /* Configure read array */
    dataIn.
     Name=LO_PA_LIMITS_ENTRIES_KEY;
    dataIn.
     VarType=Cfg_Byte;
    dataIn.
     DataPtr=&cnt;

    /* Access configuration file. */
    if (ReadCfg(frontend.
                 cartridge[currentModule].
                  lo.
                   configFile,
                LO_PA_LIMITS_SECTION,
                &dataIn) != LO_PA_LIMITS_EXPECTED) {
        /* not found: */
        cnt = 0;
    }

    /* Allocate the max safe LO PA entries table. */
    if (cnt > 0) {
        frontend.
         cartridge[currentModule].
          lo.
           maxSafeLoPaTable = (MAX_SAFE_LO_PA_ENTRY *) malloc(cnt * sizeof(MAX_SAFE_LO_PA_ENTRY));
        
        /* if allocation succeeeded... */
        if (frontend.
             cartridge[currentModule].
              lo.
               maxSafeLoPaTable != NULL) {

            /* store the table size. */
            frontend.
             cartridge[currentModule].
              lo.
               maxSafeLoPaTableSize = cnt;

            printf("    - ENTRIES=%d\n", (int) cnt);
        }
    }

    if (cnt > 0) {
        /* start loading the entries */
        
        unsigned char actualCnt=0;

        MAX_SAFE_LO_PA_ENTRY *nextEntry = frontend.
                                           cartridge[currentModule].
                                            lo.
                                             maxSafeLoPaTable;

        for(cnt=0;
            cnt<frontend.
                 cartridge[currentModule].
                  lo.
                   maxSafeLoPaTableSize;
            cnt++)
        {
            /* format entry name */
            sprintf(entryName, LO_PA_LIMITS_ENTRY_KEY, (int) (cnt + 1));
            printf(entryName);
            printf("=");

            /* Configure read array */
            dataIn.
             Name=entryName;
            dataIn.
             VarType=Cfg_String;
            dataIn.
             DataPtr=entryText;

            /* Access configuration file.  If error, ignore this entry */
            if(myReadCfg(frontend.
                          cartridge[currentModule].
                           lo.
                            configFile,
                         LO_PA_LIMITS_SECTION,
                         &dataIn,
                         LO_PA_LIMITS_EXPECTED)==NO_ERROR){

                //printf(entryText);

                memset(nextEntry, 0, sizeof(MAX_SAFE_LO_PA_ENTRY));

                str = strtok(entryText, " ,\t");
                if (str) {
                    (*nextEntry).ytoEndpoint = (unsigned int) atoi(str);
                    str = strtok(NULL, " ,\t");
                }
                if (str) {
                    (*nextEntry).maxVD0 = atof(str);
                    str = strtok(NULL, " ,\t");
                }
                if (str) {
                    (*nextEntry).maxVD1 = atof(str);
                }

                printf("yto=%u, vd0=%.2f, vd1=%.2f", 
                       (*nextEntry).ytoEndpoint, (*nextEntry).maxVD0, (*nextEntry).maxVD1);

                nextEntry++;
                actualCnt++;
            }                        
            printf("\n");
        }

        /* save the number of entries actually loaded */
        frontend.
         cartridge[currentModule].
          lo.
           maxSafeLoPaTableSize = actualCnt;

        if (actualCnt) {
            // Get the ESN from LO PA entries table
            dataIn.
             Name=LO_PA_LIMITS_ESN_KEY;
            dataIn.
             VarType=Cfg_String;
            dataIn.
             DataPtr=entryText;

            if (ReadCfg(frontend.
                         cartridge[currentModule].
                          lo.
                           configFile,
                        LO_PA_LIMITS_SECTION,
                        &dataIn) != LO_PA_LIMITS_ESN_EXPECTED) 
            {
                // not found.  Save 8-bytes FF to the table:
                memset(frontend.
                        cartridge[currentModule].
                         lo.
                          maxSafeLoPaESN, 0xFF, SERIAL_NUMBER_SIZE);
            
            } else {
                // found.  Parse it into byte array:
                str = entryText;
                for (cnt = 0; cnt < 8; cnt++) {
                    sscanf(str, "%2hhx", &frontend.
                                           cartridge[currentModule].
                                            lo.
                                             maxSafeLoPaESN[cnt]);
                    str += 2;
                }   
            }

            str = frontend.
                   cartridge[currentModule].
                    lo.
                     maxSafeLoPaESN;

            printf("    - ESN: %02X %02X %02X %02X %02X %02X %02X %02X\n",
                   str[0],
                   str[1],
                   str[2],
                   str[3],
                   str[4],
                   str[5],
                   str[6],
                   str[7]);
        }
    }
    printf("      done!\n"); // max safe power
    printf("    done!\n"); // Control message limits
    printf(" done!\n\n"); // LO
    return NO_ERROR;
}

/* LO subsystem shutdown */
/*! Free resources that were used by all LOs at shutdown time. */
int loShutdown(void){

    unsigned char module;
    printf(" Shutting Down LO Subsystem...\n");

    for(module=0;
        module<CARTRIDGES_NUMBER;
        module++){
    
        free(frontend.
         cartridge[module].
          lo.
           maxSafeLoPaTable);
    }

    printf(" done!\n\n"); // Control message limits

    return NO_ERROR;
}

/* LO handler */
/*! This function will be called by the CAN message handling subrutine when the
    received message is pertinent to the LO. */
void loHandler(void){
    #ifdef DEBUG
        printf("   LO\n");
    #endif /* DEBUG */

    /* It is assumed that if the cartridge is available, then the WCA is
       installed as well. */

    /* Check if the submodule is in range */
    currentLoModule=(CAN_ADDRESS&LO_MODULES_RCA_MASK)>>LO_MODULES_MASK_SHIFT;
    if(currentLoModule>=LO_MODULES_NUMBER){
        storeError(ERR_LO,
                   0x02); // Error 0x02 -> LO submodule out of range

        CAN_STATUS = HARDW_RNG_ERR; // Notify incoming CAN message of error

        return;
    }

    /* Call the correct handler */
    (loModulesHandler[currentLoModule])();
}


/* find entry in the max safe LO PA table */
/*! Find entry in the max safe LO PA table corresponding to the given YTO tuning word
    Perform linear interpolation if the given YTO word is between entries.
    If yto is above or below first and last entries in the table, return the nearest.
    
    \param yto      tuning word to look up

    \return         pointer to entry or NULL if not found */
MAX_SAFE_LO_PA_ENTRY *findMaxSafeLoPaEntry(unsigned int yto) {

    MAX_SAFE_LO_PA_ENTRY *entry = NULL;
    MAX_SAFE_LO_PA_ENTRY *lastEntry = NULL;
    MAX_SAFE_LO_PA_ENTRY *table = frontend.
                                   cartridge[currentModule].
                                    lo.
                                     maxSafeLoPaTable;

    unsigned char tableSize = frontend.
                               cartridge[currentModule].
                                lo.
                                 maxSafeLoPaTableSize;

    static MAX_SAFE_LO_PA_ENTRY interpEntry;
    //< if we need to interpolate, we'll return a pointer to this entry.

    float interpFactor;
    //< interpolation factor for scaling the maxVD0 and maxVD1 values betwen table entries.

    int index = 0;
    int done = 0;
    unsigned int lastEndpoint = 0;

    // if table is empty or not allocated, get out:
    if (tableSize == 0 || table == NULL)
        return NULL;

    // look at the first entry:
    entry = &table[0];

    // if there is only one entry or if the requested YTO is at or below 
    //  the first endpoint, return the first entry:
    if ((tableSize == 1) || (yto <= (*entry).ytoEndpoint))
        return entry;

    // look at the YTO tuning for the last entry in the table:
    lastEndpoint = table[tableSize - 1].ytoEndpoint;

    // if the requested YTO is at or above the last YTO tuning,
    //  return the last entry:
    if (yto >= lastEndpoint) {
        entry = &table[tableSize - 1];
        return entry;
    }

    // loop to find an exact endpoint match or a pair of entries
    //  between which to interpolate:
    index = 0;
    done = 0;
    while (!done) {
        // check for past end of table.
        if (index >= tableSize) {
            // this should be an impossible case because of lastEndpoint check above.
            lastEntry = entry = NULL;                        
            done = 1;

        } else {
            // save the last entry for endpoint comparison:
            lastEntry = entry;
            
            // get the current entry:
            entry = &table[index];
            
            // check for exact match current endpoint:
            if (yto == (*entry).ytoEndpoint)
                done = 1;

            // check for case where we need to interpolate:
            else if (lastEntry && (*lastEntry).ytoEndpoint < yto && yto < (*entry).ytoEndpoint) {
                // interpolate:
                interpEntry.ytoEndpoint = yto;
                interpFactor = ((float) (yto - (*lastEntry).ytoEndpoint)) / 
                               ((float) ((*entry).ytoEndpoint - (*lastEntry).ytoEndpoint));
                interpEntry.maxVD0 = (*lastEntry).maxVD0 + (interpFactor * 
                                     ((*entry).maxVD0 - (*lastEntry).maxVD0));
                interpEntry.maxVD1 = (*lastEntry).maxVD1 + (interpFactor * 
                                     ((*entry).maxVD1 - (*lastEntry).maxVD1));
                entry = &interpEntry;
                done = 1;          

            } else
                ++index;
        }
    }
    return entry;
}


/* LO PA max safe level limits check */
/*! Limit the CONV_FLOAT value about to be sent to the PA channel for drain voltage
      to the safe maximum for the current YTO tuning.

    \param paModule     which polarization PA channel to check.

    \return
        - \ref NO_ERROR -> if no error occurred
        - \ref HARDW_BLKD_ERR -> the drain voltage was disallowed by the max safe level table */
int limitSafePaDrainVoltage(unsigned char paModule) {

    MAX_SAFE_LO_PA_ENTRY *entry;
    unsigned int yto = frontend.
                        cartridge[currentModule].
                         lo.
                          yto.
                           ytoCoarseTune[CURRENT_VALUE];

//    printf("limitSafePaDrainVoltage yto=%u ", yto);

    entry = findMaxSafeLoPaEntry(yto);

    if (!entry) {   
//        printf("\n");
        return NO_ERROR;
    }

//    printf("maxVD0=%.2f maxVD1=%.2f ", (*entry).maxVD0, (*entry).maxVD1);

    if (paModule == 0) {
//        printf("vd0=%.2f\n", CONV_FLOAT);
        if (CONV_FLOAT > (*entry).maxVD0) {
            CONV_FLOAT = (*entry).maxVD0;
            return HARDW_BLKD_ERR;
        }
    }

    if (paModule == 1) {
//        printf("vd1=%.2f\n", CONV_FLOAT);
        if (CONV_FLOAT > (*entry).maxVD1) {
            CONV_FLOAT = (*entry).maxVD1;
            return HARDW_BLKD_ERR;
        }
    }

    return NO_ERROR;
}

/* LO PA max safe level limits check */
/*! Prior to YTO tuning to CONV_UINT(0), send commands to reduce the LO PA drain 
      voltages to the limits in the max safe level table.

    \param paModule     which polarization PA channel to check.

    \return
        - \ref NO_ERROR -> if no error occurred
        - \ref HARDW_BLKD_ERR -> the drain voltage was disallowed by the max safe level table */
int limitSafeYtoTuning(){
    MAX_SAFE_LO_PA_ENTRY *entry;
    unsigned int yto = CONV_UINT(0);
    long int backup = CONV_LONGINT;     // backup copy of the conversion buffer
    float vd0, vd1;
    unsigned char *lastCommandData;
    int ret0 = NO_ERROR;
    int ret1 = NO_ERROR;

//    printf("limitSafeYtoTuning yto=%u ", yto);

    entry = findMaxSafeLoPaEntry(yto);
    
    if (!entry) {
//        printf("\n");
        return NO_ERROR;
    }

    // make a pointer to the data word in the last commanded Pol0 PA drain voltage:
    currentPaModule=PA_CHANNEL_A;
    lastCommandData = &(frontend.
                         cartridge[currentModule].
                          lo.
                           pa.
                            paChannel[currentPaChannel()].
                             lastDrainVoltage.
                              data);

    // get the last commanded setting:
    changeEndian(CONV_CHR_ADD, lastCommandData);
    vd0 = CONV_FLOAT;
    
    // if we are about to exceed the max pol0 VD at the new yto tuning...
    if (vd0 > (*entry).maxVD0) {
        // use the max setting instead:
        CONV_FLOAT=(*entry).maxVD0;

        // save it back as the last commanded value
        changeEndian(lastCommandData, CONV_CHR_ADD);

        // send the command to reduce the LO PA drain voltage:
        currentPaChannelModule=PA_CHANNEL_DRAIN_VOLTAGE;
        if(setPaChannel()==ERROR)
            ret0 = ERROR;
        else
            ret0 = HARDW_BLKD_ERR;
    }

    // make a pointer to the data word in the last commanded Pol1 PA drain voltage:
    currentPaModule=PA_CHANNEL_B;
    lastCommandData = &(frontend.
                         cartridge[currentModule].
                          lo.
                           pa.
                            paChannel[currentPaChannel()].
                             lastDrainVoltage.
                              data);

    // get the last commanded setting:
    changeEndian(CONV_CHR_ADD, lastCommandData);
    vd1 = CONV_FLOAT;

    // if we are about to exceed the max pol0 VD at the new yto tuning...
    if (vd1 > (*entry).maxVD1) {
        // use the max setting instead:
        CONV_FLOAT=(*entry).maxVD1;

        // save it back as the last commanded value
        changeEndian(lastCommandData, CONV_CHR_ADD);

        // send the command to reduce the LO PA drain voltage:
        currentPaChannelModule=PA_CHANNEL_DRAIN_VOLTAGE;
        if(setPaChannel()==ERROR)
            ret1 = ERROR;
        else
            ret1 = HARDW_BLKD_ERR;
    }

//    printf("maxVD0=%.2f maxVD1=%.2f vd0=%.2f vd1=%.2f\n", (*entry).maxVD0, (*entry).maxVD1, vd0, vd1);

    // restore the conversion buffer to its prior state:
    CONV_LONGINT = backup;

    // return any error which was seen:
    if (ret0 == ERROR || ret1 == ERROR)
        return ERROR;
    // HARDW_BLKD_ERR tells caller that one or both PA VD settings were reduced:
    if (ret0 == HARDW_BLKD_ERR || ret1 == HARDW_BLKD_ERR)
        return HARDW_BLKD_ERR;
    return NO_ERROR;
}
