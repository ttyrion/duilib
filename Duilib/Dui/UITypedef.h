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

    typedef struct DUILIB_API
    {
        std::vector<char> yuv[3];
        UINT width = 0;
        UINT height = 0;
        bool empty() const {
            return width == 0 || height == 0 || yuv[0].empty() || yuv[1].empty() || yuv[2].empty();
        }

        void clear() {
            for (auto& item : yuv) {
                item.clear();
            }

            width = 0;
            height = 0;
        }

    } VideoFrame;
}

