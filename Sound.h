/* $Id: Sound.h,v 1.18 2003/08/03 10:12:59 nan Exp $ */

// Copyright (C) 2000-2003  ¿ÀÆî µÈ¹¨(Kanna Yoshihiro)
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

#ifndef _Sound_
#define _Sound_

#define SOUND_QUEUESIZE 16

struct buffer {
  unsigned char const *start;
  unsigned long length;
};

class Sound {
public:
  virtual ~Sound();

  static Sound* TheSound();

  virtual bool Init( long sndMode );
  virtual void Clear();

  //bool Play( char *sndData, long count );
  bool Play( long soundID, double x, double y );
  bool PlayScore( long score1, long score2 );
  bool PlayNumber( long number );

  long GetSoundMode();

  long InitBGM( char *filename );
  long PlayBGM();
  long StopBGM();

  // For BGM
  char *m_bgmSound;

#ifdef HAVE_LIBSDL_MIXER
  Mix_Chunk *m_sound[16];
  Mix_Chunk *m_score[31];
  Mix_Music *m_opening;

  static void PlayFinishHandler( int channel );
#endif

  long m_soundMode;
private:
  Sound();
  static Sound *m_theSound;

#ifdef HAVE_LIBSDL_MIXER
  Mix_Chunk *m_chunkqueue[SOUND_QUEUESIZE];
  int m_queuehead;
  int m_queuetail;

  bool PlayBlocking( int channel, Mix_Chunk *chunk );
#endif
};

#endif // _Sound_
