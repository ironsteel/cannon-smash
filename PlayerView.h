/* $Id: PlayerView.h,v 1.10 2003/02/06 14:51:53 nan Exp $ */

// Copyright (C) 2000, 2002, 2003  神南 吉宏(Kanna Yoshihiro)
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

#ifndef _PlayerView_
#define _PlayerView_
#include "View.h"

#if !defined(CHIYO)
class partsmotion;
typedef partsmotion partsmotion_t;
#else
class body_parts;
typedef body_parts partsmotion_t;
#endif

class Player;

class PlayerView : public View {
public:
  PlayerView();
  virtual ~PlayerView();

  static void * LoadData(void *dum);

  virtual bool Init( Player *player );

  virtual bool Redraw();
  virtual bool RedrawAlpha();

  void DrawTargetCircle( double diff );
private:
  Player  *m_player;	// Model

  partsmotion_t *m_Fnormal;
  partsmotion_t *m_Bnormal;
  partsmotion_t *m_Fdrive;
  partsmotion_t *m_Bdrive;
  partsmotion_t *m_Fcut;
  partsmotion_t *m_Bcut;
  partsmotion_t *m_Fpeck;
  partsmotion_t *m_Bpeck;
  partsmotion_t *m_Fsmash;
  partsmotion_t *m_Bsmash;

  static partsmotion_t *motion_Fnormal;
  static partsmotion_t *motion_Bnormal;
  static partsmotion_t *motion_Fdrive;
  static partsmotion_t *motion_Fcut;
  static partsmotion_t *motion_Bcut;
  static partsmotion_t *motion_Fpeck;
  static partsmotion_t *motion_Bpeck;
  static partsmotion_t *motion_Fsmash;

  virtual bool SubRedraw();
  double GetHitpointY();

  void DrawPlayer();
  void DrawTarget();
  void DrawMeter();

  float m_xdiff;
  float m_ydiff;
  float m_zdiff;
};

#endif	// _PlayerView
