//
//   $Id: ALLOCDEF.H 1.10 2012/01/04 22:19:04Z johnb Exp $
//
//   *** DO NOT REMOVE THE FOLLOWING NOTICE ***
//
//   Copyright (c) 1993 - 2012 Datalight, Inc.
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
// IDs for allocation checking

#ifndef _ALLOCDEF_H
#define _ALLOCDEF_H
enum {
AllocApiDcSockets,
AllocApiSockets,
AllocApiAlarmTimer,
AllocApiUCB,
AllocApiUCQ,
AllocApiTimer,
AllocApiSndBufTc,
AllocApiSndBufUc,
AllocApiSndBufT,
AllocApiSndBufU,
AllocArpArp,
AllocArpMac,
AllocArpRif,
AllocArpSnd,
AllocArpcMac,
AllocAsyRbuf,
AllocAsyIbuf,
AllocAsyIbuf1,
AllocAsyIbuf2,
AllocAsyIbuf3,
AllocAsyIbuf4,
AllocBootpSnd,
AllocBootpFile,
AllocBootpSname,
AllocBootpMbuf,
AllocComUser,
AllocDnsServ,
AllocDnsSufx,
AllocDnsMsg,
AllocDnsSnd,
AllocDnsRequest,
AllocEnetEth,
AllocEnetIeee,
AllocEnetToken,
AllocFilIoFil,
AllocIcmpPing,
AllocIcmpSnd,
AllocIcmpBuf,
AllocIcmpData,
AllocIcmpcSnd,
AllocIfuncEnv,
AllocIgmpGa,
AllocIgmpSnd,
AllocIpRa,
AllocIpFrag,
AllocIpRoute,
AllocIpFilter,
AllocIpFilterRe,
AllocIpFragSnd,
AllocIpSnd,
AllocIpMc,
AllocIpLb,
AllocLpdNm,
AllocLpdCon,
AllocLpdSndAck,
AllocLpdNprintf,
AllocMainInbf,
AllocModCom,
AllocModPar,
AllocNetfIpA,
AllocPcs100,
AllocPktBuf,
AllocPktiIf,
AllocPktiIfNm,
AllocPktiMac,
AllocPktiSbuf,
AllocPktiStack,
AllocPppHdr,
AllocPppDup,
AllocPppLopt,
AllocPppIopt,
AllocPppAsnd,
AllocPppChal,
AllocPppUsr,
AllocPppPass,
AllocPppMsg,
AllocPppRMsg,
AllocPppRChal,
AllocPppMd5,
AllocPppPeer,
AllocPppAMsg,
AllocPppFSnd,
AllocPppPeerName,
AllocPppApMsg,
AllocPppcPpp,
AllocPppcLcp,
AllocPppcAp,
AllocPppcIpcp,
AllocPppcUser,
AllocPppcPass,
AllocPrnStat,
AllocPrnCon,
AllocPrnSnd,
AllocRipSnd,
AllocRipSnd1,
AllocRipSnd2,
AllocSifSer,
AllocSifIf,
AllocSifNm,
AllocSlhcComp,
AllocSlhcRState,
AllocSlhcTState,
AllocSlhcDeltaS,
AllocSlhcDeltaS1,
AllocSlipDup,
AllocSlipBuf,
AllocSlipbCrc,
AllocTcpRcv,
AllocTcpTcb,
AllocTcpHdr,
AllocTcpLTcb,
AllocTcpRCom,
AllocTcpReseq,
AllocTcpTrim,
AllocTcpTrimFin,
AllocTcpKa,
AllocTermSnd,
AllocUdpUdp,
AllocUdpDup,
AllocUdpSock,
AllocUdpHdr,
AllocIcmp6LocalIpAd,
AllocIcmp6SndNa,
AllocIcmp6Buf,
AllocIcmp6Data,
AllocIcmp6SndRs,
AllocIcmp6SndNs,
AllocIcmp6HtoN,
AllocIcmp6SndMld,
AllocIcmp6McIpAd,
AllocIp6Nc,
AllocIp6Dc,
AllocIp6Pl,
AllocIp6Dr,
AllocIp6Q,
AllocIp6FragSnd,
AllocIp6Lb,
AllocIp6Mc,
AllocIp6Snd,
AllocIp6ExtHdr,
AllocRcsSend,
AllocRcsData,
AllocRcsiniUpw,
// For Inet
AllocMainIni,
AllocMainSessions,
AllocSchedTasks,
AllocComAltf,
AllocComArgs,
AllocComTimer,
AllocComTimerArg,
AllocImenMenu,
AllocImenTitle,
AllocTwinWin,
AllocTnserTelnet,
AllocTnserDhses,
AllocTnserRldat0,
AllocTnserRldat1,
AllocTnserDat,
AllocTnibmDat,
AllocTnibmTelnet,
AllocTnremData,
AllocTnremCr,
AllocTnremTelnet,
AllocTnremDifdat,
AllocTnremCursdat,
AllocPrntsData,
AllocLpdLogfile,
AllocLpdDatfile,
AllocLpdBuf,
AllocLpdLpd,
AllocLpdFile,
AllocLpdAck,
AllocLpdPrdat,
AllocTnrlName,
AllocTnrlData,
AllocTnrlFf,
AllocTnrl200resp,
AllocTnrlDat1,
AllocTnrlTelnet,
AllocTeltTelnet,
AllocTeltBuf,
AllocTeltTeldef,
AllocTelnTtnames,
AllocTelnTtnamesRe,
AllocTelnPlog,
AllocTelnHname,
AllocTelnTelnet,
AllocTelnCusr,
AllocTelnSusr,
AllocTelnDat1,
AllocTelnDat2,
AllocTelnDat3,
AllocTelnDat4,
AllocTelnDat5,
AllocTelnPrDat,
AllocTelnRlDat1,
AllocTelnTnDat1,
AllocTelnRlDat2,
AllocTelnTnDat2,
AllocTelnTnDat3,
AllocTelnTnDat4,
AllocTelnDat81,
AllocTestName,
AllocTestTelnet,
AllocTestData,
AllocUteEmname,
AllocUteLuName,
AllocUteFname,
AllocUteEmArg,
AllocUteRtm,
AllocUteEmName1,
AllocUteRtmcb,
AllocUteData,
AllocSmiscTimer,
AllocSnmpRecv,
AllocFtpsEof,
AllocFtpsRawDat,
AllocFtpsUser,
AllocFtpsFile,
AllocFtpsDirec,
AllocFtpsPath,
AllocFtpcHname,
AllocFtpcLbuf,
AllocFtpcStr,
AllocFtpcLdup,
AllocFtpcDispdat,
AllocFtpcLname,
AllocFtpcBuf35,
AllocFtpcMsg,
AllocFtpSnd,
AllocFtpFtp,
AllocFtpBuf,
AllocTcpTprintf,
AllocUdpcBuf,
AllocUdpcBuf1,
AllocIcmpcXping,
AllocIcmpcPing,
AllocCrctTab,
AllocPppVjc,
AllocSifBuf,
AllocDumIface,
AllocDumName,
AllocPathname,
AllocDiruPath,
AllocDiruSort,
AllocDiruFblock,
AllocDiruCommas,
AllocAsyIbuf0,
AllocTraceDup,
AllocTraceDup1,
AllocPc100,
AllocHttpcHttp,
AllocHttpsData,
AllocHttpsHttp,
AllocSlipbBuf,
AllocLapbX25,
AllocLapbFrmr,
AllocLapbData,
AllocLapbCf,
AllocLapbtRexmit,
AllocLcsLcs,
AllocLcsIf,
AllocLcsName,
AllocLcsDat,
AllocDigiChan,
AllocDigiIf,
AllocDigiName,
AllocDigiBuf,
AllocAsyDatab,
AllocAsyDatab1,
AllocAsyDatab2,
AllocX25Rbuf,
AllocX25If,
AllocX25Name,
AllocX25Cb,
AllocMkName,
AllocAllocLib,
};

#ifdef _DIST_ALLOC
char _DIST_ALLOC szAlloc[] = {   // AllocMessages
   "ApiDcSocks"
   "ApiSockets"
   "ApiAlrmTim"
   "ApiUCB    "
   "ApiUCQ    "
   "ApiTimer  "
   "ApiSndBfTc"
   "ApiSndBfUc"
   "ApiSndBufT"
   "ApiSndBufU"
   "ArpArp    "
   "ArpMac    "
   "ArpRif    "
   "ArpSnd    "
   "ArpcMac   "
   "AsyRbuf   "
   "AsyIbuf   "
   "AsyIbuf1  "
   "AsyIbuf2  "
   "AsyIbuf3  "
   "AsyIbuf4  "
   "BootpSnd  "
   "BootpFile "
   "BootpSname"
   "BootpMbuf "
   "ComUser   "
   "DnsServ   "
   "DnsSufx   "
   "DnsMsg    "
   "DnsSnd    "
   "DnsRequest"
   "EnetEth   "
   "EnetIeee  "
   "EnetToken "
   "FilIoFil  "
   "IcmpPing  "
   "IcmpSnd   "
   "IcmpBuf   "
   "IcmpData  "
   "IcmpcSnd  "
   "IfuncEnv  "
   "IgmpGa    "
   "IgmpSnd   "
   "IpRa      "
   "IpFrag    "
   "IpRoute   "
   "IpFilter  "
   "IpFilterRe"
   "IpFragSnd "
   "IpSnd     "
   "IpMc      "
   "IpLb      "
   "LpdNm     "
   "LpdCon    "
   "LpdSndAck "
   "LpdNprintf"
   "MainInbf  "
   "ModCom    "
   "ModPar    "
   "NetfIpA   "
   "Pcs100    "
   "PktBuf    "
   "PktiIf    "
   "PktiIfNm  "
   "PktiMac   "
   "PktiSbuf  "
   "PktiStack "
   "PppHdr    "
   "PppDup    "
   "PppLopt   "
   "PppIopt   "
   "PppAsnd   "
   "PppChal   "
   "PppUsr    "
   "PppPass   "
   "PppMsg    "
   "PppRMsg   "
   "PppRChal  "
   "PppMd5    "
   "PppPeer   "
   "PppAMsg   "
   "PppFSnd   "
   "PppPeerNme"
   "PppApMsg  "
   "PppcPpp   "
   "PppcLcp   "
   "PppcAp    "
   "PppcIpcp  "
   "PppcUser  "
   "PppcPass  "
   "PrnStat   "
   "PrnCon    "
   "PrnSnd    "
   "RipSnd    "
   "RipSnd1   "
   "RipSnd2   "
   "SifSer    "
   "SifIf     "
   "SifNm     "
   "SlhcComp  "
   "SlhcRState"
   "SlhcTState"
   "SlhcDeltaS"
   "SlhcDltaS1"
   "SlipDup   "
   "SlipBuf   "
   "SlipbCrc  "
   "TcpRcv    "
   "TcpTcb    "
   "TcpHdr    "
   "TcpLTcb   "
   "TcpRCom   "
   "TcpReseq  "
   "TcpTrim   "
   "TcpTrimFin"
   "TcpKa     "
   "TermSnd   "
   "UdpUdp    "
   "UdpDup    "
   "UdpSock   "
   "UdpHdr    "
   "Icmp6LIpAd"
   "Icmp6SndNa"
   "Icmp6Buf  "
   "Icmp6Data "
   "Icmp6SndRs"
   "Icmp6SndNs"
   "Icmp6HtoN "
   "Icmp6SndMl"
   "Icmp6McIpA"
   "Ip6Nc     "
   "Ip6Dc     "
   "Ip6Pl     "
   "Ip6Dr     "
   "Ip6Q      "
   "Ip6FragSnd"
   "Ip6Lb     "
   "Ip6Mc     "
   "Ip6Snd    "
   "Ip6ExtHdr "
   "RcsSend   "
   "RcsData   "
   "RcsiniUpw "
#if APL==1
   "MainIni   "
   "MainSessns"
   "SchedTasks"
   "ComAltf   "
   "ComArgs   "
   "ComTimer  "
   "ComTimrArg"
   "ImenMenu  "
   "ImenTitle "
   "TwinWin   "
   "TnserTelnt"
   "TnserDhses"
   "TnserRldt0"
   "TnserRldt1"
   "TnserDat  "
   "TnibmDat  "
   "TnibmTelnt"
   "TnremData "
   "TnremCr   "
   "TnremTelnt"
   "TnremDifdt"
   "TnremCursd"
   "PrntsData "
   "LpdLogfile"
   "LpdDatfile"
   "LpdBuf    "
   "LpdLpd    "
   "LpdFile   "
   "LpdAck    "
   "LpdPrdat  "
   "TnrlName  "
   "TnrlData  "
   "TnrlFf    "
   "Tnrl200rsp"
   "TnrlDat1  "
   "TnrlTelnet"
   "TeltTelnet"
   "TeltBuf   "
   "TeltTeldef"
   "TelnTtnams"
   "TelnTtnmsR"
   "TelnPlog  "
   "TelnHname "
   "TelnTelnet"
   "TelnCusr  "
   "TelnSusr  "
   "TelnDat1  "
   "TelnDat2  "
   "TelnDat3  "
   "TelnDat4  "
   "TelnDat5  "
   "TelnPrDat "
   "TelnRlDat1"
   "TelnTnDat1"
   "TelnRlDat2"
   "TelnTnDat2"
   "TelnTnDat3"
   "TelnTnDat4"
   "TelnDat81 "
   "TestName  "
   "TestTelnet"
   "TestData  "
   "UteEmname "
   "UteLuName "
   "UteFname  "
   "UteEmArg  "
   "UteRtm    "
   "UteEmName1"
   "UteRtmcb  "
   "UteData   "
   "SmiscTimer"
   "SnmpRecv  "
   "FtpsEof   "
   "FtpsRawDat"
   "FtpsUser  "
   "FtpsFile  "
   "FtpsDirec "
   "FtpsPath  "
   "FtpcHname "
   "FtpcLbuf  "
   "FtpcStr   "
   "FtpcLdup  "
   "FtpcDispdt"
   "FtpcLname "
   "FtpcBuf35 "
   "FtpcMsg   "
   "FtpSnd    "
   "FtpFtp    "
   "FtpBuf    "
   "TcpTprintf"
   "UdpcBuf   "
   "UdpcBuf1  "
   "IcmpcXping"
   "IcmpcPing "
   "CrctTab   "
   "PppVjc    "
   "SifBuf    "
   "DumIface  "
   "DumName   "
   "Pathname  "
   "DiruPath  "
   "DiruSort  "
   "DiruFblock"
   "DiruCommas"
   "AsyIbuf0  "
   "TraceDup  "
   "TraceDup1 "
   "Pc100     "
   "HttpcHttp "
   "HttpsData "
   "HttpsHttp "
   "SlipbBuf  "
   "LapbX25   "
   "LapbFrmr  "
   "LapbData  "
   "LapbCf    "
   "LapbtRexmt"
   "LcsLcs    "
   "LcsIf     "
   "LcsName   "
   "LcsDate   "
   "DigiChan  "
   "DigiIf    "
   "DigiName  "
   "DigiBuf   "
   "AsyDatab  "
   "AsyDatab1 "
   "AsyDatab2 "
   "X25Rbuf   "
   "X25If     "
   "X25Name   "
   "X25Cb     "
   "MkName    "
   "AllocLib  "
#endif
};
#endif
#endif
