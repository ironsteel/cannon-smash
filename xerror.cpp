/***********************************************************************
 *	d:/Werk/src/csmash-0.4.0/xerror.cpp
 *	$Id: xerror.cpp,v 1.5 2002/11/10 15:11:48 nan Exp $
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
#include "ttinc.h"

#include <stdio.h>
#include <errno.h>
#include <stdarg.h>
#include <string.h>

#ifdef WIN32
#if defined (__CYGWIN__) || defined(__MINGW32__)
#else
#include <iostream>
#include "win32/wsaerror.h"
#endif
#endif

void do_xerror(const char *string, va_list va)
{
#ifdef WIN32
#if defined (__CYGWIN__) || defined(__MINGW32__)
#else
    DWORD err = WSAGetLastError();
    WSASetLastError(0);
    if (0 != err) {
	// WSAError!
	fputs(wsaerrorstring(err), stderr);
    } else 
#endif
#endif
      fputs(strerror(errno), stderr);

    fputs(": ", stderr);
    vfprintf(stderr, string, va);
    fputc('\n', stderr);
    fflush(stderr);
}

void xerror(const char *string, ...)
{
    va_list va;
    va_start(va, string);
    do_xerror(string, va);
    va_end(va);
}

/***********************************************************************
 *	END OF xerror.cpp
 ***********************************************************************/
