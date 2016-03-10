//------------------------------------------------------------------------------
//  Draw.cc
//------------------------------------------------------------------------------
#include "Draw.h"

using namespace Oryol;
using namespace yakc;

//------------------------------------------------------------------------------
void
Draw::Setup(const GfxSetup& gfxSetup, int frame) {
    this->crtEffectEnabled = false;
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
    this->irmTexture = Gfx::CreateResource(irmSetup);
    this->crtDrawState.FSTexture[Textures::IRM] = irmTexture;
    this->nocrtDrawState.FSTexture[Textures::IRM] = irmTexture;

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

    this->crtFsParams.ColorTV = true;
    this->nocrtFsParams.ColorTV = true;
}

//------------------------------------------------------------------------------
void
Draw::Discard() {
    // nothing to do here
}

//------------------------------------------------------------------------------
void
Draw::UpdateParams(bool enableCrtEffect, bool colorTV, const glm::vec2& crtWarp) {
    this->crtEffectEnabled = enableCrtEffect;
    this->crtFsParams.ColorTV = this->nocrtFsParams.ColorTV = colorTV;
    this->crtFsParams.CRTWarp = this->nocrtFsParams.CRTWarp = crtWarp;
}

//------------------------------------------------------------------------------
void
Draw::Render(const kc85& kc) {
    o_trace_scoped(yakc_draw);

    // copy decoded RGBA8 into texture
    Gfx::UpdateTexture(this->irmTexture, kc.video.LinearBuffer, this->texUpdateAttrs);
    this->applyViewport();
    if (this->crtEffectEnabled) {
        Gfx::ApplyDrawState(this->crtDrawState);
        Gfx::ApplyUniformBlock(this->crtFsParams);
    }
    else {
        Gfx::ApplyDrawState(this->nocrtDrawState);
        Gfx::ApplyUniformBlock(this->nocrtFsParams);
    }
    Gfx::Draw(0);
    this->restoreViewport();
}

//------------------------------------------------------------------------------
void
Draw::applyViewport() {
    float aspect = float(320) / float(256);
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
