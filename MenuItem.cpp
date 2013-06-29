/* $Id: MenuItem.cpp,v 1.6 2002/09/07 05:06:29 nan Exp $ */

// Copyright (C) 2000, 2001, 2002  神南 吉宏(Kanna Yoshihiro)
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
#include "MenuItem.h"
#include "MenuItemView.h"
#include "MenuItemView2D.h"
#include "BaseView.h"
#include "Title.h"
#include "RCFile.h"

extern RCFile *theRC;

MenuItem::MenuItem() {
  m_View = NULL;
  m_selected = false;
  m_x = 0;
  m_y = 0;
  m_width = 0;
  m_height = 0;
}

MenuItem::~MenuItem() {
  if ( m_View ){
    if ( m_parent )
      ((TitleView *)m_parent->GetView())->RemoveView( m_View );
    delete m_View;
  }
}

bool
MenuItem::Init( long x, long y, long width, long height, char *fileName,
		Title *parent ) {
  m_x = x;
  if ( theRC->gmode == GMODE_2D )
    m_y = BaseView::GetWinHeight()-y-height;
  else
    m_y = y;
  m_width = width;
  m_height = height;

  m_parent = parent;

  m_View = (MenuItemView *)View::CreateView( VIEW_MENUITEM );

  m_View->Init( this, fileName );

  if ( m_parent )
    ((TitleView *)m_parent->GetView())->AddView( m_View );

  return true;
}

bool
MenuItem::SetSelected( bool select ) {
  m_selected = select;
  return m_selected;
}
