/***********************************************************************
 *	d:/users/wata/src/ese/src/std/GetArgs.cpp
 *	$Id: GetArgs.cpp,v 1.3 2002/03/05 14:22:08 yotsuya Exp $
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
#include <stdlib.h>
#include <malloc.h>

#include "GetArgs.h"

static int get_escape_char()
{
    char s[1];
    if (GetEnvironmentVariable("SHLVL", s, 1) /* csh */
	|| GetEnvironmentVariable("_", s, 1)  /* sh  */ ) {
	return GETARGS_ESCAPE_UNIX;
    } else {
	return GETARGS_ESCAPE_CMD;
    }
}

static inline int isquote_char(int c)
{
    if ('"' == c /* || '\'' == c */)
	return 1;
    else
	return 0;
}

int GetArgs(int *pargc, char ***pargv, const char *cmdline_, int esc)
{
    if (NULL == cmdline_) cmdline_ = GetCommandLine();
    if (0 == esc) esc = get_escape_char();

    char *cmdline = (char *)calloc(strlen(cmdline_) + 2, sizeof(char));
    char **argv  = (char **)calloc(1024, sizeof(char*));
    char *cur = (char*)cmdline_, *p = cmdline, *last;
    int argc = 0, quote_char = 0, c;
    int first = 1;

    do {
	c = *cur++;
	if (!quote_char) {
	    if (esc == c) {
		if (first) {
		    if (GETARGS_ESCAPE_UNIX == esc) {
			*p++ = '/';
		    } else {
			*p++ = c;
		    }
		} else {
		    c = *cur++;
		    *p++ = c;
		}
	    }
	    else if (isquote_char(c)) {
		quote_char = c;
	    }
	    else if (' ' == c || '\t' == c) {
		first = 0;
		*p++ = '\0';
	    } else {
		*p++ = c;
	    }
	} else {
	    if (c == quote_char) {
		quote_char = 0;
	    } else {
		*p++ = c;
	    }
	}
    } while ('\0' != *cur);
    last = p;

// if (0 != quote_char), there is unterminated quote(s)

    cur = cmdline;
    do {
	for (;'\0' == *cur && last > cur; cur++);
	if (!(last > cur)) break;
	argv[argc++] = cur;

	for (;'\0' != *cur && last > cur; cur++);
    } while ((last > cur) && (argc < 1024));
    argv[argc] = NULL;
    *pargv = argv;
    *pargc = argc;

    if (quote_char) {
//	fprintf(stderr, "Unmached quote %c\n", quote_char);
	return quote_char;
    } else {
  	return 0;
    }
}
