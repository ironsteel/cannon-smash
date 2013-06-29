/* $Id: OpeningView.cpp,v 1.5 2002/11/04 14:24:52 nan Exp $ */

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
#include "OpeningView.h"
#include "Player.h"
#include "Opening.h"


OpeningView::OpeningView() {
}

OpeningView::~OpeningView() {
}

bool
OpeningView::Init( Opening *opening ) {
  m_opening = opening;

  return true;
}

bool
OpeningView::Redraw() {
  GLfloat light_intensity_amb[] = { 0.6F, 0.6F, 0.6F, 1.0F };
  GLfloat light_intensity_dif[] = { 1.0F, 1.0F, 1.0F, 1.0F };

  long phrase, mod;

  m_opening->GetPhrase( phrase, mod );

  if ( phrase < 56 ) {
  } else if ( phrase < 64 ) {
    light_intensity_amb[0] *= (3200-m_opening->GetCount())/400.0F;
    light_intensity_amb[1] *= (3200-m_opening->GetCount())/400.0F;
    light_intensity_amb[2] *= (3200-m_opening->GetCount())/400.0F;
    light_intensity_dif[0] *= (3200-m_opening->GetCount())/400.0F;
    light_intensity_dif[1] *= (3200-m_opening->GetCount())/400.0F;
    light_intensity_dif[2] *= (3200-m_opening->GetCount())/400.0F;
  } else if ( phrase < 72 ) {
    if ( phrase >= 68 && mod > 20 ) {
      /*
      light_intensity_amb[0] *= (10-mod)/10.0F;
      light_intensity_amb[1] *= (10-mod)/10.0F;
      light_intensity_amb[2] *= (10-mod)/10.0F;
      light_intensity_dif[0] *= (10-mod)/10.0F;
      light_intensity_dif[1] *= (10-mod)/10.0F;
      light_intensity_dif[2] *= (10-mod)/10.0F;
      */
    } else {
      light_intensity_amb[0] *= 0.0F;
      light_intensity_amb[1] *= 0.0F;
      light_intensity_amb[2] *= 0.0F;
      light_intensity_dif[0] *= 0.0F;
      light_intensity_dif[1] *= 0.0F;
      light_intensity_dif[2] *= 0.0F;
    }
  }

  glLightfv(GL_LIGHT0, GL_AMBIENT, light_intensity_amb);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light_intensity_dif);
  glLightfv(GL_LIGHT0, GL_SPECULAR, light_intensity_dif);

  m_opening->GetThePlayer()->m_View->Redraw();
  m_opening->GetComPlayer()->m_View->Redraw();

  if ( phrase >= 200 && phrase < 232 )
    m_opening->m_telop[0]->GetView()->Redraw();

  return true;
}

bool
OpeningView::RedrawAlpha() {
  m_opening->GetThePlayer()->m_View->RedrawAlpha();
  m_opening->GetComPlayer()->m_View->RedrawAlpha();

  long phrase, mod;

  m_opening->GetPhrase( phrase, mod );

  if ( phrase >= 200 && phrase < 232 )
    m_opening->m_telop[0]->GetView()->RedrawAlpha();

  return true;
}
