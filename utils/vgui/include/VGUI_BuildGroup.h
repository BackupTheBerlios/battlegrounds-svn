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


#ifndef VGUI_BUILDGROUP_H
#define VGUI_BUILDGROUP_H

#include<VGUI.h>
#include<VGUI_Dar.h>


namespace vgui
{

enum KeyCode;
enum MouseCode;
class Panel;
class Cursor;
class ChangeSignal;

class VGUIAPI BuildGroup
{
private:	
	bool      _enabled;
	int       _snapX;
	int       _snapY;
	Cursor*   _cursor_sizenwse;
	Cursor*   _cursor_sizenesw;
	Cursor*   _cursor_sizewe;
	Cursor*   _cursor_sizens;
	Cursor*   _cursor_sizeall;
	bool      _dragging;
	MouseCode _dragMouseCode;
	int       _dragStartPanelPos[2];
	int       _dragStartCursorPos[2];
	Panel*    _currentPanel;
	Dar<ChangeSignal*> _currentPanelChangeSignalDar;
	Dar<Panel*> _panelDar;
	Dar<char*>  _panelNameDar;
public:
	BuildGroup();
public:
	virtual void   setEnabled(bool state);
	virtual bool   isEnabled();
	virtual void   addCurrentPanelChangeSignal(ChangeSignal* s);
	virtual Panel* getCurrentPanel();
	virtual void   copyPropertiesToClipboard();
private:
	virtual void applySnap(Panel* panel);
	virtual void fireCurrentPanelChangeSignal();
protected:
	friend class Panel;
	virtual void    panelAdded(Panel* panel,const char* panelName);
	virtual void    cursorMoved(int x,int y,Panel* panel);
	virtual void    mousePressed(MouseCode code,Panel* panel);
	virtual void    mouseReleased(MouseCode code,Panel* panel);
	virtual void    mouseDoublePressed(MouseCode code,Panel* panel);
	virtual void    keyTyped(KeyCode code,Panel* panel);
	virtual Cursor* getCursor(Panel* panel);
};

}

#endif