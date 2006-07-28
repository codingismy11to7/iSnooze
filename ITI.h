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
    static void GetPlaylists( std::vector<tstring> &out/*, std::vector<ITID> &outid*/ )
    { ITI::instance()->getPlaylists( out/*, outid*/ ); }
    static void PlayPlaylist( const tstring &plname, bool shuffle = false )
    { ITI::instance()->playPlaylist( plname, shuffle ); }

	~ITI();

private:
    ITI():m_iTunesApp(NULL){}

	static ITI *instance()
	{ if( !m_inst ) m_inst = new ITI; return m_inst; }

    void connect();
    void disconnect();
    void getPlaylists( std::vector<tstring> &out/*, std::vector<ITID> &outid*/ );
    void playPlaylist( const tstring &plname, bool shuffle );

	static ITI *m_inst;
    IiTunes* m_iTunesApp;
};
