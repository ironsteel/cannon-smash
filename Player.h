/* $Id: Player.h,v 1.21 2003/07/16 16:11:14 nan Exp $ */

// Copyright (C) 2000, 2001, 2002  神南 吉宏(Kanna Yoshihiro)
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

#ifndef _Player_
#define _Player_
#include "PlayerView.h"
#include "PlayerView2D.h"

// m_playerType
#define PLAYER_PROTO		0	// Prototype player
#define PLAYER_PENATTACK	1	// Pen Attack
#define PLAYER_SHAKECUT		2	// Cut
#define PLAYER_PENDRIVE		3	// Pen Drive


// m_swingType
#define SWING_NORMAL	0	// 
#define SWING_POKE	1	// push?
#define SWING_SMASH	2	// 
#define SWING_DRIVE	3	// 
#define SWING_CUT	4	// 
#define SWING_BLOCK	5	// 


class PlayerView;
class HitMark;
class Ball;

class Player {
  friend class Howto;
  friend class Opening;
  friend class OpeningView;
public:
  Player();
  Player( long side );
  Player( long playerType, long side, double x, double y, double z, 
	  double vx, double vy, double vz,long status, long swing, 
	  long swingType, bool swingSide, long afterSwing, long swingError, 
	  double targetX, double targetY, double eyeX, double eyeY,
	  double eyeZ, long pow, double spin, double stamina, long statusMax );

  virtual ~Player();

  void operator=(Player&);

  static Player* Create( long player, long side, long type );

  virtual bool Init();

  //virtual bool Reset( struct PlayerData *p );
  virtual bool Reset( Player *p );

  virtual bool Move( SDL_keysym *KeyHistory, long *MouseXHistory,
		     long *MouseYHistory, unsigned long *MouseBHistory,
		     int Histptr );

  virtual bool AddStatus( long diff );

  virtual View *GetView() { return m_View; };

  virtual long   GetSide() { return m_side; }
  virtual long   GetPlayerType() { return m_playerType; }

  virtual double GetX() { return m_x; }
  virtual double GetY() { return m_y; }
  virtual double GetZ() { return m_z; }
  virtual double GetVX() { return m_vx; }
  virtual double GetVY() { return m_vy; }
  virtual double GetVZ() { return m_vz; }
  virtual long   GetPower() { return m_pow; }
  virtual double GetSpin() { return m_spin; }
  virtual double GetTargetX() { return m_targetX; }
  virtual double GetTargetY() { return m_targetY; }
  virtual double GetEyeX() { return m_eyeX; }
  virtual double GetEyeY() { return m_eyeY; }
  virtual double GetEyeZ() { return m_eyeZ; }
  virtual double GetLookAtX() { return m_lookAtX; }
  virtual double GetLookAtY() { return m_lookAtY; }
  virtual double GetLookAtZ() { return m_lookAtZ; }
  virtual double GetStamina() { return m_stamina; }
  virtual long   GetStatus() { return m_status; }
  virtual long   GetSwing() { return m_swing; }
  virtual long   GetSwingType() { return m_swingType; }
  virtual bool   GetSwingSide() { return m_swingSide; }
  virtual long   GetSwingError() { return m_swingError; }
  virtual long   GetAfterSwing() { return m_afterSwing; }

  virtual long   GetDragX() { return m_dragX; }
  virtual long   GetDragY() { return m_dragY; }

  virtual bool   GetShoulder( double &x, double &y, double &deg );
  virtual bool   GetElbow( double &degx, double& degy );
  virtual bool   GetHand( double &degx, double &degy, double &degz );

  // true  -> forehand
  // false -> backhand
  virtual bool ForeOrBack();

  virtual bool Warp( double x, double y, double z,
		     double vx, double vy, double vz );
  virtual bool ExternalSwing( long pow, double spin, long swingType, long swing );

  virtual bool Warp( char *buf );
  virtual bool ExternalSwing( char *buf );

  virtual char * SendSwing( char *buf );
  virtual char * SendLocation( char *buf );
  virtual bool SendAll( int sd );

  virtual bool GetModifiedTarget( double &targetX, double &targetY );

  virtual void CalcLevel( Ball *ball, double &diff, double &level, double &maxVy );

  long StatusBorder();
protected:
  long m_playerType;	// Player type

  long m_side;		// 1  --- ( y < 0 )
			// -1 --- ( y > 0 )

  double m_x;		// player location
  double m_y;
  double m_z;
  double m_vx;		// player velocity
  double m_vy;
  double m_vz;

  long m_status;	// status gauge
  long m_swing;		// swing status
  long m_swingType;	// swing type
  bool m_swingSide;	// forehand or backhand
  long m_afterSwing;	// 
  long m_swingError;	// Error when hitting
                        // 0 --- Perfect
                        // 1 --- Great
                        // 2 --- Good
                        // 3 --- Boo
                        // 4 --- Miss
  double m_targetX;	// location of target circle
  double m_targetY;	// location of target circle

  double m_eyeX;	// Viewpoint
  double m_eyeY;
  double m_eyeZ;

  double m_lookAtX;
  double m_lookAtY;
  double m_lookAtZ;

  long m_pow;		// power
  double m_spin;	// topspin/backspin

  double m_stamina;

  long m_statusMax;	// Max status value

  long m_dragX;
  long m_dragY;		// Mouse drag

  PlayerView* m_View;

  double m_lastSendX;
  double m_lastSendY;
  double m_lastSendZ;
  double m_lastSendVX;
  double m_lastSendVY;
  double m_lastSendVZ;
  long m_lastSendCount;

  virtual bool KeyCheck( SDL_keysym *KeyHistory, long *MouseXHistory,
			 long *MouseYHistory, unsigned long *MouseBHistory,
			 int Histptr );
  virtual bool Swing( long power );
  virtual bool StartSwing( long power );

  virtual bool HitBall();

  virtual bool SwingError();

  void UpdateLastSend();

  void AddError( double &vx, double &vy, double &vz );
};

#endif // _Player_
