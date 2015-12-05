//------------------------------------------------------------------------------
//  MemoryMapWindow.cc
//------------------------------------------------------------------------------
#include "MemoryMapWindow.h"
#include "IMUI/IMUI.h"
#include "Core/String/StringBuilder.h"

OryolClassImpl(MemoryMapWindow);

using namespace Oryol;
using namespace yakc;

//------------------------------------------------------------------------------
void
MemoryMapWindow::Setup(kc85& kc) {
    this->setName("Memory Map");
}

//------------------------------------------------------------------------------
void
MemoryMapWindow::drawRect(int layer, uword addr, uword len, type t) {
    static const ImVec4 grey(0.25f, 0.25f, 0.25f, 1.0f);
    static const ImVec4 light_green(0.0f, 1.0f, 0.0f, 1.0f);
    static const ImVec4 dark_green(0.0f, 0.5f, 0.0f, 1.0f);

    ImU32 color = 0;
    switch (t) {
        case type::off:
            color = ImGui::ColorConvertFloat4ToU32(grey);
            break;
        case type::mapped:
            color = ImGui::ColorConvertFloat4ToU32(light_green);
            break;
        case type::hidden:
            color = ImGui::ColorConvertFloat4ToU32(dark_green);
            break;
    }

    // compute rectangle coordinates
    const int h = 20;
    const int w = len / 160;
    const int x = addr / 160;
    const int y = layer * h;
    const int padding = 4;
    ImVec2 canvas_pos = ImGui::GetCursorScreenPos();

    ImVec2 a(x+canvas_pos.x, y+canvas_pos.y);
    ImVec2 b((a.x+w)-padding,(a.y+h)-padding);
    ImDrawList* l = ImGui::GetWindowDrawList();
    l->PushClipRectFullScreen();
    l->AddRectFilled(a, b, color);
    l->PopClipRect();
}

//------------------------------------------------------------------------------
bool
MemoryMapWindow::Draw(kc85& kc) {
    StringBuilder strBuilder;
    static const ImVec4 grey(0.25f, 0.25f, 0.25f, 1.0f);
    static const ImVec4 light_green(0.0f, 1.0f, 0.0f, 1.0f);
    static const ImVec4 dark_green(0.0f, 0.5f, 0.0f, 1.0f);
    static const ImVec2 page_size(12, 0);
    bool is_kc85_4 = kc.model() == kc85_model::kc85_4;
    bool is_kc85_2 = kc.model() == kc85_model::kc85_2;
    const int window_height = is_kc85_4 ? 200 : 100;
    ImGui::SetNextWindowSize(ImVec2(512, window_height), ImGuiSetCond_Always);
    if (ImGui::Begin(this->title.AsCStr(), &this->Visible, ImGuiWindowFlags_NoResize)) {
        const ubyte pio_a = kc.pio.read(z80pio::A);
        const ubyte pio_b = kc.pio.read(z80pio::B);

        // built-in memory at 0x0000
        this->drawRect(0, 0x0000, 0x4000, (pio_a & kc85::PIO_A_RAM) ? type::mapped : type::off) ;

        // built-in memory at 0x4000 (KC85/4 only)
        if (is_kc85_4) {
            this->drawRect(0, 0x4000, 0x4000, (kc.io86 & kc85::IO86_RAM4) ? type::mapped : type::off);
        }

        // video memory
        if (pio_a & kc85::PIO_A_IRM) {
            if (is_kc85_4) {
                for (int layer = 0; layer < 4; layer++) {
                    const uword len = (0 == layer) ? 0x4000 : 0x2800;
                    const int irm_index = (kc.io84 & 6)>>1;
                    if (layer == irm_index) {
                        // KC85/4: irm0 mapped (image 0 pixel buffer)
                        this->drawRect(layer, 0x8000, len, type::mapped);
                    }
                    else {
                        // KC85/4: irm0 pixel buffer unmapped, area at 0xA800 always mapped
                        this->drawRect(layer, 0x8000, len, type::off);
                        if (0 == layer) {
                            this->drawRect(0, 0xA800, 0x1800, type::mapped);
                        }
                    }
                }
            }
            else {
                // KC85/2, /3: video memory bank active
                this->drawRect(0, 0x8000, 0x4000, type::mapped);
            }
        }
        else {
            // video memory completely off
            this->drawRect(0, 0x8000, 0x4000, type::off);
            if (is_kc85_4) {
                for (int layer = 1; layer < 4; layer++) {
                    this->drawRect(layer, 0x8000, 0x2800, type::off);
                }
            }
        }

        // KC85/4 RAM8 banks
        if (is_kc85_4) {
            type ram8_0 = (pio_a & kc85::PIO_A_IRM) ? type::hidden : type::mapped;
            type ram8_1 = ram8_0;
            if (pio_b & kc85::PIO_B_RAM8) {
                if (kc.io84 & kc85::IO84_SEL_RAM8) {
                    ram8_0 = type::off;
                }
                else {
                    ram8_1 = type::off;
                }
            }
            else {
                ram8_0 = type::off;
                ram8_1 = type::off;
            }
            this->drawRect(4, 0x8000, 0x4000, ram8_0);
            this->drawRect(5, 0x8000, 0x4000, ram8_1);
        }

        // BASIC / CAOS-C banks
        if (!is_kc85_2) {
            this->drawRect(0, 0xC000, 0x2000, (pio_a & kc85::PIO_A_BASIC_ROM) ? type::mapped : type::off);
        }
        if (is_kc85_4) {
            this->drawRect(1, 0xC000, 0x1000, (kc.io86 & kc85::IO86_CAOS_ROM_C) ? type::mapped : type::off);
        }

        // CAOS-E bank
        this->drawRect(0, 0xE000, 0x2000, (pio_a & kc85::PIO_A_CAOS_ROM) ? type::mapped : type::off);

        // modules
        for (int mem_layer = 1; mem_layer < 3; mem_layer++) {
            const memory& mem = kc.cpu.mem;
            const ubyte slot_addr = mem_layer == 1 ? 0x08 : 0x0C;
            if (kc.exp.slot_occupied(slot_addr)) {
                const int draw_layer = (is_kc85_4 ? 5 : 0) + mem_layer;
                const auto& slot = kc.exp.slot_by_addr(slot_addr);
                YAKC_ASSERT(slot.mod.mem_size > 0);
                const unsigned int num_banks = ((slot.mod.mem_size-1)>>14)+1;
                for (unsigned int bank = 0; bank < num_banks; bank++) {
                    const uword addr = slot.addr + bank * 0x4000;
                    const uword len  = slot.mod.mem_size - bank * 0x4000;
                    type t = type::off;
                    if (slot.control_byte & 1) {
                        t = (mem.ptr(addr) == slot.mod.mem_ptr) ? type::mapped : type::hidden;
                    }
                    this->drawRect(draw_layer, addr, len, t);
                }
            }
        }
    }
    ImGui::End();
    return this->Visible;
}
