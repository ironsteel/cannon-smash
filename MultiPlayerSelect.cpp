/* $Id: MultiPlayerSelect.cpp,v 1.8 2003/11/19 17:02:25 nan Exp $ */

// Copyright (C) 2003  神南 吉宏(Kanna Yoshihiro)
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
#include "MultiPlayerSelect.h"
#include "BaseView.h"
#include "Player.h"
#include "Sound.h"
#include "PracticeSelectView.h"
#include "Event.h"
#include "MultiPlay.h"
#include "Network.h"
#include "RCFile.h"

#ifdef LOGGING
#include "Logging.h"
#endif

extern long mode;

extern bool isComm;

extern long wins;

extern RCFile *theRC;
extern SDL_mutex *networkMutex;
extern int theSocket;

MultiPlayerSelect::MultiPlayerSelect() {
  m_rotate = 0; m_opponentRotate = 0;
  m_View = NULL;
  m_selected = 0; m_opponentSelected = 0;

  m_lastRotate = m_lastOpponentRotate = 0;

  m_isConnected = false;
}

MultiPlayerSelect::~MultiPlayerSelect() {
}

bool
MultiPlayerSelect::Init() {
  m_View = (PracticeSelectView *)View::CreateView( VIEW_PRACTICESELECT );

  m_View->Init( this );

  BaseView::TheView()->AddView( m_View );

  networkMutex = SDL_CreateMutex();
  SDL_CreateThread( MultiPlayerSelect::Connect, NULL );

  return true;
}

void
MultiPlayerSelect::Create() {
  Control::ClearControl();

  m_theControl = new MultiPlayerSelect();
  m_theControl->Init();

  SDL_ShowCursor(0);
}

bool
MultiPlayerSelect::Move( SDL_keysym *KeyHistory, long *MouseXHistory,
			 long *MouseYHistory, unsigned long *MouseBHistory,
			 int Histptr ) {
  if ( m_selected > 500 && m_opponentSelected > 500 ) {
    mode = MODE_MULTIPLAY;
    return true;
  }

  if ( MouseBHistory[Histptr]&BUTTON_LEFT && 
       ( (Histptr > 0 && !(MouseBHistory[Histptr-1]&BUTTON_LEFT)) ||
	 (Histptr == 0 && !(MouseBHistory[MAX_HISTORY]&BUTTON_LEFT)) ) ) {
    if ( m_selected == 0 ) {
      m_selected = 1;
      Sound::TheSound()->Play( SOUND_CLICK, 0, 0 );
      SendPT(1);
    }
  }

  if ( m_selected > 0 ) {
    m_selected++;
  } else {
    if ( m_lastRotate == 0 ) {
      if ( MouseXHistory[Histptr] - BaseView::GetWinWidth()/2 > 10 ) {
	m_lastRotate = 2;
      } else if ( MouseXHistory[Histptr] - BaseView::GetWinWidth()/2 < -10 ) {
	m_lastRotate = -2;
      }

      if ( m_lastRotate != 0 ) {
	m_rotate += m_lastRotate;
	if ( m_rotate < 0 )
	  m_rotate += 360;
	else
	  m_rotate %= 360;
	SendPT(0);
      }
    } else {
      long nextRotate = m_rotate + m_lastRotate;

      if ( nextRotate < 0 )
	nextRotate += 360;
      else
	nextRotate %= 360;

      if ( (m_rotate)/(360/PLAYERS) != nextRotate/(360/PLAYERS) ) {
	m_rotate = (nextRotate+360/PLAYERS/2)/(360/PLAYERS)*(360/PLAYERS);
	m_lastRotate = 0;
	Sound::TheSound()->Play( SOUND_CLICK, 0, 0 );
	SendPT(0);
      } else
	m_rotate = nextRotate;
    }
  }

  if ( m_opponentSelected > 0 ) {
    m_opponentSelected++;
  } else {
    if ( m_lastOpponentRotate != 0 ) {
      long nextRotate = m_opponentRotate + m_lastOpponentRotate;

      if ( nextRotate < 0 )
	nextRotate += 360;
      else
	nextRotate %= 360;

      if ( (m_opponentRotate)/(360/PLAYERS) != nextRotate/(360/PLAYERS) ) {
	m_opponentRotate = (nextRotate+360/PLAYERS/2)/(360/PLAYERS)*(360/PLAYERS);
	m_lastOpponentRotate = 0;
      } else
	m_opponentRotate = nextRotate;
    }
  }

  return true;
}

long
MultiPlayerSelect::GetOpponentNum() {
  if ( GetOpponentRotate() < 0 )
    return (360+(GetOpponentRotate()%360))/(360/PLAYERS);
  else
    return (GetOpponentRotate()%360)/(360/PLAYERS);
}

// Make connection with opponent
int
MultiPlayerSelect::Connect( void *dum ) {
  long side;

  if ( !(theRC->serverName[0]) )
    side = 1;	// server side
  else
    side = -1;	// client side

  if ( side == 1 ) {
    try {
      WaitForClient();
    } catch ( NetworkError ) {
      xerror("%s(%d) WaitForClient", __FILE__, __LINE__);
      exit(1);
    }
    ServerAdjustClock();
  } else {
    try {
      ConnectToServer();
    } catch ( NetworkError ) {
      xerror("%s(%d) ConnectToServer", __FILE__, __LINE__);
      exit(1);
    }

    ClientAdjustClock();
  }

  ((MultiPlayerSelect *)m_theControl)->m_isConnected = true;

  if ( ((MultiPlayerSelect *)m_theControl)->m_selected > 0 )
    ((MultiPlayerSelect *)m_theControl)->SendPT(1);
  else
    ((MultiPlayerSelect *)m_theControl)->SendPT(0);

  SDL_CreateThread( MultiPlay::WaitForData, NULL );

  return 0;
}

void
MultiPlayerSelect::ReadPT( char *data ) {
  long rotate;

  // get player type
  ReadLong( &(data[1]), rotate );

  m_lastOpponentRotate = rotate%(360/PLAYERS);

  while ( m_lastOpponentRotate > 10 )
    m_lastOpponentRotate -= (360/PLAYERS);

  while ( m_lastOpponentRotate < -10 )
    m_lastOpponentRotate += (360/PLAYERS);

  m_opponentRotate = rotate;

  if ( data[0] != 0 )
    m_opponentSelected = 1;
}

void
MultiPlayerSelect::SendPT( char fixed ) {
  long rotate;

  if ( theSocket < 0 || !m_isConnected )
    return;

  send( theSocket, "PT", 2, 0 );
  send( theSocket, &fixed, 1, 0 );
  rotate = GetRotate();
  SendLong( theSocket, rotate );

#ifdef LOGGING
  char buf[256];

  sprintf( buf, "fixed=%d rotate=%d\n", fixed, rotate );
  Logging::GetLogging()->LogTime( LOG_COMMISC );
  Logging::GetLogging()->Log( LOG_COMMISC, buf );
#endif
}
