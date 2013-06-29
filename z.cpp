/***********************************************************************
 * n:/Werk/src/csmash-0.3.3/z.cpp
 * $Id: z.cpp,v 1.4 2002/11/11 13:59:47 nan Exp $
 *
 * Copyright (C) 2000  渡辺 正泰(Watanabe Masahiro)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 ***********************************************************************/
#include "ttinc.h"
#ifdef HAVE_LIBZ

gzFile gzopenx(const char *filename, const char *mode) {
  int fd, m = 0;

#ifdef _WIN32
  m |= O_BINARY;
#ifndef __MINGW32__
  if (NULL != strchr(mode, 's')) {
    m |= O_SEQUENTIAL;
  }
#endif
#endif
  if (NULL != strchr(mode, 'a')) {
    m |= O_APPEND;
  }

  bool r = false, w = false, plus = false; 
  if (NULL != strchr(mode, 'r')) {
    r = true;
  }
  if (NULL != strchr(mode, 'w')) {
    w = true;
  }
  if (NULL != strchr(mode, '+')) {
    plus = true;
  }

  if (plus) {
    if (!w) {
      m |= O_RDWR;
    } else {
      m |= O_RDWR | O_CREAT | O_TRUNC;
    }
  } else if (r) {
    m |= O_RDONLY;
  } else if (w) {
    m |= O_WRONLY | O_CREAT | O_TRUNC;
  }

  char buf[512];
  strncpy(buf, filename, 512);
  fd = open(buf, m, S_IREAD|S_IWRITE);
  if (0 > fd) {
    strncat(buf, ".gz", 512);
    fd = open(buf, m, S_IREAD|S_IWRITE);
  }
  gzFile zfp = gzdopen(fd, mode);
  return zfp;
}
#endif
/***********************************************************************
 * END OF z.cpp
 ***********************************************************************/
