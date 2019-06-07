//  $Id: compiler.h 1.20 2008/11/03 20:57:00Z gerritvn Exp $
//
//  *** DO NOT REMOVE THE FOLLOWING NOTICE ***
//
//  Copyright (c) 2000-2008 Datalight, Inc.
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
/*
   $DOCFILE:COMPILER.H

   Compiler specific header file for the
   Sockets Compatible API (CAPI)

   $DOCHISTORY:
        09/18/2008 gvn PUSH_DS and POP_DS for Watcom large data models 
        02/04/2008 gvn new DOS buffer allocation scheme, WATCOM support with
                       major contribution from Paul Santa Maria from Twelve Strike
        11/26/2004 gvn change X86Regs, add FP_Seg and FP_Off definitions
        07/21/2003 gvn change Borland CLK_TCK floating point definition
        07/19/2002 jmb added CARRY_FLAG comparison macro
        10/03/2001 jmb added macros to support DJGPP
        05/11/2001 jmb updated ISR_ROUTINE
        03/29/2001 jmb removed MSVC compatibility changaes -- caused too
                       many problems.
        03/20/2001 jmb Fixed REALMODE_SEGMENT, REALMODE_OFFSET and
                       MAKE_REAL POINTERS macros to work under MSVC.
                       I also added a #define for interrupt to be
                       compatible with MSVC.
   1.00 2000-11-29 pkg Extracted from CAPI.H
*/

/*
   This define must be set to produce a 32 bit (seg:off pointer)
*/
#ifndef _COMPILER_H
#define _COMPILER_H

#ifdef __cplusplus
  extern "C" {
#endif

#ifdef __DJGPP__
   #include <dpmi.h>
   #include <go32.h>
   #include <sys\exceptn.h>
#endif

#ifdef __WATCOMC__
   #ifdef __FLAT__
      #define __WATCOMF__
   #else
      #define __WATCOM16__
   #endif
   #define ARGV_CAST char const * const *
   extern void PUSH_DS(void);
   #pragma aux PUSH_DS = "push ds"

   extern void POP_DS(void);
   #pragma aux POP_DS = "pop ds"

#else
   #define ARGV_CAST char **
   #define PUSH_DS()
   #define POP_DS()
#endif

#if !defined(__32BIT__) && (defined(__DJGPP__) || defined(__WIN32__) || defined(_OSE) || defined(__WATCOMF__))
   #define __32BIT__
#endif

#if defined(__DJGPP__) && !defined(_SOCK32)
/*
   The following macro allows a structure to be created without
   inserting gaps to align for faster access
*/
   #define __ATTR__ __attribute__((packed))
#else
   #define __ATTR__
#endif

#if !defined(__PACKED__)
   #if defined(__DJGPP__) && !defined(_SOCK32)
      #define __PACKED__ __attribute__((packed))
   #elif defined(__WATCOMC__) && !defined(_SOCK32)
      #define __PACKED__
   #else
      #define __PACKED__
   #endif
#endif //__PACKED__

#if defined(__WATCOMC__) && !defined(_SOCK32)
   #define __SPACKED__ _Packed
#else
   #define __SPACKED__
#endif

#if !defined(D_FAR) && defined(__32BIT__)
   #define D_FAR
#else
   #define D_FAR far
#endif

#ifdef __32BIT__
   #define LOADDS
   #define far
   #define huge
   #ifndef FS
      #define FS "%s"
   #endif
   #if defined(__WATCOMF__)
      #define FAR
      #include <i86.h>     // Watcom versions of REGS, SREGS, int386 (), etc
      #include <string.h>
   #endif
   #if defined(__DJGPP__) || defined(__WATCOMF__) || defined(_SOCKLIB)
      #define CARRY_FLAG_CHECK ((px86regs->r.x.flags & 0x0001) > 0)
   #else
      #define CARRY_FLAG_CHECK (px86regs->r.x.cflag)
   #endif
   #define ISR_ROUTINE
#else   //#ifdef __32BIT__
   #define LOADDS __loadds
   #ifndef FS
      #define FS "%Fs"
   #endif
   #ifndef REGS
      #include <dos.h>
   #endif //REGS
   #define CARRY_FLAG_CHECK (px86regs->r.x.cflag)
   // Function declaration prefix for an ISR routine.
   #define ISR_ROUTINE __loadds
#endif //#else #ifdef __32BIT__

/*
   Portable types which must be set:
   BYTE (8bits) WORD (16bits) DWORD (32bits)
*/
#ifndef __WIN_TYPES
 #define __WIN_TYPES
 typedef unsigned char BYTE;
 typedef unsigned short WORD;
 typedef unsigned long DWORD;
#endif

/*
   The following macros are used to work with both native
   and real mode pointers and buffers.  Each is generally
   unique to a given compiler and environment.  For
   example, an application using the 32-bit DJGPP compiler
   and the 16 bit Sockets stack will require a specially
   allocated buffer and a means to move data between both
   native and real mode buffers.

   Each macro must be carefully written for a particular
   environment.
*/
#ifdef _SOCK32
#define SET_32BITREG(r1,r2,v) r2 = (DWORD)v
#else
#define SET_32BITREG(r1,r2,v) r1 = (WORD)(v >> 16);r2 = (WORD)v
#endif //_SOCK32

#ifdef _SOCK32
#define REALMODE_SEGMENT(s,p)   ((DWORD)p)
#define REALMODE_SEGMENTC(s,p)  ((DWORD)p)
#define SET_SEGMENT(r,s,p)
#define SET_SEGMENTC(r,s,p)
#define SET_SEGMENTZ(r)
#define REALMODE_OFFSET(s,p)    ((DWORD)p)
#define REALMODE_OFFSETC(s,p)   ((DWORD)p)
#define MAKE_REAL_POINTER(s, o) ((DWORD)o)

#elif defined(__DJGPP__) || defined(__WATCOMF__)

#define REALMODE_SEGMENT(s,p)   (s).rm_segment
#define REALMODE_SEGMENTC(s,p)  (p ? (s).rm_segment : 0)
#define SET_SEGMENT(r,s,p)      r = (s).rm_segment
#define SET_SEGMENTC(r,s,p)     r = (p ? (s).rm_segment : 0)
#define SET_SEGMENTZ(r)         r = 0
#define REALMODE_OFFSET(s,p)    (s).rm_offset
#define REALMODE_OFFSETC(s,p)     (p ? (s).rm_offset : 0)
#define MAKE_REAL_POINTER(s, o) (DWORD)(((DWORD)(s) << 4l) \
                                   + ((DWORD)(o) & 0xFFFFl))
#define DPMI_INT(i) int (i)
#else

#ifndef __WATCOMC__
#define REALMODE_SEGMENT(s,p)   (WORD)((DWORD)(p) >> 16)
#define REALMODE_SEGMENTC(s,p)  (p ? (WORD)((DWORD)(p) >> 16) : 0)
#define SET_SEGMENT(r,s,p)      r = (WORD)((DWORD)(p) >> 16)
#define SET_SEGMENTC(r,s,p)     r = (p ? (WORD)((DWORD)(p) >> 16) : 0)
#else //__WATCOMC__
#define REALMODE_SEGMENT(s,p)   FP_SEG(p)
#define REALMODE_SEGMENTC(s,p)  (p ? FP_SEG(p) : 0)
#define SET_SEGMENT(r,s,p)      r = FP_SEG(p)
#define SET_SEGMENTC(r,s,p)     r = (p ? FP_SEG(p) : 0)
#endif //__WATCOMC__
#define SET_SEGMENTZ(r)         r = 0
#define REALMODE_OFFSET(s,p)    (WORD)((DWORD)(p) & 0xFFFFL)
#define REALMODE_OFFSETC(s,p)   (p ? (WORD)((DWORD)(p) & 0xFFFFL) : 0)
#define MAKE_REAL_POINTER(s, o) (DWORD)(((DWORD)(s) << 16) \
                                   + ((DWORD)(o) &0xFFFFL))
#endif

// define CAPI Callback function
#ifdef __WATCOMF__
   #define CAPI_CB __interrupt
#else
   #define CAPI_CB D_FAR LOADDS
#endif

#if (defined(__DJGPP__) || defined(__WATCOMF__)) && !defined(_SOCK32)
#define DPMI_16BIT_STACK 1
#define NUM_DOSBUF_LEVELS 4
typedef unsigned short PSTRUCT; // internal pointer to structure

//
// define structure to hold pm selector as well as realmode seg and offset
// of transfer buffer
#ifdef __DJGPP__
#define DPMIINFO_STRUCT  _go32_dpmi_seginfo

#define DPMI_ALLOCATE_DOS_MEMORY(i) _go32_dpmi_allocate_dos_memory(i)
#define DPMI_RESIZE_DOS_MEMORY(i) _go32_dpmi_resize_dos_memory(i)
#define DPMI_FREE_DOS_MEMORY(i) _go32_dpmi_free_dos_memory(i)
#define _dpmi_simulate_fcall(r) _go32_dpmi_simulate_fcall(r)

// copy pmode data into transfer buffer
#define DOSBUF_PUT(s, p, n)  dosmemput(p, n, (s.rm_segment<<4)+s.rm_offset)
// copy data from transfer buffer to pmode
#define DOSBUF_GET(s, p, n)  dosmemget((s.rm_segment<<4)+s.rm_offset, n, p)
// allocate a callback
#define DPMI_ALLOC_RMCB(i,r) __dpmi_allocate_real_mode_callback((void *)(i)->pm_offset,r,(__dpmi_raddr *)&(i)->rm_offset)
#define DPMI_FREE_RMCB(i)  __dpmi_free_real_mode_callback((__dpmi_raddr *)&(i)->rm_offset)

// Real Mode Callbacks
#define ENTER_RMCB()  __asm__ ( \
   "push   %%es\n\t"\
   "push   %%ds\n\t"\
   "push   %%es\n\t"\
   "pop    %%ds\n\t"\
   "movw   ___djgpp_ds_alias,%%ax\n\t"\
   "mov    %%ax,%%ds\n\t"\
   "mov    %%ax,%%es\n\t"\
   "mov    %%eax,%%fs\n\t"\
   "mov    %%eax,%%gs\n\t"\
   "mov    __pcRmCbStackTop,%%ebx\n\t"\
   "cld\n\t"\
   "mov    %%esp,%%ecx\n\t"\
   "mov    %%ss,%%dx\n\t"\
   "mov    %%ax,%%ss\n\t"\
   "mov    %%ebx,%%esp\n\t"\
   "push   %%edx\n\t"\
   "push   %%ecx\n\t"\
   "push   %%esi\n\t"\
   "push   %%edi\n\t" ::: "ax","bx","cx","dx")

#define RMCB_PARAMS() \
   iSocket = cb_regs.x.bx;\
   iEvent  = cb_regs.x.cx;\
   wArgLo  = cb_regs.x.dx;\
   wArgHi  = cb_regs.x.si;\
   wHintHi = cb_regs.x.es;\
   wHintLo = cb_regs.x.di

// The following code makes assumptions about the prologue code generated by the
// compiler which may be different for different optimization options.
#define EXIT_RMCB()  __asm__ ( \
   "pop    %%edi\n\t"\
   "pop    %%esi\n\t"\
   "pop    %%eax\n\t"\
   "pop    %%ebx\n\t"\
   "mov    %%bx,%%ss\n\t"\
   "mov    %%eax,%%esp\n\t"\
   "pop    %%ds\n\t"\
   "pop    %%es\n\t"\
   "mov    (%%esi),%%eax\n\t"\
   "mov    %%eax,%%es:0x2a(%%edi)\n\t"\
   "addw   $0x4,%%es:0x2e(%%edi)\n\t"\
   "pop    %%ebx\n\t"\
   "leave\n\t"\
   "iret\n\t" ::: "ax","bx","cx","dx")

#endif

#if defined(__WATCOMF__)
/*
 * Adapted from DJGPP dpmi.h, modified for Watcom
 */
typedef union {
  __SPACKED__ struct {
    unsigned long edi;
    unsigned long esi;
    unsigned long ebp;
    unsigned long esp;
    unsigned long ebx;
    unsigned long edx;
    unsigned long ecx;
    unsigned long eax;
  } d;
  __SPACKED__ struct {
    unsigned short di, di_hi;
    unsigned short si, si_hi;
    unsigned short bp, bp_hi;
    unsigned short esp, esp_hi;
    unsigned short bx, bx_hi;
    unsigned short dx, dx_hi;
    unsigned short cx, cx_hi;
    unsigned short ax, ax_hi;
    unsigned short flags;
    unsigned short es;
    unsigned short ds;
    unsigned short fs;
    unsigned short gs;
    unsigned short ip;
    unsigned short cs;
    unsigned short sp;
    unsigned short ss;
  } x;
  __SPACKED__ struct {
    unsigned char edi[4];
    unsigned char esi[4];
    unsigned char ebp[4];
    unsigned char esp[4];
    unsigned char bl, bh, ebx_b2, ebx_b3;
    unsigned char dl, dh, edx_b2, edx_b3;
    unsigned char cl, ch, ecx_b2, ecx_b3;
    unsigned char al, ah, eax_b2, eax_b3;
  } h;
} __dpmi_regs;

__SPACKED__ struct _dpmi_callregs
{
  DWORD edi, esi, ebp, esp;
  DWORD ebx, edx, ecx, eax;
  WORD flags;
  WORD es, ds;
  WORD fs, gs;
  WORD ip, cs;
  WORD sp, ss;
};

typedef __SPACKED__ struct {
  unsigned short offset16 __PACKED__ ;
  unsigned short segment __PACKED__;
} __dpmi_raddr;

typedef __SPACKED__ struct {
  unsigned long size;          /* size (16-byte paragraphs */
  unsigned long pm_offset;     /* Protected mode selector & offset */
  unsigned short pm_selector;
  unsigned short rm_offset;    /* Real mode segment & offset */
  unsigned short rm_segment;
} _dpmi_seginfo;

typedef __SPACKED__ struct {
  unsigned long total_allocated_bytes_of_physical_memory_host;
  unsigned long total_allocated_bytes_of_virtual_memory_host;
  unsigned long total_available_bytes_of_virtual_memory_host;
  unsigned long total_allocated_bytes_of_virtual_memory_vcpu;
  unsigned long total_available_bytes_of_virtual_memory_vcpu;
  unsigned long total_allocated_bytes_of_virtual_memory_client;
  unsigned long total_available_bytes_of_virtual_memory_client;
  unsigned long total_locked_bytes_of_memory_client;
  unsigned long max_locked_bytes_of_memory_client;
  unsigned long highest_linear_address_available_to_client;
  unsigned long size_in_bytes_of_largest_free_memory_block;
  unsigned long size_of_minimum_allocation_unit_in_bytes;
  unsigned long size_of_allocation_alignment_unit_in_bytes;
  unsigned long reserved[19];
} __dpmi_memory_info;

extern unsigned short __dpmi_error;
extern unsigned short _watcom_ds;

/* returns zero if success, else dpmi error and info->size is max size */
int _dpmi_allocate_dos_memory(_dpmi_seginfo *info);
        /* set size to bytes/16, call, use rm_segment.  Do not
           change anthing but size until the memory is freed.
           If error, max size is returned in size as bytes/16. */
int _dpmi_free_dos_memory(_dpmi_seginfo *info);
        /* set new size to bytes/16, call.  If error, max size
           is returned in size as bytes/16 */
int _dpmi_resize_dos_memory(_dpmi_seginfo *info);
        /* uses pm_selector to free memory */

/* These both use the rm_segment:rm_offset fields only */
int _dpmi_get_real_mode_interrupt_vector(int vector, _dpmi_seginfo *info);
int _dpmi_set_real_mode_interrupt_vector(int vector, _dpmi_seginfo *info);

/* These do NOT wrap the function in pm_offset in an iret handler.
   You must provide an assembler interface yourself, or alloc one below.
   You may NOT longjmp out of an interrupt handler. */
int _dpmi_get_protected_mode_interrupt_vector(int vector, _dpmi_seginfo *info);
        /* puts vector in pm_selector:pm_offset. */
int _dpmi_set_protected_mode_interrupt_vector(int vector, _dpmi_seginfo *info);
        /* sets vector from pm_offset and pm_selector */

/* RMCB functions, automatically restructure the real-mode stack for the
   proper return type and set up correct PM stack.  The callback
   (info->pm_offset) is called as (*pmcb)(__dpmi_regs *regs); */
int _dpmi_allocate_real_mode_callback_retf(_dpmi_seginfo *info,__dpmi_regs *regs);
        /* points callback at pm_offset, returns seg:ofs of callback addr
           in rm_segment:rm_offset.  Do not change any fields until freed.
           Interface is added to simulate far return */
int _dpmi_allocate_real_mode_callback_iret(_dpmi_seginfo *info,__dpmi_regs *regs);
        /* same, but simulates iret */
int _dpmi_free_real_mode_callback(_dpmi_seginfo *info);
        /* frees callback */
int _dpmi_simulate_fcall(__dpmi_regs *regs);
  /* This function simulates a real-mode far call to a function that returns
     with a far return.  The registers are set up from REGS, including `CS'
     and `IP', which indicate the address of the call.  Any registers the
     function modifies are reflected in REGS on return.

     If `SS' and `SP' are both zero, a small temporary stack is used when in
     real mode.  If not, they are used AS IS.  It's a good idea to use
    `memset' to initialize the register structure before using it. */

int  __dpmi_get_memory_information(__dpmi_memory_info *_buffer);                             /* DPMI 1.0 AX=050b */
int  __dpmi_get_real_mode_interrupt_vector (int vector, __dpmi_raddr * raddr);
int  __dpmi_set_real_mode_interrupt_vector (int vector, __dpmi_raddr * raddr);
int __dpmi_int (int int_no,__dpmi_regs *regs);  /* DPMI function AX = 0x0300 */
int dpmi_lock_linear (void *lockaddr, unsigned long locksize);
void *dpmi_map_physical_address (char *physical, int len);
void dpmi_unmap_physical_address (char *physical);
int _dpmi_lock_code ( void *lockaddr, unsigned long locksize);
int _dpmi_lock_data ( void *lockaddr, unsigned long locksize);

#define DPMIINFO_STRUCT  _dpmi_seginfo
#define DPMI_ALLOCATE_DOS_MEMORY(i) _dpmi_allocate_dos_memory(i)
#define DPMI_RESIZE_DOS_MEMORY(i) _dpmi_resize_dos_memory(i)
#define DPMI_FREE_DOS_MEMORY(i) _dpmi_free_dos_memory(i)
#define dosmemget(s,n,p) memcpy(p,(char *)(s),n)
// copy pmode data into transfer buffer
#define DOSBUF_PUT(s, p, n) memcpy((char *)((s.rm_segment<<4)+s.rm_offset),p,n)
// copy data from transfer buffer to pmode
#define DOSBUF_GET(s, p, n) memcpy(p, (char *)((s.rm_segment<<4)+s.rm_offset),n)
// allocate a callback
#define DPMI_ALLOC_RMCB(r,i) _dpmi_allocate_real_mode_callback_retf(r,i)
#define DPMI_FREE_RMCB  _dpmi_free_real_mode_callback

// Real Mode Callbacks
extern char *_pcRmCbStackTop;
extern void ENTER_RMCB(void);
#pragma aux ENTER_RMCB = \
   "mov    ebx,_pcRmCbStackTop"\
   "mov    ax,ds"\
   "mov    ecx,esp"\
   "mov    dx,ss"\
   "mov    ss,ax"\
   "mov    esp,ebx"\
   "push   edx"\
   "push   ecx"\
   modify [eax ebx ecx edx]

#define RMCB_PARAMS() \
   iSocket = cb_regs.x.bx;\
   iEvent  = cb_regs.x.cx;\
   wArgLo  = cb_regs.x.dx;\
   wArgHi  = cb_regs.x.si;\
   wHintHi = cb_regs.x.es;\
   wHintLo = cb_regs.x.di

/*
struct INTPACKX {
   unsigned gs,fs,es,ds,edi,esi,ebp,esp,ebx,edx,ecx,eax,eip,cs,flags;
             0  4  8 12  16  20
};
*/

extern void EXIT_RMCB(void);
#pragma aux EXIT_RMCB = \
   "pop    eax"\
   "pop    ebx"\
   "mov    ss,bx"\
   "mov    esp,eax"\
   "mov    esi,[ebp+20]"\
   "mov    edi,[ebp+16]"\
   "mov    eax,[ebp+8]"\
   "mov    es,ax"\
   "mov    eax,[ebp+12]"\
   "mov    ds,ax"\
   "mov    eax,[esi]"\
   "add    word ptr es:[edi+02eh],4"\
   "mov    es:[edi+02ah],eax"\
   modify [eax ebx ecx edx];
#endif //__WATCOMF__

//
// declare registers for callback
#define DPMI_CB_REGS(r) __dpmi_regs r
extern __dpmi_regs cb_regs;

// define chain of DPMIINFO_STRUCTs for callbacks
typedef __SPACKED__ struct s_info_chain {
   __SPACKED__ struct s_info_chain *psNext;
   DPMIINFO_STRUCT sInfo;
} INFO_STRUCT_CHAIN;

extern INFO_STRUCT_CHAIN *psInfoChainHead;
int _DosBufEnter(int iCount);
void _DosBufSetup(DPMIINFO_STRUCT *psInfo,void *pBuf,int iCount,int iPut);
void _DosBufExit(void);
void _DosBufFree(int iUse);
int _AllocateCapiDosBuffer(int iUse,int iSize);

//
// setup
//
// init structure, alloc buffer and copy data
#define DOSBUF_ENTER(n,e) if (_DosBufEnter(n) < 0) return e
#define DOSBUF_SETUP(s,p,n,w) _DosBufSetup(&s,p,n,w)
//
// keep track of DOS buffer use
#define DOSBUF_EXIT() _DosBufExit()
//
#define DOSBUF_FREE(i) _DosBufFree(i)

#define RMCB_FREE FreeRealModeCallbacks
//
#else
typedef unsigned int PSTRUCT; // internal pointer to structure
//
// no DPMI info required in real-mode
#define DPMIINFO_STRUCT(i)
#define INFO_STRUCT_CHAIN(i)
#define DPMI_INT(i)
//
// not used in real mode
#define DOSBUF_ENTER(n,e)
#define DOSBUF_SETUP(s, p, n, w)
//
// no data to retrieve for real mode
#define DOSBUF_GET(i, p, s)
//
// no xfer buffer to keep track of
#define DOSBUF_EXIT()
#define DOSBUF_FREE(i)
#define RMCB_FREE()
// no memory to free in real mode
#define DPMIINFO_FREE()

//
// define pointer for callback info
#define DPMI_CB_INFO(i) void D_FAR *(i)
//
// no callback allocation is necessary
#define DPMI_CB_ALLOCRETF(i, p, r)
//
// no callback regs are necessary
#define DPMI_CB_REGS(r)

#endif

/*
   The compatibility API is called called by setting up
   the CPU registers and memory structures and issueing
   an interrupt 0x61 (or appropriate interrupt)  The
   following macros provide a semi-portable means of
   embedding assembly code in C code.
*/

/* Microsoft C uses _asm and cflag */
#ifdef _MSC_VER
   // disable warnings for FP_OFF,FP_SEG,REALMODE_OFFSET,REALMODE_SEGMENT
   #pragma warning (disable : 4759 4769)
   #ifdef _ASM
      #undef _ASM
   #endif
   #define _ASM _asm
   // BorlandC compatible FP_SEG and FP_OFF
   #define FP_Seg(fp)   ((unsigned)(__segment)(void __far *)(fp))
   #define FP_Off(fp)   ((unsigned)(fp))
#else
   #define FP_Seg(fp)   FP_SEG(fp)
   #define FP_Off(fp)   FP_OFF(fp)
#endif

// Temporary for debugging - remove !!!
//#undef FP_SEG
//#undef FP_OFF

/* Borland C uses asm and flags*/
#ifdef __TURBOC__
 #ifdef _ASM
  #undef _ASM
 #endif
 #define _ASM asm
 /* Undo the floating point definition of CLK_TCK */
 #ifdef CLK_TCK
  #undef CLK_TCK
 #endif
 #define CLK_TCK 1000
#endif

#ifdef __DJGPP__
#define _ASM __asm__
#endif

#if defined(__WATCOMC__)
  #define _ASM __asm
#endif

/*
   These are required for Borland and Microsoft C, other
   compilers may or may not use them.  The following
   macros must be defined appropriately.
*/
#if !defined(_ASM) && defined(DOS)
 #error _ASM not defined.  Required for _CAPI
#endif

/*
   MSDELAY must be defined to sleep or delay execution
   for the given number of milliseconds.  Note this is
   only used for testing purposes.
*/
#ifdef WIN32
 #define MSDELAY(x) sleep((x))
#else
 #define MSDELAY(x) delay((x))
#endif

/*
   The interface to the Sockets driver is performed via
   a software interrupt with appropriate registers
   set to indicate the function, various parameters,
   and to pass pointers for data transfer.
*/
#ifdef _SOCK32
typedef struct _tagX86Regs
{
   union {
      struct {
         unsigned long di;
         unsigned long si;
         unsigned long bp;
         unsigned long es;
         unsigned long bx;
         unsigned long dx;
         unsigned long cx;
         unsigned long ax;
         unsigned long flags;
      } x;
      struct {
         unsigned short di, di_hi;
         unsigned short si, si_hi;
         unsigned short bp, bp_hi;
         unsigned short res, res_hi;
         unsigned short bx, bx_hi;
         unsigned short dx, dx_hi;
         unsigned short cx, cx_hi;
         unsigned short ax, ax_hi;
         unsigned short flags;
         unsigned short es;
         unsigned short ds;
         unsigned short fs;
         unsigned short gs;
         unsigned short ip;
         unsigned short cs;
         unsigned short sp;
         unsigned short ss;
      } w;
      struct {
         unsigned char edi[4];
         unsigned char esi[4];
         unsigned char ebp[4];
         unsigned char res[4];
         unsigned char bl, bh, ebx_b2, ebx_b3;
         unsigned char dl, dh, edx_b2, edx_b3;
         unsigned char cl, ch, ecx_b2, ecx_b3;
         unsigned char al, ah, eax_b2, eax_b3;
      } h;
   } r;
} X86Regs;
#elif defined(_SOCKLIB)
typedef struct _tagX86Regs
{
   union {
      struct {
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
      } x;
      struct {
         unsigned char es[2];
         unsigned char dl,dh;
         unsigned char cl,ch;
         unsigned char bl,bh;
         unsigned char al,ah;
         unsigned char di[2];
         unsigned char si[2];
         unsigned char ds[2];
         unsigned char flags[2];
         unsigned char sp[2];
         unsigned char ss[2];
      } h;
   } r;
} X86Regs;
#define _s r.x
#elif defined (__DJGPP__) || defined (__WATCOMF__)
typedef __SPACKED__ struct _tagX86Regs
{
   __dpmi_regs r;
} X86Regs;
#define _s r.x
#elif defined (__WIN32__)
typedef struct _tagX86Regs
{
   int r;   // dummy
} X86Regs;
#define _s
#else
typedef __SPACKED__ struct _tagX86Regs
{
   union REGS  r;
   __SPACKED__ struct SREGS _s;
} X86Regs;
#endif

/* Global API calls used within the Sockets Compatible API */
int CallSocketsDosApi(X86Regs *px86regs);

/* macro to get the current timer ticks from BDA */
#ifdef __DJGPP__
#include <sys/farptr.h>
#define GET_TICKS(x) _farpeekw(_dos_ds, 0x46c)
#else
#define GET_TICKS(x) *((WORD D_FAR *)0x46c)
#endif

/* macro to declare the DPMI sections to lock */
#ifdef __DJGPP__
#define DPMI_SECTION_EXTERNS \
   extern char stext _ASM("stext"); \
   extern char etext _ASM("etext"); \
   extern char sdata _ASM("sdata"); \
   extern char edata _ASM("edata"); \
   extern char sbss  _ASM("sbss"); \
   extern char ebss  _ASM("ebss")

#define DPMI_LOCK_SECTIONS \
   _go32_dpmi_lock_code(&stext, (DWORD)&etext - (DWORD)&stext); \
   _go32_dpmi_lock_data(&sdata, (DWORD)&edata - (DWORD)&sdata); \
   _go32_dpmi_lock_data(&sbss,  (DWORD)&ebss  - (DWORD)&sbss)

#else
#define DPMI_SECTION_EXTERNS
#define DPMI_LOCK_SECTIONS
#endif

#ifdef DEBUG
//
// macro to write a value to a debug port
//
#ifdef __DJGPP__
#define DebugWritePort(port, val) ({ \
   _ASM( \
      "pushw   %%ax\n\t" \
      "pushw   %%dx\n\t" \
      "movb    %1, %%al\n\t" \
      "movw    %0, %%dx \n\t" \
      "outb    %%al, %%dx \n\t" \
      "popw    %%dx\n\t" \
      "popw    %%ax" \
      : \
      : "g" (port), "g" (val) \
      : "ax" , "dx", "memory"); \
   })
#else
#define DebugWritePort(port, val) ({ \
   _ASM  push  ax \
   _ASM  push  dx \
   _ASM  mov   al, val \
   _ASM  mov   dx, port \
   _ASM  out   dx, al \
   _ASM  pop   dx \
   _asm  pop   ax \
   })
#endif
#else
#define DebugWritePort(port, val)
#endif

#ifdef __cplusplus
};
#endif

#endif //_COMPILER_H

