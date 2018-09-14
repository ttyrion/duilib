#include "stdafx.h"
#include "Tools.h"

namespace DuiLib {
    bool OverlapRect(RECT& overlap, const RECT& src1, const RECT& src2) {
        if (::IntersectRect(&overlap, &src1, &src2)) {
            return true;
        }

        //overlap rect
        if (src1.left >= src2.left      &&
            src1.top >= src2.top        &&
            src1.right <= src2.right    &&
            src1.bottom <= src2.bottom 
            ) {
            overlap = src1;
            return true;
        }
        else if (src1.left <= src2.left      &&
                 src1.top <= src2.top        &&
                 src1.right >= src2.right    &&
                 src1.bottom >= src2.bottom
            ) {
            overlap = src2;
            return true;
        }
        
        return false;
    }

    bool IsSubRect(const RECT& parent, const RECT& sub) {
        return parent.left < sub.left && parent.top < sub.top && parent.right > sub.right && parent.bottom > sub.bottom;
    }
}