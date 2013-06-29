/* $Id: Control.cpp,v 1.4 2002/11/04 14:13:18 nan Exp $ */

// Copyright (C) 2000, 2002  神南 吉宏(Kanna Yoshihiro)
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
#include "Control.h"
#include "Player.h"
#include "Network.h"
#include "PlayGame.h"

extern long wins;

extern long score1;
extern long score2;

Control *Control::m_theControl = NULL;
Player* Control::m_thePlayer = NULL;
Player* Control::m_comPlayer = NULL;

Control::Control() {
}

Control::~Control() {
  m_theControl = NULL;
}

void
Control::ClearControl() {
  if ( m_thePlayer && wins == 0 ) {
    delete m_thePlayer;
    m_thePlayer = NULL;
    ClearSocket();
  }
  if ( m_comPlayer ) {
    delete m_comPlayer;
    m_comPlayer = NULL;
  }

  if ( m_theControl ) {
    if ( m_theControl->IsPlaying() ) {
      score1 = ((PlayGame *)m_theControl)->GetScore(1);
      score2 = ((PlayGame *)m_theControl)->GetScore(-1);
    }
    delete m_theControl;
    m_theControl = NULL;
  }
}
