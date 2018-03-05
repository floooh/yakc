//------------------------------------------------------------------------------
//  M6526Window.cc
//------------------------------------------------------------------------------
#include "M6526Window.h"
#include "IMUI/IMUI.h"

using namespace Oryol;

namespace YAKC {

//------------------------------------------------------------------------------
M6526Window::M6526Window(const char* name, m6526_t* cia):
Name(name),
CIA(cia) {
    // empty
}

//------------------------------------------------------------------------------
void
M6526Window::Setup(yakc& emu) {
    this->setName(this->Name);
}

//------------------------------------------------------------------------------
static void drawPortState(const char* label, m6526_port_t* p) {
    ImGui::Text("%s reg:%02X ddr:%02X inp:%02X", label, p->reg, p->ddr, p->inp);
}

//------------------------------------------------------------------------------
static void drawPipeline(const char* label, uint8_t pip) {
    ImGui::Text("%s >> %c%c%c%c%c%c%c%c >>",
        label,
        pip & (1<<7)?'X':'-',
        pip & (1<<6)?'X':'-',
        pip & (1<<5)?'X':'-',
        pip & (1<<4)?'X':'-',
        pip & (1<<3)?'X':'-',
        pip & (1<<2)?'X':'-',
        pip & (1<<1)?'X':'-',
        pip & (1<<0)?'X':'-');
}

//------------------------------------------------------------------------------
static void drawTimerState(int type, m6526_timer_t* t) {
    ImGui::Text("cnt: %04X latch: %04X", t->counter, t->latch);
    ImGui::Text("out: %s bit: %s", t->t_out?"ON ":"OFF", t->t_bit?"ON ":"OFF");
    if (0 == type) {
        ImGui::Text("%s %s %s %s %s %s %s",
            t->cr & (1<<0)?"START":"STOP ",
            t->cr & (1<<1)?"PBON ":"PBOFF",
            t->cr & (1<<2)?"TOGGLE":"PULSE ",
            t->cr & (1<<3)?"ONESHOT":"CONTIN ",
            t->cr & (1<<5)?"COUNT":"CLOCK",
            t->cr & (1<<6)?"SPOUT":"SPINP",
            t->cr & (1<<7)?"50HZ":"60HZ");
    }
    else {
        const char* inmode = "???";
        switch ((t->cr>>5) & 3) {
            case 0: inmode = "CLOCK"; break;
            case 1: inmode = "COUNT"; break;
            case 2: inmode = "TA   "; break;
            case 3: inmode = "TACNT"; break;
        }
        ImGui::Text("%s %s %s %s %s %s",
            t->cr & (1<<0)?"START":"STOP ",
            t->cr & (1<<1)?"PBON ":"PBOFF",
            t->cr & (1<<2)?"TOGGLE":"PULSE ",
            t->cr & (1<<3)?"ONESHOT":"CONTIN ",
            inmode,
            t->cr & (1<<7)?"ALARM":"TOD  ");
    }
    drawPipeline("pip_count:   ", t->pip_count);
    drawPipeline("pip_load:    ", t->pip_load);
    drawPipeline("pip_oneshot: ", t->pip_oneshot);
}

//------------------------------------------------------------------------------
static void drawInterrupt(m6526_int_t* intr) {
    ImGui::Text("     INT  FLG SP  ALM TB  TA");
    ImGui::Text("ICR: %s  %s %s %s %s %s",
        (0 != (intr->icr & (1<<7))) ? "ON ":"OFF",
        (0 != (intr->icr & (1<<4))) ? "ON ":"OFF",
        (0 != (intr->icr & (1<<3))) ? "ON ":"OFF",
        (0 != (intr->icr & (1<<2))) ? "ON ":"OFF",
        (0 != (intr->icr & (1<<1))) ? "ON ":"OFF",
        (0 != (intr->icr & (1<<0))) ? "ON ":"OFF");
    ImGui::Text("ICM:      %s %s %s %s %s",
        (0 != (intr->imr & (1<<4))) ? "ON ":"OFF",
        (0 != (intr->imr & (1<<3))) ? "ON ":"OFF",
        (0 != (intr->imr & (1<<2))) ? "ON ":"OFF",
        (0 != (intr->imr & (1<<1))) ? "ON ":"OFF",
        (0 != (intr->imr & (1<<0))) ? "ON ":"OFF");
    drawPipeline("pip_irq: ", intr->pip_irq);
}

//------------------------------------------------------------------------------
bool
M6526Window::Draw(yakc& emu) {
    ImGui::SetNextWindowSize(ImVec2(320, 240), ImGuiSetCond_Once);
    if (ImGui::Begin(this->title.AsCStr(), &this->Visible)) {
        if (ImGui::CollapsingHeader("Ports", "#port_a", true, true)) {
            drawPortState("A:", &this->CIA->pa);
            drawPortState("B:", &this->CIA->pb);
        }
        if (ImGui::CollapsingHeader("Interrupt", "#int", true, true)) {
            drawInterrupt(&this->CIA->intr);
        }
        if (ImGui::CollapsingHeader("Timer A", "#timer_a", true, true)) {
            drawTimerState(0, &this->CIA->ta);
        }
        if (ImGui::CollapsingHeader("Timer B", "#timer_b", true, true)) {
            drawTimerState(1, &this->CIA->tb);
        }
    }
    ImGui::End();
    return this->Visible;
}

} // namespace YAKC
