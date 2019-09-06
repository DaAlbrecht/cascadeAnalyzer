#ifndef _FRAMECANVAS_H
#define _FRAMECANVAS_H

#include "wx/wxsf/wxShapeFramework.h"
#include "rf_cascade_Shapes/ConnectionLineShape.h"
#include "rf_cascade_Shapes/gainblockShape.h"
#include "rf_cascade_Shapes/MultiportBlockShape.h"
class MainFrm;
class FrameCanvas : public wxSFShapeCanvas
{
public:

    FrameCanvas(){;}
	FrameCanvas(wxSFDiagramManager* manager, wxWindow* parent, wxWindowID id);
	virtual ~FrameCanvas(void);
	// public virtual functions
	virtual void OnLeftDown(wxMouseEvent& event);
	virtual void OnKeyDown(wxKeyEvent& event);
	virtual void OnConnectionFinished(wxSFLineShape* connection);
	virtual PRECONNECTIONFINISHEDSTATE OnPreConnectionFinished(wxSFLineShape* connection);
    virtual void OnMouseWheel(wxMouseEvent& event);
    virtual void OnTextChanged(wxSFShapeTextEvent& event);

    wxSFDiagramManager m_Manager;
protected:
	// protected data members
	MainFrm *m_pParentFrame;
};

#endif //_FRAMECANVAS_H
