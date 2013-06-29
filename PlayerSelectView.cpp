/* $Id: PlayerSelectView.cpp,v 1.15 2002/12/24 14:56:15 nan Exp $ */

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
#include "PlayerSelectView.h"
#include "PlayerSelect.h"
#include "LoadImage.h"
#include "BaseView.h"
#include "RCFile.h"

extern RCFile *theRC;

extern long wins;

extern bool isComm;

PlayerSelectView::PlayerSelectView() {
  m_offset = 0;
  m_textures[0] = 0;
  m_selectPlayer = 0;
}

PlayerSelectView::~PlayerSelectView() {
  if ( m_selectPlayer )
    delete m_selectPlayer;
}

bool
PlayerSelectView::Init( PlayerSelect *playerSelect ) {
  ImageData image;
  int i, j;

  static char pname[][30] = {"images/PenAttack.jpg", "images/ShakeCut.jpg",
			     "images/PenDrive.jpg"};

  m_playerSelect = playerSelect;

  if ( m_textures[0] == 0 ) {
    glGenTextures( PLAYERS+2, m_textures );

    for ( i = 0 ; i < PLAYERS ; i++ ){
      image.LoadFile( &(pname[i][0]) );
      glBindTexture(GL_TEXTURE_2D, m_textures[i] );
      glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

      glTexImage2D(GL_TEXTURE_2D, 0, 3, image.GetWidth(), image.GetHeight(), 
		   0, GL_RGBA, GL_UNSIGNED_BYTE, image.GetImage() );
    }
  }

  char filename[256];
  sprintf( filename, _("%s.pbm"), "images/SelectPlayer" );

  m_selectPlayer = new ImageData();
  m_selectPlayer->LoadFile( filename );

  return true;
}

bool
PlayerSelectView::Redraw() {
  int i;

  glColor4f( 0.0, 0.0, 0.0, 0.0 );

  if ( m_playerSelect->GetSelected() > 0 ) {
    int player;

    if ( m_playerSelect->GetRotate() < 0 )
      player = (360+(m_playerSelect->GetRotate()%360))/(360/PLAYERS);
    else
      player = (m_playerSelect->GetRotate()%360)/(360/PLAYERS);

    glPushMatrix();
    if ( m_playerSelect->GetSelected() < 100 ) {
      if ( theRC->gmode != GMODE_SIMPLE )
	glEnable(GL_TEXTURE_2D);
      glTranslatef( -0.01F*m_playerSelect->GetSelected(),
		    -1.5F+0.01F*m_playerSelect->GetSelected(), 1.4F );
      glRotatef( m_playerSelect->GetSelected()*360.0F/100, 0.0F, 0.0F, 1.0F );
    } else {
      glEnable(GL_TEXTURE_2D);
      glTranslatef( -0.01F*100, -1.5F+0.01F*100, 1.4F );
    }

    glBindTexture(GL_TEXTURE_2D, m_textures[player] );
    glBegin(GL_QUADS);
    glTexCoord2f(0.0F, 1.0F); glVertex3f(-0.60F, 0.0F, -0.84F);
    glTexCoord2f(0.0F, 0.0F); glVertex3f(-0.60F, 0.0F,  0.84F);
    glTexCoord2f(1.0F, 0.0F); glVertex3f( 0.60F, 0.0F,  0.84F);
    glTexCoord2f(1.0F, 1.0F); glVertex3f( 0.60F, 0.0F, -0.84F);
    glEnd();
    glPopMatrix();

    if ( !isComm ) {
      if ( m_playerSelect->GetSelected() > 100 ) {
	glPushMatrix();
	if ( m_playerSelect->GetSelected() < 150 )
	  glTranslatef( 0.02F*(m_playerSelect->GetSelected()-100),
			18.5F-0.4F*(m_playerSelect->GetSelected()-100), 1.4F );
	else
	  glTranslatef( 0.01F*100, -1.5F+0.01F*100, 1.4F );

	glBindTexture(GL_TEXTURE_2D, m_textures[(player+wins+1)%PLAYERS] );
	glBegin(GL_QUADS);
	glTexCoord2f(0.0F, 1.0F); glVertex3f(-0.60F, 0.0F, -0.84F);
	glTexCoord2f(0.0F, 0.0F); glVertex3f(-0.60F, 0.0F,  0.84F);
	glTexCoord2f(1.0F, 0.0F); glVertex3f( 0.60F, 0.0F,  0.84F);
	glTexCoord2f(1.0F, 1.0F); glVertex3f( 0.60F, 0.0F, -0.84F);
	glEnd();
	glPopMatrix();
      }
    }
  } else {
    if ( theRC->gmode != GMODE_SIMPLE ||
	 (m_playerSelect->GetRotate()%360)%(360/PLAYERS) == 0 )
      glEnable(GL_TEXTURE_2D);

    for ( i = 0 ; i < PLAYERS ; i++ ){
      glPushMatrix();
        glRotatef( m_playerSelect->GetRotate()-i*360.0F/PLAYERS, 0.0F, 0.0F, 1.0F );
	glBindTexture(GL_TEXTURE_2D, m_textures[i] );
	glBegin(GL_QUADS);
	glTexCoord2f(0.0F, 1.0F); glVertex3f(-0.60F, -1.5F, 1.4F-0.84F);
	glTexCoord2f(0.0F, 0.0F); glVertex3f(-0.60F, -1.5F, 1.4F+0.84F);
	glTexCoord2f(1.0F, 0.0F); glVertex3f( 0.60F, -1.5F, 1.4F+0.84F);
	glTexCoord2f(1.0F, 1.0F); glVertex3f( 0.60F, -1.5F, 1.4F-0.84F);
	glEnd();
      glPopMatrix();
    }

    glColor4f( 1.0F, 1.0F, 1.0F, 0.0F );

    glPushMatrix();
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D( 0.0F, (GLfloat)BaseView::GetWinWidth(),
		0.0F, (GLfloat)BaseView::GetWinHeight() );
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glRasterPos2i( 200, 100 );
    glBitmap( 400, 70, 0.0F, 0.0F, 0.0F, 0, m_selectPlayer->GetImage() );

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
  }

  glDisable(GL_TEXTURE_2D);

  return true;
}

bool
PlayerSelectView::RedrawAlpha() {
  return true;
}
