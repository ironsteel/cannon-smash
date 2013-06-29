/* $Id: Logging.h,v 1.5 2003/01/15 14:10:28 nan Exp $ */

// Copyright (C) 2001, 2002, 2003  神南 吉宏(Kanna Yoshihiro)
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

#ifndef _Logging_
#define _Logging_

#define LOG_COMBALL		0
#define LOG_COMTHEPLAYER	1
#define LOG_COMCOMPLAYER	2
#define LOG_COMMISC		3
#define LOG_ACTBALL		4
#define LOG_ACTTHEPLAYER	5
#define LOG_ACTCOMPLAYER	6
#define LOG_ACTMISC		7

class Player;
class Ball;
class ExternalBVData;
class ExternalPVData;
class ExternalPSData;
class ExternalPTData;

class Logging {
public:
  ~Logging();
  static Logging* GetLogging();
  bool Init();

  bool Log( long logType, char *logString );
  bool StartLog();
  bool LogTime( long logType, struct timeb *tb );
  bool LogTime( long logType );
  bool LogBall( long logType, Ball *ball );
  bool LogPlayer( long logType, Player *player );
  bool LogSendBVMessage( Ball *ball );
  bool LogRecvBVMessage( ExternalBVData *bv );
  bool LogSendPVMessage( Player *player );
  bool LogRecvPVMessage( ExternalPVData *pv );
  bool LogSendPSMessage( Player *player );
  bool LogRecvPSMessage( ExternalPSData *ps );
  bool LogRecvPTMessage( ExternalPTData *pt );

private:
  Logging();

  static Logging *m_logging;

  FILE *m_fp[8];
};

#endif
