/* $Id: FieldView2D.cpp,v 1.4 2001/09/19 14:05:22 nan Exp $ */

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
#include "FieldView2D.h"
#include "BaseView.h"
#include "BaseView2D.h"

extern bool
RenderRect( double x1, double y1, double z1, 
	    double x2, double y2, double z2, 
	    SDL_Rect *rect );

FieldView2D::FieldView2D() {
}

FieldView2D::~FieldView2D() {
}

bool
FieldView2D::Init() {
  m_floor = SDL_LoadBMP( "images/BG.bmp" );
  m_title = SDL_LoadBMP( "images/Title.bmp" );
  m_table = SDL_LoadBMP( "images/BGAlpha.bmp" );

  SDL_SetColorKey( m_title, SDL_SRCCOLORKEY|SDL_RLEACCEL, 0 );
  SDL_SetAlpha( m_table, SDL_SRCALPHA|SDL_RLEACCEL, 100 );

  m_field = SDL_CreateRGBSurface( SDL_SWSURFACE, BaseView::GetWinWidth(),
					BaseView::GetWinHeight(),
					32, 0, 0, 0, 0 );

  SDL_Rect dest;

  dest.x = 0; 
  dest.y = 0; 
  dest.w = m_floor->w; 
  dest.h = m_floor->h; 
  SDL_BlitSurface(m_floor, NULL, m_field, &dest); 
 
  dest.x = 800-256; 
  dest.y = 600-256; 
  dest.w = m_title->w; 
  dest.h = m_title->h; 
  SDL_BlitSurface(m_title, NULL, m_field, &dest); 

  return true;
}

bool
FieldView2D::Redraw() {
  SDL_Rect dest;

  dest.x = 0;
  dest.y = 0;
  dest.w = m_field->w;
  dest.h = m_field->h;
  SDL_BlitSurface(m_field, NULL, BaseView::TheView()->GetSurface(), &dest);

  return true;
}

// Draw transparent object
bool
FieldView2D::RedrawAlpha() {
  SDL_Rect dest;

  dest.x = 0;
  dest.y = 0;
  dest.w = m_table->w;
  dest.h = m_table->h;
  SDL_BlitSurface(m_table, NULL, BaseView::TheView()->GetSurface(), &dest);

  return true;
}

bool
FieldView2D::GetDamageRect() {
  return true;
}
