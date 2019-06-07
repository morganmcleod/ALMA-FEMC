/* $OpenBSD: netdb.h,v 1.18 2003/06/02 19:34:12 millert Exp $  */

/*
 * Copyright (C) 1995, 1996, 1997, and 1998 WIDE Project.
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the project nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE PROJECT AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE PROJECT OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/*
 * ++Copyright++ 1980, 1983, 1988, 1993
 * -
 * Copyright (c) 1980, 1983, 1988, 1993
 * The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 * -
 * Portions Copyright (c) 1993 by Digital Equipment Corporation.
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies, and that
 * the name of Digital Equipment Corporation not be used in advertising or
 * publicity pertaining to distribution of the document or software without
 * specific, written prior permission.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND DIGITAL EQUIPMENT CORP. DISCLAIMS ALL
 * WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS.   IN NO EVENT SHALL DIGITAL EQUIPMENT
 * CORPORATION BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 * -
 * --Copyright--
 */

/*
 * Copyright (c) 1995, 1996, 1997, 1998, 1999 Craig Metz. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the author nor the names of any contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/*
 *      @(#)netdb.h  8.1 (Berkeley) 6/2/93
 * $From: netdb.h,v 8.7 1996/05/09 05:59:09 vixie Exp $
 */

#ifndef _NETDB_H_
#define _NETDB_H_

//#include <sys/param.h>
#if (!defined(BSD)) || (BSD < 199306)
//# include <sys/bitypes.h>
#endif
//#include <sys/cdefs.h>

#define  _PATH_HEQUIV   "/etc/hosts.equiv"
#define  _PATH_HOSTS "/etc/hosts"
#define  _PATH_NETWORKS "/etc/networks"
#define  _PATH_PROTOCOLS   "/etc/protocols"
#define  _PATH_SERVICES "/etc/services"

extern int h_errno;

/*
 * Error return codes from gethostbyname() and gethostbyaddr()
 * (left in extern int h_errno).
 */

#define  NETDB_INTERNAL -1 /* see errno */
#define  NETDB_SUCCESS  0  /* no problem */

/* Values for getaddrinfo() and getnameinfo() */
#define AI_PASSIVE   1  /* socket address is intended for bind() */
#define AI_CANONNAME 2  /* request for canonical name */
#define AI_NUMERICHOST  4  /* don't ever try nameservice */
/* valid flags for addrinfo */
#define AI_MASK      (AI_PASSIVE | AI_CANONNAME | AI_NUMERICHOST)

#define NI_NUMERICHOST  1  /* return the host address, not the name */
#define NI_NUMERICSERV  2  /* return the service address, not the name */
#define NI_NOFQDN 4  /* return a short name if in the local domain */
#define NI_NAMEREQD  8  /* fail if either host or service name is unknown */
#define NI_DGRAM  16 /* look up datagram service instead of stream */
#if 0 /* obsolete */
#define NI_WITHSCOPEID  32 /* KAME hack: attach scopeid to host portion */
#endif

#define NI_MAXHOST   MAXHOSTNAMELEN /* max host name returned by getnameinfo */
#define NI_MAXSERV   32 /* max serv. name length returned by getnameinfo */

/*
 * Scope delimit character (KAME hack)
 */
#define SCOPE_DELIMITER '%'

#define EAI_BADFLAGS -1 /* invalid value for ai_flags */
#define EAI_NONAME   -2 /* name or service is not known */
#define EAI_AGAIN -3 /* temporary failure in name resolution */
#define EAI_FAIL  -4 /* non-recoverable failure in name resolution */
/* #define EAI_NODATA   -5 (obsoleted) */
#define EAI_FAMILY   -6 /* ai_family not supported */
#define EAI_SOCKTYPE -7 /* ai_socktype not supported */
#define EAI_SERVICE  -8 /* service not supported for ai_socktype */
/* #define EAI_ADDRFAMILY  (obsoleted) */
#define EAI_MEMORY   -10   /* memory allocation failure */
#define EAI_SYSTEM   -11   /* system error (code indicated in errno) */
#define EAI_BADHINTS -12   /* invalid value for hints */
#define EAI_PROTOCOL -13   /* resolved protocol is unknown */

/*
 * Flags for getrrsetbyname()
 */
#define RRSET_VALIDATED    1

/*
 * Return codes for getrrsetbyname()
 */
#define ERRSET_SUCCESS     0
#define ERRSET_NOMEMORY    1
#define ERRSET_FAIL     2
#define ERRSET_INVAL    3
#define ERRSET_NONAME      4
#define ERRSET_NODATA      5

/*
 * Structures used by getrrsetbyname() and freerrset()
 */
struct rdatainfo {
   unsigned int      rdi_length; /* length of data */
   unsigned char     *rdi_data;  /* record data */
};

struct rrsetinfo {
   unsigned int      rri_flags;  /* RRSET_VALIDATED ... */
   unsigned int      rri_rdclass;   /* class number */
   unsigned int      rri_rdtype; /* RR type number */
   unsigned int      rri_ttl; /* time to live */
   unsigned int      rri_nrdatas;   /* size of rdatas array */
   unsigned int      rri_nsigs;  /* size of sigs array */
   char        *rri_name;  /* canonical name */
   struct rdatainfo  *rri_rdatas;   /* individual records */
   struct rdatainfo  *rri_sigs;  /* individual signatures */
};


#endif /* !_NETDB_H_ */
