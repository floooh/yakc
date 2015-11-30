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
MemoryMapWindow::pageInfo
MemoryMapWindow::getPageInfo(kc85& kc, int layer_index, int page_index) const {
    pageInfo info;
    unsigned int addr = page_index * memory::page::size;
    if (0 == layer_index) {
        if (kc.model() == kc85_model::kc85_3) {
            if (addr < 0x4000) {
                info.name = "RAM0";
                info.addr = 0x0000;
                info.size = 0x4000;
            }
            else if ((addr >= 0x8000) && (addr < 0xC000)) {
                info.name = "IRM";
                info.addr = 0x8000;
                info.size = 0x4000;
            }
            else if ((addr >= 0xC000) && (addr < 0xE000)) {
                info.name = "BASIC ROM";
                info.addr = 0xC000;
                info.size = 0x2000;
            }
            else if ((addr >= 0xE000) && (addr < 0x10000)){
                info.name = "CAOS ROM";
                info.addr = 0xE000;
                info.size = 0x2000;
            }
        }
        else if (kc.model() == kc85_model::kc85_4) {
            if (addr < 0x4000) {
                info.name = "RAM0";
                info.addr = 0x0000;
                info.size = 0x4000;
            }
            else if ((addr >= 0x4000) && (addr < 0x8000)) {
                info.name = "RAM4";
                info.addr = 0x4000;
                info.size = 0x4000;
            }
            else if ((addr >= 0xE000) && (addr < 0x10000)){
                info.name = "CAOS ROM E";
                info.addr = 0xE000;
                info.size = 0x2000;
            }
        }
    }
    else if (1 == layer_index) {
        // base device module slot 08
        if (kc.exp.slot_occupied(0x08)) {
            const auto& slot = kc.exp.slot_by_addr(0x08);
            if ((addr >= slot.addr) && (addr < (slot.addr + slot.mod.mem_size))) {
                info.name = slot.mod.name;
                info.addr = slot.addr;
                info.size = slot.mod.mem_size;
            }
        }
    }
    else if (2 == layer_index) {
        // base device module slot 0C
        if (kc.exp.slot_occupied(0x0C)) {
            const auto& slot = kc.exp.slot_by_addr(0x0C);
            if ((addr >= slot.addr) && (addr < (slot.addr + slot.mod.mem_size))) {
                info.name = slot.mod.name;
                info.addr = slot.addr;
                info.size = slot.mod.mem_size;
            }
        }
    }
    return info;
}

//------------------------------------------------------------------------------
bool
MemoryMapWindow::Draw(kc85& kc) {
    StringBuilder strBuilder;
    static const ImVec4 grey(0.25f, 0.25f, 0.25f, 1.0f);
    static const ImVec4 light_green(0.0f, 1.0f, 0.0f, 1.0f);
    static const ImVec4 dark_green(0.0f, 0.5f, 0.0f, 1.0f);
    static const ImVec2 page_size(12, 0);
    ImGui::SetNextWindowSize(ImVec2(512, 100), ImGuiSetCond_Once);
    if (ImGui::Begin(this->title.AsCStr(), &this->Visible, ImGuiWindowFlags_NoResize)) {
        static const int num_layers = 3;
        static const char* layer_name[num_layers] = { "BUILT-IN", "SLOT 08", "SLOT 0C" };
        for (int layer=0; layer<num_layers; layer++) {
            ImGui::Text(layer_name[layer]);
            for (int page=0; page<memory::num_pages; page++) {
                ImGui::SameLine(72 + page*(page_size.x));
                bool is_mapped = false;
                if (kc.cpu.mem.layers[layer][page].ptr) {
                    is_mapped = true;
                    if (kc.cpu.mem.get_mapped_layer_index(page) == layer) {
                        ImGui::PushStyleColor(ImGuiCol_Button, light_green);
                        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, light_green);
                    }
                    else {
                        ImGui::PushStyleColor(ImGuiCol_Button, dark_green);
                        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, dark_green);
                    }
                }
                else {
                    ImGui::PushStyleColor(ImGuiCol_Button, grey);
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, grey);
                }
                ImGui::Button("",page_size);
                if (ImGui::IsItemHovered()) {
                    pageInfo info = this->getPageInfo(kc, layer, page);
                    if (info.name) {
                        strBuilder.Format(48, "%s\n(%04X-%04X)", info.name, info.addr, (info.addr + info.size) - 1);
                        ImGui::SetTooltip(strBuilder.AsCStr());
                    }
                }
                ImGui::PopStyleColor();
                ImGui::PopStyleColor();
            }
        }
    }
    ImGui::End();
    return this->Visible;
}
