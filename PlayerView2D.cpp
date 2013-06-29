/* $Id: PlayerView2D.cpp,v 1.12 2001/12/25 15:18:05 yotsuya Exp $ */

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


#include "ttinc.h"
#include "PlayerView2D.h"
#include "BaseView2D.h"
#include "Player.h"

extern long mode;

PlayerView2D::PlayerView2D() {
  m_player = NULL;
  m_damageRect.x = m_damageRect.y = m_damageRect.w = m_damageRect.h = 0;
}

PlayerView2D::~PlayerView2D() {
}

bool
PlayerView2D::Init( Player *player ) {
//  static char pname[][30] = {"images/PenAttack.bmp", "images/ShakeCut.bmp",
//			     "images/PenDrive.bmp"};
  static char pname[][30] = {"images/PenAttack.jpg", "images/ShakeCut.jpg",
			     "images/PenDrive.jpg"};

  m_player = player;

  //m_playerBMP = SDL_LoadBMP( pname[m_player->GetPlayerType()-1] );
  m_playerBMP = IMG_Load( pname[m_player->GetPlayerType()-1] );
  SDL_SetColorKey( m_playerBMP, SDL_SRCCOLORKEY|SDL_RLEACCEL, 0 );

  return true;
}

bool
PlayerView2D::Redraw() {
  SDL_SetAlpha( m_playerBMP, 0, 0 );
  return SubRedraw();

  return true;
}

bool
PlayerView2D::RedrawAlpha() {
  SDL_SetAlpha( m_playerBMP, SDL_SRCALPHA|SDL_RLEACCEL, 100 );
  return SubRedraw();

  return true;
}

bool
PlayerView2D::SubRedraw() {
  SDL_Rect destRect;
  if ( m_player->GetY() > -3.5 ) {
    GetDrawRect( &destRect );

    SDL_BlitSurface(m_playerBMP, NULL, BaseView::TheView()->GetSurface(),
		    &destRect);
  }

  return true;
}

bool
PlayerView2D::GetDamageRect() {
  if ( m_player->GetY() > -3.5 ) {
    SDL_Rect _rect;

    GetDrawRect( &_rect );

    if ( m_damageRect.x != _rect.x || m_damageRect.y != _rect.y ||
	 m_damageRect.w != _rect.w || m_damageRect.h != _rect.h ) {
      ((BaseView2D *)BaseView::TheView())->AddUpdateRect( &m_damageRect );

      m_damageRect = _rect;

      ((BaseView2D *)BaseView::TheView())->AddUpdateRect( &m_damageRect );
    }
  }

  return true;
}

bool
PlayerView2D::GetDrawRect( SDL_Rect *drawRect ) {
  drawRect->x = drawRect->y = drawRect->w = drawRect->h = 0;

  if ( m_player->GetY() > -3.5 ) {
    int x, y;

    RenderPoint( m_player->GetX(), m_player->GetY(), 1.7, &x, &y );

    drawRect->x = x-m_playerBMP->w/2;
    drawRect->y = y;
    drawRect->w = m_playerBMP->w;
    drawRect->h = m_playerBMP->h;

    if ( drawRect->x < 0 ) {
      drawRect->w += drawRect->x;
      drawRect->x = 0;
    }
    if ( drawRect->x > BaseView::GetWinWidth() )
      drawRect->x = (short)BaseView::GetWinWidth();
    if ( drawRect->x+drawRect->w > BaseView::GetWinWidth() )
      drawRect->w = BaseView::GetWinWidth()-drawRect->x;
    if ( drawRect->y < 0 ) {
      drawRect->h += drawRect->y;
      drawRect->y = 0;
    }
    if ( drawRect->y > BaseView::GetWinHeight() )
      drawRect->y = (short)BaseView::GetWinHeight();
    if ( drawRect->y+drawRect->h > BaseView::GetWinHeight() )
      drawRect->h = BaseView::GetWinHeight()-drawRect->y;
  }

  return true;
}
