/* $Id: LobbyClient.cpp,v 1.41 2003/08/03 10:09:27 nan Exp $ */

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
#include "MultiPlay.h"
#include "Network.h"
#include "RCFile.h"

#if !defined(WIN32)
#include <netinet/tcp.h>
#include <locale.h>
#endif

extern RCFile *theRC;

extern bool isComm;
extern long mode;

extern void StartGame();

extern int listenSocket[];
extern int one;

// dirty... used by "QP"
long score1 = 0;
long score2 = 0;

LobbyClient* LobbyClient::m_lobbyClient = NULL;

LobbyClient::LobbyClient() {
  m_playerNum = 0;
  m_canBeServer = false;
  m_selected = -1;
}

LobbyClient::~LobbyClient() {
  if ( m_view )
    delete m_view;
  m_lobbyClient = NULL;
}

LobbyClient*
LobbyClient::Create() {
  m_lobbyClient = new LobbyClient();
  return m_lobbyClient;
}

bool
LobbyClient::Init( char *nick, char *message ) {
  char buf[1024];

#ifdef ENABLE_IPV6
  char port[10];

  sprintf( port, "%d", theRC->csmash_port );
#endif

  // open listening port
  if ( listenSocket[0] < 0 ) {
    if ( !::GetSocket() )
      return false;
  }

  // Get language code
  int i = 0;
#ifdef WIN32
  while ( table[i].langID != -1 ) {
    if ( PRIMARYLANGID(GetUserDefaultLangID()) == table[i].langID )
      break;
    i++;
  }
#else
  while ( table[i].langID != -1 ) {
    if ( strncmp( setlocale(LC_MESSAGES, NULL), table[i].code, 2 ) == 0 )
      break;
    i++;
  }
#endif
  m_lang = i;

  // connect to lobby server
#ifdef ENABLE_IPV6
  int error;
  struct addrinfo saddr, *res, *res0;

  memset(&saddr, 0, sizeof(saddr));

  if ( theRC->protocol == IPv6 )
    saddr.ai_family = PF_UNSPEC;
  else
    saddr.ai_family = PF_INET;

  saddr.ai_socktype = SOCK_STREAM;
  sprintf( port, "%d", LOBBYSERVER_PORT );
  error = getaddrinfo( LOBBYSERVER_NAME, port, &saddr, &res0 );
  if (error) {
    xerror("%s: %s(%d) getaddrinfo",
	   gai_strerror(error), __FILE__, __LINE__);
    return false;
  }

  m_socket = -1;

  for ( res = res0 ; res ; res = res->ai_next ) {
    if ( (m_socket = socket( res->ai_family, res->ai_socktype,
			     res->ai_protocol )) < 0 ) 
      continue;

    if ( connect( m_socket, res->ai_addr, res->ai_addrlen ) < 0 ) {
      close(m_socket);
      m_socket = -1;
      continue;
    }
    break;
  }

  freeaddrinfo(res0);

  if ( m_socket < 0 ) {
    xerror("%s(%d) connect", __FILE__, __LINE__);
    return false;
  }
#else
  struct hostent *hent;
  struct sockaddr_in saddr;

  memset(&saddr, 0, sizeof(saddr));

  hent = gethostbyname( LOBBYSERVER_NAME );
  memcpy( &saddr.sin_addr, hent->h_addr, hent->h_length );

  saddr.sin_family = AF_INET;
  saddr.sin_port = htons(LOBBYSERVER_PORT);

  // connect
  if ( (m_socket = socket( PF_INET, SOCK_STREAM, 0 )) < 0 ) {
    xerror("%s(%d) socket", __FILE__, __LINE__);
    return false;
  }
  if ( connect( m_socket, (struct sockaddr *)&saddr, sizeof(saddr) ) ) {
    xerror("%s(%d) connect", __FILE__, __LINE__);
    return false;
  }
#endif

  // Connect to Lobby Server
  send( m_socket, "CN", 2, 0 );
  long len = 11 + 32 + 64;
  SendLong( m_socket, len );

  // Send version number(Must be changed)
  char ver;
  ver = MAJOR_VERSION;
  send( m_socket, &ver, 1, 0 );
  ver = MIDDLE_VERSION;
  send( m_socket, &ver, 1, 0 );
  ver = MINOR_VERSION;
  send( m_socket, &ver, 1, 0 );

  // Send port number(Must be changed, too)
  SendLong( m_socket, theRC->csmash_port );
  SendLong( m_socket, table[m_lang].langID );

  // send nick
  strncpy( buf, nick, 32 );
  send( m_socket, buf, 32, 0 );

  // send message
  strncpy( buf, message, 64 );
  send( m_socket, buf, 64, 0 );

  m_view = new LobbyClientView();
  m_view->Init( this );

  strncpy( m_nickname, nick, 32 );
  return true;
}

gint
LobbyClient::PollServerMessage( gpointer data ) {
  LobbyClient *lobby = (LobbyClient *)data;
  fd_set rdfds;
  struct timeval to;

  while (1) {
    int max = 0;
    FD_ZERO( &rdfds );
    FD_SET( (SOCKET)lobby->m_socket, &rdfds );
    int i = 0;
    while ( listenSocket[i] >= 0 ) {
      FD_SET( listenSocket[i], &rdfds );
      if ( listenSocket[i] > max )
	max = listenSocket[i];
      i++;
    }

    if ( (SOCKET)lobby->m_socket > max )
      max = lobby->m_socket;

    to.tv_sec = to.tv_usec = 0;

    if ( select( max+1, &rdfds, NULL, NULL, &to ) <= 0 ) {
      return 1;
    } else {
      int i = 0;
      bool listenIsSet = false;
      while ( listenSocket[i] >= 0 ) {
	if ( FD_ISSET( listenSocket[i], &rdfds ) ) {
	  int acSocket;
	  acSocket = accept( listenSocket[i], NULL, NULL );

	  closesocket( acSocket );
	  lobby->m_canBeServer = true;
	  listenIsSet = true;
	  break;
	}
	i++;
      }

      if ( !listenIsSet ) {
	char buf[1024];

	ReadHeader( lobby->m_socket, buf );

	if ( !strncmp( buf, "UI", 2 ) ) {
	  lobby->ReadUI();
	  lobby->m_view->UpdateTable();
	} else if ( !strncmp( buf, "PI", 2 ) ) {
	  lobby->ReadPI();
	} else if ( !strncmp( buf, "OI", 2 ) ) {
	  lobby->ReadOI();
	} else if ( !strncmp( buf, "AP", 2 ) ) {
	  char *buffer;
	  ReadEntireMessage( lobby->m_socket, &buffer );

	  delete buffer;

	  isComm = true;
	  mode = MODE_MULTIPLAYSELECT;

	  if ( lobby->m_canBeServer == true ) {	// I can be server
	    theRC->serverName[0] = '\0';
	  }

	  lobby->m_view->SetSensitive( true );

	  lobby->SendSP();

	  ::StartGame();

	  lobby->SendQP();
	} else if ( !strncmp( buf, "DP", 2 ) ) {
	  char *buffer;
	  ReadEntireMessage( lobby->m_socket, &buffer );

	  delete buffer;

	  lobby->m_view->SetSensitive( true );
	} else if ( !strncmp( buf, "OV", 2 ) ) {
	  lobby->ReadOV();
	} else if ( !strncmp( buf, "MS", 2 ) ) {
	  lobby->ReadMS();
	} else {
	  xerror("%s(%d) read header", __FILE__, __LINE__);
	  exit(1);
	}
      }
    }
  }
}

void
LobbyClient::Connect( GtkWidget *widget, gpointer data ) {
  LobbyClient *lobby = (LobbyClient *)data;

  // Get selected player information
  send( lobby->m_socket, "OR", 2, 0 );
  long len = 4;
  SendLong( lobby->m_socket, len );
  SendLong( lobby->m_socket, lobby->m_selected );

  // Send Private IRC Message(Although it is not IRC at now)
  send( lobby->m_socket, "PI", 2, 0 );
  len = 4;
  SendLong( lobby->m_socket, len );
  SendLong( lobby->m_socket, lobby->m_selected );

  lobby->m_view->SetSensitive( false );
  //printf( "%d\n", lobby->m_selected );
}

void
LobbyClient::ReadUI() {
  // get length
  long len = 0;

  char *buffer;
  ReadEntireMessage( m_socket, &buffer );

  // Get player number
  ReadLong( buffer, m_playerNum );
  len = 4;

  m_player = new PlayerInfo[m_playerNum];

  // Analyse data
  int i;
  for ( i = 0 ; i < m_playerNum ; i++ ) {
    if ( buffer[len] )
      m_player[i].m_canBeServer = true;
    else
      m_player[i].m_canBeServer = false;
    len++;

    if ( buffer[len] )
      m_player[i].m_playing = true;
    else
      m_player[i].m_playing = false;
    len++;

    ReadLong( buffer+len, m_player[i].m_ID );
    len += 4;

    strncpy( m_player[i].m_nickname, &(buffer[len]), 32 );
    len += 32;

    strncpy( m_player[i].m_message, &(buffer[len]), 64 );
    len += 64;
  }	// add protocolLen check later

  delete buffer;
}

void
LobbyClient::ReadPI() {
  long len = 0;
  long uniqID;

  char *buffer;
  ReadEntireMessage( m_socket, &buffer );

  // get uniq ID
  ReadLong( buffer, uniqID );

  // Get opponent info(Not so good. Must be modified later. )
  send( m_socket, "OR", 2, 0 );
  len = 4;
  SendLong( m_socket, len );
  SendLong( m_socket, uniqID );

  // Is it OK to do this here? 
  PIDialog *piDialog = new PIDialog( this );
  piDialog->PopupDialog( uniqID );

  delete buffer;
}

void
LobbyClient::ReadOI() {
  char *buffer;
  ReadEntireMessage( m_socket, &buffer );

  // get IP address
  sprintf( theRC->serverName, "%d.%d.%d.%d",
	   (unsigned char)buffer[0], (unsigned char)buffer[1],
	   (unsigned char)buffer[2], (unsigned char)buffer[3] );
  // get port number
  long tmpPort;
  ReadLong( &(buffer[4]), tmpPort );
  theRC->csmash_port = tmpPort;

  // At now, Ignore server or client. 

  //printf( "%s %d\n", theRC->serverName, theRC->csmash_port );

  delete buffer;
}

void
LobbyClient::ReadOV() {
  char *buffer;
  ReadEntireMessage( m_socket, &buffer );

  // get version number
  char version[16];
  sprintf( version, "%d.%d.%d",
	   (unsigned char)buffer[0], (unsigned char)buffer[1],
	   (unsigned char)buffer[2] );
  m_view->ShowUpdateDialog(version, &(buffer[3])); // second argument is URL. 

  delete buffer;
}

void
LobbyClient::ReadMS() {
  char *buffer;
  long len;
  len = ReadEntireMessage( m_socket, &buffer );

  long channelID;
  ReadLong( buffer, channelID );
  buffer[len] = 0;
  m_view->AddChatMessage( channelID, &(buffer[4]) );

  delete buffer;
}

void
LobbyClient::SendAP( long uniqID ) {
  send( m_socket, "AP", 2, 0 );
  long len = 4;
  SendLong( m_socket, len );
  SendLong( m_socket, uniqID );
}

void
LobbyClient::SendSP() {
  send( m_socket, "SP", 2, 0 );
  long len = 0;
  SendLong( m_socket, len );
}

void
LobbyClient::SendQP() {
  send( m_socket, "QP", 2, 0 );
  long len = 8;
  SendLong( m_socket, len );

  SendLong( m_socket, score1 );	// Temp
  SendLong( m_socket, score2 );
}

void
LobbyClient::SendDP( long uniqID ) {
  send( m_socket, "DP", 2, 0 );
  long len = 4;
  SendLong( m_socket, len );
  SendLong( m_socket, uniqID );
}

void
LobbyClient::SendQT() {
  send( m_socket, "QT", 2, 0 );
  long len = 0;
  SendLong( m_socket, len );
  shutdown( m_socket, 2 );
}

void
LobbyClient::SendSC( int score1, int score2 ) {
  send( m_socket, "SC", 2, 0 );
  long len = 8;
  SendLong( m_socket, len );

  SendLong( m_socket, score1 );	// Temp
  SendLong( m_socket, score2 );
}

void
LobbyClient::SendMS( char *message, long channel ) {
  send( m_socket, "MS", 2, 0 );
  long len = 4+strlen(message);
  SendLong( m_socket, len );

  SendLong( m_socket, channel );
  send( m_socket, message, strlen(message), 0 );
}


PlayerInfo::PlayerInfo() {
}

PlayerInfo::~PlayerInfo() {
}
