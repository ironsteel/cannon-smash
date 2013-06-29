/* $Id: MultiPlay.cpp,v 1.74 2003/11/19 17:02:24 nan Exp $ */

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
#include "MultiPlay.h"
#include "Ball.h"
#include "Player.h"
#include "Event.h"
#include "BaseView.h"
#include "RCFile.h"
#include "Network.h"
#include "MultiPlayerSelect.h"
#include "NetPenAttack.h"
#include "NetPenDrive.h"
#include "NetShakeCut.h"

#ifdef LOGGING
#include "Logging.h"
#endif

#if !defined(WIN32)
#include <netinet/tcp.h>
#endif

#if defined(WIN32) || defined(__FreeBSD__)

typedef int socklen_t;		/* mimic Penguin's typedef */

#else	/* ! WIN32 */

#define closesocket(FD) close(FD)

#endif

extern long mode;
extern Ball theBall;
extern RCFile *theRC;
extern int theSocket;
extern long timeAdj;

extern void QuitGame();

int one=1;

SDL_mutex *networkMutex;

void
MultiPlay::StartServer() {
}

void
MultiPlay::StartClient() {
  //Event::TheEvent()->SendBall();
}

MultiPlay::MultiPlay() {
}

MultiPlay::~MultiPlay() {
}

bool
MultiPlay::Init() {
#ifdef LOGGING
  Logging::GetLogging()->StartLog();
#endif

  theRC->gameLevel = LEVEL_HARD;
  theRC->gameMode = GAME_11PTS;

  // Init timer again
  struct timeb tb;
  getcurrenttime(&tb);

  Event::m_lastTime = tb;

  m_View = (PlayGameView *)View::CreateView( VIEW_PLAYGAME );

  m_View->Init( this );

  BaseView::TheView()->AddView( m_View );

  return true;
}

void
MultiPlay::Create( long player, long com ) {
  long side;

  Control::ClearControl();

  m_theControl = new MultiPlay();
  m_theControl->Init();

  theBall.Warp( 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -1000 );

  if ( !(theRC->serverName[0]) )
    side = 1;	// server side
  else
    side = -1;	// client side

  m_thePlayer = Player::Create( player, side, 0 );
  m_comPlayer = Player::Create( com, -side, 0 );

  networkMutex = SDL_CreateMutex();

  if ( side == 1 ) {
    ((MultiPlay *)m_theControl)->StartServer();
  } else {
    ((MultiPlay *)m_theControl)->StartClient();
  }

  m_thePlayer->Init();
  m_comPlayer->Init();
}

bool
MultiPlay::Move( SDL_keysym *KeyHistory, long *MouseXHistory,
		 long *MouseYHistory, unsigned long *MouseBHistory,
		 int Histptr ) {
  bool reDraw = false;

  theBall.Move();
  reDraw |= m_thePlayer->Move( KeyHistory, MouseXHistory,
			       MouseYHistory, MouseBHistory, Histptr );
  reDraw |= m_comPlayer->Move( NULL, NULL, NULL, NULL, 0 );

  return reDraw;
}

bool
MultiPlay::LookAt( double &srcX, double &srcY, double &srcZ,
		   double &destX, double &destY, double &destZ ) {
  if (m_thePlayer) {
    srcX = m_thePlayer->GetX() + m_thePlayer->GetEyeX();
    srcY = m_thePlayer->GetY() + m_thePlayer->GetEyeY();
    srcZ = m_thePlayer->GetZ() + m_thePlayer->GetEyeZ();
    destX = m_thePlayer->GetLookAtX();
    destY = m_thePlayer->GetLookAtY();
    destZ = m_thePlayer->GetLookAtZ();
  }

  return true;
}


void
MultiPlay::SendTime( char *buf ) {
  char v;
  long sec, count;

  sec = Event::m_lastTime.time;
  count = Event::m_lastTime.millitm/10;

  count += timeAdj;

  while ( count >= 100 ) {
    count -= 100;
    sec++;
  }
  while ( count < 0 ) {
    count += 100;
    sec--;
  }

  memcpy( buf, (char *)&sec, 4 );
  v = (char)(count);
  memcpy( &(buf[4]), (char *)&v, 1 );
}

void
MultiPlay::EndGame() {
  QuitGame();
  //mode = MODE_TITLE;
}


int
MultiPlay::WaitForData( void *dum ) {
  fd_set rdfds;

  while (1) {
    FD_ZERO( &rdfds );
    FD_SET( (unsigned int)theSocket, &rdfds );

    if ( select( theSocket+1, &rdfds, NULL, NULL, NULL ) > 0 ) {
      bool ret;
      long side;
      if ( !(theRC->serverName[0]) )
	side = -1;
      else
	side = 1;

      if ( m_comPlayer )
	side = m_comPlayer->GetSide();

      SDL_mutexP( networkMutex );
      ret = Event::TheEvent()->GetExternalData( side );
      SDL_mutexV( networkMutex );

      if ( !ret ) {
	break;
      }
    } else {
      printf( "Select failed\n" );
      break;
    }
  }

  return 0;
}


ExternalData::ExternalData() {
  side = 1;
  dataType = 0;
  sec = 0;
  count = 0;
  memset( data, 0, 256 );
  next = NULL;
}

ExternalData::ExternalData( long s ) {
  side = s;
  dataType = 0;
  sec = 0;
  count = 0;
  memset( data, 0, 256 );
  next = NULL;
}

ExternalData::~ExternalData() {
}

void
ExternalData::ReadTime( int sd, long *sec, char *count ) {
  char buf[256];
  long len = 0;
  long ctmp;

  while (1) {
    if ( (len+=recv( sd, buf+len, 5-len, 0 )) == 5 )
      break;
  }
  memcpy( sec, &buf[0], 4 );
  memcpy( count, &buf[4], 1 );

  ctmp = *count;
  ctmp -= timeAdj;

  while ( ctmp >= 100 ) {
    ctmp -= 100;
    (*sec)++;
  }
  while ( ctmp < 0 ) {
    ctmp += 100;
    (*sec)--;
  }
  *count = (char)ctmp;
}

ExternalData *
ExternalData::ReadData( long s ) {
  char buf[256];
  ExternalData *extNow;

  ReadHeader( theSocket, buf );

  if ( !strncmp( buf, "PV", 2 ) ) {
    extNow = new ExternalPVData(s);
    extNow->Read( theSocket );
  } else if ( !strncmp( buf, "PS", 2 ) ) {
    extNow = new ExternalPSData(s);
    extNow->Read( theSocket );
  } else if ( !strncmp( buf, "BV", 2 ) ) {
    extNow = new ExternalBVData(s);
    extNow->Read( theSocket );
  } else if ( !strncmp( buf, "PT", 2 ) ) {
    extNow = new ExternalPTData(s);
    extNow->Read( theSocket );
  } else if ( !strncmp( buf, "QT", 2 ) ) {
    QuitGame();
    //mode = MODE_TITLE;
    return NULL;
  } else {
    printf( "Comm Error!!\n" );
    return NULL;
  }

  return extNow;
}


ExternalPVData::ExternalPVData() : ExternalData() {
  dataType = DATA_PV;
}

ExternalPVData::ExternalPVData( long s ) : ExternalData(s) {
  dataType = DATA_PV;
}

bool
ExternalPVData::Apply( Player *targetPlayer, bool &fThePlayer,
		       bool &fComPlayer, bool &fTheBall ) {
  targetPlayer->Warp( data );
  if ( targetPlayer == Control::GetThePlayer() )
    fThePlayer = true;
  else if ( targetPlayer == Control::GetComPlayer() )
    fComPlayer = true;

#ifdef LOGGING
  Logging::GetLogging()->LogRecvPVMessage( this );
#endif

  return true;
}

bool
ExternalPVData::Read( long sock ) {
  ReadTime( sock, &sec, &count );

  long len = 0;
  while (1) {
    if ( (len+=recv( sock, data+len, 48-len, 0 )) == 48 )
      break;
  }

#ifdef LOGGING
  char buf[256];
  Logging::GetLogging()->LogTime( LOG_COMCOMPLAYER );
  sprintf( buf, "Recv PV: %d.%3d\n", (int)sec, (int)count );
  Logging::GetLogging()->Log( LOG_COMCOMPLAYER, buf );
#endif

  return true;
}


ExternalPSData::ExternalPSData() : ExternalData() {
  dataType = DATA_PS;
}

ExternalPSData::ExternalPSData( long s ) : ExternalData(s) {
  dataType = DATA_PS;
}

bool
ExternalPSData::Apply( Player *targetPlayer, bool &fThePlayer,
		       bool &fComPlayer, bool &fTheBall ) {
  targetPlayer->ExternalSwing( data );
  if ( targetPlayer == Control::GetThePlayer() )
    fThePlayer = true;
  else if ( targetPlayer == Control::GetComPlayer() )
    fComPlayer = true;

#ifdef LOGGING
  Logging::GetLogging()->LogRecvPSMessage( this );
#endif

  return true;
}

bool
ExternalPSData::Read( long sock ) {
  ReadTime( sock, &sec, &count );

  long len = 0;
  while (1) {
    if ( (len+=recv( sock, data+len, 24-len, 0 )) == 24 )
      break;
  }

#ifdef LOGGING
  char buf[256];
  Logging::GetLogging()->LogTime( LOG_COMCOMPLAYER );
  sprintf( buf, "Recv PS: %d.%3d\n", (int)sec, (int)count );
  Logging::GetLogging()->Log( LOG_COMCOMPLAYER, buf );
#endif

  return true;
}


ExternalBVData::ExternalBVData() : ExternalData() {
  dataType = DATA_BV;
}

ExternalBVData::ExternalBVData( long s ) : ExternalData(s) {
  dataType = DATA_BV;
}

bool
ExternalBVData::Apply( Player *targetPlayer, bool &fThePlayer,
		       bool &fComPlayer, bool &fTheBall ) {
  theBall.Warp( data );
  fTheBall = true;

#ifdef LOGGING
  Logging::GetLogging()->LogRecvBVMessage( this );
#endif

  return true;
}

bool
ExternalBVData::Read( long sock ) {
  ReadTime( sock, &sec, &count );

  long len = 0;
  while (1) {
    if ( (len+=recv( sock, data+len, 60-len, 0 )) == 60 )
      break;
  }

#ifdef LOGGING
  char buf[256];
  Logging::GetLogging()->LogTime( LOG_COMBALL );
  sprintf( buf, "Recv BV: %d.%3d\n", (int)sec, (int)count );
  Logging::GetLogging()->Log( LOG_COMBALL, buf );
#endif

  return true;
}


ExternalPTData::ExternalPTData() : ExternalData() {
  dataType = DATA_PT;
}

ExternalPTData::ExternalPTData( long s ) : ExternalData(s) {
  dataType = DATA_PT;
}

bool
ExternalPTData::Apply( Player *targetPlayer, bool &fThePlayer,
		       bool &fComPlayer, bool &fTheBall ) {
  ((MultiPlayerSelect *)Control::TheControl())->ReadPT( data );

#ifdef LOGGING
  Logging::GetLogging()->LogRecvPTMessage( this );
#endif

  return true;
}

bool
ExternalPTData::Read( long sock ) {
  //ReadTime( sock, &sec, &count );

  long len = 0;
  while (1) {
    if ( (len+=recv( sock, data+len, 5-len, 0 )) == 5 )
      break;
  }

#ifdef LOGGING
  char buf[256];
  Logging::GetLogging()->LogTime( LOG_COMMISC );
  sprintf( buf, "Recv PT: \n" );
  Logging::GetLogging()->Log( LOG_COMMISC, buf );
#endif

  return true;
}
