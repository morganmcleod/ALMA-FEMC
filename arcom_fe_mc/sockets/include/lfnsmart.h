// LFNSMART.H

#ifndef NODLLIB
#include "dostruct.h"   //  DOS structures
#undef min
#include "dosdef.h"
#include "datalght.h"
#include "dllfn.h"      //  LFN and LFN "smart" functions
#define ff_attrib attr
#define ff_name  name
#define ff_fdate  date
#define ff_ftime  time
#define ff_fsize  size
#else //DLLIB
#ifdef __DJGPP__
#include <unistd.h>
#define SmartMakeDirectory(p) !mkdir(p,S_IWUSR)
#if __DJGPP_MINOR__ > 3
#define SmartExpandPath(c,f,e) realpath(f,e)
#else
#define SmartExpandPath(c,f,e) (_fixpath(f,e),1)
#endif
#define SmartGetFileAttributes(p,pa) ((*pa = _chmod(p,0,0)) == -1 ? 0 : 1)
#else //__DJGPP__
#define SmartMakeDirectory(p) !mkdir(p)
#define SmartExpandPath(c,f,e) strcpy(e,f)
#ifndef _OSE
#define SmartGetFileAttributes(p,pa) ((*pa = _rtl_chmod(p,0,0)) == (unsigned)-1 ? 0 : 1)
//#define S_IWUSR S_IWRITE
#else //_OSE
int SmartGetFileAttributes(char *pszPath,unsigned *pAttrib);
#endif //_OSE
#endif //__DJGPP__
#include <sys/stat.h>
#ifndef _OSE
#include <io.h>
#endif
#include <fcntl.h>
typedef struct ffblk * PFIND;
typedef struct ffblk FIND;
typedef int bool;
/* FINDFIRST attributes */
#define A_NORMAL     0x00
#define A_READONLY   0x01
#define A_HIDDEN     0x02
#define A_SYSTEM     0x04
#define A_VOLUME     0x08
#define A_SUBDIR     0x10
#define A_ARCHIVE    0x20
#define A_DEVICE     0x40
#define A_ALLFILES   0x06  /* hidden and system */
#define ALL_ATTR     0x16
#define SmartFindFirst(atr, path, ps) !findfirst(path,ps,atr)
#define SmartFindNext(ps) !findnext(ps)
#define SmartFindClose(ps)
#define SmartRemoveDirectory(p) !rmdir(p)
#define SmartRenameFileOrDirectory(o,n) !rename(o,n)
#define SmartChangeDirectory(n) !chdir(n)
#define SmartGetCurrentDirectory(d,n) getcwd(n,260)
#define LFNfopen(p,m) fopen(p,m)
#define A_ALLDIR (FA_RDONLY|FA_HIDDEN|FA_SYSTEM|FA_DIREC)
#define LFN_MAX_PATH 240
#endif //NODLLIB

