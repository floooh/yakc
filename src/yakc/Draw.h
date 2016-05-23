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
    void Render(const void* pixels, int width, int height);
    /// update rendering parameters
    void UpdateParams(bool enableCrtEffect, bool colorTV, const glm::vec2& crtWarp);

    /// apply view port to keep proper aspect ratio when fullscreen
    void applyViewport(int width, int height);
    /// restore fullscreen viewport
    void restoreViewport();

    bool crtEffectEnabled;

    Oryol::Id irmTexture320x256;
    Oryol::Id irmTexture256x256;
    Oryol::DrawState crtDrawState;
    Oryol::CRTShader::FSParams crtFsParams;

    Oryol::DrawState nocrtDrawState;
    Oryol::NoCRTShader::FSParams nocrtFsParams;

    Oryol::ImageDataAttrs texUpdateAttrs;
    int frameSize = 0;
};
