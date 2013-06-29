/* $Id: RCFile.h,v 1.5 2003/07/28 16:56:15 nan Exp $ */

// Copyright (C) 2001, 2003  ¿ÀÆî µÈ¹¨(Kanna Yoshihiro)
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

#ifndef _RCFile_
#define _RCFile_

#include "ttinc.h"

class RCFile {
public:
  static RCFile *GetRCFile();

  bool isTexture;
  bool fullScreen;
  long myModel;
  long gmode;
  long gameLevel;
  long gameMode;
  long sndMode;
  long protocol;

  char serverName[256];
  char nickname[32];
  char message[64];

  short csmash_port;

  bool ReadRCFile();
  bool WriteRCFile();

private:
  RCFile();

  static RCFile* m_rcFile;

  FILE * OpenRCFile( char *mode );
};

#endif	// _RCFile_
