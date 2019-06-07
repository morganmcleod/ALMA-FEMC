//  $Id: arpapi.h 1.7 2005/01/13 17:06:04Z johnb Exp $
//
//  *** DO NOT REMOVE THE FOLLOWING NOTICE ***
//
//  Copyright (c) 1999-2005 Datalight, Inc.
//  All Rights Reserved
//
//  Datalight, Inc. is a Washington corporation located at:
//
//  21520 30th Dr SE, Suite 110,          Tel: +1.425.951.8086
//  Bothell, WA  98021                    Fax: +1.425.951.8094
//  USA                                   Http://www.datalight.com
//
//  This software, including without limitation all source code and
//  documentation, is the confidential, trade secret property of
//  Datalight, Inc., and is protected under the copyright laws of
//  the United States and other jurisdictions.  Portions of the
//  software may also be subject to one or more the following US
//  patents: US#5860082, US#6260156.
//
//  In addition to civil penalties for infringement of copyright
//  under applicable U.S. law, 17 U.S.C. 1204 provides criminal
//  penalties for violation of (a) the restrictions on circumvention
//  of copyright protection systems found in 17 U.S.C. 1201 and
//  (b) the protections for the integrity of copyright management
//  information found in 17 U.S.C. 1202.
//
//  U.S. Government Restricted Rights.  Use, duplication,
//  reproduction, or transfer of this commercial product and
//  accompanying documentation is restricted in accordance with
//  FAR 12.212 and DFARS 227.7202 and by a License Agreement.
//
//  IN ADDITION TO COPYRIGHT AND PATENT LAW, THIS SOFTWARE IS
//  PROTECTED UNDER A SOURCE CODE AGREEMENT, NON-DISCLOSURE
//  AGREEMENT (NDA), OR SIMILAR BINDING CONTRACT BETWEEN DATALIGHT,
//  INC. AND THE LICENSEE ("BINDING AGREEMENT").  YOUR RIGHT, IF ANY,
//  TO COPY, PUBLISH, MODIFY OR OTHERWISE USE THE SOFTWARE,IS SUBJECT
//  TO THE TERMS AND CONDITIONS OF THE BINDING AGREEMENT, AND BY
//  USING THE SOFTWARE IN ANY MANNER, IN WHOLE OR IN PART, YOU AGREE
//  TO BE BOUND BY THE TERMS OF THE BINDING AGREEMENT.  CONTACT
//  DATALIGHT, INC. AT THE ADDRESS SET FORTH ABOVE IF YOU OBTAINED
//  THIS SOFTWARE IN ERROR.
//
/* ARPAPI.H
 * Header file for ARPAPI
 */
// Copyright (c) GP van Niekerk Ondernemings 1990-2002
// Portions Copyright (c) 1986-1989 Phil Karn, KA9Q

/* Size of ARP hash table */
#define  ARPSIZE  17
/* Maximum ARP queue per target */
#define  MAX_ARP_Q 10
/* Lifetime of a valid ARP entry */
#define  ARPLIFE     900   /* 15 minutes */
/* Lifetime of a pending ARP entry */
#define  PENDTIME 15       /* 15 seconds */

/* ARP definitions (see RFC 826) */

/* Address size definitions */
#define  IPALEN      4     /* Length in bytes of an IP address */
#define  MAXHWALEN   16    /* Maximum length of a hardware address */

/* Hardware types */
#define  ARP_ETHER   1  /* Assigned to 10 megabit Ethernet */
#define  ARP_EETHER  2  /* Assigned to experimental Ethernet */
#define  ARP_AX25    3  /* Assigned to AX.25 Level 2 */
#define  ARP_PRONET  4  /* Assigned to PROnet token ring */
#define  ARP_CHAOS   5  /* Assigned to Chaosnet */
#define  ARP_IEEE    6  /* RFC 1042 */
#define  ARP_ARCNET  7
#define  ARP_APPLETALK  8
#define  ARP_IPX     9

extern char *arptypes[];   /* Type fields in ASCII, defined in arpcmd */
#define  NHWTYPES 10

/* Format of ARP table */
struct arp_tab {
   WORD next     __ATTR__; // struct arp_tab __near *;   /* Doubly-linked list pointers */
   WORD prev     __ATTR__; // struct arp_tab __near *;
   DWORD ip_addr __ATTR__; /* IP Address, host order */
   WORD hardware __ATTR__; /* Hardware type */
   WORD hw_addr  __ATTR__; /* BYTE __near *, Hardware address */
   BYTE state    __ATTR__;
#define  ARP_PENDING 0
#define  ARP_VALID   1
   TIMER timer   __ATTR__; /* Time until aging this entry */
   WORD pending  __ATTR__; /* void __near, * Queue of datagrams awaiting resolution */
   WORD rif      __ATTR__; // BYTE __near *, Pointer to routing information
   WORD psIface  __ATTR__; // void __near *, Interface pointer
};


/*
 * Local Variables:
 * tab-width:3
 * End:
 */

/*  EOF: arpapi.h */


