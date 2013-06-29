/* $Id: MenuItemView2D.cpp,v 1.4 2001/12/25 15:18:05 yotsuya Exp $ */

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
#include "MenuItemView2D.h"
#include "MenuItem.h"
#include "LoadImage.h"
#include "BaseView.h"

MenuItemView2D::MenuItemView2D() {
  m_image = NULL;
}

MenuItemView2D::~MenuItemView2D() {
  if ( m_image )
    free(m_image);
}

bool
MenuItemView2D::Init( MenuItem *menu, char *fileName ) {
  char fname[256];

  m_menuItem = menu;

  sprintf( fname, "%sW.bmp", fileName );
  m_imageBMP = SDL_LoadBMP( fname );
  SDL_SetColorKey( m_imageBMP, SDL_SRCCOLORKEY|SDL_RLEACCEL, 0 );

  sprintf( fname, "%sY.bmp", fileName );
  m_selectedImageBMP = SDL_LoadBMP( fname );
  SDL_SetColorKey( m_selectedImageBMP, SDL_SRCCOLORKEY|SDL_RLEACCEL, 0 );

  return true;
}

bool
MenuItemView2D::Redraw() {
  return true;
}

bool
MenuItemView2D::RedrawAlpha() {
  SDL_Rect rect;

  rect.x = (short)m_menuItem->GetX();
  rect.y = (short)m_menuItem->GetY();
  rect.w = m_imageBMP->w;
  rect.h = m_imageBMP->h;

  if ( m_menuItem->GetSelected() )
    SDL_BlitSurface(m_selectedImageBMP, NULL,
		    BaseView::TheView()->GetSurface(), &rect);
  else
    SDL_BlitSurface(m_imageBMP, NULL,
		    BaseView::TheView()->GetSurface(), &rect);

  return true;
}

