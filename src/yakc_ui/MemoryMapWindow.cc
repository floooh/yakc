//------------------------------------------------------------------------------
//  MemoryMapWindow.cc
//------------------------------------------------------------------------------
#include "MemoryMapWindow.h"
#include "IMUI/IMUI.h"
#include "Core/String/StringBuilder.h"
#include "yakc_ui/UI.h"

using namespace Oryol;

namespace YAKC {

static const int bank_height = 20;
static const int left_padding = 80;
static const int bank_div = 160;

//------------------------------------------------------------------------------
void
MemoryMapWindow::Setup(yakc& emu) {
    this->setName("Memory Map");
}

//------------------------------------------------------------------------------
void
MemoryMapWindow::drawGrid(bool is_kc85_4) {
    StringBuilder strBuilder;
    ImDrawList* l = ImGui::GetWindowDrawList();
    const ImVec2 canvas_pos = ImGui::GetCursorScreenPos();
    const ImVec2 canvas_area = ImGui::GetContentRegionAvail();
    const float glyph_width = ImGui::CalcTextSize("F").x;
    for (int i = 0; i < 5; i++) {
        const int addr = 0x4000 * i;
        const int x = addr / bank_div;
        const ImVec2 a(x+canvas_pos.x+left_padding, canvas_pos.y);
        const ImVec2 b(a.x, a.y+canvas_area.y+4-bank_height);
        l->AddLine(a, b, UI::CanvasLineColor);

        strBuilder.Format(32, "%04X", addr == 0x10000 ? 0xFFFF:addr);
        float text_x = addr == 0x10000 ? b.x - 4*glyph_width : b.x;
        l->AddText(ImVec2(text_x, b.y), UI::CanvasTextColor, strBuilder.AsCStr());
    }
    const ImVec2 a(canvas_pos.x+left_padding, canvas_pos.y+canvas_area.y-bank_height);
    const ImVec2 b(a.x+0x10000/bank_div, a.y);
    l->AddLine(a, b, UI::CanvasLineColor);

    ImVec2 text_pos(canvas_pos.x, canvas_pos.y + 2);
    for (int i = 0; i < (is_kc85_4 ? 6 : 1); i++) {
        l->AddText(text_pos, UI::CanvasTextColor, "internal");
        text_pos.y += bank_height;
    }
    l->AddText(text_pos, UI::CanvasTextColor, "slot 0x08");
    text_pos.y += bank_height;
    l->AddText(text_pos, UI::CanvasTextColor, "slot 0x0C");
}

//------------------------------------------------------------------------------
void
MemoryMapWindow::drawRect(int layer, uword addr, unsigned int len, const char* tooltip, type t) {
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
    const int h = bank_height;
    const int y = layer * h;
    const ImVec2 canvas_pos = ImGui::GetCursorScreenPos();

    const int w = len / bank_div;
    const int x = addr / bank_div;
    ImVec2 a(x+canvas_pos.x+left_padding+3, y+canvas_pos.y+2);
    ImVec2 b(a.x+w-5, a.y+h-4);
    ImGui::GetWindowDrawList()->AddRectFilled(a, b, color);

    if (ImGui::IsMouseHoveringRect(a, b)) {
        if (tooltip) {
            ImGui::SetTooltip("%s", tooltip);
        }
    }
}

//------------------------------------------------------------------------------
bool
MemoryMapWindow::Draw(yakc& emu) {
    StringBuilder strBuilder;
    static const ImVec4 grey(0.25f, 0.25f, 0.25f, 1.0f);
    static const ImVec4 light_green(0.0f, 1.0f, 0.0f, 1.0f);
    static const ImVec4 dark_green(0.0f, 0.5f, 0.0f, 1.0f);
    static const ImVec2 page_size(12, 0);
    bool is_kc85_4 = emu.model == device::kc85_4;
    bool is_kc85_2 = emu.model == device::kc85_2;
    const float window_height = is_kc85_4 ? 220.0f : 120.0f;
    ImGui::SetNextWindowSize(ImVec2(512.0f, window_height), ImGuiSetCond_Always);
    if (ImGui::Begin(this->title.AsCStr(), &this->Visible, ImGuiWindowFlags_NoResize|ImGuiWindowFlags_ShowBorders)) {

        // draw the background grid
        this->drawGrid(is_kc85_4);

        // FIXME: need to handle KC85 PIO port backing
        // as 'peripheral device' with callbacks!
        const ubyte pio_a = emu.board.pio.read_data(z80pio::A);
        const ubyte pio_b = emu.board.pio.read_data(z80pio::B);

        // built-in memory at 0x0000
        this->drawRect(0, 0x0000, 0x4000, "RAM 0", (pio_a & kc85::PIO_A_RAM) ? type::mapped : type::off) ;

        // built-in memory at 0x4000 (KC85/4 only)
        if (is_kc85_4) {
            this->drawRect(0, 0x4000, 0x4000, "RAM 4", (emu.kc85.io86 & kc85::IO86_RAM4) ? type::mapped : type::off);
        }

        // video memory
        if (pio_a & kc85::PIO_A_IRM) {
            if (is_kc85_4) {
                for (int layer = 0; layer < 4; layer++) {
                    const uword len = (0 == layer) ? 0x4000 : 0x2800;
                    const int irm_index = (emu.kc85.io84 & 6)>>1;
                    strBuilder.Format(32, "IRM %d", layer);
                    if (layer == irm_index) {
                        // KC85/4: irm0 mapped (image 0 pixel buffer)
                        this->drawRect(layer, 0x8000, len, strBuilder.AsCStr(), type::mapped);
                    }
                    else {
                        // KC85/4: irm0 pixel buffer unmapped, area at 0xA800 always mapped
                        this->drawRect(layer, 0x8000, len, strBuilder.AsCStr(), type::off);
                        if (0 == layer) {
                            this->drawRect(0, 0xA800, 0x1800, "IRM 0", type::mapped);
                        }
                    }
                }
            }
            else {
                // KC85/2, /3: video memory bank active
                this->drawRect(0, 0x8000, 0x4000, "IRM", type::mapped);
            }
        }
        else {
            // video memory completely off
            this->drawRect(0, 0x8000, 0x4000, "IRM 0", type::off);
            if (is_kc85_4) {
                for (int layer = 1; layer < 4; layer++) {
                    strBuilder.Format(32, "IRM %d", layer);
                    this->drawRect(layer, 0x8000, 0x2800, strBuilder.AsCStr(), type::off);
                }
            }
        }

        // KC85/4 RAM8 banks
        if (is_kc85_4) {
            type ram8_0 = (pio_a & kc85::PIO_A_IRM) ? type::hidden : type::mapped;
            type ram8_1 = ram8_0;
            if (pio_b & kc85::PIO_B_RAM8) {
                if (emu.kc85.io84 & kc85::IO84_SEL_RAM8) {
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
            this->drawRect(4, 0x8000, 0x4000, "RAM 8 / 0", ram8_0);
            this->drawRect(5, 0x8000, 0x4000, "RAM 8 / 1", ram8_1);
        }

        // BASIC / CAOS-C banks
        if (!is_kc85_2) {
            this->drawRect(0, 0xC000, 0x2000, "BASIC ROM", (pio_a & kc85::PIO_A_BASIC_ROM) ? type::mapped : type::off);
        }
        if (is_kc85_4) {
            this->drawRect(1, 0xC000, 0x1000, "CAOS ROM C", (emu.kc85.io86 & kc85::IO86_CAOS_ROM_C) ? type::mapped : type::off);
        }

        // CAOS-E bank
        this->drawRect(0, 0xE000, 0x2000, "CAOS ROM E", (pio_a & kc85::PIO_A_CAOS_ROM) ? type::mapped : type::off);

        // modules
        for (int mem_layer = 1; mem_layer < 3; mem_layer++) {
            const memory& mem = emu.board.cpu.mem;
            const ubyte slot_addr = mem_layer == 1 ? 0x08 : 0x0C;
            if (emu.kc85.exp.slot_occupied(slot_addr)) {
                const int draw_layer = (is_kc85_4 ? 5 : 0) + mem_layer;
                const auto& slot = emu.kc85.exp.slot_by_addr(slot_addr);
                YAKC_ASSERT(slot.mod.mem_size > 0);

                // split modules > 16 KByte into multiple banks
                unsigned int addr = slot.addr;
                unsigned int len  = slot.mod.mem_size;
                unsigned int end  = addr + len;
                while (addr < end) {
                    type t = type::off;
                    if (slot.control_byte & 1) {
                        t = emu.kc85.exp.module_in_slot_owns_pointer(slot_addr, mem.ptr(addr)) ? type::mapped : type::hidden;
                    }
                    this->drawRect(draw_layer, addr, len >= 0x4000 ? 0x4000:(len&0x3FFF), slot.mod.name, t);
                    addr += 0x4000;
                    len  -= 0x4000;
                }
            }
        }
    }
    ImGui::End();
    return this->Visible;
}

} // namespace YAKC
