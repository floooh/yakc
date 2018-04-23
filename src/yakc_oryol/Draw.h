#pragma once
//------------------------------------------------------------------------------
/**
    @class YAKC::Draw
    @brief render the KC video memory via Oryol Gfx module
*/
#include "Gfx/Gfx.h"
#include "glm/vec2.hpp"

namespace YAKC {

class Draw {
public:
    /// setup the renderer
    void Setup(const Oryol::GfxDesc& desc, int frameSizeX, int frameSizeY);
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

    /// check if texture size matches, if no, discard old one, create new one
    void validateTexture(int width, int height);

    bool crtEffectEnabled = false;
    bool crtColorEnabled = true;
    glm::vec2 crtWarp;

    int texWidth = 0;
    int texHeight = 0;
    Oryol::Id texture;
    Oryol::DrawState crtDrawState;
    Oryol::DrawState nocrtDrawState;

    Oryol::ImageContent imageContent;
    int frameSizeX = 0;
    int frameSizeY = 0;
};

} // namespace YAKC

