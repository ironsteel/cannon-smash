/* $Id: Sound.cpp,v 1.29 2003/08/03 10:11:56 nan Exp $ */

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

#include "ttinc.h"
#include "Sound.h"
#include "Control.h"

#ifdef WIN32
#include <io.h>
#endif

extern long mode;
extern SDL_mutex *loadMutex;

Sound* Sound::m_theSound = NULL;

Sound::Sound() {
#ifdef HAVE_LIBSDL_MIXER
  m_queuehead = m_queuetail = 0;
#else
  m_soundMode = SOUND_NONE;
#endif
}

Sound::~Sound() {
#ifdef HAVE_LIBSDL_MIXER
  for ( int i = 0 ; i < 16 ; i++ ) {
    if ( m_sound[i] != 0 ) {
      Mix_FreeChunk( m_sound[i] );
      m_sound[i] = 0;
    }
  }

  if ( m_opening != 0 ) {
    Mix_FreeMusic( m_opening );
    m_opening = 0;
  }

  Mix_CloseAudio();
#endif
}

Sound*
Sound::TheSound() {
  if ( Sound::m_theSound )
    return Sound::m_theSound;
  else
    return (Sound::m_theSound = new Sound());
}

bool
Sound::Init( long sndMode ) {
  m_soundMode = sndMode;

#ifdef HAVE_LIBSDL_MIXER
  for ( int i = 0 ; i < 16 ; i++ ) {
    m_sound[i] = 0;
  }
  m_opening = 0;
#endif

  if ( m_soundMode == SOUND_NONE )
    return true;

#ifdef HAVE_LIBSDL_MIXER
#ifdef WIN32
  if ( Mix_OpenAudio( 44100, AUDIO_S16SYS, 2, 4096 ) < 0 ) {
#else
  if ( Mix_OpenAudio( 44100, AUDIO_S16SYS, 2, 128 ) < 0 ) {
#endif
    perror( _("SDL Mix_OpenAudio failed\n") );
  }

  m_sound[SOUND_RACKET] = Mix_LoadWAV( "wav/racket.wav" );
  m_sound[SOUND_TABLE] = Mix_LoadWAV( "wav/table.wav" );
  m_sound[SOUND_CLICK] = Mix_LoadWAV( "wav/click.wav" );
  m_sound[SOUND_LOVEALL] = Mix_LoadWAV( "wav/loveall.wav" );
  m_sound[SOUND_ALL] = Mix_LoadWAV( "wav/all.wav" );

  m_score[0] = Mix_LoadWAV( "wav/zero.wav" );
  m_score[1] = Mix_LoadWAV( "wav/one.wav" );
  m_score[2] = Mix_LoadWAV( "wav/two.wav" );
  m_score[3] = Mix_LoadWAV( "wav/three.wav" );
  m_score[4] = Mix_LoadWAV( "wav/four.wav" );
  m_score[5] = Mix_LoadWAV( "wav/five.wav" );
  m_score[6] = Mix_LoadWAV( "wav/six.wav" );
  m_score[7] = Mix_LoadWAV( "wav/seven.wav" );
  m_score[8] = Mix_LoadWAV( "wav/eight.wav" );
  m_score[9] = Mix_LoadWAV( "wav/nine.wav" );
  m_score[10] = Mix_LoadWAV( "wav/ten.wav" );
  m_score[11] = Mix_LoadWAV( "wav/eleven.wav" );
  m_score[12] = Mix_LoadWAV( "wav/twelve.wav" );
  m_score[13] = Mix_LoadWAV( "wav/thirteen.wav" );
  m_score[14] = Mix_LoadWAV( "wav/fourteen.wav" );
  m_score[15] = Mix_LoadWAV( "wav/fifteen.wav" );
  m_score[16] = Mix_LoadWAV( "wav/sixteen.wav" );
  m_score[17] = Mix_LoadWAV( "wav/seventeen.wav" );
  m_score[18] = Mix_LoadWAV( "wav/eighteen.wav" );
  m_score[19] = Mix_LoadWAV( "wav/nineteen.wav" );
  m_score[20] = Mix_LoadWAV( "wav/twenty.wav" );
  m_score[30] = Mix_LoadWAV( "wav/thirty.wav" );

  Mix_ChannelFinished( Sound::PlayFinishHandler );
#endif

  return true;
}

void
Sound::Clear() {
#ifdef HAVE_LIBSDL_MIXER
  for ( int i = 0 ; i < 16 ; i++ ) {
    if ( m_sound[i] != 0 ) {
      Mix_FreeChunk( m_sound[i] );
      m_sound[i] = 0;
    }
  }

  if ( m_opening != 0 ) {
    Mix_FreeMusic( m_opening );
    m_opening = 0;
  }

  Mix_ChannelFinished( NULL );
  Mix_CloseAudio();
#endif
}

bool
Sound::Play( long soundID, double x, double y ) {
  if ( m_soundMode == SOUND_NONE )
    return true;

#ifdef HAVE_LIBSDL_MIXER
  double srcX, srcY, srcZ;
  double destX, destY, destZ;
  double angle, destAngle;

  Control::TheControl()->LookAt( srcX, srcY, srcZ, destX, destY, destZ );

  destX -= srcX; destY -= srcY;
  x -= srcX; y -= srcY;

  destAngle = acos( destX/hypot(destX, destY) );
  if ( destY < 0 )
    destAngle += 3.14159265;

  angle = acos( x/hypot(x, y) );
  if ( y < 0 )
    angle += 3.14159265;

  angle -= destAngle;

  Mix_SetPosition( 0, (int)(angle*180.0/3.14159265), hypot(x, y)*8 );
  Mix_PlayChannel( 0, m_sound[soundID], 0 );
#endif

  return true;
}

bool
Sound::PlayScore( long score1, long score2 ) {
  if ( m_soundMode == SOUND_NONE )
    return true;

#ifdef HAVE_LIBSDL_MIXER
  double x = -TABLEWIDTH/2-0.5;
  double y = 0;
  double srcX, srcY, srcZ;
  double destX, destY, destZ;
  double angle, destAngle;

  Control::TheControl()->LookAt( srcX, srcY, srcZ, destX, destY, destZ );

  destX -= srcX; destY -= srcY;
  x -= srcX; y -= srcY;

  destAngle = acos( destX/hypot(destX, destY) );
  if ( destY < 0 )
    destAngle += 3.14159265;

  angle = acos( x/hypot(x, y) );
  if ( y < 0 )
    angle += 3.14159265;

  angle -= destAngle;

  Mix_SetPosition( 0, (int)(angle*180.0/3.14159265), hypot(x, y)*8 );
  PlayNumber( score1 );
  if ( score1 == score2 )
    PlayBlocking( 1, m_sound[SOUND_ALL] );
  else
    PlayNumber( score2 );
#endif

  return true;
}

bool
Sound::PlayNumber( long number ) {
  if ( m_soundMode == SOUND_NONE )
    return true;

#ifdef HAVE_LIBSDL_MIXER
  number %= 40;
  if ( number > 30 ) {
    PlayBlocking( 1, m_score[30] );
    PlayBlocking( 1, m_score[number-30] );
  } else if ( number > 20 ) {
    PlayBlocking( 1, m_score[20] );
    PlayBlocking( 1, m_score[number-20] );
  } else
    PlayBlocking( 1, m_score[number] );
#endif

  return true;
}

/* Play the sound on blocking mode */
bool
Sound::PlayBlocking( int channel, Mix_Chunk *chunk ) {
  if ( !Mix_Playing( channel ) ) {
    Mix_PlayChannel( channel, chunk, 0 );
  } else {
    m_chunkqueue[m_queuetail%SOUND_QUEUESIZE] = chunk;
    m_queuetail++;
  }

  return true;
}

void
Sound::PlayFinishHandler( int channel ) {
  if ( channel != 1 )
    return;

  if ( Sound::TheSound()->m_queuehead != Sound::TheSound()->m_queuetail ) {
    Mix_PlayChannel( channel,
		     Sound::TheSound()->
		     m_chunkqueue[Sound::TheSound()->
				 m_queuehead%SOUND_QUEUESIZE],
		     0 );
    Sound::TheSound()->m_queuehead++;
  }
}

long
Sound::GetSoundMode() {
  return m_soundMode;
}

long
Sound::InitBGM( char *filename ) {
  if ( m_soundMode == SOUND_NONE )
    return true;

#ifdef HAVE_LIBSDL_MIXER
  m_opening = Mix_LoadMUS( OPENINGFILENAME );

  if ( m_opening == 0 )
    printf( "%s\n", SDL_GetError() );
#endif

  return 0;
}

// It is better to move it to other thread
long
Sound::PlayBGM() {
  if ( m_soundMode == SOUND_NONE )
    return true;

#ifdef HAVE_LIBSDL_MIXER
  Mix_PlayMusic( m_opening, 1 );
#endif

  return 0;
}

long
Sound::StopBGM() {
  if ( m_soundMode == SOUND_NONE )
    return true;

#ifdef HAVE_LIBSDL_MIXER
  Mix_FadeOutMusic( 2000 );
#endif

  return 0;
}
