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
        if (board.m6581) {
            const m6581_t& sid = *board.m6581;
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
                    ImGui::Text("ATTACK:  %02X  DECAY:   %02X", v.env_attack_add, v.env_decay_sub);
                    ImGui::Text("SUSTAIN: %02X  RELEASE: %02X", v.env_sustain_level, v.env_release_sub);
                    ImGui::Text("wav_accum: %06X wav_output: %04X", v.wav_accum, v.wav_output);
                    ImGui::Text("noise_shift: %06X", v.noise_shift);
                    static const char* env_state[3] = { "ATTACK", "DECAY_SUSTAIN", "RELEASE" };
                    ImGui::Text("env_state: %s", env_state[v.env_state % 3]);
                    ImGui::Text("env_count: %02X", v.env_counter);
                }
                ImGui::PopID();
            }
            if (ImGui::CollapsingHeader("Filter", "#filter", true, true)) {
                const  m6581_filter_t& f = sid.filter;
                ImGui::Text("CUTOFF: %04X", f.cutoff);
                ImGui::Text("RESONANCE: %04X", f.resonance);
                ImGui::Text("VOICE1 VOICE2 VOICE3");
                ImGui::Text("%s    %s    %s",
                    f.voices & M6581_FILTER_FILT1 ? " ON":"OFF",
                    f.voices & M6581_FILTER_FILT2 ? " ON":"OFF",
                    f.voices & M6581_FILTER_FILT3 ? " ON":"OFF");
                ImGui::Text("LOWP  BANDP HIGHP 3OFF");
                ImGui::Text("%s   %s   %s   %s",
                    f.mode & M6581_FILTER_LP ? " ON":"OFF",
                    f.mode & M6581_FILTER_BP ? " ON":"OFF",
                    f.mode & M6581_FILTER_HP ? " ON":"OFF",
                    f.mode & M6581_FILTER_3OFF ? " ON":"OFF");
                ImGui::Text("VOLUME: %02X", f.volume);
                ImGui::Text("w0:  %d", f.w0);
                ImGui::Text("Vlp: %d", f.v_lp);
                ImGui::Text("Vbp: %d", f.v_bp);
                ImGui::Text("Vhp: %d", f.v_hp);
            }
        }
    }
    ImGui::End();
    return this->Visible;
}

} // namespace YAKC
