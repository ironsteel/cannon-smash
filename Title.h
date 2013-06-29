/* $Id: Title.h,v 1.11 2003/07/16 15:58:17 nan Exp $ */

// Copyright (C) 2000, 2001  神南 吉宏(Kanna Yoshihiro)
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

#ifndef _Title_
#define _Title_
#include "Control.h"
#include "TitleView.h"

// major menu
#define MENU_MAIN	0
#define MENU_CONFIG	1

// minor menu
#define MENU_ALL		0	// number of all menus
#define MENU_CONFIG_LEVEL	1	// level menu(in config menu)
#define MENU_CONFIG_MODE	2	// mode(in config menu)
#define MENU_CONFIG_PLAYER	3	// player(in config menu)

class MenuItem;

class Title : public Control {
public:
  Title();
  virtual ~Title();

  virtual bool Init();

  static void Create();

  virtual bool Move( SDL_keysym *KeyHistory, long *MouseXHistory,
		     long *MouseYHistory, unsigned long *MouseBHistory,
		     int Histptr );

  MenuItem *GetSelected();
  long GetSelectMode();
  long GetCount();
  long GetMenuNum( long major, long minor=0 );

  virtual bool LookAt( double &srcX, double &srcY, double &srcZ,
		       double &destX, double &destY, double &destZ );

  virtual bool IsPlaying() { return false; };

  virtual View *GetView() { return m_View; };
protected:
  TitleView *m_View;
  long m_selected;	// Selected : m_selected > 0
  long m_selectMode;	// Normal   : 0
			// config   : 1
  long m_count;

  MenuItem *m_menuItem[16];

  void CreateMenu( long menuMajorNum );
  long SetSelected( long selected );
  long HitTest( long x, long y );
};

#endif	// _Title_
