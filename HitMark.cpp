/* $Id: HitMark.cpp,v 1.11 2002/01/19 13:17:18 nan Exp $ */

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
#include "HitMark.h"
#include "LoadImage.h"
#include "RCFile.h"

extern RCFile *theRC;

GLuint HitMark::m_textures[2] = {0, 0};

HitMark::HitMark() {
  m_x = m_y = m_z = m_vx = m_vy = m_vz = 0.0;
  m_swingError = SWING_PERFECT;
}

HitMark::~HitMark() {
}

bool
HitMark::Init() {
  ImageData errorImage[2];
  int i, j;

  if ( m_textures[0] == 0 ) {
    errorImage[0].LoadPPM( "images/Nice.ppm" );
    errorImage[1].LoadPPM( "images/Bad.ppm" );

    for ( i = 0 ; i < errorImage[0].GetWidth() ; i++ ) {
      for ( j = 0 ; j < errorImage[0].GetHeight() ; j++ ) {
	errorImage[0].SetPixel( i, j, 1, 0 );
	errorImage[0].SetPixel( i, j, 2, 0 );
	if ( errorImage[0].GetPixel( i, j, 0 ) >= 5 )
	  errorImage[0].SetPixel( i, j, 3, 255 );
	else
	  errorImage[0].SetPixel( i, j, 3, 0 );
	errorImage[1].SetPixel( i, j, 0, 0 );
	errorImage[1].SetPixel( i, j, 1, 0 );
	if ( errorImage[1].GetPixel( i, j, 2 ) >= 5 )
	  errorImage[1].SetPixel( i, j, 3, 255 );
	else
	  errorImage[1].SetPixel( i, j, 3, 0 );
      }
    }

    glGenTextures( 2, m_textures );
    for ( i = 0 ; i < 2 ; i++ ) {
      glBindTexture(GL_TEXTURE_2D, m_textures[i] );
      glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
		   errorImage[i].GetWidth(), errorImage[i].GetHeight(),
		   0, GL_RGBA, GL_UNSIGNED_BYTE, errorImage[i].GetImage() );
    }
  }

  return true;
}

bool
HitMark::Hit( double x, double y, double z, double vx, double vy, double vz,
	       long swingError ) {
  m_x = x;
  m_y = y;
  m_z = z;
  m_vx = vx;
  m_vy = vy;
  m_vz = vz;
  m_swingError = swingError;

#ifndef WIN32
  struct timeval tv;
  struct timezone tz;
#endif

#ifdef WIN32
  ftime( &startTime );
#else
  gettimeofday( &tv, &tz );
  startTime.time = tv.tv_sec;
  startTime.millitm = tv.tv_usec/1000;
#endif

  return true;
}

bool
HitMark::Redraw() {
  struct timeb now;

#ifndef WIN32
  struct timeval tv;
  struct timezone tz;
#endif

#ifdef WIN32
  ftime( &now );
#else
  gettimeofday( &tv, &tz );
  now.time = tv.tv_sec;
  now.millitm = tv.tv_usec/1000;
#endif

  m_time = (now.time-startTime.time)*100+(now.millitm-startTime.millitm)/10;

  if ( m_time > 50 )
    return false;

  if ( m_time < 20 )
    m_time += m_time/2;
  else if ( m_time < 30 )
    m_time = 20+(30-m_time);
  else
    m_time = 20;

  return true;
}

bool
HitMark::RedrawAlpha() {
  if ( !theRC->isTexture )
    return true;

  glDisable(GL_DEPTH_TEST);
  glDepthMask(0);
  
  glPushMatrix();
  glTranslatef( (float)m_x, (float)m_y, TABLEHEIGHT+0.5F );

  switch ( m_swingError ) {
  case SWING_PERFECT:
    glEnable(GL_TEXTURE_2D);
    glColor3f( 0.0F, 0.0F, 0.0F );
    glBindTexture(GL_TEXTURE_2D, m_textures[0] );
    glBegin(GL_QUADS);
    glTexCoord2f(0.0F, 1.0F); glVertex3f( -0.3F*m_time/50, 0.0F, -0.42F*m_time/50);
    glTexCoord2f(0.0F, 0.0F); glVertex3f( -0.3F*m_time/50, 0.0F,  0.42F*m_time/50);
    glTexCoord2f(1.0F, 0.0F); glVertex3f(  0.3F*m_time/50, 0.0F,  0.42F*m_time/50);
    glTexCoord2f(1.0F, 1.0F); glVertex3f(  0.3F*m_time/50, 0.0F, -0.42F*m_time/50);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    break;
  case SWING_GREAT:
    glEnable(GL_TEXTURE_2D);
    glColor3f( 0.0F, 0.0F, 0.0F );
    glBindTexture(GL_TEXTURE_2D, m_textures[0] );
    glBegin(GL_QUADS);
    glTexCoord2f(0.0F, 1.0F); glVertex3f( -0.3F*m_time/50, 0.0F, -0.42F*m_time/50);
    glTexCoord2f(0.0F, 0.0F); glVertex3f( -0.3F*m_time/50, 0.0F,  0.42F*m_time/50);
    glTexCoord2f(1.0F, 0.0F); glVertex3f(  0.3F*m_time/50, 0.0F,  0.42F*m_time/50);
    glTexCoord2f(1.0F, 1.0F); glVertex3f(  0.3F*m_time/50, 0.0F, -0.42F*m_time/50);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    break;
  case SWING_GOOD:
    glEnable(GL_TEXTURE_2D);
    glColor3f( 0.0, 0.0, 0.0 );
    glBindTexture(GL_TEXTURE_2D, m_textures[1] );
    glBegin(GL_QUADS);
    glTexCoord2f(0.0F, 1.0F); glVertex3f( -0.3F*m_time/50, 0.0F, -0.42F*m_time/50);
    glTexCoord2f(0.0F, 0.0F); glVertex3f( -0.3F*m_time/50, 0.0F,  0.42F*m_time/50);
    glTexCoord2f(1.0F, 0.0F); glVertex3f(  0.3F*m_time/50, 0.0F,  0.42F*m_time/50);
    glTexCoord2f(1.0F, 1.0F); glVertex3f(  0.3F*m_time/50, 0.0F, -0.42F*m_time/50);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    break;
  case SWING_BOO:
    glEnable(GL_TEXTURE_2D);
    glColor3f( 0.0F, 0.0F, 0.0F );
    glBindTexture(GL_TEXTURE_2D, m_textures[1] );
    glBegin(GL_QUADS);
    glTexCoord2f(0.0F, 1.0F); glVertex3f( -0.3F*m_time/50, 0.0F, -0.42F*m_time/50);
    glTexCoord2f(0.0F, 0.0F); glVertex3f( -0.3F*m_time/50, 0.0F,  0.42F*m_time/50);
    glTexCoord2f(1.0F, 0.0F); glVertex3f(  0.3F*m_time/50, 0.0F,  0.42F*m_time/50);
    glTexCoord2f(1.0F, 1.0F); glVertex3f(  0.3F*m_time/50, 0.0F, -0.42F*m_time/50);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    break;
  case SWING_MISS:
/*
    glEnable(GL_TEXTURE_2D);
    glColor3f( 0.0F, 0.0F, 0.0F );
    glBindTexture(GL_TEXTURE_2D, m_textures[0] );
    glBegin(GL_QUADS);
    glTexCoord2f(0.0F, 1.0F); glVertex3f( -0.3F*time/50, 0.0F, -0.42F*time/50);
    glTexCoord2f(0.0F, 0.0F); glVertex3f( -0.3F*time/50, 0.0F,  0.42F*time/50);
    glTexCoord2f(1.0F, 0.0F); glVertex3f(  0.3F*time/50, 0.0F,  0.42F*time/50);
    glTexCoord2f(1.0F, 1.0F); glVertex3f(  0.3F*time/50, 0.0F, -0.42F*time/50);
    glEnd();
    glDisable(GL_TEXTURE_2D);
*/
    break;
  }

  glPopMatrix();

  glDepthMask(1);
  if ( theRC->gmode != GMODE_SIMPLE )
    glEnable(GL_DEPTH_TEST);

  return true;
}
