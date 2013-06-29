/* $Id: TitleView2D.cpp,v 1.7 2001/12/25 15:18:05 yotsuya Exp $ */

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
#include "TitleView2D.h"
#include "Title.h"
#include "MenuItem.h"
#include "LoadImage.h"
#include "BaseView2D.h"

TitleView2D::TitleView2D() {
  m_View = NULL;
}

TitleView2D::~TitleView2D() {
  ((BaseView2D *)BaseView::TheView())->AddUpdateRect( NULL );
}

bool
TitleView2D::AddView( View *view ) {
  view->m_next = m_View;
  m_View = view;

  return true;
}

bool
TitleView2D::RemoveView( View *view ) {
  View* _view = m_View;

  if ( _view == view ){
    m_View = _view->m_next;
    return true;
  }

  while ( _view ){
    if ( _view->m_next == view ){
      _view->m_next = view->m_next;
      return true;
    }
    _view = _view->m_next;
  }

  return false;
}
      
bool
TitleView2D::Init( Title *title ) {
  static char configTitle[][30] = {"images/LevelSelect.ppm",
				   "images/ModeSelect.ppm"
  };

  m_triangleBMP = SDL_LoadBMP( "images/Triangle.bmp" );
  SDL_SetColorKey( m_triangleBMP, SDL_SRCCOLORKEY|SDL_RLEACCEL, 0 );

  m_title = title;

  ((BaseView2D *)BaseView::TheView())->AddUpdateRect( NULL );

  return true;
}

bool
TitleView2D::Redraw() {
  View *view;

  view = m_View;
  while ( view ){
    view->Redraw();
    view = view->m_next;
  }

  return true;
}

bool
TitleView2D::RedrawAlpha() {
  View *view;
  SDL_Rect rect;

  switch ( m_title->GetSelectMode() ) {
  case MENU_MAIN:
    rect.x = m_title->GetSelected()->GetX()-80;
    rect.y = (short)m_title->GetSelected()->GetY();
    rect.w = m_triangleBMP->w;
    rect.h = m_triangleBMP->h;

    SDL_BlitSurface(m_triangleBMP, NULL, BaseView::TheView()->GetSurface(),
		    &rect);
    break;
  case MENU_CONFIG:
    // Title
    if ( m_title->GetSelected()->GetHeight() == 70 ) {
      rect.x = m_title->GetSelected()->GetX()-80;
      rect.y = (short)m_title->GetSelected()->GetY();
      rect.w = m_triangleBMP->w;
      rect.h = m_triangleBMP->h;

      SDL_BlitSurface(m_triangleBMP, NULL, BaseView::TheView()->GetSurface(),
		      &rect);
    } else {
      rect.x = m_title->GetSelected()->GetX()-40;
      rect.y = (short)m_title->GetSelected()->GetY();
      rect.w = m_triangleBMP->w;
      rect.h = m_triangleBMP->h;

      SDL_BlitSurface(m_triangleBMP, NULL, BaseView::TheView()->GetSurface(),
		      &rect);
    }
    break;
  }

  view = m_View;
  while ( view ){
    view->RedrawAlpha();
    view = view->m_next;
  }

  return true;
}

bool
TitleView2D::GetDamageRect() {
  static SDL_Rect rect = {0, 0, 0, 0};
  switch ( m_title->GetSelectMode() ) {
  case MENU_MAIN:
    if ( rect.x != m_title->GetSelected()->GetX()-80 ||
	 rect.y != m_title->GetSelected()->GetY() ||
	 rect.w != m_triangleBMP->w ||
	 rect.h != m_triangleBMP->h ) {
      ((BaseView2D *)BaseView::TheView())->AddUpdateRect( &rect );
      rect.x = m_title->GetSelected()->GetX()-80;
      rect.y = (short)m_title->GetSelected()->GetY();
      rect.w = 480;
      rect.h = 70;
      ((BaseView2D *)BaseView::TheView())->AddUpdateRect( &rect );
    }
    break;
  case MENU_CONFIG:
    // Title
    if ( m_title->GetSelected()->GetHeight() == 70 ) {
      if ( rect.x != m_title->GetSelected()->GetX()-80 ||
	   rect.y != m_title->GetSelected()->GetY() ||
	   rect.w != m_triangleBMP->w ||
	   rect.h != m_triangleBMP->h ) {
	((BaseView2D *)BaseView::TheView())->AddUpdateRect( &rect );
	rect.x = m_title->GetSelected()->GetX()-80;
	rect.y = (short)m_title->GetSelected()->GetY();
	rect.w = 280;
	rect.h = 70;
	((BaseView2D *)BaseView::TheView())->AddUpdateRect( &rect );
      }
    } else {
      if ( rect.x != m_title->GetSelected()->GetX()-40 ||
	   rect.y != m_title->GetSelected()->GetY() ||
	   rect.w != m_triangleBMP->w ||
	   rect.h != m_triangleBMP->h ) {
	((BaseView2D *)BaseView::TheView())->AddUpdateRect( &rect );
	rect.x = m_title->GetSelected()->GetX()-40;
	rect.y = (short)m_title->GetSelected()->GetY();
	rect.w = 240;
	rect.h = 70;
	((BaseView2D *)BaseView::TheView())->AddUpdateRect( &rect );
      }
    }
    break;
  }

  return true;
}
