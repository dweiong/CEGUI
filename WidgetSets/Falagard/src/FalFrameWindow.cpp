/************************************************************************
    filename:   FalFrameWindow.cpp
    created:    Sat Jul 2 2005
    author:     Paul D Turner <paul@cegui.org.uk>
*************************************************************************/
/*************************************************************************
    Crazy Eddie's GUI System (http://www.cegui.org.uk)
    Copyright (C)2004 - 2005 Paul D Turner (paul@cegui.org.uk)
 
    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.
 
    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.
 
    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*************************************************************************/
#include "FalFrameWindow.h"
#include "falagard/CEGUIFalWidgetLookManager.h"
#include "falagard/CEGUIFalWidgetLookFeel.h"
#include "CEGUIWindowManager.h"
#include "elements/CEGUITitlebar.h"
#include "elements/CEGUIPushButton.h"

// Start of CEGUI namespace section
namespace CEGUI
{
    const utf8 FalagardFrameWindow::WidgetTypeName[] = "Falagard/FrameWindow";

    FalagardFrameWindow::FalagardFrameWindow(const String& type, const String& name) :
        FrameWindow(type, name)
    {
    }

    FalagardFrameWindow::~FalagardFrameWindow()
    {
    }

    void FalagardFrameWindow::populateRenderCache()
    {
        // do not render anything for the rolled-up state.
        if (d_rolledup)
            return;

        const StateImagery* imagery;

        try
        {
            // get WidgetLookFeel for the assigned look.
            const WidgetLookFeel& wlf = WidgetLookManager::getSingleton().getWidgetLook(d_lookName);
            // try and get imagery for our current state
            if (d_enabled)
                imagery = &wlf.getStateImagery((d_parent && d_parent->isActive()) ? "Active" : "Inactive");
            else
                imagery = &wlf.getStateImagery("Disabled");
        }
        catch (UnknownObjectException)
        {
            // log error so we know imagery is missing, and then quit.
            return;
        }

        // peform the rendering operation.
        imagery->render(*this);
    }

    Rect FalagardFrameWindow::getUnclippedInnerRect(void) const
    {
        if (d_rolledup)
            return Rect(0,0,0,0);

        // get WidgetLookFeel for the assigned look.
        const WidgetLookFeel& wlf = WidgetLookManager::getSingleton().getWidgetLook(d_lookName);
        return wlf.getNamedArea("ClientArea").getArea().getPixelRect(*this, getPixelRect());
    }

    Titlebar* FalagardFrameWindow::createTitlebar(const String& name) const
    {
        return static_cast<Titlebar*>(WindowManager::getSingleton().getWindow(getName() + "__auto_titlebar__"));
    }

    PushButton* FalagardFrameWindow::createCloseButton(const String& name) const
    {
        return static_cast<PushButton*>(WindowManager::getSingleton().getWindow(getName() + "__auto_closebutton__"));
    }

    void FalagardFrameWindow::layoutComponentWidgets()
    {
        // get WidgetLookFeel for the assigned look.
        try
        {
            const WidgetLookFeel& wlf = WidgetLookManager::getSingleton().getWidgetLook(d_lookName);
            wlf.layoutChildWidgets(*this);
        }
        catch (UnknownObjectException)
        {}
    }

    //////////////////////////////////////////////////////////////////////////
    /*************************************************************************

        Factory Methods

    *************************************************************************/
    //////////////////////////////////////////////////////////////////////////
    Window* FalagardFrameWindowFactory::createWindow(const String& name)
    {
        return new FalagardFrameWindow(d_type, name);
    }

    void FalagardFrameWindowFactory::destroyWindow(Window* window)
    {
        if (window->getType() == d_type)
            delete window;
    }

} // End of  CEGUI namespace section
