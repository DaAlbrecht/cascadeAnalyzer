#ifndef _CONNECTIONLINESHAPE_H
#define _CONNECTIONLINESHAPE_H

#include <wx/wxsf/OrthoShape.h>

/*!
 * \brief Orthogonal line shape. The class extends wxSFLineShape class and allows
 * user to create connection line orthgonal to base axis.
 */
class ConnectionLineShape : public wxSFOrthoLineShape
{
public:
    XS_DECLARE_CLONABLE_CLASS(ConnectionLineShape);
    ConnectionLineShape();
    ConnectionLineShape(long src, long trg, const wxXS::RealPointList& path, wxSFDiagramManager* manager);
	ConnectionLineShape(const ConnectionLineShape& obj);
    void LineEvent();
    virtual ~ConnectionLineShape();
};

#endif //_CONNECTIONLINESHAPE_H
