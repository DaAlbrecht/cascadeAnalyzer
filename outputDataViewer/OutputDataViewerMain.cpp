/***************************************************************
 * Name:      OutputDataViewerMain.cpp
 * Purpose:   Code for Application Frame
 * Author:    da ()
 * Created:   2019-07-02
 * Copyright: da ()
 * License:
 **************************************************************/

#include "OutputDataViewerMain.h"
#include <wx/msgdlg.h>
//(*InternalHeaders(OutputDataViewerFrame)
#include <wx/intl.h>
#include <wx/string.h>
//*)

//helper functions
enum wxbuildinfoformat {
    short_f, long_f };

wxString wxbuildinfo(wxbuildinfoformat format)
{
    wxString wxbuild(wxVERSION_STRING);

    if (format == long_f )
    {
#if defined(__WXMSW__)
        wxbuild << _T("-Windows");
#elif defined(__UNIX__)
        wxbuild << _T("-Linux");
#endif

#if wxUSE_UNICODE
        wxbuild << _T("-Unicode build");
#else
        wxbuild << _T("-ANSI build");
#endif // wxUSE_UNICODE
    }

    return wxbuild;
}

//(*IdInit(OutputDataViewerFrame)
const long OutputDataViewerFrame::idMenuQuit = wxNewId();
const long OutputDataViewerFrame::idMenuAbout = wxNewId();
const long OutputDataViewerFrame::ID_STATUSBAR1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(OutputDataViewerFrame,wxFrame)
    //(*EventTable(OutputDataViewerFrame)
    //*)
END_EVENT_TABLE()

OutputDataViewerFrame::OutputDataViewerFrame(wxWindow* parent,wxWindowID id)
{
    //(*Initialize(OutputDataViewerFrame)
    wxMenuItem* MenuItem2;
    wxMenuItem* MenuItem1;
    wxMenu* Menu1;
    wxMenuBar* MenuBar1;
    wxMenu* Menu2;

    Create(parent, id, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE, _T("id"));
    MenuBar1 = new wxMenuBar();
    Menu1 = new wxMenu();
    MenuItem1 = new wxMenuItem(Menu1, idMenuQuit, _("Quit\tAlt-F4"), _("Quit the application"), wxITEM_NORMAL);
    Menu1->Append(MenuItem1);
    MenuBar1->Append(Menu1, _("&File"));
    Menu2 = new wxMenu();
    MenuItem2 = new wxMenuItem(Menu2, idMenuAbout, _("About\tF1"), _("Show info about this application"), wxITEM_NORMAL);
    Menu2->Append(MenuItem2);
    MenuBar1->Append(Menu2, _("Help"));
    SetMenuBar(MenuBar1);
    StatusBar1 = new wxStatusBar(this, ID_STATUSBAR1, 0, _T("ID_STATUSBAR1"));
    int __wxStatusBarWidths_1[1] = { -1 };
    int __wxStatusBarStyles_1[1] = { wxSB_NORMAL };
    StatusBar1->SetFieldsCount(1,__wxStatusBarWidths_1);
    StatusBar1->SetStatusStyles(1,__wxStatusBarStyles_1);
    SetStatusBar(StatusBar1);

    Connect(idMenuQuit,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&OutputDataViewerFrame::OnQuit);
    Connect(idMenuAbout,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&OutputDataViewerFrame::OnAbout);
    //*)

    ListCtrl1 = new wxListCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT);
    OutputDataViewerFrame::ParseCsvFile(parent, id, ListCtrl1);
}

void OutputDataViewerFrame::ParseCsvFile(wxWindow* &parent,wxWindowID &id, wxListCtrl*  &ListCtrl1)
{
    std::vector<std::string> buf;
    std::ifstream csv;
    std::string line;
    uint8_t parsedlines = 0;
    size_t bufoffset =0;
    csv.open("outputdata.csv");
    while(std::getline(csv,line))
    {
        boost::char_separator<char> sep(";");
        boost::tokenizer< boost::char_separator<char> > tokens(line, sep);
        BOOST_FOREACH (const std::string& t, tokens)
        {
            buf.push_back(t);
        }
        OutputDataViewerFrame::CreatList( buf, parsedlines, bufoffset,  parent, id, ListCtrl1);
        ++parsedlines;
    }
}
 void OutputDataViewerFrame::AutoSizeHeader(wxListCtrl *const &ListCtrl1)
  {
    if(ListCtrl1)
    {
      for(int i = 0; i < ListCtrl1->GetColumnCount(); ++i)
      {
        ListCtrl1->SetColumnWidth(i, wxLIST_AUTOSIZE);
        const int a_width = ListCtrl1->GetColumnWidth(i);
        ListCtrl1->SetColumnWidth(i, wxLIST_AUTOSIZE_USEHEADER);
        const int h_width = ListCtrl1->GetColumnWidth(i);
        ListCtrl1->SetColumnWidth(i, (std::max)(a_width, h_width));
      }
    }
  }
void OutputDataViewerFrame::CreatList(std::vector<std::string> &buf, uint8_t &parsedlines, size_t &bufoffset, wxWindow* &parent, wxWindowID &id, wxListCtrl*  &ListCtrl1)
{
    wxListItem item;
    wxListItem col;

    if(parsedlines == 0)
    {
        for( size_t i = 0; i < buf.size(); ++i )
        {
            col.SetId(i);
            col.SetAlign(wxLIST_FORMAT_RIGHT);
            col.SetText(buf.at(i));
            col.SetWidth(400);
            ListCtrl1->InsertColumn(i, col);
            ++bufoffset;
            OutputDataViewerFrame::AutoSizeHeader(ListCtrl1);
        }
    }
    else
    {
        item.SetId(parsedlines-1);
        int colnr = 0;
        ListCtrl1->InsertItem(item);
        for( size_t i = bufoffset; i < buf.size(); ++i )
        {
            ListCtrl1->SetItem(parsedlines-1, colnr, buf.at(i));
            ++colnr;
            ++bufoffset;
        }
    }
}
OutputDataViewerFrame::~OutputDataViewerFrame()
{
    //(*Destroy(OutputDataViewerFrame)
    //*)
}

void OutputDataViewerFrame::OnQuit(wxCommandEvent& event)
{
    Close();
}

void OutputDataViewerFrame::OnAbout(wxCommandEvent& event)
{
    wxString msg = wxbuildinfo(long_f);
    wxMessageBox(msg, _("Help "));
}
