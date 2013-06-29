/* $Id: TitleView.cpp,v 1.18 2003/07/28 17:00:52 nan Exp $ */

// Copyright (C) 2000, 2002, 2003  ¿ÀÆî µÈ¹¨(Kanna Yoshihiro)
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
#include "TitleView.h"
#include "Title.h"
#include "MenuItem.h"
#include "LoadImage.h"
#include "BaseView.h"
#include "RCFile.h"

extern RCFile *theRC;

TitleView::TitleView() {
  m_View = NULL;
}

TitleView::~TitleView() {
}

bool
TitleView::Init( Title *title ) {
  int i, j, k;

  static char configTitle[][30] = {"images/LevelSelect",
				   "images/ModeSelect"
				    };

  m_title = title;

  char fname[256];
  for ( i = 0 ; i < 2 ; i++ ) {
    sprintf( fname, _("%s.pbm"), &(configTitle[i][0]) );
    m_configTitle[i].LoadFile(fname);
  }

  return true;
}

bool
TitleView::AddView( View *view ) {
  view->m_next = m_View;
  m_View = view;

  return true;
}

bool
TitleView::RemoveView( View *view ) {
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
TitleView::Redraw() {
  View *view;

  view = m_View;
  while ( view ){
    view->Redraw();
    view = view->m_next;
  }

  return true;
}

bool
TitleView::RedrawAlpha() {
  View *view;

  glColor4f( 1.0F, 1.0F, 1.0F, 1.0F );

  glPushMatrix();
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  gluOrtho2D( 0.0, (GLfloat)BaseView::GetWinWidth(),
	      0.0, (GLfloat)BaseView::GetWinHeight() );
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  GLboolean depthtestenabled = glIsEnabled(GL_DEPTH_TEST);
  glDisable(GL_DEPTH_TEST);
  glDepthMask(0);

  switch ( m_title->GetSelectMode() ) {
  case MENU_MAIN:
    glColor4f( 1.0F, 1.0F, 0.0F, 1.0F );
    glBegin(GL_TRIANGLES);
    glVertex2i( m_title->GetSelected()->GetX()-80,
		m_title->GetSelected()->GetY() );
    glVertex2i( m_title->GetSelected()->GetX()-20,
		m_title->GetSelected()->GetY()+35 );
    glVertex2i( m_title->GetSelected()->GetX()-80,
		m_title->GetSelected()->GetY()+70 );
    glEnd();
    break;
  case MENU_CONFIG:
    // Title
    glColor4f( 0.0F, 0.2F, 0.0F, 0.5F );
    glBegin(GL_QUADS);
    glVertex2i(  30, 250 );
    glVertex2i( 330, 250 );
    glVertex2i( 330, 570 );
    glVertex2i(  30, 570 );

    glVertex2i( 430, 320 );
    glVertex2i( 730, 320 );
    glVertex2i( 730, 570 );
    glVertex2i( 430, 570 );

    glVertex2i( 430, 50 );
    glVertex2i( 730, 50 );
    glVertex2i( 730, 300 );
    glVertex2i( 430, 300 );
    glEnd();

    glColor4f( 1.0F, 1.0F, 1.0F, 1.0F );
    glRasterPos2i( 80, 520 );
    glBitmap( 200, 35, 0.0F, 0.0F, 0.0F, 0, m_configTitle[0].GetImage() );
    glRasterPos2i( 480, 520 );
    glBitmap( 200, 35, 0.0F, 0.0F, 0.0F, 0, m_configTitle[1].GetImage() );
//    glRasterPos2i( 480, 250 );
//    glBitmap( 200, 35, 0.0F, 0.0F, 0.0F, 0, &m_configTitle[2][0] );

    glColor4f( 1.0F, 1.0F, 0.0F, 1.0F );
    glBegin(GL_TRIANGLES);
    if ( m_title->GetSelected()->GetHeight() == 70 ) {
      glVertex2i( m_title->GetSelected()->GetX()-80,
		  m_title->GetSelected()->GetY() );
      glVertex2i( m_title->GetSelected()->GetX()-20,
		  m_title->GetSelected()->GetY()+35 );
      glVertex2i( m_title->GetSelected()->GetX()-80,
		  m_title->GetSelected()->GetY()+70 );
    } else {
      glVertex2i( m_title->GetSelected()->GetX()-40,
		  m_title->GetSelected()->GetY() );
      glVertex2i( m_title->GetSelected()->GetX()-10,
		  m_title->GetSelected()->GetY()+18 );
      glVertex2i( m_title->GetSelected()->GetX()-40,
		  m_title->GetSelected()->GetY()+36 );
    }
    glEnd();
    break;
  }

  glDepthMask(1);
  if (depthtestenabled) glEnable(GL_DEPTH_TEST);

  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();

  view = m_View;
  while ( view ){
    view->RedrawAlpha();
    view = view->m_next;
  }

  return true;
}
