/* $Id: HowtoView.cpp,v 1.23 2002/12/24 17:07:04 nan Exp $ */

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
#include "HowtoView.h"
#include "Howto.h"
#include "BaseView.h"
#include "RCFile.h"

extern RCFile *theRC;

GLuint HowtoView::m_textures[4] = {0, 0, 0, 0};

HowtoView::HowtoView() {
}

HowtoView::~HowtoView() {
}

bool
HowtoView::Init( Howto *howto ) {
  ImageData image;
  int i, j, k;
#ifndef HAVE_LIBZ
  FILE *fp;
#else
  gzFile fp;
#endif

  static char fname[][30] = {"images/Mouse1.jpg", "images/Mouse2.jpg",
			     "images/Mouse3.jpg", "images/Mouse4.jpg"};
  static char arrowname[][30] = {"images/rightArrow", "images/downArrow",
				 "images/leftArrow", "images/upArrow"};
  static char howtoText[][30] = {"images/Howto1", "images/Howto2", 
				 "images/Howto3", "images/Howto4", 
				 "images/Howto5", "images/Howto6"};
  char filename[256];

  m_howto = howto;

  if ( m_textures[0] == 0 ) {
    glGenTextures( 4, m_textures );

    for ( i = 0 ; i < 4 ; i++ ){
      image.LoadFile( &(fname[i][0]) );
      for ( j = 0 ; j < image.GetWidth() ; j++ ) {
	for ( k = 0 ; k < image.GetHeight() ; k++ ) {
	  if ( image.GetPixel( j, k, 0 ) >= 5 ||
	       image.GetPixel( j, k, 1 ) >= 5 ||
	       image.GetPixel( j, k, 2 ) >= 5 )
	    image.SetPixel( j, k, 3 , 255 );
	  else
	    image.SetPixel( j, k, 3 , 0 );
	}
      }

      glBindTexture(GL_TEXTURE_2D, m_textures[i] );
      glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

      glTexImage2D(GL_TEXTURE_2D, 0, 4, image.GetWidth(), image.GetHeight(), 
		   0, GL_RGBA, GL_UNSIGNED_BYTE, image.GetImage() );
    }
  }

  for ( i = 0 ; i < 6 ; i++ ) {
    sprintf( filename, _("%s.pbm"), howtoText[i] );
    m_howtoText[i].LoadFile(filename);
  }

  for ( i = 0 ; i < 4 ; i++ ) {
    sprintf( filename, _("%s.pbm"), arrowname[i] );
    m_arrow[i].LoadFile(filename);
  }

  glGenTextures( 1, m_keyboard );
  image.LoadPPM( "images/Keyboard.ppm" );
  for ( j = 0 ; j < image.GetWidth() ; j++ ) {
    for ( k = 0 ; k < image.GetHeight() ; k++ ) {
      if ( image.GetPixel( j, k, 0 ) >= 50 ||
	   image.GetPixel( j, k, 1 ) >= 50 ||
	   image.GetPixel( j, k, 2 ) >= 50 )
	image.SetPixel( j, k, 3 , 255 );
      else
	image.SetPixel( j, k, 3 , 0 );
    }
  }

  glBindTexture(GL_TEXTURE_2D, m_keyboard[0]);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

  glTexImage2D(GL_TEXTURE_2D, 0, 4, image.GetWidth(), image.GetHeight(),
	       0, GL_RGBA, GL_UNSIGNED_BYTE, image.GetImage() );

  return true;
}

bool
HowtoView::Redraw() {
  return true;
}

bool
HowtoView::RedrawAlpha() {
  glPushMatrix();
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  gluOrtho2D( 0.0, (GLfloat)BaseView::GetWinWidth(),
	      0.0, (GLfloat)BaseView::GetWinHeight() );
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glDisable(GL_DEPTH_TEST);

  switch ( m_howto->GetMode() ) {	// Background
  case 0:
  case 1:
  case 3:
  case 4:
    glColor4f( 0.0F, 0.0F, 0.0F, 0.3F );
    glBegin(GL_QUADS);
    glVertex2i(             0,               0);
    glVertex2i(IMAGE_WIDTH+50,               0);
    glVertex2i(IMAGE_WIDTH+50, IMAGE_HEIGHT+50);
    glVertex2i(             0, IMAGE_HEIGHT+50);
    glEnd();

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, m_textures[m_howto->GetMouseB()] );
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 1.0);
    glVertex2i( m_howto->GetMouseX(), m_howto->GetMouseY() );
    glTexCoord2f(1.0, 1.0);
    glVertex2i( IMAGE_WIDTH+m_howto->GetMouseX(), m_howto->GetMouseY() );
    glTexCoord2f(1.0, 0.0);
    glVertex2i( IMAGE_WIDTH+m_howto->GetMouseX(),
		IMAGE_HEIGHT+m_howto->GetMouseY() );
    glTexCoord2f(0.0, 0.0);
    glVertex2i( m_howto->GetMouseX(), IMAGE_HEIGHT+m_howto->GetMouseY() );
    glEnd();
    glDisable(GL_TEXTURE_2D);

    break;
  case 2:
    if ( m_howto->GetCount() > 100 && m_howto->GetCount() < 1000 ) {
      glColor4f( 0.0, 0.0, 0.0, 1.0 );

      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D, m_keyboard[0] );
      glBegin(GL_QUADS);
      glTexCoord2f(0.0, 1.0); glVertex2i( 120, -180 );
      glTexCoord2f(1.0, 1.0); glVertex2i( 800, -180 );
      glTexCoord2f(1.0, 0.0); glVertex2i( 800,  300 );
      glTexCoord2f(0.0, 0.0); glVertex2i( 120,  300 );
      glEnd();
      glDisable(GL_TEXTURE_2D);
    }
    break;
  }

  switch ( m_howto->GetMode() ) {
  case 0:
    glColor4f( 1.0, 1.0, 1.0, 1.0 );
    switch ( (m_howto->GetCount()%400)/100 ) {
    case 0:
      glRasterPos2i( IMAGE_WIDTH, IMAGE_HEIGHT/2 );
      break;
    case 1:
      glRasterPos2i( IMAGE_WIDTH/2, 0 );
      break;
    case 2:
      glRasterPos2i( 0, IMAGE_HEIGHT/2 );
      break;
    case 3:
      glRasterPos2i( IMAGE_WIDTH/2, IMAGE_HEIGHT );
      break;
    }

    glBitmap( 50, 50, 0.0, 0.0, 0.0, 0, 
	      m_arrow[(m_howto->GetCount()%400)/100].GetImage() );
    break;
  case 2:
    glColor4f( 0.0, 0.5, 0.0, 1.0 );
    if ( m_howto->GetCount() > 200 && m_howto->GetCount() < 1000 &&
	 m_howto->GetCount()%100 > 0 && m_howto->GetCount()%100 < 30 ) {
      glBegin(GL_QUADS);
      //glVertex2i( 202+m_howto->GetMouseX()*36, 297-m_howto->GetMouseY()*72 );
      //glVertex2i( 235+m_howto->GetMouseX()*36, 297-m_howto->GetMouseY()*72 );
      //glVertex2i( 235+m_howto->GetMouseX()*36, 252-m_howto->GetMouseY()*72 );
      //glVertex2i( 202+m_howto->GetMouseX()*36, 252-m_howto->GetMouseY()*72 );
      glVertex2i( 192+m_howto->GetMouseX()*27, 296-m_howto->GetMouseY()*56 );
      glVertex2i( 262+m_howto->GetMouseX()*27, 296-m_howto->GetMouseY()*56 );
      glVertex2i( 262+m_howto->GetMouseX()*27, 242-m_howto->GetMouseY()*56 );
      glVertex2i( 192+m_howto->GetMouseX()*27, 242-m_howto->GetMouseY()*56 );
      glEnd();
    }
    break;
  case 4:
    glColor4f( 1.0, 1.0, 1.0, 1.0 );
    if ( m_howto->GetCount() < 1000 )
      glRasterPos2i( 460, 170 );
    else
      glRasterPos2i( 240, 170 );

    glBitmap( 50, 50, 0.0, 0.0, 0.0, 0, m_arrow[1].GetImage() );
    break;
  }

  glColor4f( 1.0, 1.0, 1.0, 1.0 );
  if ( m_howto->GetCount()%100 < 75 ) {
    glRasterPos2i( 10, 450 );
    glBitmap( 400, 100, 0.0, 0.0, 0.0, 0,
	      m_howtoText[m_howto->GetMode()].GetImage() );
  }

  if ( theRC->gmode != GMODE_SIMPLE )
    glEnable(GL_DEPTH_TEST);

  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();

  return true;
}
