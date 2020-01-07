#include "OutputDataViewerMain.h"
#include <wx/msgdlg.h>

//(*InternalHeaders(OutputDataViewerFrame)
#include <wx/intl.h>
#include <wx/string.h>
//*)

//(*IdInit(OutputDataViewerFrame)
const long OutputDataViewerFrame::idMenuQuit = wxNewId();
const long OutputDataViewerFrame::idMenuAbout = wxNewId();
const long OutputDataViewerFrame::ID_STATUSBAR1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(OutputDataViewerFrame,wxFrame)
    //(*EventTable(OutputDataViewerFrame)
    //*)
END_EVENT_TABLE()

OutputDataViewerFrame::OutputDataViewerFrame(wxWindow* parent, wxWindowID id)
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
    ParseCsvFile();
}

void OutputDataViewerFrame::ParseCsvFile()
{
    std::vector<std::string> buf;
    std::ifstream csv;
    std::string line;
    uint8_t parsedlines = 0;
    size_t bufoffset = 0;

    csv.open("D:\\cascadeAnalayzer\\outputDataViewer\\outputdata.csv"); /// ???
    if(!csv.is_open())
    {
        wxMessageBox("file doesn't exist");
        Close();
    }
    else
    {
        while(std::getline(csv, line))
        {
            boost::char_separator<char> sep(";");
            boost::tokenizer< boost::char_separator<char> > tokens(line, sep);
            BOOST_FOREACH (const std::string& t, tokens)
            {
                buf.push_back(t);
            }
            CreateList(buf, parsedlines, bufoffset);
            ++parsedlines;
        }
    }
}

void OutputDataViewerFrame::AutoSizeHeader(int &colsize)
{
    if(ListCtrl1)
    {
        for(int i = 0; i < ListCtrl1->GetColumnCount(); ++i)
        {
            ListCtrl1->SetColumnWidth(i, wxLIST_AUTOSIZE);
            const int a_width = colsize;
            ListCtrl1->SetColumnWidth(i, wxLIST_AUTOSIZE_USEHEADER);
            const int h_width = colsize;
            ListCtrl1->SetColumnWidth(i, (std::max)(a_width, h_width));
        }
    }
}

void OutputDataViewerFrame::CreateList(std::vector<std::string> &buf, uint8_t &parsedlines, size_t &bufoffset)
{
    wxListItem item;
    wxListItem col;
    int colsize;
    if(parsedlines == 0)
    {
        wxListItem colsizer;
        colsizer.SetId(0);
        colsizer.SetAlign(wxLIST_FORMAT_RIGHT);
        colsizer.SetText(buf.back());
        colsizer.SetWidth(80);
        ListCtrl1->InsertColumn(0, col);
        colsize = ListCtrl1->GetColumnWidth(0);

        for(size_t i = 0; i < buf.size(); ++i)
        {
            col.SetId(i);
            col.SetAlign(wxLIST_FORMAT_RIGHT);
            col.SetText(buf.at(i));
            col.SetWidth(400);
            ListCtrl1->InsertColumn(i, col);
            ++bufoffset;
            AutoSizeHeader(colsize);
        }
    }
    else
    {
        item.SetId(parsedlines-1);
        int colnr = 0;
        ListCtrl1->InsertItem(item);
        for(size_t i = bufoffset; i < buf.size(); ++i)
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
    wxMessageBox(_("OutputDataViewerFrame"), _("Help "));
}
