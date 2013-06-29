/* $Id: ComTrainingPenDrive.h,v 1.4 2001/02/09 19:10:19 nan Exp $ */

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

#ifndef _ComTrainingPenDrive_
#define _ComTrainingPenDrive_
#include "comPenDrive.h"

class ComTrainingPenDrive : public ComPenDrive {
public:
  ComTrainingPenDrive();
  ComTrainingPenDrive(long side);
  ComTrainingPenDrive( long playerType, long side,
		       double x, double y, double z, 
		       double vx, double vy, double vz,
		       long status, long swing, long swingType,
		       bool swingSide, long afterSwing, long swingError, 
		       double targetX, double targetY,
		       double eyeX, double eyeY, double eyeZ,
		       long pow, double spin, double stamina, long statusMax );

  virtual ~ComTrainingPenDrive();

protected:
  virtual bool Think();

  virtual bool HitBall();
};

#endif // _ComTrainingPenDrive_

