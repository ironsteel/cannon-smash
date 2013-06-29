/* $Id: TrainingSelect.cpp,v 1.14 2003/07/16 15:55:24 nan Exp $ */

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
#include "TrainingSelect.h"
#include "BaseView.h"
#include "Player.h"
#include "Sound.h"
#include "TrainingSelectView.h"
#include "Event.h"

extern long mode;

extern long wins;

TrainingSelect::TrainingSelect() : PlayerSelect() {
}

bool
TrainingSelect::Init() {
  m_View = (TrainingSelectView *)View::CreateView( VIEW_TRAININGSELECT );

  m_View->Init( this );

  BaseView::TheView()->AddView( m_View );

  return true;
}

void
TrainingSelect::Create() {
  Control::ClearControl();

  m_theControl = new TrainingSelect();
  m_theControl->Init();

  SDL_ShowCursor(0);
}

bool
TrainingSelect::Move( SDL_keysym *KeyHistory, long *MouseXHistory,
		    long *MouseYHistory, unsigned long *MouseBHistory,
		    int Histptr ) {
  static long lastRotate = 0;
  static long nothing = 0;

  // Next opponent
  if ( wins > 0 && m_selected == 0 ) {
    m_selected = 1;
    m_rotate = (m_thePlayer->GetPlayerType()-1)*(360/TRAININGPLAYERS);
    return true;
  }

  if ( KeyHistory[Histptr].unicode == SDLK_ESCAPE ||
       KeyHistory[Histptr].unicode == 'Q' ) {
    mode = MODE_TITLE;
    return true;
  }

  if ( m_selected > 500 ) {
    mode = MODE_TRAINING;
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
      mode = MODE_TRAINING;
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

    m_rotate += lastRotate;
    if ( m_rotate < 0 )
      m_rotate += 360;
    else
      m_rotate %= 360;
  } else {
    long nextRotate = m_rotate + lastRotate;

    if ( nextRotate < 0 )
      nextRotate += 360;
    else
      nextRotate %= 360;

    if ( m_rotate/(360/TRAININGPLAYERS) != nextRotate/(360/TRAININGPLAYERS) ) {
      m_rotate = (nextRotate+360/TRAININGPLAYERS/2)/(360/TRAININGPLAYERS)*
	(360/TRAININGPLAYERS);
      lastRotate = 0;
      Sound::TheSound()->Play( SOUND_CLICK, 0, 0 );
    } else
      m_rotate = nextRotate;
  }

  if ( nothing > 1000 ) {
    nothing = 0;
    mode = MODE_TITLE;
  }

  return true;
}

long
TrainingSelect::GetPlayerNum() {
  if ( GetRotate() < 0 )
    return (360+(GetRotate()%360))/(360/TRAININGPLAYERS);
  else
    return (GetRotate()%360)/(360/TRAININGPLAYERS);
}

