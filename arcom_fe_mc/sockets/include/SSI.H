//  $Id: SSI.H 1.7 2005/01/13 17:06:06Z johnb Exp $
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
   $DOCFILE:SSI.H

   Copyright (C) 1999-2001 Datalight, Inc.
   All Rights Reserved

   Server Side Include (SSI) functions header file
   for Sockets HTTP server

   $DOCHISTORY:

   2001-05-11  GPvN cleanup
   2000-05-18  GVvN first version
 */



/*
 * initialize ssi adapter: hook HTTP server
 * pszCgiName - The name on which to react, eg. cgi-bin/getpage.exe
 */
int CgiInit(char *pszCgiName);

/*
 * Unhook HTTP server. Function MUST be called before exiting, otherwise
 * system will crash.
 */
void CgiQuit(void);

/*
 * Add a variable and its value to the specified parameter set.
 */
int AddVar(int iHandle, char *pszName, char* pszValue);

/*
 * Set the filename of the specified handle
 */
int SetFile(int iHandle, char *pszFilename);


/*
 * the only function the user of the ssi interface must implement
 *
 * This function will be called each time a new request arrives.
 * pszQuery will hold the query.
 *
 * A user must use the 'SetFile' and 'AddVar' functions to set the
 * template file and add ssi variables. The provided iHandle should
 * be used when calling these functions
 */
void SetVariables(char *pszQuery, int iHandle);


