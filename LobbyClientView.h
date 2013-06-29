/* $Id: LobbyClientView.h,v 1.10 2003/07/13 04:39:35 nan Exp $ */

// Copyright (C) 2001-2003  神南 吉宏(Kanna Yoshihiro)
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

#ifndef _LobbyClientView_
#define _LobbyClientView_

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

class LobbyClient;

class LobbyClientView {
public:
  LobbyClientView();
  ~LobbyClientView();

  void Init( LobbyClient *lobby );

  void SetSensitive( bool sensitive );
  void UpdateTable();
  void ShowUpdateDialog( char *version, char *URL );
  void AddChatMessage( long channelID, char *message );

protected:
  static gint IdleFunc( gpointer data );
  static gboolean checkSelection( GtkTreeSelection *selection,
				  GtkTreeModel *model,
				  GtkTreePath *path,
				  gboolean path_currently_selected,
				  gpointer data);

  static void Connect( GtkWidget *widget, gpointer data );
  static void WarmUp( GtkWidget *widget, gpointer data );
  static gboolean KeyPress( GtkWidget *widget, GdkEventKey *event, gpointer data );
  static void SwitchChatPage( GtkNotebook *notebook, GtkNotebookPage *page,
			      gint page_num, gpointer data );
  static void Quit( GtkWidget *widget, gpointer data );

#ifdef WIN32
  static LRESULT CALLBACK EditWindowProc( HWND hwnd, UINT msg,
					  WPARAM wparam, LPARAM lparam);
#endif

  guint m_timeout;
  guint m_idle;

  gint  m_chatChannel;
  int   m_langID[2];

  LobbyClient *m_parent;

  GtkWidget *m_window;
  GtkWidget *m_table;
  GtkWidget *m_chat[2];
  GtkWidget *m_chatinput;
  GtkWidget *m_connectButton;
  GtkWidget *m_warmUpButton;

  long m_selected;		// Selected row of the table
};

class PIDialog {
public:
  PIDialog();
  PIDialog( LobbyClient *parent );
  ~PIDialog();

  void PopupDialog( long uniqID );

  GtkWidget *m_window;
  LobbyClient *m_parent;

  long m_uniqID;

  static void PIOK( GtkWidget *widget, gpointer data );
  static void PINo( GtkWidget *widget, gpointer data );
};

#endif // _LobbyClientView_
