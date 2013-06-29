/* $Id: BallView2D.cpp,v 1.10 2002/11/04 14:01:24 nan Exp $ */

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
#include "BallView2D.h"
#include "Ball.h"
#include "Player.h"
#include "Control.h"
#include "LoadImage.h"
#include "PlayGame.h"
#include "BaseView2D.h"

extern Ball   theBall;
extern long mode;

extern bool RenderRect( double x1, double y1, double z1, 
			double x2, double y2, double z2, 
			SDL_Rect *rect );

BallView2D::BallView2D() {
}

BallView2D::~BallView2D() {
}

bool
BallView2D::Init() {
  return true;
}
 
bool
BallView2D::Redraw() {
  static SDL_Rect rect = {0, 0, 0, 0};

  // Draw the Ball itself
  if ( theBall.GetY() > -3.5 ) {
    RenderRect( theBall.GetX()-BALL_R/2, theBall.GetY()-BALL_R/2,
		theBall.GetZ()-BALL_R/2, 
		theBall.GetX()+BALL_R/2, theBall.GetY()+BALL_R/2,
		theBall.GetZ()+BALL_R/2, 
		&rect );

    SDL_FillRect( BaseView::TheView()->GetSurface(), &rect, 
		  SDL_MapRGB( BaseView::TheView()->GetSurface()->format,
			      255, 144, 47 ) );

    // Draw the ball shadow
    if ( theBall.GetY() > -TABLELENGTH/2 && theBall.GetY() < TABLELENGTH/2 ){
      RenderRect(theBall.GetX()-BALL_R/2, theBall.GetY()-BALL_R/2, TABLEHEIGHT,
		 theBall.GetX()+BALL_R/2, theBall.GetY()+BALL_R/2, TABLEHEIGHT,
		 &rect );

      SDL_FillRect( BaseView::TheView()->GetSurface(), &rect, 0 );
    } else {
      RenderRect(theBall.GetX()-BALL_R/2, theBall.GetY()-BALL_R/2, 0.0,
		 theBall.GetX()+BALL_R/2, theBall.GetY()+BALL_R/2, 0.0,
		 &rect );

      SDL_FillRect( BaseView::TheView()->GetSurface(), &rect, 0 );
    }
  }

  // Draw the ball location in the future

  return true;
}

bool
BallView2D::RedrawAlpha() {
  // Score

  return true;
}

bool
BallView2D::GetDamageRect() {
  static SDL_Rect rect = {0, 0, 0, 0}, rectShadow = {0, 0, 0, 0};

  // Draw the Ball itself
  if ( theBall.GetY() > -3.5 ) {
    SDL_Rect _rect, _rectShadow;

    RenderRect( theBall.GetX()-BALL_R/2, theBall.GetY()-BALL_R/2,
		theBall.GetZ()-BALL_R/2, 
		theBall.GetX()+BALL_R/2, theBall.GetY()+BALL_R/2,
		theBall.GetZ()+BALL_R/2, 
		&_rect );

    if ( rect.x != _rect.x || rect.y != _rect.y ||
	 rect.w != _rect.w || rect.h != _rect.h ) {
      ((BaseView2D *)BaseView::TheView())->AddUpdateRect( &rect );
      ((BaseView2D *)BaseView::TheView())->AddUpdateRect( &_rect );
      rect = _rect;
    }

    if ( theBall.GetY() > -TABLELENGTH/2 && theBall.GetY() < TABLELENGTH/2 ){
      RenderRect(theBall.GetX()-BALL_R/2, theBall.GetY()-BALL_R/2, TABLEHEIGHT,
		 theBall.GetX()+BALL_R/2, theBall.GetY()+BALL_R/2, TABLEHEIGHT,
		 &_rectShadow );
    } else {
      RenderRect(theBall.GetX()-BALL_R/2, theBall.GetY()-BALL_R/2, 0.0,
		 theBall.GetX()+BALL_R/2, theBall.GetY()+BALL_R/2, 0.0,
		 &_rectShadow );
    }

    if ( rectShadow.x != _rectShadow.x || rectShadow.y != _rectShadow.y ||
	 rectShadow.w != _rectShadow.w || rectShadow.h != _rectShadow.h ) {
      ((BaseView2D *)BaseView::TheView())->AddUpdateRect( &rectShadow );
      ((BaseView2D *)BaseView::TheView())->AddUpdateRect( &_rectShadow );
      rectShadow = _rectShadow;
    }
  }

  // Draw the ball shadow

  // Draw the ball location in the future

  return true;
}
