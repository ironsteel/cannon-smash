/***********************************************************************
 *	d:/users/wata/src/ese/src/std/wsaerror.h
 *	$Id: wsaerror.h,v 1.2 2002/12/22 15:42:54 yotsuya Exp $
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
#ifndef __wata_ESESoft_0308__wsaerror_h__INCLUDED__
#define __wata_ESESoft_0308__wsaerror_h__INCLUDED__
/***********************************************************************/
/* __BEGIN__BEGIN__ */
#ifndef __cplusplus
/* This is C code */
const char *wsaerrorstring(int err);

#else
/* This is C++ code */
extern "C" const char *wsaerrorstring(int err);

class wsa_error
{
public:
    int rr;

    inline explicit wsa_error(int rr) throw() : rr(rr) {}
    inline wsa_error() throw() : rr(WSAGetLastError()) {}
};

/*__napi__*/ std::ostream & operator <<(std::ostream &, const wsa_error &);

#endif

/* __END__END__ */
/***********************************************************************/
#endif
/***********************************************************************
 *	END OF wsaerror.h
 ***********************************************************************/
