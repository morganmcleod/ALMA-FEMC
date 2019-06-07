//
//   $Id: CMDPARSE.H 1.6 2006/01/30 06:00:04Z johnb Exp $
//
//   *** DO NOT REMOVE THE FOLLOWING NOTICE ***
//
//   Copyright (c) 1993 - 2005 Datalight, Inc.
//   All Rights Reserved
//
//   Datalight, Inc. is a Washington corporation with an address at
//   21520 30TH DR SE MS110 Bothell, WA, 98021
//   Tel: 800.221.6630  Fax: 425.951.8091
//   Http://www.datalight.com
//
//   This software, including without limitation all source code and
//   documentation, is the confidential, trade secret property of
//   Datalight, Inc., and is protected under the copyright laws of
//   the United States and other jurisdictions. Portions of the
//   software may also be subject to one or more the following US
//   patents: US#5860082, US#6260156.
//
//   In addition to civil penalties for infringement of copyright
//   under applicable U.S. law, 17 U.S.C. §1204 provides criminal
//   penalties for violation of (a) the restrictions on circumvention
//   of copyright protection systems found in 17 U.S.C. §1201 and
//   (b) the protections for the integrity of copyright management
//   information found in 17 U.S.C. §1202.
//
//   U.S. Government Restricted Rights. Use, duplication,
//   reproduction, or transfer of this commercial product and
//   accompanying documentation is restricted in accordance with
//   FAR 12.212 and DFARS 227.7202 and by a License Agreement.
//
//   IN ADDITION TO COPYRIGHT AND PATENT LAW, THIS SOFTWARE IS
//   PROTECTED UNDER A SOURCE CODE AGREEMENT, NON-DISCLOSURE
//   AGREEMENT (NDA), OR SIMILAR BINDING CONTRACT BETWEEN DATALIGHT,
//   INC. AND THE LICENSEE ("BINDING AGREEMENT"). YOUR RIGHT, IF ANY,
//   TO COPY, PUBLISH, MODIFY OR OTHERWISE USE THE SOFTWARE,IS SUBJECT
//   TO THE TERMS AND CONDITIONS OF THE BINDING AGREEMENT, AND BY
//   USING THE SOFTWARE IN ANY MANNER, IN WHOLE OR IN PART, YOU AGREE
//   TO BE BOUND BY THE TERMS OF THE BINDING AGREEMENT. CONTACT
//   DATALIGHT, INC. AT THE ADDRESS SET FORTH ABOVE IF YOU OBTAINED
//   THIS SOFTWARE IN ERROR.
//
/* CMDPARSE.H
Updated @ Date 5 March 1996, 22:41 by Gerrit van Niekerk
*/

#define  NARG     12 /* Max number of args to commands */

struct cmds {
   char name[9];     /* Name of command */
   int (*func)(int argc,char *argv[]
      #if APL
      ,void *pvfArg
      #endif
                  );/* Function to execute command */
};
struct scmds {
   char name[9];     /* Name of command */
   int (*func)(int argc,char *argv[],void *p);     /* Function to execute command */
};
extern struct cmds SOFAR cmds[];

#ifndef  NULLCHAR
#define  NULLCHAR (char *)0
#endif

int _Parse(char *line,char **argv);
int cmdparse(struct cmds SOFAR *cmds,char *line,char error);
int subcmd(struct scmds SOFAR *tab,int argc,char *argv[],void *p);
int fstrcmp(char *str1,char far *str2);
int setbool(int *var,char *label,int argc,char *argv[]);
int bit16cmd(WORD *bits, WORD mask, char *label, int argc, char *argv[]);
int setint(int *var,char *label,int argc,char *argv[]);
int setlong(int32 *var,char *label,int argc,char *argv[]);
int setshort(unsigned short *var,char *label,int argc,char *argv[]);
int setuns(unsigned *var,char *label,int argc,char *argv[]);
