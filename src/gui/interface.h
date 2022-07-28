#pragma once
//-----------------------------------------------------------------------------
// mvUtilities
//
//     - This file contains typically platform specific functions. May need
//       to rename to a more appropriate name.
//     
//-----------------------------------------------------------------------------

#include <vector>
#include <string>
#include <cstdint>
#include <unordered_map>
#include <memory>

#ifndef PyObject_HEAD
struct _object;
typedef _object PyObject;
#endif

namespace BlendArMocapGUI
{
    void RenderUI();
    void CVTexturePanel();
    void InputConfigPanel();
    void OutputConfigPanel();
    void CreateRawTexture(); ///format?

    void* LoadTextureFromArrayRaw(std::uint32_t width, std::uint32_t height, float* data, std::int32_t components);
    void  UpdateRawTexture(void* texture, std::uint32_t width, std::uint32_t height, float* data, std::int32_t components);
}

