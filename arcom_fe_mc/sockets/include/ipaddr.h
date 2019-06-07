//
//   $Id: ipaddr.h 1.4 2005/01/13 17:06:06Z johnb Exp $
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
//   Definition of IP address representation

#ifndef __IPADDR_H
#define __IPADDR_H

#ifdef IPV6
#define  IP_ADLEN       16
#else
#define  IP_ADLEN       4
#endif

// union representing IP address

typedef union ipaddr {
   BYTE abAddr[IP_ADLEN];        // address in bytes
   WORD awAddr[IP_ADLEN / 2];    // address in words
   DWORD adwAddr[IP_ADLEN / 4];  // address in dwords
   DWORD dwAddr;                 // IPv4 DWORD address
} IPAD;

// union representing IP address in IPv4 stack

typedef union ipaddr4 {
   BYTE abAddr[4];         // address in bytes
   WORD awAddr[2];         // address in words
   DWORD adwAddr[1];       // address in dwords
   DWORD dwAddr;           // IPv4 DWORD address
} IPAD4;


// union representing IP address in IPv4/IPv6 and IPv6 stack

typedef union ipaddr6 {
   BYTE abAddr[16];        // address in bytes
   WORD awAddr[8];         // address in words
   DWORD adwAddr[4];       // address in dwords
   DWORD dwAddr;           // IPv4 DWORD address
} IPAD6;
#endif

