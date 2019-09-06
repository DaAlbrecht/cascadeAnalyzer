#ifndef IDS_H_INCLUDED
#define IDS_H_INCLUDED

enum APPIDS
{
    // menu IDs
    //---------------------------------------------------------------//
    IDM_SAVEASBITMAP = wxID_HIGHEST + 1,

    // tool IDs
    //---------------------------------------------------------------//
    IDT_FIRST_TOOLMARKER,

    IDT_GRID,
    IDT_SHADOW,
	IDT_GC,
    IDT_TOOL,
    IDT_GAINBLOCKSHP,
    IDT_MULTIPORTBLOCK,
    IDT_BITMAPSHP,
    IDT_GRIDSHP,
    IDT_FLEXGRIDSHP,
	IDT_ORTHOSHP,
    IDT_LAST_TOOLMARKER,
	IDM_AUTOLAYOUT_FIRST,
	IDM_AUTOLAYOUT_LAST = IDM_AUTOLAYOUT_FIRST + 100,

    // other constrols
    //---------------------------------------------------------------//
    IDT_COLORPICKER
};

#endif // IDS_H_INCLUDED
