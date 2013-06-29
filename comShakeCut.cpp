/* $Id: comShakeCut.cpp,v 1.19 2003/07/25 17:28:27 nan Exp $ */

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
#include "comShakeCut.h"
#include "Control.h"
#include "Ball.h"
#include "Player.h"
#include "PlayGame.h"
#include "RCFile.h"

extern RCFile *theRC;

extern Ball   theBall;

ComShakeCut::ComShakeCut() : ShakeCut(), ComPlayer() {
}

ComShakeCut::ComShakeCut(long side) : ShakeCut(side), ComPlayer() {
}

ComShakeCut::ComShakeCut( long playerType, long side,
			  double x, double y, double z,
			  double vx, double vy, double vz,
			  long status, long swing,
			  long swingType, bool swingSide,
			  long afterSwing, long swingError,
			  double targetX, double targetY,
			  double eyeX, double eyeY, double eyeZ,
			  long pow, double spin, double stamina,
			  long statusMax ) :
  ShakeCut( playerType, side, x, y, z, vx, vy, vz, status, swing, swingType,
	    swingSide, afterSwing, swingError, targetX, targetY,
	    eyeX, eyeY, eyeZ, pow, spin, stamina, statusMax ), ComPlayer() {
}

ComShakeCut::~ComShakeCut() {
}

bool
ComShakeCut::Move( SDL_keysym *KeyHistory, long *MouseXHistory,
		 long *MouseYHistory, unsigned long *MouseBHistory,
		 int Histptr ) {
  double prevVx = m_vx;
  double prevVy = m_vy;

  ShakeCut::Move( KeyHistory, MouseXHistory, MouseYHistory, MouseBHistory,
		Histptr );
  Think();

// Calc status
  if ( hypot( m_vx-prevVx, m_vy-prevVy ) > 0.8-theRC->gameLevel*0.1 ) {
    AddStatus(-1);
  }

  return true;
}

bool
ComShakeCut::Think() {
  double hitT;	// estimation time until ball reaches _hitX, _hitY
  double mx;

  // If the ball status changes, change _hitX, _hitY
  if ( _prevBallstatus != theBall.GetStatus() && theBall.GetStatus() >= 0 ){
    Hitarea( _hitX, _hitY );

    _prevBallstatus = theBall.GetStatus();
  }

  if ( theBall.GetVY() != 0.0 )
    hitT = (_hitY - theBall.GetY())/theBall.GetVY()-TICK;
  else
    hitT = -1.0;

  if ( fabs( _hitX-(m_x+m_side*0.3) ) < fabs( _hitX-(m_x-m_side*0.3) ) ||
       theBall.GetStatus() == 8 || _hitX*m_side > 0 )
    mx = m_x+m_side*0.3;
  else
    mx = m_x-m_side*0.3;

  if ( m_swing > 10 && m_swing <= 20 ) {
  } else {
    if ( hitT > 0.0 ) {
      double vx = (_hitX-mx)/hitT;
      if ( vx > m_vx+0.1 )
	m_vx += 0.1;
      else if ( vx < m_vx-0.1 )
	m_vx -= 0.1;
      else
	m_vx = vx;
    } else {
      if ( m_vx*fabs(m_vx*0.1)/2 < _hitX - mx )
	m_vx += 0.1;
      else
	m_vx -= 0.1;
    }
  }

  if ( m_swing > 10 && m_swing <= 20 ) {
  } else {
    if ( hitT > 0.0 ) {
      double vy = (_hitY-m_y)/hitT;
      if ( vy > m_vy+0.1 )
	m_vy += 0.1;
      else if ( vy < m_vy-0.1 )
	m_vy -= 0.1;
      else
	m_vy = vy;
    } else {
      if ( m_vy*fabs(m_vy*0.1)/2 < _hitY - m_y )
	m_vy += 0.1;
      else
	m_vy -= 0.1;
    }
  }

  if ( m_swing == 19 ) {
    Player *opponent;
    if ( m_side == -1 )
      opponent = Control::TheControl()->GetThePlayer();
    else
      opponent = Control::TheControl()->GetComPlayer();

    SetTargetX( opponent );
  }

  if ( (theBall.GetStatus() == 0 && m_side == -1) ||
       (theBall.GetStatus() == 1 && m_side == -1) ||
       (theBall.GetStatus() == 2 && m_side == 1) ||
       (theBall.GetStatus() == 3 && m_side == 1) ||
       (theBall.GetStatus() == 4 && m_side == -1) ||
       (theBall.GetStatus() == 5 && m_side == 1) ) {
    if ( m_vx > 5.0 )
      m_vx = 5.0;
    else if ( m_vx < -5.0 )
      m_vx = -5.0;
    if ( m_vy > 5.0 )
      m_vy = 5.0;
    else if ( m_vy < -5.0 )
      m_vy = -5.0;
  } else {
    if ( hypot( m_vx, m_vy ) >= 2.0 ) {
      double v = hypot( m_vx, m_vy );
      m_vx = m_vx/v*1.99;
      m_vy = m_vy/v*1.99;
    }
  }

  // Toss
  if ( theBall.GetStatus() == 8 &&
       ( (Control::TheControl()->IsPlaying() &&
	  ((PlayGame *)Control::TheControl())->GetService() == GetSide()) ||
	 (!Control::TheControl()->IsPlaying() && GetSide() == 1) ) &&
       fabs(m_vx) < 0.1 && fabs(m_vy) < 0.1 &&
       fabs(m_x+m_side*0.3-_hitX) < 0.1 && fabs(m_y-_hitY) < 0.1 &&
       m_swing == 0 ) {
    theBall.Toss( this, 2 );
    StartSwing( 3 );
    m_targetY = TABLELENGTH/6*m_side;

    return true;
  }

  if ( fabs( theBall.GetY()+theBall.GetVY()*0.1 - _hitY ) < 0.2 /*&&
	  m_swing == 0*/ ){
    // Calc the ball location of 0.1 second later. 
    // This part seems to be the same as Swing(). Consider again. 
    Ball *tmpBall;
    double tmpBallX, tmpBallY, tmpBallZ;
    double tmpX, tmpY;

    tmpBall = new Ball( theBall.GetX(), theBall.GetY(), theBall.GetZ(),
			theBall.GetVX(), theBall.GetVY(), theBall.GetVZ(),
			theBall.GetSpin(), theBall.GetStatus() );

    for ( int i = 0 ; i < 9 ; i++ )
      tmpBall->Move();
    tmpX = m_x+m_vx*0.08;
    tmpY = m_y+m_vy*0.08;

    if ( ((tmpBall->GetStatus() == 3 && m_side == 1) ||
	  (tmpBall->GetStatus() == 1 && m_side == -1)) &&
	 (tmpY-tmpBall->GetY())*m_side < 0.3 &&
	 (tmpY-tmpBall->GetY())*m_side > -0.6 &&
	 m_swing <= 10 ) {
      tmpBallX = tmpBall->GetX();
      tmpBallY = tmpBall->GetY();
      tmpBallZ = tmpBall->GetZ();

      // If the ball location becomes better at 1/100 second later, wait. 
      tmpBall->Move();
      if ( fabs(tmpY+m_vy*0.01-tmpBall->GetY()) < fabs(tmpY-tmpBallY) &&
	   fabs(tmpY-tmpBallY) > LEVELMARGIN ) {
	delete tmpBall;
	return true;
      }

      _hitX = tmpBallX;
      _hitY = tmpBallY;

      if ( (tmpBallZ-TABLEHEIGHT)/fabs(m_y - m_targetY) < 0.0 )
	m_targetY = TABLELENGTH/4*m_side;
      else if ( (tmpBallZ-TABLEHEIGHT)/fabs(m_y-m_targetY) < 0.1 )
	m_targetY = TABLELENGTH/3*m_side;
      else
	m_targetY = TABLELENGTH/16*6*m_side;

      if ( (m_x-tmpBallX)*m_side < 0 )
	Swing(3);
      else
	Swing(1);

      m_pow = 7 + theRC->gameLevel;
    }
    delete tmpBall;
  }

  return true;
}

// Calc hit point
// (1) If the ball haven't bound, calc bound point
// (2) Calc hit point from current ball location or bound location
bool
ComShakeCut::Hitarea( double &hitX, double &hitY ) {
  Ball *tmpBall;
  double max = -1.0;             /* highest point of the ball */
  double bestX = AREAXSIZE, bestY = AREAYSIZE;

  if ( (theBall.GetStatus() == 3 && m_side == 1) ||
       (theBall.GetStatus() == 2 && m_side == 1) ||
       (theBall.GetStatus() == 0 && m_side == -1) ||
       (theBall.GetStatus() == 1 && m_side == -1) ||
       (theBall.GetStatus() == 4 && m_side == -1) ||
       (theBall.GetStatus() == 5 && m_side == 1) ) {
    tmpBall = new Ball( theBall.GetX(), theBall.GetY(), theBall.GetZ(),
			theBall.GetVX(), theBall.GetVY(), theBall.GetVZ(),
			theBall.GetSpin(), theBall.GetStatus() );

    while ( tmpBall->GetStatus() != -1 ) {
      if ( (tmpBall->GetStatus() == 3 && m_side == 1) ||
	   (tmpBall->GetStatus() == 1 && m_side == -1) ) {
	if ( tmpBall->GetZ() > max ) {
	  max = tmpBall->GetZ();
	  if ( tmpBall->GetY() < TABLELENGTH/2 ) {
	    bestX = tmpBall->GetX();
	    bestY = tmpBall->GetY();
	  }
	}
	if ( tmpBall->GetVZ() < 0.0 && tmpBall->GetY() > TABLELENGTH/2 &&
	     tmpBall->GetZ() > TABLEHEIGHT ) {
	  bestX = tmpBall->GetX();
	  bestY = tmpBall->GetY();
	}
      }
      tmpBall->Move();
    }

    delete tmpBall;

    if ( bestX != AREAXSIZE && bestY != AREAYSIZE ) {
      hitX = bestX;
      hitY = bestY;
    }
  } else if ( theBall.GetStatus() == 8 ) {
    if ( (Control::TheControl()->IsPlaying() &&
	  ((PlayGame *)Control::TheControl())->GetService() == GetSide()) ||
	 GetSide() == 1 ) {
      if ( RAND(2) )
	hitX = m_targetX;
      else
	hitX = -m_targetX;
    } else
      hitX = 0.0;
    hitY = -(TABLELENGTH/2+0.2)*m_side;
  } else if ( theBall.GetStatus() < 6 ) {
    hitX = 0.0;
    hitY = -(TABLELENGTH/2+1.0)*m_side;
  }

  return true;
}

bool
ComShakeCut::SetTargetX( Player *opponent ) {
  double width;

  switch ( theRC->gameLevel ) {
  case LEVEL_EASY:
    width = TABLEWIDTH/4;
    break;
  case LEVEL_NORMAL:
    width = TABLEWIDTH/2;
    break;
  case LEVEL_HARD:
  case LEVEL_TSUBORISH:
    width = TABLEWIDTH;
    break;
  default:
    return false;
  }

  if ( opponent->GetPlayerType() == PLAYER_PENDRIVE ) {
    switch ( RAND(4) ) {
    case 0:
      m_targetX = -width*7/16;
      break;
    case 1:
      m_targetX = -width*5/16;
      break;
    case 2:
      m_targetX = -width*3/16;
      break;
    case 3:
      m_targetX = -width*1/16;
      break;
    }
  } else {
    switch ( RAND(8) ) {
    case 0:
      m_targetX = -width*7/16;
      break;
    case 1:
      m_targetX = width*5/16;
      break;
    case 2:
      m_targetX = -width*3/16;
      break;
    case 3:
      m_targetX = -width*1/16;
      break;
    case 4:
      m_targetX = width*1/16;
      break;
    case 5:
      m_targetX = width*3/16;
      break;
    case 6:
      m_targetX = width*5/16;
      break;
    case 7:
      m_targetX = width*7/16;
      break;
    }
  }

  if ( theRC->gameLevel == LEVEL_TSUBORISH ) {
    if ( opponent->GetX()+opponent->GetVX()*0.5 < 0.0 ) {
      m_targetX = width*7/16;
    } else {
      m_targetX = -width*7/16;
    }

    if ( RAND(4) == 0 ) {
      m_targetX = -m_targetX;
    }
  }

  if ( m_vx > 1.5 ) {
    m_targetX += TABLEWIDTH/2;
  } else if ( m_vx > 0.5 ) {
    m_targetX += TABLEWIDTH/4;
  } else if ( m_vx < -1.5 ) {
    m_targetX -= TABLEWIDTH/2;
  } else if ( m_vx < -0.5 ) {
    m_targetX -= TABLEWIDTH/4;
  }

  if ( m_targetX > TABLEWIDTH/2 )
    m_targetX = TABLEWIDTH*7/16;
  if ( m_targetX < -TABLEWIDTH/2 )
    m_targetX = -TABLEWIDTH*7/16;

  return true;
}
