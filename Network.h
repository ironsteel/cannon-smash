/* $Id: Network.h,v 1.9 2003/01/19 15:41:00 nan Exp $ */

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

#ifndef _Network_
#define _Network_

class Player;

// Endian
void EndianCheck();

double SwapDbl( double d );
long SwapLong( long l );

bool SendDouble( int sd, double d );
bool SendLong( int sd, long l );
char *ReadDouble( char *buf, double& d );
char *ReadLong( char *buf, long& l );

// Send/Recv data
void ReadHeader( int socket, char *buf );
long ReadEntireMessage( int socket, char **buf );

void WritePlayerData();

void SendSwing( Player *player );

void SendTime( int sd, struct timeb* tb );
void ReadTime( int sd, struct timeb* tb );

void getcurrenttime( struct timeb *tb );
void ReadBI();
#ifdef ENABLE_IPV6
struct addrinfo* findhostname();
#else
void findhostname( struct sockaddr_in *saddr );
#endif

void ClearSocket();
bool GetSocket();

bool WaitForClient();
bool ConnectToServer();
void ServerAdjustClock();
void ClientAdjustClock();

// For exception. 
struct NetworkError { };

#endif	// _Network_
