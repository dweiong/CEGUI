/***********************************************************************
created:    29/5/2012
author:     Lukas E Meindl
*************************************************************************/
/***************************************************************************
*   Copyright (C) 2004 - 2012 Paul D Turner & The CEGUI Development Team
*
*   Permission is hereby granted, free of charge, to any person obtaining
*   a copy of this software and associated documentation files (the
*   "Software"), to deal in the Software without restriction, including
*   without limitation the rights to use, copy, modify, merge, publish,
*   distribute, sublicense, and/or sell copies of the Software, and to
*   permit persons to whom the Software is furnished to do so, subject to
*   the following conditions:
*
*   The above copyright notice and this permission notice shall be
*   included in all copies or substantial portions of the Software.
*
*   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
*   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
*   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
*   IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
*   OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
*   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
*   OTHER DEALINGS IN THE SOFTWARE.
***************************************************************************/
#ifndef _Sample_h_
#define _Sample_h_

#include "CEGUI/String.h"

// This header serves as a base for all samples and is needed inside the
// SamplesFramework as interface for Samples that will be loaded.
#include "CEGUI/ForwardRefs.h"

enum SampleType
{
    ST_Module,
    ST_Python,
    ST_Lua,

    ST_Count
};

class Sample
{
public:
    Sample() : d_inputAggregator(0) {}

    virtual bool initialise(CEGUI::GUIContext* guiContext) = 0;
    virtual void deinitialise() = 0;

    virtual void onEnteringSample() {}
    virtual void update(float timeSinceLastUpdate) {}

    const CEGUI::String& getUsedFilesString() {return d_usedFiles;}
    //! Get an explicit instance of input aggregator to be used with this sample
    CEGUI::InputAggregator* getInputAggregator() { return d_inputAggregator; }

    CEGUI::String getName() const { return d_name; }
    CEGUI::String getCredits() const { return d_credits; }
    CEGUI::String getSummary() const { return d_summary; }
    CEGUI::String getDescription() const { return d_description; }
    SampleType getType() const { return d_type; }

protected:
    CEGUI::String d_usedFiles;
    CEGUI::InputAggregator* d_inputAggregator;

    CEGUI::String d_name;
    CEGUI::String d_credits;
    CEGUI::String d_summary;
    CEGUI::String d_description;
    SampleType d_type;
};

#endif