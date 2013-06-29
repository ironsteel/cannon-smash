/* $Id: comPlayer.h,v 1.7 2001/12/05 15:39:25 nan Exp $ */

// Copyright (C) 2000  $B?@Fn(B $B5H9((B(Kanna Yoshihiro)
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

#ifndef _comPlayer_
#define _comPlayer_

#define LEVELMARGIN	(0.30/(theRC->gameLevel*theRC->gameLevel*2+1))
//#define LEVELMARGIN	(0.15-gameLevel*0.05)

class ComPlayer {
public:
  ComPlayer();
  virtual ~ComPlayer();

protected:
  virtual bool Think() = 0;

// Valuable for Think()
  int _prevBallstatus;		// previous ball status
  double _hitX;			// hit point
  double _hitY;

// Calc hit point
  virtual bool Hitarea( double &hitX, double &hitY) = 0;
  virtual double GetBallTop( double &maxX, double &maxY, Player *p );
};

#endif // _comPlayer_
