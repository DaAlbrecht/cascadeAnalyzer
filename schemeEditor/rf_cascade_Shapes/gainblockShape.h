#ifndef _GAINBLOCKSHAPE_H
#define _GAINBLOCKSHAPE_H
#include "wx/wxsf/wxShapeFramework.h"

#include <wx/wxsf/PolygonShape.h>

/*!
 * \brief Class encapsulating the diamond shape. It extends the basic polygon shape.
 * \sa wxSFPolygonShape
 */
class gainBlock : public wxSFPolygonShape
{
public:

    enum CONNECTIONS{
    OUTPUT,
    INPUT
    };

    XS_DECLARE_CLONABLE_CLASS(gainBlock);

    /*! \brief Default constructor. */
    gainBlock();
    /*!
	 * \brief User constructor.
     * \param pos Initial position
     * \param manager Pointer to parent diagram manager
     */
	gainBlock(const wxRealPoint& pos, wxSFDiagramManager* manager);
	/*!
	 * \brief Copy constructor.
	 * \param obj Reference to the source object
	 */
	gainBlock(const gainBlock& obj);
	/*! \brief Destructor. */
    virtual ~gainBlock();

    // public virtual functions
    /*!
	 * \brief Test whether the given point is inside the shape. The function
     * can be overrided if neccessary.
     * \param pos Examined point
     * \return TRUE if the point is inside the shape area, otherwise FALSE
     */
    void OnTextChanged();
    virtual bool Contains(const wxPoint& pos);
    void Initialize();
    void setName(size_t number);
    const wxString getGain();
    const wxString getNF();
    const wxString getBlockName();
    CONNECTIONS findConnectionPoint(wxRealPoint &Point);
private:
    std::vector<std::pair<wxSFConnectionPoint*,CONNECTIONS>> connectionVec;
    wxSFEditTextShape *m_pName;
    wxSFEditTextShape *m_pGain;
    wxSFEditTextShape *m_pGainNumber;
    wxSFEditTextShape *m_pNF;
    wxSFEditTextShape *m_pNFNumber;
    wxString m_sDescription;


};
#endif
