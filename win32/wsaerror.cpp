/***********************************************************************
 *	d:/users/wata/src/ese/src/std/wsaerror.cc
 *	$Id: wsaerror.cpp,v 1.2 2002/03/05 14:22:08 yotsuya Exp $
 *
 *	Copyright by ESESoft.
 *
 *	Redistribution and use in source and binary forms, with or without
 *	modification, are permitted provided that the following conditions
 *	are met:
 *
 *	Redistributions of source code must retain the above copyright
 *	notice, this list of conditions and the following disclaimer. 
 *
 *	Redistributions in binary form must reproduce the above copyright
 *	notice, this list of conditions and the following disclaimer
 *	in the documentation and/or other materials provided with the
 *	distribution. 
 *
 *	The name of the author may not be used to endorse or promote
 *	products derived from this software without specific prior written
 *	permission. 
 *
 *	THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 *	OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *	WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *	ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 *	DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *	DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 *	GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *	INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 *	WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *	NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *	SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ***********************************************************************/
#include <iostream>

#include "minwin32.h"
#include <winsock2.h>

#include "wsaerror.h"

static const char *undefined = "WSAERROR_UNDEFINED";
static const char *errors100[] =
{
    undefined,				/* 0 */
    undefined,				/* 1 */
    undefined,				/* 2 */
    undefined,				/* 3 */
    "WSAEINTR",				/* 4 */
    undefined,				/* 5 */
    undefined,				/* 6 */
    undefined,				/* 7 */
    undefined,				/* 8 */
    "WSAEBADF",				/* 9 */
    undefined,				/* 10 */
    undefined,				/* 11 */
    undefined,				/* 12 */
    "WSAEACCES",			/* 13 */
    "WSAEFAULT",			/* 14 */
    undefined,				/* 15 */
    undefined,				/* 16 */
    undefined,				/* 17 */
    undefined,				/* 18 */
    undefined,				/* 19 */
    undefined,				/* 20 */
    undefined,				/* 21 */
    "WSAEINVAL",			/* 22 */
    undefined,				/* 23 */
    "WSAEMFILE",			/* 24 */
    undefined,				/* 25 */
    undefined,				/* 26 */
    undefined,				/* 27 */
    undefined,				/* 28 */
    undefined,				/* 29 */
    undefined,				/* 30 */
    undefined,				/* 31 */
    undefined,				/* 32 */
    undefined,				/* 33 */
    undefined,				/* 34 */
    "WSAEWOULDBLOCK",			/* 35 */
    "WSAEINPROGRESS",			/* 36 */
    "WSAEALREADY",			/* 37 */
    "WSAENOTSOCK",			/* 38 */
    "WSAEDESTADDRREQ",			/* 39 */
    "WSAEMSGSIZE",			/* 40 */
    "WSAEPROTOTYPE",			/* 41 */
    "WSAENOPROTOOPT",			/* 42 */
    "WSAEPROTONOSUPPORT",		/* 43 */
    "WSAESOCKTNOSUPPORT",		/* 44 */
    "WSAEOPNOTSUPP",			/* 45 */
    "WSAEPFNOSUPPORT",			/* 46 */
    "WSAEAFNOSUPPORT",			/* 47 */
    "WSAEADDRINUSE",			/* 48 */
    "WSAEADDRNOTAVAIL",			/* 49 */
    "WSAENETDOWN",			/* 50 */
    "WSAENETUNREACH",			/* 51 */
    "WSAENETRESET",			/* 52 */
    "WSAECONNABORTED",			/* 53 */
    "WSAECONNRESET",			/* 54 */
    "WSAENOBUFS",			/* 55 */
    "WSAEISCONN",			/* 56 */
    "WSAENOTCONN",			/* 57 */
    "WSAESHUTDOWN",			/* 58 */
    "WSAETOOMANYREFS",			/* 59 */
    "WSAETIMEDOUT",			/* 60 */
    "WSAECONNREFUSED",			/* 61 */
    "WSAELOOP",				/* 62 */
    "WSAENAMETOOLONG",			/* 63 */
    "WSAEHOSTDOWN",			/* 64 */
    "WSAEHOSTUNREACH",			/* 65 */
    "WSAENOTEMPTY",			/* 66 */
    "WSAEPROCLIM",			/* 67 */
    "WSAEUSERS",			/* 68 */
    "WSAEDQUOT",			/* 69 */
    "WSAESTALE",			/* 70 */
    "WSAEREMOTE",			/* 71 */
    undefined,				/* 72 */
    undefined,				/* 73 */
    undefined,				/* 74 */
    undefined,				/* 75 */
    undefined,				/* 76 */
    undefined,				/* 78 */
    undefined,				/* 78 */
    undefined,				/* 79 */
    undefined,				/* 80 */
    undefined,				/* 81 */
    undefined,				/* 82 */
    undefined,				/* 83 */
    undefined,				/* 84 */
    undefined,				/* 85 */
    undefined,				/* 86 */
    undefined,				/* 87 */
    undefined,				/* 88 */
    undefined,				/* 89 */
    undefined,				/* 90 */
    "WSASYSNOTREADY",			/* 91 */
    "WSAVERNOTSUPPORTED",		/* 92 */
    "WSANOTINITIALISED",		/* 93 */
    undefined,				/* 94 */
    undefined,				/* 95 */
    undefined,				/* 96 */
    undefined,				/* 97 */
    undefined,				/* 98 */
    undefined,				/* 99 */
    undefined,				/* 100 */
    "WSAEDISCON",			/* 101 */
    "WSAENOMORE",			/* 102 */
    "WSAECANCELLED",			/* 103 */
    "WSAEINVALIDPROCTABLE",		/* 104 */
    "WSAEINVALIDPROVIDER",		/* 105 */
    "WSAEPROVIDERFAILEDINIT",		/* 106 */
    "WSASYSCALLFAILURE",		/* 107 */
    "WSASERVICE_NOT_FOUND",		/* 108 */
    "WSATYPE_NOT_FOUND",		/* 109 */
    "WSA_E_NO_MORE",			/* 110 */
    "WSA_E_CANCELLED",			/* 111 */
    "WSAEREFUSED",			/* 112 */
};

#define ERRORS100MAX (sizeof(errors100) / sizeof(const char*) - 1)

static const char *errors1000[] = {
    undefined,				/* 1000 */
    "WSAHOST_NOT_FOUND",		/* 1001 */
    "WSATRY_AGAIN",			/* 1002 */
    "WSANO_RECOVERY",			/* 1003 */
    "WSANO_DATA",			/* 1004 */
    "WSA_QOS_RECEIVERS",		/* 1005 */
    "WSA_QOS_SENDERS",			/* 1006 */
    "WSA_QOS_NO_SENDERS",		/* 1007 */
    "WSA_QOS_NO_RECEIVERS",		/* 1008 */
    "WSA_QOS_REQUEST_CONFIRMED",	/* 1009 */
    "WSA_QOS_ADMISSION_FAILURE",	/* 1010 */
    "WSA_QOS_POLICY_FAILURE",		/* 1011 */
    "WSA_QOS_BAD_STYLE",		/* 1012 */
    "WSA_QOS_BAD_OBJECT",		/* 1013 */
    "WSA_QOS_TRAFFIC_CTRL_ERROR",	/* 1014 */
    "WSA_QOS_GENERIC_ERROR",		/* 1015 */
    "WSA_QOS_ESERVICETYPE",		/* 1016 */
    "WSA_QOS_EFLOWSPEC",		/* 1017 */
    "WSA_QOS_EPROVSPECBUF",		/* 1018 */
    "WSA_QOS_EFILTERSTYLE",		/* 1019 */
    "WSA_QOS_EFILTERTYPE",		/* 1020 */
    "WSA_QOS_EFILTERCOUNT",		/* 1021 */
    "WSA_QOS_EOBJLENGTH",		/* 1022 */
    "WSA_QOS_EFLOWCOUNT",		/* 1023 */
    "WSA_QOS_EUNKOWNPSOBJ",		/* 1024 */
    "WSA_QOS_EPOLICYOBJ",		/* 1025 */
    "WSA_QOS_EFLOWDESC",		/* 1026 */
    "WSA_QOS_EPSFLOWSPEC",		/* 1027 */
    "WSA_QOS_EPSFILTERSPEC",		/* 1028 */
    "WSA_QOS_ESDMODEOBJ",		/* 1029 */
    "WSA_QOS_ESHAPERATEOBJ",		/* 1030 */
    "WSA_QOS_RESERVED_PETYPE",		/* 1031 */
};	

#define ERRORS1000MAX (sizeof(errors1000) / sizeof(const char*) + 1000 - 1)

extern "C" const char *wsaerrorstring(int err)
{
    err -= WSABASEERR;
    if (0 <= err && err <= ERRORS100MAX) return errors100[err];
    else if (1000 <= err && err <= ERRORS1000MAX) return errors1000[err-1000];
    else return undefined;
}

std::ostream & operator <<(std::ostream &s, const wsa_error &e)
{
    const char *err = wsaerrorstring(e.rr);
    if (undefined != err) {
	return s << wsaerrorstring(e.rr);
    } else {
	return s << wsaerrorstring(e.rr) << '(' << e.rr << ')';
    }
}

/***********************************************************************
 *	END OF wsaerror.cc
 ***********************************************************************/
