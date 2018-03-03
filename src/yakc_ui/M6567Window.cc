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
M6567Window::Draw(yakc& emu) {
    ImGui::SetNextWindowSize(ImVec2(380, 460), ImGuiSetCond_Once);
    if (ImGui::Begin(this->title.AsCStr(), &this->Visible)) {
        m6567_t& vic = board.m6567;
        const char* type = "unknown";
        switch (vic.type) {
            case M6567_TYPE_6567R8: type = "6567R8 (NTSC)"; break;
            case M6567_TYPE_6569: type = "6569 (PAL)"; break;
        }
        ImGui::Text("Type: %s", type);
        if (ImGui::CollapsingHeader("Registers", "#vicreg", true, true)) {
            const auto& r = vic.reg;
            ImGui::Text("M0XY: %02X %02X M1XY: %02X %02X M2XY: %02X %02X M3XY: %02X %02X",
                r.mob_xy[0][0], r.mob_xy[0][1], r.mob_xy[1][0], r.mob_xy[1][1],
                r.mob_xy[2][0], r.mob_xy[2][1], r.mob_xy[3][0], r.mob_xy[3][1]);
            ImGui::Text("M4XY: %02X %02X M5XY: %02X %02X M6XY: %02X %02X M7XY: %02X %02X",
                r.mob_xy[4][0], r.mob_xy[4][1], r.mob_xy[5][0], r.mob_xy[6][1],
                r.mob_xy[6][0], r.mob_xy[6][1], r.mob_xy[7][0], r.mob_xy[7][1]);
            ImGui::Text("CTRL1 RST8:%c ECM:%c BMM:%c DEN:%c RSEL:%c YSCROLL:%d\n",
                r.ctrl_1 & (1<<7) ? '1':'0',
                r.ctrl_1 & (1<<6) ? '1':'0',
                r.ctrl_1 & (1<<5) ? '1':'0',
                r.ctrl_1 & (1<<4) ? '1':'0',
                r.ctrl_1 & (1<<3) ? '1':'0',
                r.ctrl_1 & 7);
            ImGui::Text("MSBX: %c%c%c%c%c%c%c%c RASTER: %02X LPX: %02X LPY: %02X",
                UINT8_BITS(r.mob_x_msb),
                r.raster, r.lightpen_xy[0], r.lightpen_xy[1]);
            ImGui::Text("ME:   %c%c%c%c%c%c%c%c", UINT8_BITS(r.mob_enabled));
            ImGui::Text("CTRL2 RES:%c MCM:%c CSEL:%c XSCROLL:%d",
                r.ctrl_2 & (1<<5) ? '1':'0',
                r.ctrl_2 & (1<<4) ? '1':'0',
                r.ctrl_2 & (1<<3) ? '1':'0',
                r.ctrl_2 & 7);
            ImGui::Text("MYE:  %c%c%c%c%c%c%c%c", UINT8_BITS(r.mob_yexp));
            ImGui::Text("MEMPTRS VM:%02X CB:%02X", (r.mem_ptrs>>4)&0x0F, (r.mem_ptrs>>1)&0x07);
            ImGui::Text("INTR IRQ:%c ILP:%c IMMC:%c IMBC:%c IRST:%c",
                r.interrupt & (1<<7) ? '1':'0',
                r.interrupt & (1<<3) ? '1':'0',
                r.interrupt & (1<<2) ? '1':'0',
                r.interrupt & (1<<1) ? '1':'0',
                r.interrupt & (1<<0) ? '1':'0');
            ImGui::Text("INT ENABLE ELP:%c EMMC:%c IMBC:%c ERST:%c",
                r.int_enable & (1<<3) ? '1':'0',
                r.int_enable & (1<<2) ? '1':'0',
                r.int_enable & (1<<1) ? '1':'0',
                r.int_enable & (1<<0) ? '1':'0');
            ImGui::Text("MDP: %c%c%c%c%c%c%c%c  MMC: %c%c%c%c%c%c%c%c",
                UINT8_BITS(r.mob_data_priority),
                UINT8_BITS(r.mob_multicolor));
            ImGui::Text("MXE: %c%c%c%c%c%c%c%c", UINT8_BITS(r.mob_xexp));
            ImGui::Text("MCM: %c%c%c%c%c%c%c%c  MDM: %c%c%c%c%c%c%c%c",
                UINT8_BITS(r.mob_mob_coll), UINT8_BITS(r.mob_data_coll));
            this->drawColor("EC: ", r.border_color);
            this->drawColor("B0C: ", r.background_color[0]); ImGui::SameLine();
            this->drawColor("B1C: ", r.background_color[1]); ImGui::SameLine();
            this->drawColor("B2C: ", r.background_color[2]); ImGui::SameLine();
            this->drawColor("B3C: ", r.background_color[3]);
            this->drawColor("M0C: ", r.mob_color[0]); ImGui::SameLine();
            this->drawColor("M1C: ", r.mob_color[1]); ImGui::SameLine();
            this->drawColor("M2C: ", r.mob_color[2]); ImGui::SameLine();
            this->drawColor("M3C: ", r.mob_color[3]);
            this->drawColor("M4C: ", r.mob_color[4]); ImGui::SameLine();
            this->drawColor("M5C: ", r.mob_color[5]); ImGui::SameLine();
            this->drawColor("M6C: ", r.mob_color[6]); ImGui::SameLine();
            this->drawColor("M7C: ", r.mob_color[7]);
        }
        if (ImGui::CollapsingHeader("Internal State", "#vicstate", true, true)) {
            ImGui::Text("g_mode: %d\n", vic.gseq.mode);
            ImGui::Text("h_count:   %2d  v_count: %3d", vic.rs.h_count, vic.rs.v_count);
            ImGui::Text("vc: %04X: vcbase: %04X rc: %d vmli: %2d", vic.rs.vc, vic.rs.vc_base, vic.rs.rc, vic.vm.vmli);
            ImGui::Text("c_addr_or: %04X i_addr:     %04X", vic.mem.c_addr_or, vic.mem.i_addr);
            ImGui::Text("g_addr_or: %04X g_addr_and: %04X",vic.mem.g_addr_or, vic.mem.g_addr_and);
            ImGui::Text("brd_left:  %2d  brd_top: %3d", vic.brd.left, vic.brd.top);
            ImGui::Text("brd_right: %2d  brd_btm: %3d", vic.brd.right, vic.brd.bottom);
            ImGui::Text("m_border: %s v_border:  %s", vic.brd.main?"ON ":"OFF", vic.brd.vert?"ON ":"OFF");
            ImGui::Text("h_blank:  %s v_blank:   %s", vic.rs.h_sync?"ON ":"OFF", vic.rs.v_sync?"ON ":"OFF");
            ImGui::Text("badline:  %s display:   %s", vic.rs.badline?"ON ":"OFF", vic.rs.display_state?"ON ":"OFF");
            ImGui::Text("crt_x: %2d crt_y: %3d", vic.crt.x, vic.crt.y);
            ImGui::Text("line buffer:");
            const uint16_t* l = vic.vm.line;
            ImGui::Text("  %03X %03X %03X %03X %03X %03X %03X %03X %03X %03X\n"
                        "  %03X %03X %03X %03X %03X %03X %03X %03X %03X %03X\n"
                        "  %03X %03X %03X %03X %03X %03X %03X %03X %03X %03X\n"
                        "  %03X %03X %03X %03X %03X %03X %03X %03X %03X %03X",
                        l[ 0], l[ 1], l[ 2], l[ 3], l[ 4], l[ 5], l[ 6], l[ 7], l[ 8], l[ 9],
                        l[10], l[11], l[12], l[13], l[14], l[15], l[16], l[17], l[18], l[19],
                        l[20], l[21], l[22], l[23], l[24], l[25], l[26], l[27], l[28], l[29],
                        l[30], l[31], l[32], l[33], l[34], l[35], l[36], l[37], l[38], l[39]);
        }
        if (board.dbg.break_stopped()) {
            ImGui::Separator();
            if (ImGui::Button(">| Bad")) {
                this->badline = vic.rs.badline;
                emu.step_until([this](uint32_t ticks)->bool {
                    bool cur_badline = board.m6567.rs.badline;
                    bool triggered = (cur_badline != this->badline) && cur_badline;
                    this->badline = cur_badline;
                    return (ticks > (64*312)) || triggered;
                });
            }
            if (ImGui::IsItemHovered()) { ImGui::SetTooltip("Run to next badline"); }
        }
    }
    ImGui::End();
    return this->Visible;
}


} // namespace YAKC
