/* $Id: Howto.cpp,v 1.17 2003/07/16 16:03:38 nan Exp $ */

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

#include "ttinc.h"
#include "Howto.h"
#include "HowtoView.h"
#include "BaseView.h"
#include "Player.h"
#include "Ball.h"
#include "PenAttack.h"
#include "ShakeCut.h"
#include "Event.h"

extern long mode;

extern Ball theBall;

Howto::Howto() {
  m_View = NULL;
  m_mode = 0;
  m_count = 0;

  m_mouseX = 25;
  m_mouseY = 25;
  m_mouseB = 0;
}

Howto::~Howto() {
  if ( m_View ){
    BaseView::TheView()->RemoveView( m_View );
    delete m_View;
  }
}

bool
Howto::Init() {
  m_View = (HowtoView *)View::CreateView( VIEW_HOWTO );
  m_View->Init( this );

  BaseView::TheView()->AddView( m_View );

  m_thePlayer = new PenAttack(1);
  m_comPlayer = new ShakeCut(-1);

  m_thePlayer->Init();
  m_comPlayer->Init();

  return true;
}

void
Howto::Create() {
  Control::ClearControl();

  m_theControl = new Howto();
  m_theControl->Init();
}

bool
Howto::Move( SDL_keysym *KeyHistory, long *MouseXHistory,
	     long *MouseYHistory, unsigned long *MouseBHistory,
	     int Histptr ) {
  if ( IsMove() ) {
    theBall.Move();
    m_thePlayer->Move( NULL, NULL, NULL, NULL, 0 );
    m_comPlayer->Move( NULL, NULL, NULL, NULL, 0 );
  }

  if ( KeyHistory[Histptr].unicode == SDLK_ESCAPE ) {
    mode = MODE_TITLE;
    return true;
  }

  switch ( m_mode ) {
  case 0:	// How to move
    if ( m_count == 0 ) {
      m_thePlayer->m_x = -TABLEWIDTH/4;
    }

    switch ( (m_count%400)/100 ) {
    case 0:
      m_thePlayer->m_vx = (double)(50-abs((m_count%100)-50))/25.0;
      m_thePlayer->m_vy = 0.0;
      m_mouseX = (m_count%100)/2;
      m_mouseY = 50;
      break;
    case 1:
      m_thePlayer->m_vx = 0.0;
      m_thePlayer->m_vy = -(double)(50-abs((m_count%100)-50))/25.0;
      m_mouseX = 50;
      m_mouseY = 50-(m_count%100)/2;
      break;
    case 2:
      m_thePlayer->m_vx = -(double)(50-abs((m_count%100)-50))/25.0;
      m_thePlayer->m_vy = 0.0;
      m_mouseX = 50-(m_count%100)/2;
      m_mouseY = 0;
      break;
    case 3:
      m_thePlayer->m_vx = 0.0;
      m_thePlayer->m_vy = (double)(50-abs((m_count%100)-50))/25.0;
      m_mouseX = 0;
      m_mouseY = (m_count%100)/2;
      break;
    }
    m_mouseB = 0;
    break;
  case 1:	// Serve
    m_thePlayer->m_x = -TABLEWIDTH/2;
    m_thePlayer->m_y = -TABLELENGTH/2-0.5;
    m_thePlayer->m_vx = 0.0;
    m_thePlayer->m_vy = 0.0;
    m_mouseX = 25;
    m_mouseY = 25;
    if ( m_count%350 == 50 ) {
      theBall.Toss( m_thePlayer, 1 );
      m_thePlayer->StartSwing( m_count/350+1 );
    }
    if ( (m_count%350) >= 50 && (m_count%350) < 100 )
      m_mouseB = m_count/350+1;
    else
      m_mouseB = 0;
    break;
  case 2:	// How to move target
    if ( m_count == 200 ) {
      m_thePlayer->m_targetX = -TABLEWIDTH/2*0.9;
      m_thePlayer->m_targetY = TABLELENGTH/12*5;
      m_mouseX = 0;
      m_mouseY = 0;
    } else if ( m_count == 300 ) {
      m_thePlayer->m_targetX = -TABLEWIDTH/2*0.9;
      m_thePlayer->m_targetY = TABLELENGTH/12*2;
      m_mouseX = 0;
      m_mouseY = 3;
    } else if ( m_count == 400 ) {
      m_thePlayer->m_targetX = TABLEWIDTH/2*0.9;
      m_thePlayer->m_targetY = TABLELENGTH/12*5;
      m_mouseX = 12;
      m_mouseY = 0;
    } else if ( m_count == 500 ) {
      m_thePlayer->m_targetX = TABLEWIDTH/2*0.9;
      m_thePlayer->m_targetY = TABLELENGTH/12*2;
      m_mouseX = 12;
      m_mouseY = 3;
    } else if ( m_count == 600 ) {
      m_thePlayer->m_targetX = -TABLEWIDTH/2*0.6;
      m_thePlayer->m_targetY = TABLELENGTH/12*3;
      m_mouseX = 2;
      m_mouseY = 2;
    } else if ( m_count == 700 ) {
      m_thePlayer->m_targetX = TABLEWIDTH/2*0.15;
      m_thePlayer->m_targetY = TABLELENGTH/12*4;
      m_mouseX = 7;
      m_mouseY = 1;
    } else if ( m_count == 800 ) {
      m_thePlayer->m_targetX = TABLEWIDTH/2*0.6;
      m_thePlayer->m_targetY = TABLELENGTH/12*4;
      m_mouseX = 10;
      m_mouseY = 1;
    } else if ( m_count == 900 ) {
      m_thePlayer->m_targetX = TABLEWIDTH/2*0.75;
      m_thePlayer->m_targetY = TABLELENGTH/12*3;
      m_mouseX = 11;
      m_mouseY = 2;
    } else if ( m_count == 1150 ) {
      theBall.Toss( m_thePlayer, 1 );
      m_thePlayer->StartSwing( 1 );
    } else if ( m_count == 1450 ) {
      m_thePlayer->m_targetX = -TABLEWIDTH/2*0.9;
      m_thePlayer->m_targetY = TABLELENGTH/12*5;
      m_mouseX = 0;
      m_mouseY = 0;
    } else if ( m_count == 1550 ) {
      theBall.Toss( m_thePlayer, 1 );
      m_thePlayer->StartSwing( 1 );
    }

    break;
  case 3:	// Hit ball (1)
    if ( m_count == 0 ) {
      m_thePlayer->m_targetX = TABLEWIDTH/2*0.3;
      m_thePlayer->m_targetY = TABLELENGTH/16*5;
      m_thePlayer->m_x = -TABLEWIDTH/2+0.1;

      m_comPlayer->m_x = 0.221131;
      m_comPlayer->m_y = 1.855788;
      m_mouseX = 0;
      m_mouseY = 0;
      m_mouseB = 0;
    } else if ( m_count == 50 ) {
      theBall.Toss( m_thePlayer, 1 );
      m_thePlayer->StartSwing( 3 );
      m_mouseB = 1;
    } else if ( m_count > 50 && m_count < 100 ) {
      m_mouseB = 1;
    } else if ( m_count == 180 ) {
      m_comPlayer->m_targetX = -TABLEWIDTH/2*0.15;
      m_comPlayer->Swing( 1 );
      m_comPlayer->m_pow = 10;
      m_mouseB = 0;
    } else {
      m_mouseB = 0;
    }
    break;
  case 4:	// Hit ball (2)
  case 5:
    if ( m_count > 0 && m_count < 500 ) {
      m_thePlayer->m_x = -TABLEWIDTH/2+0.1 +
	(500-m_count)/1000.0*sin( m_count*2*3.141592/500 );
    } else if ( m_count == 570 ) {
      m_thePlayer->Swing( 3 );
      m_thePlayer->m_pow = 10;
      m_mouseB = 1;
    } else if ( m_count > 570 && m_count < 700 ) {
      m_mouseB = 1;
    } else if ( m_count == 1000 ) {
      m_thePlayer->m_x = 0.2;
      theBall.Warp( 0.243705, 0.673854, 1.017764,
		    -1.042663, -4.457266, 0.108404, -1.0, 2 );
      m_thePlayer->m_x = -0.1;
      m_mouseB = 0;
    } else if ( m_count > 1000 && m_count < 1500 ) {
      m_thePlayer->m_x = -0.1 +
	(1500-m_count)/1000.0*sin( (m_count-1000)*2*3.141592/500 );
    } else if ( m_count == 1570 ) {
      m_thePlayer->Swing( 1 );
      m_thePlayer->m_pow = 10;
      m_mouseB = 3;
    } else if ( m_count > 1570 && m_count < 1700 ) {
      m_mouseB = 3;
    } else {
      m_mouseB = 0;
    }
  }

  m_count++;

  switch ( m_mode ) {
  case 0:
    if ( m_count >= 800 ) {
      m_mode = 1;
      m_count = 0;
    }
    break;
  case 1:
    if ( m_count >= 1050 ) {
      m_mode = 2;
      m_count = 0;
    }
    break;
  case 2:
    if ( m_count >= 1900 ) {
      m_mode = 3;
      m_count = 0;
    }
    break;
  case 3:
    if ( m_count > 440 ) {
      m_mode = 4;
      m_count = 0;
    }
    break;
  case 4:
    if ( m_count > 500 ) {
      m_mode = 5;
    }
  case 5:
    if ( m_count > 2200 ) {
      mode = MODE_TITLE;
      return true;
    }
  }

  return true;
}

bool
Howto::LookAt( double &srcX, double &srcY, double &srcZ,
	       double &destX, double &destY, double &destZ ) {
  srcX = m_thePlayer->GetX() + m_thePlayer->GetEyeX();
  srcY = m_thePlayer->GetY() + m_thePlayer->GetEyeY();
  srcZ = m_thePlayer->GetZ() + m_thePlayer->GetEyeZ();

  switch ( m_mode ) {
  case 2:
    if ( m_count < 100 ) {
      srcX *= (double)(100-m_count)/100;
      srcY += (TABLELENGTH/3 - srcY)*(double)(m_count)/100;
      srcZ += (double)(m_count)*2.0/100;
    } else if ( m_count < 1000 ) {
      srcX = 0.0;
      srcY = TABLELENGTH/3;
      srcZ += 2.0;
    } else if ( m_count < 1100 ) {
      srcX *= (double)(m_count-1000)/100;
      srcY += (TABLELENGTH/3 - srcY)*(double)(100-(m_count-1000))/100;
      srcZ += (double)(100-(m_count-1000))*2.0/100;
    }
    break;
  }

  return true;
}

/* Decide weather ball and player should be moved or not. */
bool
Howto::IsMove() {
  switch ( m_mode ) {
  case 0:
  case 1:
  case 2:
    return true;
  case 3:
    if ( m_count > 215 )
      return false;
    else
      return true;
  case 4:
  case 5:
    if ( m_count < 570 ) {
      if ( m_count%10 == 0 )
	return true;
      else
	return false;
    } else if ( m_count < 670 ) {
      return false;
    } else if ( m_count < 1000 ) {
      return true;
    } else if ( m_count < 1570 ) {
      if ( m_count%10 == 0 )
	return true;
      else
	return false;
    } else if ( m_count < 1670 ) {
      return false;
    } else 
      return true;
  }

  return true;
}
