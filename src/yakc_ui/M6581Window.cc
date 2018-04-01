//------------------------------------------------------------------------------
//  M6581Window.cc
//------------------------------------------------------------------------------
#include "M6581Window.h"
#include "IMUI/IMUI.h"
#include "yakc/util/breadboard.h"

using namespace Oryol;

namespace YAKC {

//------------------------------------------------------------------------------
void
M6581Window::Setup(yakc& emu) {
    this->setName("M6581 (SID) State");
}

//------------------------------------------------------------------------------
bool
M6581Window::Draw(yakc& emu) {
    ImGui::SetNextWindowSize(ImVec2(320, 200), ImGuiSetCond_Once);
    if (ImGui::Begin(this->title.AsCStr(), &this->Visible)) {
        const m6581_t& sid = board.m6581;
        for (int i = 0; i < 3; i++) {
            ImGui::PushID(i);
            static const char* labels[3] = {
                "Voice 1", "Voice 2", "Voice 3"
            };
            if (ImGui::CollapsingHeader(labels[i], "#voice", true, true)) {
                const auto& v = sid.voice[i];
                ImGui::Text("FREQ: %04X", v.freq);
                ImGui::Text("PULSE WIDTH: %04X", v.pulse_width);
                ImGui::Text("GATE SYNC RING TEST TRIA SAWT PULS NOIS");
                ImGui::Text("%s  %s  %s  %s  %s  %s  %s  %s",
                    v.ctrl & M6581_CTRL_GATE        ? " ON":"OFF",
                    v.ctrl & M6581_CTRL_SYNC        ? " ON":"OFF",
                    v.ctrl & M6581_CTRL_RINGMOD     ? " ON":"OFF",
                    v.ctrl & M6581_CTRL_TEST        ? " ON":"OFF",
                    v.ctrl & M6581_CTRL_TRIANGLE    ? " ON":"OFF",
                    v.ctrl & M6581_CTRL_SAWTOOTH    ? " ON":"OFF",
                    v.ctrl & M6581_CTRL_PULSE       ? " ON":"OFF",
                    v.ctrl & M6581_CTRL_NOISE       ? " ON":"OFF");
                ImGui::Text("ATTACK:  %02X  DECAY:   %02X", v.attack, v.decay);
                ImGui::Text("SUSTAIN: %02X  RELEASE: %02X", v.sustain, v.release);
            }
            ImGui::PopID();
        }
        if (ImGui::CollapsingHeader("Filter", "#filter", true, true)) {
            const  m6581_filter_t& f = sid.filter;
            ImGui::Text("CUTOFF: %04X", f.cutoff);
            ImGui::Text("RESONANCE: %04X", f.resonance);
            ImGui::Text("FILT1 FILT2 FILT3 FILTEX");
            ImGui::Text("%s   %s   %s   %s",
                f.filter & M6581_FILTER_FILT1 ? " ON":"OFF",
                f.filter & M6581_FILTER_FILT2 ? " ON":"OFF",
                f.filter & M6581_FILTER_FILT3 ? " ON":"OFF",
                f.filter & M6581_FILTER_FILTEX ? " ON":"OFF");
            ImGui::Text("LOWP  BANDP HIGHP 3OFF");
            ImGui::Text("%s   %s   %s   %s",
                f.mode & M6581_FILTER_LOWPASS ? " ON":"OFF",
                f.mode & M6581_FILTER_BANDPASS ? " ON":"OFF",
                f.mode & M6581_FILTER_HIGHPASS ? " ON":"OFF",
                f.mode & M6581_FILTER_3OFF ? " ON":"OFF");
            ImGui::Text("VOLUME: %02X", f.volume);
        }
    }
    ImGui::End();
    return this->Visible;
}

} // namespace YAKC
