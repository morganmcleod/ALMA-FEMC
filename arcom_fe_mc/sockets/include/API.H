//  $Id: API.H 1.28 2012/07/19 00:16:44Z johnb Exp $
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
/* API.H
   API definitions
*/
// 2005-12-07 gvn define __PACKED__ if not defined
// 2004-12-21 gvn IPv6, PPP
// 2003-11-08 gvn SNMP MIB-II stats
// 2002-12-30 gvn DJGPP compatible, TCB and other updates
// 2002-02-12 gvn add entries to tcpstat

#ifndef __WIN_TYPES
#define __WIN_TYPES
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned long DWORD;
#endif

#ifndef _API_H
#define _API_H

#define API_INT      0x7f

#define API_NOOP        0  /* just keep INET alive */
#define API_SENDPING    1  /* send an ICMP echo request */
#define API_GETPING     2  /* get an ICMP echo request */
#define API_STARTMAIL   3  /* start the mail sender (client) */
#define API_GETIPADR    4  /* get an IP address, given a name */
#define API_TCPOPEN     5  /* open a TCP connection */
#define API_TCPREAD     6  /* read from a TCP connection */
#define API_TCPWRITE    7  /* write to a TCP connection */
#define API_TCPCLOSE    8  /* close a TCP connection */
#define API_TCPRESET    9  /* reset a TCP connection */
#define API_TCPDELET    10 /* delete TCP connection resources */
#define API_UDPOPEN     11 /* open a udp socket to receive dg's */
#define API_UDPREAD     12 /* read a udp datagram */
#define API_UDPWRITE    13 /* write a udp datagram */
#define API_UDPCLOSE    14 /* close a udp socket */
#define API_GETVARPTR   15 /* get pointer to variables */
#define API_GETKEY      16 /* get key from INET */
#define API_FILTER      17 // modify source route filter
#define API_TIMERCREATE 18 // create a timer
#define API_TIMERSET    19 // set a timer
#define API_TIMERSTART  20 // start a timer
#define API_TIMERSTOP   21 // stop a timer
#define API_TIMERDELETE 22 // delete a timer
#define API_UDPREADEX   23 /* read a udp datagram with extended IP addressing */

/* Error return code */
#define NONE         0  /* No error */
#define CON_EXISTS   1  /* Connection already exists */
#define NO_CONN      2  /* Connection does not exist */
#define CON_CLOS     3  /* Connection closing */
#define NO_SPACE     4  /* No memory for TCB creation */
#define WOULDBLK     5  /* Would block */
#define NOPROTO      6  /* Protocol or mode not supported */
#define INVALID      7  /* Invalid arguments */
#define BUFSHORT     8  /* Buffer too short for data */
#define NO_LOCADDR   9  // No local IPv6 address
#define RE_ENTER     27 /* Re-entry of Inet */
#define NO_API       50 // API not loaded

/* Mode for the API_TCPOPEN call */
#define  TCP_PASSIVE 0
#define  TCP_ACTIVE  1
#define  TCP_SERVER  2  /* Passive, clone on opening */

/* Include IP address definition */
#ifndef __IPADDR_H
#include "ipaddr.h"
#endif

#ifndef __PACKED__
#define __PACKED__ __ATTR__
#endif

/* Socket structure */
#ifndef SOCKET_DEF
#define SOCKET_DEF

#define ISOCKET_STRUCT(isocket_s,ipad_t,isocket_t)\
typedef __SPACKED__ struct isocket_s {\
   ipad_t   sIpAddress  __PACKED__;   /* IP address */\
   WORD wPort           __PACKED__;   /* port number */\
} isocket_t
ISOCKET_STRUCT(isocket,IPAD,ISOCKET);
ISOCKET_STRUCT(isocket4,IPAD4,ISOCKET4);
ISOCKET_STRUCT(isocket6,IPAD6,ISOCKET6);

/* Connection structure (two sockets) */
#define CONNECTION_STRUCT(connection_s,ISOCKET_T,CONNECTION_T)\
typedef __SPACKED__ struct connection_s {\
   ISOCKET_T sLocal;\
   ISOCKET_T sRemote;\
} CONNECTION_T
CONNECTION_STRUCT(connection,ISOCKET,CONNECTION);
CONNECTION_STRUCT(connection4,ISOCKET4,CONNECTION4);
CONNECTION_STRUCT(connection6,ISOCKET6,CONNECTION6);

#endif

#ifndef TIMER_DEF
#define TIMER_DEF
typedef __SPACKED__ struct timer {
   PSTRUCT wNext     __ATTR__;   // struct timer __near *next;
   PSTRUCT wPrevious __ATTR__;   // struct timer __near *prev;
   DWORD start       __ATTR__;   /* Period of counter (load value) */
   DWORD count       __ATTR__;   /* Ticks to go until expiration */
   void (D_FAR *func)() __ATTR__;/* Function to call at expiration */
   PSTRUCT wArg         __ATTR__;   // char __near *arg;    /* Arg to pass function */
   char state                ;   /* Timer state */
#define  TIMER_STOP     0
#define  TIMER_RUN      1
#define  TIMER_EXPIRE   2
#define  TIMER_EXT      0x80
} TIMER;
#define MSPTICK 55               /* milliseconds per tick */
#endif

/* IP statistics */
#ifndef IP_STAT_DEF
#define IP_STAT_DEF
__SPACKED__ struct ip_stats {
   DWORD dwTotal     __ATTR__;   /* Total packets received */
   WORD wRunt        __ATTR__;   /* Smaller than minimum size */
   WORD wLength      __ATTR__;   /* IP header length field too small */
   WORD wVersion     __ATTR__;   /* Wrong IP version */
   WORD wChecksum    __ATTR__;   /* IP header checksum errors */
   WORD wBadproto    __ATTR__;   /* Unsupported protocol */
   WORD wReserved1   __ATTR__;   // Reserved location
   WORD wReserved2   __ATTR__;   // Reserved location
   BYTE bIpAddrLen           ;   // IP address length; IPv4 = 4. IPv6 = 16
   PSTRUCT wRoute    __ATTR__;   /* pointer to route table */
   BYTE bIpTTL               ;   /* Default time-to-live for IP datagrams */
   BYTE bIpForwarding                 ;   // 1 - routing, 2 - not routing
   DWORD dwIpInReceives       __ATTR__;
   DWORD dwIpInHdrErrors      __ATTR__;
   DWORD dwIpInAddrErrors     __ATTR__;
   DWORD dwIpForwDatagrams    __ATTR__;
   DWORD dwIpInUnknownProtos  __ATTR__;
   DWORD dwIpInDiscards       __ATTR__;
   DWORD dwIpInDelivers       __ATTR__;
   DWORD dwIpOutRequests      __ATTR__;
   DWORD dwIpOutDiscards      __ATTR__;
   DWORD dwIpOutNoRoutes      __ATTR__;
   DWORD dwIpReasmReqds       __ATTR__;
   DWORD dwIpReasmOKs         __ATTR__;
   DWORD dwIpReasmFails       __ATTR__;
   DWORD dwIpFragOKs          __ATTR__;
   DWORD dwIpFragFails        __ATTR__;
   DWORD dwIpFragCreates      __ATTR__;
   DWORD dwIpRoutingDiscards  __ATTR__;
   BYTE bRouteChanged                 ; // the route table has changed
   PSTRUCT psDestCache        __ATTR__; // destination cache
   PSTRUCT psPrefixList       __ATTR__; // prefix list
   PSTRUCT psDefRouterList    __ATTR__; // default router list
   DWORD dwReAsTime           __ATTR__; // re-assembly time in milli-seconds
};

// MIB-II compatible ICMP statistics
enum {
   ICMP_Msgs,
   ICMP_Errors,
   ICMP_DestUnreachs,
   ICMP_TimeExcds,
   ICMP_ParmProbs,
   ICMP_SrcQuenchs,
   ICMP_Redirects,
   ICMP_Echos,
   ICMP_EchoReps,
   ICMP_Timestamps,
   ICMP_TimestampReps,
   ICMP_AddrMasks,
   ICMP_AddrMaskReps,
   ICMP_STATLEN
};

__SPACKED__ struct icmp_stats {
   DWORD adwIn[ICMP_STATLEN]  __ATTR__;   /* ICMP input stats by type */
   DWORD adwOut[ICMP_STATLEN] __ATTR__;   /* ICMP output stats by type */
};

/* IP routing table entry */
#define NUM_ROUTE_MASKS 33 // number of routing masks
#define NUM_ROUTE_HASHC 5  // number of routing hash chains per mask
__SPACKED__ typedef struct route {
   PSTRUCT wPrev     __ATTR__;   // struct route *prev;  // Linked list pointers
   PSTRUCT wNext     __ATTR__;   // struct route *next;
   DWORD dwTarget    __ATTR__;   // Target IP address
   DWORD dwGateway   __ATTR__;   // IP address of local gateway for this target
   PSTRUCT metric    __ATTR__;   // Hop count
   PSTRUCT  wIface   __ATTR__;   // struct interface *interface;
   TIMER sTimer              ;   // route timer
   char cStatus              ;   // route status
#define RT_PROXY      1          // proxy ARP for this route
#define RT_PRIVATE    2          // route not to be advertised
#define RT_TRIG       4          // trigger pending for route
#define RT_STATIC     8          // route not to be dropped by RIP
#define RT_PROTO   0x30          // route protocol field
#define RT_LOCALIF 0x00          // local interface automatically added
#define RT_NETMGMT 0x10          // config/api added
#define RT_ICMP    0x20          // added by icmp redirect
#define RT_RIP     0x30          // added by RIP
    char cMaskBits           ;   // number of significant bits in target
    WORD wMtu;							// route MTU
} ROUTE;

// IPv6 definitions

#define DUP_ADR_DET_TRANSMITS 2

typedef enum e_lip_state {
   DELAYED,
   TENTATIVE,
   PREFERRED,
   DEPRECATEDA,
   IP_INVALID
} LIP_STATE;

typedef __SPACKED__ struct s_lipad {         // local IPv6 address descriptor
   IPAD sIp          __ATTR__;   // the IP address
   PSTRUCT psNext    __ATTR__;   // next address in chain
   LIP_STATE eState  __ATTR__;   // state of this local IP address
   PSTRUCT psIf      __ATTR__;   // Interface
   TIMER sTimer              ;
   BYTE bDupAddrDetectTransmits          ;
   DWORD dwPreferredLifetime     __ATTR__;
   DWORD dwValidLifetime         __ATTR__;
} LIPAD;

typedef __SPACKED__ struct s_mcad {          // multicast IPv6 address descriptor
   IPAD sIp          __ATTR__;   // the multicast IP address
   PSTRUCT psNext    __ATTR__;   // next address in chain
   PSTRUCT psPrev    __ATTR__;   // previous address in chain
   PSTRUCT psIf      __ATTR__;   // Interface
   TIMER sTimer              ;
   PSTRUCT iUseCount __ATTR__;
   WORD wFlags       __ATTR__;
#define  MC_DELAYING    1  // delaying report
#define  MC_NOREPORT    2  // no report (for FF02::1)
#define  MC_FIRSTREPORT 4  // first report has been sent
#define  MC_LASTREPORT  8  // last report has been sent
} MCAD;

typedef enum e_nc_reach {
   NC_INCOMPLETE,
   NC_REACHABLE,
   NC_STALE,
   NC_DELAY,
   NC_PROBE,
} NC_REACH;

#define MAX_NC_QUEUED 4 // maximum queue size waiting for ND

// Router constants:
#define MAX_INITIAL_RTR_ADVERT_INTERVAL   16000
#define MAX_INITIAL_RTR_ADVERTISEMENTS    3
#define MAX_FINAL_RTR_ADVERTISEMENTS      3
#define MIN_DELAY_BETWEEN_RAS             3
#define MAX_RA_DELAY_TIME                 500

// Host constants:
#define MAX_RTR_SOLICITATION_DELAY        1000
#define RTR_SOLICITATION_INTERVAL         4000
#define MAX_RTR_SOLICITATIONS             3

// Node constants:
#define MAX_MULTICAST_SOLICIT             3
#define MAX_UNICAST_SOLICIT               3
#define MAX_ANYCAST_DELAY_TIME            1000
#define MAX_NEIGHBOR_ADVERTISEMENT        3
#define REACHABLE_TIME                    30000 // milliseconds
#define RETRANS_TIMER                     1000  // milliseconds
#define DELAY_FIRST_PROBE_TIME            5000  // milliseconds
#define MIN_RANDOM_FACTOR                 .5
#define MAX_RANDOM_FACTOR                 1.5
#define MIN_MTU                           1280
#define UNSOLICITED_REPORT_INTERVAL       10000


typedef __SPACKED__ struct s_nc {            // neighbor cache
   IPAD sIp          __ATTR__;   // the IP address
   PSTRUCT psNext    __ATTR__;   // next entry in chain
   PSTRUCT psPrev    __ATTR__;   // previous entry in chain
   IPAD sIpSource    __ATTR__;   // source address for solicitations
   BYTE abLlAddress[6]       ;   // Link Level address
   NC_REACH eReach   __ATTR__;   // reachability state
   WORD wFlags       __ATTR__;
#define IS_ROUTER 1
   PSTRUCT psIf      __ATTR__;   // Interface
   TIMER sTimer              ;
   WORD wRetry       __ATTR__;
   PSTRUCT psQueue   __ATTR__;
   PSTRUCT iNumQueued __ATTR__;   // number of queued packets
} NC;

typedef __SPACKED__ struct s_dc {            // destination cache
   IPAD sIp          __ATTR__;   // the destination IP address
   PSTRUCT psNext    __ATTR__;   // next entry in chain
   PSTRUCT psPrev    __ATTR__;   // previous entry in chain
   PSTRUCT psNc      __ATTR__;   // Neighbor cache entry of next hop
   WORD wMtu         __ATTR__;   // path MTU
   PSTRUCT psIf      __ATTR__;   // Interface
} DC;

typedef __SPACKED__ struct s_pl {            // prefix list
   IPAD sIp          __ATTR__;   // the IP address
   PSTRUCT psNext    __ATTR__;   // next entry in chain
   PSTRUCT psPrev    __ATTR__;   // previous entry in chain
   BYTE bLen                 ;   // prefix length
   PSTRUCT psIf      __ATTR__;   // Interface
   TIMER sTimer              ;
} PL;

typedef __SPACKED__ struct s_drl {           // default router list
   IPAD sIp          __ATTR__;   // the IP address
   PSTRUCT psNext    __ATTR__;   // next entry in chain
   PSTRUCT psPrev    __ATTR__;   // previous entry in chain
   PSTRUCT psIf      __ATTR__;   // Interface
   TIMER sTimer              ;
} DRL;

#endif

/* TCP statistics and status variables */
#define  NTCB  19
#ifndef TCP_STAT_DEF
#define TCP_STAT_DEF
__SPACKED__ struct tcp_stat {
   WORD runt         __ATTR__;   /* Smaller than minimum size */
   WORD checksum     __ATTR__;   /* TCP header checksum errors */
   WORD conout       __ATTR__;   /* Outgoing connection attempts */
   WORD conin        __ATTR__;   /* Incoming connection attempts */
   WORD resets       __ATTR__;   /* Resets generated */
   WORD bdcsts       __ATTR__;   /* Broadcast packets received */
   WORD tcp_window   __ATTR__;   /* TCP Maximum offered window */
   WORD tcp_mss      __ATTR__;   /* Maximum segment size to be sent with SYN */
   WORD tcp_irtt     __ATTR__;   /* Initial guess at round trip time */
   WORD tcp_retry    __ATTR__;   /* maximum transmission retries */
   WORD tel_tos      __ATTR__;   /* TOS used by Telnet */
   BYTE flush_flag           ;   /* To override Nagle */
   WORD tcbs[NTCB]   __ATTR__;   // struct tcb __near *tcbs[NTCB];
   WORD tcp_smss     __ATTR__;   // maximum send segment size
   WORD tcp_time_max __ATTR__;   // maximum time for retry
   BYTE cAckOnPush           ;   // send an ack when push bit is received
   DWORD dwTcpActiveOpens  __ATTR__;
   DWORD dwTcpPassiveOpens __ATTR__;
   DWORD dwTcpAttemptFails __ATTR__;
   DWORD dwTcpEstabResets  __ATTR__;
   DWORD dwTcpCurrEstab    __ATTR__;
   DWORD dwTcpInSegs       __ATTR__;
   DWORD dwTcpOutSegs      __ATTR__;
   DWORD dwTcpRetransSegs  __ATTR__;
   DWORD dwTcpInErrs       __ATTR__;
   DWORD dwTcpOutRsts      __ATTR__;
};
#endif

#ifndef UDP_DEF
#define UDP_DEF

#define UDP_CB_STRUCT(udp_cb_s,ISOCKET_T,UDP_T)\
typedef __SPACKED__ struct udp_cb_s {  /* UDPP Control Block */\
   PSTRUCT  prev     __ATTR__;   /* struct udp_cb __near *prev; */\
   PSTRUCT  next     __ATTR__;   /* struct udp_cb __near *next; */\
   ISOCKET_T socket          ;   /* Local port accepting datagrams */\
   void (D_FAR *r_upcall)() __ATTR__;  /* Function to call when datagram arrives */\
   PSTRUCT rcvq      __ATTR__;   /* Queue of pending datagrams */\
   PSTRUCT rcvcnt    __ATTR__;   /* Count of pending datagrams */\
   PSTRUCT flags     __ATTR__;   /* Flags describing control block */\
} UDP_T

UDP_CB_STRUCT(udp_cb,ISOCKET,UDP);
UDP_CB_STRUCT(udp_cb4,ISOCKET4,UDP4);
UDP_CB_STRUCT(udp_cb6,ISOCKET6,UDP6);

// int flags         __ATTR__;/* Flags describing control block */
#define UDPF_EXT        1     /* Externally defined control block */
#define UDPF_IPV6       2     // IP v6
#define UDPF_DONTFRAG   4     // Don't Fragment flag for IPv4

#endif

/* UDP statistics and status variables */
#define  NUDP  20
#ifndef UDP_STAT_DEF
#define UDP_STAT_DEF
__SPACKED__ struct udp_stat {
   WORD rcvd         __ATTR__;   /* Packets received */
   WORD sent         __ATTR__;   /* Packets sent */
   WORD cksum        __ATTR__;   /* Checksum errors */
   WORD unknown      __ATTR__;   /* Unknown socket */
   WORD bdcsts       __ATTR__;   /* Incoming broadcasts */
   PSTRUCT udps[NUDP] __ATTR__;   /* Hash table for UDP structures */
   // SNMP MIB-II compatible stats
   DWORD dwUdpInDatagrams  __ATTR__;
   DWORD dwUdpNoPorts      __ATTR__;
   DWORD dwUdpInErrors     __ATTR__;
   DWORD dwUdpOutDatagrams __ATTR__;
};
#endif

typedef __SPACKED__ struct s_tracebuf {
   WORD  adr   __ATTR__;      // address of allocated buf
   WORD  id    __ATTR__;      // ID of caller
} TRACEBUF;

__SPACKED__ struct mem_stat {
   DWORD mem_avail   __ATTR__;   /* updated by API_GETVARP call */
   WORD mem_fail     __ATTR__;   /* number of memory alloc failures */
   WORD stack_avail  __ATTR__;   /* lowest stack available observed */
   WORD mem_free_err __ATTR__;   // memory release errors
   PSTRUCT trace_index __ATTR__;   // number of entries in trace buffer
   TRACEBUF D_FAR *pwTraceBuf __ATTR__; // pointer to trace buffer
   PSTRUCT pFreeChain __ATTR__;   // pointer to free chain base
   WORD wLargest     __ATTR__;   // largest free block
};

/* Line printer client */
#define LPCB_STRUCT(ISOCKET_T,LPCB_T)\
typedef __SPACKED__ struct {     /* line printer client control block */\
   ISOCKET_T fsocket      ;\
   TIMER timer            ;\
   WORD tcb       __ATTR__;\
   WORD buf       __ATTR__;\
   BYTE status            ;\
} LPCB_T
LPCB_STRUCT(ISOCKET,LPCB);
LPCB_STRUCT(ISOCKET4,LPCB4);
LPCB_STRUCT(ISOCKET6,LPCB6);


#define IFACE_STRUCT(iface_s,iface_t)\
typedef __SPACKED__ struct iface_s {\
   PSTRUCT next   __ATTR__;      /* Linked list pointer */\
   PSTRUCT name   __ATTR__;      /* Ascii string with interface name */\
   WORD mtu       __ATTR__;      /* Maximum transmission unit size */\
   DWORD ioctl    __ATTR__;      /* Function to handle device control */\
   DWORD send     __ATTR__;      /* Routine to send an IP datagram */\
   DWORD output   __ATTR__;      /* Routine to send link packet */\
   DWORD raw      __ATTR__;      /* Routine to call to send raw packet */\
   DWORD recv     __ATTR__;      /* Routine to process input */\
   DWORD stop     __ATTR__;      /* Routine to call before stopping */\
   DWORD xmit     __ATTR__;      /* Routine to xmit on serial link */\
   DWORD stxrdy   __ATTR__;      /* Routine to test for xmit ready */\
   DWORD srecv    __ATTR__;      /* Routine for receiving from serial link */\
   WORD dev       __ATTR__;      /* Subdevice number to pass to send */\
   PSTRUCT dcb    __ATTR__;      /* device control block */\
   PSTRUCT indcb  __ATTR__;      /* input device control block */\
   PSTRUCT prot_ext __ATTR__;    /* protocol extension (used for PPP) */\
   PSTRUCT mbuf   __ATTR__;      /* general purpose buffer pointer */\
   WORD flags     __ATTR__;      /* Configuration flags */\
   WORD sflags    __ATTR__;      /* serial flags */\
   WORD gpvar     __ATTR__;      /* General purpose variable */\
   WORD trace     __ATTR__;      /* Trace flags */\
   PSTRUCT hwaddr    __ATTR__;      /* Device hardware address, if any */\
   DWORD ip_addr  __ATTR__;      /* Our IP address */\
   TIMER timer            ;      /* interface timer */\
   WORD up_time   __ATTR__;      /* timeout for UP status */\
   WORD down_time __ATTR__;      /* timeout for DOWN (!UP) status */\
   DWORD ip_query __ATTR__;      /* IP address to query */\
   char stat              ;      /* Status of the interface */\
   char netbits           ;      /* number of bits in netmask */\
   char hwtype            ;      /* hardware type */\
   char hwalen            ;      /* hardware address length */\
   char dhcp_state        ;\
   TIMER t1                  ;   /* DHCP timer, used for re-xmit, T1 and T2 */\
   DWORD dhcp_server __ATTR__;\
   char send_retry           ;   /* DHCP send_retry */\
   DWORD start_time  __ATTR__;   /* time starting BOOTP/DHCP */\
   DWORD xid         __ATTR__;   /* time of last BOOTP/DHCP, also xid */\
   DWORD renew       __ATTR__;   /* renew_time */\
   DWORD rebind      __ATTR__;   /* rebind_time */\
   DWORD lease       __ATTR__;   /* lease_time */\
   DWORD TempIP      __ATTR__;\
   /* SNMP MIB-II compatible stats */\
   DWORD dwIfInOctets         __ATTR__;\
   DWORD dwIfInUcastPkts      __ATTR__;\
   DWORD dwIfInNUcastPkts     __ATTR__;\
   DWORD dwIfInDiscards       __ATTR__;\
   DWORD dwIfInErrors         __ATTR__;\
   DWORD dwIfInUnknownProtos  __ATTR__;\
   DWORD dwIfOutOctets        __ATTR__;\
   DWORD dwIfOutUcastPkts     __ATTR__;\
   DWORD dwIfOutNUcastPkts    __ATTR__;\
   DWORD dwIfOutDiscards      __ATTR__;\
   DWORD dwIfOutErrors        __ATTR__;\
   BYTE bDupAddrDetectTransmits       ;\
   DWORD dwRetransTimer       __ATTR__;\
   DWORD dwReachableTime      __ATTR__;\
   DWORD dwMtu                __ATTR__;\
   PSTRUCT psLocalIp          __ATTR__;   /* local IP address chain */\
   PSTRUCT psMcAd             __ATTR__;   /* multicast address chain */\
   PSTRUCT psNeighborCache    __ATTR__;\
   BYTE abInterfaceId[8]              ;   /* interface identifier*/\
} iface_t

IFACE_STRUCT(interface,IFACE);

// WORD flags     __ATTR__;      /* Configuration flags */
#define  DIGI_FLAG      0x0001   /* Digiboard is the interface */
#define  MDD_FLAG       0x0002   // Multi Destination Driver
#define  IEEE_FLAG      0x0004   /* Use IEEE encapsulation */
#define  X25_FLAG       0x0008   /* Using X.25 driver */
#define  LCS_FLAG       0x0010   /* LCS is the device */
#define  ETH_FLAG       0x0020   /* Ethernet interface */
#define  IPX_FLAG       0x0040   /* IPX interface */
#define  SECUR_FLAG     0x0080   /* Security bit set */
#define  TOKEN_FLAG     0x0100   /* Token ring */
#define  PPP_FLAG       0x0200   // PPP
#define  DHCPI_FLAG     0x0400   // Use DHCPINFORM
#define  NAT_FLAG       0x0800   // perform Network Address Translation
#define  GARP_FLAG      0x1000   // gratuitous ARP
#define  TIMER_FLAG     0x2000   // restart timer
#define  IPV6IF_FLAG    0x4000   // IP version 6 on this interface

// WORD sflags    __ATTR__;      // serial flags
#define  SF_RAW         0x0001
#define  SF_INUSE       0x0002
#define  SF_LAPB        0x0004   /* Use LAPB over SLIP */
#define  SF_FTP         0x0008   /* used for raw FTP */
#define  SF_TELNET      0x0010   /* used for raw TELNET */
#define  SF_PASSFC      0x0020   /* pass flow control characters on */
#define  SF_DELIVER     0x0040   /* deliver raw data */
#define  SF_CLOSEBREAK  0x0080   /* Close on break */
#define  SF_UNC         0x0100   // uncommitted
#define  SF_COMPRESS    0x0200   // use compression
#define  SF_ASSIGNED_AD 0x0400   // using an assigned address
#define  SF_PPP_TRACE   0x0800   // tracing PPP file - ignore data

// WORD trace     __ATTR__;      /* Trace flags */
#define  IF_TRACE_OUT   0x01     /* Output packets */
#define  IF_TRACE_IN    0x10     /* Packets to me except broadcast */
#define  IF_TRACE_COMP  0x80     /* dump compressed packets */
#define  IF_TRACE_CHAR  0x100    /* Dump data in char */
#define  IF_TRACE_DATA  0x200    /* Dump data in char/hex */
#define  IF_TRACE_HEX   0x400    /* Dump packets in hex/char */
#define  IF_TRACE_RAW   0x800    /* Dump packets with no header */
#define  IF_TRACE_TIME  0x1000   /* Dump packets with time info */
#define  IF_TRACE_EBCD  0x2000   /* Dump data in ebcdic */
#define  IF_TRACE_NOBC  0x4000   /* Dont dump broadcast traffic */
#define  IF_TRACE_STAT  0x8000   /* dump statistics in binary */

// char stat      __ATTR__;      // Status of the interface
#define IF_UP        1           // interface is up
#define IF_QUERY     2           // query UP status
#define IF_NORIPADV  4           // no RIP advertise
#define IF_NORIPUSE  8           // no RIP use
#define IF_THIS_RIP  0x10        // this is a RIP
#define IF_LAST_RIP  0x20        // last was a rip
#define IF_DISC      0x40        // disconnect X.25 on next timeout
#define IF_DISABLED  0x80        // disabled from sending

// char dhcp_state __ATTR__;
#define D_NONE          0        // don't use DHCP or BOOTP
#define D_BOOTP         1        // use BOOTP
#define D_INIT          2        // DHCP INIT state
#define D_SELECTING     3
#define D_REQUESTING    4
#define D_BOUND         5
#define D_RENEWING      6
#define D_REBINDING     7
#define D_DECLINING     8
#define D_INFORMING     9

typedef __SPACKED__ struct ser {    // serial control block
   DWORD oldvec   __ATTR__;   // Original interrupt vector [cs:ip]
   WORD buf       __ATTR__;   // Ring buffer
   WORD bufsize   __ATTR__;   // Size of ring buffer
   WORD wp        __ATTR__;   // Write pointer
   WORD rp        __ATTR__;   // Read pointer
   WORD icnt      __ATTR__;   // count of characters in buffer
   WORD stop_cnt  __ATTR__;   // count at which to stop flow
   WORD start_cnt __ATTR__;   // count at which to start flow
   WORD data      __ATTR__;   // current output pointer
   WORD cnt       __ATTR__;   // byte count remaining
   WORD palt      __ATTR__;   // pointer to alternate control block
   WORD flags     __ATTR__;   // flags

#define  FL_HDLC        0x01  // hdlc mode
#define  FL_OFSTOPPED   0x02  // output flow stopped
#define  FL_IFSTOPPED   0x04  // input flow stopped
#define  FL_MODEM       0x08  // modem type connection
#define  FL_CONNECTED   0x10  // connected in modem mode
#define  FL_IS16550A    0x20  // is 16550A chip
#define  FL_CHANB       0x40  // channel B for SCC's
#define  FL_LAPB        0x80  // LAPB used on serial link
#define  FL_EVENT       0x100 // modem event
#define  FL_PPP         0x200 // Point to Point Protocol
#define  FL_DISABLED    0x400 // This interface is disabled
#define  FL_SEND_XON    0x800 // Send a flow-control XON
#define  FL_SEND_XOFF   0x1000// Send a flow-control XOFF
#define  FL_SUB_ISR     0x2000// Subsequent interrupt service routine

   char iflow             ;   // input flow control
#define IF_XON      1
#define IF_DTR      2
#define IF_RTS      3
#define IF_INV      8         // inverse of modem signal
   char oflow             ;   // output flow control
#define OF_XON      1
#define OF_DCD      2
#define OF_DSR      3
#define OF_CTS      4
#define OF_INV      8         // inverse of modem signal
   char mcr               ;   // value of modem control register
   WORD addr      __ATTR__;   // Base I/O address
   WORD vec       __ATTR__;   // Interrupt vector
   WORD next      __ATTR__;   // next serial controller using same int vector
   DWORD speed    __ATTR__;   // Line speed
   char datamask          ;   // data bits MASK (7F or FF)
   char lcr               ;   // value of line control register
   char oldlcr            ;   // old value of line control register
   char oldmask           ;   // old interrupt mask
   // raw interface usage
   WORD iface     __ATTR__;   // Pointer to interface
   TIMER timer            ;   // Character timer
   WORD overrun   __ATTR__;   // overrun error count
   WORD parerr    __ATTR__;   // parity error count
   WORD frmerr    __ATTR__;   // framing/CRC error count
   WORD brkerr    __ATTR__;   // break/abort error count
   WORD buferr    __ATTR__;   // insufficient buffer space
   WORD undererr  __ATTR__;   // xmit underrun errors
   DWORD rcvcount __ATTR__;   // number of characters received
   DWORD xmtcount __ATTR__;   // number of characters xmitted
   char deliver           ;   // set when input to deliver
#define DEL_CR 1              // cr seen in data
#define DEL_LF 2              // lf seen in data
#define DEL_TO 4              // character timeout
#define DEL_BH 8              // buffer half full
   char ostate            ;   // output state
#define OS_IDLE      0
#define OS_RAW       1
#define OS_FSTART    2
#define OS_DATA      3
#define OS_FRESC     4
#define OS_FREND     5
#define OS_CRC1      6
#define OS_CRC1ESC   7
#define OS_CRC1END   8
#define OS_CRC2      9
#define OS_CRC2ESC   10
#define OS_CRC2END   11
#define OS_END       12
#define OS_FHDLC     13
#define OS_HDLCDATA  14
#define OS_HDLCEND   15
#define OS_HDLCCRC   16
#define OS_HDLC_EA   17
#define OS_FCS1      18
#define OS_FCS1ESC   19
#define OS_FCS2      20
#define OS_FCS2ESC   21
#define OS_MODEM     22

   char istate            ;   // Input state
#define IS_RAW       0
#define IS_DATA      1
#define IS_ESC       2
#define IS_IGN       3
#define IS_DATAB     4
#define IS_ESCB      5
#define IS_IGNB      6
#define IS_HDLC      7
#define IS_PPP       8
#define IS_PPPESC    9
#define IS_PPPIGN    10
#define IS_MODEM     11

   // slip structures
   WORD sndq      __ATTR__;   // send queue
   WORD tbp       __ATTR__;   // Transmit mbuf being sent
   WORD oaccum    __ATTR__;   // CRC output accumulator
   WORD rcvq      __ATTR__;   // receive queue
   WORD rbp       __ATTR__;   // Pointer to mbuf currently being written
   WORD accum     __ATTR__;   // CRC input accumulator
   WORD errors    __ATTR__;   // Receiver input errors
   // PPP values
   DWORD accm_o   __ATTR__;   // async char control mask - output
   DWORD accm_i   __ATTR__;   // async char control mask - input
   BYTE last_o_ch         ;   // last output char
   BYTE last_i_ch         ;   // last input char
   // modem parameters
   WORD pmpar     __ATTR__;   // pointer to modem parameters
} SER;

// Supported Configuration Protocol index
enum {
   LCP,
   AUTHP,
   IPCP,
   IPV6CP_ONLY = 2,
   IPV6CP,
   FSM_SZ = 3,
   FSM_SZ_DUAL
};

// Finite State Machine Control Block
typedef __SPACKED__ struct fsm_s {
   BYTE state                   ;   // FSM state
   BYTE lastid                  ;   // ID of last REQ we sent

   BYTE flags                   ;
#define PPP_ESCAPED  0x01
#define PPP_TOSS     0x02
#define FSM_PASSIVE  0x40           // opened passive
#define FSM_ACTIVE   0x80           // opened active

   BYTE retry                   ;   // counter for timeouts
   BYTE try_req                 ;   // # tries for request
   BYTE try_terminate           ;   // # tries for terminate

   BYTE retry_nak               ;   // counter for naks of requests
#define bChallengeRetry retry_nak   // for AP use as Challenge retry
   BYTE try_nak                 ;   // # tries for nak substitutes
#define bResponseRetry try_nak      // for AP use as Response retry

   PSTRUCT psPPP        __ATTR__;   // the ppp we belong to
   TIMER timer                  ;
   PSTRUCT pdc          __ATTR__;   // protocol dependent constants
   PSTRUCT pdv          __ATTR__;   // protocol dependent variables
   PSTRUCT pdv_out      __ATTR__;   // pdv for outgoing modem_connections
   PSTRUCT pdv_in       __ATTR__;   // pdv for incoming modem connections
} FSM;

// Authentication Protocol control block
typedef __SPACKED__ struct ap_s {
   PSTRUCT psApUsername __ATTR__;   // Username for REQ
   PSTRUCT psApPassword __ATTR__;   // Password for REQ
   PSTRUCT psApMessage  __ATTR__;   // message from last ACK/NAK
} AP;

// PPP control block
#define PPP_STRUCT(fsm_size,ppp_t)\
typedef __SPACKED__ struct {\
   PSTRUCT iface           __ATTR__;   /* pointer to interface block */\
   BYTE phase                      ;   /* phase of link initialization */\
   BYTE id                         ;   /* id counter for connection */\
   BYTE flags                      ;\
   BYTE trace;                         /* trace flags for connection */\
   FSM arsFsm[fsm_size]            ;   /* finite state machines */\
   DWORD upsince           __ATTR__;   /* Timestamp when Link Opened */\
   PSTRUCT peername        __ATTR__;   /* Peername from remote (if any) */\
   DWORD OutTxOctetCount   __ATTR__;   /* # octets sent */\
   DWORD OutOpenFlag       __ATTR__;   /* # of open flags sent */\
   WORD OutNCP[fsm_size]   __ATTR__;   /* # NCP packets sent by protocol */\
   WORD OutError           __ATTR__;   /* # packets with error on send */\
   WORD OutMemory          __ATTR__;   /* # alloc failures on send */\
   DWORD InRxOctetCount    __ATTR__;   /* # octets received */\
   DWORD InOpenFlag        __ATTR__;   /* # of open flags */\
   WORD InNCP[fsm_size]    __ATTR__;   /* # NCP packets by protocol */\
   WORD InUnknown          __ATTR__;   /* # unknown packets received */\
   WORD InChecksum         __ATTR__;   /* # packets with bad checksum */\
   WORD InFrame            __ATTR__;   /* # packets with frame error */\
   WORD InError            __ATTR__;   /* # packets with other error */\
   WORD InMemory           __ATTR__;   /* # alloc failures */\
} ppp_t

PPP_STRUCT(FSM_SZ,PPP);
PPP_STRUCT(FSM_SZ_DUAL,PPP_DUAL);


// BYTE flags;
#define PPP_CBV         0x01  // callback verification
#define PPP_CBVCALL     0x02  // callback in progress
#define PPP_AP_LOCAL    0x10  // local authentication
#define PPP_AP_REMOTE   0x20  // remote authentication

typedef __SPACKED__ struct {
   PSTRUCT  ip_stats    __ATTR__;   // struct ip_stats __near *ipstat;
   PSTRUCT  tcp_stat    __ATTR__;   // struct tcp_stat __near *tcpstat;
   PSTRUCT  memstat     __ATTR__;   // struct mem_stat __near *memstat;
   PSTRUCT  lpcb        __ATTR__;   // LPCB __near *lpcb;
   PSTRUCT  ndesc       __ATTR__;   // void __near *ndesc;
   PSTRUCT  gdesc       __ATTR__;   // void __near *gdesc;
   PSTRUCT  nndesc      __ATTR__;   // number of network descriptors (local)
   PSTRUCT  ngdesc      __ATTR__;   // number of global network descriptors
   DWORD    http_addr   __ATTR__;   // HTTP server address (set by BOOTP/DHCP)
   PSTRUCT  ppIface     __ATTR__;   // Pointer to interface chain
   PSTRUCT  udp_stat    __ATTR__;   // UDP status address
   PSTRUCT  icmp_stat   __ATTR__;   // ICMP status address
   DWORD    dwFeatures  __ATTR__;   // Features of this kernel
   PSTRUCT  psUserPass  __ATTR__;   // User/password for PPP
} VARBLOCK;

// Kernel feature bit definitions
#define KF_I386      0x0001   // Compiled for 80386 processor
#define KF_PKTDRV    0x0002   // Packet Driver supported
#define KF_MODEM     0x0004   // Modem supported
#define KF_SLIP      0x0008   // Slip supported
#define KF_CSLIP     0x0010   // Compressed Slip supported
#define KF_PPP       0x0020   // PPP supported
#define KF_CHAPMD5   0x0040   // CHAP MD5 supported
#define KF_MIB2      0x0080   // MIB-II supported
#define KF_IPV4      0x0100   // IPv4 supported
#define KF_IPV6      0x0200   // IPv6 supported
#define KF_RIP       0x0400   // RIP supported
#define KF_IGMP      0x0800   // IGMP supported
#define KF_ALTIF     0x1000   // Alternate interface supported
#define KF_PRNT      0x2000   // Printers supported
#define KF_LPD       0x4000   // LPD supported
#define KF_RCS       0x8000   // Remote Console Server supported

/* TCP port numbers */
#define  ECHO_PORT      7           /* Echo data port */
#define  DISCARD_PORT   9           /* Discard data port */
#define  FTPD_PORT      20          /* FTP Data port */
#define  FTP_PORT       21          /* FTP Control port */
#define  TELNET_PORT    23          /* Telnet port */
#define  SMTP_PORT      25          /* Mail port */

typedef __SPACKED__ struct {                    /* UDP write parameter block */
   DWORD dwRemIPAd      __ATTR__;   /* remote IP address, 4 or 16 for IPv4/6 */
   WORD  wRemPort       __ATTR__;   /* remote port */
   WORD  wLength        __ATTR__;   /* data count */
   char  D_FAR *lpBuf   __ATTR__;   /* data buffer address */
   IPAD6 sIp            __ATTR__;   // IPv4/IPv6 address
} UDP_WR_P;

#ifndef TCB_DEF
#define TCB_DEF
#define TCB_STRUCT(tcb_s,connection,tcb_t)\
typedef __SPACKED__ struct tcb_s {  /* TCP Control Block */\
   PSTRUCT  prev     __ATTR__;   /* struct tcb *prev;*/\
   PSTRUCT  next     __ATTR__;   /* struct tcb *next;*/\
   connection conn           ;                         \
   BYTE  state               ;   /* Connection state */\
   BYTE  reason              ;   /* Reason for closing */\
   /* If reason == NETWORK, the ICMP type and code values are stored here */\
   char  type                ;                         \
   char  code                ;                         \
                                                       \
   /* Send sequence variables */                       \
   __SPACKED__ struct {                                            \
      long una       __ATTR__;   /* First unacknowledged sequence number */\
      long nxt       __ATTR__;   /* Next sequence num to be sent for the first time */\
      long ptr       __ATTR__;   /* Working transmission pointer */\
      WORD wnd       __ATTR__;   /* Other end's offered receive window */\
      long up        __ATTR__;   /* Send urgent pointer */\
      long wl1       __ATTR__;   /* Sequence number used for last window update */\
      long wl2       __ATTR__;   /* Ack number used for last window update */\
   } snd                     ;                         \
   long iss          __ATTR__;   /* Initial send sequence number */\
   WORD cwind        __ATTR__;   /* Congestion window */\
   WORD ssthresh     __ATTR__;   /* Slow-start threshold */\
   long resent       __ATTR__;   /* Count of bytes retransmitted */\
   /* Receive sequence variables */                    \
   __SPACKED__ struct {                                            \
      long nxt       __ATTR__;   /* Incoming sequence number expected next */\
      WORD wnd       __ATTR__;   /* Our offered receive window */\
      long up        __ATTR__;   /* Receive urgent pointer */\
   } rcv                     ;                         \
   long irs          __ATTR__;   /* Initial receive sequence number */\
   WORD mss          __ATTR__;   /* Maximum segment size */\
   long rerecv       __ATTR__;   /* Count of duplicate bytes received */\
   WORD window       __ATTR__;   /* Receiver window and send queue limit */\
   char backoff              ;   /* Backoff interval */\
   char retry                ;   /* retry count */\
   void (D_FAR *r_upcall)()   __ATTR__;   /* Call when "significant" amount of data arrives */\
   void (D_FAR *t_upcall)()   __ATTR__;   /* Call when ok to send more data */\
   void (D_FAR *s_upcall)()   __ATTR__;   /* Call when connection state changes */\
   char flags                ;   /* Control flags */\
   char tos                  ;   /* Type of service (for IP) */\
   PSTRUCT rcvq      __ATTR__;   /* Receive queue */\
   WORD  rcvcnt      __ATTR__;                      \
   PSTRUCT sndq      __ATTR__;   /* Send queue */\
   WORD  sndcnt      __ATTR__;   /* Number of unacknowledged sequence */\
                                 /* numbers on send queue.*/\
   PSTRUCT reseq     __ATTR__;   /* Out-of-order segment queue */\
   TIMER timer               ;   /* Retransmission timer */\
   TIMER rtt_timer           ;   /* Round trip timer */\
   long rttseq       __ATTR__;   /* Sequence number being timed */\
   long srtt         __ATTR__;   /* Smoothed round trip time, milliseconds */\
   long mdev         __ATTR__;   /* Mean deviation, milliseconds */\
   PSTRUCT user      __ATTR__;   /* User parameter */\
   TIMER ack_timer           ;   /* Ack timer */\
   WORD rcv_wndav    __ATTR__;   /* Our real available receive window */\
   WORD mss_rcv      __ATTR__;   /* MSS for receiving */\
   WORD cnt_to_urg   __ATTR__;   /* count to urgent data */\
   WORD urg_cnt      __ATTR__;   /* count of urgent data */\
   char flags1               ;   /* some more flags      */\
   BYTE num_nacc             ;   /* Number of not accepted connections */\
   BYTE max_nacc             ;   /* Maximum number not accepted connections */\
   PSTRUCT naccq     __ATTR__;   /* Not accepted connections queue */\
   PSTRUCT psIf      __ATTR__;   /* for IPv6 scope id */\
} tcb_t

// BYTE  state       __ATTR__;   /* Connection state */
#define  CLOSED         0
#define  LISTEN         1
#define  SYN_SENT       2
#define  SYN_RECEIVED   3
#define  ESTABLISHED    4
#define  FINWAIT1       5
#define  FINWAIT2       6
#define  CLOSE_WAIT     7
#define  CLOSING        8
#define  LAST_ACK       9
#define  TIME_WAIT      10

// BYTE  reason      __ATTR__;   /* Reason for closing */
#define  R_NORMAL    0           /* Normal close */
#define  NORMAL_R    0           // for compatibility
#define  R_RESET     1           /* Reset by other end */
#define  R_TIMEOUT   2           /* Excessive retransmissions */
#define  R_NETWORK   3           /* Network problem (ICMP message) */

// char flags        __ATTR__;   /* Control flags */
#define  FORCE       1           /* We owe the other end an ACK or window update */
#define  CLONE       2           /* Server-type TCB, cloned on incoming SYN */
#define  RETRAN      4           /* A retransmission has occurred */
#define  TCP_RCV     8           /* Set on data arrival, reset by user */
#define  TCP_XMT     0x10        /* Set when ok to send more, reset by user */
#define  TCP_STC     0x20        /* Set on state change, reset by user */
#define  SEND_URG    0x40        /* Set when sending urgent data */
#define  RECV_URG    0x80        /* Set when receiving urgent data */

// char flags1       __ATTR__;   // some more flags
#define F1_LISTEN0      1        /* was listening on 0.0.0.0:0 */
#define F1_KEEPALIVE    2        /* keep alive is active       */
#define F1_KATIMING     4        /* keep alive timer running   */
#define F1_MUSTACK      8        /* Ack is outstanding         */
#define F1_MAXSEGRCVD   0x10     /* Max segment has been received */
#define F1_IPV6         0x20     // IPv6 connection

TCB_STRUCT(tcb,CONNECTION,TCB);
TCB_STRUCT(tcb4,CONNECTION4,TCB4);
TCB_STRUCT(tcb6,CONNECTION6,TCB6);
#endif

#ifndef TCBH
#if defined(KERNEL) || defined(_SOCK32)
#define TCBH __SPACKED__ struct tcb *
#else
#define TCBH unsigned
#endif   //#if defined(KERNEL) || defined(_SOCK32)
#endif //TCBH

typedef __SPACKED__ struct {     /* TCP open parameter block */
   DWORD dwRemIPAd   __ATTR__;   /* remote IP address (0 for server, 1 for IPv4/6) */
   WORD  wRemPort    __ATTR__;   /* remote port (0 for server) */
   WORD  wLocPort    __ATTR__;   /* local port (0 for auto assign, set by kernel) */
   WORD  wMode       __ATTR__;   /* active/passive/server */
   WORD  wWindow     __ATTR__;   /* window size (0 for default) */
   /* data receive upcall handler (0 for none) */
   void (D_FAR *pRcvUpcall)(TCBH psTcb,unsigned uCnt)  __ATTR__;
   /* data send upcall handler (0 for none) */
   void (D_FAR *pSndUpcall)(TCBH psTcb,unsigned uCnt)  __ATTR__;
   /* state change upcall handler (0 for none) */
   void (D_FAR *pStcUpcall)(TCBH psTcb,char cOldState,char cNewState)   __ATTR__;
   WORD pUser        __ATTR__;   // void __near *pUser;  /* user id field in TCB */
   char cTos                 ;   /* type of service to use (normally 0) */
   // the next two values are used when dwRemIPAd == 1
   BYTE  bAdLen              ;   // Address length (4 or 16)
   IPAD6 sIp         __ATTR__;   // IPv4/IPv6 address
   DWORD dwScopeId   __ATTR__;   // Scope ID for IPv6 scopes
} TCP_OP_P;

// CAPI internal structures

// also defined in CAPI.H, must be exactly the same
#define NET_AS_ERROR                0x07
#define MAX_AS_EVENT                NET_AS_ERROR

typedef __SPACKED__ struct {     /* event */
   int (D_FAR *handler)(void)    __ATTR__;
   unsigned long hint            __ATTR__;
} EVENT;

typedef __SPACKED__ struct {     /* network descriptor */
   PSTRUCT flags         __ATTR__;
#define ND_ASSIGNED  1
#define NO_BLOCK     2
#define TCP_CON      4
#define UDP_CON      8
#define TIME_OUT     0x10
#define WRITE_NOTIFY 0x20  // write notification enabled
#define KEEP_ALIVE   0x40  // keep-alive is set
#define WAIT_FLUSH   0x80  // NET_OPT_WAITFLUSH is set
#define LISTEN_FLAG  0x100 // listen() was used
#define IPV6_FLAG    0x200 // IPv6 socket
#define EXTADDR_FLAG 0x2000// Extended address info flag
   WORD  tcb               __ATTR__;   //TCB *tcb;
   EVENT event[MAX_AS_EVENT + 1]   ;
   unsigned long timer     __ATTR__;   // timer value set by set_option()
   unsigned long ctime     __ATTR__;   // current timer value while timing
   WORD alarm_timer        __ATTR__;   // TIMER *alarm_timer;
} ND;

#define UCB_DEF
#define UCB_STRUCT(SOCKET_T,UCB_T)\
typedef __SPACKED__ struct {     /* UDP structure referenced in ND */\
   SOCKET_T remote_socket          ;\
   unsigned lport          __ATTR__;\
} UCB_T

UCB_STRUCT(ISOCKET,UCB);
UCB_STRUCT(ISOCKET4,UCB4);
UCB_STRUCT(ISOCKET6,UCB6);

// Prototypes for API.C interface
// This is work in progress

DWORD GetIPAddress(char *pszName);
int TcpOpen(TCP_OP_P *pOp,WORD *pwHandle);
int TcpRead(WORD wHandle,char *pcBuf,int iLen);
int TcpWrite(WORD wHandle,char *pcBuf,int iLen,int iUrgent);
int TcpClose(WORD wHandle);
int TcpReset(WORD wHandle);
int TcpDelete(WORD wHandle);
char *NetErrStr(int iErrCode);

/* API documentation:
The API is called by calling interrupt API_INT with AH = function code
Other registers are set up and/or returned as detailed below:

SENDPING:
 Entry:
   AH:      API_SENDPING
   AL:      0
   BX:      Sequence number
   CX,DX:   IP address
 Exit:
   No change

SENDPING:   (Extended address)
 Entry:
   AH:      API_SENDPING
   AL:      1
   BX:      Sequence number
   CX,DX:   Pointer to IP address
 Exit:
   No change

GETPING:
 Entry:
   AH:      API_GETPING
   AL:      0
 Exit:
   AH:      == 0 -> no response
            != 0 -> valid response
   BX:      Sequence number
   CX,DX:   IP address

GETPING:    (Extended address)
 Entry:
   AH:      API_GETPING
   AL:      1
   CX,DX:   Pointer to area to return IP address
 Exit:
   AH:      == 0 -> no response
            != 0 -> valid response
   BX:      Sequence number

STARTMAIL:
 Entry:
   AH:      API_STARTMAIL
 Exit:
   No change

GETIPADR:
 Entry:
   AH:      API_GETIPADR
   AL:      0
   BX:CX:   Pointer to name
   If BX:CX == 0:0 then return local IP address and hostname
 Exit:
   CX,DX:   IP address (0 if not found)
   BX:SI:   Pointer to hostname (if BX:CX == 0:0)

GETIPADR:   (Extended address)
 Entry:
   AH:      API_GETIPADR
   AL:      1
   BX:CX:   Pointer to name
   DX:SI:   Pointer to area to return IP address
 Exit:
   AX:      0 - not found, 4 - IPv4 address, 16 - IPv6 address

TCPOPEN:    open a TCP connection
 Entry:
   AH:      API_TCPOPEN
   BX:CX:   Parameters address, SEG:OFF
 Exit:
   BX:CX:   TCB address, SEG:OFF default INET
   CX:BX:   TCB address, SEG:OFF Large Inet (BX == 4)
   AX:      Error

TCPREAD:    read from a TCP connection
 Entry:
   AH:   API_TCPREAD
   BX:   TCB address
   CX:   Count
   DX:SI Buffer address (SEG:OFF)
 Exit:
   CX:   Actual read count
   AX:   Error

TCPWRITE:      write to a TCP connection
 Entry:
   AH:   API_TCPWRITE
   AL:   1 for urgent data
   BX:   TCB address
   CX:   Count
   DX:SI Buffer address (SEG:OFF)

 Exit:
   CX:   Actual write count
   AX:   Error

TCPCLOSE:      close a TCP connection
TCPRESET:      reset a TCP connection
TCPDELET:      delete TCP connection resources
 Entry:
   AH:   API_TCPCLOSE, API_TCPRESET or API_TCPDELET
   BX:   TCB address

 Exit:
   AX:   Error

UDPOPEN:    open a udp socket to receive dg's
 Entry:
   AH:   API_UDPOPEN
   BX:   Local port number
   CX:DX:   Receive upcall address  (0000:0000 for none)
 Exit:
   AX:   Error

UDPREAD:    read a udp datagram
 Entry:
   AH:   API_UDPREAD
   BX:   Local port number
   CX:   Max count
   DX:SI Buffer address (SEG:OFF)
 Exit:
   AX:   Error
   CX:   Actual read count
   BX:   Remote port
   DX,SI:   Remote IP address

UDPREADEX:   read a udp datagram with extended addressing
 Entry:
   AH:   API_UDPREADEX
   BX:   Local port number
   CX:   Max data count
   DX:SI Buffer address (SEG:OFF), first 16 bytes receives remote IP address
 Exit:
   AX:   Error
   CX:   Actual read count
   BX:   Remote port
   DX:   Remote IP address length (4 or 16)

UDPWRITE:      write a udp datagram
 Entry:
   AH:   API_UDPWRITE
   BX:   Local port number
   DX:SI Parameter address (SEG:OFF) (UDP_WR_P)
 Exit:
   AX:   Error

UDPCLOSE:      close a udp socket
 Entry:
   AH:   API_UDPCLOSE
   BX:   Local port number
 Exit:
   AX:   Error

GETVARPTR:     get pointer to variables
 Entry:
   AH:   API_GETVARPTR
 Exit:
   BX:CX:   variable block address, SEG:OFF (VARBLOCK)

GETKEY:        get a key from INET
 Entry:
   AH:   API_GETKEY
 Exit:
   AX:   key scancode or -1 if none
   BX:   shift status

FILTER:        modify source route filter to default route
 Entry:
   AH:   API_FILTER
   BX:   FILTER_DISABLE
      FILTER_ENABLE
      FILTER_FLUSH
      FILTER_ADD
   CX,DX address to add to filter list
 Exit:   0  success
      -1 address already in filter list
      -2 not enough memory to add address
      FILTER_DROP
   CX,DX address to drop from filter list
 Exit:   0  success
      -1 address not in filter list

TIMERCREATE:   create a timer
 Entry:
   AH:      API_TIMERCREATE
   BX:     Argument to pass to callback funtion
   CX:DX    Timer period in milliseconds
   SI:DI Callback address, SEG:OFF
 Exit:
   BX:CX:   Timer address, SEG:OFF
   AX:      Error code: 0 OK, NO_SPACE if out of memory

TIMERSET:      set a timer
 Entry:
   AH:      API_TIMERSET
   BX:      Timer address offset
   CX:DX Timer period in milliseconds
 Exit:
   AX:      Error code: 0 OK

TIMERSTART     start a timer
 Entry:
   AH:      API_TIMERSTART
   BX:      Timer address offset
 Exit:
   AX:      Error code: 0 OK

TIMERSTOP      stop a timer
 Entry:
   AH:      API_TIMERSTOP
   BX:      Timer address offset
 Exit:
   AX:      Error code: 0 OK

TIMERDELETE    delete a timer, free memory used by timer
 Entry:
   AH:      API_TIMERDELETE
   BX:      Timer address offset
 Exit:
   AX:      Error code: 0 OK


                      TCP Connection State Diagram

                              +---------+ ---------\      active OPEN
                              |  CLOSED |            \    -----------
                              +---------+<---------\   \   create TCB
                                |     ^              \   \  snd SYN
                   passive OPEN |     |   CLOSE        \   \
                   ------------ |     | ----------       \   \
                    create TCB  |     | delete TCB         \   \
                                V     |                      \   \
                              +---------+            CLOSE    |    \
                              |  LISTEN |          ---------- |     |
                              +---------+          delete TCB |     |
                   rcv SYN      |     |     SEND              |     |
                  -----------   |     |    -------            |     V
 +---------+      snd SYN,ACK  /       \   snd SYN          +---------+
 |         |<-----------------           ------------------>|         |
 |   SYN_  |                    rcv SYN                     |   SYN_  |
 |RECEIVED |<-----------------------------------------------|   SENT  |
 |         |                    snd ACK                     |         |
 |         |------------------           -------------------|         |
 +---------+   rcv ACK of SYN  \       /  rcv SYN,ACK       +---------+
   |           --------------   |     |   -----------
   |                  x         |     |     snd ACK
   |                            V     V
   |  CLOSE                 +-------------+
   | -------                | ESTABLISHED |
   | snd FIN                +-------------+
   |                   CLOSE    |     |    rcv FIN
   V                  -------   |     |    -------
 +---------+          snd FIN  /       \   snd ACK          +---------+
 |FINWAIT1 |<-----------------           ------------------>| CLOSE_  |
 |         |------------------                              |  WAIT   |
 +---------+          rcv FIN  \                            +---------+
   | rcv ACK of FIN   -------   |                            CLOSE  |
   | --------------   snd ACK   |                           ------- |
   V        x                   V                           snd FIN V
 +---------+                  +---------+                   +---------+
 |FINWAIT2 |                  | CLOSING |                   | LAST_ACK|
 +---------+                  +---------+                   +---------+
   |                rcv ACK of FIN |                 rcv ACK of FIN |
   |  rcv FIN       -------------- |    Timeout=2MSL -------------- |
   |  -------              x       V    ------------        x       V
    \ snd ACK                 +---------+ delete TCB        +---------+
     ------------------------>|TIME_WAIT|------------------>| CLOSED  |
                              +---------+                   +---------+
*/

/*
 * Local Variables:
 * tab-width:3
 * End:
 */

/*  EOF: api.h */
#endif //_API_H

