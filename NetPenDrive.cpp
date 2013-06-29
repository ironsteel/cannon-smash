/* $Id: NetPenDrive.cpp,v 1.3 2003/07/16 16:03:38 nan Exp $ */

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
#include "NetPenDrive.h"
#include "Ball.h"
#include "Event.h"

extern Ball   theBall;

NetPenDrive::NetPenDrive() : PenDrive() {
}

NetPenDrive::NetPenDrive(long side) : PenDrive(side) {
}

NetPenDrive::NetPenDrive( long playerType, long side,
			  double x, double y, double z,
			  double vx, double vy, double vz,
			  long status, long swing, long swingType,
			  bool swingSide, long afterSwing,
			  long swingError,
			  double targetX, double targetY,
			  double eyeX, double eyeY, double eyeZ,
			  long pow, double spin, double stamina,
			  long statusMax ) :
  PenDrive( playerType, side, x, y, z, vx, vy, vz, status, swing, swingType,
	    swingSide, afterSwing, swingError, targetX, targetY,
	    eyeX, eyeY, eyeZ, pow, spin, stamina, statusMax ) {
}

NetPenDrive::~NetPenDrive() {
}

bool
NetPenDrive::Move( SDL_keysym *KeyHistory, long *MouseXHistory,
		   long *MouseYHistory, unsigned long *MouseBHistory,
		   int Histptr ) {
  PenDrive::Move( KeyHistory, MouseXHistory, MouseYHistory,MouseBHistory,
		  Histptr );

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
    if ( fabs(tmpY+m_vy*0.01-tmpBall->GetY()) > fabs(tmpY-tmpBallY) ) {
      if ( (m_x-tmpBallX)*m_side < 0 )
	Swing(3);
      else
	Swing(1);
    }
  }
  delete tmpBall;

  return true;
}
