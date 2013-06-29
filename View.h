/* $Id: View.h,v 1.3 2002/09/07 05:06:29 nan Exp $ */

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

#ifndef _View_
#define _View_

#define VIEW_PLAYER         1
#define VIEW_BALL           2
#define VIEW_FIELD          3
#define VIEW_HOWTO          4
#define VIEW_MENUITEM       5
#define VIEW_OPENING        6
#define VIEW_PLAYGAME       7
#define VIEW_PLAYERSELECT   8
#define VIEW_PRACTICESELECT 9
#define VIEW_TRAININGSELECT 10
#define VIEW_TITLE          11
#define VIEW_TRAINING       12

class View {
public:
  View();
  virtual ~View();

  static View* CreateView( int viewType );

  virtual bool Redraw() = 0;
  virtual bool RedrawAlpha();
  virtual bool GetDamageRect();

  View *m_next;

private:
  static View* CreateView2D( int viewType );
  static View* CreateView3D( int viewType );
};

#endif	// _View_
