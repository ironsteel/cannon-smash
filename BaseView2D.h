/* $Id: BaseView2D.h,v 1.3 2001/09/01 18:19:38 nan Exp $ */

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

#ifndef _BaseView2D_
#define _BaseView2D_

#include "BaseView.h"
#include "FieldView2D.h"

bool RenderRect( double x1, double y1, double z1, 
		 double x2, double y2, double z2, 
		 SDL_Rect *rect );
bool RenderPoint( double x, double y, double z, 
		  int *_x, int *_y );

class BaseView2D : public BaseView {
public:
  BaseView2D();
  ~BaseView2D();

  bool Init();

  static void DisplayFunc();

  virtual bool RedrawAll();
  virtual bool SetViewPosition();

  virtual void EndGame();
  virtual void QuitGame();

  virtual bool AddUpdateRect( SDL_Rect *r );
protected:
  virtual void SetLookAt();
  long m_updateX1;
  long m_updateY1;
  long m_updateX2;
  long m_updateY2;
};

#endif // _BaseView2D
