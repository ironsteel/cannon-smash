/* $Id: MenuItemView.cpp,v 1.8 2002/02/15 14:14:35 yotsuya Exp $ */

// Copyright (C) 2000, 2001, 2002  ¿ÀÆî µÈ¹¨(Kanna Yoshihiro)
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
#include "MenuItemView.h"
#include "MenuItem.h"
#include "LoadImage.h"
#include "BaseView.h"
#include "RCFile.h"

extern RCFile *theRC;

MenuItemView::MenuItemView() {
  m_image = NULL;
}

MenuItemView::~MenuItemView() {
  if ( m_image )
    delete m_image;
}

bool
MenuItemView::Init( MenuItem *menu, char *fileName ) {
  char fname[256];

  sprintf( fname, _("%s.pbm"), fileName );

  m_menuItem = menu;

  m_image = new ImageData();
  m_image->LoadFile( fname );
  return true;
}

bool
MenuItemView::Redraw() {
  return true;
}

bool
MenuItemView::RedrawAlpha() {
  glColor4f( 1.0F, 1.0F, 1.0F, 1.0F );

  glPushMatrix();
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  gluOrtho2D( 0.0F, (GLfloat)BaseView::GetWinWidth(),
	      0.0F, (GLfloat)BaseView::GetWinHeight() );
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  GLboolean depthtestenabled = glIsEnabled(GL_DEPTH_TEST);
  glDisable(GL_DEPTH_TEST);

  glDepthMask(0);

  if ( m_menuItem->GetSelected() )
    glColor4f( 1.0F, 1.0F, 0.0F, 1.0F );
  else
    glColor4f( 1.0F, 1.0F, 1.0F, 1.0F );

  glRasterPos2i( m_menuItem->GetX(), m_menuItem->GetY() );
  glBitmap( m_menuItem->GetWidth(), m_menuItem->GetHeight(),
	    0.0F, 0.0F, 0.0F, 0, m_image->GetImage() );

  glDepthMask(1);
  if (depthtestenabled) glEnable(GL_DEPTH_TEST);

  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();

  return true;
}
