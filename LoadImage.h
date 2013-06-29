/* $Id: LoadImage.h,v 1.3 2002/12/23 16:36:44 nan Exp $ */

// Copyright (C) 2000  神南 吉宏(Kanna Yoshihiro)
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

#ifndef _LoadImage_
#define _LoadImage_

class ImageData {
public:
  ImageData();
  ImageData( long width, long height, long bytes );
  virtual ~ImageData();

  GLubyte *GetImage() {return m_image;}

  long GetWidth()  {return m_width;}
  long GetHeight() {return m_height;}
  long GetBytes()  {return m_bytes;}
  GLubyte GetPixel( long width, long height, long bytes );
  bool SetPixel( long width, long height, long bytes, GLubyte val );

  bool LoadFile(const char *filename);
  bool LoadPPM(const char* filename );
  bool LoadJPG(const char* filename);

protected:
  GLubyte *m_image;
  long m_width;
  long m_height;
  long m_bytes;
};

#ifdef HAVE_LIBZ
char* getWord(gzFile fp);
#else
char* getWord( FILE *fp );
#endif

SDL_Surface* SDL_GL_LoadTexture(char *filename);

#endif	// _LoadImage_
