/* $Id: Launcher.cpp,v 1.25 2002/12/16 14:08:10 nan Exp $ */

// Copyright (C) 2001, 2002  ¿ÀÆî µÈ¹¨(Kanna Yoshihiro)
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
#include "Launcher.h"
#include "LauncherView.h"
#include "LobbyClient.h"


Launcher::Launcher() {
}

Launcher::~Launcher() {
}

void
Launcher::Init() {
#if 1
  LauncherView *view = new LauncherView();
  view->Init();
#else
  LobbyClient *lb;
  lb = LobbyClient::Create();
  lb->Init( "Robot", "I am test robot." );
#endif
}
