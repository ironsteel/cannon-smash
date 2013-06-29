/* $Id: LobbyClientView.cpp,v 1.13 2003/07/19 18:58:30 nan Exp $ */

// Copyright (C) 2001-2003  ¿ÀÆî µÈ¹¨(Kanna Yoshihiro)
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
#include "LobbyClient.h"
#include "LobbyClientView.h"
#include "RCFile.h"
#include <time.h>
#include <string>

extern RCFile *theRC;

extern bool isComm;
extern long mode;

extern void InitGame();
extern void StartGame();
extern void EndGame();
extern bool PollEvent();

extern void QuitGame();

bool isWaiting = false;		// waiting for opponent player on the internet

#ifdef WIN32
LONG pEditWndProc;
LONG pParentWndProc;

HWND pChildHWnd;
LobbyClientView *theLobbyClientView;

// Copyed from gdkim-win32.c of GTK+-2.2. 
gchar *
_gdk_ucs2_to_utf8 (const wchar_t *src,
		   gint           src_len)
{
  gint len;
  const wchar_t *wcp;
  guchar *mbstr, *bp;

  wcp = src;
  len = 0;
  while (wcp < src + src_len)
    {
      const wchar_t c = *wcp++;

      if (c < 0x80)
	len += 1;
      else if (c < 0x800)
	len += 2;
      else
	len += 3;
    }

  mbstr = (guchar *) g_malloc (len + 1);
  
  wcp = src;
  bp = mbstr;
  while (wcp < src + src_len)
    {
      int first;
      wchar_t c = *wcp++;

      if (c < 0x80)
	{
	  first = 0;
	  len = 1;
	}
      else if (c < 0x800)
	{
	  first = 0xc0;
	  len = 2;
	}
      else
	{
	  first = 0xe0;
	  len = 3;
	}
      
      /* Woo-hoo! */
      switch (len)
	{
	case 3: bp[2] = (c & 0x3f) | 0x80; c >>= 6; /* Fall through */
	case 2: bp[1] = (c & 0x3f) | 0x80; c >>= 6; /* Fall through */
	case 1: bp[0] = c | first;
	}

      bp += len;
    }
  *bp = 0;

  return (gchar *)mbstr;
}


LRESULT CALLBACK
LobbyClientView::EditWindowProc( HWND hwnd, UINT msg,
				 WPARAM wparam, LPARAM lparam) {
  if ( msg == WM_CHAR && wparam == 13 ) {
    char buf[512], unibuf[2048];
    GetWindowText( hwnd, buf, 512 );

    char locale[10];
    HKL input_locale = GetKeyboardLayout (0);
    GetLocaleInfo (MAKELCID (LOWORD (input_locale), SORT_DEFAULT),
		   LOCALE_IDEFAULTANSICODEPAGE,
		   locale, sizeof (locale));

    int srclen;
    srclen = MultiByteToWideChar(atoi(locale), 0, buf, 512,
				 (LPWSTR)unibuf, 2048 );

    gtk_entry_set_text( GTK_ENTRY(theLobbyClientView->m_chatinput), 
			_gdk_ucs2_to_utf8((const wchar_t *)unibuf, srclen) );

    GdkEventKey event;
    event.keyval = GDK_Return;

    LobbyClientView::KeyPress( theLobbyClientView->m_chatinput,
			       &event, theLobbyClientView );

    SetWindowText( hwnd, NULL );
    return 0;
  }

  return CallWindowProc((WNDPROC)pEditWndProc,hwnd,msg,wparam,lparam);
}

LRESULT CALLBACK
ParentWindowProc(HWND hwnd,UINT msg,WPARAM wparam,LPARAM lparam) {
  if ( msg == WM_SIZE ) {
    printf( "Resized\n" );
    MoveWindow( pChildHWnd, 0, 0, lparam&0xFFFF, (lparam>>16)&0xFFFF, TRUE );
  }

  return CallWindowProc((WNDPROC)pParentWndProc,hwnd,msg,wparam,lparam);
}
#endif

void
getcurrenttimestr( char *buf ) {
  struct tm *ltime;
  time_t t;

  time(&t);
  ltime = localtime( &t );

  strftime( buf, 32, "%H:%M:%S ", ltime );
}  

LobbyClientView::LobbyClientView() {
  m_timeout = 0;
  m_idle = 0;
  m_chatChannel = 0;
}

LobbyClientView::~LobbyClientView() {
  if ( m_timeout > 0 )
    gtk_timeout_remove( m_timeout );
  if ( m_idle > 0 )
    gtk_idle_remove( m_idle );
}

void
LobbyClientView::Init( LobbyClient *lobby ) {
  int i;

  m_parent = lobby;

  m_timeout = gtk_timeout_add( 1000, LobbyClient::PollServerMessage,
			       m_parent );

  // display
  m_window = gtk_dialog_new();
  gtk_container_border_width (GTK_CONTAINER (m_window), 10);

#ifdef WIN32
  char windowName[32];
  for ( i = 0 ; i < 31 ; i++ )
    windowName[i] = 'A'+RAND(26);
  windowName[31] = 0;

  gtk_window_set_title( GTK_WINDOW(m_window), windowName);
#else
  gtk_window_set_title( GTK_WINDOW(m_window), _("Cannon Smash"));
#endif
  gtk_widget_realize(m_window);
  gtk_window_set_modal( (GtkWindow *)m_window, true );
  gtk_widget_set_usize( m_window, 600, 400 );

  GtkWidget *scrolled_window;
  GtkWidget *button;

  scrolled_window = gtk_scrolled_window_new( NULL, NULL );
  gtk_container_border_width (GTK_CONTAINER (scrolled_window), 10);
  gtk_scrolled_window_set_policy( GTK_SCROLLED_WINDOW(scrolled_window),
				  GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS );
  gtk_box_pack_start( GTK_BOX(GTK_DIALOG(m_window)->vbox), scrolled_window,
		      TRUE, TRUE, 0 );

  GtkTreeViewColumn *column;
  GtkCellRenderer *renderer;
  GtkListStore *store;

  store = gtk_list_store_new( 2, G_TYPE_STRING, G_TYPE_STRING );
  m_table = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));

  renderer = gtk_cell_renderer_text_new();
  column = gtk_tree_view_column_new_with_attributes( _("Nickname"), renderer, 
						     "markup", 0, 
						     NULL );
  gtk_tree_view_column_set_resizable( column, TRUE );
  gtk_tree_view_append_column( GTK_TREE_VIEW(m_table), column );

  renderer = gtk_cell_renderer_text_new();
  column = gtk_tree_view_column_new_with_attributes( _("Message"), renderer,
						     "markup", 1, 
						     NULL );
  gtk_tree_view_column_set_resizable( column, TRUE );
  gtk_tree_view_append_column( GTK_TREE_VIEW(m_table), column );

  gtk_scrolled_window_add_with_viewport ( GTK_SCROLLED_WINDOW(scrolled_window),
					  m_table );
  GtkTreeSelection *select =
    gtk_tree_view_get_selection(GTK_TREE_VIEW(m_table));

  gtk_tree_selection_set_mode( select, GTK_SELECTION_SINGLE );

  gtk_tree_selection_set_select_function( select,
					  LobbyClientView::checkSelection,
					  this,
					  NULL);

  GtkWidget *notebook = gtk_notebook_new();
  GtkWidget *label;
  i = 0;

  m_langID[i] = table[m_parent->GetLang()].langID;
  if ( m_langID[i] != 0x09 ) {
    scrolled_window = gtk_scrolled_window_new (NULL, NULL);
    m_chat[i] = gtk_text_view_new();
    gtk_text_view_set_editable( GTK_TEXT_VIEW(m_chat[i]), FALSE );

    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),
				   GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
    gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrolled_window),
					  m_chat[i] );

    label = gtk_label_new( _(table[m_parent->GetLang()].langname) );
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), scrolled_window, label);
    i++;
  }

  m_langID[i] = 0x09;	// English
  scrolled_window = gtk_scrolled_window_new (NULL, NULL);
  m_chat[i] = gtk_text_view_new();
  gtk_text_view_set_editable( GTK_TEXT_VIEW(m_chat[i]), FALSE );

  gtk_scrolled_window_set_policy( GTK_SCROLLED_WINDOW (scrolled_window),
				  GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS );
  gtk_scrolled_window_add_with_viewport( GTK_SCROLLED_WINDOW(scrolled_window),
					 m_chat[i] );

  label = gtk_label_new( _("English") );

  gtk_notebook_append_page( GTK_NOTEBOOK(notebook), scrolled_window, label);

  gtk_box_pack_start( GTK_BOX(GTK_DIALOG(m_window)->vbox), notebook,
		      TRUE, TRUE, 0 );
  gtk_signal_connect (GTK_OBJECT (notebook), "switch-page",
		      GTK_SIGNAL_FUNC (LobbyClientView::SwitchChatPage), this);

  m_chatinput = gtk_entry_new();

  gtk_box_pack_start( GTK_BOX(GTK_DIALOG(m_window)->vbox), m_chatinput,
		      TRUE, TRUE, 10 );
  gtk_signal_connect (GTK_OBJECT (m_chatinput), "key-press-event",
		      GTK_SIGNAL_FUNC (LobbyClientView::KeyPress), this);

#ifdef WIN32
  gtk_widget_show_all(m_window);                     
  gtk_widget_show(m_chatinput);

  HWND hWnd = FindWindow( "gdkWindowTopLevel", windowName );
  gtk_window_set_title( GTK_WINDOW(m_window), _("Cannon Smash"));

  HWND cWnd = NULL;
  cWnd = FindWindowEx( hWnd, NULL, "gdkWindowChild", NULL );
  cWnd = FindWindowEx( cWnd, NULL, "gdkWindowChild", NULL );

  pParentWndProc = GetWindowLong(cWnd, GWL_WNDPROC);
  SetWindowLong(cWnd, GWL_WNDPROC, (LONG)ParentWindowProc);

  pChildHWnd = CreateWindow( "EDIT", NULL, WS_CHILD|WS_VISIBLE, 
			     CW_USEDEFAULT, CW_USEDEFAULT,
			     200, 30,
			     cWnd, NULL, NULL, NULL );

  pEditWndProc = GetWindowLong(pChildHWnd, GWL_WNDPROC);
  SetWindowLong(pChildHWnd, GWL_WNDPROC, (LONG)LobbyClientView::EditWindowProc);
  theLobbyClientView = this;
#endif

  m_connectButton = gtk_button_new_with_label (_("connect"));
  gtk_signal_connect (GTK_OBJECT (m_connectButton), "clicked",
		      GTK_SIGNAL_FUNC (LobbyClient::Connect), m_parent);
  gtk_widget_set_sensitive (m_connectButton, false);
  gtk_box_pack_start (GTK_BOX (GTK_DIALOG (m_window)->action_area),
		      m_connectButton, TRUE, TRUE, 0);

  m_warmUpButton = gtk_button_new_with_label (_("warm up"));
  gtk_signal_connect (GTK_OBJECT (m_warmUpButton), "clicked",
		      GTK_SIGNAL_FUNC (LobbyClientView::WarmUp), this);
  gtk_box_pack_start (GTK_BOX (GTK_DIALOG (m_window)->action_area),
		      m_warmUpButton, TRUE, TRUE, 0);

  button = gtk_button_new_with_label (_("close"));
  gtk_signal_connect (GTK_OBJECT (button), "clicked",
		      GTK_SIGNAL_FUNC (LobbyClientView::Quit), this);

  GTK_WIDGET_SET_FLAGS (button, GTK_CAN_DEFAULT);
  gtk_box_pack_start (GTK_BOX (GTK_DIALOG (m_window)->action_area),
		      button, TRUE, TRUE, 0);

  gtk_widget_grab_default (button);
  
  gtk_widget_show_all(m_window);                     
}

void
LobbyClientView::UpdateTable() {
  GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(m_table));

  gtk_list_store_clear( GTK_LIST_STORE(model) );
  gtk_widget_set_sensitive(m_connectButton, false);

  std::string nickname, message;
  bool selected = false;
  for ( int i = 0 ; i < m_parent->m_playerNum ; i++ ) {
    GtkTreeIter iter;
    gtk_list_store_append( GTK_LIST_STORE(model), &iter );
    if ( m_parent->m_player[i].m_playing ||
	 (m_parent->GetCanBeServer() == false &&
	  m_parent->m_player[i].m_canBeServer == false) ) {
      nickname = "<span color=\"gray\">";
      nickname += m_parent->m_player[i].m_nickname;
      nickname += "</span>";
      message = "<span color=\"gray\">";
      message += m_parent->m_player[i].m_message;
      message += "</span>";
    } else {
      nickname = "<span color=\"black\">";
      nickname += m_parent->m_player[i].m_nickname;
      nickname += "</span>";
      message = "<span color=\"black\">";
      message += m_parent->m_player[i].m_message;
      message += "</span>";
    }
    gtk_list_store_set( GTK_LIST_STORE(model), &iter,
			0, nickname.c_str(),
			1, message.c_str(),
			-1 );
    if ( m_parent->m_player[i].m_ID == m_parent->m_selected ) {
      GtkTreeSelection *selection =
	gtk_tree_view_get_selection(GTK_TREE_VIEW(m_table));
      gtk_tree_selection_select_iter( selection, &iter );
      selected = true;
    }
  }

  if ( selected == false )
    m_parent->m_selected = -1;    
}

gboolean
LobbyClientView::checkSelection( GtkTreeSelection *selection,
				 GtkTreeModel *model,
				 GtkTreePath *path,
				 gboolean path_currently_selected,
				 gpointer data ) {
  LobbyClientView *lobby = (LobbyClientView *)data;

  gchar* pathstr = gtk_tree_path_to_string(path);
  int selected = atoi( pathstr );
  g_free(pathstr);

  PlayerInfo selectedPlayer = lobby->m_parent->m_player[selected];
  if ( selectedPlayer.m_playing ||
       (lobby->m_parent->GetCanBeServer() == false &&
	selectedPlayer.m_canBeServer == false) ) {
    return FALSE;
  } else {
    lobby->m_parent->m_selected = selectedPlayer.m_ID;
    gtk_widget_set_sensitive(lobby->m_connectButton, true);
    return TRUE;
  }
}

void
LobbyClientView::WarmUp( GtkWidget *widget, gpointer data ) {
  LobbyClientView *lobby = (LobbyClientView *)data;

  isWaiting = true;
  ::InitGame();

  lobby->m_idle = gtk_idle_add( LobbyClientView::IdleFunc, data );
}

gboolean
LobbyClientView::KeyPress( GtkWidget *widget,
			   GdkEventKey *event,
			   gpointer data) {
  LobbyClientView *lobby = (LobbyClientView *)data;

  if ( event->keyval == GDK_Return &&
       strlen(gtk_entry_get_text(GTK_ENTRY(lobby->m_chatinput))) > 0 ) {
    lobby->m_parent->SendMS( (char *)gtk_entry_get_text(GTK_ENTRY(lobby->m_chatinput)),
			     lobby->m_langID[lobby->m_chatChannel] );

    char buf[32];
    getcurrenttimestr(buf);

    GtkTextBuffer *buffer;
    buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(lobby->m_chat[lobby->m_chatChannel]));

    GtkTextIter iter;
    gtk_text_buffer_get_end_iter( buffer, &iter );
    gtk_text_buffer_insert( buffer, &iter, buf, -1 );
    gtk_text_buffer_insert( buffer, &iter, ">", -1 );
    gtk_text_buffer_insert( buffer, &iter, lobby->m_parent->m_nickname, -1 );
    gtk_text_buffer_insert( buffer, &iter, "< ", -1 );
    gtk_text_buffer_insert( buffer, &iter, 
			    gtk_entry_get_text(GTK_ENTRY(lobby->m_chatinput)),
			    -1 );
    gtk_text_buffer_insert( buffer, &iter, "\n", -1 );

    GtkWidget* scrolled_window;
    scrolled_window = gtk_widget_get_parent(lobby->m_chat[lobby->m_chatChannel]);
    scrolled_window = gtk_widget_get_parent(scrolled_window);

    GtkAdjustment *adj =
      gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(scrolled_window));
    gtk_adjustment_set_value(adj, adj->upper-adj->page_size);

    gtk_entry_set_text(GTK_ENTRY(lobby->m_chatinput), "");
  }

  return FALSE;
}

void
LobbyClientView::SwitchChatPage( GtkNotebook *notebook,
				 GtkNotebookPage *page,
				 gint page_num,
				 gpointer data ) {
  LobbyClientView *lobby = (LobbyClientView *)data;

  lobby->m_chatChannel = page_num;
}


gint
LobbyClientView::IdleFunc( gpointer data ) {
  if ( !PollEvent() ) {
    ::EndGame();
    return 0;
  }

  return 1;
}

void
LobbyClientView::Quit( GtkWidget *widget, gpointer data ) {
  LobbyClientView *lobby = (LobbyClientView *)data;

  lobby->m_parent->SendQT();

  gtk_widget_destroy( lobby->m_window );
  gtk_timeout_remove( lobby->m_timeout );

  delete lobby->m_parent;
}

void
LobbyClientView::SetSensitive( bool sensitive ) {
  gtk_widget_set_sensitive (m_connectButton, sensitive);
  gtk_widget_set_sensitive (m_warmUpButton, sensitive);
  gtk_widget_set_sensitive (m_table, sensitive);
}

void
LobbyClientView::ShowUpdateDialog( char *version, char *URL ) {
  GtkWidget *dialog = gtk_dialog_new();

  char buf[256];
  sprintf( buf, _("Latest Version %s is already released. \nPlease download the latest version from \n%s\n"), version, URL );

  GtkWidget *label = gtk_label_new( buf );
  GtkWidget *button = gtk_button_new_with_label( "OK" );

  gtk_window_set_modal( (GtkWindow *)dialog, true );

  gtk_label_set_line_wrap( GTK_LABEL(label), true );

  gtk_box_pack_start( GTK_BOX(GTK_DIALOG(dialog)->vbox),
		      label, TRUE, TRUE, 0 );

  gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog)->action_area),
		      button, TRUE, TRUE, 0);
  gtk_signal_connect (GTK_OBJECT (button), "clicked",
		      GTK_SIGNAL_FUNC(LobbyClientView::Quit), this);
  gtk_signal_connect_object(GTK_OBJECT(button), "clicked",
			    GTK_SIGNAL_FUNC(gtk_widget_destroy), 
			    GTK_OBJECT(dialog));

  gtk_widget_show_all(dialog);
}

void
LobbyClientView::AddChatMessage( long channelID, char *message ) {
  char buf[32];
  getcurrenttimestr( buf );

  GtkTextBuffer *buffer;
  int i;

  for ( i = 0 ; i < 2 ; i++ ) {
    if ( m_langID[i] == channelID )
      break;
  }

  if ( i == 2 )
    return;

  buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(m_chat[i]));

  GtkTextIter iter;
  gtk_text_buffer_get_end_iter( buffer, &iter );
  gtk_text_buffer_insert( buffer, &iter, buf, -1 );
  gtk_text_buffer_insert( buffer, &iter, message, -1 );
  gtk_text_buffer_insert( buffer, &iter, "\n", -1 );

  GtkWidget* scrolled_window;
  scrolled_window = gtk_widget_get_parent(m_chat[i]);
  scrolled_window = gtk_widget_get_parent(scrolled_window);

  GtkAdjustment *adj =
    gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(scrolled_window));
  gtk_adjustment_set_value(adj, adj->upper-adj->page_size);
}

PIDialog::PIDialog() {
}

PIDialog::PIDialog( LobbyClient *parent ) {
  m_parent = parent;
}

PIDialog::~PIDialog() {
}

void
PIDialog::PopupDialog( long uniqID ) {
  GtkWidget *label, *button;
  char buf[256];

  if ( isWaiting ) {
    QuitGame();
  }

  m_uniqID = uniqID;

  m_window = gtk_dialog_new();
  gtk_window_set_title( GTK_WINDOW(m_window), _("Cannon Smash"));
  gtk_container_border_width (GTK_CONTAINER (m_window), 10);
  gtk_window_set_modal( (GtkWindow *)m_window, true );

  PlayerInfo *p = m_parent->GetPlayerInfo();

  int i;
  for ( i = 0 ; i < m_parent->GetPlayerNum() ; i++ ) {
    if ( p[i].m_ID == m_uniqID ) {
      sprintf( buf, _("\"%s\" (message: %s)want to play with you. OK?\n"),
	       p[i].m_nickname, p[i].m_message );
      break;
    }
  }

  label = gtk_label_new( buf );
  gtk_label_set_line_wrap( GTK_LABEL(label), true );

  gtk_box_pack_start( GTK_BOX(GTK_DIALOG(m_window)->vbox), label, 
		      TRUE, TRUE, 0 );

  button = gtk_button_new_with_label (_("OK!"));
  gtk_signal_connect (GTK_OBJECT (button), "clicked",
		      GTK_SIGNAL_FUNC (PIDialog::PIOK), this);

  GTK_WIDGET_SET_FLAGS (button, GTK_CAN_DEFAULT);
  gtk_box_pack_start (GTK_BOX (GTK_DIALOG (m_window)->action_area),
		      button, TRUE, TRUE, 0);

  gtk_widget_grab_default (button);

  button = gtk_button_new_with_label (_("No!"));
  gtk_signal_connect (GTK_OBJECT (button), "clicked",
		      GTK_SIGNAL_FUNC (PIDialog::PINo), this);

  gtk_box_pack_start (GTK_BOX (GTK_DIALOG (m_window)->action_area),
		      button, TRUE, TRUE, 0);

  gtk_widget_show_all(m_window);
}

void
PIDialog::PIOK( GtkWidget *widget, gpointer data ) {
  PIDialog *piDialog = (PIDialog *)data;
  gtk_widget_destroy( GTK_WIDGET(piDialog->m_window) );

  isComm = true;
  mode = MODE_MULTIPLAYSELECT;

  piDialog->m_parent->SendAP( piDialog->m_uniqID );
  piDialog->m_parent->SendSP();

  // If oppenent cannot be a server, I become server. 
  for ( int i = 0 ; i < piDialog->m_parent->m_playerNum ; i++ ) {
    if ( piDialog->m_parent->m_player[i].m_ID == piDialog->m_uniqID ) {
      if ( piDialog->m_parent->m_player[i].m_canBeServer == false ) {	// Opponent cannot be server
	theRC->serverName[0] = '\0';
      }
      break;
    }
  }

  ::StartGame();

  piDialog->m_parent->SendQP();
}

void
PIDialog::PINo( GtkWidget *widget, gpointer data ) {
  PIDialog *piDialog = (PIDialog *)data;
  gtk_widget_destroy( GTK_WIDGET(piDialog->m_window) );

  piDialog->m_parent->SendDP( piDialog->m_uniqID );
}

