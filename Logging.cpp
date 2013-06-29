/* $Id: Logging.cpp,v 1.12 2003/08/17 11:09:23 nan Exp $ */

// Copyright (C) 2001, 2002, 2003  ¿ÀÆî µÈ¹¨(Kanna Yoshihiro)
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
#ifdef LOGGING
#include "MultiPlay.h"
#include "Event.h"
#include "Ball.h"
#include "Player.h"
#include "Network.h"
#include "Logging.h"

Logging* Logging::m_logging = NULL;

Logging::Logging() {
    memset(m_fp, 0, sizeof(m_fp));
}

Logging::~Logging() {
  for ( int i = 0 ; i < 8 ; i++ ) {
    if (m_fp[i]) fclose( m_fp[i] );
  }
}

Logging*
Logging::GetLogging() {
  if ( !Logging::m_logging )
    Logging::m_logging = new Logging();

  return Logging::m_logging;
}

bool
Logging::Init() {
  static const char* fname[] = {
      "log/com_ball.log",
      "log/com_thePlayer.log", 
      "log/com_comPlayer.log",
      "log/com_misc.log", 
      "log/act_ball.log",
      "log/act_thePlayer.log", 
      "log/act_comPlayer.log",
      "log/act_misc.log",
  };

  for ( int i = 0 ; i < 8; i++ ) {
    if ( (m_fp[i] = fopen( fname[i], "w" )) == 0 )
      return false;
  }

  return true;
}

bool
Logging::Log( long logType, char *logString ) {
    if (m_fp[logType]) {
        fputs( logString, m_fp[logType] );
        fflush( m_fp[logType] );
    }
  return true;
}

bool
Logging::StartLog() {
  struct timeb tb;
  char buf[256];

#ifndef WIN32
  struct timeval tv;
  struct timezone tz;
#endif

#ifdef WIN32
  ftime( &tb );
#else
  gettimeofday( &tv, &tz );
  tb.time = tv.tv_sec;
  tb.millitm = tv.tv_usec/1000;
#endif

  snprintf( buf, sizeof(buf), "--- START LOGGING %d.%3d ---\n",
            (int)tb.time, (int)tb.millitm );

  for ( int i = 0 ; i < 7 ; i++ )
    Log( i, buf );

  return true;
}

bool
Logging::LogTime( long logType, struct timeb *tb ) {
  char buf[64];

  snprintf( buf, sizeof(buf), "%d.%3d: ", (int)tb->time, (int)tb->millitm );
  Log( logType, buf );

  return true;
}

bool
Logging::LogTime( long logType ) {
  char buf[64];
  long sec, count;

  sec = Event::m_lastTime.time;
  count = Event::m_lastTime.millitm/10;
  Event::GetAdjustedTime( sec, count );
  snprintf( buf, sizeof(buf), "%d.%2d: ", (int)sec, (int)count );
  Log( logType, buf );

  return true;
}

bool
Logging::LogBall( long logType, Ball *ball ) {
  char buf[1024];

  LogTime( logType );
  snprintf( buf, sizeof(buf),
           "status = %2d x = %4.2f y = %4.2f z = %4.2f "
           "vx = %4.2f vy = %4.2f vz = %4.2f spin = %3.2f\n",
	   (int)ball->GetStatus(), ball->GetX(), ball->GetY(), ball->GetZ(),
           ball->GetVX(), ball->GetVY(), ball->GetVZ(), ball->GetSpin() );
  Log( logType, buf );

  return true;
}

bool
Logging::LogPlayer( long logType, Player *player ) {
  char buf[1024];

  LogTime( logType );
  snprintf( buf, sizeof(buf),
            "playerType=%1d side=%2d x=%4.2f y=%4.2f z=%4.2f "
            "vx=%4.2f vy=%4.2f vz=%4.2f status=%2d "
            "swing=%2d swingType=%1d swingSide=%2d afterSwing=%2d "
            "swingError=%1d targetX=%4.2f targetY=%4.2f "
            "eyeX=%4.2f eyeY=%4.2f eyeZ=%4.2f "
            "lookAtX=%4.2f lookAtY=%4.2f lookAtZ=%4.2f "
            "pow=%1d spin=%3.2f stamina=%2.0f dragX=%2d dragY=%2d\n",
            (int)player->GetPlayerType(), (int)player->GetSide(), 
            player->GetX(), player->GetY(), player->GetZ(), 
            player->GetVX(), player->GetVY(), player->GetVZ(), 
            (int)player->GetStatus(), (int)player->GetSwing(),
            (int)player->GetSwingType(), (int)player->GetSwingSide(),
            (int)player->GetAfterSwing(), (int)player->GetSwingError(), 
            player->GetTargetX(), player->GetTargetY(),
            player->GetEyeX(), player->GetEyeY(), player->GetEyeZ(),
            player->GetLookAtX(), player->GetLookAtY(), player->GetLookAtZ(),
            (int)player->GetPower(), player->GetSpin(), player->GetStamina(),
            (int)player->GetDragX(), (int)player->GetDragY() );
  Log( logType, buf );

  return true;
}

bool
Logging::LogRecvBVMessage( ExternalBVData *bv ) {
  char buf[256];

  LogTime( LOG_COMBALL );
  snprintf( buf, sizeof(buf), "recv: %d.%3d ", (int)bv->sec, (int)bv->count );
  Log( LOG_COMBALL, buf );
  Ball *tmpBall = new Ball();
  tmpBall->Warp(bv->data);

  snprintf( buf, sizeof(buf),
            "x=%4.2f y=%4.2f z=%4.2f "
            "vx=%4.2f vy=%4.2f vz=%4.2f spin=%3.2f status=%2d\n",
	   tmpBall->GetX(), tmpBall->GetY(), tmpBall->GetZ(), 
	   tmpBall->GetVX(), tmpBall->GetVY(), tmpBall->GetVZ(), 
	   tmpBall->GetSpin(), (int)tmpBall->GetStatus() );
  Log( LOG_COMBALL, buf );

  return true;
}

bool
Logging::LogSendPVMessage( Player *player ) {
  char buf[256];

  LogTime( LOG_COMTHEPLAYER );
  snprintf( buf, sizeof(buf),
           "send PV: x=%4.2f y=%4.2f z=%4.2f vx=%4.2f vy=%4.2f vz=%4.2f\n",
	   player->GetX(), player->GetY(), player->GetZ(), 
	   player->GetVX(), player->GetVY(), player->GetVZ() );
  Log( LOG_COMTHEPLAYER, buf );

  return true;
}

bool
Logging::LogRecvPVMessage( ExternalPVData *pv ) {
  char buf[256];

  LogTime( LOG_COMCOMPLAYER );
  snprintf( buf, sizeof(buf),
            "recv PV: %d.%3d ", (int)pv->sec, (int)pv->count );
  Log( LOG_COMCOMPLAYER, buf );
  Player *tmpPlayer = new Player();
  tmpPlayer->Warp(pv->data);

  snprintf( buf, sizeof(buf),
            "x=%4.2f y=%4.2f z=%4.2f vx=%4.2f vy=%4.2f vz=%4.2f\n",
            tmpPlayer->GetX(), tmpPlayer->GetY(), tmpPlayer->GetZ(), 
            tmpPlayer->GetVX(), tmpPlayer->GetVY(), tmpPlayer->GetVZ() );
  Log( LOG_COMCOMPLAYER, buf );

  return true;
}

bool
Logging::LogSendPSMessage( Player *player ) {
  char buf[256];

  LogTime( LOG_COMTHEPLAYER );

  snprintf( buf, sizeof(buf),
            "send PS: pow=%2d spin=%3.2f swingType=%1d swingSide=%2d swing=%2d\n",
	   (int)player->GetPower(), player->GetSpin(),
	   (int)player->GetSwingType(), player->GetSwingSide(),
	   (int)player->GetSwing() );
  Log( LOG_COMTHEPLAYER, buf );

  return true;
}

bool
Logging::LogRecvPSMessage( ExternalPSData *ps ) {
  char buf[256];

  LogTime( LOG_COMCOMPLAYER );
  snprintf( buf, sizeof(buf),
            "recv PS: %d.%3d ", (int)ps->sec, (int)ps->count );
  Log( LOG_COMCOMPLAYER, buf );
  Player *tmpPlayer = new Player();
  tmpPlayer->ExternalSwing(ps->data);

  snprintf( buf, sizeof(buf),
           "pow=%2d spin=%3.2f swingType=%1d swingSide=%2d swing=%2d\n",
	   (int)tmpPlayer->GetPower(), tmpPlayer->GetSpin(),
	   (int)tmpPlayer->GetSwingType(), tmpPlayer->GetSwingSide(),
	   (int)tmpPlayer->GetSwing() );
  Log( LOG_COMCOMPLAYER, buf );

  return true;
}

bool
Logging::LogRecvPTMessage( ExternalPTData *pt ) {
  char buf[256];

  LogTime( LOG_COMMISC );
  snprintf( buf, sizeof(buf),
            "recv PT: %d.%3d ", (int)pt->sec, (int)pt->count );
  Log( LOG_COMMISC, buf );

  long rotate;
  ReadLong( &(pt->data[1]), rotate );

  snprintf( buf, sizeof(buf), "fixed=%d type=%ld\n", pt->data[0],
	    rotate );
  Log( LOG_COMMISC, buf );

  return true;
}

#endif
