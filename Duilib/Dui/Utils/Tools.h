#ifndef DUILIB_UTILS_TOOLS_H
#define DUILIB_UTILS_TOOLS_H
#include "Windef.h"

namespace DuiLib {
    bool OverlapRect(RECT& overlap, const RECT& src1, const RECT& src2);
}

#endif