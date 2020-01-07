#ifndef OUTPUTDATAVIEWERMAIN_H
#define OUTPUTDATAVIEWERMAIN_H

//(*Headers(OutputDataViewerFrame)
#include <wx/aui/aui.h>
#include <wx/frame.h>
#include <wx/listctrl.h>
#include <wx/menu.h>
#include <wx/statusbr.h>
//*)

#include <fstream>
#include <boost/regex.hpp>
#include <boost/foreach.hpp>
#include <boost/tokenizer.hpp>
class OutputDataViewerFrame: public wxFrame
{
public:

    OutputDataViewerFrame(wxWindow* parent, wxWindowID id = -1);
    virtual ~OutputDataViewerFrame();

private:

    //(*Handlers(OutputDataViewerFrame)
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    //*)

    //(*Identifiers(OutputDataViewerFrame)
    static const long ID_LISTCTRL1;
    static const long idMenuQuit;
    static const long idMenuAbout;
    static const long ID_STATUSBAR1;
    //*)

    //(*Declarations(OutputDataViewerFrame)
    wxAuiManager* AuiManager1;
    wxListCtrl* ListCtrl1;
    wxStatusBar* StatusBar1;
    //*)

    DECLARE_EVENT_TABLE()

    void ParseCsvFile();
    void CreateList(std::vector<std::string> &buf, uint8_t &parsedlines, size_t &bufoffset);
    void AutoSizeHeader(int &colsize);
};

#endif
