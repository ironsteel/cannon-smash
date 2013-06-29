/* $Id: TrainingPenAttack.cpp,v 1.8 2003/07/16 15:55:24 nan Exp $ */

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
#include "TrainingPenAttack.h"
#include "Ball.h"

extern Ball   theBall;

TrainingPenAttack::TrainingPenAttack() : PenAttack() {
}

TrainingPenAttack::TrainingPenAttack(long side) : PenAttack(side) {
}

TrainingPenAttack::TrainingPenAttack( long playerType, long side,
				      double x, double y, double z,
				      double vx, double vy, double vz,
				      long status, long swing,
				      long swingType, bool swingSide,
				      long afterSwing, long swingError,
				      double targetX, double targetY,
				      double eyeX, double eyeY, double eyeZ,
				      long pow, double spin, double stamina,
				      long statusMax) :
  PenAttack( playerType, side, x, y, z, vx, vy, vz, status, swing, swingType,
	     swingSide, afterSwing, swingError, targetX, targetY,
	     eyeX, eyeY, eyeZ, pow, spin, stamina, statusMax ) {
}

TrainingPenAttack::~TrainingPenAttack() {
}

bool
TrainingPenAttack::AddStatus( long diff ) {
  // Add something in the future
  return true;
}

bool
TrainingPenAttack::Move( SDL_keysym *KeyHistory, long *MouseXHistory,
			 long *MouseYHistory, unsigned long *MouseBHistory,
			 int Histptr ) {
  Player::Move( KeyHistory, MouseXHistory, MouseYHistory,MouseBHistory,
		Histptr );

  m_vy = m_vz = 0.0;
  m_y = -(TABLELENGTH/2+0.3)*m_side;
  //m_targetX = -TABLEWIDTH/5*2*m_side;
  m_targetX = -TABLEWIDTH/5*m_side;
  //m_targetY = TABLELENGTH/16*5*m_side;
  m_targetY = TABLELENGTH/16*4*m_side;

  return true;
}

bool
TrainingPenAttack::HitBall() {
  double vx, vy, vz;

  // Serve
  if ( ( (m_side == 1 && theBall.GetStatus() == 6) ||
         (m_side ==-1 && theBall.GetStatus() == 7) ) &&
       fabs( m_x-theBall.GetX() ) < 0.6 && fabs( m_y-theBall.GetY() ) < 0.3 ){
    SwingError();

    theBall.TargetToVS( m_targetX, m_targetY, 0.8, 0.5, vx, vy, vz );

    theBall.Hit( vx, vy, vz, 0.5, this );
  } else {
    PenAttack::HitBall();
  }
  return true;
}
