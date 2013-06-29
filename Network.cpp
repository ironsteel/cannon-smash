/* $Id: Network.cpp,v 1.15 2003/08/17 11:31:37 nan Exp $ */

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

#include "ttinc.h"
#include "Network.h"
#include "MultiPlay.h"
#include "Event.h"
#include "Ball.h"
#include "Player.h"
#include "NetPenAttack.h"
#include "NetPenDrive.h"
#include "NetShakeCut.h"
#include "RCFile.h"
#include "Control.h"

#ifdef LOGGING
#include "Logging.h"
#endif

#if !defined(WIN32)
#include <netinet/tcp.h>
#endif

#if defined(WIN32) || defined(__FreeBSD__)

typedef int socklen_t;		/* mimic Penguin's typedef */

#else	/* ! WIN32 */

#define closesocket(FD) close(FD)

#endif

bool endian;

long timeAdj = 0;

extern int theSocket;
extern Ball theBall;
extern RCFile *theRC;
extern long mode;

int listenSocket[16] = {-1, -1, -1, -1, -1, -1, -1, -1,
			-1, -1, -1, -1, -1, -1, -1, -1 };
extern int one;

// convert endian
double
SwapDbl( double d ) {
  if ( endian ) {
    return d;
  } else {
    double swp;
    char *c1 = (char *)&d, *c2 = (char *)&swp;

    for ( int i = 0 ; i < 8 ; i++ )
      *(c2+7-i) = *(c1+i);

    return swp;
  }
}

long
SwapLong( long l ) {
  if ( endian ) {
    return l;
  } else {
    long swp;
    char *c1 = (char *)&l, *c2 = (char *)&swp;

    for ( int i = 0 ; i < 4 ; i++ )
      *(c2+3-i) = *(c1+i);

    return swp;
  }
}

// test endian
void
EndianCheck() {
  long n = 1;
  if ( *((char *)&n) == 1 )
    endian = false;
  else
    endian = true;
}

bool
SendDouble( int sd, double d ) {
  d = SwapDbl(d);

  if ( send( sd, (char *)&d, 8, 0 ) == 8 )
    return true;
  else
    return false;
}

bool
SendLong( int sd, long l ) {
  l = SwapLong(l);

  if ( send( sd, (char *)&l, 4, 0 ) == 4 )
    return true;
  else
    return false;
}

// It seems to be strange...
char *
ReadDouble( char *buf, double& d ) {
  memcpy( &d, buf, 8 );
  d = SwapDbl(d);

  return buf+8;
}

char *
ReadLong( char *buf, long& l ) {
  memcpy( &l, buf, 4 );
  l = SwapLong(l);

  return buf+4;
}

// Send time using "TM" protocol
void
SendTime( int sd, struct timeb* tb ) {
  long millitm = tb->millitm;
  send( sd, "TM", 2, 0 );
  SendLong( sd, tb->time );
  SendLong( sd, millitm );
#ifdef LOGGING
  Logging::GetLogging()->LogTime( LOG_COMMISC, tb );
  Logging::GetLogging()->Log( LOG_COMMISC, "SendTime\n" );
#endif
}

void
ReadTime( int sd, struct timeb* tb ) {
  char buf[256];
  long len;

  len = 0;
  while (1) {
    if ( (len+=recv( sd, buf+len, 2-len, 0 )) == 2 )
      break;
  }

  len = 0;
  while (1) {
    if ( (len+=recv( sd, buf+len, 8-len, 0 )) == 8 )
      break;
  }

  char *b = buf;
  long millitm;
  b = ReadLong( b, tb->time );
  b = ReadLong( b, millitm );
  tb->millitm = (unsigned short)millitm;

#ifdef LOGGING
  struct timeb tbCurrent;

  getcurrenttime( &tbCurrent );

  Logging::GetLogging()->LogTime( LOG_COMMISC, &tbCurrent );
  Logging::GetLogging()->Log( LOG_COMMISC, "ReadTime " );
  Logging::GetLogging()->LogTime( LOG_COMMISC, tb );
  Logging::GetLogging()->Log( LOG_COMMISC, "\n" );
#endif
}

// Read two character header of incoming message
void
ReadHeader( int socket, char *buf ) {
  long len = 0;
  while (1) {
    if ( (len+=recv( socket, buf+len, 2-len, 0 )) == 2 )
      break;
  }
}

// Read entire message, except for header and length field
long
ReadEntireMessage( int socket, char **buf ) {
  long msgLength;
  long len = 0;
  char lengthBuf[16];

  while (1) {
    if ( (len+=recv( socket, lengthBuf+len, 4-len, 0 )) == 4 )
      break;
  }
  ReadLong( lengthBuf, msgLength );

  // Read all
  (*buf) = new char[msgLength+1];
  len = 0;
  while (1) {
    if ( (len+=recv( socket, (*buf)+len, msgLength-len, 0 )) == msgLength )
      break;
  }

  return msgLength;
}

// Send PlayerSwing using "PS" and "PV" protocol
void
SendSwing( Player *player ) {
  char buf[256];

  if ( Event::TheEvent()->IsBackTracking() || mode != MODE_MULTIPLAY )
    return;

  strncpy( buf, "PS", 2 );
  ((MultiPlay *)Control::TheControl())->SendTime( &(buf[2]) );

  player->SendSwing( &(buf[7]) );

  // Player 位置情報も送信する
  strncpy( &(buf[31]), "PV", 2 );
  ((MultiPlay *)Control::TheControl())->SendTime( &(buf[33]) );

  player->SendLocation( &(buf[38]) );

  send( theSocket, buf, 31+55, 0 );
}

void
getcurrenttime( struct timeb *tb ) {
#ifdef WIN32
  ftime( tb );
#else
  struct timeval tv;
  struct timezone tz;

  gettimeofday( &tv, &tz );
  tb->time = tv.tv_sec;
  tb->millitm = tv.tv_usec/1000;
#endif
}

void
ReadBI() {
  long len;
  char buf[256];

  // Read Ball Data
  len = 0;
  while (1) {
    if ( (len+=recv( theSocket, buf+len, 2-len, 0 )) == 2 )
      break;
  }

  if ( strncmp( buf, "BI", 2 ) ) {
    xerror("%s(%d) recv BI", __FILE__, __LINE__);
    throw NetworkError();
  }

  len = 0;
  while (1) {
    if ( (len+=recv( theSocket, buf+len, 60-len, 0 )) == 60 )
      break;
  }

#ifdef LOGGING
  Logging::GetLogging()->LogTime( LOG_COMBALL );
  Logging::GetLogging()->Log( LOG_COMBALL, "recv BI: " );
#endif

  theBall.Warp( buf );

#ifdef LOGGING
  Logging::GetLogging()->LogBall( LOG_COMBALL, &theBall );
#endif
}

#ifdef ENABLE_IPV6
struct addrinfo *
findhostname() {
  if (1 == theRC->serverName[0]) { // Broadcast mode
      printf( "Broadcast is not supported\n" );
      throw NetworkError();
  }
  struct addrinfo hent, *res;
  int error;

  memset(&hent, 0, sizeof(hent));
  if ( theRC->protocol == IPv6 )
    hent.ai_family = PF_UNSPEC;
  else
    hent.ai_family = PF_INET;

  hent.ai_socktype = SOCK_STREAM;

  error = getaddrinfo( theRC->serverName, NULL, &hent, &res );
  if (error) {
    xerror("%s: %s(%d) getaddrinfo",
	   gai_strerror(error), __FILE__, __LINE__);
    throw NetworkError();
  }

  return res;
}
#else

void
findhostname( struct sockaddr_in *saddr ) {
  if (1 == theRC->serverName[0]) { // Broadcast mode
    struct sockaddr_in sba;
    memset(&sba, 0, sizeof(sba));
    unsigned int sb;
    if (0 > (sb = socket(PF_INET, SOCK_DGRAM, 0))) {
      xerror("%s(%d) socket", __FILE__, __LINE__);
      throw NetworkError();
    }

    int one = 1;
    setsockopt( sb, IPPROTO_TCP, TCP_NODELAY, (char*)&one, sizeof(int) );

    one = 1;
    setsockopt(sb, SOL_SOCKET, SO_BROADCAST, (char*)&one, sizeof(one));
    sba.sin_family = AF_INET;
    sba.sin_addr.s_addr = INADDR_ANY;
    sba.sin_port = 0;
    if (0 != bind(sb, (sockaddr*)&sba, sizeof(sba))) {
      xerror("%s(%d) bind", __FILE__, __LINE__);
      throw NetworkError();
    }

    int x = 0;
    do {
      char buf[8];
      memset(&sba, 0, sizeof(sba));
      sba.sin_family = AF_INET;
      sba.sin_addr.s_addr = INADDR_BROADCAST;
      sba.sin_port = htons(theRC->csmash_port);

      sendto(sb, buf, 0, 0, (sockaddr*)&sba, sizeof(sba));
      fd_set fd;
      FD_ZERO(&fd);
      FD_SET(sb, &fd);
      timeval tv;
      memset(&tv, 0, sizeof(tv));
      tv.tv_sec = 1;
      if (0 < select(sb+1, &fd, 0, 0, &tv)) {
	socklen_t l = sizeof(sba);
	int i;
	i = recvfrom(sb, buf, sizeof(buf), 0, (sockaddr*)&sba, &l);
	printf("recvfrom %d %s\n", i, inet_ntoa(sba.sin_addr));      
	break;
      } else {
	printf("%d ", x);
      }
    } while (x++ < 30);
    closesocket(sb);
    if (30 <= x) {
      // notfound
      printf("timeout\n");
      throw NetworkError();
    }
    saddr->sin_addr.s_addr = sba.sin_addr.s_addr;
  } else {
    struct hostent *hent;
    hent = gethostbyname( theRC->serverName );
    memcpy( &saddr->sin_addr, hent->h_addr, hent->h_length );
  }
}
#endif

void
ClearSocket() {
  if ( theSocket >= 0 ) {
    send( theSocket, "QT", 2, 0 );
    closesocket( theSocket );
    theSocket = -1;
  }
}

bool
GetSocket() {
#ifdef ENABLE_IPV6
  int error;
  struct addrinfo saddr, *res, *res0;
  int s;
  char port[10];

  sprintf( port, "%d", theRC->csmash_port );

  memset( &saddr, 0, sizeof(saddr) );
  if ( theRC->protocol == IPv6 )
    saddr.ai_family = PF_UNSPEC;
  else
    saddr.ai_family = PF_INET;
  saddr.ai_socktype = SOCK_STREAM;
  saddr.ai_flags = AI_PASSIVE;
  error = getaddrinfo( NULL, port, &saddr, &res0 );
  if (error) {
    xerror("%s(%d) getaddrinfo", __FILE__, __LINE__);
    return false;
  }

  int i = 0;

  for ( res = res0 ; res ; res = res->ai_next ) {
    if ( (s = socket( res->ai_family, res->ai_socktype,
		      res->ai_protocol )) < 0 ) {
      continue;
    }

#ifdef IPV6_V6ONLY
    if ( res->ai_family == AF_INET6 &&
	 setsockopt( s, IPPROTO_IPV6, IPV6_V6ONLY, &on, sizeof(on) ) < 0 ) {
      close(s);
      continue;
    }
#endif

    setsockopt( s, IPPROTO_TCP, TCP_NODELAY,
		(char*)&one, sizeof(int) );

    if ( bind( s, res->ai_addr, res->ai_addrlen ) < 0 ) {
      close(s);
      continue;
    }

    if ( listen( s, 1 ) < 0 ) {
      close(s);
      continue;
    }

    listenSocket[i] = s;
    i++;
  }

  if ( i == 0 ) {
    xerror("%s(%d) socket", __FILE__, __LINE__);
    return false;
  }

  freeaddrinfo( res0 );
  return true;
#else
  struct sockaddr_in saddr;

  if ( (listenSocket[0] = socket( PF_INET, SOCK_STREAM, 0 )) < 0 ) {
    xerror("%s(%d) socket", __FILE__, __LINE__);
    return false;
  }

  setsockopt( listenSocket[0], IPPROTO_TCP, TCP_NODELAY,
	      (char*)&one, sizeof(int) );

  saddr.sin_addr.s_addr = htonl(INADDR_ANY);
  saddr.sin_family = AF_INET;
  saddr.sin_port = htons(theRC->csmash_port);
  if (bind(listenSocket[0], (struct sockaddr *)&saddr, sizeof(saddr) ) < 0) {
    xerror("%s(%d) bind", __FILE__, __LINE__);
    return false;
  }

  if ( listen( listenSocket[0], 1 ) < 0 ) {
    xerror("%s(%d) socket", __FILE__, __LINE__);
    return false;
  }

  return true;
#endif
}

bool
WaitForClient() {
  socklen_t fromlen;
#ifdef ENABLE_IPV6
  char port[10];

  sprintf( port, "%d", theRC->csmash_port );
#endif

  if ( listenSocket[0] < 0 ) {
    if ( !GetSocket() )
      throw NetworkError();
  }

  unsigned int sb;
#ifdef ENABLE_IPV6
  struct addrinfo sba;
  struct addrinfo *res;
  memset(&sba, 0, sizeof(sba));

  int error;
  if ( theRC->protocol == IPv6 )
    sba.ai_family = AF_INET6;
  else
    sba.ai_family = AF_INET;

  sba.ai_socktype = SOCK_DGRAM;
  sba.ai_flags = AI_PASSIVE;
  error = getaddrinfo( NULL, port, &sba, &res );
  if (error || res->ai_next) {
    xerror("%s(%d) getaddrinfo", __FILE__, __LINE__);
    throw NetworkError();
  }

  if ( 0 > (sb = socket( res->ai_family, res->ai_socktype,
			 res->ai_protocol )) ) {
    xerror("%s(%d) socket", __FILE__, __LINE__);
    throw NetworkError();
  }

#ifdef IPV6_V6ONLY
  if ( res->ai_family == AF_INET6 &&
       setsockopt( s, IPPROTO_IPV6, IPV6_V6ONLY, &on, sizeof(on) ) < 0 ) {
    close(sb);
    xerror("%s(%d) setsockopt", __FILE__, __LINE__);
    return false;
  }
#endif

  if (0 > bind(sb, res->ai_addr, res->ai_addrlen)) {
    xerror("%s(%d) bind", __FILE__, __LINE__);
    throw NetworkError();
  }
#else
  struct sockaddr_in sba;
  if (0 > (sb = socket(PF_INET, SOCK_DGRAM, 0))) {
    xerror("%s(%d) socket", __FILE__, __LINE__);
    throw NetworkError();
  }
  sba.sin_addr.s_addr = INADDR_ANY;
  sba.sin_family = AF_INET;
  sba.sin_port = htons(theRC->csmash_port);
  if (0 > bind(sb, (struct sockaddr*)&sba, sizeof(sba))) {
    xerror("%s(%d) bind", __FILE__, __LINE__);
    throw NetworkError();
  }
#endif

  // wait for connection / broadcast packet
  printf("server selecting\n");

  fd_set fd;

  do {
    int max = 0;
    int i = 0;

    FD_ZERO(&fd);
    while ( listenSocket[i] >= 0 ) {
      FD_SET(listenSocket[i], &fd);
      if ( listenSocket[i] > max )
	max = listenSocket[i];
      i++;
    }
    FD_SET(sb, &fd);

    max = max > sb ? max : sb;
    if (0 <= select(max+1, &fd, NULL, NULL, NULL)) {
      if (FD_ISSET(sb, &fd)) {
	// datagram to udp port
	printf("server recived broadcast packet");
	if ( theRC->protocol == IPv6 ) {
	  printf(", but not supported\n" );
	  throw NetworkError();
	} else {
	  char buf[1];
	  sockaddr_in client;
	  socklen_t l = sizeof(client);

	  if ( 0 <= recvfrom(sb, (char*)&buf, sizeof(buf), 0,
			     (sockaddr*)&client, &l)) {
	    printf("%s\n", inet_ntoa(client.sin_addr));
	    struct hostent *h;
	    char buf[128];
	    if (0 == gethostname(buf, sizeof(buf))) {
	      h = gethostbyname(buf);
	      if (h) {
		sockaddr_in a;
		memcpy(&a.sin_addr, h->h_addr, h->h_length);
		printf("send %s\n", inet_ntoa(a.sin_addr));
		l = sizeof(client);
		sendto(sb, (char*)&a.sin_addr.s_addr,
		       sizeof(a.sin_addr.s_addr), 0, (sockaddr*)&client, l);
	      }
	    }
	  }
	}
      } else {
	// connection to tcp port
	break;
      }
    } else {
      // select error... No, Timeout
      xerror("%s(%d): select");
    }
  } while (1);
  closesocket(sb);

  if ( theRC->protocol == IPv6 ) {
#ifdef ENABLE_IPV6
    struct addrinfo faddr;
    int i = 0;

    fromlen = sizeof(faddr);
    while ( listenSocket[i] >= 0 ) {
      if (FD_ISSET(listenSocket[i], &fd)) {
	theSocket = accept( listenSocket[i],
			    (struct sockaddr *)&faddr, &fromlen );
	break;
      }
      i++;
    }
#endif
  } else {
    struct sockaddr_in faddr;
    int i = 0;

    fromlen = sizeof(faddr);
    while ( listenSocket[i] >= 0 ) {
      if (FD_ISSET(listenSocket[i], &fd)) {
	theSocket = accept( listenSocket[i],
			    (struct sockaddr *)&faddr, &fromlen );
	break;
      }
      i++;
    }
  }
  setsockopt( theSocket, IPPROTO_TCP, TCP_NODELAY, (char*)&one, sizeof(int) );

  if (0 > theSocket) {
    xerror("%s(%d) accept", __FILE__, __LINE__);
    throw NetworkError();
  }

  return true;
}

bool
ConnectToServer() {
  char buf[128];
  int i;

#ifdef ENABLE_IPV6
  struct addrinfo *saddr, saddr2, *res, *res0;
  char hbuf[32];
  char port[10];
  int error;

  sprintf( port, "%d", theRC->csmash_port );

  saddr = findhostname();

  if ( getnameinfo( saddr->ai_addr, saddr->ai_addrlen,
		    hbuf, sizeof(hbuf), NULL, 0, 0 ) == 0 )
    printf( "server is %s\n", hbuf );

  memset( &saddr2, 0, sizeof(saddr2) );
  if ( theRC->protocol == IPv6 )
    saddr2.ai_family = PF_UNSPEC;
  else
    saddr2.ai_family = PF_INET;
  saddr2.ai_socktype = SOCK_STREAM;

  error = getaddrinfo( hbuf, port, &saddr2, &res0 );

  if (error) {
    xerror("%s: %s(%d) getaddrinfo",
	   gai_strerror(error), __FILE__, __LINE__);
    throw NetworkError();
  }

  theSocket = -1;

  for ( res = res0 ; res ; res = res->ai_next ) {
    if ( (theSocket = socket( res->ai_family, res->ai_socktype,
			      res->ai_protocol )) < 0 ) 
      continue;

    setsockopt(theSocket, IPPROTO_TCP, TCP_NODELAY, (char*)&one, sizeof(int));

    for ( i = 0 ; i < 10 ; i++ ) {
      if ( !connect( theSocket, res->ai_addr, res->ai_addrlen ) )
	break;
#ifdef WIN32
      Sleep(3000);
#else
      sleep(3);
#endif
    }

    if ( i < 10 )
      break;

    close(theSocket);
    theSocket = -1;
    continue;
  }

  if ( theSocket < 0 ) {
    xerror("%s(%d) connect", __FILE__, __LINE__);
    throw NetworkError();
  }

  freeaddrinfo(saddr);
  freeaddrinfo(res0);
#else
  struct sockaddr_in saddr;
  memset(&saddr, 0, sizeof(saddr));

  findhostname(&saddr);

  printf("server is %s\n", inet_ntoa(saddr.sin_addr));
  saddr.sin_family = AF_INET;
  saddr.sin_port = htons(theRC->csmash_port);

  // connect
  if ( (theSocket = socket( PF_INET, SOCK_STREAM, 0 )) < 0 ) {
    xerror("%s(%d) socket", __FILE__, __LINE__);
    throw NetworkError();
  }
  setsockopt(theSocket, IPPROTO_TCP, TCP_NODELAY, (char*)&one, sizeof(int));

  for ( i = 0 ; i < 10 ; i++ ) {
    if ( !connect( theSocket, (struct sockaddr *)&saddr, sizeof(saddr) ) )
      break;
#ifdef WIN32
    Sleep(3000);
#else
    sleep(3);
#endif
  }

  if ( i == 10 ) {
    xerror("%s(%d) connect", __FILE__, __LINE__);
    throw NetworkError();
  }
#endif
}

void
ServerAdjustClock() {
  long adjLog[16];
  int i, j;

  for ( i = 0 ; i < 16 ; i++ ) {
    struct timeb tb1, tb2, tb3;
    long diff;

    getcurrenttime( &tb1 );

    ::SendTime( theSocket, &tb1 );
    ReadTime( theSocket, &tb3 );

    getcurrenttime( &tb2 );

    diff = ((long)(tb2.time-tb1.time)*1000 + tb2.millitm-tb1.millitm)/2;
    long mtm = tb1.millitm+diff;
    while ( mtm >= 1000 ) {
      mtm -= 1000;
      tb1.time++;
    }
    while ( mtm < 1000 ) {
      mtm += 1000;
      tb1.time--;
    }
    tb1.millitm = (unsigned short)mtm;

    adjLog[i] = (tb3.time-tb1.time)*1000 + tb3.millitm-tb1.millitm;
  }

  // Bubble sort
  i = 0;
  while ( i == 0 ) {
    i = 1;
    for ( j = 0 ; j < 15 ; j++ ) {
      if ( adjLog[j] > adjLog[j+1] ) {
	long k = adjLog[j];
	adjLog[j] = adjLog[j+1];
	adjLog[j+1] = k;
	i = 0;
      }
    }
  }

  for ( i = 0 ; i < 16 ; i++ ) {
    printf( "%d ", adjLog[i] );
  }
  printf( "\n" );

  // Use 8 medium value
  timeAdj = 0;
  for ( i = 4 ; i < 12 ; i++ ) {
    timeAdj += adjLog[i];
  }

  timeAdj /= 80;	/* 8*10 */

  printf( "%d\n", timeAdj );
}


void
ClientAdjustClock() {
  // Respond server's AdjustClock()
  for ( int i = 0 ; i < 16 ; i++ ) {
    struct timeb tb;

    ReadTime( theSocket, &tb );	// Dispose
    getcurrenttime( &tb );
    ::SendTime( theSocket, &tb );
  }
}
