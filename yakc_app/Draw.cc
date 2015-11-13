//------------------------------------------------------------------------------
//  Draw.cc
//------------------------------------------------------------------------------
#include "Draw.h"

using namespace Oryol;
using namespace yakc;

//------------------------------------------------------------------------------
void
Draw::Setup(const GfxSetup& gfxSetup) {

    this->texUpdateAttrs.NumFaces = 1;
    this->texUpdateAttrs.NumMipMaps = 1;
    this->texUpdateAttrs.Sizes[0][0] = sizeof(this->irmBuffer);

    auto irmSetup = TextureSetup::Empty(irmWidth, irmHeight, 1, TextureType::Texture2D, PixelFormat::RGBA8, Usage::Stream);
    irmSetup.TextureUsage = Usage::Stream;
    irmSetup.Sampler.MinFilter = TextureFilterMode::Nearest;
    irmSetup.Sampler.MagFilter = TextureFilterMode::Nearest;
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
}

//------------------------------------------------------------------------------
void
Draw::Discard() {
    // nothing to do here
}

//------------------------------------------------------------------------------
void
Draw::Render(const kc85& kc) {
    this->DecodeVideoMemory(kc);
    Gfx::UpdateTexture(this->fsTextures.IRM, this->irmBuffer, this->texUpdateAttrs);
    this->applyViewport();
    Gfx::ApplyDrawState(this->drawState, this->fsTextures);
    Gfx::Draw(0);
    this->restoreViewport();
}

//------------------------------------------------------------------------------
void
Draw::applyViewport() {
    float aspect = float(320) / float(256);
    const int fbWidth = Gfx::DisplayAttrs().FramebufferWidth;
    const int fbHeight = Gfx::DisplayAttrs().FramebufferHeight;
    int viewPortY = 0;
    int viewPortH = fbHeight;
    int viewPortW = (const int) (fbHeight * aspect);
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
static uint32 fgPalette[16] = {
    // foreground colors, pixel channel order is ABGR
    // see: http://www.mpm-kc85.de/html/CAOS_42.htm
    0xFF000000,     // black
    0xFFFF0000,     // blue
    0xFF0000FF,     // red
    0xFFFF00FF,     // magenta
    0xFF00FF00,     // green
    0xFFFFFF00,     // cyan
    0xFF00FFFF,     // yellow
    0xFFFFFFFF,     // white

    0xFF000000,     // black #2
    0xFFFF00A0,     // violet
    0xFF00A0FF,     // orange
    0xFFA000FF,     // purple
    0xFFA0FF00,     // blueish green
    0xFFFFA000,     // greenish blue
    0xFF00FFA0,     // yellow-green
    0xFFFFFFFF,     // white #2
};

//------------------------------------------------------------------------------
static uint32 bgPalette[8] = {
    // background colors
    0xFF000000,     // dark-black
    0xFFA00000,     // dark-blue
    0xFF0000A0,     // dark-red
    0xFFA000A0,     // dark-magenta
    0xFF00A000,     // dark-green
    0xFFA0A000,     // dark-cyan
    0xFF00A0A0,     // dark-yellow
    0xFFA0A0A0,     // gray
};

//------------------------------------------------------------------------------
static void
Draw8Pixels(uint32* ptr, ubyte pixels, ubyte colors, bool blinkBg) {

    // select foreground- and background color:
    //  bit 7: blinking
    //  bits 6..3: foreground color
    //  bits 2..0: background color
    //
    // index 0 is background color, index 1 is foreground color
    ubyte bg_index = colors & 0x7;
    ubyte fg_index = (colors>>3)&0xF;
    uint32 bg = bgPalette[bg_index];
    uint32 fg;
    if (blinkBg & (colors & 0x80)) {
        fg = bg;
    }
    else {
        fg = fgPalette[fg_index];
    }
    ptr[0] = pixels & 0x80 ? fg : bg;
    ptr[1] = pixels & 0x40 ? fg : bg;
    ptr[2] = pixels & 0x20 ? fg : bg;
    ptr[3] = pixels & 0x10 ? fg : bg;
    ptr[4] = pixels & 0x08 ? fg : bg;
    ptr[5] = pixels & 0x04 ? fg : bg;
    ptr[6] = pixels & 0x02 ? fg : bg;
    ptr[7] = pixels & 0x01 ? fg : bg;
};

//------------------------------------------------------------------------------
void
Draw::DecodeVideoMemory(const kc85& kc) {

    bool blink_off = !kc.blink_state();
    if (kc.model() == kc85::kc_model::kc85_3) {
        const uint8* pixel_data = kc.irm0;
        const uint8* color_data = kc.irm0 + 0x2800;

        int pixel_offset;
        int color_offset;
        for (int y = 0; y < irmHeight; y++) {

            int left_pixel_offset  = (((y>>2)&0x3)<<5) | ((y&0x3)<<7) | (((y>>4)&0xF)<<9);
            int left_color_offset  = (((y>>2)&0x3f)<<5);
            int right_pixel_offset = (((y>>4)&0x3)<<3) | (((y>>2)&0x3)<<5) | ((y&0x3)<<7) | (((y>>6)&0x3)<<9);
            int right_color_offset = (((y>>4)&0x3)<<3) | (((y>>2)&0x3)<<5) | (((y>>6)&0x3)<<7);

            uint32* dst_ptr = &(this->irmBuffer[y][0]);

            // x is not per-pixel, but per byte
            const int width = irmWidth >> 3;
            for (int x = 0; x < width; x++) {
                if (x < 0x20) {
                    // left 256x256 quad
                    pixel_offset = x | left_pixel_offset;
                    color_offset = x | left_color_offset;
                }
                else {
                    // right 64x256 strip
                    pixel_offset = 0x2000 + ((x&0x7) | right_pixel_offset);
                    color_offset = 0x0800 + ((x&0x7) | right_color_offset);
                }
                ubyte src_pixels = pixel_data[pixel_offset];
                ubyte src_colors = color_data[color_offset];
                Draw8Pixels(&(dst_ptr[x<<3]), src_pixels, src_colors, blink_off);
            }
        }
    }
}
