//------------------------------------------------------------------------------
//  I8255Window.cc
//------------------------------------------------------------------------------
#include "I8255Window.h"
#include "IMUI/IMUI.h"
#include "yakc_ui/UI.h"

using namespace Oryol;

namespace YAKC {

//------------------------------------------------------------------------------
void
I8255Window::Setup(yakc& emu) {
    this->setName("i8255 State");
}

//------------------------------------------------------------------------------
static const char* bit_name(const yakc& emu, int port, int bit) {
    if (emu.is_system(system::any_cpc)) {
        switch (port) {
            case I8255_PORT_A:
                return "psg/kbd";

            case I8255_PORT_B:
                switch (bit) {
                    case 0: return "vsync";
                    case 1:
                    case 2:
                    case 3: return "distr id";
                    case 4: return "60/50 hz";
                    case 5: return "/exp";
                    case 6: return "prn busy";
                    case 7: return "cass in";
                }
                break;
            case I8255_PORT_C:
                switch (bit) {
                    case 0:
                    case 1:
                    case 2:
                    case 3: return "kbd line";
                    case 4: return "cas motor";
                    case 5: return "cas write";
                    case 6:
                    case 7: return "psg func";
                }
                break;
        }
    }
    else if (emu.is_system(system::acorn_atom)) {
        switch (port) {
            case I8255_PORT_A:
                switch (bit) {
                    case 0:
                    case 1:
                    case 2:
                    case 3: return "kbd col";
                    case 4: return "vdg a/g";
                    case 5: return "vdg gm0";
                    case 6: return "vdg gm1";
                    case 7: return "vdg gm2";
                }
                break;
            case I8255_PORT_B:
                switch (bit) {
                    case 0:
                    case 1:
                    case 2:
                    case 3:
                    case 4:
                    case 5: return "kbd row";
                    case 6: return "kbd ctrl";
                    case 7: return "kbd shft";
                }
                break;
            case I8255_PORT_C:
                switch (bit) {
                    case 0: return "cas out";
                    case 1: return "cas 2.4khz";
                    case 2: return "speaker";
                    case 3: return "vdg css";
                    case 4: return "in 2.4khz";
                    case 5: return "cas in";
                    case 6: return "kbd rept";
                    case 7: return "fsync 60hz";
                }
        }
    }
    return "???";
}

//------------------------------------------------------------------------------
static void
put_bits(yakc& emu, int first, int num, int port, uint8_t val, const char* dir) {
    for (int bit = first; bit < (first + num); bit++) {
        ImGui::Text("%d: %s %d (%s)", bit, dir, (val&(1<<bit))?1:0, bit_name(emu, port, bit));
    }
}

//------------------------------------------------------------------------------
bool
I8255Window::Draw(yakc& emu) {
    ImGui::SetNextWindowSize(ImVec2(200, 292), ImGuiSetCond_Once);
    if (ImGui::Begin(this->title.AsCStr(), &this->Visible, ImGuiWindowFlags_ShowBorders)) {
        const i8255_t& ppi = board.i8255;
        ImGui::Text("only updated on CPU I/O!");
        if (ImGui::CollapsingHeader("Port A", "#ppi_a", true, true)) {
            if ((ppi.control & I8255_CTRL_A) == I8255_CTRL_A_INPUT) {
                put_bits(emu, 0, 8, I8255_PORT_A, ppi.dbg_input[I8255_PORT_A], "<=");
            }
            else {
                put_bits(emu, 0, 8, I8255_PORT_A, ppi.output[I8255_PORT_A], "=>");
            }
        }
        if (ImGui::CollapsingHeader("Port B", "#ppi_b", true, true)) {
            if ((ppi.control & I8255_CTRL_B) == I8255_CTRL_B_INPUT) {
                put_bits(emu, 0, 8, I8255_PORT_B, ppi.dbg_input[I8255_PORT_B], "<=");
            }
            else {
                put_bits(emu, 0, 8, I8255_PORT_B, ppi.output[I8255_PORT_B], "=>");
            }
        }
        if (ImGui::CollapsingHeader("Port C", "#ppi_c", true, true)) {
            if ((ppi.control & I8255_CTRL_CLO) == I8255_CTRL_CLO_INPUT) {
                put_bits(emu, 0, 4, I8255_PORT_C, ppi.dbg_input[I8255_PORT_C], "<=");
            }
            else {
                put_bits(emu, 0, 4, I8255_PORT_C, ppi.output[I8255_PORT_C], "=>");
            }
            if ((ppi.control & I8255_CTRL_CHI) == I8255_CTRL_CHI_INPUT) {
                put_bits(emu, 4, 4, I8255_PORT_C, ppi.dbg_input[I8255_PORT_C], "<=");
            }
            else {
                put_bits(emu, 4, 4, I8255_PORT_C, ppi.output[I8255_PORT_C], "=>");
            }
        }
    }
    ImGui::End();
    return this->Visible;
}

} // namespace YAKC
