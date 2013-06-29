/* $Id: TitleView2D.h,v 1.3 2001/09/11 17:31:37 nan Exp $ */

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

#ifndef _TitleView2D_
#define _TitleView2D_
#include "TitleView.h"

class Title;

class TitleView2D : public TitleView {
public:
  TitleView2D();
  virtual ~TitleView2D();

  virtual bool Init( Title * );

  virtual bool Redraw();
  virtual bool RedrawAlpha();
  virtual bool GetDamageRect();

  virtual bool AddView( View *view );
  virtual bool RemoveView( View *view );
protected:
  Title       *m_title;
  View        *m_View;
  SDL_Surface *m_triangleBMP;
};

#endif	// _TitleView_
