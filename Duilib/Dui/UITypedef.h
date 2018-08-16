#pragma once
#include "Utils\Utils.h"

namespace DuiLib {
    typedef struct DUILIB_API tagTFontInfo
    {
        HFONT hFont;
        CDuiString sFontName;
        int iSize;
        bool bBold;
        bool bUnderline;
        bool bItalic;
        TEXTMETRIC tm;
    } TFontInfo;
}

