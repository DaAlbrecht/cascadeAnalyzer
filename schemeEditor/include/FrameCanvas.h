#ifndef _FRAMECANVAS_H
#define _FRAMECANVAS_H

#include "wx/wxsf/wxShapeFramework.h"
#include "ConnectionLineShape.h"
#include "gainblockShape.h"
#include "MultiportBlockShape.h"
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
