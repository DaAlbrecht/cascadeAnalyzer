#ifdef _DEBUG_MSVC
#define new DEBUG_NEW
#endif
#include "MultiportBlockShape.h"
#include "wx/wxsf/CommonFcn.h"

const wxRealPoint MultiportBlock[4]={wxRealPoint(0,0), wxRealPoint(0,80),
                                     wxRealPoint(80,80),wxRealPoint(80,0)
                                    };
XS_IMPLEMENT_CLONABLE_CLASS(multiportBlock, wxSFPolygonShape);

multiportBlock::multiportBlock()
: wxSFPolygonShape()
{
    Initialize();
	EnablePropertySerialization(wxT("vertices"), false);
	SetVertices(4, MultiportBlock);
}

multiportBlock::multiportBlock(const wxRealPoint& pos, wxSFDiagramManager* manager)
: wxSFPolygonShape(4, MultiportBlock, pos, manager)
{
	EnablePropertySerialization(wxT("vertices"), false);
}

multiportBlock::multiportBlock(const multiportBlock& obj)
: wxSFPolygonShape(obj)
{

}

multiportBlock::~multiportBlock()
{

}

//----------------------------------------------------------------------------------//
// public virtual functions
//----------------------------------------------------------------------------------//

bool multiportBlock::Contains(const wxPoint& pos)
{
    wxRect bbRct = this->GetBoundingBox();
    if(!bbRct.Contains(pos))return false;

    wxRealPoint center = GetCenter();
    double k = ((double)bbRct.GetHeight()/2)/((double)bbRct.GetWidth()/2);

    if(pos.x <= center.x)
    {
        // test left-top quadrant
        if((pos.y <= center.y) && (pos.y >= (center.y - (pos.x - bbRct.GetLeft())*k))) return true;
        // test left-bottom quadrant
        if((pos.y >= center.y) && (pos.y <= (center.y + (pos.x - bbRct.GetLeft())*k))) return true;
    }
    else
    {
        // test right-top quadrant
        if((pos.y <= center.y) && (pos.y >= (bbRct.GetTop() + (pos.x - center.x)*k))) return true;
        // test left-bottom quadrant
        if((pos.y >= center.y) && (pos.y <= (bbRct.GetBottom() - (pos.x - center.x)*k))) return true;
    }
    return false;
}


const std::vector<std::pair<wxRealPoint,wxRealPoint>> multiportBlock::getConnectioPoints()
{
    std::vector<std::pair<wxRealPoint,wxRealPoint>> inputconnectionpair;
    size_t numberOfOutputs = wxAtoi(m_pInputsNumber->GetText());
    size_t devider = numberOfOutputs +1;
    wxRect shpBB = this->GetBoundingBox();
    int height =  shpBB.GetTop() / devider;
    for(size_t i = 0; i < devider; ++i)
    {
        wxRealPoint portpoint = (wxRealPoint(shpBB.GetLeft(), (height * i) + shpBB.GetHeight()/2));
        wxRealPoint arrowpoint = (wxRealPoint(shpBB.GetLeft()-5, (height * i) + shpBB.GetHeight()/2));
        inputconnectionpair.push_back(std::make_pair(portpoint,arrowpoint));
    }
    return inputconnectionpair;
}


void multiportBlock::setName(size_t number)
{
   	m_pName = new wxSFEditTextShape();
    // set some properties
    if(m_pName)
    {
        wxString namenumberstring = std::to_string(number);

        m_pName->SetRelativePosition({30,32});
		m_pName->SetStyle(sfsALWAYS_INSIDE | sfsHOVERING | sfsPROCESS_DEL | sfsPROPAGATE_DRAGGING | sfsPROPAGATE_SELECTION | sfsPROPAGATE_INTERACTIVE_CONNECTION );
		SF_ADD_COMPONENT( m_pName, wxT("name") );
        m_pName->SetText("S" + namenumberstring);
    }
}

void multiportBlock::Initialize()
{
	// create associated shape(s)

    m_pGain = new wxSFEditTextShape();
    // set some properties
    if(m_pGain)
    {
        wxRect shpBB = this->GetBoundingBox();
        wxRealPoint txtpoint = (wxRealPoint(shpBB.GetLeft(), (shpBB.GetTop()+55) + shpBB.GetHeight()/2));
        m_pGain->SetRelativePosition(txtpoint);

		m_pGain->SetStyle(sfsHOVERING | sfsPROCESS_DEL | sfsPROPAGATE_DRAGGING | sfsPROPAGATE_SELECTION | sfsPROPAGATE_INTERACTIVE_CONNECTION );

		SF_ADD_COMPONENT( m_pGain, wxT("Gain") );

        m_pGain->SetText("Gain = ");
    }
    m_pGainNumber = new wxSFEditTextShape();
    // set some properties
    if(m_pGainNumber)
    {
        wxRect shpBB = this->GetBoundingBox();
        wxRealPoint txtpoint = (wxRealPoint(shpBB.GetLeft() + 50, (shpBB.GetTop()+55) + shpBB.GetHeight()/2));
        m_pGainNumber->SetRelativePosition(txtpoint);

		m_pGainNumber->SetStyle(sfsHOVERING | sfsPROCESS_DEL | sfsPROPAGATE_DRAGGING | sfsPROPAGATE_SELECTION | sfsPROPAGATE_INTERACTIVE_CONNECTION );

		SF_ADD_COMPONENT( m_pGainNumber, wxT("Gain") );

        m_pGainNumber->SetText("0dB");
    }

    m_pNF = new wxSFEditTextShape();
    // set some properties
    if(m_pNF)
    {
        wxRect shpBB = this->GetBoundingBox();
        wxRealPoint txtpoint = (wxRealPoint(shpBB.GetLeft(), (shpBB.GetTop()-45) + shpBB.GetHeight()/2));
        m_pNF->SetRelativePosition(txtpoint);

		m_pNF->SetStyle(sfsHOVERING | sfsPROCESS_DEL | sfsPROPAGATE_DRAGGING | sfsPROPAGATE_SELECTION | sfsPROPAGATE_INTERACTIVE_CONNECTION );

		SF_ADD_COMPONENT( m_pNF, wxT("NF") );
        m_pNF->SetText("NF = ");
    }

    m_pNFNumber = new wxSFEditTextShape();
    // set some properties
    if(m_pNFNumber)
    {
        wxRect shpBB = this->GetBoundingBox();
        wxRealPoint txtpoint = (wxRealPoint(shpBB.GetLeft()+45, (shpBB.GetTop()-45) + shpBB.GetHeight()/2));
        m_pNFNumber->SetRelativePosition(txtpoint);

		m_pNFNumber->SetStyle(sfsHOVERING | sfsPROCESS_DEL | sfsPROPAGATE_DRAGGING | sfsPROPAGATE_SELECTION | sfsPROPAGATE_INTERACTIVE_CONNECTION );

		SF_ADD_COMPONENT( m_pNFNumber, wxT("NF") );
        m_pNFNumber->SetText("0dB");
    }


    m_pOutputs = new wxSFEditTextShape();
    // set some properties
    if(m_pOutputs)
    {
        wxRect shpBB = this->GetBoundingBox();
        wxRealPoint txtpoint = (wxRealPoint(shpBB.GetLeft(), (shpBB.GetTop()-60) + shpBB.GetHeight()/2));
        m_pOutputs->SetRelativePosition(txtpoint);

		m_pOutputs->SetStyle(sfsHOVERING | sfsPROCESS_DEL | sfsPROPAGATE_DRAGGING | sfsPROPAGATE_SELECTION | sfsPROPAGATE_INTERACTIVE_CONNECTION );

		SF_ADD_COMPONENT( m_pOutputs, wxT("Outputs"));
        m_pOutputs->SetText("Outputs = ");
    }


    m_pOutputsNumber = new wxSFEditTextShape();
    // set some properties
    if(m_pOutputsNumber)
    {
        wxRect shpBB = this->GetBoundingBox();
        wxRealPoint txtpoint = (wxRealPoint(shpBB.GetLeft()+75, (shpBB.GetTop()-60) + shpBB.GetHeight()/2));
        m_pOutputsNumber->SetRelativePosition(txtpoint);

		m_pOutputsNumber->SetStyle(sfsHOVERING | sfsPROCESS_DEL | sfsPROPAGATE_DRAGGING | sfsPROPAGATE_SELECTION | sfsPROPAGATE_INTERACTIVE_CONNECTION );

		SF_ADD_COMPONENT( m_pOutputsNumber, wxT("Outputs"));
        m_pOutputsNumber->SetText("1");
    }

    m_pInputs = new wxSFEditTextShape();
    // set some properties
    if(m_pInputs)
    {
        wxRect shpBB = this->GetBoundingBox();
        wxRealPoint txtpoint = (wxRealPoint(shpBB.GetLeft(), (shpBB.GetTop()+70) + shpBB.GetHeight()/2));
        m_pInputs->SetRelativePosition(txtpoint);

		m_pInputs->SetStyle(sfsHOVERING | sfsPROCESS_DEL | sfsPROPAGATE_DRAGGING | sfsPROPAGATE_SELECTION | sfsPROPAGATE_INTERACTIVE_CONNECTION );

		SF_ADD_COMPONENT( m_pInputs, wxT("Inputs") );
        m_pInputs->SetText("Inputs = ");
    }

    m_pInputsNumber = new wxSFEditTextShape();
     // set some properties
    if(m_pInputsNumber)
    {
        wxRect shpBB = this->GetBoundingBox();
        wxRealPoint txtpoint = (wxRealPoint(shpBB.GetLeft()+65, (shpBB.GetTop()+70) + shpBB.GetHeight()/2));
        m_pInputsNumber->SetRelativePosition(txtpoint);

		m_pInputsNumber->SetStyle(sfsHOVERING | sfsPROCESS_DEL | sfsPROPAGATE_DRAGGING | sfsPROPAGATE_SELECTION | sfsPROPAGATE_INTERACTIVE_CONNECTION );

		SF_ADD_COMPONENT( m_pInputsNumber, wxT("Inputs"));
        m_pInputsNumber->SetText("1");
    }

}

const wxString multiportBlock::getGain()
{
    return m_pGainNumber->GetText();
}
const wxString multiportBlock::getNF()
{
    return m_pNFNumber->GetText();
}
const wxString multiportBlock::getBlockName()
{
    return m_pName->GetText();
}
const wxString multiportBlock::getNumberOfOutputPorts()
{
    return m_pOutputsNumber->GetText();
}
const wxString multiportBlock::getNumberOfInputPorts()
{
    return m_pInputsNumber->GetText();
}

