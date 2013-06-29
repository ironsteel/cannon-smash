/* $Id: MenuItemView2D.h,v 1.2 2001/12/25 15:18:05 yotsuya Exp $ */

// Copyright (C) 2001  神南 吉宏(Kanna Yoshihiro)
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

#ifndef _MenuItemView2D_
#define _MenuItemView2D_
#include "MenuItemView.h"

class MenuItemView2D : public MenuItemView {
public:
  MenuItemView2D();
  virtual ~MenuItemView2D();

  bool Init( MenuItem *menu, char *fileName );

  virtual bool Redraw();
  virtual bool RedrawAlpha();

protected:
  MenuItem    *m_menuItem;
  SDL_Surface *m_imageBMP;
  SDL_Surface *m_selectedImageBMP;
};

#endif	// _MenuItemView2D_

