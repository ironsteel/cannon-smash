/* $Id: Ball.h,v 1.15 2003/08/17 12:37:53 nan Exp $ */

// Copyright (C) 2000, 2003  神南 吉宏(Kanna Yoshihiro)
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

#ifndef _Ball_
#define _Ball_

#include "BallView.h"
#include "BallView2D.h"
#include "Player.h"

class Ball {
  friend class Opening;
public:
  Ball();
  Ball( double _x, double _y, double _z, double _vx, double _vy, double _vz,
	double _spin, long _status );
  virtual ~Ball();

  virtual bool Init();

  double GetX() { return m_x; }
  double GetY() { return m_y; }
  double GetZ() { return m_z; }
  double GetVX() { return m_vx; }
  double GetVY() { return m_vy; }
  double GetVZ() { return m_vz; }
  double GetSpin() { return m_spin; }

  long GetStatus() { return m_status; }

  bool Move();	// move to 1turn(0.01 sec) later

  bool Hit( double vx, double vy, double vz, double spin, Player *player );
  						// hit ball
  bool Toss( Player *player, long power );	// toss

  void Warp( double x, double y, double z, double vx, double vy, double vz, 
	     double spin, long status );
  void Warp( char *buf );

  // Calc vy, vz from bound location
  bool TargetToV( double targetX, double targetY, double height, double spin, 
		  double &vx, double &vy, double &vz, double vMin = 0.1,
		  double vMax = 30.0 );
  // For serve
  bool TargetToVS( double targetX, double targetY, double height, double spin, 
		   double &vx, double &vy, double &vz );

  char * Send( char *buf );

  BallView *GetView() { return m_View; };
protected:
  double m_x;		// ball location
  double m_y;
  double m_z;
  double m_vx;		// ball velocity
  double m_vy;
  double m_vz;

  double m_spin;	// spin. plus --- top spin   minus --- back spin

  long m_status;	// 0 --- From the time side=1 hit to bound
  			// 1 --- During side=-1 can hit
  			// 2 --- From the time side=-1 hit to bound
  			// 3 --- During side=1 can hit
  			// 4 --- From the time side=1 serve to bound
  			// 5 --- From the time side=-1 serve to bound
  			// 6 --- From the time side=1 toss to hit
  			// 7 --- From the time side=-1 toss to hit
  			// 8 --- Until player serve
  			//-1 --- Ball dead

  BallView* m_View;

  long m_lastSendCount;

private:
  void BallDead();
};

#endif // _Ball_
