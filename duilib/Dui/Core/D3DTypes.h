#pragma once
#include <DirectXMath.h>

namespace DuiLib {

    typedef struct {
        DirectX::XMFLOAT3    pos;           // vertex untransformed position
        DirectX::XMFLOAT4    color;
    } COLOR_VERTEX;

} // namespace DuiLib
