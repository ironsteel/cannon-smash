/* $Id: Player.cpp,v 1.55 2003/11/19 16:49:31 nan Exp $ */

// Copyright (C) 2000-2003  神南 吉宏(Kanna Yoshihiro)
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
#include "Player.h"
#include "Ball.h"
#include "BaseView.h"
#include "Control.h"
#include "Event.h"
#include "PenAttack.h"
#include "PenDrive.h"
#include "ShakeCut.h"
#include "comPenAttack.h"
#include "comPenDrive.h"
#include "comShakeCut.h"
#include "TrainingPenAttack.h"
#include "TrainingPenDrive.h"
#include "ComTrainingPenAttack.h"
#include "ComTrainingPenDrive.h"
#include "HitMark.h"
#include "PlayGame.h"
#include "Network.h"
#include "RCFile.h"
#ifdef LOGGING
#include "Logging.h"
#endif

extern RCFile *theRC;

extern Ball   theBall;

extern long mode;

Player::Player() {
  m_side = 1;
  m_playerType = PLAYER_PROTO;

  m_x = 0;
  m_y = -TABLELENGTH/2-0.2;
  m_z = 1.4;
  m_vx = m_vy = m_vz = 0.0;

  m_status = 200;
  m_swing = 0;
  m_afterSwing = 0;
  m_swingType = SWING_NORMAL;
  m_swingError = SWING_PERFECT;
  m_targetX = 0.0;
  m_targetY = TABLELENGTH/16*5;

  m_eyeX = 0.0;
  m_eyeY = -1.0;
  m_eyeZ = 0.2;

  m_pow = 0;
  m_spin = 0.0;

  m_stamina = 80.0;

  m_statusMax = 200;

  m_lookAtX = 0.0;
  m_lookAtY = TABLELENGTH/2*m_side;
  m_lookAtZ = TABLEHEIGHT;

  m_View = NULL;

  m_lastSendX = m_lastSendY = m_lastSendZ = 0.0;
  m_lastSendVX = m_lastSendVY = m_lastSendVZ = 0.0;
  m_lastSendCount = 0;
}

Player::Player( long side ) {
  m_playerType = PLAYER_PROTO;

  m_x = 0;
  m_y = -TABLELENGTH/2-0.2;
  m_z = 1.4;
  m_vx = m_vy = m_vz = 0.0;

  m_status = 200;
  m_swing = 0;
  m_afterSwing = 0;
  m_swingType = SWING_NORMAL;
  m_swingError = SWING_PERFECT;
  m_targetX = 0.0;
  m_targetY = TABLELENGTH/16*5;

  m_eyeX = 0.0;
  m_eyeY = -1.0;
  m_eyeZ = 0.2;

  m_pow = 0;
  m_spin = 0.0;

  m_stamina = 80.0;

  m_View = NULL;

  m_side = side;
  if ( side < 0 ) {
    m_y = -m_y;
    m_targetY = -m_targetY;
    m_eyeY = -m_eyeY;
  }

  m_lookAtX = 0.0;
  m_lookAtY = TABLELENGTH/2*m_side;
  m_lookAtZ = TABLEHEIGHT;

  m_lastSendX = m_lastSendY = m_lastSendZ = 0.0;
  m_lastSendVX = m_lastSendVY = m_lastSendVZ = 0.0;
  m_lastSendCount = 0;
}

Player::Player( long playerType, long side, double x, double y, double z, 
		double vx, double vy, double vz,long status, long swing, 
		long swingType, bool swingSide, long afterSwing,
		long swingError, 
		double targetX, double targetY, double eyeX, double eyeY,
		double eyeZ, long pow, double spin, double stamina,
		long statusMax ) {
  m_side = side;
  m_playerType = playerType;

  m_x = x;
  m_y = y;
  m_z = z;
  m_vx = vx;
  m_vy = vy;
  m_vz = vz;

  m_status = status;
  m_swing = swing;
  m_afterSwing = afterSwing;
  m_swingType = swingType;
  m_swingSide = swingSide;
  m_swingError = swingError;
  m_targetX = targetX;
  m_targetY = targetY;

  m_eyeX = eyeX;
  m_eyeY = eyeY;
  m_eyeZ = eyeZ;

  m_pow = pow;
  m_spin = spin;

  m_stamina = stamina;
  m_statusMax = statusMax;

  m_lookAtX = 0.0;
  m_lookAtY = TABLELENGTH/2*m_side;
  m_lookAtZ = TABLEHEIGHT;

  m_View = NULL;

  m_lastSendX = m_lastSendY = m_lastSendZ = 0.0;
  m_lastSendVX = m_lastSendVY = m_lastSendVZ = 0.0;
  m_lastSendCount = 0;
}

Player::~Player() {
  if ( m_View ){
    BaseView::TheView()->RemoveView( m_View );
    delete m_View;
  }
}

void
Player::operator=(Player& p) {
  m_playerType = p.m_playerType;
  m_side = p.m_side;

  m_x = p.m_x;
  m_y = p.m_y;
  m_z = p.m_z;
  m_vx = p.m_vx;
  m_vy = p.m_vy;
  m_vz = p.m_vz;

  m_status = p.m_status;
  m_swing = p.m_swing;
  m_swingType = p.m_swingType;
  m_swingSide = p.m_swingSide;
  m_afterSwing = p.m_afterSwing;
  m_swingError = p.m_swingError;
  m_targetX = p.m_targetX;
  m_targetY = p.m_targetY;

  m_eyeX = p.m_eyeX;
  m_eyeY = p.m_eyeY;
  m_eyeZ = p.m_eyeZ;

  m_lookAtX = p.m_lookAtX;
  m_lookAtY = p.m_lookAtY;
  m_lookAtZ = p.m_lookAtZ;

  m_pow = p.m_pow;
  m_spin = p.m_spin;

  m_stamina = p.m_stamina;

  m_dragX = p.m_dragX;
  m_dragY = p.m_dragY;

  m_View = NULL;
}

Player*
Player::Create( long player, long side, long type ) {
  switch (type) {
  case 0:	// normal
    switch (player) {
    case 0:
      return new PenAttack(side);
    case 1:
      return new ShakeCut(side);
    case 2:
      return new PenDrive(side);
    }
    break;
  case 1:	// Com
    switch (player) {
    case 0:
      return new ComPenAttack(side);
    case 1:
      return new ComShakeCut(side);
    case 2:
      return new ComPenDrive(side);
    }
    break;
  case 2:	// Training
    switch (player) {
    case 0:
      return new TrainingPenAttack(side);
    case 1:
      return new TrainingPenDrive(side);
    }
    break;
  case 3:	// ComTraining
    switch (player) {
    case 0:
      return new ComTrainingPenAttack(side);
    case 1:
      return new ComTrainingPenDrive(side);
    }
    break;
  }

  printf( _("no player %ld\n"), player );
  exit(1);
}

bool
Player::Init() {
  m_View = (PlayerView *)View::CreateView( VIEW_PLAYER );

  m_View->Init( this );

  BaseView::TheView()->AddView( m_View );

  if ( theRC->gmode != GMODE_2D )
    HitMark::Init();

  return true;
}

bool
Player::Reset( Player *p ) {
  *this = *p;

  return true;
}

bool
Player::Move( SDL_keysym *KeyHistory, long *MouseXHistory,
	      long *MouseYHistory, unsigned long *MouseBHistory,
	      int Histptr ) {
  //static double  lastSendX = 0,  lastSendY = 0,  lastSendZ = 0;
  //static double lastSendVX = 0, lastSendVY = 0, lastSendVZ = 0;
  //static long lastSendCount = 0;

// swing
  if ( m_swing > 0 ){
    if ( m_swing > 30 && m_afterSwing > 0 ) {
      m_afterSwing--;
    } else {
      if ( theBall.GetStatus() == 6 || theBall.GetStatus() == 7 ) {
	if ( theBall.GetVZ() < 0 )
	  m_swing++;
      } else {
	if ( m_swing == 10 ) {
	  if ( theBall.GetStatus() == -1 )
	    m_swing = 0;
	  else if ( ( m_side > 0 && theBall.GetStatus() == 0) ||
		    ( m_side < 0 && theBall.GetStatus() == 2) ) {
	    m_swing++;
	  }
	} else
	  m_swing++;
      }
    }
  }

  // If the ball goes out of sight, look at the ball direction
  double x, y, z;
  double tx, ty, tz;
  double vx1, vy1, vz1;
  double vxt, vyt, vzt;
  double vx2, vy2, vz2;
  double vl;
  double p, q;
  double sinP, cosP;

  tx = 0.0;
  ty = TABLELENGTH/2*m_side;
  tz = TABLEHEIGHT;

  x = m_x + m_eyeX;
  y = m_y + m_eyeY;
  z = m_z + m_eyeZ;

  vx1 = tx-x;
  vy1 = ty-y;
  vz1 = tz-z;
  vl = sqrt(vx1*vx1+vy1*vy1+vz1*vz1);
  vx1 /= vl;
  vy1 /= vl;
  vz1 /= vl;

  vxt = theBall.GetX()-x;
  vyt = theBall.GetY()-y;
  if ( theBall.GetStatus() == 6 || theBall.GetStatus() == 7 )
    vzt = TABLEHEIGHT + 0.15-z;
  else
    vzt = theBall.GetZ()-z;
  vl = sqrt(vxt*vxt+vyt*vyt+vzt*vzt);
  vxt /= vl;
  vyt /= vl;
  vzt /= vl;

  if ( (cosP = vx1*vxt+vy1*vyt+vz1*vzt) < cos(3.141592/180.0*15) &&
       fabs(theBall.GetY()) > fabs(y) ) {
    sinP = sqrt(1-cosP*cosP);
    p = cos(3.141592/180.0*15) - sin(3.141592/180.0*15)*cosP/sinP;
    q = sin(3.141592/180.0*15)/sinP;

    vx2 = p*vxt+q*vx1;
    vy2 = p*vyt+q*vy1;
    vz2 = p*vzt+q*vz1;

    m_lookAtX = x+vx2;
    m_lookAtY = y+vy2;
    m_lookAtZ = z+vz2;
  } else if ( (cosP = (vy1*vyt+vz1*vzt)/(hypot(vy1, vz1)*hypot(vyt, vzt)))
	      < cos(3.141592/180.0*15) && theBall.GetZ() > z &&
	      (theBall.GetStatus() == 0 || theBall.GetStatus() == 2) ) {
    sinP = sqrt(1-cosP*cosP);
    p = cos(3.141592/180.0*15) - sin(3.141592/180.0*15)*cosP/sinP;
    q = sin(3.141592/180.0*15)/sinP;

    vx2 = p*vxt+q*vx1;
    vy2 = p*vyt+q*vy1;
    vz2 = p*vzt+q*vz1;

    m_lookAtX = x+vx2;
    m_lookAtY = y+vy2;
    m_lookAtZ = z+vz2;
  } else {
    m_lookAtX = tx;
    m_lookAtY = ty;
    m_lookAtZ = tz;
  }

// backswing and inpact
  if ( m_swing == 20 ){
    HitBall();

    if ( Control::TheControl()->GetThePlayer() == this &&
	 theRC->gmode != GMODE_2D ) {
      HitMark *hit;

      hit = new HitMark();
      hit->Hit( theBall.GetX(), m_y, theBall.GetZ(), 
		theBall.GetVX(), theBall.GetVY(), theBall.GetVZ(), 
		GetSwingError() );

      BaseView::TheView()->AddView( hit );
    }
    m_spin = 0.0;
  }
  else if ( m_swing == 50 ){
    m_swing = 0;
    m_swingType = SWING_NORMAL;
  }

  // Automatically move towards the ball
  // Only for human. 
  if ( (mode == MODE_SOLOPLAY || mode == MODE_MULTIPLAY ||
	mode == MODE_PRACTICE) && KeyHistory &&
       theRC->gameLevel != LEVEL_TSUBORISH ) {
    if ( m_swing > 10 && m_swing < 20 ) {
      Ball *tmpBall;

      tmpBall = new Ball( theBall.GetX(), theBall.GetY(), theBall.GetZ(),
			  theBall.GetVX(), theBall.GetVY(), theBall.GetVZ(),
			  theBall.GetSpin(), theBall.GetStatus() );

      for ( int i = 0 ; i < 20-m_swing ; i++ )
	tmpBall->Move();

      if ( ((tmpBall->GetStatus() == 3 && m_side == 1) ||
	    (tmpBall->GetStatus() == 1 && m_side == -1)) ) {
	double hitpointX = m_swingSide ? m_x+0.3*m_side : m_x-0.3*m_side;
	double xdiff = tmpBall->GetX() - (hitpointX+m_vx*(20-m_swing)*TICK);
	double ydiff = tmpBall->GetY() - (m_y+m_vy*(20-m_swing)*TICK);

	double vxdiff, vydiff;
	vxdiff = xdiff/TICK/(20-m_swing);

	if ( vxdiff > 2.0 )
	  vxdiff = 2.0;
	else if ( vxdiff < -2.0 )
	  vxdiff = -2.0;

	vxdiff /= theRC->gameLevel+1;

	m_vx += vxdiff;

	if ( fabs(ydiff) > 0.3 ) {
	  vydiff = ydiff/TICK/(20-m_swing);
	  if ( vydiff > 2.0 )
	    vydiff = 2.0;
	  else if ( vydiff < -2.0 )
	    vydiff = -2.0;

	  vydiff /= theRC->gameLevel+1;

	  m_vy += vydiff;
	}
      }
      delete tmpBall;
    }
  }

// move player
  if ( m_x+m_vx*TICK < -AREAXSIZE/2 ){
    m_x = -AREAXSIZE/2;
    m_vx = 0.0;
  }
  else if ( m_x+m_vx*TICK > AREAXSIZE/2 ){
    m_x = AREAXSIZE/2;
    m_vx = 0.0;
  }
  else if ( m_x <= -TABLEWIDTH/2 && m_x+m_vx*TICK >= -TABLEWIDTH/2 &&
	    m_y > -TABLELENGTH/2+0.5 && m_y < TABLELENGTH/2-0.5 ){
    m_x = -TABLEWIDTH/2;
    m_vx = 0.0;
  }
  else if ( m_x >= TABLEWIDTH/2 && m_x+m_vx*TICK <= TABLEWIDTH/2 &&
	    m_y > -TABLELENGTH/2+0.5 && m_y < TABLELENGTH/2-0.5 ){
    m_x = TABLEWIDTH/2;
    m_vx = 0.0;
  }
  else
    m_x += m_vx*TICK;

  if ( m_y+m_vy*TICK < -AREAYSIZE/2 ) {
    m_y = -AREAYSIZE/2;
    m_vy = 0.0;
  } else if ( m_y+m_vy*TICK > AREAYSIZE/2 ) {
    m_y = AREAYSIZE/2;
    m_vy = 0.0;
  } else if ( m_y <= -TABLELENGTH/2+0.5 && m_y+m_vy*TICK >= -TABLELENGTH/2+0.5
	      && m_x > -TABLEWIDTH/2 && m_x < TABLEWIDTH/2 ) {
    m_y = -TABLELENGTH/2+0.5;
    m_vy = 0.0;
  } else if ( m_y >= TABLELENGTH/2-0.5 && m_y+m_vy*TICK <= TABLELENGTH/2-0.5
	      && m_x > -TABLEWIDTH/2 && m_x < TABLEWIDTH/2 ) {
    m_y = TABLELENGTH/2-0.5;
    m_vy = 0.0;
  } else
    m_y += m_vy*TICK;

// Go back to the endline before serve
  if ( Control::TheControl()->IsPlaying() && theBall.GetStatus() == 8 &&
       ((PlayGame *)Control::TheControl())->GetService() == GetSide() ) {
    if ( m_side > 0 && m_y > -TABLELENGTH/2 )
      m_y = -TABLELENGTH/2;
    else if ( m_side < 0 && m_y < TABLELENGTH/2 )
      m_y = TABLELENGTH/2;
  }

  // Auto backswing
  if ( m_swing == 0 ) {
    Ball *tmpBall;

    tmpBall = new Ball( theBall.GetX(), theBall.GetY(), theBall.GetZ(),
			theBall.GetVX(), theBall.GetVY(), theBall.GetVZ(),
			theBall.GetSpin(), theBall.GetStatus() );

    for ( int i = 0 ; i < 30 ; i++ ) {	/* A bit earlier */
      tmpBall->Move();

      if ( ((tmpBall->GetStatus() == 3 && m_side == 1) ||
	    (tmpBall->GetStatus() == 1 && m_side == -1)) &&
	   (m_y-tmpBall->GetY())*m_side < 0.3 &&
	   (m_y-tmpBall->GetY())*m_side > -0.05 ){
	StartSwing( 3 );
	break;
      }
    }
    delete tmpBall;
  }

// calc status
  if ( hypot( m_vx, m_vy ) > 2.0 )
    AddStatus( -1 );

  if ( m_swing > 10 )
    AddStatus( -1 );

  if ( theBall.GetStatus() == 8 || theBall.GetStatus() == -1 )
    AddStatus( 200 );

  if ( SDL_WM_GrabInput( SDL_GRAB_QUERY ) == SDL_GRAB_ON )
    KeyCheck( KeyHistory, MouseXHistory, MouseYHistory, MouseBHistory,Histptr );

  if ( Control::TheControl()->GetThePlayer() == this &&
       mode == MODE_MULTIPLAY ) {
    m_lastSendCount++;

    m_lastSendX += m_lastSendVX*TICK;
    m_lastSendY += m_lastSendVY*TICK;
    m_lastSendZ += m_lastSendVZ*TICK;

    if ( m_lastSendCount >= 100 ||
	 fabs(m_lastSendVX-m_vx) >= 0.25 ||
	 fabs(m_lastSendVY-m_vy) >= 0.25 || fabs(m_lastSendVZ-m_vz) >= 0.25 ) {
      Event::TheEvent()->SendPlayer( this );
    }

    // theBall goes out of hitting area. 
    if ( ((theBall.GetStatus() == 1 && m_side == -1) ||
	  (theBall.GetStatus() == 3 && m_side == 1 ) ) &&
	 m_swing <= 10 &&
	 (m_y-theBall.GetY())*m_side > 0.3 &&
	 (m_y+m_vy*TICK-(theBall.GetY()+theBall.GetVY()*TICK))*m_side > 0.3 )
      Event::TheEvent()->SendBall();
  }

  return true;
}

bool
Player::KeyCheck( SDL_keysym *KeyHistory, long *MouseXHistory,
		  long *MouseYHistory, unsigned long *MouseBHistory,
		  int Histptr ) {
  long mouse, lastmouse;

const char keytable[][5] = {
  {'\0', '\0', '\0', '\0', '\0'},
  {'\0', '\0', '\0', '\0', '\0'},
  {'1', '1', '1', '[', '\0'},
  {'2', '2', '2', '7', '\0'},
  {'3', '3', '3', '5', '\0'},
  {'4', '4', '4', '3', '\0'},
  {'5', '5', '5', '1', '\0'},
  {'6', '6', '6', '9', '\0'},
  {'7', '7', '7', '0', '\0'},
  {'8', '8', '8', '2', '\0'},
  {'9', '9', '9', '6', '\0'},
  {'0', '0', '0', '8', '\0'},
  {'\0', '\0', '\0', '\0', '\0'},
  {'\0', '\0', '\0', '\0', '\0'},
  {'\0', '\0', '\0', '\0', '\0'},
  {'\0', '\0', '\0', '\0', '\0'},
  {'q', 'q', 'a', '/', '\0'},
  {'w', 'w', 'z', ',', '\0'},
  {'e', 'e', 'e', '.', '\0'},
  {'r', 'r', 'r', 'p', '\0'},
  {'t', 't', 't', 'y', '\0'},
  {'y', 'z', 'y', 'f', '\0'},
  {'u', 'u', 'u', 'g', '\0'},
  {'i', 'i', 'i', 'c', '\0'},
  {'o', 'o', 'o', 'r', '\0'},
  {'p', 'p', 'p', 'l', '\0'},
  {'\0', '\0', '\0', '\0', '\0'},
  {'\0', '\0', '\0', '\0', '\0'},
  {'\0', '\0', '\0', '\0', '\0'},
  {'\0', '\0', '\0', '\0', '\0'},
  {'a', 'a', 'q', 'a', '\0'},
  {'s', 's', 's', 'o', '\0'},
  {'d', 'd', 'd', 'e', '\0'},
  {'f', 'f', 'f', 'u', '\0'},
  {'g', 'g', 'g', 'i', '\0'},
  {'h', 'h', 'h', 'd', '\0'},
  {'j', 'j', 'j', 'h', '\0'},
  {'k', 'k', 'k', 't', '\0'},
  {'l', 'l', 'l', 'n', '\0'},
  {'\0', '\0', '\0', '\0', '\0'},	// {          'm', 's', '\0'},
  {'\0', '\0', '\0', '\0', '\0'},
  {'\0', '\0', '\0', '\0', '\0'},
  {'\0', '\0', '\0', '\0', '\0'},
  {'\0', '\0', '\0', '\0', '\0'},
  {'z', 'y', 'w', ';', '\0'},
  {'x', 'x', 'x', 'q', '\0'},
  {'c', 'c', 'c', 'j', '\0'},
  {'v', 'v', 'v', 'k', '\0'},
  {'b', 'b', 'b', 'x', '\0'},
  {'n', 'n', 'n', 'b', '\0'},
  {'m', 'm'     , 'm', '\0', '\0'},
  {'\0', '\0', '\0', '\0', '\0'},	// {               'w', '\0'},
  {'\0', '\0', '\0', '\0', '\0'},	// {               'v', '\0'},
  {'\0', '\0', '\0', '\0', '\0'}	// {               'z', '\0'}
};

// COM
  if ( !KeyHistory || !MouseXHistory || !MouseYHistory || !MouseBHistory )
    return true;

// key input
  // Check keyboard type and modify keycode. 
  int code = -1;

  if ( KeyHistory[Histptr].scancode < 54 ) {
    int i = 0;
    while (keytable[KeyHistory[Histptr].scancode][i]) {
      if ( keytable[KeyHistory[Histptr].scancode][i]
	   == KeyHistory[Histptr].unicode ) {
	code = keytable[KeyHistory[Histptr].scancode][0];
	break;
      }
      i++;
    }
  }

  if ( KeyHistory[Histptr].scancode >= 8 && 
       KeyHistory[Histptr].scancode < 62 && 
       code < 0 ) {	// for X11
    int i = 0;
    while (keytable[KeyHistory[Histptr].scancode-8][i]) {
      if ( keytable[KeyHistory[Histptr].scancode-8][i]
	   == KeyHistory[Histptr].unicode ) {
	code = keytable[KeyHistory[Histptr].scancode-8][0];
	break;
      }
      i++;
    }
  }

  if ( code < 0 )
    code = KeyHistory[Histptr].unicode;


  switch ( code ) {
  case '1':  case 'q':  case 'a':  case 'z':
  case '2':  case 'w':  case 's':  case 'x':
  case '3':
    m_targetX = -TABLEWIDTH/2*0.9*GetSide();
    break;
  case 'e':
    m_targetX = -TABLEWIDTH/2*0.75*GetSide();
    break;
  case 'd':
    m_targetX = -TABLEWIDTH/2*0.6*GetSide();
    break;
  case '4':  case 'c':
    m_targetX = -TABLEWIDTH/2*0.45*GetSide();
    break;
  case 'r':
    m_targetX = -TABLEWIDTH/2*0.3*GetSide();
    break;
  case 'f':
    m_targetX = -TABLEWIDTH/2*0.15*GetSide();
    break;
  case '5':  case 'v':
    m_targetX = 0;
    break;
  case 't':
    m_targetX = TABLEWIDTH/2*0.15*GetSide();
    break;
  case 'g':
    m_targetX = TABLEWIDTH/2*0.3*GetSide();
    break;
  case '6':  case 'b':
    m_targetX = TABLEWIDTH/2*0.45*GetSide();
    break;
  case 'y':
    m_targetX = TABLEWIDTH/2*0.6*GetSide();
    break;
  case 'h':
    m_targetX = TABLEWIDTH/2*0.75*GetSide();
    break;
  case '7':  case 'n':  case 'u':  case 'j':
  case '8':  case 'm':  case 'i':  case 'k':
  case '9':  case ',':  case 'o':  case 'l':
  case '0':  case '.':  case 'p':  case ';':
    m_targetX = TABLEWIDTH/2*0.9*GetSide();
    break;
  }

  switch ( code ){
  case '1':  case '2':  case '3':  case '4':  case '5':  case '6':
  case '7':  case '8':  case '9':  case '0':  case '-':  case '^':
    m_targetY = TABLELENGTH/12*5*GetSide();
    break;
  case 'q':  case 'w':  case 'e':  case 'r':  case 't':  case 'y':
  case 'u':  case 'i':  case 'o':  case 'p':  case '@':  case '[':
    m_targetY = TABLELENGTH/12*4*GetSide();
    break;
  case 'a':  case 's':  case 'd':  case 'f':  case 'g':  case 'h':
  case 'j':  case 'k':  case 'l':  case ';':  case ':':  case ']':
    m_targetY = TABLELENGTH/12*3*GetSide();
    break;
  case 'z':  case 'x': case 'c':  case 'v':  case 'b':  case 'n':
  case 'm':  case ',':  case '.':  case '/':  case '\\':
    m_targetY = TABLELENGTH/12*2*GetSide();
    break;
  }


  if ( (Histptr == 0 &&
	KeyHistory[Histptr].unicode != KeyHistory[MAX_HISTORY-1].unicode) ||
       (Histptr != 0 &&
	KeyHistory[Histptr].unicode != KeyHistory[Histptr-1].unicode) ) {
    switch ( KeyHistory[Histptr].unicode ) {
    case 'H':
      m_eyeX -= 0.05;
      break;
    case 'J':
      m_eyeZ -= 0.05;
      break;
    case 'K':
      m_eyeZ += 0.05;
      break;
    case 'L':
      m_eyeX += 0.05;
      break;
    case '<':
      m_eyeY -= 0.05;
      break;
    case '>':
      m_eyeY += 0.05;
      break;

    case 'A':
      m_lookAtX -= 0.05;
      break;
    case 'S':
      m_lookAtZ -= 0.05;
      break;
    case 'D':
      m_lookAtZ += 0.05;
      break;
    case 'F':
      m_lookAtX += 0.05;
      break;
    case 'C':
      m_lookAtY -= 0.05;
      break;
    case 'V':
      m_lookAtY += 0.05;
      break;

    }
  }

  // Sorry in Japanese...
  // スイング中は速度が変わらないようにする. こうすることで
  // MultiPlay 時により同期をとりやすくなる. 
  // その理由は, スイングを開始した時点で Player のインパクト
  // までの行動が決定されるため, スイング開始時に情報交換すれば
  // 同期がとれるようになるためである. これまでは, インパクト時に
  // 情報を交換していたので, 0.1秒早く同期がとれることになる. 

  // ちなみに, スイング後のマウスのドラッグによってボールの回転を
  // 制御する場合, この方法は使えないかも知れない. 

  if ( SDL_WM_GrabInput( SDL_GRAB_QUERY ) == SDL_GRAB_OFF )
    return true;

  if ( m_swing > 10 && m_swing <= 20 ) {
    long hptr = Histptr-(m_swing-11);
    if ( hptr < 0 )
      hptr += MAX_HISTORY;

    m_dragX = MouseXHistory[Histptr]-MouseXHistory[hptr];
    m_dragY = MouseYHistory[Histptr]-MouseYHistory[hptr];
  } else {
    m_vx = (MouseXHistory[Histptr] - BaseView::GetWinWidth()/2) /
      (BaseView::GetWinWidth()/40)*GetSide();
    m_vy = -(MouseYHistory[Histptr] - BaseView::GetWinHeight()/2) /
      (BaseView::GetWinHeight()/40)*GetSide();
    m_vx /= 4;
    m_vy /= 4;
  }

  mouse = MouseBHistory[Histptr];
  if ( Histptr-1 < 0 )
    lastmouse = MouseBHistory[MAX_HISTORY-1];
  else
    lastmouse = MouseBHistory[Histptr-1];

  if ( (mouse & BUTTON_RIGHT) && !(lastmouse & BUTTON_RIGHT) ){
    if ( theBall.GetStatus() == 8 &&
	 ((PlayGame *)Control::TheControl())->GetService() == GetSide() ) {
      theBall.Toss( this, 3 );
      StartSwing( 3 );
    } else {
      AddStatus( (m_swing-10)*10 );
      Swing( 3 );
    }
  }
  else if ( (mouse & BUTTON_MIDDLE) && !(lastmouse & BUTTON_MIDDLE) ){
    if ( theBall.GetStatus() == 8 &&
	 ((PlayGame *)Control::TheControl())->GetService() == GetSide() ) {
      theBall.Toss( this, 2 );
      StartSwing( 2 );
    } else {
      AddStatus( (m_swing-10)*10 );
      Swing( 2 );
    }
  }
  else if ( (mouse & BUTTON_LEFT) && !(lastmouse & BUTTON_LEFT) ){
    if ( theBall.GetStatus() == 8 &&
	 ((PlayGame *)Control::TheControl())->GetService() == GetSide() ) {
      theBall.Toss( this, 1 );
      StartSwing( 1 );
    } else {
      AddStatus( (m_swing-10)*10 );
      Swing( 1 );
    }
  }

  return true;
}

bool
Player::AddStatus( long diff ) {
  if ( diff == 200 ) {	// Not good...
    m_statusMax = 200;
    m_status = 200;
  } else {
    m_status += diff;

    if ( m_status > m_statusMax )
      m_status = m_statusMax;

    if ( m_status < 1 ) {
      m_stamina += (m_status-1) / 10.0;
      m_status = 1;
    }

    if ( diff < -3 ) {	// Not good... When status decreased without moving...
      if ( Control::TheControl()->GetThePlayer() == this ) {
	switch (theRC->gameLevel) {
	case LEVEL_EASY:
	  m_statusMax += diff/4;
	  break;
	case LEVEL_NORMAL:
	  m_statusMax += diff/3;
	  break;
	case LEVEL_HARD:
	  m_statusMax += diff/2;
	  break;
	case LEVEL_TSUBORISH:
	  m_statusMax += diff/2;
	  break;
	}
      } else {
	m_statusMax += diff/4;	/* 打球位置でのペナルティを相殺 */
      }
    }
  }

  return true;
}

// Calc the shoulder location
// x   --- x
// y   --- y
// deg --- rotation degree around z axis
bool
Player::GetShoulder( double &x, double &y, double &deg ) {
  double px, py, bx, by, bvx, bvy;
  double t, btx;

  // rotate 180 when the player is in the opposite side
  if ( m_side < 0 ){
    px = -m_x;
    py = -m_y;
    bx = -theBall.GetX();
    by = -theBall.GetY();
    bvx = -theBall.GetVX();
    bvy = -theBall.GetVY();
  }
  else{
    px = m_x;
    py = m_y;
    bx = theBall.GetX();
    by = theBall.GetY();
    bvx = theBall.GetVX();
    bvy = theBall.GetVY();
  }

  // target
  if ( bvy == 0.0 || m_swing == 0 ){
    btx = bx;
    t = -1;
  }
  else{
    t = (py - by) / bvy;
    btx = bx + bvx*t;
  }

  // Forehand or backhand?
  if ( btx - px > 0 ){	// Forehand
    switch ( m_swingType ){
    case SWING_NORMAL:
    case SWING_DRIVE:
      if ( m_swing < 10 )
	deg = -(double)m_swing/10*15-30;
      else if ( m_swing < 30 )
	deg = -45+(double)(m_swing-10)/20*45;
      else
	deg = -(double)(m_swing-30)/20*30;
      break;
    case SWING_POKE:
      if ( m_swing < 10 )
	deg = -(double)m_swing/10*10-30;
      else if ( m_swing < 30 )
	deg = -40+(double)(m_swing-10)/20*20;
      else
	deg = -20-(double)(m_swing-30)/20*10;
      break;
    case SWING_CUT:
      if ( m_swing < 10 )
	deg = -(double)m_swing/10*60-30;
      else if ( m_swing < 30 )
	deg = -60+(double)(m_swing-10)/20*60;
      else
	deg = -(double)(m_swing-30)/20*30;
      break;
    case SWING_SMASH:
      if ( m_swing < 10 )
	deg = -(double)m_swing/10*30-30;
      else if ( m_swing < 30 )
	deg = -60+(double)(m_swing-10)/20*90;
      else
	deg = 30-(double)(m_swing-30)/20*60;
      break;
    }

    if ( m_swing == 0 ){
      x = y = 0.0;
      return true;
    }

    // The distance from the ideal location for hitting
    if ( btx - px < 0.6 ){
      x = btx - px - 0.3;
      y = 0.0;
    }
    else {
      x = 0.3;
      y = 0.0;
    }
  }
  else{		// Backhand
    switch ( m_swingType ){
    case SWING_NORMAL:
    case SWING_SMASH:
    case SWING_DRIVE:
      if ( m_swing < 10 )
	deg = (double)m_swing/10*30-30;
      else if ( m_swing < 30 )
	deg = -(double)(m_swing-10)/20*45;
      else
	deg = -45+(double)(m_swing-30)/20*15;
      break;
    case SWING_POKE:
      if ( m_swing < 30 )
	deg = (double)m_swing/30*60-30;
      else
	deg = 30-(double)(m_swing-30)/20*60;
      break;
    case SWING_CUT:
      if ( m_swing < 10 )
	deg = (double)m_swing/10*120-30;
      else if ( m_swing < 30 )
	deg = 90-(double)(m_swing-10)/20*90;
      else
	deg = -(double)(m_swing-30)/20*30;
      break;
    }

    if ( m_swing == 0 ){
      x = y = 0.0;
      return true;
    }

    // The distance from the ideal location for hitting
    if ( btx - px > -0.6 ){
      x = btx - px + 0.3;
      y = 0.0;
    }
    else{
      x = -0.3;
      y = 0.0;
    }
  }

  if ( m_swing < 30 ) {
    x *= (double)m_swing / 30.0;
    y *= (double)m_swing / 30.0;
  } else {
    x *= (double)(50-m_swing) / 20.0;
    y *= (double)(50-m_swing) / 20.0;
  }

  return true;
}

// Calc the elbow location
// degx --- rotation degree around x axis
// degy --- rotation degree around y axis. default=-15
bool
Player::GetElbow( double &degx, double &degy ) {
  if ( ForeOrBack() ){
    switch ( m_swingType ){
    case SWING_NORMAL:
    case SWING_SMASH:
      if ( m_swing < 10 )
	degx = 0.0;
      else if ( m_swing < 30 )
	degx = (double)(m_swing-10)/20*15;
      else
	degx = 15-(double)(m_swing-30)/20*15;
      degy = -15.0;
      break;
    case SWING_POKE:
      degx = 0.0;
      degy = -15.0;
      break;
    case SWING_CUT:
      if ( m_swing < 10 ){
	degx = 0.0;
	degy = -15-(double)m_swing/10*75;
      } else if ( m_swing < 30 ){
	degx = 0.0;
	degy = -90+(double)(m_swing-10)/20*90;
      } else {
	degx = 0.0;
	degy = -(double)(m_swing-30)/20*15;
      }
      break;
    case SWING_DRIVE:
      if ( m_swing < 10 ){
	degx = -(double)m_swing/10*10;
	degy = -15.0;
      } else if ( m_swing < 30 ){
	degx = -10+(double)(m_swing-10)/20*160;
	degy = -15+(double)(m_swing-10)/20*45;
      } else{
	degx = 150-(double)(m_swing-30)/20*150;
	degy = 30-(double)(m_swing-30)/20*45;
      }
      break;
    }
  }
  else{
    switch ( m_swingType ){
    case SWING_NORMAL:
    case SWING_SMASH:
    case SWING_DRIVE:
      degx = 0.0;
      degy = -15.0;
      break;
    case SWING_POKE:
      if ( m_swing < 10 ){
	degx = (double)m_swing/10*15;
	degy = -15.0;
      }
      else if ( m_swing < 30 ){
	degx = 15 - (double)(m_swing-10)/20*15;
	degy = -15.0;
      } else{
	degx = 0.0;
	degy = -15.0;
      }
      break;
    case SWING_CUT:
      if ( m_swing < 10 ){
	degx = (double)m_swing/10*10;
	degy = (double)m_swing/10*45;
      } else if ( m_swing < 30 ){
	degx = 10-(double)(m_swing-10)/20*10;
	degy = 45-(double)(m_swing-10)/20*45;
      } else{
	degx = 0.0;
	degy = -(double)(m_swing-30)/20*15;
      }
      break;
    }
  }
  return true;
}

// Calc the hand location
// degx --- rotation degree around x axis
// degy --- rotation degree around y axis
// degz --- rotation degree around z axis
bool
Player::GetHand( double &degx, double &degy, double &degz ) {

  if ( ForeOrBack() ){
    switch ( m_swingType ){
    case SWING_NORMAL:
    case SWING_SMASH:
      if ( m_swing < 10 ){
	degz = -(double)m_swing/10*90;
	degx = degy = 0.0;
      }
      else if ( m_swing < 30 ){
	degz = -90+(double)(m_swing-10)/20*120;
	degx = (double)(m_swing-10)/20*75;
	degy = 0.0;
      }
      else{
	degz = 30-(double)(m_swing-30)/20*30;
	degx = 75-(double)(m_swing-30)/20*75;
	degy = 0.0;
      }
      break;
    case SWING_POKE:
      if ( m_swing < 10 ){
	degz = -(double)m_swing/10*90;
	degx = (double)m_swing/10*45;
	degy = (double)m_swing/10*45;
      }
      else if ( m_swing < 30 ){
	degz = -90+(double)(m_swing-10)/20*150;
	degx = 45 - (double)(m_swing-10)/20*45;
	degy = 45.0;
      }
      else{
	degz = 60-(double)(m_swing-30)/20*30;
	degx = 0.0;
	degy = 45.0 - (double)(m_swing-30)/20*45;
      }
      break;
    case SWING_CUT:
      if ( m_swing < 10 ){
	degz = -(double)m_swing/10*90;
	degx = 0.0;
	degy = (double)m_swing/10*45;
      }
      else if ( m_swing < 30 ){
	degz = -90+(double)(m_swing-10)/20*150;
	degx = -(double)(m_swing-10)/20*60;
	degy = 45.0;
      }
      else{
	degz = 60-(double)(m_swing-30)/20*30;
	degx = -60+(double)(m_swing-30)/20*60;
	degy = 45.0 - (double)(m_swing-30)/20*45;
      }
      break;
    case SWING_DRIVE:
      if ( m_swing < 10 ){
	degz = -(double)m_swing/10*90;
	degx = -(double)m_swing/10*90;
	degy = 0.0;
      }
      else if ( m_swing < 30 ){
	degz = -90+(double)(m_swing-10)/20*120;
	degx = -90;
	degy = 0.0;
      }
      else{
	degz = 30-(double)(m_swing-30)/20*30;
	degx = -90+(double)(m_swing-30)/20*90;
	degy = 0.0;
      }
      break;
    }
  }
  else{
    switch ( m_swingType ){
    case SWING_NORMAL:
    case SWING_SMASH:
    case SWING_DRIVE:
      if ( m_swing < 10 ){
	degz = (double)m_swing/10*90;
	degx = degy = 0.0;
      }
      else if ( m_swing < 30 ){
	degz = 90-(double)(m_swing-10)/20*120;
	degx = (double)(m_swing-10)/20*75;
	degy = 0.0;
      }
      else{
	degz = -30+(double)(m_swing-30)/20*30;
	degx = 75-(double)(m_swing-30)/20*75;
	degy = 0.0;
      }
      break;
    case SWING_POKE:
      if ( m_swing < 10 ){
	degz = (double)m_swing/10*90;
	degx = 0.0;
	degy = -(double)m_swing/10*45;
      }
      else if ( m_swing < 30 ){
	degz = 90-(double)(m_swing-10)/20*120;
	degx = 0.0;
	degy = -45.0;
      }
      else{
	degz = -30+(double)(m_swing-30)/20*30;
	degx = 0.0;
	degy = -45.0 + (double)(m_swing-30)/20*45;
      }
      break;
    case SWING_CUT:
      if ( m_swing < 10 ){
	degz = (double)m_swing/10*90;
	degx = (double)m_swing/10*30;
	degy = -(double)m_swing/10*45;
      }
      else if ( m_swing < 30 ){
	degz = 90-(double)(m_swing-10)/20*120;
	degx = 30-(double)(m_swing-10)/20*90;
	degy = -45.0;
      }
      else{
	degz = -30+(double)(m_swing-30)/20*30;
	degx = -60+(double)(m_swing-30)/20*60;
	degy = -45.0 + (double)(m_swing-30)/20*45;
      }
      break;
    }
  }

  return true;
}

bool
Player::ForeOrBack() {
  return GetSwingSide();
}

bool
Player::SwingError() {
  double diff;

  if ( (m_y-theBall.GetY())*m_side < 0 )
    diff = fabs(m_y-theBall.GetY())/2;
  else
    diff = fabs(m_y-theBall.GetY());

  if ( diff < 0.03 )
    m_swingError = SWING_PERFECT;
  else if ( diff < 0.1 )
    m_swingError = SWING_GREAT;
  else if ( diff < 0.2 )
    m_swingError = SWING_GOOD;
  else
    m_swingError = SWING_BOO;

  return true;
}

bool
Player::Warp( double x, double y, double z, double vx, double vy, double vz ) {
  m_x = x;
  m_y = y;
  m_z = z;
  m_vx = vx;
  m_vy = vy;
  m_vz = vz;

  return true;
}

bool
Player::Warp( char *buf ) {
  char *b = buf;
  b = ReadDouble( b, m_x );
  b = ReadDouble( b, m_y );
  b = ReadDouble( b, m_z );
  b = ReadDouble( b, m_vx );
  b = ReadDouble( b, m_vy );
  b = ReadDouble( b, m_vz );

  return true;
}

bool
Player::ExternalSwing( long pow, double spin, long swingType, long swing ) {
  m_swing = swing;
  m_pow = pow;
  m_spin = spin;
  m_swingType =swingType;

  return true;
}

bool
Player::ExternalSwing( char *buf ) {
  char *b = buf;
  long swingSide;
  b = ReadLong( b, m_pow );
  b = ReadDouble( b, m_spin );
  b = ReadLong( b, m_swingType );
  b = ReadLong( b, swingSide );
  b = ReadLong( b, m_swing );

  m_swingSide = (bool)(swingSide != 0);

  return true;
}

char *
Player::SendSwing( char *buf ) {
  long l;
  double d;

  l = SwapLong(m_pow);
  memcpy( buf, (char *)&l, 4 );
  d = SwapDbl(m_spin);
  memcpy( &(buf[4]), (char *)&d, 8 );
  l = SwapLong(m_swingType);
  memcpy( &(buf[12]), (char *)&l, 4 );
  l = SwapLong((long)m_swingSide);
  memcpy( &(buf[16]), (char *)&l, 4 );
  l = SwapLong(m_swing);
  memcpy( &(buf[20]), (char *)&l, 4 );

#ifdef LOGGING
  Logging::GetLogging()->LogSendPSMessage( this );
#endif

  return buf;
}

char *
Player::SendLocation( char *buf ) {
  double d;

  d = SwapDbl(m_x);
  memcpy( buf, (char *)&d, 8 );
  d = SwapDbl(m_y);
  memcpy( &(buf[8]), (char *)&d, 8 );
  d = SwapDbl(m_z);
  memcpy( &(buf[16]), (char *)&d, 8 );
  d = SwapDbl(m_vx);
  memcpy( &(buf[24]), (char *)&d, 8 );
  d = SwapDbl(m_vy);
  memcpy( &(buf[32]), (char *)&d, 8 );
  d = SwapDbl(m_vz);
  memcpy( &(buf[40]), (char *)&d, 8 );

  UpdateLastSend();

#ifdef LOGGING
  Logging::GetLogging()->LogSendPVMessage( this );
#endif

  return buf;
}

bool
Player::SendAll( int sd ) {
  SendLong( sd, m_playerType );
  SendLong( sd, m_side );

  SendDouble( sd, m_x );
  SendDouble( sd, m_y );
  SendDouble( sd, m_z );
  SendDouble( sd, m_vx );
  SendDouble( sd, m_vy );
  SendDouble( sd, m_vz );

  SendLong( sd, m_status );
  SendLong( sd, m_swing );
  SendLong( sd, m_swingType );
  SendLong( sd, (long)m_swingSide );
  SendLong( sd, m_afterSwing );
  SendLong( sd, m_swingError );

  SendDouble( sd, m_targetX );
  SendDouble( sd, m_targetY );
  SendDouble( sd, m_eyeX );
  SendDouble( sd, m_eyeY );
  SendDouble( sd, m_eyeZ );

  SendLong( sd, m_pow );

  SendDouble( sd, m_spin );
  SendDouble( sd, m_stamina );

  SendLong( sd, m_statusMax );

  return true;
}

// Must be overridden
bool
Player::GetModifiedTarget( double &targetX, double &targetY ) {
  return false;
}

void
Player::CalcLevel( Ball *ball, double &diff, double &level, double &maxVy ) {
}

bool
Player::Swing( long power ) {
  return false;
}

bool
Player::StartSwing( long power ) {
  return false;
}

bool
Player::HitBall() {
  return false;
}

void
Player::UpdateLastSend() {
  m_lastSendCount = 0;

  m_lastSendX = m_x;
  m_lastSendY = m_y;
  m_lastSendZ = m_z;
  m_lastSendVX = m_vx;
  m_lastSendVY = m_vy;
  m_lastSendVZ = m_vz;
}

void
Player::AddError( double &vx, double &vy, double &vz ) {
  double v;
  double n1x, n1y, n1z, n2x, n2y, n2z;
  double radDiff, radRand;

#if 0
  radDiff = hypot( fabs(fabs(m_x-theBall.GetX())-0.3)/0.3, 
		   fabs(m_y-theBall.GetY())/0.3 );
  radDiff = sqrt( radDiff );
  radDiff *= (double)(200-m_status)/200*3.141592/12;
#else
  radDiff = (double)(200-m_status)/200*3.141592/12;
#endif

  v = sqrt(vx*vx+vy*vy+vz*vz);
  n1x = vy/hypot(vx, vy) * v*tan(radDiff);
  n1y = -vx/hypot(vx, vy) * v*tan(radDiff);
  n1z = 0;
  n2x = vx*vz/(v*hypot(vx, vy)) * v*tan(radDiff);
  n2y = vy*vz/(v*hypot(vx, vy)) * v*tan(radDiff);
  n2z = (vx*vx+vy*vy)/(v*hypot(vx, vy)) * v*tan(radDiff);

  // Hit the ball too fast --- net miss
  // Hit the ball too slow --- over miss
  if ( (m_y-theBall.GetY())*m_side < 0 )
    radRand = (RAND(180)+180)*3.141592/180.0;
  else
    radRand = RAND(180)*3.141592/180.0;

  vx += n1x*cos(radRand)+n2x*sin(radRand);
  vy += n1y*cos(radRand)+n2y*sin(radRand);
  vz += n1z*cos(radRand)+n2z*sin(radRand);
}

// If status point is less than this value, player will miss. 
long
Player::StatusBorder() {
  double nearEdge = TABLEWIDTH/2-fabs(m_targetX);
  if ( TABLELENGTH/4-fabs(fabs(m_targetY)-TABLELENGTH/4) < nearEdge )
    nearEdge = TABLELENGTH/4-fabs(fabs(m_targetY)-TABLELENGTH/4);

  return 50+(TABLELENGTH/4-nearEdge)*40;

}
