//------------------------------------------------------------------------------
//  main.cc
//  yakc app main source.
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/App.h"
#include "Gfx/Gfx.h"
#include "Synth/Synth.h"
#include "Input/Input.h"
#include "IO/IO.h"
#include "KC85Oryol.h"
#include "Draw.h"
#include "Audio.h"
#if YAKC_UI
#include "yakc_ui/UI.h"
#endif
#include "HTTP/HTTPFileSystem.h"

using namespace Oryol;
using namespace yakc;

class YakcApp : public App {
public:
    AppState::Code OnInit();
    AppState::Code OnRunning();
    AppState::Code OnCleanup();
    void handleInput();

    ubyte last_ascii = 0;
    Id drawState;
    kc85 kc;
    Draw draw;
    Audio audio;
    #if YAKC_UI
    UI ui;
    #endif
};
OryolMain(YakcApp);

//------------------------------------------------------------------------------
AppState::Code
YakcApp::OnInit() {

    // setup IO system for loading from webserver
    #if ORYOL_DEBUG
    String baseUrl = "http://localhost:8000/";
    #elif ORYOL_EMSCRIPTEN
    // ok, this is strange, why is the github URL different from Oryol??
    String baseUrl = "http://floooh.github.com/";
    #else
    String baseUrl = "http://floooh.github.com/virtualkc/";
    #endif
    IOSetup ioSetup;
    ioSetup.FileSystems.Add("http", HTTPFileSystem::Creator());
    ioSetup.Assigns.Add("kcc:", baseUrl);
    IO::Setup(ioSetup);

    // we only need few resources, so don't waste memory
    const int frameSize = 10;
    const int width = 2*frameSize + 2*320;
    const int height = 2*frameSize + 2*256;
    auto gfxSetup = GfxSetup::Window(width, height, "KC85");
    gfxSetup.SetPoolSize(GfxResourceType::Mesh, 4);
    gfxSetup.SetPoolSize(GfxResourceType::Texture, 4);
    gfxSetup.SetPoolSize(GfxResourceType::DrawState, 4);
    gfxSetup.SetPoolSize(GfxResourceType::Shader, 4);
    Gfx::Setup(gfxSetup);
    Input::Setup();
    Input::BeginCaptureText();

    Synth::Setup(SynthSetup());

    #if YAKC_UI
    this->ui.Setup(this->kc);
    #endif
    const kc85_model model = kc85_model::kc85_3;
    this->kc.poweron(model);
    this->draw.Setup(gfxSetup, frameSize);
    this->audio.Setup(this->kc);

    // on KC85/3 put a 16kByte module into slot 8 by default, CAOS will initialize
    // this automatically on startup
    if (model == kc85_model::kc85_3) {
        this->kc.exp.insert_module(0x08,kc85_module::create_ram(0xF4, 0x4000,
            "M022 EPANDER RAM",
            "16 KByte RAM expansion module.\n\n"
            "SWITCH [SLOT] 43: map to address 0x4000\n"
            "SWITCH [SLOT] 83: map to address 0x8000\n"
            "SWITCH [SLOT] 00: switch module off\n\n"
            "...where [SLOT] is 08 or 0C"));
    }

    return AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
YakcApp::OnRunning() {
    Gfx::ApplyDefaultRenderTarget(ClearState::ClearColor(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)));
    int micro_secs = 1000000 / 60;
    this->handleInput();
    #if YAKC_UI
        this->kc.onframe(this->ui.Settings.cpuSpeed, micro_secs);
        this->draw.fsParams.CRTEffect = this->ui.Settings.crtEffect;
        this->draw.fsParams.ColorTV = this->ui.Settings.colorTV;
        this->draw.fsParams.CRTWarp = glm::vec2(this->ui.Settings.crtWarp);
    #else
        this->kc.onframe(1, micro_secs);
        this->draw.fsParams.CRTEffect = true;
        this->draw.fsParams.ColorTV = true;
        this->draw.fsParams.CRTWarp = glm::vec2(1.0f/64.0f);
    #endif
    this->draw.Render(this->kc);
    Synth::Update();
    #if YAKC_UI
    this->ui.OnFrame(this->kc);
    #endif
    Gfx::CommitFrame();
    return Gfx::QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
YakcApp::OnCleanup() {
    this->audio.Discard();
    this->draw.Discard();
    #if YAKC_UI
    this->ui.Discard();
    #endif
    Synth::Discard();
    Input::Discard();
    Gfx::Discard();
    IO::Discard();
    return App::OnCleanup();
}

//------------------------------------------------------------------------------
void
YakcApp::handleInput() {
    const Keyboard& kbd = Input::Keyboard();
    const Touchpad& touch = Input::Touchpad();

    #if YAKC_UI
    // don't handle KC input if IMGUI has the keyboard focus
    if (ImGui::GetIO().WantCaptureKeyboard) {
        return;
    }
    // toggle UI?
    if (kbd.KeyDown(Key::Tab) || touch.DoubleTapped) {
        this->ui.Toggle();
    }
    #endif

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
    if ((ascii == 0) && (kbd.AnyKeyPressed())) {
        ascii = this->last_ascii;
    }
    this->last_ascii = ascii;

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
        if (kbd.KeyPressed(key.key)) {
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
    this->kc.put_key(ascii);
}
