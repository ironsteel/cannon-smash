/* $Id: MultiPlay.h,v 1.20 2003/07/16 16:03:38 nan Exp $ */

// Copyright (C) 2000, 2001, 2003  神南 吉宏(Kanna Yoshihiro)
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

#ifndef _MultiPlay_
#define _MultiPlay_

#include "PlayGame.h"

class ExternalData {
public:
  ExternalData();
  ExternalData( long side );

  virtual ~ExternalData();

  long side;
  long dataType;
  long sec;
  char count;
  char data[256];
  ExternalData *next;

  virtual bool Apply( Player *targetPlayer, bool &fThePlayer, bool &fComPlayer,
		      bool &fTheBall ) = 0;
  virtual bool Read( long sock ) = 0;

  static ExternalData* ReadData( long s );

  virtual bool isNull() { return false; };

protected:
  void ReadTime( int sd, long *sec, char *count );
};

class ExternalPVData : public ExternalData {
public:
  ExternalPVData();
  ExternalPVData( long side );

  virtual bool Apply( Player *targetPlayer, bool &fThePlayer, bool &fComPlayer,
		      bool &fTheBall );
  virtual bool Read( long sock );
};

class ExternalPSData : public ExternalData {
public:
  ExternalPSData();
  ExternalPSData( long side );

  virtual bool Apply( Player *targetPlayer, bool &fThePlayer, bool &fComPlayer,
		      bool &fTheBall );
  virtual bool Read( long sock );
};

class ExternalBVData : public ExternalData {
public:
  ExternalBVData();
  ExternalBVData( long side );

  virtual bool Apply( Player *targetPlayer, bool &fThePlayer, bool &fComPlayer,
		      bool &fTheBall );
  virtual bool Read( long sock );
};

class ExternalPTData : public ExternalData {
public:
  ExternalPTData();
  ExternalPTData( long side );

  virtual bool Apply( Player *targetPlayer, bool &fThePlayer, bool &fComPlayer,
		      bool &fTheBall );
  virtual bool Read( long sock );
};

class ExternalNullData : public ExternalData {
public:
  virtual bool Apply( Player *targetPlayer, bool &fThePlayer, bool &fComPlayer,
		      bool &fTheBall ) { return false; };
  virtual bool Read( long sock ) { return false; };

  virtual bool isNull() { return true; };
};


class MultiPlay : public PlayGame {
public:
  MultiPlay();
  virtual ~MultiPlay();

  virtual bool Init();

  static void Create( long player, long com );

  virtual bool Move( SDL_keysym *KeyHistory, long *MouseXHistory,
		     long *MouseYHistory, unsigned long *MouseBHistory,
		     int Histptr );

  //long GetSmashPtr() { return m_smashPtr; };

  virtual bool LookAt( double &srcX, double &srcY, double &srcZ,
		       double &destX, double &destY, double &destZ );


  void SendTime( char *buf );

  virtual void EndGame();

  //long GetTimeAdj() { return m_timeAdj; };

  void StartServer();
  void StartClient();

  static int WaitForData( void * );
  static int Connect( void * );

protected:
  //bool m_smash;
  //long m_smashCount;
  //long m_smashPtr;
};

#endif	// _MultiPlay_
