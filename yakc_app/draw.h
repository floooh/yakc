#pragma once
//------------------------------------------------------------------------------
/**
    @class draw
    @brief render the KC video memory via Oryol Gfx module
*/
#include "Gfx/Gfx.h"
#include "yakc_core/kc85.h"
#include "shaders.h"

class draw {
public:
    /// setup the renderer
    void setup(const Oryol::GfxSetup& setup);
    /// discard the renderer
    void discard();
    /// render one frame
    void render(const yakc::kc85& kc);
    /// decode video memory into linear RGBA8 buffer
    void decode(const yakc::kc85& kc);

    Oryol::Id drawState;
    Oryol::Shaders::CRT::FSTextures fsTextures;
    Oryol::ImageDataAttrs texUpdateAttrs;

    static const int irmWidth = 320;
    static const int irmHeight = 256;
    Oryol::uint32 irmBuffer[irmHeight][irmWidth];
};
