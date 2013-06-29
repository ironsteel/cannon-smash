/* $Id: TrainingView.cpp,v 1.9 2002/11/04 14:37:01 nan Exp $ */

// Copyright (C) 2000, 2002  神南 吉宏(Kanna Yoshihiro)
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
#include "TrainingView.h"
#include "Training.h"
#include "LoadImage.h"
#include "Ball.h"
#include "BallView.h"

extern Ball theBall;

TrainingView::TrainingView() {
}

TrainingView::~TrainingView() {
}

bool
TrainingView::Init( PlayGame *training ) {
  static char max[20] = "images/Max.ppm";
  ImageData Image;

  PlayGameView::Init( training );

  glGenTextures( 1, &m_max );
  Image.LoadPPM( max );
  for ( int i = 0 ; i < Image.GetWidth() ; i++ ) {
    for ( int j = 0 ; j < Image.GetHeight() ; j++ ) {
      if ( Image.GetPixel( i, j, 0 ) >= 5 )
	Image.SetPixel( i, j, 3 , 255 );
      else
	Image.SetPixel( i, j, 3 , 0 );
    }
  }

  glBindTexture( GL_TEXTURE_2D, m_max );
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
	       Image.GetWidth(), Image.GetHeight(), 
	       0, GL_RGBA, GL_UNSIGNED_BYTE, Image.GetImage() );

  return true;
}

bool
TrainingView::RedrawAlpha() {
  PlayGameView::RedrawAlpha();

  glPushMatrix();
  glTranslatef( TABLEWIDTH/2-0.3F, 0, TABLEHEIGHT+NETHEIGHT );

  glEnable(GL_TEXTURE_2D);
  glColor3f( 0.0F, 0.0F, 0.0F );

  glBindTexture(GL_TEXTURE_2D, m_max );
  glBegin(GL_QUADS);
  glTexCoord2f(0.0F, 80.0F/256); glVertex3f(-0.2F, 0.0F, 0.2F );
  glTexCoord2f(0.0F,      0.0F); glVertex3f(-0.2F, 0.0F, 0.4F );
  glTexCoord2f(1.0F,      0.0F); glVertex3f( 0.2F, 0.0F, 0.4F );
  glTexCoord2f(1.0F, 80.0F/256); glVertex3f( 0.2F, 0.0F, 0.2F );
  glEnd();

  if ( ((Training *)m_playGame)->GetTrainingCount() < 10 ) {
    glBindTexture(GL_TEXTURE_2D, theBall.GetView()->m_number[((Training *)m_playGame)->GetTrainingCount()] );
    glBegin(GL_QUADS);
    glTexCoord2f(0.0F, 1.0F); glVertex3f( 0.2F, 0.0F, 0.0F );
    glTexCoord2f(0.0F, 0.0F); glVertex3f( 0.2F, 0.0F, 0.2F );
    glTexCoord2f(1.0F, 0.0F); glVertex3f( 0.4F, 0.0F, 0.2F );
    glTexCoord2f(1.0F, 1.0F); glVertex3f( 0.4F, 0.0F, 0.0F );
    glEnd();
  } else {	/* Y2K :-) */
    glBindTexture(GL_TEXTURE_2D, theBall.GetView()->m_number[(((Training *)m_playGame)->GetTrainingCount()/10)%10] );
    glBegin(GL_QUADS);
    glTexCoord2f(0.0F, 1.0F); glVertex3f( 0.2F, 0.0F, 0.0F );
    glTexCoord2f(0.0F, 0.0F); glVertex3f( 0.2F, 0.0F, 0.2F );
    glTexCoord2f(1.0F, 0.0F); glVertex3f( 0.3F, 0.0F, 0.2F );
    glTexCoord2f(1.0F, 1.0F); glVertex3f( 0.3F, 0.0F, 0.0F );
    glEnd();
    glBindTexture(GL_TEXTURE_2D, theBall.GetView()->m_number[((Training *)m_playGame)->GetTrainingCount()%10] );
    glBegin(GL_QUADS);
    glTexCoord2f(0.0F, 1.0F); glVertex3f( 0.3F, 0.0F, 0.0F );
    glTexCoord2f(0.0F, 0.0F); glVertex3f( 0.3F, 0.0F, 0.2F );
    glTexCoord2f(1.0F, 0.0F); glVertex3f( 0.4F, 0.0F, 0.2F );
    glTexCoord2f(1.0F, 1.0F); glVertex3f( 0.4F, 0.0F, 0.0F );
    glEnd();
  }
  if ( ((Training *)m_playGame)->GetTrainingMax() < 10 ) {
    glBindTexture(GL_TEXTURE_2D,
		  theBall.GetView()->m_number[((Training *)m_playGame)->GetTrainingMax()] );
    glBegin(GL_QUADS);
    glTexCoord2f(0.0F, 1.0F); glVertex3f( 0.2F, 0.0F, 0.2F );
    glTexCoord2f(0.0F, 0.0F); glVertex3f( 0.2F, 0.0F, 0.4F );
    glTexCoord2f(1.0F, 0.0F); glVertex3f( 0.4F, 0.0F, 0.4F );
    glTexCoord2f(1.0F, 1.0F); glVertex3f( 0.4F, 0.0F, 0.2F );
    glEnd();
  } else {	/* Y2K :-) */
    glBindTexture(GL_TEXTURE_2D,
		  theBall.GetView()->m_number[(((Training *)m_playGame)->GetTrainingMax()/10)%10] );
    glBegin(GL_QUADS);
    glTexCoord2f(0.0F, 1.0F); glVertex3f( 0.2F, 0.0F, 0.2F );
    glTexCoord2f(0.0F, 0.0F); glVertex3f( 0.2F, 0.0F, 0.4F );
    glTexCoord2f(1.0F, 0.0F); glVertex3f( 0.3F, 0.0F, 0.4F );
    glTexCoord2f(1.0F, 1.0F); glVertex3f( 0.3F, 0.0F, 0.2F );
    glEnd();
    glBindTexture(GL_TEXTURE_2D,
		  theBall.GetView()->m_number[((Training *)m_playGame)->GetTrainingMax()%10] );
    glBegin(GL_QUADS);
    glTexCoord2f(0.0F, 1.0F); glVertex3f( 0.3F, 0.0F, 0.2F );
    glTexCoord2f(0.0F, 0.0F); glVertex3f( 0.3F, 0.0F, 0.4F );
    glTexCoord2f(1.0F, 0.0F); glVertex3f( 0.4F, 0.0F, 0.4F );
    glTexCoord2f(1.0F, 1.0F); glVertex3f( 0.4F, 0.0F, 0.2F );
    glEnd();
  }

  glDisable(GL_TEXTURE_2D);
  glPopMatrix();

  return true;
}
