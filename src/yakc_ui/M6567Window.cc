//------------------------------------------------------------------------------
//  M6567Window.cc
//------------------------------------------------------------------------------
#include "M6567Window.h"
#include "IMUI/IMUI.h"
#include "Util.h"
#include "yakc/util/breadboard.h"

using namespace Oryol;

namespace YAKC {

//------------------------------------------------------------------------------
void
M6567Window::Setup(yakc& emu) {
    this->setName("M6567 (VIC-II) State");
    for (int i = 0; i < 16; i++) {
        this->paletteColors[i] = Util::RGBA8toImVec4(m6567_color(i));
    }
}

//------------------------------------------------------------------------------
void
M6567Window::drawColor(const char* text, uint8_t palIndex) {
    ImGui::Text("%s%02X", text, palIndex);
    ImGui::SameLine();
    ImGui::ColorButton(text, this->paletteColors[palIndex&0xF], ImGuiColorEditFlags_NoAlpha, ImVec2(12, 12));
}

//------------------------------------------------------------------------------
#define UINT8_BITS(m) m&(1<<7)?'1':'0',m&(1<<6)?'1':'0',m&(1<<5)?'1':'0',m&(1<<4)?'1':'0',m&(1<<3)?'1':'0',m&(1<<2)?'1':'0',m&(1<<1)?'1':'0',m&(1<<0)?'1':'0'
bool
M6567Window::Draw(yakc& emy) {
    ImGui::SetNextWindowSize(ImVec2(380, 460), ImGuiSetCond_Once);
    if (ImGui::Begin(this->title.AsCStr(), &this->Visible)) {
        m6567_t& vic = board.m6567;
        const char* type = "unknown";
        switch (vic.type) {
            case M6567_TYPE_6567R8: type = "6567R8 (NTSC)";
            case M6567_TYPE_6569: type = "6569 (PAL)";
        }
        ImGui::Text("Type: %s", type);
        ImGui::Separator();
        ImGui::Text("M0XY: %02X %02X M1XY: %02X %02X M2XY: %02X %02X M3XY: %02X %02X",
            vic.mob_xy[0][0], vic.mob_xy[0][1], vic.mob_xy[1][0], vic.mob_xy[1][1],
            vic.mob_xy[2][0], vic.mob_xy[2][1], vic.mob_xy[3][0], vic.mob_xy[3][1]);
        ImGui::Text("M4XY: %02X %02X M5XY: %02X %02X M6XY: %02X %02X M7XY: %02X %02X",
            vic.mob_xy[4][0], vic.mob_xy[4][1], vic.mob_xy[5][0], vic.mob_xy[6][1],
            vic.mob_xy[6][0], vic.mob_xy[6][1], vic.mob_xy[7][0], vic.mob_xy[7][1]);
        ImGui::Text("CTRL1 RST8:%c ECM:%c BMM:%c DEN:%c RSEL:%c YSCROLL:%d\n",
            vic.ctrl_1 & (1<<7) ? '1':'0',
            vic.ctrl_1 & (1<<6) ? '1':'0',
            vic.ctrl_1 & (1<<5) ? '1':'0',
            vic.ctrl_1 & (1<<4) ? '1':'0',
            vic.ctrl_1 & (1<<3) ? '1':'0',
            vic.ctrl_1 & 7);
        ImGui::Text("MSBX: %c%c%c%c%c%c%c%c RASTER: %02X LPX: %02X LPY: %02X",
            UINT8_BITS(vic.mob_x_msb),
            vic.raster, vic.lightpen_xy[0], vic.lightpen_xy[1]);
        ImGui::Text("ME:   %c%c%c%c%c%c%c%c", UINT8_BITS(vic.mob_enabled));
        ImGui::Text("CTRL2 RES:%c MCM:%c CSEL:%c XSCROLL:%d",
            vic.ctrl_2 & (1<<5) ? '1':'0',
            vic.ctrl_2 & (1<<4) ? '1':'0',
            vic.ctrl_2 & (1<<3) ? '1':'0',
            vic.ctrl_2 & 7); 
        ImGui::Text("MYE:  %c%c%c%c%c%c%c%c", UINT8_BITS(vic.mob_yexp));
        ImGui::Text("MEMPTRS VM:%02X CB:%02X", (vic.mem_ptrs>>4)&0x0F, (vic.mem_ptrs>>1)&0x07);
        ImGui::Text("INTR IRQ:%c ILP:%c IMMC:%c IMBC:%c IRST:%c",
            vic.interrupt & (1<<7) ? '1':'0',
            vic.interrupt & (1<<3) ? '1':'0',
            vic.interrupt & (1<<2) ? '1':'0',
            vic.interrupt & (1<<1) ? '1':'0',
            vic.interrupt & (1<<0) ? '1':'0');
        ImGui::Text("INT ENABLE ELP:%c EMMC:%c IMBC:%c ERST:%c",
            vic.int_enable & (1<<3) ? '1':'0',
            vic.int_enable & (1<<2) ? '1':'0',
            vic.int_enable & (1<<1) ? '1':'0',
            vic.int_enable & (1<<0) ? '1':'0');
        ImGui::Text("MDP: %c%c%c%c%c%c%c%c  MMC: %c%c%c%c%c%c%c%c",
            UINT8_BITS(vic.mob_data_priority),
            UINT8_BITS(vic.mob_multicolor));
        ImGui::Text("MXE: %c%c%c%c%c%c%c%c", UINT8_BITS(vic.mob_xexp));
        ImGui::Text("MCM: %c%c%c%c%c%c%c%c  MDM: %c%c%c%c%c%c%c%c",
            UINT8_BITS(vic.mob_mob_coll), UINT8_BITS(vic.mob_data_coll));
        this->drawColor("EC: ", vic.border_color);
        this->drawColor("B0C: ", vic.background_color[0]); ImGui::SameLine();
        this->drawColor("B1C: ", vic.background_color[1]); ImGui::SameLine();
        this->drawColor("B2C: ", vic.background_color[2]); ImGui::SameLine();
        this->drawColor("B3C: ", vic.background_color[3]);
        this->drawColor("M0C: ", vic.mob_color[0]); ImGui::SameLine();
        this->drawColor("M1C: ", vic.mob_color[1]); ImGui::SameLine();
        this->drawColor("M2C: ", vic.mob_color[2]); ImGui::SameLine();
        this->drawColor("M3C: ", vic.mob_color[3]);
        this->drawColor("M4C: ", vic.mob_color[4]); ImGui::SameLine();
        this->drawColor("M5C: ", vic.mob_color[5]); ImGui::SameLine();
        this->drawColor("M6C: ", vic.mob_color[6]); ImGui::SameLine();
        this->drawColor("M7C: ", vic.mob_color[7]);
        ImGui::Separator();
        ImGui::Text("h_count:   %2d  v_count: %3d", vic.h_count, vic.v_count);
        ImGui::Text("brd_left:  %2d  brd_top: %3d", vic.border_left, vic.border_top);
        ImGui::Text("brd_right: %2d  brd_btm: %3d", vic.border_right, vic.border_bottom);
        ImGui::Text("m_border: %s v_border: %s", vic.main_border?"ON ":"OFF", vic.vert_border?"ON ":"OFF");
        ImGui::Text("h_blank:  %s v_blank:  %s", vic.hs?"ON ":"OFF", vic.vs?"ON ":"OFF");
        ImGui::Text("badline:  %s", vic.badline?"ON ":"OFF");
        ImGui::Text("crt_x: %2d crt_y: %3d", vic.crt_x, vic.crt_y);

    }
    ImGui::End();
    return this->Visible;
}


} // namespace YAKC
