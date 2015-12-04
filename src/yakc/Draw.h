#pragma once
//------------------------------------------------------------------------------
/**
    @class Draw
    @brief render the KC video memory via Oryol Gfx module
*/
#include "KC85Oryol.h"
#include "Gfx/Gfx.h"
#include "shaders.h"

class Draw {
public:
    /// setup the renderer
    void Setup(const Oryol::GfxSetup& setup, int frameSize);
    /// discard the renderer
    void Discard();
    /// render one frame
    void Render(const yakc::kc85& kc);

    /// apply view port to keep proper aspect ratio when fullscreen
    void applyViewport();
    /// restore fullscreen viewport
    void restoreViewport();

    Oryol::Id drawState;
    Oryol::Shaders::CRT::FSTextures fsTextures;
    Oryol::Shaders::CRT::FSParams fsParams;
    Oryol::ImageDataAttrs texUpdateAttrs;
    int frameSize = 0;

    Oryol::uint8 decodeBuffer[320 * 256 * 4];
};
