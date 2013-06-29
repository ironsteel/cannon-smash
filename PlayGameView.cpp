/* $Id: PlayGameView.cpp,v 1.6 2003/01/15 14:23:23 nan Exp $ */

// Copyright (C) 2001, 2002, 2003  神南 吉宏(Kanna Yoshihiro)
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
#include "PlayGameView.h"
#include "PlayGame.h"
#include "LoadImage.h"
#include "BaseView.h"

PlayGameView::PlayGameView() {
  m_image = 0;
}

PlayGameView::~PlayGameView() {
  if ( m_image )
    delete m_image;
}

bool
PlayGameView::Init( PlayGame *playGame ) {
  int j, k;

  static char imageFile[30] = "images/Pause";

  m_playGame = playGame;

  char filename[256];
  sprintf( filename, _("%s.pbm"), imageFile );

  m_image = new ImageData();
  m_image->LoadFile( filename );

  SDL_ShowCursor(SDL_DISABLE);
  SDL_WM_GrabInput( SDL_GRAB_ON );

  return true;
}

bool
PlayGameView::Redraw() {
  return true;
}

bool
PlayGameView::RedrawAlpha() {
  glColor4f( 1.0, 1.0, 1.0, 1.0 );

  if ( m_playGame->IsPause() ) {
    glPushMatrix();
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D( 0.0, (GLfloat)BaseView::GetWinWidth(),
		0.0, (GLfloat)BaseView::GetWinHeight() );
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glDepthMask(0);

    glRasterPos2i( BaseView::GetWinWidth()/2-128, BaseView::GetWinHeight()/2 );
    glBitmap( 256, 150, 0.0F, 0.0F, 0.0F, 0, m_image->GetImage() );

    glDepthMask(1);

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
  }

  return true;
}
