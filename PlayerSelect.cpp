/* $Id: PlayerSelect.cpp,v 1.23 2003/07/19 14:25:44 nan Exp $ */

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

#include "ttinc.h"
#include "PlayerSelect.h"
#include "BaseView.h"
#include "Player.h"
#include "Sound.h"
#include "PlayerSelectView.h"
#include "PlayerSelectView2D.h"
#include "Event.h"
#include "RCFile.h"

extern RCFile *theRC;

extern long mode;

extern bool isComm;

extern long wins;

PlayerSelect::PlayerSelect() {
  m_rotate = 0;
  m_View = NULL;
  m_selected = 0;
}

PlayerSelect::~PlayerSelect() {
  if ( m_View ){
    BaseView::TheView()->RemoveView( m_View );
    delete m_View;
  }
}

bool
PlayerSelect::Init() {
  m_View = (PlayerSelectView *)View::CreateView( VIEW_PLAYERSELECT );

  m_View->Init( this );

  BaseView::TheView()->AddView( m_View );

  return true;
}

void
PlayerSelect::Create() {
  Control::ClearControl();

  m_theControl = new PlayerSelect();
  m_theControl->Init();

  SDL_ShowCursor(SDL_DISABLE);
}

bool
PlayerSelect::Move( SDL_keysym *KeyHistory, long *MouseXHistory,
		    long *MouseYHistory, unsigned long *MouseBHistory,
		    int Histptr ) {
  static long lastRotate = 0;
  static long nothing = 0;

  // Next opponent
  if ( wins > 0 && m_selected == 0 ) {
    m_selected = 1;
    m_rotate = (m_thePlayer->GetPlayerType()-1)*(360/PLAYERS);
    return true;
  }

  if ( (KeyHistory[Histptr].unicode == SDLK_ESCAPE ||
	KeyHistory[Histptr].unicode == 'Q') && !isComm ) {
    mode = MODE_TITLE;
    wins = 0;
    return true;
  }

  if ( m_selected > 500 ) {
    if (isComm)
      mode = MODE_MULTIPLAY;
    else
      mode = MODE_SOLOPLAY;
    return true;
  }

  if ( MouseBHistory[Histptr]&BUTTON_LEFT && 
       ( (Histptr > 0 && !(MouseBHistory[Histptr-1]&BUTTON_LEFT)) ||
	 (Histptr == 0 && !(MouseBHistory[MAX_HISTORY]&BUTTON_LEFT)) ) ){
    nothing = 0;
    if ( m_selected == 0 ) {
      m_selected = 1;
      Sound::TheSound()->Play( SOUND_CLICK, 0, 0 );
    } else if ( m_selected > 100 ) {
      if (isComm)
	mode = MODE_MULTIPLAY;
      else
	mode = MODE_SOLOPLAY;
      return true;
    }
  }

  if ( m_selected > 0 ) {
    m_selected++;
    return true;
  }

  if ( lastRotate == 0 ) {
    if ( MouseXHistory[Histptr] - BaseView::GetWinWidth()/2 > 10 ) {
      nothing = 0;
      lastRotate = 2;
    } else if ( MouseXHistory[Histptr] - BaseView::GetWinWidth()/2 < -10 ) {
      nothing = 0;
      lastRotate = -2;
    } else
      nothing++;

    if ( lastRotate != 0 ) {
      m_rotate += lastRotate;
      if ( m_rotate < 0 )
	m_rotate += 360;
      else
	m_rotate %= 360;
    }
  } else {
    long nextRotate = m_rotate + lastRotate;

    if ( nextRotate < 0 )
      nextRotate += 360;
    else
      nextRotate %= 360;

    if ( m_rotate/(360/PLAYERS) != nextRotate/(360/PLAYERS) ) {
      m_rotate = (nextRotate+360/PLAYERS/2)/(360/PLAYERS)*(360/PLAYERS);
      lastRotate = 0;
      Sound::TheSound()->Play( SOUND_CLICK, 0, 0 );
    } else
      m_rotate = nextRotate;
  }

  if ( nothing > 1000 && !isComm ) {
    nothing = 0;
    //mode = MODE_DEMO;
    mode = MODE_TITLE;
  }

  if ( nothing != 0 )
    return false;
  else
    return true;
}

long
PlayerSelect::GetPlayerNum() {
  if ( GetRotate() < 0 )
    return (360+(GetRotate()%360))/(360/PLAYERS);
  else
    return (GetRotate()%360)/(360/PLAYERS);
}

long
PlayerSelect::GetOpponentNum() {
  return (GetPlayerNum()+wins+1)%PLAYERS;
}

bool
PlayerSelect::LookAt( double &srcX, double &srcY, double &srcZ,
		      double &destX, double &destY, double &destZ ) {
  srcX = 0.0;
  srcY = -TABLELENGTH-1.2;
  srcZ = 1.4;

  return true;
}
