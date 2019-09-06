#include "wx_pch.h"

#include <wx/artprov.h>

#include "MainFrame.h"
#include "Art.h"
#include "Ids.h"
#include <fstream>
BEGIN_EVENT_TABLE(MainFrm, wxFrame)
    EVT_CLOSE(MainFrm::OnClose)
	EVT_MENU(wxID_EXIT, MainFrm::OnExit)
	EVT_MENU(wxID_NEW, MainFrm::OnNew)
	EVT_MENU(wxID_OPEN, MainFrm::OnLoad)
	EVT_MENU(wxID_SAVE, MainFrm::OnSave)
	EVT_MENU(wxID_UNDO, MainFrm::OnUndo)
	EVT_MENU(wxID_REDO, MainFrm::OnRedo)
	EVT_MENU(wxID_COPY, MainFrm::OnCopy)
	EVT_MENU(wxID_CUT, MainFrm::OnCut)
	EVT_MENU(wxID_PASTE, MainFrm::OnPaste)
	EVT_MENU(wxID_ABOUT, MainFrm::OnAbout)
	EVT_MENU(wxID_SELECTALL, MainFrm::OnSelectAll)
	EVT_MENU(IDM_SAVEASBITMAP, MainFrm::OnExportToBMP)
	EVT_MENU(wxID_PRINT, MainFrm::OnPrint)
	EVT_MENU(wxID_PREVIEW, MainFrm::OnPrintPreview)
	EVT_MENU(wxID_PAGE_SETUP, MainFrm::OnPageSetup)

	EVT_MENU(wxID_GENERATEDATAFILE, MainFrm::OnGenerateInputDataFile)

	EVT_MENU_RANGE(IDM_AUTOLAYOUT_FIRST, IDM_AUTOLAYOUT_LAST, MainFrm::OnAutoLayout)
	EVT_COMMAND_SCROLL(wxID_ZOOM_FIT, MainFrm::OnSlider)
	EVT_TOOL_RANGE(IDT_FIRST_TOOLMARKER, IDT_LAST_TOOLMARKER, MainFrm::OnTool)
	EVT_COLOURPICKER_CHANGED(IDT_COLORPICKER, MainFrm::OnHowerColor)
	EVT_UPDATE_UI(wxID_COPY, MainFrm::OnUpdateCopy)
	EVT_UPDATE_UI(wxID_CUT, MainFrm::OnUpdateCut)
	EVT_UPDATE_UI(wxID_PASTE, MainFrm::OnUpdatePaste)
	EVT_UPDATE_UI(wxID_UNDO, MainFrm::OnUpdateUndo)
	EVT_UPDATE_UI(wxID_REDO, MainFrm::OnUpdateRedo)
	EVT_UPDATE_UI_RANGE(IDT_FIRST_TOOLMARKER, IDT_LAST_TOOLMARKER, MainFrm::OnUpdateTool)
	EVT_UPDATE_UI_RANGE(IDM_AUTOLAYOUT_FIRST, IDM_AUTOLAYOUT_LAST, MainFrm::OnUpdateAutoLayout)
	EVT_IDLE(MainFrm::OnIdle)
END_EVENT_TABLE()

//----------------------------------------------------------------------------------//
// canvas thumbnail
//----------------------------------------------------------------------------------//

ThumbFrm::ThumbFrm(wxWindow* parent) : _ThumbFrm(parent)
{
	m_pThumbnail = new wxSFThumbnail(this);
	m_pMainSizer->Add(m_pThumbnail, 1, wxEXPAND, 0);
	Layout();
}

ThumbFrm::~ThumbFrm()
{
}

//----------------------------------------------------------------------------------//
// main application frame
//----------------------------------------------------------------------------------//

MainFrm::MainFrm( wxWindow* parent ) : _MainFrm( parent )
{
	wxInitAllImageHandlers();

    // set icon
	SetIcon(wxIcon(wx_xpm));

	m_pFileMenu->Append(wxID_NEW, wxT("&New\tCtrl+N"), wxT("New chart"), wxITEM_NORMAL);
	m_pFileMenu->Append(wxID_OPEN, wxT("&Open\tCtrl+O"), wxT("Load a chart from XML file"), wxITEM_NORMAL);
	m_pFileMenu->Append(wxID_SAVE, wxT("&Save as...\tCtrl+Shift+S"), wxT("Save the chart to XML file"), wxITEM_NORMAL);
	m_pFileMenu->AppendSeparator();
	m_pFileMenu->Append(IDM_SAVEASBITMAP, wxT("&Export to image..."), wxT("Export the chart to BMP file"), wxITEM_NORMAL);
	m_pFileMenu->AppendSeparator();
	m_pFileMenu->Append(wxID_PRINT, wxT("&Print...\tCtrl+P"), wxT("Open pring dialog"), wxITEM_NORMAL);
	m_pFileMenu->Append(wxID_PREVIEW, wxT("Print pre&view...\tAlt+P"), wxT("Open print preview window"), wxITEM_NORMAL);
	m_pFileMenu->Append(wxID_PAGE_SETUP, wxT("Pa&ge setup..."), wxT("Set print page properties"), wxITEM_NORMAL);
	m_pFileMenu->AppendSeparator();
    m_pFileMenu->Append(wxID_EXIT, wxT("E&xit\tAlt+X"), wxT("Close application"), wxITEM_NORMAL);
    m_pFileMenu->Append(wxID_GENERATEDATAFILE, _("generate output file\tCtrl+shift+G"), _("Open diagram from XML file"));


	m_pEditMenu->Append(wxID_UNDO, wxT("&Undo\tCtrl+Z"), wxT("Discard previous action"), wxITEM_NORMAL);
	m_pEditMenu->Append(wxID_REDO, wxT("&Redo\tCtrl+Y"), wxT("Re-do previously discarded action"), wxITEM_NORMAL);
	m_pEditMenu->AppendSeparator();
	m_pEditMenu->Append(wxID_SELECTALL, wxT("Select &all\tCtrl+A"), wxT("Select all shapes"), wxITEM_NORMAL);
	m_pEditMenu->AppendSeparator();
	m_pEditMenu->Append(wxID_COPY, wxT("&Copy\tCtrl+C"), wxT("Copy shapes to the clipboard"), wxITEM_NORMAL);
	m_pEditMenu->Append(wxID_CUT, wxT("Cu&t\tCtrl+X"), wxT("Cut shapes to the clipboard"), wxITEM_NORMAL);
	m_pEditMenu->Append(wxID_PASTE, wxT("&Paste\tCtrl+V"), wxT("Paste shapes to the canvas"), wxITEM_NORMAL);

    m_pEditMenu->AppendSeparator();
	m_pEditMenu->Append(IDT_GAINBLOCKSHP, wxT("&GainBlock\tCtrl+G"), wxT("GainBlock"), wxITEM_NORMAL);
	m_pEditMenu->Append(IDT_MULTIPORTBLOCK, wxT("&MultiPortBlock\tCtrl+S"), wxT("MultiPortBlock"), wxITEM_NORMAL);
	m_pEditMenu->Append(IDT_ORTHOSHP, wxT("&Connection\tCtrl+W"), wxT("Connection"), wxITEM_NORMAL);
	m_pEditMenu->Append(IDT_TOOL, wxT("&Design\tCtrl+D"), wxT("Design tool"), wxITEM_NORMAL);

	wxArrayString arrLayouts = m_AutoLayout.GetRegisteredAlgorithms();

	for( size_t i = 0; i < arrLayouts.GetCount(); ++i )
	{
		m_pAutoLayoutMenu->Append( IDM_AUTOLAYOUT_FIRST + i, arrLayouts[i] );
	}
    m_pHelpMenu->Append(wxID_ABOUT, wxT("&About..."), wxT("About application..."), wxITEM_NORMAL);

    // set shape canvas and associate it with diagram manager
    m_pShapeCanvas = new FrameCanvas(&m_DiagramManager, m_pCanvasPanel, wxID_ANY);
	m_pCanvasSizer->Add(m_pShapeCanvas, 1, wxEXPAND, 0);
	m_pCanvasPanel->Layout();
    // set whether wxGraphicsContext should be used (valid only if wxUSE_GRAPHICS_CONTEXT if set to 1 for both wxSF and wxWidgets libraries)
    wxSFShapeCanvas::EnableGC(true);

	// create and show canvas thumbnail
//	m_pThumbFrm = new ThumbFrm(this);
//	m_pThumbFrm->GetThumbnail()->SetCanvas(m_pShapeCanvas);
//	m_pThumbFrm->Show();

	// create colour picker
	#ifdef __WXMSW__
	cpicker = new wxColourPickerCtrl(m_pToolBar, IDT_COLORPICKER, wxColor(120, 120, 255), wxDefaultPosition, wxSize(22, 22));
	#else
	cpicker = new wxColourPickerCtrl(m_pToolBar, IDT_COLORPICKER, wxColor(120, 120, 255), wxDefaultPosition, wxSize(28, 28));
	#endif
	cpicker->SetToolTip(wxT("Set hower color"));

	// add m_pToolBar tools
	m_pToolBar->SetToolBitmapSize(wxSize(16, 15));
	m_pToolBar->AddTool(wxID_NEW, wxT("New"), wxArtProvider::GetBitmap(wxART_NEW, wxART_MENU), wxT("New diagram"));
	m_pToolBar->AddTool(wxID_OPEN, wxT("Load"), wxArtProvider::GetBitmap(wxART_FILE_OPEN, wxART_MENU), wxT("Open file..."));
	m_pToolBar->AddTool(wxID_SAVE, wxT("Save"), wxArtProvider::GetBitmap(wxART_FILE_SAVE, wxART_MENU), wxT("Save file..."));
	m_pToolBar->AddSeparator();
	m_pToolBar->AddTool(wxID_PRINT, wxT("Print"), wxArtProvider::GetBitmap(wxART_PRINT, wxART_MENU), wxT("Print..."));
	m_pToolBar->AddTool(wxID_PREVIEW, wxT("Preview"), wxArtProvider::GetBitmap(wxART_FIND, wxART_MENU), wxT("Print preview..."));
	m_pToolBar->AddSeparator();
	m_pToolBar->AddTool(wxID_COPY, wxT("Copy"), wxArtProvider::GetBitmap(wxART_COPY, wxART_MENU), wxT("Copy to clipboard"));
	m_pToolBar->AddTool(wxID_CUT, wxT("Cut"), wxArtProvider::GetBitmap(wxART_CUT, wxART_MENU), wxT("Cut to clipboard"));
	m_pToolBar->AddTool(wxID_PASTE, wxT("Paste"), wxArtProvider::GetBitmap(wxART_PASTE, wxART_MENU), wxT("Paste from clipboard"));
	m_pToolBar->AddSeparator();
	m_pToolBar->AddTool(wxID_UNDO, wxT("Undo"), wxArtProvider::GetBitmap(wxART_UNDO, wxART_MENU), wxT("Undo"));
	m_pToolBar->AddTool(wxID_REDO, wxT("Redo"), wxArtProvider::GetBitmap(wxART_REDO, wxART_MENU), wxT("Redo"));
	m_pToolBar->AddSeparator();
	m_pToolBar->AddCheckTool(IDT_GRID, wxT("Grid"), wxBitmap(Grid_xpm), wxNullBitmap, wxT("Show/hide grid"));
	m_pToolBar->AddCheckTool(IDT_SHADOW, wxT("Shadows"), wxBitmap(Shadow_xpm), wxNullBitmap, wxT("Show/hide shadows"));
	m_pToolBar->AddCheckTool(IDT_GC, wxT("Enhanced graphics context"), wxBitmap(GC_xpm), wxNullBitmap, wxT("Use enhanced graphics context (wxGraphicsContext)"));
	m_pToolBar->AddSeparator();
	m_pToolBar->AddRadioTool(IDT_TOOL, wxT("Tool"), wxBitmap(Tool_xpm), wxNullBitmap, wxT("Design tool"));
	m_pToolBar->AddRadioTool(IDT_GAINBLOCKSHP, wxT("GainBlock"), wxBitmap(Diamond_xpm), wxNullBitmap, wxT("GainBlock"));
	m_pToolBar->AddRadioTool(IDT_MULTIPORTBLOCK, wxT("MultiPortBlock"), wxBitmap(Rect_xpm), wxNullBitmap, wxT("MultiportBlock"));
	m_pToolBar->AddRadioTool(IDT_ORTHOSHP, wxT("Ortho line"), wxBitmap(OrthoLine_xpm), wxNullBitmap, wxT("Orthogonal connection"));
	m_pToolBar->AddControl(cpicker);
	m_pToolBar->Realize();

	m_pStatusBar->SetStatusText(wxT("Ready"));

	// initialize data members
	m_nToolMode = modeDESIGN;
	m_fShowGrid = true;
	m_fShowShadows = false;

	Centre();
}

//----------------------------------------------------------------------------------//
// protected functions
//----------------------------------------------------------------------------------//

void MainFrm::CleanUp()
{
    m_DiagramManager.SetShapeCanvas(NULL);
	m_DiagramManager.Clear();

//	m_pThumbFrm->Hide();
//	m_pThumbFrm->GetThumbnail()->SetCanvas(NULL);

	// clean up registered (standard) autolayout algorithms
	wxSFAutoLayout::CleanUp();

 	Destroy();
}

//----------------------------------------------------------------------------------//
// common events
//----------------------------------------------------------------------------------//

void MainFrm::OnClose(wxCloseEvent& WXUNUSED(event))
{
	CleanUp();
}

//----------------------------------------------------------------------------------//
// menu events
//----------------------------------------------------------------------------------//

void MainFrm::OnExit(wxCommandEvent& WXUNUSED(event))
{
    CleanUp();
}

void MainFrm::OnNew(wxCommandEvent& WXUNUSED(event))
{
	if(wxMessageBox(wxT("Current chart will be lost. Do you want to proceed?"), wxT("Cascade Analyzer"), wxYES_NO | wxICON_QUESTION) == wxYES)
	{
		m_DiagramManager.Clear();

		m_pShapeCanvas->ClearCanvasHistory();
        m_pShapeCanvas->SaveCanvasState();

		m_pShapeCanvas->Refresh();
	}
}

void MainFrm::OnLoad(wxCommandEvent& WXUNUSED(event))
{
	wxFileDialog dlg(this, wxT("Load canvas from XML..."), wxGetCwd(), wxT(""), wxT("XML Files (*.xml)|*.xml"), wxOPEN | wxFD_FILE_MUST_EXIST);

	if(dlg.ShowModal() == wxID_OK)
	{
		m_pShapeCanvas->LoadCanvas(dlg.GetPath());

		m_pZoomSlider->SetValue(int(m_pShapeCanvas->GetScale()*50));

		cpicker->SetColour(m_pShapeCanvas->GetHoverColour());
	}
}

void MainFrm::OnSave(wxCommandEvent& WXUNUSED(event))
{
	wxFileDialog dlg(this, wxT("Save canvas to XML..."), wxGetCwd(), wxT(""), wxT("XML Files (*.xml)|*.xml"), wxSAVE | wxFD_OVERWRITE_PROMPT);

	if(dlg.ShowModal() == wxID_OK)
	{
		m_pShapeCanvas->SaveCanvas(dlg.GetPath());

		wxMessageBox(wxString::Format(wxT("The chart has been saved to '%s'."), dlg.GetPath().GetData()), wxT("Cascade Analyzer"));
	}
}

void MainFrm::OnUndo(wxCommandEvent& WXUNUSED(event))
{
	m_pShapeCanvas->Undo();
}

void MainFrm::OnUpdateUndo(wxUpdateUIEvent& event)
{
	if( m_pShapeCanvas ) event.Enable(m_pShapeCanvas->CanUndo());
}

void MainFrm::OnRedo(wxCommandEvent& WXUNUSED(event))
{
	m_pShapeCanvas->Redo();
}

void MainFrm::OnUpdateRedo(wxUpdateUIEvent& event)
{
	if( m_pShapeCanvas ) event.Enable(m_pShapeCanvas->CanRedo());
}

void MainFrm::OnCopy(wxCommandEvent& WXUNUSED(event))
{
	m_pShapeCanvas->Copy();
}

void MainFrm::OnUpdateCopy(wxUpdateUIEvent& event)
{
	if( m_pShapeCanvas ) event.Enable(m_pShapeCanvas->CanCopy());
}

void MainFrm::OnCut(wxCommandEvent& WXUNUSED(event))
{
	m_pShapeCanvas->Cut();
}

void MainFrm::OnUpdateCut(wxUpdateUIEvent& event)
{
	if( m_pShapeCanvas ) event.Enable(m_pShapeCanvas->CanCut());
}

void MainFrm::OnPaste(wxCommandEvent& WXUNUSED(event))
{
	m_pShapeCanvas->Paste();
}

void MainFrm::OnUpdatePaste(wxUpdateUIEvent& event)
{
	if( m_pShapeCanvas ) event.Enable(m_pShapeCanvas->CanPaste());
}

void MainFrm::OnSelectAll(wxCommandEvent& WXUNUSED(event))
{
	m_pShapeCanvas->SelectAll();
}

void MainFrm::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox(wxString::Format(wxT("Cascade Analyzer"), m_DiagramManager.GetVersion().c_str()), wxT("Cascade Analyzer"));
}

void MainFrm::OnExportToBMP(wxCommandEvent& WXUNUSED(event))
{
	wxFileDialog dlg(this, wxT("Export canvas to image..."), wxGetCwd(), wxT(""),
		wxT("BMP Files (*.bmp)|*.bmp|GIF Files (*.gif)|(*.gif)|XPM Files (*.xpm)|*.xpm|PNG Files (*.png)|*.png|JPEG Files (*.jpg)|*.jpg"), wxSAVE);

	if(dlg.ShowModal() == wxID_OK)
	{
		wxBitmapType type = wxBITMAP_TYPE_ANY;

		switch( dlg.GetFilterIndex() )
		{
			case 0:
				type = wxBITMAP_TYPE_BMP;
				break;
			case 1:
				type = wxBITMAP_TYPE_GIF;
				break;
			case 2:
				type = wxBITMAP_TYPE_XPM;
				break;
			case 3:
				type = wxBITMAP_TYPE_PNG;
				break;
			case 4:
				type = wxBITMAP_TYPE_JPEG;
				break;
		}

        m_pShapeCanvas->SaveCanvasToImage( dlg.GetPath(), type, sfWITH_BACKGROUND );
	}
}

//----------------------------------------------------------------------------------//
// tool events
//----------------------------------------------------------------------------------//

void MainFrm::OnTool(wxCommandEvent& event)
{
    if(m_pShapeCanvas->GetMode() == FrameCanvas::modeCREATECONNECTION)m_pShapeCanvas->AbortInteractiveConnection();

    switch(event.GetId())
    {
        case IDT_GRID:
        	m_fShowGrid = !m_fShowGrid;
			if( m_fShowGrid )
			{
				m_pShapeCanvas->AddStyle(wxSFShapeCanvas::sfsGRID_SHOW);
				m_pShapeCanvas->AddStyle(wxSFShapeCanvas::sfsGRID_USE);
			}
			else
			{
				m_pShapeCanvas->RemoveStyle(wxSFShapeCanvas::sfsGRID_SHOW);
				m_pShapeCanvas->RemoveStyle(wxSFShapeCanvas::sfsGRID_USE);
			}
            //m_pShapeCanvas->UseGrid(m_fShowGrid); !!! DEPRECATED !!!
            //m_pShapeCanvas->ShowGrid(m_fShowGrid); !!! DEPRECATED !!!
            m_pShapeCanvas->Refresh(false);
            break;

        case IDT_SHADOW:
        	m_fShowShadows = !m_fShowShadows;

        	m_pShapeCanvas->ShowShadows(m_fShowShadows, wxSFShapeCanvas::shadowALL);
        	// also shadows for topmost shapes only are allowed:
        	//m_pShapeCanvas->ShowShadows(m_fShowShadows, wxSFShapeCanvas::shadowTOPMOST);

            m_pShapeCanvas->Refresh(false);
            break;

        case IDT_GC:
			#if wxUSE_GRAPHICS_CONTEXT
        	wxSFShapeCanvas::EnableGC(!wxSFShapeCanvas::IsGCEnabled());
//        	wxSFShapeCanvas::EnableGC( true );
			// update all shapes in the manager
			m_DiagramManager.UpdateAll();
			// refresh shape canvas
            m_pShapeCanvas->Refresh(false);
			#else
			wxMessageBox( wxT("Could not enable enhanced graphics context due to wxUSE_GRAPHICS_CONTEXT=0"), wxT("Cascade Analyzer"), wxOK | wxICON_WARNING );
			#endif
            break;

        case IDT_ORTHOSHP:
            m_nToolMode = modeORTHOLINE;
            break;

        case IDT_GAINBLOCKSHP:
            m_nToolMode = modeGAINBLOCK;
            break;
        case IDT_MULTIPORTBLOCK:
            m_nToolMode = modeMULTIPORTBLOCK;
            break;

        case IDT_TOOL:
            m_nToolMode = modeDESIGN;
            break;

        default:
            event.Skip();
            break;
    }
}

void MainFrm::OnUpdateTool(wxUpdateUIEvent& event)
{
    switch(event.GetId())
    {
        case IDT_GRID:
        	event.Check(m_fShowGrid);
            break;

        case IDT_GC:
        	event.Check( wxSFShapeCanvas::IsGCEnabled() );
            break;

        case IDT_ORTHOSHP:
            event.Check(m_nToolMode == modeORTHOLINE);
            break;

        case IDT_GAINBLOCKSHP:
            event.Check(m_nToolMode == modeGAINBLOCK);
            break;
        case IDT_MULTIPORTBLOCK:
            event.Check(m_nToolMode == modeMULTIPORTBLOCK);
            break;

        case IDT_TOOL:
            event.Check(m_nToolMode == modeDESIGN);
            break;

        default:
            event.Skip();
            break;
    }
}

void MainFrm::OnUpdateAutoLayout(wxUpdateUIEvent& event)
{
	event.Enable( !m_DiagramManager.IsEmpty() );
}

//----------------------------------------------------------------------------------//
// printing functions
//----------------------------------------------------------------------------------//

void MainFrm::OnPrint(wxCommandEvent& WXUNUSED(event))
{
    m_pShapeCanvas->Print();
}

void MainFrm::OnPrintPreview(wxCommandEvent& WXUNUSED(event))
{
    m_pShapeCanvas->PrintPreview();
}

void MainFrm::OnPageSetup(wxCommandEvent& WXUNUSED(event))
{
    m_pShapeCanvas->PageSetup();
}

//----------------------------------------------------------------------------------//
// other events
//----------------------------------------------------------------------------------//

void MainFrm::OnSlider(wxScrollEvent& WXUNUSED(event))
{
	m_pShapeCanvas->SetScale(double(m_pZoomSlider->GetValue())/50);
	m_pShapeCanvas->Refresh(false);
}

void MainFrm::OnHowerColor(wxColourPickerEvent& event)
{
	m_pShapeCanvas->SetHoverColour(event.GetColour());
}

void MainFrm::OnAutoLayout(wxCommandEvent& event)
{
	m_AutoLayout.Layout( m_pShapeCanvas, m_pAutoLayoutMenu->GetLabel( event.GetId() ) );
	m_pShapeCanvas->SaveCanvasState();
}

void MainFrm::OnIdle(wxIdleEvent& event)
{
	if( m_DiagramManager.IsModified() ) SetTitle( wxT("Cascade Analyzer (diagram is modified)") );
	else SetTitle( wxT("Cascade Analyzer") );
}
void MainFrm::OnGenerateInputDataFile(wxCommandEvent& event)
{
    int processReturnValue;
    std::vector<wxString> BlockType;
    std::vector<wxString> BlockName;
    std::vector<wxString> Gain;
    std::vector<wxString> NF;
    std::vector<wxString> OutputName;
    std::vector<wxString> InputName;
    std::vector<wxString> NumberOfOutPorts;
    std::vector<wxString> NumberOfInPorts;

    std::ofstream outfile ("D:\\cascadeAnalyzer\\calculateData\\input_data.txt");
    outfile << "-block" << "\n";

    getBlockData(BlockType, BlockName, Gain, NF, InputName, OutputName, NumberOfOutPorts, NumberOfInPorts);

    for(size_t i = 0; i < BlockName.size(); ++i)
    {
        if(BlockType.at(i) == "G")
            outfile << BlockType.at(i) << "\t" << BlockName.at(i) << "\t" << "G="  << Gain.at(i) << "\t" << "NF=" << NF.at(i) <<"\n";
        if(BlockType.at(i) == "S")
        {
            outfile << BlockType.at(i) << "\t" << BlockName.at(i) << "\t"
            << "G=" << Gain.at(i) << "\t" << "NF=" <<  NF.at(i) << "  \t"
            << "Outputs=" << NumberOfOutPorts.at(i) << "\t"  << "Inputs=" << NumberOfInPorts.at(i) << "\n";
        }
    }
    outfile << "-nets" << "\n";
    for(size_t i = 0; i < OutputName.size(); ++i)
    {
        outfile << OutputName.at(i) << "\t" << InputName.at(i) << "\n";
    }
    wxMessageBox("file successfully generated");
    startCalculation(processReturnValue);
    if(processReturnValue == 0)
    {
        dataViewer();
    }
    dataViewer();
}

void MainFrm::getBlockData(vecString &BlockType, vecString &BlockName, vecString &Gain, vecString &NF,
                            vecString &InputName, vecString &OutputName, vecString &NumberOfInPorts, vecString &NumberOfOutPorts)
{
    ShapeList lstShapes;
    m_DiagramManager.GetShapes(sfANY, lstShapes, xsSerializable::searchBFS);
    wxSFShapeBase *pChild;
    std::vector<wxSFShapeBase*> shapeArray;
    if(!lstShapes.IsEmpty())
    {
        ShapeList::compatibility_iterator node = lstShapes.GetFirst();
        while(node)
        {
            pChild = node->GetData();

            gainBlock *pGainBlock = dynamic_cast<gainBlock *>(pChild);
            if(pGainBlock)
            {
                shapeArray.push_back(pChild);
                BlockType.push_back("G");
                BlockName.push_back(pGainBlock->getBlockName());
                Gain.push_back(pGainBlock->getGain());
                NF.push_back(pGainBlock->getNF());
                NumberOfOutPorts.push_back("1");
                NumberOfInPorts.push_back("1");
            }

            multiportBlock *pmultiportBlock = dynamic_cast<multiportBlock *>(pChild);
            if(pmultiportBlock)
            {
                shapeArray.push_back(pChild);
                BlockType.push_back("S");
                BlockName.push_back(pmultiportBlock->getBlockName());
                Gain.push_back(pmultiportBlock->getGain());
                NF.push_back(pmultiportBlock->getNF());
                NumberOfOutPorts.push_back(pmultiportBlock->getNumberOfOutputPorts());
                NumberOfInPorts.push_back(pmultiportBlock->getNumberOfInputPorts());
            }
            node = node->GetNext();
        }
        getNetData(shapeArray, InputName, OutputName);
    }
}

void MainFrm::getNetData(std::vector<wxSFShapeBase*> &shapeArray, vecString &InputName, vecString &OutputName)
{
    for(size_t i = 0; i < shapeArray.size(); ++i)
    {
        ShapeList connections;
        wxSFShapeBase* sourceBlock = shapeArray.at(i);
        m_DiagramManager.GetAssignedConnections(sourceBlock, CLASSINFO(wxSFLineShape), wxSFShapeBase::lineSTARTING, connections);
        if(!connections.IsEmpty())
        {
            ShapeList::compatibility_iterator connection = connections.GetFirst();
            wxString inputportnumber;
            wxString outoutportnumber;
            size_t inputportcounter = 1;
            size_t outputportcounter = 1;
            //size_t outPort = 0;
            while(connection)
            {
                wxSFShapeBase *pLine = connection->GetData();
                //++outPort;
                if(wxSFLineShape *pLineShape = dynamic_cast<wxSFLineShape *>(pLine))
                {
                    if(gainBlock *pGainBlock = dynamic_cast<gainBlock *>(sourceBlock))
                        OutputName.push_back(pGainBlock->getBlockName() + "." + std::to_string(outputportcounter));
                    else if( multiportBlock *pmultiportBlock = dynamic_cast<multiportBlock *>(sourceBlock))
                        OutputName.push_back(pmultiportBlock->getBlockName() + "." + std::to_string(outputportcounter));
                    for(size_t n = 0; n < shapeArray.size(); ++n)
                    {
                        if (shapeArray.at(n)->GetId() == pLineShape->GetTrgShapeId())
                        {
                            ///TODO (DAL) here we need to check for the correct output port number
                            gainBlock *pGainBlock = dynamic_cast<gainBlock *>(shapeArray.at(n));
                            multiportBlock *pmultiportBlock = dynamic_cast<multiportBlock *>(shapeArray.at(n));
                            if(i > 1)
                            {
//                                inputportcounter += std::count_if(shapeArray.begin(),shapeArray.end(),[&](wxSFShapeBase*)
//                                {return shapeArray.at(i-1)->GetId() == shapeArray.at(n)->GetId();});
                            }
                            if(pGainBlock)
                                InputName.push_back(pGainBlock->getBlockName() + "." + std::to_string(inputportcounter));
                            else if (pmultiportBlock)
                                InputName.push_back(pmultiportBlock->getBlockName() + "." + std::to_string(inputportcounter));
                        }
                    }
                    connection = connection->GetNext();
                }
            }
        }
    }
}
void MainFrm::startCalculation(int &processReturnValue)
{
    processReturnValue = wxExecute("D:\\cascadeAnalyzer\\calculateData\\bin\\Release\\Rf_Cascade_Analyzer.exe D:\\cascadeAnalyzer\\calculateData\\input_data.txt D:\\cascadeAnalyzer\\outputDataViewer\\outputdata.csv",
           wxEXEC_NODISABLE,
           NULL,
           NULL
           );
            ///TODO(DAL) check DRC if DRC is oke ->

}

void MainFrm::dataViewer()
{
    wxExecute("D:\\cascadeAnalyzer\\outputDataViewer\\bin\\Release\\OutputDataViewer.exe",
           wxEXEC_NODISABLE ,
           NULL,
           NULL
           );

}

