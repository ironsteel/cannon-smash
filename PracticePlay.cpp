/* $Id: PracticePlay.cpp,v 1.4 2002/11/04 14:31:44 nan Exp $ */

// Copyright (C) 2001, 2002  神南 吉宏(Kanna Yoshihiro)
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
#include "Event.h"
#include "PracticePlay.h"
#include "Player.h"
#include "Ball.h"

extern Ball theBall;
extern long mode;

PracticePlay::PracticePlay() : SoloPlay() {
}

PracticePlay::~PracticePlay() {
}

void
PracticePlay::Create( long player, long com ) {
  Control::ClearControl();

  m_theControl = new PracticePlay();
  m_theControl->Init();

  m_thePlayer = Player::Create( player, 1, 0 );
  m_comPlayer = Player::Create( com, -1, 1 );

  m_thePlayer->Init();
  m_comPlayer->Init();

  // Move it to view?
  SDL_ShowCursor(0);
  SDL_WM_GrabInput( SDL_GRAB_ON );
}

void
PracticePlay::EndGame() {
  mode = MODE_TITLE;
}
