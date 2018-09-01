//------------------------------------------------------------------------------
//  MC6845Window.cc
//------------------------------------------------------------------------------
#include "MC6845Window.h"
#include "IMUI/IMUI.h"
#include "Util.h"
#include "yakc/util/breadboard.h"

using namespace Oryol;

namespace YAKC {

//------------------------------------------------------------------------------
void
MC6845Window::Setup(yakc& emu) {
    this->setName("MC6845 State");
}

//------------------------------------------------------------------------------
bool
MC6845Window::Draw(yakc& emu) {
    ImGui::SetNextWindowSize(ImVec2(300, 340), ImGuiSetCond_Once);
    if (ImGui::Begin(this->title.AsCStr(), &this->Visible)) {
        if (board.mc6845) {
            mc6845_t& mc = *board.mc6845;

            Util::InputHex8("H TOTAL", mc.h_total); ImGui::SameLine(128);
            Util::InputHex8("H DISPLAYED", mc.h_displayed);

            Util::InputHex8("H SYNC POS", mc.h_sync_pos); ImGui::SameLine(128);
            Util::InputHex8("SYNC WIDTHS", mc.sync_widths);

            Util::InputHex8("V TOTAL", mc.v_total); ImGui::SameLine(128);
            Util::InputHex8("V TOTAL ADJUST", mc.v_total_adjust);

            Util::InputHex8("V DISPLAYED", mc.v_displayed); ImGui::SameLine(128);
            Util::InputHex8("V SYNC POS", mc.v_sync_pos);

            Util::InputHex8("INTERLACE", mc.interlace_mode); ImGui::SameLine(128);
            Util::InputHex8("MAX SCANLINE", mc.max_scanline_addr);

            Util::InputHex8("CURSOR START", mc.cursor_start); ImGui::SameLine(128);
            Util::InputHex8("CURSOR END", mc.cursor_end);

            Util::InputHex8("START ADDR HI", mc.start_addr_hi); ImGui::SameLine(128);
            Util::InputHex8("START ADDR LO", mc.start_addr_lo);

            Util::InputHex8("CURSOR HI", mc.cursor_hi); ImGui::SameLine(128);
            Util::InputHex8("CURSOR LO", mc.cursor_lo);

            Util::InputHex8("LIGHTPEN HI", mc.lightpen_hi); ImGui::SameLine(128);
            Util::InputHex8("LIGHTPEN LO", mc.lightpen_lo);

            ImGui::Separator();
            ImGui::Text("Hori Counter: %02X", mc.h_ctr); ImGui::SameLine(144);
            ImGui::Text("HSync: %s", mc.hs ? "ON":"OFF");
            ImGui::Text("Scanline Ctr: %02X", mc.scanline_ctr); ImGui::SameLine(144);
            ImGui::Text("VSync: %s", mc.vs ? "ON":"OFF");
            ImGui::Text("Row Counter:  %02X", mc.row_ctr); ImGui::SameLine(144);
            ImGui::Text("Disp Enable: %s", (mc.h_de&&mc.v_de) ? "ON":"OFF");
            ImGui::Text("Mem Address:  %04X", mc.ma);

            if (board.dbg.break_stopped()) {
                ImGui::Separator();
                if (ImGui::Button(">| HSync")) {
                    this->pins = mc.pins;
                    emu.step_until([this](uint32_t ticks)->bool {
                        bool triggered = false;
                        if (board.mc6845) {
                            uint64_t cur_pins = board.mc6845->pins;
                            triggered = 0 != (((cur_pins ^ this->pins) & cur_pins) & MC6845_HS);
                            this->pins = cur_pins;
                        }
                        return (ticks > (80*4)) || triggered;
                    });
                }
                if (ImGui::IsItemHovered()) { ImGui::SetTooltip("Run to start of hsync"); }
                ImGui::SameLine();
                if (ImGui::Button("<| HSync")) {
                    this->pins = mc.pins;
                    emu.step_until([this](uint32_t ticks)->bool {
                        bool triggered = false;
                        if (board.mc6845) {
                            uint64_t cur_pins = board.mc6845->pins;
                            triggered = 0 != (((cur_pins ^ this->pins) & ~cur_pins) & MC6845_HS);
                            this->pins = cur_pins;
                        }
                        return (ticks > (80*4)) || triggered;
                    });
                }
                if (ImGui::IsItemHovered()) { ImGui::SetTooltip("Run to end of hsync"); }
                ImGui::SameLine();
                if (ImGui::Button(">| VSync")) {
                    this->pins = mc.pins;
                    emu.step_until([this](uint32_t ticks)->bool {
                        bool triggered = false;
                        if (board.mc6845) {
                            uint64_t cur_pins = board.mc6845->pins;
                            triggered = 0 != (((cur_pins ^ this->pins) & cur_pins) & MC6845_VS);
                            this->pins = cur_pins;
                        }
                        return (ticks > (32000*4)) || triggered;
                    });
                }
                if (ImGui::IsItemHovered()) { ImGui::SetTooltip("Run to start of vsync"); }
                ImGui::SameLine();
                if (ImGui::Button("|< VSync")) {
                    this->pins = mc.pins;
                    emu.step_until([this](uint32_t ticks)->bool {
                        bool triggered = false;
                        if (board.mc6845) {
                            uint64_t cur_pins = board.mc6845->pins;
                            triggered = 0 != (((cur_pins ^ this->pins) & ~cur_pins) & MC6845_VS);
                            this->pins = cur_pins;
                        }
                        return (ticks > (32000*4)) || triggered;
                    });
                }
                if (ImGui::IsItemHovered()) { ImGui::SetTooltip("Run to end of vsync"); }
            }
        }
    }
    ImGui::End();
    return this->Visible;
}

} // namespace YAKC
