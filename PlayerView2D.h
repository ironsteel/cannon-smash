/* $Id: PlayerView2D.h,v 1.5 2001/09/11 15:01:01 nan Exp $ */

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

#ifndef _PlayerView2D_
#define _PlayerView2D_
#include "PlayerView.h"

class Player;

class PlayerView2D : public PlayerView {
public:
  PlayerView2D();
  virtual ~PlayerView2D();

  virtual bool Init( Player *player );

  virtual bool Redraw();
  virtual bool RedrawAlpha();
  virtual bool GetDamageRect();
  virtual bool GetDrawRect( SDL_Rect *drawRect );

private:
  Player  *m_player;	// Model
  SDL_Rect m_damageRect;

  SDL_Surface *m_playerBMP;

  virtual bool SubRedraw();
};

#endif	// _PlayerView2D
