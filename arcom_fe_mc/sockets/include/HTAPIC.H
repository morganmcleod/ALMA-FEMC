//  $Id: HTAPIC.H 1.18 2009/01/29 18:21:54Z gerritvn Exp $
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
   $DOCFILE:HTAPIC.H

   HTTP API header file

   $DOCHISTORY:

   2005-05-19  gvn  added RefreshPassword
   2005-04-02	gvn  added R_EXIT code
	2003-11-29  gvn  implement RET_GET
   2003-05-19  gvn  add status returns
   2001-05-11  GPvN cleanup
   2000-05-18  GVvN first version
 */
#include "compiler.h"

#ifndef TCBH
#if defined(KERNEL) || defined(_SOCK32)
#define TCBH struct tcb *
#else
#define TCBH unsigned
#endif   //#if defined(KERNEL) || defined(_SOCK32)
#endif //TCBH

#ifdef KERNEL
#define MAX_DATA  2000
#else
#define MAX_DATA  512
#endif
//api functions (set in AH when api called)
#define APIF_REGISTER   0
#define APIF_DEREGISTER 1
#define APIF_GETDATA    2
#define APIF_SENDDATA   3
#define APIF_SENDFILE   4
#define APIF_GETVERSION 5
#define APIF_GETSTATUS  6
#define APIF_REFRESHPW  7

// flags
#define F_OVERLAP    1  //indicates multiple requests at a time
#define F_ENV        2  //indicates environment headers will be provided with upcalls

/* Error return code
   Note that the NEGATED value will be returned on error
*/
#define NONE         0  /* No error */
#define NO_ERR       0  /* No error */
#define OK           0  /* No error */
#define CON_EXISTS   1  /* Connection already exists */
#define NO_CONN      2  /* Connection does not exist */
#define CON_CLOS     3  /* Connection closing */
#define NO_SPACE     4  /* No memory */
#define WOULDBLK     5  /* Would block */
#define NOPROTO      6  /* Protocol or mode not supported */
#define INVALID      7  /* Invalid arguments */
#define BUFSHORT     8  /* Buffer too short for data */
#define BADFILENAME  9  /* cant open file */
#define NO_USER      11 /* User doesnt exist */
#define ERR_DUPL     10 /* duplicate name (name already exists) */
#define RE_ENTER     27 /* Re-entry of Inet */
#define NO_API       50 /* API not loaded or invalid api call */


// http request types
#define RQ_NONE   0     // unknown
#define RQ_GET    1     // get
#define RQ_POST   2     // post
#define RQ_PUT    3     // put
#define RQ_CGI    4     // cgi
#define RQ_REMCON 5     // remote console
#define RQ_XXTEA  6     // only allow encrypted

// callback:
// parameters
typedef struct HTTP_PARAMS_S
{
   int iReason;
   int iHandle;      //handle for request
   long dwUserID;    //user defined ID (set at register)
   long dwSessionID; //per connection ID, set at any upcall by user
   int iValue;       //Request type for new request, else data count
   long lContentLen;
   char D_FAR *szQuery;
   char D_FAR* D_FAR* ppEnv;//the environment (headers, ip address, etc)
   int iEnvSize;     //number of entries in environment
} HTTP_PARAMS;

// function
// int (D_FAR *pfCallback)(HTTP_PARAMS D_FAR *psHttpParams);

// return values:
#define RET_OK    0  //give me more upcalls
#define RET_DONE  1  //done, dont want any more data
#define RET_GET   2  //done CGI function, handle as normal GET or No-content for POST
#define RET_ERR   -1 //I experienced an error

// status return bits
#define RETS_SENDING    0x100
#define RETS_RECEIVING  0x200

// reasons (iReason one of)
#define R_NEWREQ  0  //new request
#define R_INDATA  1  //may read data
#define R_OUTDATA 2  //may transmit data
#define R_ENDDATA 3  //end of stream reached
#define R_CLOSED  4  //connection closed
#define R_EXIT		5	//server is exiting

//cgi
typedef struct cgiUser {
   char *szName;
   int (D_FAR *pfFunc)(HTTP_PARAMS D_FAR *psHttpParams);
   char cInuse;
   int iFlags;
   DWORD dwUserID;

   struct cgiUser* psNext;
} CGI_USER;


#define HTTP_LINELEN   128

// Connection Parameters
#ifndef XXTEA
typedef struct s_ConPar
{
   TCBH hTcb;           // TCP handle
} CON_PAR;
#else //XXTEA
typedef struct s_ConPar
{
   TCBH hTcb;           // TCP handle
   int iRecvCount;      // receive count
   WORD wBlockNumber;   // Block number for writing XXTEA blocks
   WORD wReadBlockNumber;// Block number for reading XXTEA blocks
   WORD wMaxData;       // maximum data length to send to peer
   char cEncryptFlag;
#define BLOCK_FLAG   1
#define ENCRYPT_FLAG 2
} CON_PAR;
#endif //XXTEA

// HTTP control block

typedef struct https {
   struct https *prev;
   struct https *next;
   unsigned id;
   int state;           // state of connection
#define REQUEST   0
#define HEADER    1
#define PWRITE    2     // put or post writing
#define NEWCGI    3
   int request;         // request type
#define RQ_NONE   0     // unknown
#define RQ_GET    1     // get
#define RQ_POST   2     // post
#define RQ_PUT    3     // put

   char flags;
#define HTTP_RECEIVE          1     //data available at rinet to be read
#define HTTP_SEND             2     //data available at us to be send
#define HTTP_SENDFILE         4     //we can start sending file
#define GOT_RECEIVE_UPCALL    8     //we got a receive upcall, may read data at stable time
#define GOT_TRANSMIT_UPCALL   0x10  //we got a tranmsit upcall, may send data at stable time
#define GOT_STCH_UPCALL       0x20  //we got a state change upcall, may handle at stable time
#define USE_LOCAL_COPY        0x40  // use local copy flag
#define IS_CGI                0x80  // this is a CGI request
   CON_PAR sConPar;
   #define tcbo sConPar.hTcb
   char buf[HTTP_LINELEN];          // line buffer
   int cnt;             // count of characters in buffer
   char *rqline;        // request line for CGI
   char *pQuery;        // query string for CGI
   char **pCGIEnv;      // space for CGI environment variables
    int countEnv;       // count of environment variables
   FILE *fp;            // file being sent (or received)
   char *pszFilename;   // hold the file name for as short as possible
   char *type;          // type of file
   time_t if_modified_time;
   unsigned long if_modified_length;
   long content_len;
   unsigned tcount;     //the amount we may transmit
   int rcount;          //the amount data available
   int iNameIn;         // number to form stdin name from
   int iNameOut;        // number to form stdout name from
   char *szUser;        // username (if supplied)
   char *szPass;        // password (if supplied)

   char D_FAR* D_FAR* ppEnv;//the environment (headers, ip address, etc)
   int iEnvSize;        //the size of the environment

   //api vars
   CGI_USER *pCgiUser;  //pointer to cgi user
   int iDataRead;       //amount of data cgi user read
   long dwSessionID;    //per-session ID
} HTTP;


