/* $Id: MultiPlayerSelect.h,v 1.4 2003/08/17 11:17:10 nan Exp $ */

// Copyright (C) 2003  神南 吉宏(Kanna Yoshihiro)
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

#ifndef _MultiPlayerSelect_
#define _MultiPlayerSelect_

#include "PlayerSelect.h"
#include "PracticeSelectView.h"

class MultiPlayerSelect : public PlayerSelect {
public:
  MultiPlayerSelect();
  virtual ~MultiPlayerSelect();

  virtual bool Init();

  static void Create();

  virtual bool Move( SDL_keysym *KeyHistory, long *MouseXHistory,
		     long *MouseYHistory, unsigned long *MouseBHistory,
		     int Histptr );

  virtual long GetOpponentRotate() { return m_opponentRotate; };
  virtual long GetOpponentNum();
  virtual long GetOpponentSelected() { return m_opponentSelected; };

  static int Connect( void *dum );

  void ReadPT( char *data );
  void SendPT( char fixed );

protected:
  long m_opponentRotate;	// Rotation of players
  long m_opponentSelected;	// Selected : m_selected > 0


  long m_lastRotate;
  long m_lastOpponentRotate;

  bool m_isConnected;
};

#endif	// _MultiPlayerSelect_
