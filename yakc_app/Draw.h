#pragma once
//------------------------------------------------------------------------------
/**
    @class Draw
    @brief render the KC video memory via Oryol Gfx module
*/
#include "Gfx/Gfx.h"
#include "KC85Oryol.h"
#include "shaders.h"

class Draw {
public:
    /// setup the renderer
    void Setup(const Oryol::GfxSetup& setup);
    /// discard the renderer
    void Discard();
    /// render one frame
    void Render(const yakc::kc85& kc);
    /// decode video memory into linear RGBA8 buffer
    void DecodeVideoMemory(const yakc::kc85& kc);

    /// apply view port to keep proper aspect ratio when fullscreen
    void applyViewport();
    /// restore fullscreen viewport
    void restoreViewport();

    Oryol::Id drawState;
    Oryol::Shaders::CRT::FSTextures fsTextures;
    Oryol::ImageDataAttrs texUpdateAttrs;

    static const int irmWidth = 320;
    static const int irmHeight = 256;
    Oryol::uint32 irmBuffer[irmHeight][irmWidth];
};
