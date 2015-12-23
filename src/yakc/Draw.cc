//------------------------------------------------------------------------------
//  Draw.cc
//------------------------------------------------------------------------------
#include "Draw.h"

using namespace Oryol;
using namespace yakc;

//------------------------------------------------------------------------------
void
Draw::Setup(const GfxSetup& gfxSetup, int frame) {
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
    this->fsTextures.IRM = Gfx::CreateResource(irmSetup);

    Id msh = Gfx::CreateResource(MeshSetup::FullScreenQuad(true));
    Id shd = Gfx::CreateResource(Shaders::CRT::Setup());
    auto dss = DrawStateSetup::FromMeshAndShader(msh, shd);
    dss.DepthStencilState.DepthWriteEnabled = false;
    dss.DepthStencilState.DepthCmpFunc = CompareFunc::Always;
    dss.BlendState.ColorFormat = gfxSetup.ColorFormat;
    dss.BlendState.DepthFormat = gfxSetup.DepthFormat;
    dss.RasterizerState.SampleCount = gfxSetup.SampleCount;
    this->drawState = Gfx::CreateResource(dss);

    this->fsParams.CRTEffect = true;
    this->fsParams.ColorTV = true;
}

//------------------------------------------------------------------------------
void
Draw::Discard() {
    // nothing to do here
}

//------------------------------------------------------------------------------
void
Draw::Render(const kc85& kc) {
    o_trace_scoped(yakc_draw);

    // copy decoded RGBA8 into texture
    Gfx::UpdateTexture(this->fsTextures.IRM, kc.video.LinearBuffer, this->texUpdateAttrs);
    this->applyViewport();
    Gfx::ApplyDrawState(this->drawState, this->fsTextures);
    Gfx::ApplyUniformBlock(this->fsParams);
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
