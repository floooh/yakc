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
static void drawSpriteShifter(const char* label, uint32_t shf) {
    char str[33] = { };
    for (int i = 0; i < 32; i++) {
        str[i] = (((shf<<i) & (1<<31)) != 0) ? 'X':'.';
    } 
    str[32] = 0;
    ImGui::Text("%s: %s\n", label, str);
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
        ImGui::Checkbox("Debug Visualization", &board.m6567.debug_vis);
        if (ImGui::CollapsingHeader("Registers", "#vicreg", true, true)) {
            const auto& r = vic.reg;
            ImGui::Text("M0XY: %02X %02X M1XY: %02X %02X M2XY: %02X %02X M3XY: %02X %02X",
                r.mxy[0][0], r.mxy[0][1], r.mxy[1][0], r.mxy[1][1],
                r.mxy[2][0], r.mxy[2][1], r.mxy[3][0], r.mxy[3][1]);
            ImGui::Text("M4XY: %02X %02X M5XY: %02X %02X M6XY: %02X %02X M7XY: %02X %02X",
                r.mxy[4][0], r.mxy[4][1], r.mxy[5][0], r.mxy[6][1],
                r.mxy[6][0], r.mxy[6][1], r.mxy[7][0], r.mxy[7][1]);
            ImGui::Text("CTRL1 RST8:%c ECM:%c BMM:%c DEN:%c RSEL:%c YSCROLL:%d\n",
                r.ctrl_1 & (1<<7) ? '1':'0',
                r.ctrl_1 & (1<<6) ? '1':'0',
                r.ctrl_1 & (1<<5) ? '1':'0',
                r.ctrl_1 & (1<<4) ? '1':'0',
                r.ctrl_1 & (1<<3) ? '1':'0',
                r.ctrl_1 & 7);
            ImGui::Text("MSBX: %c%c%c%c%c%c%c%c RASTER: %02X LPX: %02X LPY: %02X",
                UINT8_BITS(r.mx8),
                r.raster, r.lightpen_xy[0], r.lightpen_xy[1]);
            ImGui::Text("ME:   %c%c%c%c%c%c%c%c", UINT8_BITS(r.me));
            ImGui::Text("CTRL2 RES:%c MCM:%c CSEL:%c XSCROLL:%d",
                r.ctrl_2 & (1<<5) ? '1':'0',
                r.ctrl_2 & (1<<4) ? '1':'0',
                r.ctrl_2 & (1<<3) ? '1':'0',
                r.ctrl_2 & 7);
            ImGui::Text("MYE:  %c%c%c%c%c%c%c%c", UINT8_BITS(r.mye));
            ImGui::Text("MEMPTRS VM:%02X CB:%02X", (r.mem_ptrs>>4)&0x0F, (r.mem_ptrs>>1)&0x07);
            ImGui::Text("INT IRQ:%c ILP:%c IMMC:%c IMBC:%c IRST:%c",
                r.int_latch & (1<<7) ? '1':'0',
                r.int_latch & (1<<3) ? '1':'0',
                r.int_latch & (1<<2) ? '1':'0',
                r.int_latch & (1<<1) ? '1':'0',
                r.int_latch & (1<<0) ? '1':'0');
            ImGui::Text("INT MASK  ELP:%c EMMC:%c EMBC:%c ERST:%c",
                r.int_mask & (1<<3) ? '1':'0',
                r.int_mask & (1<<2) ? '1':'0',
                r.int_mask & (1<<1) ? '1':'0',
                r.int_mask & (1<<0) ? '1':'0');
            ImGui::Text("MDP: %c%c%c%c%c%c%c%c  MMC: %c%c%c%c%c%c%c%c",
                UINT8_BITS(r.mdp),
                UINT8_BITS(r.mmc));
            ImGui::Text("MXE: %c%c%c%c%c%c%c%c", UINT8_BITS(r.mxe));
            ImGui::Text("MCM: %c%c%c%c%c%c%c%c  MDM: %c%c%c%c%c%c%c%c",
                UINT8_BITS(r.mob_mob_coll), UINT8_BITS(r.mob_data_coll));
            this->drawColor("EC: ", r.ec);
            this->drawColor("B0C: ", r.bc[0]); ImGui::SameLine();
            this->drawColor("B1C: ", r.bc[1]); ImGui::SameLine();
            this->drawColor("B2C: ", r.bc[2]); ImGui::SameLine();
            this->drawColor("B3C: ", r.bc[3]);
            this->drawColor("MM0: ", r.mm[0]); ImGui::SameLine();
            this->drawColor("MM1: ", r.mm[1]); 
            this->drawColor("M0C: ", r.mc[0]); ImGui::SameLine();
            this->drawColor("M1C: ", r.mc[1]); ImGui::SameLine();
            this->drawColor("M2C: ", r.mc[2]); ImGui::SameLine();
            this->drawColor("M3C: ", r.mc[3]);
            this->drawColor("M4C: ", r.mc[4]); ImGui::SameLine();
            this->drawColor("M5C: ", r.mc[5]); ImGui::SameLine();
            this->drawColor("M6C: ", r.mc[6]); ImGui::SameLine();
            this->drawColor("M7C: ", r.mc[7]);
        }
        if (ImGui::CollapsingHeader("Sprite Units", "#sprite", true, false)) {
            for (int i = 0; i < 8; i++) {
                ImGui::PushID(i);
                static const char* labels[8] = {
                    "Sprite Unit 0", "Sprite Unit 1", "Sprite Unit 2", "Sprite Unit 3",
                    "Sprite Unit 4", "Sprite Unit 5", "Sprite Unit 6", "Sprite Unit 7",
                };
                if (ImGui::CollapsingHeader(labels[i], "#sprite", true, false)) {
                    const auto& su = vic.sunit[i];
                    ImGui::Text("h_first/last/offset:  %2d <=> %2d, %d", su.h_first, su.h_last, su.h_offset);
                    ImGui::Text("p_data: %02X", su.p_data); ImGui::SameLine();
                    ImGui::Text("mc: %02X", su.mc); ImGui::SameLine();
                    ImGui::Text("mc_base: %02X", su.mc_base);
                    ImGui::Text("dma_enabled: %s", su.dma_enabled ? "ON ":"OFF"); ImGui::SameLine();
                    ImGui::Text("disp_enabled: %s", su.disp_enabled ? "ON ":"OFF"); ImGui::SameLine();
                    ImGui::Text("expand: %s", su.expand ? "ON":"OFF");
                    drawSpriteShifter("shifter", su.shift);
                }
                ImGui::PopID();
            }
        }
        if (ImGui::CollapsingHeader("Internal State", "#vicstate", true, true)) {
            ImGui::Text("g_mode: %d   v_irq_line: %3d\n", vic.gunit.mode, vic.rs.v_irqline);
            ImGui::Text("h_count:   %2d  v_count: %3d", vic.rs.h_count, vic.rs.v_count);
            ImGui::Text("vc: %04X: vcbase: %04X rc: %d vmli: %2d", vic.rs.vc, vic.rs.vc_base, vic.rs.rc, vic.vm.vmli);
            ImGui::Text("c_addr_or: %04X i_addr:     %04X", vic.mem.c_addr_or, vic.mem.i_addr);
            ImGui::Text("g_addr_or: %04X g_addr_and: %04X",vic.mem.g_addr_or, vic.mem.g_addr_and);
            ImGui::Text("brd_left:  %2d  brd_top: %3d", vic.brd.left, vic.brd.top);
            ImGui::Text("brd_right: %2d  brd_btm: %3d", vic.brd.right, vic.brd.bottom);
            ImGui::Text("m_border: %s v_border:  %s", vic.brd.main?"ON ":"OFF", vic.brd.vert?"ON ":"OFF");
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
