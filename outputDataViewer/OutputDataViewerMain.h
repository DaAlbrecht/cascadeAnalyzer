/***************************************************************
 * Name:      OutputDataViewerMain.h
 * Purpose:   Defines Application Frame
 * Author:    da ()
 * Created:   2019-07-02
 * Copyright: da ()
 * License:
 **************************************************************/

#ifndef OUTPUTDATAVIEWERMAIN_H
#define OUTPUTDATAVIEWERMAIN_H

//(*Headers(OutputDataViewerFrame)
#include <wx/listctrl.h>
#include <wx/menu.h>
#include <wx/aui/aui.h>
#include <wx/frame.h>
#include <wx/statusbr.h>
//*)
#include <fstream>
#include <boost/regex.hpp>
#include <boost/foreach.hpp>
#include <boost/tokenizer.hpp>
class OutputDataViewerFrame: public wxFrame
{
    public:

        OutputDataViewerFrame(wxWindow* parent,wxWindowID id = -1);
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

        void ParseCsvFile(wxWindow* &parent,wxWindowID &id, wxListCtrl*&ListCtrl1);
        void CreatList( std::vector<std::string> &buf, uint8_t &parsedlines, size_t &bufoffset, wxWindow* &parent, wxWindowID &id, wxListCtrl*  &ListCtrl1);
        void AutoSizeHeader(wxListCtrl *const &ListCtrl1, int &colsize);
};

#endif // OUTPUTDATAVIEWERMAIN_H
