/***********************************************************************
 *	d:/users/wata/src/include/minwin32.h
 *	$Id: minwin32.h,v 1.3 2002/03/05 14:22:08 yotsuya Exp $
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
#ifndef __wata_ESESoft__minwin32_h__INCLUDED_
#define __wata_ESESoft__minwin32_h__INCLUDED_
/***********************************************************************/
/* __BEGIN__BEGIN__ */
#ifdef __cplusplus
#ifndef NOMINMAX
#define NOMINMAX
#endif
#endif

#define NOGDI
#define NONLS
#define NOUSER
#define NOSOUND
#define NOKERNEL
#define NOSERVICE
#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN

#include <windows.h>

#ifndef snprintf
# define snprintf _snprintf
# define vsnprintf _vsnprintf
#endif

/* __END__END__ */
/***********************************************************************/
#endif
/***********************************************************************
 *	END OF minwin32.h
 ***********************************************************************/
