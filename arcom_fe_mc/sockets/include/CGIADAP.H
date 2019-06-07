//  $Id: CGIADAP.H 1.11 2009/01/15 23:39:32Z gerritvn Exp $
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
   $DOCFILE:CGIADAP.H

   HTTP CGI Adapter interface header

   $DOCHISTORY:

   2001-07-17  gvn   added routines to get/set stack segment.
   2001-05-11  GPvN  cleanup
   2000-05-18  GVvN  first version
 */


//contains all the definitions for return values and error messages
#include "compiler.h"
#include "api.h"
#include "htapic.h"
#include "xxtea.h"

/*
adapter for cgi interfacing with HTTPD server
*/
/*
int HttpRegister(char far *szPath,
 int (far *pfCallback)(HTTP_PARAMS far *psHttpParams),
 int iFlags, DWORD dwUserID);

Arguments:
szPath:
   The string identifing a url. It should be an exact match of the
   abs_path part of the URI minus the leading '/'. For instance, If you want
   to capture all http://myserver.com/cgi-bin/getpage.exe, you shoud
   register 'cgi-bin/getpage.exe'.

pfCallback:
The callback is guaranteed to only be called when DOS can be called.
The DOS critical handler will be disabled and all critical errors will
result in an access error without any user intervention.
Since the callback happens at interrupt time, it should execute for
as short a time as possible. After a done or error return, no further
callbacks will be generated for the current request.

   psHttpParams:
   psHttpParams->iReason - reason for callback:
      R_NEWREQ  -    New HTTP request. pszCommandLine points to the command line passed
         in the URL. The number contained in iValue specifies the HTTP
         operation; 0 for GET and 1 for POST.
      R_INDATA  -    Input data available, iValue contains count.
      R_OUTDATA -    Can send output data, iValue contains count.
      R_ENDDATA -    Peer closed connection ie "end of input data"
      R_CLOSED  -    Connection closed.

   psHttpParams->iHandle
      HTTPD handle, used in subsequent API calls for this request.
      The user shouldnt modify it

   return values:
      RET_OK   - not done, give me more upcalls
      RET_DONE - done, no more upcalls please
      RET_ERR  - done, error

Only one callback will be active at any time.
Calling an API function while executing the callback function,
will not result in another callback before the current callback has returned.

iFlags:

F_OVERLAP -    overlapped requests (1), non-overlapped requests (0).
All other bits are reserved.

return:  OK (0) - ok
         < 0: One of the error messages (see htapic.h)

dwUserID - value passed to HttpRegister();  this value is for use by the
   extension, HTTPD does not modify it.

Remarks:
*/
int HttpRegister(char far *szPath,
 int (far *pfCallback)(HTTP_PARAMS far *psHttpParams),
 int iFlags, unsigned long dwUserID);

/*
int HttpDeRegister(char *pszPath);
return:  OK (0) - ok
         < 0: One of the error messages (see htapic.h)


Remarks:
After this call no more callbacks will be generated for this URL.
Any requests in progress will be terminated with an error to the peer.
This API must be called for all registrations made by a program before
terminating that program, otherwise the system will inevitably crash on
any subsequent request.
*/
int HttpDeRegister(char far *pszPath);


/*
int HttpGetData(int iHandle, char far *pcBuf, int iCount);

Remarks:
This API can be called when a POST operation has been indicated by the
callback. If more data is expected and the extension is busy executing
the callback function, a 0 return should be made from the callback
indicating it is still busy and getting more data should be
attempted at the next callback.


return:  >= 0 - ok, bytes received
         < 0: One of the error messages (see htapic.h)

*/
int HttpGetData(int iHandle, char far *pcBuf, int iCount);

/*
Remarks:
This API can be called to send data to the peer.
If the return indicates that less than the requested number of bytes has
been sent and the extension is busy executing the callback function,
a 0 return should be made from the callback indicating it is still busy and
an attempt to send more data should be made at the next callback.

All the required data should be sent to the peer before the HttpSubmitFile()
function is used. After HttpSubmitFile(), HttpSendData() should not be
called again.

return:  >= 0 number of bytes actually sent
   < 0 error

*/
int HttpSendData(int iHandle, char far *pcBuf, int iCount);

/*
Remarks:
The extension can submit a file to be sent to the peer in response
to a request. The file will be logically appended to any data already
sent using HttpSendData(). The file should not be exclusively opened
when it is submitted. After it is transmitted, transmit upcalls will be
issued normally. This gives the user the ability to send any number of
files on the connection with arbritary data in between.
return:     0  OK
   < 0   error

*/
int HttpSubmitFile(int iHandle, char far *pszFileName);


/*
 * Get the version number of the server
 */
int HttpGetVersion(void);

/*
 * Get the status of the HTTP server.
 * Also, if the server is running in passive mode, give the server time to
 * process any qued events
 *
 * Return: number of active connections on server
 */
int HttpGetStatus(void);

/*
 * Refresh internal Username/Password list without unloading
 * after updating SOCKET.UPW
 */

int HttpRefreshPw(void);

/*
 * TAKE NOTE - STACK POINTER:
 *
 * The stack pointer for callbacks is by default set to _SP - 1000, the first
 * time the HTTP API is called. If you
 * would need space on the stack, or for some reason want to make it tighter,
 * set the stack pointer for callbacks manually. Be careful not to overwrite
 * used memory.
 */
int GetStackPointer(void);
void SetStackPointer(int iPointer);
int GetStackSegment(void);
void SetStackSegment(int iSegment);

