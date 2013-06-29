/* $Id: MenuItem.h,v 1.3 2001/08/08 11:35:10 nan Exp $ */

// Copyright (C) 2000  神南 吉宏(Kanna Yoshihiro)
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

#ifndef _MenuItem_
#define _MenuItem_

class Title;
class MenuItemView;

class MenuItem {
public:
  MenuItem();
  virtual ~MenuItem();

  virtual bool Init( long x, long y, long width, long height, char *fileName,
		     Title *parent );

  bool GetSelected() { return m_selected; }
  long GetX() { return m_x; }
  long GetY() { return m_y; }
  long GetWidth() { return m_width; }
  long GetHeight() { return m_height; }
  bool SetSelected( bool select );
  MenuItemView *GetView() { return m_View; }

protected:
  MenuItemView *m_View;

  Title *m_parent;

  bool m_selected;	// Selected?

  long m_x;
  long m_y;
  long m_width;
  long m_height;
};

#endif	// _MenuItem_
