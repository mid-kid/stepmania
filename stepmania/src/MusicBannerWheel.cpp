#include "stdafx.h"
/*
-----------------------------------------------------------------------------
 Class: MusicBannerWheel

 Desc: See header.

 Copyright (c) 2001-2002 by the person(s) listed below.  All rights reserved.
	Andrew Livy
-----------------------------------------------------------------------------
*/

#include "MusicBannerWheel.h"
#include "RageUtil.h"
#include "GameState.h"
#include "PrefsManager.h"
#include "SongManager.h"
#include "ThemeManager.h"
#include "RageMusic.h"
#include "StyleDef.h"

#define BANNERSPACING 200
#define MAXSONGSINBUFFER 5
#define BANNERTYPE 1

MusicBannerWheel::MusicBannerWheel() 
{ 
	currentPos=0;
	scrlistPos=0;

	m_ScrollingList.UseSpriteType(BANNERTYPE);
	m_ScrollingList.SetXY( 0, 0 );
	m_ScrollingList.SetSpacing( BANNERSPACING );
	this->AddChild( &m_ScrollingList );
	
	arraySongs = SONGMAN->m_pSongs;

	// If there is no currently selected song, select one.
	if( GAMESTATE->m_pCurSong == NULL )
	{
		currentPos=0;
	}
	else // theres a song already selected (i.e. they came back from gameplay)...
	{
		// find our song and change the currentPos to wherever it may be.
		for(int i=0; i<arraySongs.size(); i++)
		{
			if( GAMESTATE->m_pCurSong == arraySongs[i])
			{
				currentPos=i;
				i=(arraySongs.size()-1); // get us out of the loop by moving i ahead to the end
			}
		}
	}

	LoadSongData();

	m_debugtext.LoadFromFont( THEME->GetPathTo("Fonts","small titles") );
	m_debugtext.SetXY( 0, -120 );
	this->AddChild(&m_debugtext);
}

/************ TODO: OPTIMIZE THIS: MUST!!!! ********************/
void MusicBannerWheel::LoadSongData()
{
	Song* pSong;
	CStringArray asGraphicPaths;
	for(int count=0; count<MAXSONGSINBUFFER; count++)
	{
		/* In essence, this element is the central one so we want the scrolling list
		to load in the song as specified by currentPos */
		if(count == scrlistPos)
		{	
			pSong = arraySongs[currentPos];
		}
		/* if it's the next element
		*/
		else if(count == scrlistPos+2 || (scrlistPos == MAXSONGSINBUFFER-2 && count == 0))
		{
			if(currentPos+2 <= arraySongs.size()-1)
				pSong = arraySongs[currentPos+2];
			else
				pSong = arraySongs[0+2];
		}
		else if(count == scrlistPos-2 || (scrlistPos == 0 && count == MAXSONGSINBUFFER-2))
		{
			if(currentPos-2 >= 0)
				pSong = arraySongs[currentPos-2];
			else
				pSong = arraySongs[arraySongs.size()-2];			
		}
		else if(count == scrlistPos+1 || (scrlistPos == MAXSONGSINBUFFER-1 && count == 0))
		{
			if(currentPos+1 <= arraySongs.size()-1)
				pSong = arraySongs[currentPos+1];
			else
				pSong = arraySongs[0];
		}
		/* if it's the previous element OR if we're at element 0..the 5th element (which will wrap to before element 0)
		will actually appear as 5 songs along and not the one immediately before it,, so pull a sneaky and make the 
		final element actually be the song before... */
		else if(count == scrlistPos-1 || (scrlistPos == 0 && count == MAXSONGSINBUFFER-1))
		{
			if(currentPos-1 >= 0)
				pSong = arraySongs[currentPos-1];
			else
				pSong = arraySongs[arraySongs.size()-1];
		}

		if( pSong == NULL ) asGraphicPaths.push_back(THEME->GetPathTo("Graphics","fallback banner"));
		else if (pSong->HasBanner()) asGraphicPaths.push_back(pSong->GetBannerPath());
		else if (PREFSMAN->m_bUseBGIfNoBanner && pSong->HasBackground() ) asGraphicPaths.push_back(pSong->GetBannerPath());
		else asGraphicPaths.push_back(THEME->GetPathTo("Graphics","fallback banner"));
	}
	m_ScrollingList.Load( asGraphicPaths );
	PlayMusicSample();
}


Song* MusicBannerWheel::GetSelectedSong()
{
	Song* pSong;
	pSong=arraySongs[currentPos];
	return(pSong);
}

void MusicBannerWheel::PlayMusicSample()
{
	Song* pSong = GetSelectedSong();
	if( pSong  &&  pSong->HasMusic() )
	{
		MUSIC->Stop();
		MUSIC->Load( pSong->GetMusicPath() );
		MUSIC->Play( true, pSong->m_fMusicSampleStartSeconds, pSong->m_fMusicSampleLengthSeconds );
	}
}

void MusicBannerWheel::BannersLeft()
{
	if(currentPos==0)
		currentPos=arraySongs.size()-1;
	else
		currentPos--;

	if(scrlistPos==0)
		scrlistPos = MAXSONGSINBUFFER-1;
	else
		scrlistPos--;

	m_ScrollingList.Unload();
	LoadSongData();
	m_debugtext.SetText(ssprintf("currentPos: %d scrlistPos: %d",currentPos,scrlistPos));
	m_ScrollingList.Left();
	ChangeNotes();
}

void MusicBannerWheel::BannersRight()
{
	if(currentPos>=arraySongs.size()-1)
		currentPos=0;
	else
		currentPos++;

	if(scrlistPos==MAXSONGSINBUFFER-1)
		scrlistPos=0;
	else
		scrlistPos++;

	m_ScrollingList.Unload();
	LoadSongData();
	m_debugtext.SetText(ssprintf("currentPos: %d scrlistPos: %d",currentPos,scrlistPos));
	m_ScrollingList.Right();
	ChangeNotes();
}

void MusicBannerWheel::ChangeNotes()
{
}


MusicBannerWheel::~MusicBannerWheel()
{
}
