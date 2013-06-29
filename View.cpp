/* $Id: View.cpp,v 1.4 2002/09/07 04:41:32 nan Exp $ */

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
#include "View.h"
#include "RCFile.h"
#include "PlayerView.h"
#include "PlayerView2D.h"
#include "BallView.h"
#include "BallView2D.h"
#include "FieldView.h"
#include "FieldView2D.h"
#include "HowtoView.h"
//#include "HowtoView2D.h"
#include "MenuItemView.h"
#include "MenuItemView2D.h"
#include "OpeningView.h"
//#include "OpeningView2D.h"
#include "PlayGameView.h"
//#include "PlayGameView2D.h"
#include "PlayerSelectView.h"
#include "PlayerSelectView2D.h"
#include "PracticeSelectView.h"
//#include "PracticeSelectView2D.h"
#include "TrainingSelectView.h"
//#include "TrainingSelectView2D.h"
#include "TitleView.h"
#include "TitleView2D.h"
#include "TrainingView.h"
//#include "TrainingView2D.h"

extern RCFile *theRC;

View::View() {
  m_next = (View *)0;
}

View::~View() {
}

bool
View::RedrawAlpha() {
  return true;
}

bool
View::GetDamageRect(){
  return true;
}

View*
View::CreateView( int viewType ) {
  if ( theRC->gmode == GMODE_2D ) {
    return CreateView2D( viewType );
  } else {
    return CreateView3D( viewType );
  }
}

View*
View::CreateView2D( int viewType ) {
  switch ( viewType ) {
  case VIEW_PLAYER:
    return new PlayerView2D();
  case VIEW_BALL:
    return new BallView2D();
  case VIEW_FIELD:
    return new FieldView2D();
  case VIEW_HOWTO:
    //return new HowtoView2D();
    return NULL;
  case VIEW_MENUITEM:
    return new MenuItemView2D();
  case VIEW_OPENING:
    //return new OpeningView2D();
    return NULL;
  case VIEW_PLAYGAME:
    //return new PlayGameView2D();
    return NULL;
  case VIEW_PLAYERSELECT:
    return new PlayerSelectView2D();
  case VIEW_PRACTICESELECT:
    //return new PracticeSelectView2D();
    return NULL;
  case VIEW_TRAININGSELECT:
    //return new TrainingSelectView2D();
    return NULL;
  case VIEW_TITLE:
    return new TitleView2D();
  case VIEW_TRAINING:
    //return new TrainingView2D();
    return NULL;
  default:
    return NULL;
  }
}

View*
View::CreateView3D( int viewType ) {
  switch ( viewType ) {
  case VIEW_PLAYER:
    return new PlayerView();
  case VIEW_BALL:
    return new BallView();
  case VIEW_FIELD:
    return new FieldView();
  case VIEW_HOWTO:
    return new HowtoView();
  case VIEW_MENUITEM:
    return new MenuItemView();
  case VIEW_OPENING:
    return new OpeningView();
  case VIEW_PLAYGAME:
    return new PlayGameView();
  case VIEW_PLAYERSELECT:
    return new PlayerSelectView();
  case VIEW_PRACTICESELECT:
    return new PracticeSelectView();
  case VIEW_TRAININGSELECT:
    return new TrainingSelectView();
  case VIEW_TITLE:
    return new TitleView();
  case VIEW_TRAINING:
    return new TrainingView();
  default:
    return NULL;
  }
}
