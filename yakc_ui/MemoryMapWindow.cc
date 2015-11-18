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
const char*
MemoryMapWindow::getPageName(kc85& kc, int layer_index, int page_index) const {
    switch (layer_index) {
        case 0:
            // built-in memory
            switch (page_index) {
                case 0:
                case 1:
                    return "RAM0";
                case 4:
                case 5:
                    return "IRM";
                case 6:
                    return "BASIC ROM";
                case 7:
                    return "CAOS ROM";
            }
            break;
        case 1:
            // base device module slot 08
            if (kc.exp.slot_occupied(0x08)) {
                return kc.exp.slot_by_addr(0x08).mod.name;
            }
            break;
        case 2:
            // base device module slot 0C
            if (kc.exp.slot_occupied(0x0C)) {
                return kc.exp.slot_by_addr(0x0C).mod.name;
            }
            break;
    }
    return "unmapped";
}

//------------------------------------------------------------------------------
bool
MemoryMapWindow::Draw(kc85& kc) {
    StringBuilder strBuilder;
    static const ImVec4 grey(0.25f, 0.25f, 0.25f, 1.0f);
    static const ImVec4 light_green(0.0f, 1.0f, 0.0f, 1.0f);
    static const ImVec4 dark_green(0.0f, 0.5f, 0.0f, 1.0f);
    static const ImVec2 page_size(48, 0);
    ImGui::SetNextWindowSize(ImVec2(480, 100), ImGuiSetCond_Once);
    if (ImGui::Begin(this->title.AsCStr(), &this->Visible, ImGuiWindowFlags_NoResize)) {
        static const int num_layers = 3;
        static const char* layer_name[num_layers] = { "BUILT-IN", "SLOT 08", "SLOT 0C" };
        for (int layer=0; layer<num_layers; layer++) {
            ImGui::Text(layer_name[layer]);
            for (int page=0; page<memory::num_pages; page++) {
                ImGui::SameLine(72 + page*(page_size.x+2));
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
                if (is_mapped && ImGui::IsItemHovered()) {
                    strBuilder.Format(48, "%s\n(%04X-%04X)",
                        this->getPageName(kc, layer, page),
                        page * 0x2000, ((page+1) * 0x2000)-1);
                    ImGui::SetTooltip(strBuilder.AsCStr());
                }
                ImGui::PopStyleColor();
                ImGui::PopStyleColor();
            }
        }
    }
    ImGui::End();
    return this->Visible;
}
