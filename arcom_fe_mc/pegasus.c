/*! \file   pegasus.c
    \brief  ARCOM Pegasus board functions

    <b> File informations: </b><br>
    Created: 2004/08/24 16:46:19 by avaccari

    <b> CVS informations: </b><br>
    \$Id: pegasus.c,v 1.13 2009/03/25 14:45:26 avaccari Exp $

    This file contains the functions necessary to initialize, and operate the
    ARCOM Pegasus board.*/

/* Includes */
#include <dos.h>
#include "error.h"
#include "pegasus.h"

/* Globals */
/* These are globals because it makes it easier for the inline assembly
   section. */
struct PACKETIN PacketIn;
int error;
/* Externs */

/*! Reboot the ARCOM Pegasus board. This function should be used for debug
    purposes only.
    This function is written in assembly to be able to address a specific
    memory location.
    \warning    With the current code implementation, the AMBSI1 board has no
                way of knowing if the ARCOM Pegasus board has been rebooted.
                It doesn't really matter because once the AMBSI1 register the
                RCAs, even if the ARCOM reboots, after a few timeouts the
                communication should be established again.
    \todo       Check the previous statement! ;-) Also check stability of this
                function... it doesn't always work... Actually it only works
                twice then everything hangs up... Need fixing. */

void reboot(void){
    _asm{
        mov ax,0xf000
        mov ds,ax
        mov bx,0xfff0
        jmp ds:bx
    }
}

/* Get Volume Serial number */
/*! Returns the serial number of the current disk
    \warning    This is valid as long as we use large models. It should be
                modified to accomodate also small models. */
long int getVolSerial(void){

    _asm {
        ; Save current segments info on stack
        push    ds;
        ; Load PacketIn segment info in DS
        mov     ax,SEG PacketIn
        mov     ds,ax
        ; Load registers for int 21h call
        mov     ah,44h      ; IOCtl call
        mov     al,0Dh      ; Block Device generic IOCtl request
        mov     bl,00h      ; Set current Device to boot device
        mov     ch,08h      ; Set current Device to Block device
        mov     cl,66h      ; Get Media ID
        lea     dx,PacketIn ; Load PacketIn offset info in dx
        ; Call dos interrupt
        int     21h
        ; Stores error
        mov     error,ax
        ; Restore previous segment info from stack
        pop     ds;
    }

    /* Check if error */
    if(error!=NO_ERROR){
        return ERROR;
    }

    /* Return the serial number */
    return PacketIn.Serial;
}





