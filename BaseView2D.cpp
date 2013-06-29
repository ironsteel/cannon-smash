/* $Id: BaseView2D.cpp,v 1.15 2002/11/04 14:02:52 nan Exp $ */

// Copyright (C) 2001, 2002  神南 吉宏(Kanna Yoshihiro)
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
#include "BaseView.h"
#include "BaseView2D.h"
#include "Player.h"
#include "Control.h"
#include "Ball.h"
#include "LoadImage.h"
#include "PlayGame.h"
#include "RCFile.h"

extern long mode;

extern Ball theBall;

extern RCFile *theRC;

// x --- x axis is the bottom line of the net. The plane x=0 represents
//       the vertical plain which includes center line. 
// y --- y axis is the center line. The plane y=0 includes the net. 
// z --- z axis is the vertical line which includes the center point of
//       the table. The plane z=0 is the floor. 

BaseView2D::BaseView2D() {
}

BaseView2D::~BaseView2D() {
}

bool
BaseView2D::Init() {
// Create and initialize Window

  if ( theRC->fullScreen )
    m_baseSurface = SDL_SetVideoMode( m_winWidth, m_winHeight, 0,
				      SDL_HWSURFACE|SDL_FULLSCREEN );
  else
    m_baseSurface = SDL_SetVideoMode( m_winWidth, m_winHeight, 0,
				      SDL_HWSURFACE );

  SDL_WM_SetCaption( "CannonSmash", NULL );

  m_fieldView = (FieldView *)View::CreateView( VIEW_FIELD );
  m_fieldView->Init();

  m_updateX1 = 0;
  m_updateY1 = 0;
  m_updateX2 = BaseView::GetWinWidth()-1;
  m_updateY2 = BaseView::GetWinHeight()-1;

  m_fieldView->Redraw();
  m_fieldView->RedrawAlpha();
  SDL_UpdateRect(m_baseSurface, 0, 0, 0, 0);

  return true;
}

void
BaseView2D::DisplayFunc() {
  BaseView::TheView()->RedrawAll();
}

bool
BaseView2D::RedrawAll() {
  SDL_Rect rect;

  SetViewPosition();

  m_fieldView->GetDamageRect();
  if ( Control::TheControl()->GetThePlayer() )
    Control::TheControl()->GetThePlayer()->GetView()->GetDamageRect();
  if ( Control::TheControl()->GetComPlayer() )
    Control::TheControl()->GetComPlayer()->GetView()->GetDamageRect();
  theBall.GetView()->GetDamageRect();
  if ( Control::TheControl()->GetView() )
    Control::TheControl()->GetView()->GetDamageRect();

  rect.x = (short)m_updateX1;
  rect.y = (short)m_updateY1;
  rect.w = (short)(m_updateX2-m_updateX1+1);
  rect.h = (short)(m_updateY2-m_updateY1+1);

  SDL_SetClipRect( m_baseSurface, &rect );

  m_fieldView->Redraw();

  if ( Control::TheControl()->GetComPlayer() )
    Control::TheControl()->GetComPlayer()->GetView()->Redraw();

  SDL_SetClipRect( m_baseSurface, &rect );
  m_fieldView->RedrawAlpha();

  SDL_SetClipRect( m_baseSurface, NULL );

  if ( Control::TheControl()->GetThePlayer() )
    Control::TheControl()->GetThePlayer()->GetView()->RedrawAlpha();
  theBall.GetView()->Redraw();

  if ( Control::TheControl()->GetView() ) {
    Control::TheControl()->GetView()->Redraw();
    Control::TheControl()->GetView()->RedrawAlpha();
  }

  // 再描画領域のみ描画する
  SDL_UpdateRects(m_baseSurface, 1, &rect);
  m_updateX1 =m_updateY1 = m_updateX2 =m_updateY2 = 0;

  return true;
}

bool
BaseView2D::SetViewPosition() {
  SetLookAt();

  return true;
}

// Move the viewpoint as the player moves
void
BaseView2D::SetLookAt() {
  // later we have to do something
}
      
void
BaseView2D::EndGame() {
  // 後で実装
#if 0
  static char file[][30] = {"images/win.ppm", "images/lose.ppm"};
  GLubyte bmp[400*70/8];
#ifndef HAVE_LIBZ
  FILE *fp;
#else
  gzFile fp;
#endif
  int i, j;

  glColor4f(1.0F, 1.0F, 1.0F, 0.0F);

  glPushMatrix();
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  gluOrtho2D( 0.0F, (GLfloat)m_winWidth, 0.0F, (GLfloat)m_winHeight );
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  printf( "EndGame %d : %d\n",
	  ((PlayGame *)Control::TheControl())->GetScore(Control::TheControl()->GetThePlayer()), 
	  ((PlayGame *)Control::TheControl())->GetScore(Control::TheControl()->GetComPlayer()) );

  if ( Control::TheControl()->IsPlaying() && 
       ((PlayGame *)Control::TheControl())->GetScore(Control::TheControl()->GetThePlayer()) >
       ((PlayGame *)Control::TheControl())->GetScore(Control::TheControl()->GetComPlayer()) ) {
#ifndef HAVE_LIBZ
    if ( (fp = fopen(&file[0][0], "r")) == NULL )
      return;
#else
    if ( (fp = gzopenx(&file[0][0], "rs")) == NULL )
      return;
#endif
  }  else {
#ifndef HAVE_LIBZ
    if ( (fp = fopen(&file[1][0], "r")) == NULL )
      return;
#else
    if ( (fp = gzopenx(&file[1][0], "rs")) == NULL )
      return;
#endif
  }

  for ( i = 69 ; i >= 0 ; i-- ) {
    for ( j = 0 ; j < 400/8 ; j++ ) {
      bmp[i*50+j] = (unsigned char)strtol( getWord(fp), NULL, 16 );
    }
  }

#ifndef HAVE_LIBZ
  fclose(fp);
#else
  gzclose(fp);
#endif

  glRasterPos2i( 220, 330 );
  glBitmap( 400, 70, 0.0F, 0.0F, 0.0F, 0, bmp );

  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();

  SDL_GL_SwapBuffers();

#ifdef WIN32
  Sleep(3000);
#else
  sleep(3);
#endif
#endif
}

void
BaseView2D::QuitGame() {
  SDL_FreeSurface( m_baseSurface );
  delete m_fieldView;
}

bool
BaseView2D::AddUpdateRect( SDL_Rect *r ) {
  if ( !r ) {
    m_updateX1 = m_updateY1 = 0;
    m_updateX2 = BaseView::GetWinWidth()-1;
    m_updateY2 = BaseView::GetWinHeight()-1;

    return true;
  }

  if ( m_updateX2 == 0 && m_updateY2 == 0 ) {
    m_updateX1 = r->x;
    m_updateY1 = r->y;
    m_updateX2 = r->x + r->w - 1;
    m_updateY2 = r->y + r->h - 1;
  } else {
    if ( r->x < m_updateX1 )
      m_updateX1 = r->x;
    if ( r->y < m_updateY1 )
      m_updateY1 = r->y;
    if ( r->x+r->w-1 > m_updateX2 )
      m_updateX2 = r->x+r->w-1;
    if ( r->y+r->h-1 > m_updateY2 )
      m_updateY2 = r->y+r->h-1;
  }

  if ( m_updateX1 < 0 )
    m_updateX1 = 0;
  if ( m_updateX2 < 0 )
    m_updateX2 = 0;
  if ( m_updateX1 > BaseView::GetWinWidth() )
    m_updateX1 = BaseView::GetWinWidth();
  if ( m_updateX2 > BaseView::GetWinWidth() )
    m_updateX2 = BaseView::GetWinWidth();
  if ( m_updateY1 < 0 )
    m_updateY1 = 0;
  if ( m_updateY2 < 0 )
    m_updateY2 = 0;
  if ( m_updateY1 > BaseView::GetWinHeight() )
    m_updateY1 = BaseView::GetWinHeight();
  if ( m_updateY2 > BaseView::GetWinHeight() )
    m_updateY2 = BaseView::GetWinHeight();

  return true;
}

// For rendering
bool
RenderRect( double x1, double y1, double z1, 
	    double x2, double y2, double z2, 
	    SDL_Rect *rect ) {
  int _x1, _y1, _x2, _y2;

  RenderPoint( x1, y1, z1, &_x1, &_y1 );
  RenderPoint( x2, y2, z2, &_x2, &_y2 );

  if ( _x1 < _x2 ) {
    rect->x = _x1;
    rect->w = _x2-_x1;
  } else {
    rect->x = _x2;
    rect->w = _x1-_x2;
  }

  if ( _y1 < _y2 ) {
    rect->y = _y1;
    rect->h = _y2-_y1;
  } else {
    rect->y = _y2;
    rect->h = _y1-_y2;
  }

  if ( rect->x > BaseView::GetWinWidth() )
    rect->x = (short)BaseView::GetWinWidth();
  if ( rect->x+rect->w > BaseView::GetWinWidth() )
    rect->w = BaseView::GetWinWidth()-rect->x;
  if ( rect->y > BaseView::GetWinHeight() )
    rect->y = (short)BaseView::GetWinHeight();
  if ( rect->y+rect->h > BaseView::GetWinHeight() )
    rect->h = BaseView::GetWinHeight()-rect->y;

  return true;
}

bool
RenderPoint( double x, double y, double z, 
	     int *_x, int *_y ) {
  double __y, __z;

  y += 4.0; z -= 2.0;

  __y = y*1.7320508/2 - z*0.5;
  __z = y*0.5 + z*1.7320508/2;

  y = __y; z = __z;

  *_x = (int)(x/y*0.87*BaseView::GetWinWidth()+BaseView::GetWinWidth()/2);
  *_y = (int)(-z/y*0.87*BaseView::GetWinHeight()+BaseView::GetWinHeight()/2);

  return true;
}

