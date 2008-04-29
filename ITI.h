// Copyright (c) 2004, Steven Scott (progoth@gmail.com)
//
// This file is part of iSnooze.
//
// iSnooze is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// iSnooze is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with iSnooze; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

#pragma once

#include <string>
#include <vector>

#include "t.h"
#include "iTunesCOMInterface.h"

typedef struct _ITID
{
    long sourceID;
    long playlistID;
    long trackID;
    long databaseID;
    bool operator==( const struct _ITID &o ) const
    { return ( (sourceID == o.sourceID) && (playlistID == o.playlistID) && (trackID == o.trackID) && (databaseID == o.databaseID) ); }
    bool operator!=( const struct _ITID &o ) const
    { return !( *this == o ); }
    void toBin( void *out ) const
    { ((long*)out)[0] = sourceID; ((long*)out)[1] = playlistID; ((long*)out)[2] = trackID; ((long*)out)[3] = databaseID; }
    void fromBin( const void *in )
    { sourceID = ((long*)in)[0]; playlistID = ((long*)in)[1]; trackID = ((long*)in)[2]; databaseID = ((long*)in)[3]; }
    struct _ITID( const struct _ITID &o )
    { if( this != &o ){ sourceID = o.sourceID; playlistID = o.playlistID; trackID = o.trackID; databaseID = o.databaseID; }}
    struct _ITID() : sourceID(0), playlistID(0), trackID(0), databaseID(0){}
} ITID;

/**
 * iTunes Interface Singleton
 */
class ITI
{
public:

    static void Connect()
    { ITI::instance()->connect(); }
    static void Disconnect()
    { ITI::instance()->disconnect(); }
    static void GetPlaylists( std::vector<tstring> &out, std::vector<ITID> &outid )
    { ITI::instance()->getPlaylists( out, outid ); }
    static void PlayPlaylist( const ITID &plid, const tstring &plname, bool shuffle = false )
    { ITI::instance()->playPlaylist( plid, plname, shuffle ); }
	static void SetVolume( UINT level )
	{ ITI::instance()->setVolume( level ); }
	static void ZeroVolume()
	{ ITI::instance()->zeroVolume(); }
    static void Minimize()
    { ITI::instance()->minimize(); }
    static void Stop()
    { ITI::instance()->stop(); }
    static void Pause()
    { ITI::instance()->pause(); }
    static void Play()
    { ITI::instance()->play(); }
	static void Next()
	{ ITI::instance()->next(); }

	~ITI();

private:
    ITI():m_iTunesApp(NULL){}

	static ITI *instance()
	{ if( !inst ) inst = new ITI; InitializeCriticalSection(&ITI::cs); return inst; }

    void connect();
    void disconnect();
    void getPlaylists( std::vector<tstring> &out, std::vector<ITID> &outid );
    void playPlaylist( const ITID &plid, const tstring &plname, bool shuffle );
	void setVolume( UINT level );
	void zeroVolume()
	{ m_iTunesApp->put_SoundVolume(0); }
    void minimize();
    void stop()
    { m_iTunesApp->Stop(); }
	void next()
	{ m_iTunesApp->NextTrack(); }
    void pause()
    { m_iTunesApp->Pause(); }
    void play()
    { m_iTunesApp->Play(); }

	static ITI *inst;
	static long refcount;
	static CRITICAL_SECTION cs;
    IiTunes* m_iTunesApp;
};
