#ifdef _DEBUG_MSVC
#define new DEBUG_NEW
#endif
#include "gainblockShape.h"
#include "wx/wxsf/CommonFcn.h"

const wxRealPoint gainBlockShape[6]={wxRealPoint(0,80), wxRealPoint(80,40),
                                     wxRealPoint(0,0)
                                    };
size_t  namenumber = 0;
XS_IMPLEMENT_CLONABLE_CLASS(gainBlock, wxSFPolygonShape);

gainBlock::gainBlock()
: wxSFPolygonShape()
{
    Initialize();
	EnablePropertySerialization(wxT("vertices"), false);
	SetVertices(6, gainBlockShape);
}

gainBlock::gainBlock(const wxRealPoint& pos, wxSFDiagramManager* manager)
: wxSFPolygonShape(6, gainBlockShape, pos, manager)
{
	EnablePropertySerialization(wxT("vertices"), false);
}

gainBlock::gainBlock(const gainBlock& obj)
: wxSFPolygonShape(obj)
{

}

gainBlock::~gainBlock()
{

}

//----------------------------------------------------------------------------------//
// public virtual functions
//----------------------------------------------------------------------------------//

bool gainBlock::Contains(const wxPoint& pos)
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

const wxString gainBlock::getGain()
{
    return m_pGainNumber->GetText();
}
const wxString gainBlock::getNF()
{
    return m_pNFNumber->GetText();
}
const wxString gainBlock::getBlockName()
{
    return m_pName->GetText();
}
void gainBlock::setName(size_t number)
{
   	m_pName = new wxSFEditTextShape();
    // set some properties
    if(m_pName)
    {
        wxString namenumberstring = std::to_string(number);

        m_pName->SetRelativePosition({20,30});
		m_pName->SetStyle(sfsALWAYS_INSIDE | sfsHOVERING | sfsPROCESS_DEL | sfsPROPAGATE_DRAGGING | sfsPROPAGATE_SELECTION | sfsPROPAGATE_INTERACTIVE_CONNECTION );
		SF_ADD_COMPONENT( m_pName, wxT("name") );
        m_pName->SetText("G" + namenumberstring);
    }

}

void gainBlock::Initialize()
{
	// create associated shape(s)

    m_pGain = new wxSFEditTextShape();
    // set some properties
    if(m_pGain)
    {
        wxRect shpBB = this->GetBoundingBox();
        wxRealPoint txtpoint = (wxRealPoint(shpBB.GetLeft(), (shpBB.GetTop()+60) + shpBB.GetHeight()/2));
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
        wxRealPoint txtpoint = (wxRealPoint(shpBB.GetLeft()+50, (shpBB.GetTop()+60) + shpBB.GetHeight()/2));
        m_pGainNumber->SetRelativePosition(txtpoint);

		m_pGainNumber->SetStyle(sfsHOVERING | sfsPROCESS_DEL | sfsPROPAGATE_DRAGGING | sfsPROPAGATE_SELECTION | sfsPROPAGATE_INTERACTIVE_CONNECTION );

		SF_ADD_COMPONENT( m_pGainNumber, wxT("Gain") );

        m_pGainNumber->SetText("10dB");
    }


    m_pNF = new wxSFEditTextShape();
    // set some properties
    if(m_pNF)
    {
        wxRect shpBB = this->GetBoundingBox();
        wxRealPoint txtpoint = (wxRealPoint(shpBB.GetLeft(), (shpBB.GetTop()-50) + shpBB.GetHeight()/2));
        m_pNF->SetRelativePosition(txtpoint);

		m_pNF->SetStyle(sfsHOVERING | sfsPROCESS_DEL | sfsPROPAGATE_DRAGGING | sfsPROPAGATE_SELECTION | sfsPROPAGATE_INTERACTIVE_CONNECTION );

		SF_ADD_COMPONENT( m_pNF, wxT("NF") );
        m_pNF->SetText(" NF = ");
    }
     m_pNFNumber = new wxSFEditTextShape();
    // set some properties
    if(m_pNFNumber)
    {
        wxRect shpBB = this->GetBoundingBox();
        wxRealPoint txtpoint = (wxRealPoint(shpBB.GetLeft()+45, (shpBB.GetTop()-50) + shpBB.GetHeight()/2));
        m_pNFNumber->SetRelativePosition(txtpoint);

		m_pNFNumber->SetStyle(sfsHOVERING | sfsPROCESS_DEL | sfsPROPAGATE_DRAGGING | sfsPROPAGATE_SELECTION | sfsPROPAGATE_INTERACTIVE_CONNECTION );

		SF_ADD_COMPONENT( m_pNFNumber, wxT("NF") );
        m_pNFNumber->SetText("3dB");
    }
     wxSFShapeBase::AddConnectionPoint({95,50}, 0, false);
//     wxSFShapeBase::SetCustomDockPoint(1);
     wxSFShapeBase::AddConnectionPoint({0,20}, 1, true);
     wxSFShapeBase::AddConnectionPoint({0,80}, 1, true);


}


