//------------------------------------------------------------------------------
//  Draw.cc
//------------------------------------------------------------------------------
#include "Draw.h"
#include "yakc_shaders.h"

using namespace Oryol;

namespace YAKC {

//------------------------------------------------------------------------------
void
Draw::Setup(const GfxSetup& gfxSetup, int frame) {
    this->crtEffectEnabled = false;
    this->crtColorEnabled = true;
    this->frameSize = frame;
    this->texUpdateAttrs.NumFaces = 1;
    this->texUpdateAttrs.NumMipMaps = 1;
    this->texUpdateAttrs.Sizes[0][0] = 320*256*4;

    auto irmSetup = TextureSetup::Empty(320, 256, 1, TextureType::Texture2D, PixelFormat::RGBA8, Usage::Stream);
    irmSetup.TextureUsage = Usage::Stream;
    irmSetup.Sampler.MinFilter = TextureFilterMode::Linear;
    irmSetup.Sampler.MagFilter = TextureFilterMode::Linear;
    irmSetup.Sampler.WrapU = TextureWrapMode::ClampToEdge;
    irmSetup.Sampler.WrapV = TextureWrapMode::ClampToEdge;
    this->irmTexture320x256 = Gfx::CreateResource(irmSetup);
    irmSetup.Height = 192;
    this->irmTexture320x192 = Gfx::CreateResource(irmSetup);
    irmSetup.Width = 256;
    irmSetup.Height = 256;
    this->irmTexture256x256 = Gfx::CreateResource(irmSetup);

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
    this->crtColorEnabled = colorTV;
    this->crtWarp = warp;
}

//------------------------------------------------------------------------------
void
Draw::Render(const void* pixels, int width, int height) {
    o_trace_scoped(yakc_draw);

    // copy decoded RGBA8 into texture
    Id tex;
    if ((320 == width) && (256 == height)) {
        tex = this->irmTexture320x256;
    }
    else if ((320 == width) && (192 == height)) {
        tex = this->irmTexture320x192;
    }
    else if ((256 == width) && (256 == height)) {
        tex = this->irmTexture256x256;
    }
    else {
        o_error("Draw::Render(): invalid display size!\n");
    }
    this->crtDrawState.FSTexture[YAKCTextures::IRM] = tex;
    this->nocrtDrawState.FSTexture[YAKCTextures::IRM] = tex;
    this->texUpdateAttrs.Sizes[0][0] = width*height*4;
    Gfx::UpdateTexture(tex, pixels, this->texUpdateAttrs);
    this->applyViewport(width, height);
    if (this->crtEffectEnabled) {
        Oryol::CRTShader::YAKCFSParams fsParams;
        fsParams.ColorTV = this->crtColorEnabled;
        fsParams.CRTWarp = this->crtWarp;
        Gfx::ApplyDrawState(this->crtDrawState);
        Gfx::ApplyUniformBlock(fsParams);
    }
    else {
        Oryol::NoCRTShader::YAKCFSParams fsParams;
        fsParams.ColorTV = this->crtColorEnabled;
        fsParams.CRTWarp = this->crtWarp;
        Gfx::ApplyDrawState(this->nocrtDrawState);
        Gfx::ApplyUniformBlock(fsParams);
    }
    Gfx::Draw(0);
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
    int viewPortY = this->frameSize;
    int viewPortH = fbHeight - 2*frameSize;
    int viewPortW = (const int) (fbHeight * aspect) - 2*this->frameSize;
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

} // namespace YAKC
