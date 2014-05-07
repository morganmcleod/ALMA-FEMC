/*! \file   pegasus.c
    \brief  ARCOM Pegasus board functions

    <b> File informations: </b><br>
    Created: 2004/08/24 16:46:19 by avaccari

    <b> CVS informations: </b><br>
    \$Id: pegasus.c,v 1.11 2007/09/07 22:00:06 avaccari Exp $

    This file contains the functions necessary to initialize, and operate the
    ARCOM Pegasus board.*/

/* Includes */

/* Globals */
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
    /* Assumes Small Model */
    _asm{
        mov ax,0xffff
        mov bx,0xfff0
        jmp ax:bx
    }
}


