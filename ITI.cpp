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

#include "StdAfx.h"
#include ".\iti.h"

ITI* ITI::inst = NULL;
long ITI::refcount = 0;
CRITICAL_SECTION ITI::cs;

ITI::~ITI()
{
}

void ITI::connect()
{
	EnterCriticalSection(&ITI::cs);
	if( !m_iTunesApp )
	{
		if( FAILED(::CoCreateInstance( CLSID_iTunesApp, NULL, CLSCTX_LOCAL_SERVER, 
			IID_IiTunes, (void**)&m_iTunesApp )) )
		{
			LeaveCriticalSection( &ITI::cs );
			throw trterror( _T("couldn't start itunes interface") );
		}
	}

	ITI::refcount++;
	LeaveCriticalSection( &ITI::cs );
}

void ITI::disconnect()
{
	EnterCriticalSection( &ITI::cs );
    if( refcount == 0 ) return;
	if( --refcount == 0 )
	{
		m_iTunesApp->Release();
		m_iTunesApp = NULL;
	}
	LeaveCriticalSection( &ITI::cs );
}

void ITI::getPlaylists( std::vector<tstring> &out, std::vector<ITID> &outids )
{
    IITSource *libSource;
    if( FAILED(m_iTunesApp->get_LibrarySource( &libSource )) )
        return;

    IITPlaylistCollection *playlists;
    if( FAILED(libSource->get_Playlists( &playlists )) )
    {
        libSource->Release();
        return;
    }

    long plsCount;
    if( FAILED(playlists->get_Count( &plsCount )) )
    {
        playlists->Release();
        libSource->Release();
        return;
    }

    IITPlaylist *pl;
    BSTR name;
    ITID id;
    for( long i = 1; i <= plsCount; i++ )
    {
        if( FAILED(playlists->get_Item( i, &pl )) )
        {
            playlists->Release();
            libSource->Release();
            return;
        }

        if( FAILED(pl->get_Name( &name )) )
        {
            pl->Release();
            playlists->Release();
            libSource->Release();
            return;
        }

        tstring t = (TCHAR*)name;
        SysFreeString( name );

        if( t != _T("Library") && t != _T("Party Shuffle") )
        {
            if( FAILED(pl->GetITObjectIDs( &id.sourceID, &id.playlistID, &id.trackID, &id.databaseID )) )
            {
                pl->Release();
                playlists->Release();
                libSource->Release();
                return;
            }

            out.push_back( t );
            outids.push_back( id );
        }

        pl->Release();
    }
      
    playlists->Release();
    libSource->Release();
}

void ITI::playPlaylist( const ITID &plid, const tstring &plname, bool shuffle )
{
    IITPlaylist *pl;
    IITObject *tmp;
	if( m_iTunesApp->GetITObjectByID( plid.sourceID, plid.playlistID, plid.trackID, plid.databaseID, &tmp ) == S_OK )
	{
		if( SUCCEEDED(tmp->QueryInterface( IID_IITPlaylist, (void**)&pl )) )
		{
			pl->put_SongRepeat(ITPlaylistRepeatModeAll);
			pl->put_Shuffle( ( (shuffle) ? -1 : 0 ) );
			m_iTunesApp->Stop();
			pl->put_Shuffle(0);
			pl->put_Shuffle(shuffle ? -1 : 0);
			pl->PlayFirstTrack();

			pl->Release();
			tmp->Release();
			return;
		}
		else
		{
			tmp->Release();
		}
	}

    
	IITSource *libSource;
    if( FAILED(m_iTunesApp->get_LibrarySource( &libSource )) )
        return;

    IITPlaylistCollection *playlists;
    if( FAILED(libSource->get_Playlists( &playlists )) )
    {
        libSource->Release();
        return;
    }

    //IITPlaylist *pl;
    BSTR tmps = SysAllocString( plname.c_str() );
    if( FAILED(playlists->get_ItemByName( tmps, &pl )) )
    {
        SysFreeString( tmps );
        playlists->Release();
        libSource->Release();
        return;
    }

    pl->put_Shuffle( ( (shuffle) ? -1 : 0 ) );
    m_iTunesApp->Stop();
    pl->PlayFirstTrack();

    pl->Release();
    SysFreeString( tmps );
    playlists->Release();
    libSource->Release();
}

void ITI::setVolume( UINT level )
{
	if( level <= 100 )
    {
        ITI::Connect(); //should already be connected, so this shouldn't hurt
		m_iTunesApp->put_SoundVolume( level );
        ITI::Disconnect();
    }
}

void ITI::minimize()
{
    IITBrowserWindow *bw;
    if( FAILED(m_iTunesApp->get_BrowserWindow( &bw )) )
        return;
    bw->put_Minimized( -1 );
    bw->Release();
}
