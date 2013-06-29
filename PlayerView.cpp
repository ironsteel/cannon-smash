/* $Id: PlayerView.cpp,v 1.36 2003/07/28 16:57:35 nan Exp $ */

// Copyright (C) 2000-2003  ¿ÀÆî µÈ¹¨(Kanna Yoshihiro)
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

#include "PlayerView.h"
#include "Player.h"
#include "parts.h"
#if defined(CHIYO)
#include "loadparts.h"
#endif
#include "Ball.h"
#include "BaseView.h"
#include "RCFile.h"
#include "Control.h"

extern RCFile *theRC;

extern SDL_mutex *loadMutex;

extern Ball   theBall;

extern long mode;

partsmotion_t *PlayerView::motion_Fnormal = NULL;
partsmotion_t *PlayerView::motion_Bnormal = NULL;
partsmotion_t *PlayerView::motion_Fdrive = NULL;
partsmotion_t *PlayerView::motion_Fcut = NULL;
partsmotion_t *PlayerView::motion_Bcut = NULL;
partsmotion_t *PlayerView::motion_Fpeck = NULL;
partsmotion_t *PlayerView::motion_Bpeck = NULL;
partsmotion_t *PlayerView::motion_Fsmash = NULL;

PlayerView::PlayerView() {
  m_player = NULL;

  m_Fnormal = m_Bnormal = NULL;
  m_Fdrive  = m_Bdrive  = NULL;
  m_Fcut    = m_Bcut    = NULL;
  m_Fpeck   = m_Bpeck   = NULL;
  m_Fsmash  = m_Bsmash  = NULL;

  m_xdiff = m_ydiff = m_zdiff = 0.0;
}

PlayerView::~PlayerView() {
}

void *
PlayerView::LoadData(void *dum) {
#if !defined(CHIYO)
  motion_Fnormal = new partsmotion("Parts/Fnormal/Fnormal");
  motion_Bnormal = new partsmotion("Parts/Bnormal/Bnormal");
  motion_Fdrive = new partsmotion("Parts/Fdrive/Fdrive");
  motion_Fcut = new partsmotion("Parts/Fcut/Fcut");
  motion_Bcut = new partsmotion("Parts/Bcut/Bcut");
  motion_Fpeck = new partsmotion("Parts/Fpeck/Fpeck");
  motion_Bpeck = new partsmotion("Parts/Bpeck/Bpeck");
  motion_Fsmash = new partsmotion("Parts/Fsmash/Fsmash");
#else /* CHIYO */
  chdir("Parts");
  parts::loadobjects("body.txt");
  chdir("..");
#endif /* !CHIYO */

  return NULL;
}

bool
PlayerView::Init( Player *player ) {
  m_player = player;

  SDL_mutexP( loadMutex );

#if !defined(CHIYO)
  m_Fnormal = motion_Fnormal;
  m_Bnormal = motion_Bnormal;
  m_Fdrive = motion_Fdrive;
  m_Fcut = motion_Fcut;
  m_Bcut = motion_Bcut;
  m_Fpeck = motion_Fpeck;
  m_Bpeck = motion_Bpeck;
  m_Fsmash = motion_Fsmash;
#else /* CHIYO */
# define GETBODY(name, ptype) \
    m_##name = reinterpret_cast<body_parts*>(parts::getobject(#name#ptype)); \
	if (!m_##name || parts::sym_body != m_##name->type()) {\
        printf("Could not load " #name #ptype "\n"); exit(1); \
    }

  switch ( m_player->GetPlayerType() ) {
  case PLAYER_PENATTACK:
  case PLAYER_PENDRIVE:
    GETBODY(Fnormal, Pen);
    GETBODY(Bnormal, Pen);
    GETBODY(Fdrive, Pen);
    GETBODY(Fcut, Pen);
    GETBODY(Bcut, Pen);
    GETBODY(Fpeck, Pen);
    GETBODY(Bpeck, Pen);
    GETBODY(Fsmash, Pen);
    break;
  case PLAYER_SHAKECUT:
    GETBODY(Fnormal, Shake);
    GETBODY(Bnormal, Shake);
    GETBODY(Fdrive, Shake);
    GETBODY(Fcut, Shake);
    GETBODY(Bcut, Shake);
    GETBODY(Fpeck, Shake);
    GETBODY(Bpeck, Shake);
    GETBODY(Fsmash, Shake);
    break;
  default:
    GETBODY(Fnormal, Pen);
    GETBODY(Bnormal, Pen);
    GETBODY(Fdrive, Pen);
    GETBODY(Fcut, Pen);
    GETBODY(Bcut, Pen);
    GETBODY(Fpeck, Pen);
    GETBODY(Bpeck, Pen);
    GETBODY(Fsmash, Pen);
    break;
  }

# undef GETBODY
#endif /* !CHIYO */

  SDL_mutexV( loadMutex );
  return true;
}

bool
PlayerView::Redraw() {
  static GLfloat mat_green[] = { 0.1F, 0.1F, 0.1F, 1.0F };

  if ( Control::TheControl()->GetComPlayer() == m_player ) {
    glColor4f( 0.4F, 0.4F, 0.4F, 0.0F );
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_green);

    return SubRedraw();
  }

  return true;
}

bool
PlayerView::RedrawAlpha() {
  static GLfloat mat_black[] = { 0.0F, 0.0F, 0.0F, 1.0F };

  if ( Control::TheControl()->GetThePlayer() == m_player ) {
    if ( theRC->myModel == MODEL_WIREFRAME ||
	 theRC->myModel == MODEL_ARMONLY ) {
      glColor4f( 1.0F, 1.0F, 1.0F, 1.0F );
    } else {
      glColor4f( 0.05F, 0.05F, 0.05F, 1.0F );
      glMaterialfv(GL_FRONT, GL_SPECULAR, mat_black);
    }

    return SubRedraw();
  }

  return true;
}

bool
PlayerView::SubRedraw() {

  DrawPlayer();

  // Target
  if ( Control::TheControl()->GetThePlayer() == m_player ) {
    DrawTarget();
#if 0
    if ( mode == MODE_SOLOPLAY || mode == MODE_MULTIPLAY ||
	 mode == MODE_PRACTICE )
      DrawMeter();
#endif
  }

  return true;
}

void
PlayerView::DrawTargetCircle( double diff ) {
  Ball *tmpBall;
  double vx, vy, vz;
  double rad;
  static double ballHeight = 1.4F;
  static bool count = true;

  if ( theRC->gmode == GMODE_SIMPLE ) {
    count = !count;
    if ( count )
      return;
  }

  // Make pseudo ball near the Player
  if ( (m_player->GetSide() == 1 &&
	(theBall.GetStatus() == 2 || theBall.GetStatus() == 3 || 
       theBall.GetStatus() == 5)) ||
       (m_player->GetSide() == -1 &&
	(theBall.GetStatus() == 0 || theBall.GetStatus() == 1 || 
       theBall.GetStatus() == 4)) ) {
    ballHeight = theBall.GetZ();
    tmpBall = new Ball( theBall.GetX(), theBall.GetY(), theBall.GetZ(),
			theBall.GetVX(), theBall.GetVY(), theBall.GetVZ(),
			theBall.GetSpin(), theBall.GetStatus() );

    while ( tmpBall->GetStatus() >= 0 &&
	    tmpBall->GetY()*m_player->GetSide() >
	    m_player->GetY()*m_player->GetSide() )
      tmpBall->Move();

    if ( tmpBall->GetStatus() < 0 )
      tmpBall->Warp( m_player->GetX()+0.3F*m_player->GetSide(),
		     m_player->GetY(), ballHeight, 
		     0.0F, 0.0F, 0.0F, 0.0F, 3 );
  } else {
    tmpBall = new Ball( m_player->GetX()+0.3F*m_player->GetSide(),
			m_player->GetY(), ballHeight, 
			0.0F, 0.0F, 0.0F, 0.0F, 3 );
  }

  // Calc initial speed of the ball when the player hit the ball
  double dum, level, maxVy;

  m_player->CalcLevel( tmpBall, dum, level, maxVy );
  tmpBall->TargetToV( m_player->GetTargetX(), m_player->GetTargetY(), 
		      level, m_player->GetSpin(), vx, vy, vz, 0.1F, maxVy );

  // Add difference to the initial speed. Calc bound location
  double v, v1x, v1y, v1z;
  double n1x, n1y, n1z, n2x, n2y, n2z;
  double bx = tmpBall->GetX(), by = tmpBall->GetY(), bz = tmpBall->GetZ();
  double polygon1[64][3], polygon2[64][3];
  int polyNum1 = 0, polyNum2 = 0;
  int i;

  v = sqrt(vx*vx+vy*vy+vz*vz);
  n1x = vy/hypot(vx, vy) * v*tan(diff);
  n1y = -vx/hypot(vx, vy) * v*tan(diff);
  n1z = 0;
  n2x = vx*vz/(v*hypot(vx, vy)) * v*tan(diff);
  n2y = vy*vz/(v*hypot(vx, vy)) * v*tan(diff);
  n2z = (vx*vx+vy*vy)/(v*hypot(vx, vy)) * v*tan(diff);

  for ( rad = 0.0 ; rad < 3.141592F*2 ; rad += 3.141592F/32 ) {
    v1x = vx+n1x*cos(rad)+n2x*sin(rad);
    v1y = vy+n1y*cos(rad)+n2y*sin(rad);
    v1z = vz+n1z*cos(rad)+n2z*sin(rad);

    if ( m_player->GetSide() > 0 ) {
      tmpBall->Warp( bx, by, bz, 
		     v1x, v1y, v1z, m_player->GetSpin(), 0 );
      while ( tmpBall->GetZ() > TABLEHEIGHT &&
	      tmpBall->GetZ()+tmpBall->GetVZ()*TICK > TABLEHEIGHT &&
	      tmpBall->GetVY() > 0.0 &&	// When the ball hits the net
	      tmpBall->GetStatus() == 0 )
	tmpBall->Move();
    } else {
      tmpBall->Warp( bx, by, bz, 
		     v1x, v1y, v1z, m_player->GetSpin(), 2 );
      while ( tmpBall->GetZ() > TABLEHEIGHT &&
	      tmpBall->GetZ()+tmpBall->GetVZ()*TICK > TABLEHEIGHT &&
	      tmpBall->GetVY() < 0.0 &&	// When the ball hits the net
	      tmpBall->GetStatus() == 2 )
	tmpBall->Move();
    }

    if ( tmpBall->GetY()*m_player->GetSide() > 0.0 ) {
      polygon1[polyNum1][0] = tmpBall->GetX();
      polygon1[polyNum1][1] = tmpBall->GetY();
      polygon1[polyNum1][2] = TABLEHEIGHT+0.01F;
      polyNum1++;
    } else {
      polygon2[polyNum2][0] = tmpBall->GetX();
      polygon2[polyNum2][1] = tmpBall->GetY();
      polygon2[polyNum2][2] = tmpBall->GetZ();
      polyNum2++;
    }
  }

  glBegin(GL_POLYGON);
    for ( i = 0 ; i < polyNum1 ; i++ )
      glVertex3f( polygon1[i][0], polygon1[i][1], polygon1[i][2] );
  glEnd();

  glBegin(GL_POLYGON);
    for ( i = 0 ; i < polyNum2 ; i++ )
      glVertex3f( polygon2[i][0], polygon2[i][1], polygon2[i][2] );
  glEnd();

  delete tmpBall;
}

// BallView ¤È¤«¤Ö¤ë. 
double
PlayerView::GetHitpointY() {
  Ball* tmpBall;

  if ( Control::TheControl()->GetThePlayer() &&
       (((theBall.GetStatus() == 2 || theBall.GetStatus() == 3) &&
	 Control::TheControl()->GetThePlayer()->GetSide() > 0) ||
	((theBall.GetStatus() == 0 || theBall.GetStatus() == 1) &&
	 Control::TheControl()->GetThePlayer()->GetSide() < 0)) ) {
    tmpBall = new Ball( theBall.GetX(), theBall.GetY(), theBall.GetZ(),
			theBall.GetVX(), theBall.GetVY(), theBall.GetVZ(),
			theBall.GetSpin(), theBall.GetStatus() );
    long t1 = 0, t2 = 0;
    // get time until the ball reaches hit point
    while ( tmpBall->GetStatus() != -1 ){
      tmpBall->Move();
      if ( tmpBall->GetY() < Control::TheControl()->GetThePlayer()->GetY() &&
	   tmpBall->GetStatus() == 3 )
	break;
      t1++;
    }
    if ( tmpBall->GetStatus() == -1 )
      return m_player->GetY()+0.6;

    delete tmpBall;

    tmpBall = new Ball( theBall.GetX(), theBall.GetY(), theBall.GetZ(),
			theBall.GetVX(), theBall.GetVY(), theBall.GetVZ(),
			theBall.GetSpin(), theBall.GetStatus() );

    while ( tmpBall->GetStatus() != -1 ){
      tmpBall->Move();
      t2++;
      if ( t1-10 == t2 ){
	return tmpBall->GetY();
      }
    }
    delete tmpBall;
  }

  return m_player->GetY()+0.6;
}

void
PlayerView::DrawPlayer() {
  double swing;
  partsmotion_t *motion;

  swing = m_player->GetSwing();
  switch( m_player->GetSwingType() ) {
  case SWING_NORMAL:
    if ( m_player->ForeOrBack() )
      motion = m_Fnormal;
    else
      motion = m_Bnormal;
    break;
  case SWING_POKE:
    if ( m_player->ForeOrBack() )
      motion = m_Fpeck ? m_Fpeck : m_Fnormal;
    else
      motion = m_Bpeck ? m_Bpeck : m_Bnormal;
    break;
  case SWING_SMASH:
    if ( m_player->ForeOrBack() )
      motion = m_Fsmash ? m_Fsmash : m_Fnormal;
    else
      motion = m_Bsmash ? m_Bsmash : m_Bnormal;
    break;
  case SWING_DRIVE:
    if ( m_player->ForeOrBack() )
      motion = m_Fdrive ? m_Fdrive : m_Fnormal;
    else
      motion = m_Bdrive ? m_Bdrive : m_Bnormal;
    break;
  case SWING_CUT:
    if ( m_player->ForeOrBack() )
      motion = m_Fcut ? m_Fcut : m_Fnormal;
    else
      motion = m_Bcut ? m_Bcut : m_Bnormal;
    break;
  default:
    return;
  }

  float xdiff = 0.0, ydiff = 0.0, zdiff = 0.0;

  if ( m_player->GetSwing() > 10 && m_player->GetSwing() < 20 ) {
    Ball *tmpBall;

    tmpBall = new Ball( theBall.GetX(), theBall.GetY(), theBall.GetZ(),
			theBall.GetVX(), theBall.GetVY(), theBall.GetVZ(),
			theBall.GetSpin(), theBall.GetStatus() );

    for ( int i = m_player->GetSwing() ; i < 20 ; i++ )
      tmpBall->Move();

    if ( ((tmpBall->GetStatus() == 3 && m_player->GetSide() == 1) ||
	  (tmpBall->GetStatus() == 1 && m_player->GetSide() == -1)) ) {
      double hitpointX = m_player->GetSwingSide() ?
	m_player->GetX()+0.3*m_player->GetSide() :
	m_player->GetX()-0.3*m_player->GetSide();

      xdiff = tmpBall->GetX() -
	(hitpointX+m_player->GetX()*(20-m_player->GetSwing())*TICK);

      ydiff =
        (tmpBall->GetY() -
         (m_player->GetY()+m_player->GetVY()*(20-m_player->GetSwing())*TICK))*
        m_player->GetSide();

      zdiff = tmpBall->GetZ() - 0.85;	/* temporary */
    }

    if ( xdiff > 0.3 )
      xdiff = 0.3;
    if ( xdiff < -0.3 )
      xdiff = -0.3;
    if ( ydiff > 0.3 )
      ydiff = 0.3;
    if ( ydiff < -0.3 )
      ydiff = -0.3;
  }

  if ( m_player->GetSwing() >= 30 ) {
    m_xdiff *= 0.95;
    m_ydiff *= 0.95;
    m_zdiff *= 0.95;
  } else {
    m_xdiff = xdiff;
    m_ydiff = ydiff;
    m_zdiff = zdiff;
  }

  glPushMatrix();
    glTranslatef( m_player->GetX()-0.3F*m_player->GetSide(),
		  m_player->GetY(), 0 );

    glRotatef( -atan2( m_player->GetTargetX()-m_player->GetX(),
		       m_player->GetTargetY()-m_player->GetY() )*180/3.141592F,
	       0.0F, 0.0F, 1.0F );

    if ( theRC->gmode == GMODE_SIMPLE ) {
      if ( Control::TheControl()->GetThePlayer() == m_player &&
	   theRC->myModel == MODEL_ARMONLY )
	motion->renderArmOnly(swing, m_xdiff, m_ydiff, m_zdiff);
      else
	motion->renderWire(swing, m_xdiff, m_ydiff, m_zdiff);
    } else {
      if (Control::TheControl()->GetComPlayer() == m_player) {
	motion->render(swing, m_xdiff, m_ydiff, m_zdiff);
      }
      if (Control::TheControl()->GetThePlayer() == m_player) {
	switch ( theRC->myModel ) {
	case MODEL_TRANSPARENT:
	  glEnable(GL_CULL_FACE);
	  glDisable(GL_DEPTH_TEST);
	  glDepthMask(0);
	  motion->render(swing, m_xdiff, m_ydiff, m_zdiff);
	  glDepthMask(1);
	  glEnable(GL_DEPTH_TEST);
	  glDisable(GL_CULL_FACE);
	  break;
	case MODEL_WIREFRAME:
	  motion->renderWire(swing, m_xdiff, m_ydiff, m_zdiff);
	  break;
	case MODEL_ARMONLY:
	  motion->renderArmOnly(swing, m_xdiff, m_ydiff, m_zdiff);
	  break;
	}
      }
    }

  glPopMatrix();
}

void
PlayerView::DrawTarget() {
  glColor4f( 1.0F, 0.0F, 0.0F, 0.5F );

  static GLfloat mat_default[] = { 0.0F, 0.0F, 0.0F, 1.0F };
  glMaterialfv(GL_FRONT, GL_SPECULAR, mat_default);

  double targetX, targetY;
  if ( theBall.GetStatus() == 2 || theBall.GetStatus() == 3 )
    m_player->GetModifiedTarget( targetX, targetY );
  else {
    targetX = m_player->GetTargetX();
    targetY = m_player->GetTargetY();
  }

  double diff;

  diff = (double)(220-m_player->GetStatus())/220*3.141592F/18;
  //DrawTargetCircle( diff );

  targetX = Control::TheControl()->GetThePlayer()->GetTargetX();
  targetY = Control::TheControl()->GetThePlayer()->GetTargetY();

  glColor4f( 1.0F, 0.0F, 0.0F, 1.0F );
  glBegin(GL_POLYGON);
    glNormal3f( 0.0F, 1.0F, 0.0F );
    glVertex3f( targetX-0.08F, targetY, TABLEHEIGHT+1.7320508F*0.08F );
    glVertex3f( targetX+0.08F, targetY, TABLEHEIGHT+1.7320508F*0.08F );
    glVertex3f( targetX, targetY, TABLEHEIGHT );
  glEnd();
}

void
PlayerView::DrawMeter() {
  static long count = 0;

  count++;

  long status = m_player->GetStatus();

  if ( status < m_player->StatusBorder() && count%10 < 5 )
    return;

  glDisable(GL_DEPTH_TEST);
  glDepthMask(0);

  glPushMatrix();
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  gluOrtho2D( 0.0, (GLfloat)BaseView::GetWinWidth(),
	      (GLfloat)BaseView::GetWinHeight(), 0.0 );
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  long statusBarX, statusBarY, statusBarWidth, statusBarHeight;
  statusBarX = 0;
  statusBarY = 1;

  statusBarWidth = BaseView::GetWinWidth()-2;
  statusBarHeight = BaseView::GetWinHeight()/20;

  glColor4f( 0.8F, 0.8F, 0.4F, 1.0F );
  glBegin(GL_QUADS);
    glVertex2i( statusBarX, statusBarY );
    glVertex2i( statusBarX+statusBarWidth*status/200,
		statusBarY );
    glVertex2i( statusBarX+statusBarWidth*status/200,
		statusBarY+statusBarHeight );
    glVertex2i( statusBarX, statusBarY+statusBarHeight );
  glEnd();

  glColor4f( 1.0F, 0.0F, 0.0F, 1.0F );
  glBegin(GL_QUADS);
    glVertex2i( statusBarX+statusBarWidth*status/200,
		statusBarY );
    glVertex2i( statusBarX+statusBarWidth*status/200,
		statusBarY+statusBarHeight );
    glVertex2i( statusBarX+statusBarWidth, statusBarY+statusBarHeight );
    glVertex2i( statusBarX+statusBarWidth, statusBarY );
  glEnd();

  glColor4f( 1.0F, 1.0F, 1.0F, 1.0F );
  glBegin(GL_LINE_LOOP);
    glVertex2i( statusBarX, statusBarY );
    glVertex2i( statusBarX+statusBarWidth, statusBarY );
    glVertex2i( statusBarX+statusBarWidth, statusBarY+statusBarHeight );
    glVertex2i( statusBarX, statusBarY+statusBarHeight );
  glEnd();

  glColor4f( 1.0F, 0.0F, 0.0F, 1.0F );
  glBegin(GL_LINES);
    glVertex2i( statusBarX+statusBarWidth*m_player->StatusBorder()/200,
		statusBarY );
    glVertex2i( statusBarX+statusBarWidth*m_player->StatusBorder()/200,
		statusBarY+statusBarHeight );
  glEnd();

  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();

  glDepthMask(1);
  if ( theRC->gmode != GMODE_SIMPLE )
    glEnable(GL_DEPTH_TEST);
}
