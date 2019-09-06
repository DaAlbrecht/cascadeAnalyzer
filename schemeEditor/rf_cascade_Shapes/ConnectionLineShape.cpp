#ifdef _DEBUG_MSVC
#define new DEBUG_NEW
#endif
#include <algorithm>
#include <math.h>
#include "ConnectionLineShape.h"
#include "gainBlockShape.h"
#include "wx/wxsf/ShapeCanvas.h"
#include "wx/wxsf/CommonFcn.h"
#include "wx_pch.h"


XS_IMPLEMENT_CLONABLE_CLASS(ConnectionLineShape, wxSFOrthoLineShape);

ConnectionLineShape::ConnectionLineShape() : wxSFOrthoLineShape()
{
}

ConnectionLineShape::ConnectionLineShape(long src, long trg, const wxXS::RealPointList& path, wxSFDiagramManager* manager)
: wxSFOrthoLineShape(src, trg, path, manager)
{
}

ConnectionLineShape::ConnectionLineShape(const ConnectionLineShape& obj)
: wxSFOrthoLineShape(obj)
{
}

ConnectionLineShape::~ConnectionLineShape()
{

}





