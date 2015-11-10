//------------------------------------------------------------------------------
//  main.cc
//  yakc app main source.
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/App.h"
#include "Gfx/Gfx.h"
#include "Input/Input.h"
#include "KC85Oryol.h"
#include "Draw.h"
#if YAKC_UI
#include "yakc_ui/UI.h"
#endif
#include "Messaging/Dispatcher.h"
#include "Input/InputProtocol.h"

using namespace Oryol;
using namespace yakc;

class YakcApp : public App {
public:
    AppState::Code OnInit();
    AppState::Code OnRunning();
    AppState::Code OnCleanup();
    void handleInput();

    Id drawState;
    kc85 kc;
    Draw draw;
    #if YAKC_UI
    UI ui;
    #endif
};
OryolMain(YakcApp);

//------------------------------------------------------------------------------
AppState::Code
YakcApp::OnInit() {

    // we only need few resources, so don't waste memory 
    auto gfxSetup = GfxSetup::Window(640, 512, "KC85");
    gfxSetup.SetPoolSize(GfxResourceType::Mesh, 4);
    gfxSetup.SetPoolSize(GfxResourceType::Texture, 4);
    gfxSetup.SetPoolSize(GfxResourceType::DrawState, 4);
    gfxSetup.SetPoolSize(GfxResourceType::Shader, 4);
    Gfx::Setup(gfxSetup);
    Input::Setup();
    Input::BeginCaptureText();
    this->kc.switchon(kc85::kc_model::kc85_3);

    #if YAKC_UI
    this->ui.Setup(this->kc);
    #endif
    this->draw.Setup(gfxSetup);
    
    return AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
YakcApp::OnRunning() {
    Gfx::ApplyDefaultRenderTarget(ClearState::ClearColor(glm::vec4(0.5f,0.5f,0.5f,1.0f)));
    int micro_secs = 1000000 / 60;
    this->handleInput();
    this->kc.onframe(micro_secs);
    this->draw.Render(this->kc);
    #if YAKC_UI
    this->ui.OnFrame(this->kc);
    #endif
    Gfx::CommitFrame();
    return Gfx::QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
YakcApp::OnCleanup() {
    this->draw.Discard();
    #if YAKC_UI
    this->ui.Discard();
    #endif
    Input::Discard();
    Gfx::Discard();
    return App::OnCleanup();
}

//------------------------------------------------------------------------------
void
YakcApp::handleInput() {

    // don't handle KC input if IMGUI has the keyboard focus
    #if YAKC_UI
    if (ImGui::GetIO().WantCaptureKeyboard) {
        return;
    }
    #endif

    const Keyboard& kbd = Input::Keyboard();
    const wchar_t* text = kbd.CapturedText();
    ubyte ascii = 0;

    // alpha-numerics
    if (text[0] && (text[0] >= 32) && (text[0] < 127)) {
        ascii = (ubyte) text[0];
        // shift is inverted on KC
        if (islower(ascii)) {
            ascii = toupper(ascii);
        }
        else if (isupper(ascii)) {
            ascii = tolower(ascii);
        }
    }

    // special keys
    struct toAscii {
        Key::Code key;
        ubyte ascii;
    };
    const static toAscii keyTable[] = {
        // FIXME:
        //  HOME, PAGE UP/DOWN, START/END of line, INSERT,
        { Key::Left, 0x08 },
        { Key::Right, 0x09 },
        { Key::Down, 0x0A },
        { Key::Up, 0x0B },
        { Key::Enter, 0x0D },
        { Key::BackSpace, 0x01 },
        { Key::Escape, 0x03 },
        { Key::F1, 0xF1 },
        { Key::F2, 0xF2 },
        { Key::F3, 0xF3 },
        { Key::F4, 0xF4 },
        { Key::F5, 0xF5 },
        { Key::F6, 0xF6 },
        { Key::F7, 0xF7 },
        { Key::F8, 0xF8 },
        { Key::F9, 0xF9 },
        { Key::F10, 0xFA },
        { Key::F11, 0xFB },
        { Key::F12, 0xFC },
    };
    for (const auto& key : keyTable) {
        if (kbd.KeyDown(key.key)) {
            // special case: shift-backspace clears screen
            if (kbd.KeyPressed(Key::LeftShift) && (key.key == Key::BackSpace)) {
                ascii = 0x0C;
            }
            else {
                ascii = key.ascii;
            }
            break;
        }
    }

    if (0 != ascii) {
        this->kc.put_key(ascii);
    }
}
