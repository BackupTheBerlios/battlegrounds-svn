/*
    Copyright 2001 to 2004. The Battle Grounds Team and Contributors

    This file is part of the Battle Grounds Modification for Half-Life.

    The Battle Grounds Modification for Half-Life is free software;
    you can redistribute it and/or modify it under the terms of the
    GNU Lesser General Public License as published by the Free
    Software Foundation; either version 2.1 of the License, or
    (at your option) any later version.

    The Battle Grounds Modification for Half-Life is distributed in
    the hope that it will be useful, but WITHOUT ANY WARRANTY; without
    even the implied warranty of MERCHANTABILITY or FITNESS FOR A
    PARTICULAR PURPOSE.  See the GNU Lesser General Public License
    for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with The Battle Grounds Modification for Half-Life;
    if not, write to the Free Software Foundation, Inc., 59 Temple Place,
    Suite 330, Boston, MA  02111-1307  USA

    You must obey the GNU General Public License in all respects for
    all of the code used other than code  distributed with the Half-Life
    SDK developed by Valve.  If you modify this file, you may extend this
    exception to your version of the file, but you are not obligated to do so.
    If you do not wish to do so, delete this exception statement from your
    version.
*/

//This file was created on the 4/8/2k+2 at 16:51 by Ben Banfield for the Battle Grounds modification of Half-Life
//Its purpose is to define the class which will display the status of all points on the hud

#include "STDIO.H"
#include "STDLIB.H"

#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"
#include <string.h>

#include "vgui_int.h"
#include "vgui_TeamFortressViewport.h"
#include "vgui_ServerBrowser.h"

cvar_t *g_ShowPoints;

DECLARE_MESSAGE(m_Points, Points)

int CHudPoints::Init()
{
	HOOK_MESSAGE( Points );

	gHUD.AddHudElem(this);

	for(int i = 0; i < MAX_POINTS; i++)
	{
		if(m_aPoints[i])
			delete m_aPoints[i];

		m_aPoints[i] = NULL;
	}

	m_bInvolvedInCapture = false;

	return 1;
}

int CHudPoints::VidInit()
{
	// text
	for(int i = 0; i < MAX_POINTS; i++)
	{
		if(m_aPoints[i] != NULL)
			delete m_aPoints[i];

		m_aPoints[i] = NULL;
	}

	// icons
	m_HUD_IconWhite = gHUD.GetSpriteIndex( "point_white" );
	m_HUD_IconRed = gHUD.GetSpriteIndex( "point_red" );
	m_HUD_IconBlue = gHUD.GetSpriteIndex( "point_blue" );

	m_hIconWhite = gHUD.GetSprite( m_HUD_IconWhite );
	m_hIconRed = gHUD.GetSprite( m_HUD_IconRed );
	m_hIconBlue = gHUD.GetSprite( m_HUD_IconBlue );

	m_SpriteAreaIconWhite = &gHUD.GetSpriteRect( m_HUD_IconWhite );
	m_SpriteAreaIconRed = &gHUD.GetSpriteRect( m_HUD_IconRed );
	m_SpriteAreaIconBlue = &gHUD.GetSpriteRect( m_HUD_IconBlue );
	
	m_HUD_Clock = gHUD.GetSpriteIndex( "capture_clock" );
	m_HUD_FillUp = gHUD.GetSpriteIndex( "capture_fillup" );

	m_hClock = gHUD.GetSprite( m_HUD_Clock );
	m_hFillUp = gHUD.GetSprite( m_HUD_FillUp );

	m_SpriteAreaClock = &gHUD.GetSpriteRect( m_HUD_Clock );
	m_SpriteAreaFillUp = &gHUD.GetSpriteRect( m_HUD_FillUp );

	return 1;
}

void CHudPoints::ResetAllPoints()
{
	int i = 0;
	for(; i < MAX_POINTS; i++)
	{
		if(m_aPoints[i])
			delete m_aPoints[i];

		m_aPoints[i] = NULL;
	}
	m_bInvolvedInCapture = false;
}

int CHudPoints::MsgFunc_Points(const char *pszName, int iSize, void *pbuf)
{
	int iIndex = 0;

	// point variables which are specific to certain types of points
	int iTimeNeeded = 0;
	int iPlayersInPoint = 0;
	int iPlayersNeeded = 0;
	int iHealthStarted = 0;
	int iHealth = 0;

	BEGIN_READ(pbuf, iSize);

	int iID	= READ_SHORT();

	if(iID == -1)
	{
		ResetAllPoints();
		return 1;
	}

	int iPosition = READ_SHORT();

	if(iPosition != UNDEFINED)
		iIndex = iPosition;
	else
		iIndex = iID;

	int iStatus = READ_SHORT();
	char *name = READ_STRING(sizeof(m_aPoints[0]->name)-1);
	int iType = READ_SHORT();
	int iTeamAllowed = READ_SHORT();
	int iTeamInvolved = READ_SHORT();

	if(iType == CAPTURE_POINT) 
	{
		iTimeNeeded = READ_SHORT();
		iPlayersInPoint = READ_SHORT();
		iPlayersNeeded = READ_SHORT();
	}
	else if(iType == DESTRUCTION_POINT)
	{
		iHealthStarted = READ_SHORT();
		iHealth = READ_SHORT();
	}

	if(m_aPoints[iIndex])
	{
		_snprintf(m_aPoints[iIndex]->name, sizeof(m_aPoints[iID]->name)-1, "%s", name);
		m_aPoints[iIndex]->name[sizeof(m_aPoints[iID]->name)-1] = '\0';

		m_aPoints[iIndex]->iID = iID;
		m_aPoints[iIndex]->iType = iType;
		m_aPoints[iIndex]->iStatus = iStatus;
		m_aPoints[iIndex]->iTeamAllowed = iTeamAllowed;
		m_aPoints[iIndex]->iTeamInvolved = iTeamInvolved;

		m_aPoints[iIndex]->iTimeNeededForCapture = iTimeNeeded;
		m_aPoints[iIndex]->iPlayersInPoint = iPlayersInPoint;
		m_aPoints[iIndex]->iPlayersNeeded = iPlayersNeeded;

		m_aPoints[iIndex]->iHealthStarted = iHealthStarted;
		m_aPoints[iIndex]->iHealth = iHealth;

		if(iType == CAPTURE_POINT && iStatus == FREE) {
			m_aPoints[iIndex]->iTimeCaptureStarted = 0;
		}
	} else {
		// -create a new point if the point with this id doesn't exist
		m_aPoints[iIndex] = new point_s;

		strncpy(m_aPoints[iIndex]->name, name, (256-2));
		m_aPoints[iIndex]->iID = iID;
		m_aPoints[iIndex]->iType = iType;
		m_aPoints[iIndex]->iStatus = iStatus;
		m_aPoints[iIndex]->iTeamAllowed = iTeamAllowed;
		m_aPoints[iIndex]->iTeamInvolved = iTeamInvolved;

		m_aPoints[iIndex]->iTimeNeededForCapture = iTimeNeeded;
		m_aPoints[iIndex]->iTimeAdded = 0;
		m_aPoints[iIndex]->iTimeCaptureStarted = 0;
		m_aPoints[iIndex]->iPlayersInPoint = iPlayersInPoint;
		m_aPoints[iIndex]->iPlayersNeeded = iPlayersNeeded;

		m_aPoints[iIndex]->iHealthStarted = iHealthStarted;
		m_aPoints[iIndex]->iHealth = iHealth;
	}

	m_iFlags |= HUD_ACTIVE;

	return 1;
}

int CHudPoints::Draw( float flTime )
{
	if ( gHUD.m_iHideHUDDisplay & (HIDEHUD_ALL) )
		return 1;

	if( g_ShowPoints->value < 1)
		return 1;

	const int x_max = 512;
	const int EITHER_TEAM = UNDEFINED;

	int iBritishTargets = 0;
	int iAmericanTargets = 0;
	int iEitherTargets = 0;

	int i = 0;
	for(; i < MAX_POINTS; i++)
	{
		if(m_aPoints[i])
		{
			point_s *pPoint = m_aPoints[i];
			if(pPoint->iTeamAllowed == BRITS)
				iBritishTargets++;

			else if(pPoint->iTeamAllowed == AMERS)
				iAmericanTargets++;

			else if(pPoint->iTeamAllowed == EITHER_TEAM)
				iEitherTargets++;

			else {}

			// add in the time this point was received if none has been set
			if(pPoint->iTimeAdded == 0)
				pPoint->iTimeAdded = flTime;

			// clean up points from previous maps as we can't have points timestamped with times that haven't happened yet
			if(pPoint->iTimeAdded > flTime)
			{
				delete pPoint;
				continue;
			}

			// add in the capture start time
			if(pPoint->iType == CAPTURE_POINT && pPoint->iTimeCaptureStarted == 0 && pPoint->iStatus == CAPTURING)
				pPoint->iTimeCaptureStarted = flTime;

			if(pPoint->iType == CAPTURE_POINT && pPoint->iStatus == FREE)
				pPoint->iTimeCaptureStarted = 0;
		}
	}

	int x = 5, y = 10, r = 0, g = 0, b = 0;

	if(g_iUser1)
		y += YRES(30);

	if(gEngfuncs.IsSpectateOnly())
		y += YRES(15);

	if(iEitherTargets > 0)
	{
		UnpackRGB(r, g, b, RGB_WHITEISH);
		gHUD.DrawHudString(x, y, x_max, "Either Team:", r, g, b);
		y += 20;

		for(i = 0; i < MAX_POINTS; i++)
		{
			if(m_aPoints[i])
			{
				point_s *pPoint = m_aPoints[i];
				if(pPoint->iTeamAllowed == EITHER_TEAM)
					DrawPoint(pPoint, flTime, x, y);
			}
		}
	}
	if(iAmericanTargets > 0)
	{
		UnpackRGB(r, g, b, RGB_BLUEISH);
		gHUD.DrawHudString(x, y, x_max, "American only Targets:", r, g, b);
		y += 20;

		for(i = 0; i < MAX_POINTS; i++)
		{
			if(m_aPoints[i])
			{
				point_s *pPoint = m_aPoints[i];
				if(pPoint->iTeamAllowed == AMERS)
					DrawPoint(pPoint, flTime, x, y);
			}
		}
	}
	if(iBritishTargets > 0)
	{
		UnpackRGB(r, g, b, RGB_REDISH);
		gHUD.DrawHudString(x, y, x_max, "British only Targets:", r, g, b);
		y += 20;

		for(i = 0; i < MAX_POINTS; i++)
		{
			if(m_aPoints[i])
			{
				point_s *pPoint = m_aPoints[i];
				if(pPoint->iTeamAllowed == BRITS)
					DrawPoint(pPoint, flTime, x, y);
			}
		}
	}

	// capture clock

	if(m_bInvolvedInCapture)
	{
		int r = 0; int g = 0; int b = 0;
		int y_length, x_length;
		int x,y;
		int frame = 0;
		float flCapturedRatio = 0.0f;

		// fill up sprite
		y_length = gHUD.GetSpriteRect(m_HUD_FillUp).bottom - gHUD.GetSpriteRect(m_HUD_FillUp).top;
		x_length = gHUD.GetSpriteRect(m_HUD_FillUp).right - gHUD.GetSpriteRect(m_HUD_FillUp).left;	
		
		x = ScreenWidth / 2 - x_length / 2;
		y = 50;
		
		point_s *pPoint = m_aPoints[ReturnIndexById(m_iInvolvedInCaptureId)];
		
		flCapturedRatio = 1.0f - (float)(((float)pPoint->iTimeNeededForCapture - (flTime - (float)pPoint->iTimeCaptureStarted)) / (float)pPoint->iTimeNeededForCapture);
		
		if (flCapturedRatio > 1)
			flCapturedRatio = 1;
		if (flCapturedRatio < 0)
			flCapturedRatio = 0;

		frame = flCapturedRatio * 24;

		UnpackRGB(r, g, b, RGB_WHITEISH);

		if(pPoint->iTeamInvolved == BRITS)
		{
			r = 201;
			g = 0;
			b = 0;
		}
		else if(pPoint->iTeamInvolved == AMERS)
		{
			r = 0;
			g = 19;
			b = 201;
		}

		SPR_Set(m_hFillUp, r, g, b);
		SPR_DrawHoles(frame, x, y, m_SpriteAreaFillUp);

		// clock
		y_length = gHUD.GetSpriteRect(m_HUD_Clock).bottom - gHUD.GetSpriteRect(m_HUD_Clock).top;
		x_length = gHUD.GetSpriteRect(m_HUD_Clock).right - gHUD.GetSpriteRect(m_HUD_Clock).left;	
		
		x = ScreenWidth / 2 - x_length / 2;
		y = 3;

		r = 255;
		g = 255;
		b = 255;

		SPR_Set(m_hClock, r, g, b);
		SPR_DrawHoles(0, x, y, m_SpriteAreaClock);
	}

	return 1;
}

void CHudPoints::DrawPoint(point_s *pPoint, float flTime, int &x, int &y) 
{
	int r = 0; int g = 0; int b = 0;
	int y_length, x_length;
	int frame = 0;
	int iTimeTillCapture = 0;
	float flCapturedRatio = 0.0f;
	char name[512];

	const int EITHER_TEAM = UNDEFINED;
	const int x_max = 512;

	if(pPoint->iType == CAPTURE_POINT)
	{
		
		char szTime[32];
		if(pPoint->iStatus == CAPTURING)
		{
			iTimeTillCapture = (pPoint->iTimeNeededForCapture - (int)(flTime - pPoint->iTimeCaptureStarted));
			
			if (pPoint->iTimeNeededForCapture > 0.01f)
				flCapturedRatio = 1 - ((pPoint->iTimeNeededForCapture - (flTime - pPoint->iTimeCaptureStarted)) / (float)pPoint->iTimeNeededForCapture);

			sprintf(szTime, " - Time: %i\n\0", iTimeTillCapture);
		}

		else if(pPoint->iStatus == HELD)
		{
			pPoint->iTimeCaptureStarted = 0;
			sprintf(szTime, "\n\0");
		}

		else 
		{
			pPoint->iTimeCaptureStarted = 0;
			sprintf(szTime, "\n\0");
		}

		sprintf(name, "%s: %s - Players: (%i/%i)%s", 
			pPoint->name,
			szPointStatus[pPoint->iType][pPoint->iStatus], 
			pPoint->iPlayersInPoint,
			pPoint->iPlayersNeeded,
			szTime);
	}

	else if(pPoint->iType == DESTRUCTION_POINT)
	{
		if(pPoint->iStatus == DESTROYED)
		{
			sprintf(name, "%s: %s", 
				pPoint->name, 
				szPointStatus[pPoint->iType][(pPoint->iStatus - 10)]);
		}
		else
		{
			sprintf(name, "%s: %s (%i/%i)", 
				pPoint->name, 
				szPointStatus[pPoint->iType][(pPoint->iStatus - 10)],
				pPoint->iHealth,
				pPoint->iHealthStarted);
		}
	}

	if (iTimeTillCapture > 0)
		frame = 1;
	else
		frame = 0;

	float blinkspeed = 0.5f;
	int exponentialness = 8;

	if(pPoint->iTeamInvolved == EITHER_TEAM)
	{
		// icon white
		UnpackRGB(r, g, b, RGB_WHITEISH);
				
		// make it blink if it is being captured
		if (flCapturedRatio > 0.01f)
		{
			blinkspeed = blinkspeed + (blinkspeed / exponentialness / (1 - flCapturedRatio));
			r = 64*(cos(flTime*blinkspeed))+128;
			g = 64*(cos(flTime*blinkspeed))+128;
			b = 64*(cos(flTime*blinkspeed))+128;
		}

		SPR_Set(m_hIconWhite, r, g, b);
		y_length = gHUD.GetSpriteRect(m_HUD_IconWhite).bottom - gHUD.GetSpriteRect(m_HUD_IconWhite).top;
		x_length = gHUD.GetSpriteRect(m_HUD_IconWhite).right - gHUD.GetSpriteRect(m_HUD_IconWhite).left;
		SPR_DrawHoles(frame, x, y, m_SpriteAreaIconWhite);

		// set text color
		UnpackRGB(r, g, b, RGB_WHITEISH);
	}
	else if(pPoint->iTeamInvolved == BRITS)
	{
		// icon red
		UnpackRGB(r, g, b, RGB_WHITEISH);
		
		// make it blink if it is being captured
		if (flCapturedRatio > 0.01f)
		{
			blinkspeed = blinkspeed + (blinkspeed / exponentialness / (1 - flCapturedRatio));
			r = 64*(cos(flTime*blinkspeed))+128;
			g = 64*(cos(flTime*blinkspeed))+128;
			b = 64*(cos(flTime*blinkspeed))+128;
		}

		SPR_Set(m_hIconRed, r, g, b);
		y_length = gHUD.GetSpriteRect(m_HUD_IconRed).bottom - gHUD.GetSpriteRect(m_HUD_IconRed).top;
		x_length = gHUD.GetSpriteRect(m_HUD_IconRed).right - gHUD.GetSpriteRect(m_HUD_IconRed).left;
		SPR_DrawHoles(frame, x, y, m_SpriteAreaIconRed);
		
		// set text color
		UnpackRGB(r, g, b, RGB_REDISH);
	}
	else if(pPoint->iTeamInvolved == AMERS)
	{
		// icon blue
		UnpackRGB(r, g, b, RGB_WHITEISH);
						
		// make it blink if it is being captured
		if (flCapturedRatio > 0.01f)
		{
			blinkspeed = blinkspeed + (blinkspeed / exponentialness / (1 - flCapturedRatio));
			r = 64*(cos(flTime*blinkspeed))+128;
			g = 64*(cos(flTime*blinkspeed))+128;
			b = 64*(cos(flTime*blinkspeed))+128;
		}
		
		SPR_Set(m_hIconBlue, r, g, b);
		y_length = gHUD.GetSpriteRect(m_HUD_IconBlue).bottom - gHUD.GetSpriteRect(m_HUD_IconBlue).top;
		x_length = gHUD.GetSpriteRect(m_HUD_IconBlue).right - gHUD.GetSpriteRect(m_HUD_IconBlue).left;
		SPR_DrawHoles(frame, x, y, m_SpriteAreaIconBlue);

		// set text color
		UnpackRGB(r, g, b, RGB_BLUEISH);
	}
	else{}
	
	int spacing = 3;
	
	gHUD.DrawHudString((x + x_length + spacing), y, x_max, (char*)name, r, g, b);

	y += 23;
}
char *CHudPoints::ReturnNameById( int iId )
{
	int i = 0;
	for( i = 0; i < MAX_POINTS; i++ )
	{
		if(m_aPoints[i] && (m_aPoints[i]->iID == iId))
		{
			return m_aPoints[i]->name;
		}
	}
	return NULL;
}

void CHudPoints::StartClock(int iId)
{
	m_bInvolvedInCapture = true;
	m_iInvolvedInCaptureId = iId;
}

void CHudPoints::ClearClock(int iId)
{
	m_bInvolvedInCapture = false;
}

int CHudPoints::ReturnIndexById( int iId )
{
	for(int i = 1; i < MAX_POINTS; i++ )
	{
		if(m_aPoints[i] && (m_aPoints[i]->iID == iId))
		{
			return i;
		}
	}
	return NULL;
}
