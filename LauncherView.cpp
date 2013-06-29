/* $Id: LauncherView.cpp,v 1.12 2003/07/29 17:14:30 nan Exp $ */

// Copyright (C) 2001, 2002, 2003  ¿ÀÆî µÈ¹¨(Kanna Yoshihiro)
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
#include "LauncherView.h"
#include "Launcher.h"
#include "RCFile.h"
#include "Network.h"

#ifdef WIN32
extern gchar *_gdk_ucs2_to_utf8 (const wchar_t *src, gint src_len);

HWND LauncherView::hWnd = NULL;
LONG ModeNote::pEditWndProc = 0;
LONG ModeNote::pParentWndProc = 0;
HWND ModeNote::pChildHWnd = NULL;
ModeNote *theInternetPlayView;

// Copyed from gdkim-win32.c of GTK+-2.2. 
gint
_gdk_utf8_to_ucs2 (wchar_t     *dest,
                   const gchar *src,
                   gint         src_len,
                   gint         dest_max)
{
  wchar_t *wcp;
  guchar *cp, *end;
  gint n;
  
  wcp = dest;
  cp = (guchar *) src;
  end = cp + src_len;
  n = 0;
  while (cp != end && wcp != dest + dest_max)
    {
      gint i, mask = 0, len;
      guchar c = *cp;

      if (c < 0x80)
        {
          len = 1;
          mask = 0x7f;
        }
      else if ((c & 0xe0) == 0xc0)
        {
          len = 2;
          mask = 0x1f;
        }
      else if ((c & 0xf0) == 0xe0)
        {
          len = 3;
          mask = 0x0f;
        }
      else /* Other lengths are not possible with 16-bit wchar_t! */
        return -1;

      if (cp + len > end)
        return -1;

      *wcp = (cp[0] & mask);
      for (i = 1; i < len; i++)
        {
          if ((cp[i] & 0xc0) != 0x80)
            return -1;
          *wcp <<= 6;
          *wcp |= (cp[i] & 0x3f);
        }
      if (*wcp == 0xFFFF)
        return -1;

      cp += len;
      wcp++;
      n++;
    }
  if (cp != end)
    return -1;

  return n;
}

LRESULT CALLBACK
ModeNote::EditWindowProc( HWND hwnd, UINT msg,
			  WPARAM wparam, LPARAM lparam) {
  LRESULT ret;

  ret = CallWindowProc((WNDPROC)pEditWndProc,hwnd,msg,wparam,lparam);

  if ( msg == WM_CHAR ) {
    char buf[512], unibuf[2048];
    GetWindowText( hwnd, buf, 512 );

    char locale[10];
    HKL input_locale = GetKeyboardLayout(0);
    GetLocaleInfo (MAKELCID (LOWORD (input_locale), SORT_DEFAULT),
		   LOCALE_IDEFAULTANSICODEPAGE,
		   locale, sizeof (locale));

    int srclen;
    srclen = MultiByteToWideChar(atoi(locale), 0, buf, 512,
				 (LPWSTR)unibuf, 2048 );

    gtk_entry_set_text( GTK_ENTRY(theInternetPlayView->m_lobbyEdit[0]), 
			_gdk_ucs2_to_utf8((const wchar_t *)unibuf, srclen) );
  }

  return ret;
}

LRESULT CALLBACK
ModeNote::ParentWindowProc(HWND hwnd,UINT msg,WPARAM wparam,LPARAM lparam) {
  if ( msg == WM_SIZE ) {
    MoveWindow( pChildHWnd, 0, 0, lparam&0xFFFF, (lparam>>16)&0xFFFF, TRUE );
  }

  return CallWindowProc((WNDPROC)ModeNote::pParentWndProc,
			hwnd,msg,wparam,lparam);
}
#endif

extern bool isComm;
extern long mode;

extern RCFile *theRC;

extern void StartGame();

extern bool WriteRCFile();

LauncherHeader::LauncherHeader() {
}

LauncherHeader::~LauncherHeader() {
}

void
LauncherHeader::Init( GtkBox *box ) {
  GtkWidget *frame;

  frame = FullScreenFrame();
  gtk_box_pack_start( box, frame, FALSE, FALSE, 10 );
  frame = GraphicsFrame();
  gtk_box_pack_start( box, frame, FALSE, FALSE, 10 );
  frame = SoundFrame();
  gtk_box_pack_start( box, frame, FALSE, FALSE, 10 );
#ifdef ENABLE_IPV6
  frame = ProtocolFrame();
  gtk_box_pack_start( box, frame, FALSE, FALSE, 10 );
#endif
}

GtkWidget *
LauncherHeader::FullScreenFrame() {
  GtkWidget *frame;
  GtkWidget *box;
  GtkWidget *button;
  GSList *list;

  frame = gtk_frame_new( _("FullScreen") );

  box = gtk_hbox_new( FALSE, 10 );
  gtk_container_border_width (GTK_CONTAINER (box), 5);

  button = gtk_radio_button_new_with_label ((GSList *)NULL, _("On"));
  list = gtk_radio_button_group( GTK_RADIO_BUTTON(button) );
  gtk_box_pack_start( GTK_BOX(box), button, FALSE, FALSE, 10 );
  if (theRC->fullScreen)
    gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON(button), TRUE );

  gtk_signal_connect (GTK_OBJECT (button), "clicked",
		      GTK_SIGNAL_FUNC (LauncherHeader::ToggleFullScreen),
		      &theRC->fullScreen);

  button = gtk_radio_button_new_with_label (list, _("Off"));
  list = gtk_radio_button_group( GTK_RADIO_BUTTON(button) );
  gtk_box_pack_start( GTK_BOX(box), button, FALSE, FALSE, 10 );
  if (!theRC->fullScreen)
    gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON(button), TRUE );

  gtk_signal_connect (GTK_OBJECT (button), "clicked",
		      GTK_SIGNAL_FUNC (LauncherHeader::ToggleFullScreen),
		      &theRC->fullScreen);

  gtk_container_add (GTK_CONTAINER (frame), box);

  return frame;
}

GtkWidget *
LauncherHeader::GraphicsFrame() {
  GtkWidget *frame;
  GtkWidget *box;
  GtkWidget *twoDButton, *simpleButton, *normalButton;
  GSList *list;

  frame = gtk_frame_new( _("Graphics") );

  box = gtk_hbox_new( FALSE, 10 );
  gtk_container_border_width (GTK_CONTAINER (box), 5);

#if 0
  twoDButton = gtk_radio_button_new_with_label ( (GSList *)NULL, "2D");
  list = gtk_radio_button_group( GTK_RADIO_BUTTON(twoDButton) );
  gtk_box_pack_start( GTK_BOX(box), twoDButton, FALSE, FALSE, 10 );
  gtk_widget_show (twoDButton);
  if ( theRC->gmode == GMODE_2D ) {
    gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON(twoDButton), TRUE );
  }

  simpleButton = gtk_radio_button_new_with_label ( list, _("Simple"));
#else
  simpleButton = gtk_radio_button_new_with_label ( (GSList *)NULL, _("Simple"));
#endif

  list = gtk_radio_button_group( GTK_RADIO_BUTTON(simpleButton) );
  gtk_box_pack_start( GTK_BOX(box), simpleButton, FALSE, FALSE, 10 );
  if ( theRC->gmode == GMODE_SIMPLE ) {
    gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON(simpleButton), TRUE );
  }

  normalButton = gtk_radio_button_new_with_label (list, _("Normal"));
  list = gtk_radio_button_group( GTK_RADIO_BUTTON(normalButton) );
  gtk_box_pack_start( GTK_BOX(box), normalButton, FALSE, FALSE, 10 );
  if ( theRC->gmode == GMODE_FULL ) {
    gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON(normalButton), TRUE );
  }

#if 0
  gtk_signal_connect (GTK_OBJECT (twoDButton), "clicked",
		      GTK_SIGNAL_FUNC (LauncherHeader::Toggle), &theRC->gmode);
#endif
  gtk_signal_connect (GTK_OBJECT (simpleButton), "clicked",
		      GTK_SIGNAL_FUNC (LauncherHeader::Toggle), &theRC->gmode);
  gtk_signal_connect (GTK_OBJECT (normalButton), "clicked",
		      GTK_SIGNAL_FUNC (LauncherHeader::Toggle), &theRC->gmode);

  gtk_container_add (GTK_CONTAINER (frame), box);

  return frame;
}

GtkWidget *
LauncherHeader::SoundFrame() {
  GtkWidget *frame;
  GtkWidget *box;
  GtkWidget *button;
  GSList *list;

  frame = gtk_frame_new( _("Sound") );

  box = gtk_hbox_new( FALSE, 10 );
  gtk_container_border_width (GTK_CONTAINER (box), 5);

  button = gtk_radio_button_new_with_label ((GSList *)NULL, _("On"));
  list = gtk_radio_button_group( GTK_RADIO_BUTTON(button) );
  gtk_box_pack_start( GTK_BOX(box), button, FALSE, FALSE, 10 );
  if (theRC->sndMode == SOUND_SDL)
    gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON(button), TRUE );

  gtk_signal_connect (GTK_OBJECT (button), "clicked",
		      GTK_SIGNAL_FUNC (LauncherHeader::ToggleSound),
		      &theRC->sndMode);

  button = gtk_radio_button_new_with_label (list, _("Off"));
  list = gtk_radio_button_group( GTK_RADIO_BUTTON(button) );
  gtk_box_pack_start( GTK_BOX(box), button, FALSE, FALSE, 10 );
  if (theRC->sndMode == SOUND_NONE)
    gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON(button), TRUE );

  gtk_signal_connect (GTK_OBJECT (button), "clicked",
		      GTK_SIGNAL_FUNC (LauncherHeader::ToggleSound),
		      &theRC->sndMode);

  gtk_container_add (GTK_CONTAINER (frame), box);

  return frame;
}

#ifdef ENABLE_IPV6
GtkWidget *
LauncherHeader::ProtocolFrame() {
  GtkWidget *frame;
  GtkWidget *box;
  GtkWidget *button;
  GSList *list;

  frame = gtk_frame_new( "Protocol" );

  box = gtk_hbox_new( FALSE, 10 );
  gtk_container_border_width (GTK_CONTAINER (box), 5);

  button = gtk_radio_button_new_with_label ((GSList *)NULL, "IPv4");
  list = gtk_radio_button_group( GTK_RADIO_BUTTON(button) );
  gtk_box_pack_start( GTK_BOX(box), button, FALSE, FALSE, 10 );
  if (theRC->protocol == IPv4)
    gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON(button), TRUE );

  gtk_signal_connect (GTK_OBJECT (button), "clicked",
		      GTK_SIGNAL_FUNC (LauncherHeader::ToggleProtocol),
		      &theRC->protocol);

  button = gtk_radio_button_new_with_label (list, "IPv6");
  list = gtk_radio_button_group( GTK_RADIO_BUTTON(button) );
  gtk_box_pack_start( GTK_BOX(box), button, FALSE, FALSE, 10 );
  if (theRC->protocol == IPv6)
    gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON(button), TRUE );

  gtk_signal_connect (GTK_OBJECT (button), "clicked",
		      GTK_SIGNAL_FUNC (LauncherHeader::ToggleProtocol),
		      &theRC->protocol);

  gtk_container_add (GTK_CONTAINER (frame), box);

  return frame;
}
#endif

void
LauncherHeader::ToggleFullScreen( GtkWidget *widget, gpointer data ) {
  GSList *list = gtk_radio_button_group( (GtkRadioButton *)widget );

  if ( gtk_toggle_button_get_active
       ( (GtkToggleButton *)g_slist_nth_data( list, 0 ) ) ) {
    theRC->fullScreen = false;
  } else {
    theRC->fullScreen = true;
  }
}

void
LauncherHeader::ToggleSound( GtkWidget *widget, gpointer data ) {
  GSList *list = gtk_radio_button_group( (GtkRadioButton *)widget );

  if ( gtk_toggle_button_get_active
       ( (GtkToggleButton *)g_slist_nth_data( list, 0 ) ) ) {
    theRC->sndMode = SOUND_NONE;
  } else {
    theRC->sndMode = SOUND_SDL;
  }
}

#ifdef ENABLE_IPV6
void
LauncherHeader::ToggleProtocol( GtkWidget *widget, gpointer data ) {
  GSList *list = gtk_radio_button_group( (GtkRadioButton *)widget );

  if ( gtk_toggle_button_get_active
       ( (GtkToggleButton *)g_slist_nth_data( list, 0 ) ) ) {
    theRC->protocol = IPv6;
  } else {
    theRC->protocol = IPv4;
  }
}
#endif


void
LauncherHeader::Toggle( GtkWidget *widget, gpointer data ) {
  GSList *list = gtk_radio_button_group( (GtkRadioButton *)widget );

  if ( gtk_toggle_button_get_active
       ( (GtkToggleButton *)g_slist_nth_data( list, 0 ) ) ) {
    theRC->gmode = GMODE_FULL;
    theRC->isTexture = true;
  } else if ( gtk_toggle_button_get_active
	      ( (GtkToggleButton *)g_slist_nth_data( list, 1 ) ) ) {
    theRC->gmode = GMODE_SIMPLE;
    theRC->isTexture = false;
  } else if ( gtk_toggle_button_get_active
	      ( (GtkToggleButton *)g_slist_nth_data( list, 2 ) ) ) {
    theRC->gmode = GMODE_2D;
    theRC->isTexture = false;
  }
}

ModeNote::ModeNote() {
}

ModeNote::~ModeNote() {
}

void
ModeNote::Init( GtkBox *box ) {
  GtkWidget *notebook;
  GtkWidget *noteBox;
  GtkWidget *label;

  notebook = gtk_notebook_new();

  gtk_box_pack_start( box, notebook, FALSE, FALSE, 10 );
  gtk_widget_show(notebook);

  // Solo Play
  noteBox = InitSoloPlayPanel();
  label = gtk_label_new( _("Solo") );
  gtk_notebook_append_page( GTK_NOTEBOOK(notebook), noteBox, label);

  // LAN Play
  noteBox = InitLANPlayPanel();
  label = gtk_label_new( _("LAN") );
  gtk_notebook_append_page( GTK_NOTEBOOK(notebook), noteBox, label);

  // Internet Play
  noteBox = InitInternetPlayPanel();
  label = gtk_label_new( _("Internet(Experimental)") );
  gtk_notebook_append_page( GTK_NOTEBOOK(notebook), noteBox, label);

#ifdef WIN32
  HWND cWnd = NULL;
  cWnd = FindWindowEx( LauncherView::hWnd, NULL, "gdkWindowChild", NULL );
  cWnd = FindWindowEx( LauncherView::hWnd, cWnd, "gdkWindowChild", NULL );
  cWnd = FindWindowEx( cWnd, NULL, "gdkWindowChild", NULL );

  ModeNote::pParentWndProc = GetWindowLong(cWnd, GWL_WNDPROC);
  SetWindowLong(cWnd, GWL_WNDPROC, (LONG)ModeNote::ParentWindowProc);

  ModeNote::pChildHWnd = CreateWindow( "EDIT", NULL, WS_CHILD|WS_VISIBLE, 
				       CW_USEDEFAULT, CW_USEDEFAULT,
				       200, 30,
				       cWnd, NULL, NULL, NULL );

  char buf[512], unibuf[2048];
  char locale[10];
  HKL input_locale = GetKeyboardLayout(0);
  GetLocaleInfo (MAKELCID (LOWORD (input_locale), SORT_DEFAULT),
		 LOCALE_IDEFAULTANSICODEPAGE,
		 locale, sizeof (locale));

  _gdk_utf8_to_ucs2((wchar_t *)unibuf, theRC->nickname, 32, 2048);

  WideCharToMultiByte(atoi(locale), 0, (LPWSTR)unibuf, 2048, 
		      (LPSTR)buf, 512, NULL, NULL);

  SetWindowText( ModeNote::pChildHWnd, buf );

  ModeNote::pEditWndProc = GetWindowLong(ModeNote::pChildHWnd, GWL_WNDPROC);
  SetWindowLong(ModeNote::pChildHWnd, GWL_WNDPROC, (LONG)ModeNote::EditWindowProc);

  theInternetPlayView = this;
#endif
}

GtkWidget *
ModeNote::InitSoloPlayPanel() {
  GtkWidget *box;
  GtkWidget *button;

  box = gtk_vbox_new( FALSE, 10 );
  gtk_container_border_width (GTK_CONTAINER (box), 5);

  button = gtk_button_new_with_label (_("Game Start!"));
  gtk_box_pack_start( GTK_BOX(box), button, FALSE, FALSE, 10 );
  gtk_widget_show (button);

  gtk_signal_connect( GTK_OBJECT (button), "clicked",
		      GTK_SIGNAL_FUNC (ModeNote::StartGame), NULL );

  gtk_widget_show (box);

  return box;
}

GtkWidget *
ModeNote::InitLANPlayPanel() {
  GtkWidget *box, *toggleBox, *editBox;
  GtkWidget *button, *label;
  GtkWidget *toggleButton[2];
  GSList *list;

  box = gtk_vbox_new( FALSE, 5 );
  gtk_container_border_width (GTK_CONTAINER (box), 5);

  // Upper toggle button
  toggleBox = gtk_hbox_new( FALSE, 10 );
  gtk_container_border_width (GTK_CONTAINER (toggleBox), 5);

  toggleButton[0] = gtk_radio_button_new_with_label ((GSList *)NULL, _("Server"));
  list = gtk_radio_button_group( GTK_RADIO_BUTTON(toggleButton[0]) );
  gtk_box_pack_start( GTK_BOX(toggleBox), toggleButton[0], FALSE, FALSE, 5 );
  gtk_widget_show (toggleButton[0]);
  toggleButton[1] = gtk_radio_button_new_with_label (list, _("Client"));
  list = gtk_radio_button_group( GTK_RADIO_BUTTON(toggleButton[1]) );
  gtk_box_pack_start( GTK_BOX(toggleBox), toggleButton[1], FALSE, FALSE, 5 );
  gtk_widget_show (toggleButton[1]);

  gtk_widget_show (toggleBox);
  gtk_box_pack_start( GTK_BOX(box), toggleBox, FALSE, FALSE, 5 );

  if (theRC->serverName[0])
    gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON(toggleButton[1]), TRUE );

  // Lower input area for server name
  editBox = gtk_table_new( 2, 2, FALSE );

  label = gtk_label_new( _("Server name:") );
  gtk_table_attach( GTK_TABLE(editBox), label, 0, 1, 1, 2,
		    GTK_FILL, GTK_EXPAND, 0, 0 );
  gtk_widget_show (label);
  m_serverName = gtk_entry_new ();
  gtk_table_attach( GTK_TABLE(editBox), m_serverName, 1, 2, 1, 2,
		    GTK_FILL, GTK_EXPAND, 0, 0 );
  if ( theRC->serverName[0] > 1 ) {
    gtk_entry_set_text( GTK_ENTRY(m_serverName), theRC->serverName );
    gtk_widget_show ( GTK_WIDGET(m_serverName) );
  }

  gtk_widget_show (editBox);
  gtk_box_pack_start( GTK_BOX(box), editBox, FALSE, FALSE, 5 );

  gtk_signal_connect (GTK_OBJECT (toggleButton[0]), "pressed",
		      GTK_SIGNAL_FUNC (ModeNote::Toggle), m_serverName);
  gtk_signal_connect (GTK_OBJECT (toggleButton[1]), "pressed",
		      GTK_SIGNAL_FUNC (ModeNote::Toggle), m_serverName);

  // lowest area(start button)
  button = gtk_button_new_with_label (_("Game Start!"));
  gtk_box_pack_start( GTK_BOX(box), button, FALSE, FALSE, 5 );
  gtk_widget_show (button);

  gtk_signal_connect (GTK_OBJECT (button), "clicked",
		      GTK_SIGNAL_FUNC (ModeNote::LANStartGame), m_serverName);

  gtk_widget_show (box);

  return box;
}

GtkWidget *
ModeNote::InitInternetPlayPanel() {
  GtkWidget *box, *editBox;
  GtkWidget *button, *label;

  box = gtk_vbox_new( FALSE, 10 );
  gtk_container_border_width (GTK_CONTAINER (box), 5);

  gtk_widget_show (box);

  editBox = gtk_table_new( 2, 2, FALSE );
  gtk_box_pack_start( GTK_BOX(box), editBox, FALSE, FALSE, 5 );

  gtk_widget_show (editBox);

  label = gtk_label_new( _("Nickname:") );
  gtk_table_attach( GTK_TABLE(editBox), label, 0, 1, 0, 1,
		    GTK_FILL, GTK_EXPAND, 0, 0 );
  gtk_widget_show (label);
  m_lobbyEdit[0] = gtk_entry_new();
  gtk_table_attach( GTK_TABLE(editBox), m_lobbyEdit[0], 1, 2, 0, 1,
		    GTK_FILL, GTK_EXPAND, 0, 0 );
  gtk_entry_set_text( GTK_ENTRY(m_lobbyEdit[0]), theRC->nickname );
  gtk_widget_show (m_lobbyEdit[0]);

#if 0
  label = gtk_label_new( _("Message:") );
  gtk_table_attach( GTK_TABLE(editBox), label, 0, 1, 1, 2,
		    GTK_FILL, GTK_EXPAND, 0, 0 );
  gtk_widget_show (label);
  m_lobbyEdit[1] = gtk_entry_new();
  gtk_table_attach( GTK_TABLE(editBox), m_lobbyEdit[1], 1, 2, 1, 2,
		    GTK_FILL, GTK_EXPAND, 0, 0 );
  gtk_widget_show (m_lobbyEdit[1]);
  gtk_entry_set_text( GTK_ENTRY(m_lobbyEdit[1]), theRC->message );
#endif

  button = gtk_button_new_with_label(_("Connect to Lobby Server"));
  gtk_box_pack_start( GTK_BOX(box), button, FALSE, FALSE, 10 );
  gtk_widget_show(button);

  gtk_signal_connect( GTK_OBJECT(button), "clicked",
		      GTK_SIGNAL_FUNC(ModeNote::InternetStartGame), m_lobbyEdit);

  return box;
}

void
ModeNote::Toggle( GtkWidget *widget, gpointer data ) {
  GSList *list = gtk_radio_button_group( (GtkRadioButton *)widget );

  if ( g_slist_index( list, widget ) == 0 ) {
    theRC->serverName[0] = 1;	// :-p
    gtk_widget_show ( GTK_WIDGET(data) );
  } else {
    theRC->serverName[0] = '\0';
    gtk_widget_hide ( GTK_WIDGET(data) );
  }
}

void
ModeNote::StartGame( GtkWidget *widget, gpointer data ) {
  if ( theRC->gmode == GMODE_2D )
    mode = MODE_TITLE;
  ::StartGame();
}

void
ModeNote::LANStartGame( GtkWidget *widget, gpointer data ) {
  if ( theRC->serverName[0] == 1 &&
       strlen(gtk_entry_get_text( GTK_ENTRY(data) )) > 0 )
    strncpy( theRC->serverName, gtk_entry_get_text( GTK_ENTRY(data) ), 256 );
  isComm = true;
  mode = MODE_MULTIPLAYSELECT;
  try {
    ::StartGame();
  } catch ( NetworkError ) {
    LauncherView::ConnectionFailedDialog();
  }
}

void
ModeNote::InternetStartGame( GtkWidget *widget, gpointer data ) {
  LobbyClient *lb;
  lb = LobbyClient::Create();
  if ( lb->
#if 0
       Init((char *)gtk_entry_get_text(GTK_ENTRY(((GtkWidget **)data)[0])),
	    (char *)gtk_entry_get_text(GTK_ENTRY(((GtkWidget **)data)[1])))
#else
       Init((char *)gtk_entry_get_text(GTK_ENTRY(((GtkWidget **)data)[0])),
	    "")
#endif
       == false ) {
    LauncherView::ConnectionFailedDialog();
  }
}


LauncherView::LauncherView() {
}

LauncherView::~LauncherView() {
}

void
LauncherView::Init() {
  GtkStyle *style;
  GtkWidget *label;

  GtkWidget *allbox, *mainbox, *quitBox;

  /* ½é´ü²½ */
  gtk_init( (int *)NULL, (char ***)NULL );
  /*
  style = gtk_widget_get_default_style();
#ifdef WIN32
  //style->font = gdk_font_load(_("-unknown-MS UI Gothic-normal-r-normal--12-100-*-*-*-*-windows-*") );
  style->font = gdk_font_load(_("-*-ms gothic-normal-r-normal--14-*-*-*-*-*-jisx0208.1983-0") );
#endif
  gtk_widget_set_default_style(style);
  */

  /* Window À¸À® */
  m_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_container_border_width (GTK_CONTAINER (m_window), 5);

#ifdef WIN32
  char windowName[32];
  int i;
  for ( i = 0 ; i < 31 ; i++ )
    windowName[i] = 'A'+RAND(26);
  windowName[31] = 0;

  gtk_window_set_title( GTK_WINDOW(m_window), windowName);
#else
  gtk_window_set_title( GTK_WINDOW(m_window), _("Cannon Smash"));
#endif

  gtk_widget_realize(m_window);

#ifdef WIN32
  LauncherView::hWnd = FindWindow( "gdkWindowTopLevel", windowName );
  gtk_window_set_title( GTK_WINDOW(m_window), _("Cannon Smash"));
#endif

  allbox = gtk_hbox_new( FALSE, 5 );
  gtk_container_add (GTK_CONTAINER (m_window), allbox);

  mainbox = gtk_vbox_new( FALSE, 5 );
  quitBox = gtk_vbox_new( FALSE, 5 );

  m_quit = gtk_button_new_with_label (_("Quit"));
  gtk_box_pack_start( GTK_BOX(quitBox), m_quit, FALSE, TRUE, 10 );

  gtk_box_pack_start( GTK_BOX(allbox), mainbox, FALSE, FALSE, 10 );
  gtk_box_pack_start( GTK_BOX(allbox), quitBox, FALSE, TRUE, 10 );

  m_header = new LauncherHeader();
  m_header->Init( GTK_BOX(mainbox) );
  gtk_widget_show_all(m_window);
  m_note = new ModeNote();
  m_note->Init( GTK_BOX(mainbox) );

  label = gtk_label_new( "http://CannonSmash.SourceForge.net\nmailto: nan@utmc.or.jp" );
  gtk_label_set_justify( GTK_LABEL(label), GTK_JUSTIFY_LEFT );
  gtk_box_pack_start( GTK_BOX(mainbox), label, FALSE, TRUE, 0 );
  gtk_widget_show(label);

  gtk_signal_connect( GTK_OBJECT (m_window), "destroy",
		      GTK_SIGNAL_FUNC (LauncherView::Destroy), this );
  gtk_signal_connect( GTK_OBJECT (m_quit), "clicked",
		      GTK_SIGNAL_FUNC (LauncherView::Destroy),
		      this );

  gtk_main();
}

void
LauncherView::Destroy(GtkWidget *widget, gpointer data) {
  ModeNote *note = ((LauncherView *)data)->m_note;

  strncpy( theRC->serverName,
	   gtk_entry_get_text( GTK_ENTRY(note->m_serverName) ), 256 );
  strncpy( theRC->nickname,
	   gtk_entry_get_text( GTK_ENTRY(note->m_lobbyEdit[0]) ), 32 );
#if 0
  strncpy( theRC->message,
	   gtk_entry_get_text( GTK_ENTRY(note->m_lobbyEdit[1]) ), 64 );
#endif

  theRC->WriteRCFile();

  gtk_main_quit();

  gtk_exit(2);
}

void
LauncherView::ConnectionFailedDialog() {
  GtkWidget *dialog, *label, *button;

  dialog = gtk_dialog_new();
  gtk_window_set_title( GTK_WINDOW(dialog), _("Connection failed"));
  gtk_container_border_width( GTK_CONTAINER (dialog), 5 );
  gtk_window_set_modal( (GtkWindow *)dialog, true );

  label = gtk_label_new( "Connection failed." );
  gtk_label_set_line_wrap( GTK_LABEL(label), true );
  gtk_widget_show( label );

  gtk_box_pack_start( GTK_BOX(GTK_DIALOG(dialog)->vbox), label, 
		      TRUE, TRUE, 0 );

  button = gtk_button_new_with_label( "OK" );
  GTK_WIDGET_SET_FLAGS( button, GTK_CAN_DEFAULT );
  gtk_signal_connect_object( GTK_OBJECT(button), "clicked",
			     GTK_SIGNAL_FUNC(gtk_widget_destroy),
			     GTK_OBJECT(dialog) );
  gtk_box_pack_start( GTK_BOX (GTK_DIALOG (dialog)->action_area),
		      button, TRUE, TRUE, 0 );

  gtk_widget_show(button);

  gtk_widget_show(dialog);
}
