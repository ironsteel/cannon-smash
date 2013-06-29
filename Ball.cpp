/* $Id: Ball.cpp,v 1.36 2003/11/19 16:49:14 nan Exp $ */

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
#include "Ball.h"
#include "BaseView.h"
#include "Control.h"
#include "PlayGame.h"
#include "Sound.h"
#include "Event.h"
#include "Network.h"
#include "RCFile.h"
#include "LobbyClient.h"

#ifdef LOGGING
#include "Logging.h"
#endif

extern RCFile *theRC;

extern Ball theBall;
extern long mode;

#if 0
inline double LOG(double f) { return log(f); }
#else
inline double LOG(double f)
{
    if (f <= 0) {
	printf("log(%g)!\n", f);
	return -10e10;
    } else {
	return log(f);
    }
}
#endif

Ball::Ball() {
  m_x = m_y = m_vx = m_vy = m_spin = 0.0;
  m_status = -1000;

  m_View = NULL;

  m_lastSendCount = 0;
}

Ball::Ball( double x, double y, double z, double vx, double vy, double vz,
	    double spin, long status ) {
  m_x = x;
  m_y = y;
  m_z = z;
  m_vx = vx;
  m_vy = vy;
  m_vz = vz;
  m_spin = spin;
  m_status = status;

  m_View = NULL;

  m_lastSendCount = 0;
}

Ball::~Ball() {
  if ( m_View && &theBall == this ){
    BaseView::TheView()->RemoveView( m_View );
    delete m_View;
  }
}

bool
Ball::Init() {
  m_View = (BallView *)View::CreateView( VIEW_BALL );

  m_View->Init();
  BaseView::TheView()->AddView( m_View );

  return true;
}

bool
Ball::Move() {
  double netT , tableT;         /* Flag for bound on the table, hit net */
  double x , y, z;              /* Hold x,y */
  double tableY;                /* Hold y on bounding */

// Return ball immidiately when ball dead
  if ( m_status < 0 )
    m_status--;

  if ( m_status < -100 || m_status == 8 ){
    if ( Control::TheControl()->IsPlaying() ) {
      Player *player;

      if ( ((PlayGame *)Control::TheControl())->GetService() == Control::TheControl()->GetThePlayer()->GetSide() )
	player = Control::TheControl()->GetThePlayer();
      else
	player = Control::TheControl()->GetComPlayer();

      if ( ((PlayGame *)Control::TheControl())->GetService() > 0 ) {
	m_x = player->GetX()+0.3;
	m_y = player->GetY();
      } else {
	m_x = player->GetX()-0.3;
	m_y = player->GetY();
      }

      m_z = TABLEHEIGHT + 0.15;
      m_vx = 0.0;
      m_vy = 0.0;
      m_vz = 0.0;

      m_status = 8;

      if ( &theBall == this &&
	   ((PlayGame *)Control::TheControl())->IsGameEnd() == true ){
	BaseView::TheView()->EndGame();
	((PlayGame *)Control::TheControl())->EndGame();
      }

      // To Fix the possibility of score mismatch
      if ( mode == MODE_MULTIPLAY && &theBall == this &&
	   Control::TheControl()->GetThePlayer()->GetSide() > 0 ) {
	m_lastSendCount++;
	if ( m_lastSendCount >= 100 ) {
	  Event::TheEvent()->SendBall();
	  m_lastSendCount = 0;
	}
      }
    } else {
      m_x = Control::TheControl()->GetThePlayer()->GetX()+0.3;
      m_y = Control::TheControl()->GetThePlayer()->GetY();

      m_z = TABLEHEIGHT + 0.15;
      m_vx = 0.0;
      m_vy = 0.0;
      m_vz = 0.0;

      m_status = 8;
    }

    return true;
  }

/* Add velocity */ 
  x = m_x;
  y = m_y;
  z = m_z;
#if 0
  m_x += (m_vx*2-PHY*m_vx*fabs(m_vx)*TICK)/2*TICK;
  m_y += (m_vy*2-PHY*m_vy*fabs(m_vy)*TICK)/2*TICK;
  m_z += (m_vz*2-GRAV*TICK-PHY*m_vz*fabs(m_vz)*TICK)/2*TICK;
#else
  m_x += (m_vx*2-PHY*m_vx*TICK)/2*TICK;
  m_y += (m_vy*2-PHY*m_vy*TICK)/2*TICK;
  m_z += (m_vz*2-GRAV*TICK-PHY*m_vz*TICK)/2*TICK;
#endif

/* Collision check */
  if ( y*m_y <= 0.0 ){
    netT = fabs( y/((m_y-y)/TICK) );
    if ( z+(m_z-z)*netT/TICK < TABLEHEIGHT ||
	 z+(m_z-z)*netT/TICK > TABLEHEIGHT+NETHEIGHT ||
	 x+(m_x-x)*netT/TICK < -TABLEWIDTH/2-NETHEIGHT ||
	 x+(m_x-x)*netT/TICK > TABLEWIDTH/2+NETHEIGHT )
      netT = TICK*100;
  } else
    netT = TICK*100;

  if ( (z-TABLEHEIGHT)*(m_z-TABLEHEIGHT) <= 0.0 ){
    tableT = fabs( (z-TABLEHEIGHT)/((m_z-z)/TICK) );
    if ( tableT <= 0.0 || y+(m_y-y)*tableT/TICK < -TABLELENGTH/2 ||
	 y+(m_y-y)*tableT/TICK > TABLELENGTH/2 ||
	 x+(m_x-x)*tableT/TICK < -TABLEWIDTH/2 ||
	 x+(m_x-x)*tableT/TICK > TABLEWIDTH/2 )
      tableT = TICK*100;
  } else
    tableT = TICK*100;

  if ( netT < tableT ){	// Hit net
    m_vx *= 0.5;
    m_vy = -m_vy*0.2;
    m_spin = -m_spin*0.8;
    m_y = m_vy*(TICK-netT);
  }

  if ( tableT < netT ){	// Bound on the table
    if ( this == &theBall ) {
      Sound::TheSound()->Play( SOUND_TABLE, m_x, m_y );
    }
    tableY = y+m_vy*tableT;
    if ( tableY < 0 ){		// Table of my side
      switch( m_status ){
      case 2:
	m_status = 3;
	break;
      case 4:
	m_status = 0;
	break;
      default:
	BallDead();
      }
    } else {			// Table of opponent side
      switch( m_status ) {
      case 0:
	m_status = 1;
	break;
      case 5:
	m_status = 2;
	break;
      default:
	BallDead();
      }
    }

    m_vz -= GRAV*tableT;
    m_vz += -PHY*m_vz*tableT;
    m_vz *= -TABLE_E;
    m_z = TABLEHEIGHT + (TICK-tableT)*m_vz;
    m_vz -= GRAV*(TICK-tableT);
    m_vz += -PHY*m_vz*(TICK-tableT);

    m_vy += -PHY*m_vy*tableT;

    if ( m_vy > 0 )
      m_vy += m_spin*0.8;
    else
      m_vy -= m_spin*0.8;

    m_vy += -PHY*m_vy*(TICK-tableT);
    m_vx += -PHY*m_vx*TICK;

    m_spin *= 0.8;

    return true;
  }

/* Collision check with walls */
  if ( m_x < -AREAXSIZE/2 ){
    m_x = -AREAXSIZE/2;
    m_vx = -m_vx*TABLE_E/2;
    if ( this == &theBall ) {
      Sound::TheSound()->Play( SOUND_TABLE, m_x, m_y );
    }
    BallDead();
  }
  else if ( m_x > AREAXSIZE/2 ){
    m_x = AREAXSIZE/2;
    m_vx = -m_vx*TABLE_E/2;
    if ( this == &theBall ) {
      Sound::TheSound()->Play( SOUND_TABLE, m_x, m_y );
    }
    BallDead();
  }

  if ( m_y < -AREAYSIZE/2 ){
    m_y = -AREAYSIZE/2;
    m_vy = -m_vy*TABLE_E/2;
    if ( this == &theBall ) {
      Sound::TheSound()->Play( SOUND_TABLE, m_x, m_y );
    }
    BallDead();
  }
  else if ( m_y > AREAYSIZE/2 ){
    m_y = AREAYSIZE/2;
    m_vy = -m_vy*TABLE_E/2;
    if ( this == &theBall ) {
      Sound::TheSound()->Play( SOUND_TABLE, m_x, m_y );
    }
    BallDead();
  }

  if ( m_z < 0 ){
    m_z = 0;
    m_vz = -m_vz*TABLE_E;
    if ( m_vy > 0 )
      m_vy += m_spin*0.8;
    else
      m_vy -= m_spin*0.8;

    if ( this == &theBall ) {
      Sound::TheSound()->Play( SOUND_TABLE, m_x, m_y );
    }
    BallDead();
  }
  else if ( m_z > AREAZSIZE ){
    m_z = AREAZSIZE;
    m_vz = -m_vz*0.1;
    if ( this == &theBall ) {
      Sound::TheSound()->Play( SOUND_TABLE, m_x, m_y );
    }
    BallDead();
  }

// Gravity
  m_vz -= GRAV*TICK;


// Air resistance
  m_vx += -PHY*m_vx*TICK;
  m_vy += -PHY*m_vy*TICK;
  m_vz += -PHY*m_vz*TICK;

  return true;
}

// Ball inpact
bool
Ball::Hit( double vx, double vy, double vz, double spin, Player *player ) {
// Normal inpact
  if ( this == &theBall ) {
      Sound::TheSound()->Play( SOUND_RACKET, m_x, m_y );
  }

  m_spin = spin;

  m_vx = vx;
  m_vy = vy;
  m_vz = vz;

  // m_status should be updated by the information from the opponent
  if ( player == Control::TheControl()->GetComPlayer() &&
       mode == MODE_MULTIPLAY ) {
    return true;
  }

  if ( m_status == 6 )
    m_status = 4;
  else if ( m_status == 7 )
    m_status = 5;
  else if ( m_status == 3 )
    m_status = 0;
  else if ( m_status == 1 ) {
    m_status = 2;
  }

  if ( player == Control::TheControl()->GetThePlayer() &&
       mode == MODE_MULTIPLAY && &theBall == this ) {
    Event::TheEvent()->SendBall();
  }

  return true;
}

bool
Ball::Toss( Player *player , long power ) {
  m_vz = 2.5;
  m_spin = 0.0;
  if ( player->GetSide() > 0 )
    m_status = 6;
  else
    m_status = 7;

  if ( player == Control::TheControl()->GetThePlayer() &&
       mode == MODE_MULTIPLAY && &theBall == this ) {
    Event::TheEvent()->SendBall();
  }

  return true;
}

void
Ball::Warp( double x, double y, double z, double vx, double vy, double vz, 
	    double spin, long status ) {
  m_x = x;
  m_y = y;
  m_z = z;
  m_vx = vx;
  m_vy = vy;
  m_vz = vz;

  m_spin = spin;
  m_status = status;
}

void
Ball::Warp( char *buf ) {
  char *b = buf;
  long nextStatus;

  b = ReadDouble( b, m_x );
  b = ReadDouble( b, m_y );
  b = ReadDouble( b, m_z );
  b = ReadDouble( b, m_vx );
  b = ReadDouble( b, m_vy );
  b = ReadDouble( b, m_vz );

  b = ReadDouble( b, m_spin );
  b = ReadLong( b, nextStatus );

  if ( m_status >= 0 && nextStatus < 0 )
    BallDead();
  else
    m_status = nextStatus;
}

// Get ball initial speed from current ball location, target, height
// target --- The point where ball will bound
// level ---  hit power(percentage)
bool
Ball::TargetToV( double targetX, double targetY, double level, double spin, 
		 double &vx, double &vy, double &vz, double vMin, 
		 double vMax ) {
  double y;
  double vyMin = 0.1, vyMax;
  double t1, t2, z1;

  vyMax = fabs(targetY-m_y)/hypot(targetX-m_x, targetY-m_y)*vMax;

  if ( targetY < m_y ){
    y = -m_y;
    targetY = -targetY;
  } else
    y = m_y;

  if ( targetY*y >= 0 ) {	// Never go over the net
    vy = vyMax*level*0.5;

    // t2 = time until ball reaches targetY
    t2 = -LOG( 1- PHY*(targetY-y)/vy ) / PHY;

    // define vz which satisfy z=TABLEHEIGHT when t=t2
    if (0 != t2) {
	vz = (PHY*(TABLEHEIGHT-m_z)+GRAVITY(spin)*t2)/(1-exp(-PHY*t2)) -
	    GRAVITY(spin)/PHY;
	vx = PHY*(targetX-m_x) / (1-exp(-PHY*t2));
    } else {
	vz = m_z;
	vx = m_x;
    }


    if ( y != m_y )
      vy = -vy;

    return true;
  }

  while (vyMax-vyMin > 0.001) {
    vy = (vyMin+vyMax)/2;

    // t2 = time until ball reaches targetY
    t2 = -LOG( 1- PHY*(targetY-y)/vy ) / PHY;

    // t1 = time until ball reaches the net
    t1 = -LOG( 1- PHY*(-y)/vy ) / PHY;

    // define vz which satisfy z=TABLEHEIGHT when t=t2
    if (0 != t2) {
	vz = (PHY*(TABLEHEIGHT-m_z)+GRAVITY(spin)*t2)/(1-exp(-PHY*t2)) -
	    GRAVITY(spin)/PHY;
    } else {
	vz = m_z;
    }

    // z1 = height of the ball when t=t2
    z1 = -(vz+GRAVITY(spin)/PHY)*exp(-PHY*t1)/PHY - GRAVITY(spin)*t1/PHY +
      (vz+GRAVITY(spin)/PHY)/PHY;

    if ( z1 < TABLEHEIGHT+NETHEIGHT-m_z )
      vyMax = vy;
    else
      vyMin = vy;
  }

  vy *= level;

  t2 = -LOG( 1- PHY*(targetY-y)/vy ) / PHY;
  if (0 != t2) {
      vz = (PHY*(TABLEHEIGHT-m_z)+GRAVITY(spin)*t2)/(1-exp(-PHY*t2)) -
	  GRAVITY(spin)/PHY;
  } else {
      vz = m_z;
  }

  if ( y != m_y )
    vy = -vy;

  vx = PHY*(targetX-m_x) / (1-exp(-PHY*t2));

  return true;
}

bool
Ball::TargetToVS( double targetX, double targetY, double level, double spin, 
		  double &vx, double &vy, double &vz ) {
  double boundY = -TABLELENGTH/2;
  double y;
  double tmpVX = 0.0, tmpVY = 0.0, tmpVZ = 0.0;

  if ( targetY < m_y ){
    y = -m_y;
    targetY = -targetY;
  } else
    y = m_y;

  for ( boundY = -TABLELENGTH/2 ; boundY < 0 ; boundY += TICK ) {
    double vyMin = 0.1;
    double vyMax = 30.0;
    double vyCurrent, vzCurrent;
    double t1, t2, t3;
    double z;

    while (vyMax-vyMin > 0.001) {
      vy = (vyMin+vyMax)/2;

      // t2 = time until the ball reaches boundY
      t2 = -LOG( 1- PHY*(boundY-y)/vy ) / PHY;

      // define vz which satisfy z=TABLEHEIGHT when t=t2
      if (0 != t2) {
	  vz = (PHY*(TABLEHEIGHT-m_z)+GRAVITY(spin)*t2)/(1-exp(-PHY*t2)) -
	      GRAVITY(spin)/PHY;
      } else {
	  vz = m_z;
      }

      // Bound
      vyCurrent = vy*exp(-PHY*t2);
      vzCurrent = (vz+GRAVITY(spin)/PHY)*exp(-PHY*t2) - GRAVITY(spin)/PHY;

      vyCurrent += spin*0.8;
      vzCurrent *= -TABLE_E;

      t1 = -LOG( 1- PHY*(targetY-boundY)/vyCurrent ) / PHY;

      z = -( vzCurrent+GRAVITY(spin*0.8)/PHY)*exp(-PHY*t1)/PHY
	 - GRAVITY(spin*0.8)/PHY*t1
	 + (vzCurrent+GRAVITY(spin*0.8)/PHY)/PHY;

      if ( z > 0 )
	vyMax = vy;
      else
	vyMin = vy;
    }

    if ( fabs(z) < TICK ) {
      t3 = -LOG( 1- PHY*(-boundY)/vyCurrent ) / PHY;
      z = -( vzCurrent+GRAVITY(spin*0.8)/PHY)*exp(-PHY*t3)/PHY
	 - GRAVITY(spin*0.8)/PHY*t3
	 + (vzCurrent+GRAVITY(spin*0.8)/PHY)/PHY;
      if ( z > NETHEIGHT+(1.0-level)*0.1 ) {	// temporary
        if ( vy > tmpVY ) {
	  if (0 != t1+t2) {
	    tmpVX = PHY*(targetX-m_x) / (1-exp(-PHY*(t1+t2)));
	  } else {
	    tmpVX = vx;
	  }
	  tmpVY = vy;
	  tmpVZ = vz;
	}
      }
    }
  }
  vx = tmpVX;
  vy = tmpVY;

  if ( y != m_y )
    vy = -vy;

  vz = tmpVZ;

  return true;
}

void
Ball::BallDead() {
  if ( m_status >= 0 ) {
    if ( Control::TheControl()->IsPlaying() && &theBall == this ) {
      ((PlayGame *)Control::TheControl())->ChangeScore();

      m_status = -1;

      if ( mode == MODE_MULTIPLAY &&
	   Control::TheControl()->GetThePlayer()->GetSide() > 0 ) {
	Event::TheEvent()->SendBall();

	if ( LobbyClient::TheLobbyClient() )
	  LobbyClient::TheLobbyClient()->
	    SendSC( ((PlayGame *)Control::TheControl())->GetScore(1), 
		    ((PlayGame *)Control::TheControl())->GetScore(-1) );
      }
    }

    m_status = -1;
  }
}


char *
Ball::Send( char *buf ) {
  long l;
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
  d = SwapDbl(m_spin);
  memcpy( &(buf[48]), (char *)&d, 8 );
  l = SwapLong(m_status);
  memcpy( &(buf[56]), (char *)&l, 4 );

#ifdef LOGGING
  Logging::GetLogging()->LogBall( LOG_COMBALL, this );
#endif

  return buf;
}
