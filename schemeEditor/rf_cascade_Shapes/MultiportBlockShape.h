#ifndef _MULTIPORTBLOCK_H
#define _MULTIPORTBLOCK_H
#include "wx/wxsf/wxShapeFramework.h"

#include <wx/wxsf/PolygonShape.h>

/*!
 * \brief Class encapsulating the diamond shape. It extends the basic polygon shape.
 * \sa wxSFPolygonShape
 */
class multiportBlock : public wxSFPolygonShape
{
public:
    XS_DECLARE_CLONABLE_CLASS(multiportBlock);

    /*! \brief Default constructor. */
    multiportBlock();
    /*!
	 * \brief User constructor.
     * \param pos Initial position
     * \param manager Pointer to parent diagram manager
     */
	multiportBlock(const wxRealPoint& pos, wxSFDiagramManager* manager);
	/*!
	 * \brief Copy constructor.
	 * \param obj Reference to the source object
	 */
	multiportBlock(const multiportBlock& obj);
	/*! \brief Destructor. */
    virtual ~multiportBlock();

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
    const wxString getNumberOfOutputPorts();
    const wxString getNumberOfInputPorts();
    const std::vector<std::pair<wxRealPoint,wxRealPoint>> getConnectioPoints();
private:
    wxSFEditTextShape *m_pName;

    wxSFEditTextShape *m_pGain;
    wxSFEditTextShape *m_pGainNumber;

    wxSFEditTextShape *m_pNF;
    wxSFEditTextShape *m_pNFNumber;

    wxSFEditTextShape *m_pOutputs;
    wxSFEditTextShape *m_pOutputsNumber;

    wxSFEditTextShape *m_pInputs;
    wxSFEditTextShape *m_pInputsNumber;

    wxString m_sDescription;
};
#endif
