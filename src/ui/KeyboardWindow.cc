//------------------------------------------------------------------------------
//  KeyboardWindow.cc
//------------------------------------------------------------------------------
#include "KeyboardWindow.h"
#include "IMUI/IMUI.h"

using namespace Oryol;

namespace yakc {

struct key {
    key() : pos(0.0f), name(nullptr), code(0), shift_code(0) { };
    key(float p, const char* n, ubyte c, ubyte sc) : pos(p), name(n), code(c), shift_code(sc) { };
    float pos;
    const char* name;
    ubyte code;
    ubyte shift_code;
};

// the 5 'main section' rows of keys:
// see: http://www.mpm-kc85.de/dokupack/KC85_3_uebersicht.pdf
static const int num_rows = 5;
static const int num_cols = 13;
static struct key layout[num_rows][num_cols] = {
{
    // function keys row
    {4,"F1",0xF1,0xF7}, {0,"F2",0xF2,0xF8}, {0,"F3",0xF3,0xF9}, {0,"F4",0xF4,0xFA}, {0,"F5",0xF5,0xFB}, {0,"F6",0xF6,0xFC},
    {0,"BRK",0x03,0x03}, {0,"STP",0x13,0x13}, {0,"INS",0x1A,0x14}, {0,"DEL",0x1F,0x02}, {0,"CLR",0x01,0x0F}, {0,"HOM",0x10,0x0C}
},
{
    // number keys row
    {4,"1 !",'1','!'}, {0,"2 \"",'2','\"'}, {0,"3 #",'3','#'}, {0,"4 $",'4','$'}, {0,"5 %",'5','%'}, {0,"6 &",'6','&'},
    {0,"7 '",'7',0x27}, {0,"8 (",'8','('}, {0,"9 )",'9',')'}, {0,"0 @",'0','@'}, {0,": *",':','*'}, {0,"- =",'-','='},
    {2,"CUU",0x0B,0x11}
},
{
    // QWERT row
    {16,"Q",'Q','q'}, {0,"W",'W','w'}, {0,"E",'E','e'}, {0,"R",'R','r'}, {0,"T",'T','t'}, {0,"Z",'Z','z'},
    {0,"U",'U','u'}, {0,"I",'I','i'}, {0,"O",'O','o'}, {0,"P",'P','p'}, {0,"\x5E \x5D",0x5E,0x5D},
    {10,"CUL",0x08,0x19},{0,"CUR",0x09,0x18}
},
{
    // ASDF row
    {0,"CAP",0x16,0x16}, {0,"A",'A','a'}, {0,"S",'S','s'}, {0,"D",'D','d'}, {0,"F",'F','f'}, {0,"G",'G','g'},
    {0,"H",'H','h'}, {0,"J",'J','j'}, {0,"K",'K','k'}, {0,"L",'L','l'}, {0,"+ ;",'+',';'}, {0,"\x5F |",0x5F,0x5C},
    {14,"CUD",0x0A,0x12}
},
{
    // YXCV row (NOTE: shift-key has special code which is not forwarded as key!
    {10,"SHI",0xFF,0xFF}, {0,"Y",'Y','y'}, {0,"X",'X','y'}, {0,"C",'C','c'}, {0,"V",'V','v'}, {0,"B",'B','b'},
    {0,"N",'N','n'}, {0,"M",'M','m'}, {0,", <",',','<'}, {0,". >",'.','>'}, {0,"/ ?",'/','?'},
    {36,"RET",0x0D,0x0D}
}

};

//------------------------------------------------------------------------------
void
KeyboardWindow::Setup(emu& emu) {
    this->setName("Keyboard");
}

//------------------------------------------------------------------------------
bool
KeyboardWindow::Draw(emu& emu) {
    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.75f);
    ImGui::SetNextWindowSize(ImVec2(572, 196));
    if (ImGui::Begin(this->title.AsCStr(), &this->Visible, ImGuiWindowFlags_NoResize|ImGuiWindowFlags_ShowBorders)) {

        // main section keys
        const ImVec2 size(32,24);
        for (int row = 0; row < num_rows; row++) {
            for (int col = 0; col < num_cols; col++) {
                const key& k = layout[row][col];
                if (k.name) {
                    if (col != 0) {
                        ImGui::SameLine();
                    }
                    if (k.pos > 0.0f) {
                        ImGui::Dummy(ImVec2(k.pos,0.0f)); ImGui::SameLine();
                    }
                    if (ImGui::Button(k.name, size)) {
                        // caps lock?
                        if (k.code == 0x16) {
                            this->caps_lock = !this->caps_lock;
                            this->shift = this->caps_lock;
                        }
                        // shift?
                        if (k.code == 0xFF) {
                            this->shift = true;
                        }
                        else {
                            emu.put_key(this->shift ? k.shift_code:k.code);

                            // clear shift state after one key, unless caps_lock is on
                            if (!this->caps_lock) {
                                this->shift = false;
                            }
                        }
                    }
                }
            }
        }

        // space bar
        ImGui::Dummy(ImVec2(80,0)); ImGui::SameLine();
        if (ImGui::Button("SPACE", ImVec2(224, 0))) {
            emu.put_key(this->caps_lock ? 0x5B : 0x20);
        }
    }
    ImGui::End();
    ImGui::PopStyleVar();
    return this->Visible;
}

} // namespace yakc




