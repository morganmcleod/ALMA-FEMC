/*
** This file is placed into the public domain on February 22, 1996,  by
** its author: Carey Bloodworth
**
** Modifications:
**
** 07-Dec-1999 by
** Bob Stout & Jon Guthrie
**  General cleanup, use NUL (in SNIPTYPE.H) instead of NULL where appropriate.
**  Allow spaces in tag names.
**  Allow strings in quotes.
**  Allow trailing comments.
**  Allow embedded comment separator(s) in quoted strings.
**  Speed up line processing.
**  ReadCfg() returns an error if section or variable not found.
**  Changed integer type to short.
**  Use cant() calls in lieu of fopen() calls,
**    include ERRORS.H for prototype.
*/

#ifndef INI_H_
#define INI_H_

#define NUL '\0'
#define LAST_CHAR(s) (((char *)s)[strlen(s) - 1])

#define INI_LINESIZE 128
#define BUFFERSIZE (INI_LINESIZE + 2)

typedef enum {Error_ = -1, Success_, False_ = 0, True_} Boolean_T;

enum CfgTypes {Cfg_String,
               Cfg_Byte,
               Cfg_Ushort,
               Cfg_Short,
               Cfg_Ulong,
               Cfg_Long,
               Cfg_Float,
               Cfg_Double,
               Cfg_Boolean, /* Boolean is actually an integer Y/N T/F */
               Cfg_HB_Array,
               Cfg_I_Array,
               Cfg_F_Array,
               Cfg_D_Array
              };

typedef struct CfgStruct {
      char *Name;
      void *DataPtr;
      enum CfgTypes VarType;
} CFG_STRUCT;

extern int ReadCfg(const char *FileName,
                   char *SectionName,
                   CFG_STRUCT *MyVars);

extern int SearchCfg(const char *FileName,
                     char *SectionName,
                     char *VarName,
                     void *DataPtr,
                     enum CfgTypes VarType);

extern int UpdateCfg(const char *FileName,
                     char *SectionName,
                     char *VarWanted,
                     char *NewData);

#endif
