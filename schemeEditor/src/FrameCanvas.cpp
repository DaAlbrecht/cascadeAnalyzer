#include "wx_pch.h"

#include "FrameCanvas.h"
#include "MainFrame.h"
#include "MainApp.h"

#include <wx/textdlg.h>
#include <wx/filename.h>
FrameCanvas::FrameCanvas(wxSFDiagramManager* manager, wxWindow* parent, wxWindowID id)
: wxSFShapeCanvas(manager, parent, id, wxDefaultPosition, wxDefaultSize, wxHSCROLL | wxVSCROLL | wxSTATIC_BORDER)
{
    // get pointer to main application frame
	m_pParentFrame = (MainFrm*)wxGetApp().GetTopWindow();

	// initialize grid
	AddStyle(sfsGRID_USE);
	AddStyle(sfsGRID_SHOW);
	// distances between grid lines can be modified via following function:
	SetGridLineMult(10);
	// grid line style can be set as follows:
	SetGridStyle(wxSHORT_DASH);

	// canvas background can be printed/ommited during the canvas printing job
    //AddStyle(sfsPRINT_BACKGROUND);

    // adjust the printed drawing align and style if needed
    //SetPrintVAlign(valignTOP);
    //SetPrintHAlign(halignLEFT);
    //SetPrintMode(prnMAP_TO_MARGINS);

	// the canvas background can be filled with a solid colour ...
	//RemoveStyle(sfsGRADIENT_BACKGROUND);
	//SetBackgroundColour(sfdvSHAPECANVAS_BACKGROUNDCOLOR);
	// ... or by a gradient fill
	AddStyle(sfsGRADIENT_BACKGROUND);
	SetGradientFrom(sfdvSHAPECANVAS_GRADIENT_FROM);
	SetGradientTo(sfdvSHAPECANVAS_GRADIENT_TO);

	// also shadows style can be set here:
	//SetShadowFill(wxBrush(wxColour(100, 100, 100), wxCROSSDIAG_HATCH)); // standard values can be sfdvSHAPECANVAS_SHADOWBRUSH or sfdvSHAPECANVAS_SHADOWCOLOR
	//SetShadowOffset(wxRealPoint(7, 7));

	// now you can use also these styles...

	// RemoveStyle(sfsHOVERING);
	// RemoveStyle(sfsHIGHLIGHTING);
	// RemoveStyle(sfsUNDOREDO);
	// RemoveStyle(sfsDND);
	// RemoveStyle(sfsCLIPBOARD);
	// RemoveStyle(sfsMULTI_SIZE_CHANGE);
	// RemoveStyle(sfsMULTI_SELECTION);

	// a style flag presence can be tested like this:
	// if( ContainsStyle(sfsGRID_USE) ) DoSomething();

	// multiple styles can be set in this way:
	// SetStyle( sfsGRID_USE | sfsGRID_SHOW ) ... or ...
	// SetStyle( sfsDEFAULT_CANVAS_STYLE )

	// process mouse wheel if needed
	AddStyle(sfsPROCESS_MOUSEWHEEL);
	// set scale boundaries aplied on mouse wheel scale change
	SetMinScale(0.1);
	SetMaxScale(2);

	// specify accepted shapes...
	GetDiagramManager()->ClearAcceptedShapes();
	GetDiagramManager()->AcceptShape(wxT("All"));

	// ... in addition, specify accepted top shapes (i.e. shapes that can be placed
	// directly onto the canvas)
	// GetDiagramManager()->ClearAcceptedTopShapes();
	// GetDiagramManager()->AcceptTopShape(wxT("wxSFRectShape"));

	// you can set also the canvas history manager working mode:
	// 1) Requires implementation of xsSerializable::Clone() virtual function
	// and copy constructor in all classes derived from the xsSerializable class
	// and shape manager.
	// GetHistoryManager().SetMode(wxSFCanvasHistory::histUSE_CLONING);
	// 2) No other programming overhead is required (except implementation
	// of standard serialization functionality). This working mode is default one.
	GetHistoryManager().SetMode(wxSFCanvasHistory::histUSE_SERIALIZATION);


    wxSFShapeCanvas::Connect(wxEVT_SF_TEXT_CHANGE, wxSFShapeTextEventHandler(FrameCanvas::OnTextChanged), NULL, this);

	// change of the canvas history manager working mode clears the stored states
	// so we should save initial canvas state manually
	SaveCanvasState();
}

FrameCanvas::~FrameCanvas(void)
{
}

//----------------------------------------------------------------------------------//
// public virtual functions
//----------------------------------------------------------------------------------//

void FrameCanvas::OnLeftDown(wxMouseEvent& event)
{
    wxSFShapeBase *pShape = NULL;

	switch(m_pParentFrame->GetToolMode())
	{
	case MainFrm::modeGAINBLOCK:
		{
		    size_t number = 0;
            ShapeList lstShapes;
            GetDiagramManager()->GetShapes(sfANY, lstShapes, xsSerializable::searchBFS);
            wxSFShapeBase *pChild;
            if(!lstShapes.IsEmpty())
            {
                ShapeList::compatibility_iterator node = lstShapes.GetFirst();
                while(node)
                {
                    pChild = node->GetData();
                    gainBlock *pGainBlock = dynamic_cast<gainBlock *>(pChild);
                    if(pGainBlock)
                    {
                        ++number;
                    }
                    node = node->GetNext();
                }
            }
            pShape = GetDiagramManager()->AddShape(CLASSINFO(gainBlock), event.GetPosition(), sfDONT_SAVE_STATE);
            if(gainBlock *pGainBlock = dynamic_cast<gainBlock *>(pShape))
                pGainBlock->setName(number);
			if(pShape)
			{
			    // set shape policy
				pShape->AcceptChild(wxT("wxSFTextShape"));
				pShape->AcceptChild(wxT("wxSFEditTextShape"));

				pShape->AcceptConnection(wxT("All"));
                pShape->AcceptSrcNeighbour(wxT("All"));
                pShape->AcceptTrgNeighbour(wxT("All"));
			}
		}
		break;
    case MainFrm::modeMULTIPORTBLOCK:
    {
        size_t number = 0;
        ShapeList lstShapes;
        GetDiagramManager()->GetShapes(sfANY, lstShapes, xsSerializable::searchBFS);
        wxSFShapeBase *pChild;
        if(!lstShapes.IsEmpty())
        {
            ShapeList::compatibility_iterator node = lstShapes.GetFirst();
            while(node)
            {
                pChild = node->GetData();
                multiportBlock *pmultiportBlock = dynamic_cast<multiportBlock *>(pChild);
                if(pmultiportBlock)
                {
                    ++number;
                }
                node = node->GetNext();
            }
        }
        pShape = GetDiagramManager()->AddShape(CLASSINFO(multiportBlock), event.GetPosition(), sfDONT_SAVE_STATE);
        if(multiportBlock *pmultiportBlock = dynamic_cast<multiportBlock *>(pShape))
            pmultiportBlock->setName(number);
        if(pShape)
        {
            // set shape policy
            pShape->AcceptChild(wxT("wxSFTextShape"));
            pShape->AcceptChild(wxT("wxSFEditTextShape"));

            pShape->AcceptConnection(wxT("All"));
            pShape->AcceptSrcNeighbour(wxT("All"));
            pShape->AcceptTrgNeighbour(wxT("All"));
        }
		}
		break;
     case MainFrm::modeORTHOLINE:
        {
            if(GetMode() == modeREADY)
            {
                StartInteractiveConnection(CLASSINFO(ConnectionLineShape), event.GetPosition());
            }
            else
                wxSFShapeCanvas::OnLeftDown(event);
        }
        break;

	default:
		// do default actions
		wxSFShapeCanvas::OnLeftDown(event);
	}

	if(pShape)
	{
	    ShowShadows(m_pParentFrame->m_fShowShadows, shadowALL);
	    SaveCanvasState();
        pShape->Refresh();
	}
}

void FrameCanvas::OnKeyDown(wxKeyEvent& event)
{
	switch(event.GetKeyCode())
	{
	case WXK_ESCAPE:
		m_pParentFrame->SetToolMode(MainFrm::modeDESIGN);
		break;
	}

	// do default actions
	wxSFShapeCanvas::OnKeyDown(event);
}
wxSFShapeCanvas::PRECONNECTIONFINISHEDSTATE FrameCanvas::OnPreConnectionFinished(wxSFLineShape* connection)
{
     if(connection)
     {
         ShapeList lstShapes;
         GetDiagramManager()->GetShapes(sfANY, lstShapes, xsSerializable::searchBFS);
         wxSFShapeBase *pChild;
        if(!lstShapes.IsEmpty())
        {
            std::vector<wxRealPoint> trgPoints;
            std::vector<wxRealPoint> srcPoints;
            ShapeList::compatibility_iterator node = lstShapes.GetFirst();
            while(node)
            {
                ShapeList shapes;
                pChild = node->GetData();
                pChild->GetAssignedConnections(CLASSINFO(wxSFOrthoLineShape), wxSFShapeBase::lineSTARTING, shapes);
                if(!shapes.IsEmpty())
                {
                    ShapeList::compatibility_iterator connections_itr = shapes.GetFirst();
                    while(connections_itr)
                    {
                        wxSFShapeBase *pBaseShape = connections_itr->GetData();
                        if(wx SFLineShape *pLineShape = dynamic_cast<wxSFLineShape *>(pBaseShape))
                        {
                            trgPoints.push_back(pLineShape->GetTrgPoint());
                            srcPoints.push_back(pLineShape->GetSrcPoint());
                            for(size_t i = 0; i < trgPoints.size()-1; ++i)
                            {
                                gainBlock gn;
                                if(gainBlock::OUTPUT == gn.findConnectionPoint(trgPoints.back()))
                                {
                                    wxMessageBox("this is an output");
                                }
                                if((trgPoints.back() == trgPoints.at(i)) || (srcPoints.back() == srcPoints.at(i)))
                                {
                                    wxMessageBox("port already used");
                                    return wxSFShapeCanvas::pfsFAILED_AND_CANCEL_LINE;
                                }

                                if(trgPoints.back() == srcPoints.at(i) || trgPoints.at(i) == srcPoints.back())
                                {
                                    wxMessageBox("port already used");
                                    return wxSFShapeCanvas::pfsFAILED_AND_CANCEL_LINE;
                                }
                            }
                        }
                        connections_itr = connections_itr->GetNext();
                    }
                }
                node = node->GetNext();
            }
        }
    }
    return wxSFShapeCanvas::pfsOK;
}
void FrameCanvas::OnConnectionFinished(wxSFLineShape* connection)
{
    if(connection)
    {
		// the line's ending style can be set like this:
        connection->SetTrgArrow(CLASSINFO(wxSFSolidArrow));
        connection->AcceptChild(wxT("wxSFTextShape"));
        connection->AcceptChild(wxT("wxSFEditTextShape"));
		connection->AcceptConnection(wxT("All"));
        connection->AcceptSrcNeighbour(wxT("All"));
        connection->AcceptTrgNeighbour(wxT("All"));
    }
}
void FrameCanvas::OnMouseWheel(wxMouseEvent& event)
{
	// do default actions
	wxSFShapeCanvas::OnMouseWheel(event);

	// adjust zoom slider control
	m_pParentFrame->GetZoomSlider()->SetValue(GetScale() * 50);
}

void FrameCanvas::OnTextChanged(wxSFShapeTextEvent& event)
{
    wxSFEditTextShape* pText = (wxSFEditTextShape*)event.GetShape();
    if(pText)
    {
		pText->Update();

    }
    wxSFShapeBase* pbase = (wxSFShapeBase*)event.GetShape();
    if(multiportBlock *pmultiportBlock = dynamic_cast<multiportBlock *>(pbase->GetParent()))
    {
            pmultiportBlock->creatConnectionPoints();
    }

}
