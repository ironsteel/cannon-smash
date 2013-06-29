/* $Id: PlayerSelectView2D.h,v 1.3 2001/09/01 18:19:38 nan Exp $ */

// Copyright (C) 2001  神南 吉宏(Kanna Yoshihiro)
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

#ifndef _PlayerSelectView2D_
#define _PlayerSelectView2D_
#include "PlayerSelectView.h"

class PlayerSelect;

class PlayerSelectView2D : public PlayerSelectView {
public:
  PlayerSelectView2D();
  virtual ~PlayerSelectView2D();

  virtual bool Init( PlayerSelect * );

  virtual bool Redraw();
  virtual bool RedrawAlpha();
  virtual bool GetDamageRect();

protected:
  SDL_Surface *m_playerBMP[PLAYERS];
};

#endif	// _PlayerSelectView2D_
