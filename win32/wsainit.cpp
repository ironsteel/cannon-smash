/***********************************************************************
 *	d:/users/wata/src/ese/src/std/wsainit.cpp
 *	$Id: wsainit.cpp,v 1.2 2002/03/05 14:22:08 yotsuya Exp $
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
#include "minwin32.h"
#include <winsock2.h>

#define MAJOR 2
#define MINOR 0

namespace {
    char __rcsid__[] = "$Id: wsainit.cpp,v 1.2 2002/03/05 14:22:08 yotsuya Exp $";

    class wsainit
    {
    private:
	static bool initialized;
    public:
	inline wsainit(int major, int minor) {
	    if (initialized) return;
	    WSADATA wsadata;
	    if (WSAStartup(MAKEWORD(major,minor), &wsadata)) {
		initialized = true;
	    }
	}
	inline ~wsainit() {
	    if (initialized) {
		WSACleanup();
		initialized = false;
	    }
	}
    };

    bool wsainit::initialized = false;
    wsainit __wsainit__(MAJOR, MINOR); /* constructor will run before main */
}
