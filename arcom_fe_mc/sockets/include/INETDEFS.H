//
//   $Id: INETDEFS.H 1.17 2009/01/17 00:59:40Z gerritvn Exp $
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
// INETDEFS.H

#ifndef _INETDEFS_H
#define _INETDEFS_H

#if defined(_SOCK32)
#define _Cdecl
#define far
#define near
#define interrupt
#define _fstrnicmp strnicmp
#define _fstrchr strchr
#define _fstrlen strlen
#define _fmemcpy memcpy
#define _fmemcmp memcmp
#define _fstrncpy strncpy
#define _fstrcpy strcpy
#define _fstrcmp strcmp
#define _fstricmp stricmp
#define farfree free
#define farmalloc malloc
#define farcalloc calloc
#ifndef FS
#define FS "%s"
#endif
#define FFS "s"
#define PASCAL
#ifdef __DJGPP__
#define idisable() asm("pushf; cli");
#define popf() asm("popf");
#define SPAWNARGTYPE char * const *
#else //__DJGPP__
#define idisable()
#define popf()
#define SPAWNARGTYPE const char * const *
#endif //__DJGPP__
#define REGP __dpmi_regs
#define intr __dpmi_int
#define r_ax x.ax
#define r_bx x.bx
#define r_cx x.cx
#define r_dx x.dx
#define r_si x.si
#define r_di x.di
#define r_es x.es
#define r_ds x.ds
#define r_flags x.flags

#define SETREGPTR(rs,s,o,p) rs.o = (int)p

#if !defined(__PACKED__)
#define __PACKED__
#endif //__PACKED__
#else //_SOCK32
#define SETREGPTR(rs,s,o,p) rs.s = FP_SEG(p);rs.o = FP_OFF(p)
#define MSDOS
// save and restore interrupt state
#define idisable() asm pushf;asm cli
#define SPAWNARGTYPE char **
#ifdef KERNEL
void far popf(void);
#else //KERNEL
#define popf() asm popf
#endif //KERNEL
#ifndef FS
#define FS "%Fs"
#endif
#define FFS "Fs"
#if defined(__DOS_H) && !defined(__WIN32__)
typedef struct REGPACK REGP;
#endif
#define __PACKED__
#endif

typedef struct {
#ifndef _SOCK32
#ifdef KERNEL
   unsigned reserved[8];
   unsigned bx;
   unsigned ebx;
   unsigned dx;
   unsigned edx;
   unsigned cx;
   unsigned ecx;
   unsigned eax[2];
   unsigned es;
   unsigned ax;
   unsigned di;
   unsigned si;
   unsigned ds;
   unsigned flags;
   unsigned sp;
   unsigned ss;
#else //KERNEL
   unsigned es;
   unsigned dx;
   unsigned cx;
   unsigned bx;
   unsigned ax;
   unsigned di;
   unsigned si;
   unsigned ds;
   unsigned flags;
   unsigned sp;
   unsigned ss;
#endif //KERNEL
#else //_SOCK32
   unsigned long di;
   unsigned long si;
   unsigned long bp;
   unsigned long es;
   unsigned long bx;
   unsigned long dx;
   unsigned long cx;
   unsigned long ax;
   unsigned long flags;
#endif //_SOCK32
} API_REG;

#ifdef KERNEL
#ifdef APL
#if APL
#define printf winprintf
#else
#define printf biosprintf
int cdecl biosprintf(const char *fmt, ...);
#endif
#endif
#endif

#ifdef _IOFBF		/* test for stdio.h included */
#define Stdout ((FILE *)1)
#endif

#define	READ_BINARY	0
#define	WRITE_BINARY	1
extern char *binmode[];

typedef long int32;				// 32-bit signed integer
//typedef unsigned short int16;	// 16-bit unsigned integer
#ifndef __WIN_TYPES
typedef unsigned short WORD;	// windows.h compatibility
typedef unsigned long DWORD;
typedef unsigned char BYTE;
#define __WIN_TYPES
#endif

#ifdef KERNEL
#ifdef ALLOC_TRACE
#include "allocdef.h"
#define malloct(size, id) Malloc(size, id)
#define calloct(items, size, id) Calloc(items, size, id)
#define realloct(block, size, id) ReAlloc(block, size, id)
#define strdupt(s,id) Strdup(s,id)
void *Malloc(unsigned size, unsigned id);
void *Calloc(unsigned items, unsigned size, unsigned id);
void *ReAlloc(void *block, unsigned size, unsigned id);
char *Strdup(const char *s, unsigned id);
#else
#define malloct(size, id) Malloc(size)
#define calloct(items, size, id) Calloc(items,size)
#define realloct(block, size, id) ReAlloc(block, size)
#define strdupt(s,id) Strdup(s)
void *Malloc(unsigned size);
void *Calloc(unsigned items, unsigned size);
void *ReAlloc(void *block, unsigned size);
char *Strdup(const char *s);
#endif
#endif

#define	uchar(x) ((unsigned char)(x))
#define	MAXINT16 65535U		/* Largest 16-bit integer */

/* Define null object pointer in case stdio.h isn't included */
#ifndef	NULL
/* General purpose NULL pointer */
#define	NULL (void *)0
#endif
#define	NULLCHAR (char *)0	   /* Null character pointer */
#define	NULLFP	(int (*)())0	/* Null pointer to function returning int */
#define	NULLVFP	(void (*)())0	/* Null pointer to function returning void */
#define  VFP      void (*)()
#define	NULLFILE (FILE *)0	   /* Null file pointer */

/* General purpose function macros */
#ifndef min
#define	min(x,y)	((x)<(y)?(x):(y))	/* Lesser of two args */
#define	max(x,y)	((x)>(y)?(x):(y))	/* Greater of two args */
#endif

/* Extract a short from a long */
#define	hiword(x)	((WORD)((x) >> 16))
#define	loword(x)	((WORD)(x))

/* Extract a byte from a short */
#define	hibyte(x)	(((x) >> 8) & 0xff)
#define	lobyte(x)	((x) & 0xff)

/* Extract nibbles from a byte */
#define	hinibble(x)	(((x) >> 4) & 0xf)
#define	lonibble(x)	((x) & 0xf)

// prototypes

int htoi(char *s);
char *getfilename(char *fullname,char *partname,int dos);
void set_palette(int fileflag);   // set palette for INET
void restore_palette(void);
int winprintf(const char *ln, ...);
void check_time(void);
void fatal(char *message);
void schedule(void);
void rip(char *s);
DWORD get32(BYTE *);
void SetRand(int iSeed, void *pSeed);

#endif

#ifdef DEB
int cdecl debugprintf(int row,const char *fmt, ...);
#define deb1(r,a) debugprintf(r,a)
#define deb2(r,a,b) debugprintf(r,a,b)
#define deb3(r,a,b,c) debugprintf(r,a,b,c)
#define deb4(r,a,b,c,d) debugprintf(r,a,b,c,d)
#define DE	0x100
#else
#define deb1(r,a)
#define deb2(r,a,b)
#define deb3(r,a,b,c)
#define deb4(r,a,b,c,d)
#endif

#ifdef NOMIB2
#define MIB2(x)
#else
#define MIB2(x) x
#endif

#ifndef NORIP
#define RIPC(x) x
#else
#define RIPC(x)
#endif

#ifndef NOALTIF
#define ALTIFC(x) x
#else
#define ALTIFC(x)
#endif

#ifndef NOCHAP
#define CHAPC(x) x
#else
#define CHAPC(x)
#endif

#ifndef NOMODEM
#define MODEMC(x) x
#else
#define MODEMC(x)
#endif

#if APL
#define APLC(x) x
#define SOFAR far
#else
#define APLC(x)
#define SOFAR
#endif

#ifdef _OSE
#define disable(x)
#define enable(x)
#endif //#ifdef _OSE
