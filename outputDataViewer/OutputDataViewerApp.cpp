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
