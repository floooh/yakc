//------------------------------------------------------------------------------
//  Draw.cc
//------------------------------------------------------------------------------
#include "Draw.h"
#include "yakc_shaders.h"

using namespace Oryol;

namespace YAKC {

//------------------------------------------------------------------------------
void
Draw::Setup(const GfxSetup& gfxSetup, int frame_x, int frame_y) {
    this->crtEffectEnabled = false;
    this->crtColorEnabled = true;
    this->frameSizeX = frame_x;
    this->frameSizeY = frame_y;
    this->texUpdateAttrs.NumFaces = 1;
    this->texUpdateAttrs.NumMipMaps = 1;
    this->texUpdateAttrs.Sizes[0][0] = 0;

    auto fsqSetup = MeshSetup::FullScreenQuad(true);
    Id fsq = Gfx::CreateResource(fsqSetup);
    this->crtDrawState.Mesh[0] = fsq;
    this->nocrtDrawState.Mesh[0] = fsq;

    Id crtShd = Gfx::CreateResource(CRTShader::Setup());
    Id nocrtShd = Gfx::CreateResource(NoCRTShader::Setup());
    auto pips = PipelineSetup::FromLayoutAndShader(fsqSetup.Layout, crtShd);
    pips.DepthStencilState.DepthWriteEnabled = false;
    pips.DepthStencilState.DepthCmpFunc = CompareFunc::Always;
    pips.BlendState.ColorFormat = gfxSetup.ColorFormat;
    pips.BlendState.DepthFormat = gfxSetup.DepthFormat;
    pips.RasterizerState.SampleCount = gfxSetup.SampleCount;
    this->crtDrawState.Pipeline = Gfx::CreateResource(pips);
    pips.Shader = nocrtShd;
    this->nocrtDrawState.Pipeline = Gfx::CreateResource(pips);
}

//------------------------------------------------------------------------------
void
Draw::Discard() {
    // nothing to do here
}

//------------------------------------------------------------------------------
void
Draw::UpdateParams(bool enableCrtEffect, bool colorTV, const glm::vec2& warp) {
    this->crtEffectEnabled = enableCrtEffect;
    this->crtColorEnabled = colorTV ? 1.0f : 0.0f;
    this->crtWarp = warp;
}

//------------------------------------------------------------------------------
void
Draw::Render(const void* pixels, int width, int height) {

    // create new texture is size mismatch
    this->validateTexture(width, height);
    if (!this->texture.IsValid()) {
        // width or height 0 (can happen if emulator is switched off)
        return;
    }
    this->crtDrawState.FSTexture[CRTShader::irm] = this->texture;
    this->nocrtDrawState.FSTexture[NoCRTShader::irm] = this->texture;
    this->texUpdateAttrs.Sizes[0][0] = width*height*4;
    Gfx::UpdateTexture(this->texture, pixels, this->texUpdateAttrs);
    this->applyViewport(width, height);
    if (this->crtEffectEnabled) {
        CRTShader::fsParams fsParams;
        fsParams.colorTV = this->crtColorEnabled;
        fsParams.crtWarp = this->crtWarp;
        Gfx::ApplyDrawState(this->crtDrawState);
        Gfx::ApplyUniformBlock(fsParams);
    }
    else {
        NoCRTShader::fsParams fsParams;
        fsParams.colorTV = this->crtColorEnabled;
        fsParams.crtWarp = this->crtWarp;
        Gfx::ApplyDrawState(this->nocrtDrawState);
        Gfx::ApplyUniformBlock(fsParams);
    }
    Gfx::Draw();
    this->restoreViewport();
}

//------------------------------------------------------------------------------
void
Draw::applyViewport(int width, int height) {
    float aspect = float(width) / float(height);
    if (aspect > (5.0f/4.0f)) {
        aspect = (5.0f/4.0f);
    }
    const int fbWidth = Gfx::DisplayAttrs().FramebufferWidth;
    const int fbHeight = Gfx::DisplayAttrs().FramebufferHeight;
    int viewPortY = this->frameSizeY;
    int viewPortH = fbHeight - 2*frameSizeY;
    int viewPortW = (const int) (fbHeight * aspect) - 2*this->frameSizeX;
    int viewPortX = (fbWidth - viewPortW) / 2;
    Gfx::ApplyViewPort(viewPortX, viewPortY, viewPortW, viewPortH);
}

//------------------------------------------------------------------------------
void
Draw::restoreViewport() {
    const int fbWidth = Gfx::DisplayAttrs().FramebufferWidth;
    const int fbHeight = Gfx::DisplayAttrs().FramebufferHeight;
    Gfx::ApplyViewPort(0, 0, fbWidth, fbHeight);
}

//------------------------------------------------------------------------------
void
Draw::validateTexture(int width, int height) {
    if ((this->texWidth != width) || (this->texHeight != height)) {

        this->texWidth = width;
        this->texHeight = height;

        // if a texture exists, discard it
        if (this->texture.IsValid()) {
            Gfx::DestroyResources('YAKC');
            this->texture.Invalidate();
        }

        // only create a new texture if width and height > 0
        if ((width > 0) && (height > 0)) {
            auto texSetup = TextureSetup::Empty2D(width, height, 1, PixelFormat::RGBA8, Usage::Stream);
            texSetup.Sampler.MinFilter = TextureFilterMode::Linear;
            texSetup.Sampler.MagFilter = TextureFilterMode::Linear;
            texSetup.Sampler.WrapU = TextureWrapMode::ClampToEdge;
            texSetup.Sampler.WrapV = TextureWrapMode::ClampToEdge;
            Gfx::PushResourceLabel('YAKC');
            this->texture = Gfx::CreateResource(texSetup);
            Gfx::PopResourceLabel();
        }
    }
}

} // namespace YAKC
