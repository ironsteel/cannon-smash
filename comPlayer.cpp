/* $Id: comPlayer.cpp,v 1.5 2001/12/05 15:39:25 nan Exp $ */

// Copyright (C) 2000  神南 吉宏(Kanna Yoshihiro)
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
#include "comPlayer.h"
#include "Ball.h"

extern Ball   theBall;

ComPlayer::ComPlayer() {
  _prevBallstatus = 0;
  _hitX = 0;
  _hitY = -TABLELENGTH/3;
}

ComPlayer::~ComPlayer() {
}

// Calculate top of the ball
double
ComPlayer::GetBallTop( double &maxX, double &maxY, Player *p ) {
  Ball *tmpBall;
  double max = -1.0;             /* highest point of the ball */

  tmpBall = new Ball( theBall.GetX(), theBall.GetY(), theBall.GetZ(),
		      theBall.GetVX(), theBall.GetVY(), theBall.GetVZ(),
		      theBall.GetSpin(), theBall.GetStatus() );

  while ( tmpBall->GetStatus() != -1 ) {
    if ( (tmpBall->GetStatus() == 3 && p->GetSide() == 1) ||
	 (tmpBall->GetStatus() == 1 && p->GetSide() == -1) ) {
      if ( tmpBall->GetZ() > max &&
	   fabs(tmpBall->GetY()) < TABLELENGTH/2+1.0 &&
	   fabs(tmpBall->GetY()) > TABLELENGTH/2-0.5 ) {
	max = tmpBall->GetZ();
	maxX = tmpBall->GetX();
	maxY = tmpBall->GetY();
      }
    }
    tmpBall->Move();
  }

  delete tmpBall;
  return max;
}
