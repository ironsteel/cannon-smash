/* $Id: PlayerSelectView2D.cpp,v 1.9 2001/12/05 15:36:47 nan Exp $ */

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
#include "PlayerSelectView2D.h"
#include "PlayerSelect.h"
#include "LoadImage.h"
#include "BaseView2D.h"

extern long wins;

extern bool isComm;

PlayerSelectView2D::PlayerSelectView2D() {
}

PlayerSelectView2D::~PlayerSelectView2D() {
  ((BaseView2D *)BaseView::TheView())->AddUpdateRect( NULL );
}

bool
PlayerSelectView2D::Init( PlayerSelect *playerSelect ) {
//  static char pname[][30] = {"images/PenAttack.bmp", "images/ShakeCut.bmp",
//			     "images/PenDrive.bmp"};
  static char pname[][30] = {"images/PenAttack.jpg", "images/ShakeCut.jpg",
			     "images/PenDrive.jpg"};

  m_playerSelect = playerSelect;

  for ( int i = 0 ; i < PLAYERS ; i++ ) {
//    m_playerBMP[i] = SDL_LoadBMP( pname[i] );
    m_playerBMP[i] = IMG_Load( pname[i] );
  }

  ((BaseView2D *)BaseView::TheView())->AddUpdateRect( NULL );

  return true;
}

bool
PlayerSelectView2D::Redraw() {
  int i;
  int player;
  static SDL_Rect rect = {0, 0, 0, 0};

  if ( m_playerSelect->GetRotate() < 0 )
    player = (360+(m_playerSelect->GetRotate()%360))/(360/PLAYERS);
  else
    player = (m_playerSelect->GetRotate()%360)/(360/PLAYERS);

  if ( m_playerSelect->GetSelected() > 0 ) {
    for ( i = 0 ; i < PLAYERS ; i++ ) {
      if ( i == player ){
	if ( m_playerSelect->GetSelected() < 100 ) {
	  rect.x = BaseView::GetWinWidth()/2-m_playerBMP[i]->w/2
	    - m_playerSelect->GetSelected()*2;
	  rect.y = BaseView::GetWinHeight()/2-m_playerBMP[i]->h/2
	    - m_playerSelect->GetSelected()/2;
	} else {
	  rect.x = BaseView::GetWinWidth()/2-m_playerBMP[i]->w/2 - 200;
	  rect.y = BaseView::GetWinHeight()/2-m_playerBMP[i]->h/2 - 50;
	}
	rect.w = m_playerBMP[i]->w;
	rect.h = m_playerBMP[i]->h;

	SDL_BlitSurface(m_playerBMP[i], NULL,
			BaseView::TheView()->GetSurface(), &rect);
      }
    }
  } else {
    for ( i = 0 ; i < PLAYERS ; i++ ) {
      if ( i == player ){
	rect.x = BaseView::GetWinWidth()/2-m_playerBMP[i]->w/2;
	rect.y = BaseView::GetWinHeight()/2-m_playerBMP[i]->h/2;
	rect.w = m_playerBMP[i]->w;
	rect.h = m_playerBMP[i]->h;

	SDL_BlitSurface(m_playerBMP[i], NULL,
			BaseView::TheView()->GetSurface(), &rect);
      }
    }
  }

  if ( !isComm ) {
    if ( m_playerSelect->GetSelected() > 100 ) {
      rect.x = BaseView::GetWinWidth()/2-
	m_playerBMP[(player+wins+1)%PLAYERS]->w/2 + 200;
      rect.y = BaseView::GetWinHeight()/2-
	m_playerBMP[(player+wins+1)%PLAYERS]->h/2 - 50;
      rect.w = m_playerBMP[(player+wins+1)%PLAYERS]->w;
      rect.h = m_playerBMP[(player+wins+1)%PLAYERS]->h;

      SDL_BlitSurface(m_playerBMP[(player+wins+1)%PLAYERS], NULL,
		      BaseView::TheView()->GetSurface(), &rect);
    }
  }

  return true;
}

bool
PlayerSelectView2D::RedrawAlpha() {
  return true;
}

bool
PlayerSelectView2D::GetDamageRect() {
  static SDL_Rect rect = {0, 0, 0, 0};

  ((BaseView2D *)BaseView::TheView())->AddUpdateRect( &rect );

  if ( m_playerSelect->GetSelected() > 0 ) {
    if ( m_playerSelect->GetSelected() < 100 ) {
      rect.x = BaseView::GetWinWidth()/2-m_playerBMP[0]->w/2
	- m_playerSelect->GetSelected()*2;
      rect.y = BaseView::GetWinHeight()/2-m_playerBMP[0]->h/2
	- m_playerSelect->GetSelected()/2;
    } else {
      rect.x = BaseView::GetWinWidth()/2-m_playerBMP[0]->w/2 - 200;
      rect.y = BaseView::GetWinHeight()/2-m_playerBMP[0]->h/2 - 50;
    }
    rect.w = m_playerBMP[0]->w;
    rect.h = m_playerBMP[0]->h;
  } else {
    rect.x = BaseView::GetWinWidth()/2-m_playerBMP[0]->w/2;
    rect.y = BaseView::GetWinHeight()/2-m_playerBMP[0]->h/2;
    rect.w = m_playerBMP[0]->w;
    rect.h = m_playerBMP[0]->h;
  }

  ((BaseView2D *)BaseView::TheView())->AddUpdateRect( &rect );

  if ( !isComm ) {
    if ( m_playerSelect->GetSelected() > 100 ) {
      rect.x = BaseView::GetWinWidth()/2-m_playerBMP[0]->w/2 + 200;
      rect.y = BaseView::GetWinHeight()/2-m_playerBMP[0]->h/2 - 50;
      rect.w = m_playerBMP[0]->w;
      rect.h = m_playerBMP[0]->h;
    }
  }

  ((BaseView2D *)BaseView::TheView())->AddUpdateRect( &rect );

  return true;
}
