/* $Id: HitMark.h,v 1.3 2001/03/30 03:16:51 nan Exp $ */

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

#ifndef _HitMark_
#define _HitMark_
#include "View.h"

class HitMark : public View {
public:
  HitMark();
  static bool Init();
  bool Hit( double x, double y, double z, double vx, double vy, double vz,
	     long swingError );
  virtual ~HitMark();

  virtual bool Redraw();
  virtual bool RedrawAlpha();

  static GLuint       m_textures[2];
private:
  double m_x;
  double m_y;
  double m_z;
  double m_vx;
  double m_vy;
  double m_vz;
  long m_swingError;

  struct timeb startTime;
  long m_time;
};

#endif	// _HitMark
