/* $Id: PlayGame.h,v 1.11 2003/07/16 16:09:10 nan Exp $ */

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

#ifndef _PlayGame_
#define _PlayGame_

#include "Control.h"
#include "PlayGameView.h"
class Player;

// Base class of SoloPlay, MultiPlay, Training
class PlayGame : public Control {
public:
  PlayGame();
  virtual ~PlayGame();

  virtual bool Init() = 0;

  virtual bool Move( SDL_keysym *KeyHistory, long *MouseXHistory,
		     long *MouseYHistory, unsigned long *MouseBHistory,
		     int Histptr ) = 0;
  virtual bool LookAt( double &srcX, double &srcY, double &srcZ,
		       double &destX, double &destY, double &destZ ) = 0;

  virtual bool IsPlaying() { return true; };

  virtual bool IsPause() { return m_pause; };
  virtual void SetPause( bool pause );

  long GetService();
  long GetScore( Player *p );
  long GetScore( long side );
  long GetGame( Player *p );
  long GetGame( long side );

  bool IsGameEnd();
  virtual void EndGame();
  void ChangeScore();
  void ChangeScore( long score1, long score2 );

  virtual View *GetView() { return m_View; };
protected:
  PlayGameView *m_View;

  long m_Score1;	// Point of near side. Counter when training
  long m_Game1;		// Game of near side. 
  long m_Score2;	// Point of far side
  long m_Game2;		// Game of far side. 
  bool m_pause;		// True when pause. 
};

#endif	// _PlayGame_
