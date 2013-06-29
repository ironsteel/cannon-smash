/* $Id: SoloPlay.cpp,v 1.23 2003/07/19 14:25:44 nan Exp $ */

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
#include "SoloPlay.h"
#include "Player.h"
#include "Ball.h"
#include "Event.h"
#include "BaseView.h"
#include "RCFile.h"

extern RCFile *theRC;

extern Ball theBall;
extern long mode;
extern long wins;

//extern void CopyPlayerData( struct PlayerData& dest, Player* src );
extern void CopyPlayerData( Player& dest, Player* src );

SoloPlay::SoloPlay() {
  m_smash = false;
  m_smashCount = 0;
  m_smashPtr = -1;
}

SoloPlay::~SoloPlay() {
}

bool
SoloPlay::Init() {
  m_View = (PlayGameView *)View::CreateView( VIEW_PLAYGAME );

  m_View->Init( this );

  BaseView::TheView()->AddView( m_View );

  return true;
}

void
SoloPlay::Create( long player, long com ) {
  Control::ClearControl();

  m_theControl = new SoloPlay();
  m_theControl->Init();

#ifdef SCREENSHOT
  m_thePlayer = Player::Create( 0, 1, 1 );
  m_comPlayer = Player::Create( 0, -1, 1 );
#else
  m_thePlayer = Player::Create( player, 1, 0 );
  m_comPlayer = Player::Create( com, -1, 1 );
#endif

  m_thePlayer->Init();
  m_comPlayer->Init();
}

bool
SoloPlay::Move( SDL_keysym *KeyHistory, long *MouseXHistory,
		    long *MouseYHistory, unsigned long *MouseBHistory,
		    int Histptr ) {
  bool reDraw = false;
  long prevStatus = theBall.GetStatus();

  if ( KeyHistory[Histptr].unicode == 'Q' ) {
    wins = 0;
    mode = MODE_TITLE;
    return true;
  }

  if ( m_smashPtr >= 0 ) {	// Smash replay
    ReplayAction( Histptr );

    if ( Histptr == m_smashPtr ) {
      SmashEffect(false, Histptr);
      m_smashCount++;
      if ( m_smashCount > 1 ) {
	m_smashCount = 0;
	m_smashPtr = -1;
	m_smash = false;
      } else {
	m_smashPtr = SmashEffect(true, Histptr);
      }
    } else {
      theBall.Move();
#ifdef SCREENSHOT
      reDraw |= m_thePlayer->Move( NULL, NULL, NULL, NULL, 0 );
#else
      reDraw |= m_thePlayer->Move( KeyHistory, MouseXHistory,
				 MouseYHistory, MouseBHistory, Histptr );
#endif
      reDraw |= m_comPlayer->Move( NULL, NULL, NULL, NULL, 0 );

      Event::TheEvent()->BackTrack(Histptr);
    }

    return true;
  }

  theBall.Move();
#ifdef SCREENSHOT
  reDraw |= m_thePlayer->Move( NULL, NULL, NULL, NULL, 0 );
#else
  reDraw |= m_thePlayer->Move( KeyHistory, MouseXHistory,
			     MouseYHistory, MouseBHistory, Histptr );
#endif
  reDraw |= m_comPlayer->Move( NULL, NULL, NULL, NULL, 0 );

  // Check smash replay
  if ( theBall.GetStatus() == 1 && prevStatus != 1 ) {
    if ( hypot( theBall.GetVY(), theBall.GetVZ() ) > 8.0 ) {
      m_smash = true;
    } else
      m_smash = false;
  } else if ( theBall.GetStatus() == 2 || theBall.GetStatus() == 8 ) {
    m_smash = false;
  }

  if ( m_smash && theBall.GetStatus() < 0 )
    m_smashPtr = SmashEffect(true, Histptr);

  return true;
}

bool
SoloPlay::LookAt( double &srcX, double &srcY, double &srcZ,
		  double &destX, double &destY, double &destZ ) {
  if (m_thePlayer) {
    if ( m_smashPtr >= 0 ) {	// Smash replay
      switch (m_smashCount) {
      case 0:
	srcX = 0;
	srcY = TABLELENGTH*m_thePlayer->GetSide();
	srcZ = 1.6;

	destX = 0;
	destY = -TABLELENGTH/2*m_thePlayer->GetSide();
	destZ = TABLEHEIGHT;
	break;
      case 1:
	srcX = -TABLEWIDTH;
	srcY = 0;
	srcZ = 1.6;

	destX = theBall.GetX();
	destY = theBall.GetY();
	destZ = theBall.GetZ();
	break;
      default:
	printf( "%d\n", (int)m_smashCount );
      }
    } else {
      srcX = m_thePlayer->GetX() + m_thePlayer->GetEyeX();
      srcY = m_thePlayer->GetY() + m_thePlayer->GetEyeY();
      srcZ = m_thePlayer->GetZ() + m_thePlayer->GetEyeZ();
      destX = m_thePlayer->GetLookAtX();
      destY = m_thePlayer->GetLookAtY();
      destZ = m_thePlayer->GetLookAtZ();
    }
  }

  return true;
}

long
SoloPlay::SmashEffect( bool start, long histPtr ) {
  static Player p1, p2;
  static Ball b;
  static long score1, score2;

  long smashPtr;

  smashPtr = histPtr-1;
  if ( smashPtr < 0 )
    smashPtr = MAX_HISTORY-1;

  if (start) {
    CopyPlayerData( p1, m_thePlayer );
    CopyPlayerData( p2, m_comPlayer );
    b = theBall;
    score1 = m_Score1;
    score2 = m_Score2;

    for ( int i = 0 ; i < MAX_HISTORY ; i++ ) {
      Event::TheEvent()->BackTrack( histPtr );
      if ( theBall.GetStatus() == 2 )
	break;

      histPtr--;
      if ( histPtr < 0 )
	histPtr = MAX_HISTORY-1;
    }

    Event::TheEvent()->BackTrack( histPtr );
  } else {
    m_thePlayer->Reset( &p1 );
    m_comPlayer->Reset( &p2 );
    theBall = b;
    m_Score1 = score1;
    m_Score2 = score2;
  }

  return smashPtr;
}

void 
SoloPlay::ReplayAction( int &Histptr ) {
  static long delayCounter = 0;

  // If mouse button is pressed, stop replay
  // (Unless it is the last 1 second). 
  if ( Event::TheEvent()->m_mouseButton && m_smashCount < 1 ) {
    Histptr = m_smashPtr;
    SmashEffect( false, Histptr );
    m_smashCount = 0;
    m_smashPtr = -1;
    m_smash = false;

    return;
  }

  delayCounter++;
  if ( theBall.GetStatus() == 3 &&
       (delayCounter%5) != 0 ) {
    Histptr--;
    if ( Histptr < 0 )
      Histptr = MAX_HISTORY-1;
    Event::TheEvent()->BackTrack(Histptr);
    return;
  }
}
