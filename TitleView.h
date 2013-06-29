/* $Id: TitleView.h,v 1.7 2002/02/02 21:18:11 nan Exp $ */

// Copyright (C) 2000, 2001, 2002  神南 吉宏(Kanna Yoshihiro)
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

#ifndef _TitleView_
#define _TitleView_
#include "View.h"
#include "LoadImage.h"

class Title;

class TitleView : public View {
public:
  TitleView();
  virtual ~TitleView();

  virtual bool Init( Title * );

  virtual bool Redraw();
  virtual bool RedrawAlpha();

  virtual bool AddView( View *view );
  virtual bool RemoveView( View *view );
protected:
  Title       *m_title;
  View        *m_View;

  ImageData    m_configTitle[4];
};

#endif	// _TitleView_
