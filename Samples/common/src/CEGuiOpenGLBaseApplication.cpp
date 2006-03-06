/************************************************************************
    filename:   CEGuiOpenGLBaseApplication.cpp
    created:    24/9/2004
    author:     Paul D Turner
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
#ifdef HAVE_CONFIG_H
#   include "config.h"
#endif

// this controls conditional compile of file for MSVC
#include "CEGUISamplesConfig.h"
#ifdef CEGUI_SAMPLES_USE_OPENGL

#include "CEGuiOpenGLBaseApplication.h"
#include "renderers/OpenGLGUIRenderer/openglrenderer.h"
#include "CEGuiSample.h"
#include "CEGUI.h"
#include "CEGUIDefaultResourceProvider.h"

#ifdef __APPLE__
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/glut.h>
#ifdef __FREEGLUT_STD_H__
#   include <GL/freeglut_ext.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include <stdexcept>
#include <stdlib.h>

#ifdef _MSC_VER
# if defined(DEBUG) || defined (_DEBUG)
#   pragma comment (lib, "OpenGLGUIRenderer_d.lib")
# else
#   pragma comment (lib, "OpenGLGUIRenderer.lib")
# endif
#endif


/*************************************************************************
    Simple key-map used to translate glut special keys to CEGUI keys.
    See: CEGuiOpenGLBaseApplication::keySpecial method.
*************************************************************************/
struct GlutKeyMapping
{
    int glutKey;
    CEGUI::uint  ceguiKey;
};

GlutKeyMapping specialKeyMap[] =
{
    {GLUT_KEY_F1, CEGUI::Key::F1},
    {GLUT_KEY_F2, CEGUI::Key::F2},
    {GLUT_KEY_F3, CEGUI::Key::F3},
    {GLUT_KEY_F4, CEGUI::Key::F4},
    {GLUT_KEY_F5, CEGUI::Key::F5},
    {GLUT_KEY_F6, CEGUI::Key::F6},
    {GLUT_KEY_F7, CEGUI::Key::F7},
    {GLUT_KEY_F8, CEGUI::Key::F8},
    {GLUT_KEY_F9, CEGUI::Key::F9},
    {GLUT_KEY_F10, CEGUI::Key::F10},
    {GLUT_KEY_F11, CEGUI::Key::F11},
    {GLUT_KEY_F12, CEGUI::Key::F12},
    {GLUT_KEY_LEFT, CEGUI::Key::ArrowLeft},
    {GLUT_KEY_UP, CEGUI::Key::ArrowUp},
    {GLUT_KEY_RIGHT, CEGUI::Key::ArrowRight},
    {GLUT_KEY_DOWN, CEGUI::Key::ArrowDown},
    {GLUT_KEY_PAGE_UP, CEGUI::Key::PageUp},
    {GLUT_KEY_PAGE_DOWN, CEGUI::Key::PageDown},
    {GLUT_KEY_HOME, CEGUI::Key::Home},
    {GLUT_KEY_END, CEGUI::Key::End},
    {GLUT_KEY_INSERT, CEGUI::Key::Insert},
    {-1, 0}
};

/*************************************************************************
    Static Data
*************************************************************************/
bool CEGuiOpenGLBaseApplication::d_quitFlag = false;
int  CEGuiOpenGLBaseApplication::d_lastFrameTime = 0;
int CEGuiOpenGLBaseApplication::d_modifiers = 0;
int CEGuiOpenGLBaseApplication::d_fps_lastTime = 0;
int CEGuiOpenGLBaseApplication::d_fps_frames = 0;
int CEGuiOpenGLBaseApplication::d_fps_value = 0;
char CEGuiOpenGLBaseApplication::d_fps_textbuff[16];

/*************************************************************************
    Constructor.
*************************************************************************/
CEGuiOpenGLBaseApplication::CEGuiOpenGLBaseApplication()
{
    // fake args for glutInit
    int argc = 1;
    char* argv = "SampleApp";

    // Do GLUT init
    glutInit(&argc, &argv);
    glutInitDisplayMode(GLUT_DEPTH|GLUT_DOUBLE|GLUT_RGBA);
    glutInitWindowSize(800, 600);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Crazy Eddie's GUI Mk-2 - Sample Application");
    glutSetCursor(GLUT_CURSOR_NONE);

    glutDisplayFunc(&CEGuiOpenGLBaseApplication::drawFrame);
    glutReshapeFunc(&CEGuiOpenGLBaseApplication::reshape);
    glutMotionFunc(&CEGuiOpenGLBaseApplication::mouseMotion);
    glutPassiveMotionFunc(&CEGuiOpenGLBaseApplication::mouseMotion);
    glutMouseFunc(&CEGuiOpenGLBaseApplication::mouseButton);
    glutKeyboardFunc(&CEGuiOpenGLBaseApplication::keyChar);
    glutSpecialFunc(&CEGuiOpenGLBaseApplication::keySpecial);

    #ifdef __FREEGLUT_EXT_H__
        glutMouseWheelFunc(&CEGuiOpenGLBaseApplication::handleMouseWheel_freeglut);
    #endif

    // Set the clear color
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    d_renderer = new CEGUI::OpenGLRenderer(1024);
    new CEGUI::System(d_renderer);

    // initialise the required dirs for the DefaultResourceProvider
    CEGUI::DefaultResourceProvider* rp = static_cast<CEGUI::DefaultResourceProvider*>
        (CEGUI::System::getSingleton().getResourceProvider());

    rp->setResourceGroupDirectory("schemes", "../datafiles/schemes/");
    rp->setResourceGroupDirectory("imagesets", "../datafiles/imagesets/");
    rp->setResourceGroupDirectory("fonts", "../datafiles/fonts/");
    rp->setResourceGroupDirectory("layouts", "../datafiles/layouts/");
    rp->setResourceGroupDirectory("looknfeels", "../datafiles/looknfeel/");
    rp->setResourceGroupDirectory("lua_scripts", "../datafiles/lua_scripts/");
#if defined(CEGUI_WITH_XERCES) && (CEGUI_DEFAULT_XMLPARSER == XercesParser)
    rp->setResourceGroupDirectory("schemas", "../../XMLRefSchema/");
#endif
}


/*************************************************************************
    Destructor.
*************************************************************************/
CEGuiOpenGLBaseApplication::~CEGuiOpenGLBaseApplication()
{
    delete CEGUI::System::getSingletonPtr();
    delete d_renderer;
}


/*************************************************************************
    Start the base application
*************************************************************************/
bool CEGuiOpenGLBaseApplication::execute(CEGuiSample* sampleApp)
{
    sampleApp->initialiseSample();

    // set starting time
    d_fps_lastTime = d_lastFrameTime = glutGet(GLUT_ELAPSED_TIME);

    glutMainLoop();

    return true;
}


/*************************************************************************
    Performs any required cleanup of the base application system.
*************************************************************************/
void CEGuiOpenGLBaseApplication::cleanup()
{
    // nothing to do here.
}

/*************************************************************************
    Set whether the app should quit
*************************************************************************/
void CEGuiOpenGLBaseApplication::setQuitting(bool quit)
{
    d_quitFlag = quit;
}

/*************************************************************************
    Is this app quitting
*************************************************************************/
bool CEGuiOpenGLBaseApplication::isQuitting() const
{
    return d_quitFlag;
}

/*************************************************************************
    Does whatever is required in one single frame
*************************************************************************/
void CEGuiOpenGLBaseApplication::drawFrame(void)
{
    CEGUI::System& guiSystem = CEGUI::System::getSingleton();
    // do time based updates
    int thisTime = glutGet(GLUT_ELAPSED_TIME);
    float elapsed = static_cast<float>(thisTime - d_lastFrameTime);
    d_lastFrameTime = thisTime;
    // inject the time pulse
    guiSystem.injectTimePulse(elapsed / 1000.0f);
    // update fps fields
    doFPSUpdate();

    // do rendering for this frame.
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0.0, 0.0, 12, 0.0, 0.0, -100, 0.0, 1.0, 0.0);

    guiSystem.renderGUI();

    // render FPS:
    const CEGUI::Font* fnt = guiSystem.getDefaultFont();
    if (fnt)
    {
        guiSystem.getRenderer()->setQueueingEnabled(false);
        fnt->drawText(d_fps_textbuff, CEGUI::Vector3(0, 0, 0), guiSystem.getRenderer()->getRect());
    }

    glFlush();
    glutPostRedisplay();
    glutSwapBuffers();

    // here we check the 'quitting' state and cleanup as required.
    // this is probably not the best way to do this, but since we're
    // using glut, and glutMainLoop can never return, we need some
    // way of checking when to exit.  And this is it...
    if (d_quitFlag)
    {
        // cleanup cegui system
        CEGUI::Renderer* renderer = guiSystem.getRenderer();
        delete CEGUI::System::getSingletonPtr();
        delete renderer;

        // exit
        exit(0);
    }
}

void CEGuiOpenGLBaseApplication::reshape(int w, int h)
{
    glViewport (0, 0, (GLsizei) w, (GLsizei) h);
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
    gluPerspective(60.0, (GLfloat) w/(GLfloat) h, 1.0, 50.0);
    glMatrixMode(GL_MODELVIEW);
    CEGUI::OpenGLRenderer* renderer = (CEGUI::OpenGLRenderer*)CEGUI::System::getSingleton().getRenderer();
    renderer->setDisplaySize(CEGUI::Size((float)w,(float)h));
}

void CEGuiOpenGLBaseApplication::mouseMotion(int x, int y)
{
    CEGUI::System::getSingleton().injectMousePosition(x, y);
}

void CEGuiOpenGLBaseApplication::mouseButton(int button, int state, int x, int y)
{
    switch(button)
    {
    case  GLUT_LEFT_BUTTON:
        if (state == GLUT_UP)
        {
            CEGUI::System::getSingleton().injectMouseButtonUp(CEGUI::LeftButton);
        }
        else
        {
            CEGUI::System::getSingleton().injectMouseButtonDown(CEGUI::LeftButton);

        }
        break;

    case GLUT_RIGHT_BUTTON:
        if (state == GLUT_UP)
        {
            CEGUI::System::getSingleton().injectMouseButtonUp(CEGUI::RightButton);
        }
        else
        {
            CEGUI::System::getSingleton().injectMouseButtonDown(CEGUI::RightButton);
        }
        break;

    case GLUT_MIDDLE_BUTTON:
        if (state == GLUT_UP)
        {
            CEGUI::System::getSingleton().injectMouseButtonDown(CEGUI::MiddleButton);
        }
        else
        {
            CEGUI::System::getSingleton().injectMouseButtonDown(CEGUI::MiddleButton);
        }
        break;
    }

}

void CEGuiOpenGLBaseApplication::keyChar(unsigned char key, int x, int y)
{
    handleModifierKeys();

    // extract some keys may be handled via key code and generate those too
    switch (key)
    {
    case 0x08:  // backspace
        CEGUI::System::getSingleton().injectKeyDown(CEGUI::Key::Backspace);
        break;
    case 0x7F:  // delete
        CEGUI::System::getSingleton().injectKeyDown(CEGUI::Key::Delete);
        break;
    case 0x1B:  // Escape
        d_quitFlag = true;
        break;
    case 0x0D:  // CR (Return)
        CEGUI::System::getSingleton().injectKeyDown(CEGUI::Key::Return);
        break;
    default:
        // inject Character code
        CEGUI::System::getSingleton().injectChar(static_cast<CEGUI::utf32>(key));
        break;
    }

}

void CEGuiOpenGLBaseApplication::keySpecial(int key, int x, int y)
{
    handleModifierKeys();

    GlutKeyMapping* mapping = specialKeyMap;

    while (mapping->glutKey != -1)
    {
        if (mapping->glutKey == key)
        {
            CEGUI::System::getSingleton().injectKeyDown(mapping->ceguiKey);
            return;
        }

        ++mapping;
    }
}

void CEGuiOpenGLBaseApplication::handleModifierKeys(void)
{
    int mods = glutGetModifiers();

    CEGUI::System& cesys = CEGUI::System::getSingleton();

    bool shift = mods&GLUT_ACTIVE_SHIFT;
    bool ctrl  = mods&GLUT_ACTIVE_CTRL;
    bool alt   = mods&GLUT_ACTIVE_ALT;

    // shift
    if (shift != bool(d_modifiers&GLUT_ACTIVE_SHIFT))
    {
        CEGUI::Key::Scan sc = CEGUI::Key::LeftShift;
        if (shift)
        {
            cesys.injectKeyDown(sc);
        }
        else
        {
            cesys.injectKeyUp(sc);
        }
    }

    // control
    if (ctrl != bool(d_modifiers&GLUT_ACTIVE_CTRL))
    {
        CEGUI::Key::Scan sc = CEGUI::Key::LeftControl;
        if (shift)
        {
            cesys.injectKeyDown(sc);
        }
        else
        {
            cesys.injectKeyUp(sc);
        }
    }

    // alt
    if (alt != bool(d_modifiers&GLUT_ACTIVE_ALT))
    {
        CEGUI::Key::Scan sc = CEGUI::Key::LeftAlt;
        if (shift)
        {
            cesys.injectKeyDown(sc);
        }
        else
        {
            cesys.injectKeyUp(sc);
        }
    }

    d_modifiers = mods;
}

void CEGuiOpenGLBaseApplication::doFPSUpdate()
{
    // another frame
    ++d_fps_frames;

    // has at least a second passed since we last updated the text?
    if (d_lastFrameTime - d_fps_lastTime >= 1000)
    {
        // update FPS text to output
        sprintf(d_fps_textbuff , "FPS: %d", d_fps_frames);
        // reset counter
        d_fps_frames    = 0;
        // update timer
        d_fps_lastTime  = d_lastFrameTime;
    }
}

// FreeGLUT supports wheel events
void CEGuiOpenGLBaseApplication::handleMouseWheel_freeglut(int wheel, int dir, int x, int y)
{
    if (wheel == 0)
    {
        CEGUI::System::getSingleton().injectMouseWheelChange((float)dir);
    }
}

#endif

