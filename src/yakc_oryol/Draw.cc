//------------------------------------------------------------------------------
//  Draw.cc
//------------------------------------------------------------------------------
#include "Draw.h"
#include "yakc_shader.h"

using namespace Oryol;

namespace YAKC {

//------------------------------------------------------------------------------
void
Draw::Setup(const GfxDesc& gfxDesc, int frame_x, int frame_y) {
    this->crtEffectEnabled = false;
    this->crtColorEnabled = true;
    this->frameSizeX = frame_x;
    this->frameSizeY = frame_y;
    this->imageContent.Size[0][0] = 0;

    const float quadVerts[] = { 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f };
    Id fsq = Gfx::CreateBuffer(BufferDesc().Size(sizeof(quadVerts)).Content(quadVerts));
    this->crtDrawState.VertexBuffers[0] = fsq;
    this->nocrtDrawState.VertexBuffers[0] = fsq;

    Id crtShd = Gfx::CreateShader(CRTShader::Desc());
    Id nocrtShd = Gfx::CreateShader(NoCRTShader::Desc());
    auto pipDesc = PipelineDesc()
        .Layout(0, { { "position", VertexFormat::Float2 } })
        .PrimitiveType(PrimitiveType::TriangleStrip)
        .DepthWriteEnabled(false)
        .DepthCmpFunc(CompareFunc::Always)
        .ColorFormat(gfxDesc.ColorFormat())
        .DepthFormat(gfxDesc.DepthFormat())
        .SampleCount(gfxDesc.SampleCount());
    this->crtDrawState.Pipeline = Gfx::CreatePipeline(pipDesc.Shader(crtShd));
    this->nocrtDrawState.Pipeline = Gfx::CreatePipeline(pipDesc.Shader(nocrtShd));
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
    this->imageContent.Size[0][0] = width*height*4;
    this->imageContent.Pointer[0][0] = pixels;
    Gfx::UpdateTexture(this->texture, this->imageContent);
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
    Gfx::Draw(0, 4);
    this->restoreViewport();
}

//------------------------------------------------------------------------------
void
Draw::applyViewport(int width, int height) {
    float aspect = float(width) / float(height);
    if (aspect > (5.0f/4.0f)) {
        aspect = (5.0f/4.0f);
    }
    const int fbWidth = Gfx::DisplayAttrs().Width;
    const int fbHeight = Gfx::DisplayAttrs().Height;
    int viewPortY = this->frameSizeY;
    int viewPortH = fbHeight - 2*frameSizeY;
    int viewPortW = (const int) (fbHeight * aspect) - 2*this->frameSizeX;
    int viewPortX = (fbWidth - viewPortW) / 2;
    Gfx::ApplyViewPort(viewPortX, viewPortY, viewPortW, viewPortH);
}

//------------------------------------------------------------------------------
void
Draw::restoreViewport() {
    const int fbWidth = Gfx::DisplayAttrs().Width;
    const int fbHeight = Gfx::DisplayAttrs().Height;
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
            Gfx::PushResourceLabel('YAKC');
            this->texture = Gfx::CreateTexture(TextureDesc()
                .Type(TextureType::Texture2D)
                .Width(width)
                .Height(height)
                .NumMipMaps(1)
                .Format(PixelFormat::RGBA8)
                .Usage(Usage::Stream)
                .MinFilter(TextureFilterMode::Linear)
                .MagFilter(TextureFilterMode::Linear)
                .WrapU(TextureWrapMode::ClampToEdge)
                .WrapV(TextureWrapMode::ClampToEdge));
            Gfx::PopResourceLabel();
        }
    }
}

} // namespace YAKC
