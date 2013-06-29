/* $Id: PenDrive.cpp,v 1.25 2003/07/25 17:28:27 nan Exp $ */

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
#include "PenDrive.h"
#include "Ball.h"
#include "Event.h"
#include "Network.h"
#include "Control.h"
#include "RCFile.h"

extern RCFile *theRC;

extern Ball   theBall;

extern long mode;

PenDrive::PenDrive() {
  m_playerType = PLAYER_PENDRIVE;
}

PenDrive::PenDrive(long side) : Player(side) {
  m_playerType = PLAYER_PENDRIVE;
}

PenDrive::PenDrive( long playerType, long side, double x, double y, double z,
		    double vx, double vy, double vz,long status, long swing,
		    long swingType, bool swingSide, long afterSwing,
		    long swingError,
		    double targetX, double targetY, double eyeX, double eyeY,
		    double eyeZ, long pow, double spin, double stamina,
		    long statusMax ) :
  Player( playerType, side, x, y, z, vx, vy, vz, status, swing, swingType,
	  swingSide, afterSwing, swingError, targetX, targetY,
	  eyeX, eyeY, eyeZ, pow, spin, stamina, statusMax ) {
}

PenDrive::~PenDrive() {
}

bool
PenDrive::AddStatus( long diff ) {
  // Add something in the future
  return Player::AddStatus( diff );
}

bool
PenDrive::Move( SDL_keysym *KeyHistory, long *MouseXHistory,
	      long *MouseYHistory, unsigned long *MouseBHistory,
	      int Histptr ) {
  double prevVx = m_vx;
  double prevVy = m_vy;

  Player::Move( KeyHistory, MouseXHistory, MouseYHistory,MouseBHistory,
		Histptr );

// Calc status
  if ( hypot( m_vx, m_vy ) < 1.5 )
    AddStatus( 2 );
  if ( hypot( m_vx-prevVx, m_vy-prevVy ) > 0.8-theRC->gameLevel*0.1 ) {
    AddStatus(-1);
  }


  return true;
}

bool
PenDrive::Swing( long spin ) {
  Ball *tmpBall;

  if ( theBall.GetStatus() == 6 || theBall.GetStatus() == 7 )
    return false;

  if ( m_swing > 10 && m_swing < 30 )
    return false;

  if ( m_swing >= 30 )
    AddStatus( -(50-m_swing)*2 );

  m_swing = 11;
  m_pow = 8;

  // Decide SwingType by the hit point and spin, etc. 
  // Calc the ball location of 0.1 second later
  tmpBall = new Ball( theBall.GetX(), theBall.GetY(), theBall.GetZ(),
		      theBall.GetVX(), theBall.GetVY(), theBall.GetVZ(),
		      theBall.GetSpin(), theBall.GetStatus() );

  for ( int i = 0 ; i < 10 ; i++ )
    tmpBall->Move();

  if ( spin < 3 )
    m_swingSide = false;
  else
    m_swingSide = true;

  SwingType( tmpBall, spin );

  delete tmpBall;

  if ( Control::TheControl()->GetThePlayer() == this &&
       mode == MODE_MULTIPLAY )
    ::SendSwing( this );

  return true;
}

bool
PenDrive::StartSwing( long spin ) { // Argument is valid only on serve
  Ball *tmpBall;

  if ( m_swing > 10 )
    return false;

  if ( m_swing == 0 ) {
    m_swing = 1;
    m_pow = 0;

    // Decide SwingType by the hit point and spin, etc. 
    // Calc the ball location of 0.2 second later
    tmpBall = new Ball( theBall.GetX(), theBall.GetY(), theBall.GetZ(),
			theBall.GetVX(), theBall.GetVY(), theBall.GetVZ(),
			theBall.GetSpin(), theBall.GetStatus() );

    for ( int i = 0 ; i < 20 ; i++ )
      tmpBall->Move();

    if ( (theBall.GetStatus() == 6 && m_side == 1) ||
	(theBall.GetStatus() == 7 && m_side == -1) ) {	// Serve
      switch ( spin-1 ) {
      case 0:
	m_spin = 0.2;	// straight
	m_swingType = SWING_NORMAL;
	break;
      case 1:
	m_spin = -0.1;	// knuckle
	m_swingType = SWING_POKE;
	break;
      case 2:
	m_spin = -0.6;
	m_swingType = SWING_POKE;
	break;
      }

      m_swingSide = true;

      if ( Control::TheControl()->GetThePlayer() == this &&
	   mode == MODE_MULTIPLAY )
	::SendSwing( this );
    } else {
      if ( (m_x-tmpBall->GetX())*m_side > 0 )
	m_swingSide = false;
      else
	m_swingSide = true;

      SwingType( tmpBall, spin );
    }

    delete tmpBall;
  }

  return true;
}

bool
PenDrive::HitBall() {
  double vx, vy, vz;
  double diff;
  double level;

  // Serve
  if ( ( (m_side == 1 && theBall.GetStatus() == 6) ||
         (m_side ==-1 && theBall.GetStatus() == 7) ) &&
       fabs( m_x-theBall.GetX() ) < 0.6 && fabs( m_y-theBall.GetY() ) < 0.3 ) {
    AddStatus( (long)-fabs(fabs(m_x-theBall.GetX())-0.3F)*100 );
    diff = fabs( m_y-theBall.GetY() )*0.3;

    SwingError();

    if ( fabs(m_targetY) < TABLELENGTH/16*2 )
      level = 0.95 - diff*1.0;
    else if ( fabs(m_targetY) < TABLELENGTH/16*4 )
      level = 0.93-diff*1.5;
    else if ( fabs(m_targetY) < TABLELENGTH/16*6 )
      level = 0.90-diff*2.0;
    else
      level = 0.90-diff*2.0;

    theBall.TargetToVS( m_targetX, m_targetY, level, m_spin, vx, vy, vz );

    theBall.Hit( vx, vy, vz, m_spin, this );
  } else if ( ((m_side == 1 && theBall.GetStatus() == 3) ||
	       (m_side ==-1 && theBall.GetStatus() == 1)) &&
	      fabs(m_x-theBall.GetX()) < 0.6 && 
	      ((GetSwingSide() && (m_x-theBall.GetX())*m_side < 0 ) ||
	       (!GetSwingSide() && (m_x-theBall.GetX())*m_side > 0 )) &&
	      (m_y-theBall.GetY())*m_side < 0.3 &&
	      (m_y-theBall.GetY())*m_side > -0.6 ) {
    double targetX, targetY;

    GetModifiedTarget( targetX, targetY );

    double maxVy;
    CalcLevel( &theBall, diff, level, maxVy );

    theBall.TargetToV( targetX, targetY, level, m_spin, vx, vy, vz,
		       0.1, maxVy );

    if ((m_status-StatusBorder())*3/2000.0 <
	fabs(fabs(m_x-theBall.GetX())-0.3))
      AddError( vx, vy, vz );

    // Reduce status
    m_afterSwing = (long)
      (hypot( theBall.GetVX()*0.8-vx, theBall.GetVY()*0.8+vy )
       * (1.0+diff*10.0) + fabs(m_spin)*5.0 + fabs(theBall.GetSpin())*4.0);

    if ( ForeOrBack() || m_swingType == SWING_POKE )
      AddStatus( -m_afterSwing*2 );
    else
      AddStatus( -m_afterSwing*3 );

    if ( m_status == 1 )
      m_afterSwing *= 3;

    theBall.Hit( vx, vy, vz, m_spin, this );
  } else {
    m_swingError = SWING_MISS;
    if ( Control::TheControl()->GetThePlayer() == this &&
	 mode == MODE_MULTIPLAY )
      Event::TheEvent()->SendBall();
  }

  return true;
}

bool
PenDrive::SwingType( Ball *ball, long spin ) {
  if ( (ball->GetStatus() == 3 && m_side == 1) ||
       (ball->GetStatus() == 1 && m_side == -1) ) {
    if ( fabs(ball->GetX()) < TABLEWIDTH/2 &&
	 fabs(ball->GetY()) < TABLELENGTH/2 &&
	 (ball->GetZ()-TABLEHEIGHT-NETHEIGHT)/fabs(ball->GetY()) <
	 NETHEIGHT/(TABLELENGTH/2)*0.5 ) {	// low ball on the table
      if ( ball->GetSpin() < 0 ) {
	m_swingType = SWING_POKE;
	//m_spin = -spin*0.2-0.3;
	m_spin = -0.7;
      } else {
	m_swingType = SWING_NORMAL;
	//m_spin = spin*0.2;
	m_spin = 0.4;
      }
    } else if ( ball->GetZ() < TABLEHEIGHT+NETHEIGHT*2 ) { // under the net
      if ( ForeOrBack() ) {
	m_swingType = SWING_DRIVE;
	//m_spin = spin*0.3+0.4;
	m_spin = 1.0;
      } else {
	if ( ball->GetSpin() < 0 ) {
	  m_swingType = SWING_POKE;
	  //m_spin = -spin*0.2-0.3;
	  m_spin = -0.7;
	} else {
	  m_swingType = SWING_NORMAL;
	  //m_spin = spin*0.2;
	  m_spin = 0.4;
	}
      }
    } else if ( fabs(ball->GetY()) < TABLELENGTH/2+1.0 &&
		ball->GetZ() > TABLEHEIGHT+NETHEIGHT && ForeOrBack() ) {
      m_swingType = SWING_SMASH;
      m_spin = 0.2;
    } else {
      m_swingType = SWING_NORMAL;
      //m_spin = spin*0.2;
      m_spin = 0.4;
    }
  } else {
    m_swingType = SWING_NORMAL;
    //m_spin = spin*0.2;
    m_spin = 0.4;
  }

  return true;
}

// Target will be modified by the spin
// (now invalid)
#if 0
bool
PenDrive::GetModifiedTarget( double &targetX, double &targetY ) {
  targetX = m_targetX;
  targetY = m_targetY + theBall.GetSpin()*m_side*0.4;

  return true;
}
#else
bool
PenDrive::GetModifiedTarget( double &targetX, double &targetY ) {
  targetX = m_targetX;
  targetY = m_targetY;

  return true;
}
#endif

void
PenDrive::CalcLevel( Ball *ball, double &diff, double &level, double &maxVy ) {
  double targetX, targetY;

  GetModifiedTarget( targetX, targetY );

  if ( (m_y-ball->GetY())*m_side < 0 )
    diff = fabs( m_y-ball->GetY() )*0.1;
  else
    diff = fabs( m_y-ball->GetY() )*0.15;

  diff *= fabs(ball->GetSpin())+1;

  SwingError();

  if ( m_swingType == SWING_DRIVE )
    level = 1 - fabs(targetY)/(TABLELENGTH/16)/40 -
      diff*fabs(targetY)/(TABLELENGTH/16)*0.8;
  else if ( m_swingType == SWING_NORMAL || m_swingType == SWING_SMASH )
    level = 1 - fabs(targetY)/(TABLELENGTH/16)/40 -
      diff*fabs(targetY)/(TABLELENGTH/16);
  else
    level = 1 - fabs(targetY)/(TABLELENGTH/16)/40 -
      diff*fabs(targetY)/(TABLELENGTH/16)*1.2;

  level *= (double)m_pow/20.0 + 0.5;

  if ( ForeOrBack() ) {
    switch ( m_swingType ) {
    case SWING_CUT:
    case SWING_POKE:
    case SWING_NORMAL:
      maxVy = hypot(ball->GetVX(), ball->GetVY())*0.4 + 12.0 -
	(fabs(m_spin)+fabs(ball->GetSpin()))*3.0;
      break;
    case SWING_DRIVE:
      maxVy = hypot(ball->GetVX(), ball->GetVY())*0.4 + 15.0 -
	(fabs(m_spin)+fabs(ball->GetSpin())/2)*3.0;
      break;
    case SWING_SMASH:
      maxVy = hypot(ball->GetVX(), ball->GetVY())*0.4 + 20.0 -
	(fabs(m_spin)+fabs(ball->GetSpin()))*3.0;
      break;
    default:
      maxVy = hypot(ball->GetVX(), ball->GetVY())*0.4 + 12.0 -
	(fabs(m_spin)+fabs(ball->GetSpin()))*3.0;
    }
  } else {
    switch ( m_swingType ) {
    case SWING_CUT:
    case SWING_POKE:
    case SWING_NORMAL:
    case SWING_DRIVE:
      maxVy = hypot(ball->GetVX(), ball->GetVY())*0.4 + 8.0 -
	(fabs(m_spin)+fabs(ball->GetSpin()))*4.0;
      break;
    case SWING_SMASH:
      maxVy = hypot(ball->GetVX(), ball->GetVY())*0.4 + 10.0 -
	(fabs(m_spin)+fabs(ball->GetSpin()))*4.0;
      break;
    default:
      maxVy = hypot(ball->GetVX(), ball->GetVY())*0.4 + 8.0 -
	(fabs(m_spin)+fabs(ball->GetSpin()))*4.0;
    }
  }

  if ( level > 1.0 )
    level = 1.0;
}
