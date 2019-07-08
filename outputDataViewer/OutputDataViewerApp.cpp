/***************************************************************
 * Name:      OutputDataViewerApp.cpp
 * Purpose:   Code for Application Class
 * Author:    da ()
 * Created:   2019-07-02
 * Copyright: da ()
 * License:
 **************************************************************/

#include "OutputDataViewerApp.h"

//(*AppHeaders
#include "OutputDataViewerMain.h"
#include <wx/image.h>
//*)

IMPLEMENT_APP(OutputDataViewerApp);

bool OutputDataViewerApp::OnInit()
{
    //(*AppInitialize
    bool wxsOK = true;
    wxInitAllImageHandlers();
    if ( wxsOK )
    {
    	OutputDataViewerFrame* Frame = new OutputDataViewerFrame(0);
    	Frame->Show();
    	SetTopWindow(Frame);
    }
    //*)
    return wxsOK;

}
