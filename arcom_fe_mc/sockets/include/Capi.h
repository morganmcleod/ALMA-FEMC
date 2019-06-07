//  $Id: Capi.h 1.31 2012/07/18 23:55:00Z johnb Exp $
//
//  *** DO NOT REMOVE THE FOLLOWING NOTICE ***
//
//  Copyright (c) 1999-2007 Datalight, Inc.
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
   $DOCFILE:CAPI.H

   Copyright (c) GP van Niekerk Ondernemings 1990-2007

   Sockets/RINET Compatible API (CAPI) header file

   $DOCHISTORY:
05/26/2008 gvn documentation changes
01/17/2006 gvn CONFIG_COMMAND added
09/19/2005 gvn STPPP_IPCP added
11/19/2004 gvn Extended functions for IPv4/6, fixes for C++
07/28/2004 jmb K_INF_DOMAIN, K_INF_DNS_CMPS
11/17/2003 gvn APIs for SNMP
09/11/2003 gvn K_INF_MASTER_TICK
10/01/2002 gvn ARP_API
09/16/2002 gvn IOCTL_HOTSWAP
10/03/2001 jmb DJGPP support: added attribute to structures to prevent
              addition of spaces to make access faster.
2001-07-24 gvn added ERR_NETWORK
2001-05-11     used ISR_ROUTINE after cleanup.
2001-04-19 jmb changed ISR_ROUTINE to _loadds
   2.11 2001-03-30     IOCTL PPP OPEN,START,CLOSE
   2.10 2000-12-09 pkg Factored out non portable code
   2.01 2000-06-27     Adding MSVC compatibilaty.
   2.02 2000-11-29 pkg Began makeing changes for portability
   2.00 2000-05-24     IOCTL
   1.20 1999-12-04     Added IGMP
   1.10 1999-07-02     Sockets 1.1 release
   1.02 1999-05-27     New version using Datalight Coding guidelines
   1.01 1999-04-29     Added documentation on async callbacks.
   1.00 1999-03-18     Starting version control
*/

/*
   Function numbers (AH) used to call into the Socekts API
*/
#define GET_BUSY_FLAG               0x01
#define GET_KERNEL_INFO             0x02
#define GET_ADDRESS                 0x05
#define GET_NET_INFO                0x06
#define CONVERT_DC_SOCKET           0x07
#define RELEASE_SOCKET              0x08
#define RELEASE_DC_SOCKETS          0x09
#define IS_SOCKET                   0x0d
#define SELECT_SOCKET               0x0e
#define GET_NET_VERSION             0x0f
#define SHUT_DOWN_NET               0x10
#define DISABLE_ASYNC_NOTIFICATION  0x11
#define ENABLE_ASYNC_NOTIFICATION   0x12
#define CONNECT_SOCKET              0x13
#define GET_PEER_ADDRESS            0x16
#define EOF_SOCKET                  0x18
#define ABORT_SOCKET                0x19
#define WRITE_SOCKET                0x1a
#define READ_SOCKET                 0x1b
#define WRITE_TO_SOCKET             0x1c
#define READ_FROM_SOCKET            0x1d
#define FLUSH_SOCKET                0x1e
#define SET_ASYNC_NOTIFICATION      0x1f
#define SET_OPTION                  0x20
#define GET_DC_SOCKET               0x22
#define LISTEN_SOCKET               0x23
#define ABORT_DC_SOCKETS            0x24
#define GET_SOCKET                  0x29
#define GET_KERNEL_CONFIG           0x2a
#define SET_ALARM                   0x2b
#define ICMP_PING                   0x30
#define PARSE_ADDRESS               0x50
#define LOOKUP_HOST_TABLE           0x51
#define RESOLVE_NAME                0x54
#define JOIN_GROUP                  0x60
#define LEAVE_GROUP                 0x61
#define IFACE_IOCTL                 0x62
#define GET_SOCKETS_VERSION         0x63
#define ARP_API                     0x64
#define ARP_API_ADD                 0
#define ARP_API_DROP                1
#define ARP_API_GETTABLE            2
#define ARP_API_ADDT                3     // add with timer
#define LISTEN_ACCEPT_SOCKET        0x65
#define ACCEPT_SOCKET               0x66
#define PROPRIETARY_API             0x67
#define ROUTE_ADD                   0x68  // add a route
#define ROUTE_CHANGE                0x69  // change a route
#define ROUTE_DROP                  0x6a  // drop a route
#define GET_ADDRESS_EX              0x6b  // get extended (IPv4/6) address
#define ICMP_PING_EX                0x6c  // ping extended (IPv4/6)
#define GET_ADDR_INFO               0x6d  // get IPv4/6 address info
#define GET_PEER_ADDRESS_EX         0x6e  // get IPv4/6 peer address info
#define CONFIG_COMMAND              0x6f  // do configuration command - non TSR versions only

// Flag bits for GET_ADDR_INFO
#define AI_PARSE                    1     // parse numeric address
#define AI_HOSTTAB                  2     // use host table
#define DNS_IPV4                    4     // use IPv4 (record type A) DNS lookup
#define DNS_IPV6                    8     // use IPv6 (record type AAAA) DNS lookup
#define DNS_IPV6_FIRST              0x10  // try IPv6 DNS lookup first
#define AI_NOBLOCK                  0x20  // don't block on DNS lookup
#define AI_ABORT                    0x40  // abort non-blocking request

/* IOCTL functions */
#define IOCTL_CONNECT               0   /* start dial on ASY */
#define IOCTL_DISCONNECT            1   /* drop modem on ASY */
#define IOCTL_ENABLEPORT            2   /* enable port on ASY */
#define IOCTL_DISABLEPORT           3   /* disable port on ASY */
#define IOCTL_ENABLEDOD             4   /* enable-dial-on-demand */
#define IOCTL_DISABLEDOD            5   /* disable dial-on-demand */
#define IOCTL_GETSTATUS             6   /* get dial status */
#define IOCTL_PPP_OPEN              7   /* Open PPP */
#define IOCTL_PPP_START             8   /* Start PPP */
#define IOCTL_PPP_CLOSE             9   /* Close PPP */
#define IOCTL_HOTSWAP               10 // re-read MAC address after hotswap

/* Status bits returned on IOCTL_GETSTATUS */
#define ST_DTR                      0x01   /* Modem Data Terminal Ready */
#define ST_RTS                      0x02   /* Request To Send */
#define ST_CTS                      0x04   /* Clear To Send */
#define ST_DSR                      0x08   /* Data Set Ready */
#define ST_RI                       0x10   /* Ring Indicator */
#define ST_DCD                      0x20   /* Data Carrier Detect */
#define ST_CONNECTED                0x40   /* Modem is connected */
#define ST_MODEMSTATE               0x700  /* Modem state mask */
#define STM_NONE                    0x000  /* No modem on port */
#define STM_IDLE                    0x100  /* Modem is idle */
#define STM_INITIALIZING            0x200  /* Modem is initializing */
#define STM_DIALING                 0x300  /* Modem is dialing */
#define STM_CONNECTING              0x400  /* Modem is connecting */
#define STM_ANSWERING               0x500  /* Modem is answering */
#define STPPPP_IN                   0x800  /* PPP incoming call */
#define STPPP_STATE                 0x7000 /* PPP state */
#define STPPP_DEAD                  0x0000 /* PPP dead */
#define STPPP_LCP                   0x1000 /* PPP LCP state */
#define STPPP_AP                    0x2000 /* PPP Authentication state */
#define STPPP_READY                 0x3000 /* PPP Ready */
#define STPPP_TERMINATING           0x4000 /* PPP Terminating */
#define STPPP_IPCP                  0x5000 /* PPP IPCP (IPV6CP) state */


/*
   If an error occurs, the Carry Flag is set and an error code is
   returned in the AX register as follows:
*/
#define NO_ERR                      0   /* No error */
#define ERR_IN_USE                  1   /* A connection alreadyexists */
#define ERR_DOS                     2   /* A DOS error occured */
#define ERR_NO_MEM                  3   /* No memory to perform function */
#define ERR_NOT_NET_CONN            4   /* Connection does not exist */
#define ERR_ILLEGAL_OP              5   /* Protocol or mode not supported */
#define ERR_NO_HOST                 7   /* No host address specified */
#define ERR_NET_UNREACHABLE         9   // ICMP network unreachable
#define ERR_HOST_UNREACHABLE        10  // ICMP host unreachable
#define ERR_PROT_UNREACHABLE        11  // ICMP protocol unreachable
#define ERR_PORT_UNREACHABLE        12  // ICMP port unreachable
#define ERR_TIMEOUT                 13  /* The function timed out */
#define ERR_HOST_UNKNOWN            14  /* Unknown host has been specified */
#define ERR_BAD_ARG                 18  /* Bad arguments */
#define ERR_EOF                     19  /* Connection closed by peer */
#define ERR_RESET                   20  /* Connection reset by peer */
#define ERR_WOULD_BLOCK             21  /* Operation would block */
#define ERR_UNBOUND                 22  /* Descriptor not yet assigned */
#define ERR_NO_SOCKET               23  /* No descriptor is available */
#define ERR_BAD_SYS_CALL            24  /* Bad parameter in call */
#define ERR_NOT_ESTAB               26  /* Connection not yet established */
#define ERR_RE_ENTRY                27  /* Kernel in use, try again later */
#define ERR_NETWORK                 28  /* Network error eg ICMP message */
#define ERR_TERMINATING             29  /* Kernel unloading */
#define ERR_INF_LOCKED              30  /* Kernel info locked */
#define ERR_BAD_INTERFACE           31  /* Bad interface specified */
#define ERR_API_NOT_LOADED          50  /* API is not loaded */

/* connection types for AcceptSocket(), ConnectSocket() and ListenSocket() */
#define DATA_GRAM       3
#define STREAM          4
#define TYPE_EXT        0x2000          /* Value to add for IPv4/6 operation */

/* Include IP address definition */
#ifndef __IPADDR_H
#include "ipaddr.h"
#endif

/* IP version flags */

#define IPVF_IPV4       1              // IPv4
#define IPVF_IPV6       2              // IPv6

/* Network address structure for NetConnect() and NetListen() */
typedef __SPACKED__ struct NET_ADDR
{
   DWORD dwRemoteHost   __ATTR__;  /* IP address of remote host / length of IP address */
   WORD wRemotePort     __ATTR__;  /* Remote port address */
   WORD wLocalPort      __ATTR__;  /* Local port address */
   BYTE bProtocol               ;  /* Protocol */
   IPAD sIpAddr         __ATTR__;  // IP v6 IP address
   DWORD dwFlowInfo     __ATTR__;  // IP v6 traffic class & flow information
   DWORD dwScopeId      __ATTR__;  // IP v6 set of interfaces for a scope
} NET_ADDR;


/*
   Flags for ReadSocket(), ReadFromSocket(),
   WriteSocket(), WriteToSocket()
*/
#define NET_FLG_OOB                 0x0001  /* Out of band data */
#define NET_FLG_PEEK                0x0002  /* Don't dequeue data */
#define NET_FLG_PUSH                0x0010  /* Override NET_OPT_WAIT_FLUSH option */
#define NET_FLG_NON_BLOCKING        0x0040  /* Don't block */
#define NET_FLG_BROADCAST           0x0080  /* Broadcast data */
#define NET_FLG_MC_NOECHO           0x1000  /* Don't echo multicast */
#define NET_FLG_EXT                 0x2000  /* Use IPv4/6 addressing */

/* Values used in SetOption */
#define NET_OPT_NON_BLOCKING        0x01    /* Don't block on system calls */
#define NET_OPT_TIMEOUT             0x02    /* Timeout system calls */
#define NET_OPT_KEEPALIVE           0x06    /* TCP keepalive */
#define NET_OPT_WAIT_FLUSH          0x0C    /* Wait for NET_FLG_PUSH in WriteSocket() */

/* values for Async Notification functions */
#define NET_AS_ALARM                0x00
#define NET_AS_OPEN                 0x01
#define NET_AS_RCV                  0x02
#define NET_AS_XMT                  0x03
#define NET_AS_XMT_FLUSH            0x04
#define NET_AS_FCLOSE               0x05
#define NET_AS_CLOSE                0x06
#define NET_AS_ERROR                0x07
#define NET_AS_EXTENDED             0x80  // return extended address

#define MAX_AS_EVENT                NET_AS_ERROR

/* NetInfo structure */
typedef __SPACKED__ struct netinfo
{
   WORD  iClass         __ATTR__;
   WORD  iType          __ATTR__;
   WORD  iNumber        __ATTR__;
   DWORD dwIpAddress    __ATTR__;
   DWORD dwIpSubnet     __ATTR__;
   WORD  iUp            __ATTR__;
   DWORD dwIn           __ATTR__;
   DWORD dwOut          __ATTR__;   /* # of packets transmitted */
   DWORD dwInErr        __ATTR__;   /* # of receiver errors */
   DWORD dwOutErr       __ATTR__;   /* # of transmitter errors */
   WORD  iLanLen        __ATTR__;   /* length of local net address */
   DWORD pLanAddr       __ATTR__;   /* pointer to the lan address */
} NET_INFO;


/* kernel configuration */
typedef __SPACKED__ struct kernel_conf {
   BYTE  bKMaxTcp               ;   /* # of TCP connections allowed */
   BYTE  bKMaxUdp               ;   /* # of UDP connections allowed */
   BYTE  bKMaxIp                ;   /* # of IP connections allowed */
   BYTE  bKMaxRaw               ;   /* # of RAW_NET connections allowed */
   BYTE  bKActTcp               ;   /* # of TCP connections in use */
   BYTE  bKActUdp               ;   /* # of UDP connections in use */
   BYTE  bKActIp                ;   /* # of IP connections in use */
   BYTE  bKActRaw               ;   /* # of RAW_NET connections in use */
   WORD  wKActDCS       __ATTR__;   /* # of active Dos Compatible Sockets */
   WORD  wKActSoc       __ATTR__;   /* # of active Sockets */
   BYTE  bKMaxLnh               ;   /* Maximum header on an attached network */
   BYTE  bKMaxLnt               ;   /* Maximum trailer on an attached network */
   WORD  bKLBUF_SIZE    __ATTR__;   /* Size of a large packet buffer */
   WORD  bKNnet         __ATTR__;   /* Number of network interfaces attached */
   DWORD dwKCticks      __ATTR__;   /* Milliseconds since kernel started */
   DWORD dwKBroadcast   __ATTR__;   /* IP broadcast address in use */
} KERNEL_CONFIG;

/* kernel info */
#define K_INF_TCP_CONS      0x04
#define K_INF_DOMAIN        0x07 // default domain string
#define K_INF_HOST_TABLE    0x08
#define K_INF_DNS_SERVERS   0x09
#define K_INF_DNS_CMPS      0x31 // DNS completion list
#define K_INF_HOSTNAME      0x5a
#define K_INF_LOCAL_PORT    0x5b // Next available local port
#define K_INF_TCP_CB        0x5c // TCP Control Block see API.H
#define K_INF_MASTER_TICK   0x5d // master tick used for timing
#define K_INF_VARPTR        0x5e // variable pointer
#define K_INF_VARBLOCK      0x5f // variable block
#define K_INF_MEMBLOCK      0x60 // memory block
#define K_INF_SETMEM        0x61 // set memory block
#define K_INF_MAC_ADDR      0x81
#define K_INF_BCAST_ADDR    0x84
#define K_INF_IP_ADDR       0x82
#define K_INF_SUBNET_MASK   0x85
#define K_INF_REMOTE_IP     0x86 // remote IP address on PPP

/* Group address structure for JoinGroup() and LeaveGroup() */
typedef __SPACKED__ struct group_addr
{
   DWORD dwGroupAddr __ATTR__;   // group address/ Length of IP address
   DWORD dwIFAddr    __ATTR__;   // IP address of interface to use, 0 for default
   IPAD sIpAddr      __ATTR__;   // IPv4/IPv6 address
} GROUP_ADDR;

/* Route entry for ADD_ROUTE, CHANGE_ROUTE and DROP_ROUTE */

typedef __SPACKED__ struct route_entry
{
   DWORD dwTarget    __ATTR__;   /* target network or host */
   BYTE  bNetBits            ;   /* significant bits in target */
   DWORD dwRouter    __ATTR__;   /* IP address of router (gateway) */
   WORD  wMetric     __ATTR__;   /* metric to use */
   WORD  wInterface  __ATTR__;   /* interface */
   BYTE  bStatus             ;   /* status byte */
#define RT_PROXY      1          /* proxy ARP for this route */
#define RT_PRIVATE    2          /* route not to be advertised */
#define RT_TRIG       4          /* trigger pending for route */
#define RT_STATIC     8          /* route not to be dropped by RIP */
#define RT_PROTO   0x30          /* route protocol field */
#define RT_LOCALIF 0x00          /* local interface automatically added */
#define RT_NETMGMT 0x10          /* config/api added */
#define RT_ICMP    0x20          /* added by icmp redirect */
#define RT_RIP     0x30          /* added by RIP */
   BYTE bPosition            ;   /* where to add route */
#define ROUTE_BOTTOM 0           /* add at bottom of table */
#define ROUTE_TOP    1           /* add at top of table */
	WORD	wMtu;							/* MTU for this route */
} ROUTE_ENTRY;

#ifdef __cplusplus
extern "C" {
#endif
/*
   The following globals are set after each call into sockets
   and indicate particular error codes upon failure of a
   function.
*/
extern int iNetErrNo;
extern int iNetSubErrNo;

/*
   The following globals are used to pass command line options
   to the library version of the stack
*/

extern int iSocketsArgCount;     // Sockets argument count
extern char **ppcSocketsArgVal;  // array of argument values (strings)

// Stack used for Real Mode Callbacks - defined in _CAPI.C
extern char *_pcRmCbStackTop;

int DisableAsyncNotification(void);
int EnableAsyncNotification(void);
DWORD GetAddress(int iSocket);
int GetAddressEx(int iSocket, NET_ADDR *psAddr);
int GetAddressInfo(char *pszName,unsigned uFlags, NET_ADDR *psAddress);
WORD D_FAR *GetBusyFlag(void);
int GetPeerAddress(int iSocket, NET_ADDR *psAddr);
int GetPeerAddressEx(int iSocket, NET_ADDR *psAddr);
int GetKernelInformation(int iReserved,BYTE bCode,BYTE bDevID,
                   void *pData,unsigned *puSize);
int GetVersion(void);
int GetIpVersionFlags(void);
int ICMPPing(DWORD dwHost, int iLength);
int ICMPPingEx(NET_ADDR *psHost, unsigned uSequence, unsigned uWait, int iLength);
int IsSocket(int iSocket);
int GetDCSocket(void);
int GetSocket(void);
int GetKernelConfig(KERNEL_CONFIG *psKC);
int ConvertDCSocket(int iSocket);
int GetNetInfo(int iSocket,NET_INFO *psNI);
int ConnectSocket(int iSocket, int iType, NET_ADDR *psAddr);
int ListenSocket(int iSocket, int iType, NET_ADDR *psAddr);
int SelectSocket(int iMaxID, long *plIflags, long *plOflags);
int ReadSocket(int iSocket, char *pcBuf, unsigned uLen,
            NET_ADDR *psFrom, unsigned uFlags);
int ReadFromSocket(int iSocket, char *pcBuf, unsigned uLen,
               NET_ADDR *psFrom, unsigned uFlags);
int WriteSocket(int iSocket, char *pcBuf, unsigned uLen, unsigned uFlags);
int WriteToSocket(int iSocket, char *pcBuf, unsigned uLen,
              NET_ADDR *psTo, unsigned uFlags);
int EofSocket(int iSocket);
int FlushSocket(int iSocket);
int ReleaseSocket(int iSocket);
int ReleaseDCSockets(void);
int AbortSocket(int iSocket);
int AbortDCSockets(void);
int ShutDownNet(void);
int SetAlarm(int iSocket,DWORD dwTime,void (CAPI_CB *lpHandler)(),
          DWORD dwHint);
int D_FAR *SetAsyncNotification(int iSocket,int iEvent,
                        void (CAPI_CB *lpHandler)(),DWORD dwHint);
void FreeRealModeCallbacks(void);
void CAPI_CB AsyncNotificationHandler(void);
DWORD ResolveName(char *pszName, char *pcCName, int iCNameLen);
DWORD ParseAddress(char *pszName);
int SetSocketOption(int iSocket,int iLevel,int iOption,
               DWORD dwOptionValue,int iLen);
int JoinGroup(DWORD dwGroupAddress, DWORD dwInterfaceAddress);
int LeaveGroup(DWORD dwGroupAddress, DWORD dwInterfaceAddress);
int JoinGroupEx(IPAD *psGroupAddress, DWORD dwInterfaceId, unsigned uAddrLen);
int LeaveGroupEx(IPAD *psGroupAddress, DWORD dwInterfaceId, unsigned uAddrLen);
int IfaceIOCTL(char *pszName, unsigned uFunction);
int GetSocketsVersion(void);
int ArpApi(unsigned uFunction, unsigned uHardware, DWORD dwIpAddress, void D_FAR **pppArpTab);
int ListenAcceptSocket(int iSocket, int iType, int iConnections, NET_ADDR *psAddr);
int AcceptSocket(int iSocket, int iType, NET_ADDR *psAddr);
int ProprietaryApi(unsigned uFunction,unsigned uTcb);
int RouteAdd(ROUTE_ENTRY *psRouteEntry);
int RouteChange(ROUTE_ENTRY *psRouteEntry);
int RouteDrop(ROUTE_ENTRY *psRouteEntry);
int ConfigCommand(char *pszCommand,int iShowError);
#ifdef __cplusplus
}
#endif


/*
 * Local Variables:
 * tab-width:3
 * End:
 */

/*  EOF: capi.h */

