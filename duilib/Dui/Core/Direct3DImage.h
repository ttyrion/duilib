#pragma once
#include "D3DTypes.h"

namespace DuiLib {
    class DUILIB_API Direct3DImage
    {
    public:
        Direct3DImage();
        ~Direct3DImage();

        static void LoadImage(const CDuiString& file, const CDuiString& type, DWORD mask, ImageData& image);
    };
} // namespace DuiLib
