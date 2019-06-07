//  $Id: webform.h 1.5 2009/01/19 21:07:38Z gerritvn Exp $
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
/*
   $DOCHISTORY:

   2003-11-10  gvn first version
*/

int far Callback(HTTP_PARAMS far* psParams);

#define BUF_SIZE  200

typedef struct s_field {
   BYTE bType;          // type of field
   union {
      WORD wId;
      char *pszText;
   } u;
} FIELD;

enum {   // Types of field
   TYPE_END,
   TYPE_NOCONTENT,
   TYPE_TEXT,
   TYPE_VARTEXT,
   TYPE_REPTEXT,
   TYPE_IFTEXT,
};

typedef struct sFcb {   // Form control block
   int iState;          // state
   int iRequest;        // type of HTTP request
//   int iFlags;          // flags
//#define CAN_READ  1     // read received while sending
   int (*pGetData)(struct sFcb *psFcb,WORD wId,char *pszData);
   int (*pPutData)(struct sFcb *psFcb,int iCount);
   char *pszFilename;   // template file name
   FILE *psFile;        // template file
   FIELD *psFormStart;  // start of form
   FIELD *psFormBegin;  // begin of allocated form
   FIELD *psField;      // pointer into form
   FIELD *psRepField;   // pointer to repeat field
   char *pszUrl;        // pointer to URL
   int iHandle;         // HTTP handle
   int iCount;          // count of bytes to be sent
   int iOffset;         // offset into buffer
   int iAvailable;      // available data in buffer
   char *pcBuf;         // start of buffer
   char acEncHeader[BHEADER_SIZE];
   char acBuf[BUF_SIZE + 4];// buffer for variable text
   void *pUser;         // user control block
} FORMCB;

enum {
   S_IDLE,
   S_SENDING,
   S_RECVPOST,
   S_DONE
};

#define WFID(a,b) a+b*256  // WebForm ID

extern BYTE bWebFormTrace;     // trace flags
#define TRACE_REQ    1
#define TRACE_OUT    2
#define TRACE_IN     4

FORMCB *InitForm(void *pUser,FIELD *psFormStart,char *pszUrl,
   int (*pGetData)(struct sFcb *psFcb,WORD wId,char *pszData),
   int (*pPutData)(struct sFcb *psFcb,int iCount),char *pszFilename);
void FreeForm(FORMCB *psFormCb);
int SetForm(FORMCB *psFcb, FIELD *psForm);
int SetFile(FORMCB *psFcb,char *pszFilename);

#ifdef TEST
void NewRequest(FORMCB *psFcb);
void SendRequest(void);
#endif

