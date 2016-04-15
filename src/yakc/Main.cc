//------------------------------------------------------------------------------
//  main.cc
//  yakc app main source.
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/Main.h"
#include "Gfx/Gfx.h"
#include "Input/Input.h"
#include "IO/IO.h"
#include "KC85Oryol.h"
#include "Draw.h"
#include "Audio.h"
#if YAKC_UI
#include "ui/UI.h"
#endif
#include "HTTP/HTTPFileSystem.h"
#include "Time/Clock.h"

using namespace Oryol;
using namespace yakc;

class YakcApp : public App {
public:
    AppState::Code OnInit();
    AppState::Code OnRunning();
    AppState::Code OnCleanup();
    void handleInput();
    void initRoms();
    void initModules();

    ubyte last_ascii = 0;
    kc85 kc;
    Draw draw;
    Audio audio;
    #if YAKC_UI
    UI ui;
    #endif
    TimePoint lapTimePoint;
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
    ioSetup.Assigns.Add("rom:", baseUrl);
    IO::Setup(ioSetup);

    // we only need few resources, so don't waste memory
    const int frameSize = 10;
    const int width = 2*frameSize + 2*320;
    const int height = 2*frameSize + 2*256;
    auto gfxSetup = GfxSetup::Window(width, height, "KC85");
    gfxSetup.SetPoolSize(GfxResourceType::Mesh, 4);
    gfxSetup.SetPoolSize(GfxResourceType::Texture, 4);
    gfxSetup.SetPoolSize(GfxResourceType::Pipeline, 4);
    gfxSetup.SetPoolSize(GfxResourceType::Shader, 4);
    Gfx::Setup(gfxSetup);
    Input::Setup();
    Input::BeginCaptureText();

    // initialize the ROM dumps and modules
    this->initRoms();

    #if YAKC_UI
    this->ui.Setup(this->kc);
    #endif
    this->kc.poweron(kc85_model::kc85_3, kc85_caos::caos_3_1);
    this->draw.Setup(gfxSetup, frameSize);
    this->audio.Setup(this->kc);
    this->initModules();

    // on KC85/3 put a 16kByte module into slot 8 by default, CAOS will initialize
    // this automatically on startup
    this->kc.exp.insert_module(0x08, kc85_exp::m022_16kbyte);

    this->lapTimePoint = Clock::Now();

    return AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
YakcApp::OnRunning() {
    Duration frameTime = Clock::LapTime(this->lapTimePoint);
    Gfx::ApplyDefaultRenderTarget(ClearState::ClearColor(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)));
    int micro_secs = (int) frameTime.AsMicroSeconds();
micro_secs = int(1000000.0 / 60.0);
    this->handleInput();
    #if YAKC_UI
        o_trace_begin(yakc_kc);
        this->kc.onframe(this->ui.Settings.cpuSpeed, micro_secs);
        o_trace_end();
        this->draw.UpdateParams(
            this->ui.Settings.crtEffect,
            this->ui.Settings.colorTV,
            glm::vec2(this->ui.Settings.crtWarp));
    #else
        o_trace_begin(yakc_kc);
        this->kc.onframe(1, micro_secs);
        o_trace_end();
        this->draw.UpdateParams(true, true, glm::vec2(1.0f/64.0f));
    #endif
    this->draw.Render(this->kc);
    #if YAKC_UI
    this->ui.OnFrame(this->kc);
    #endif
    Gfx::CommitFrame();
    this->audio.Update(micro_secs);
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
            // special case: shift-backspace clears screen shift-escape is STP
            if (kbd.KeyPressed(Key::LeftShift) && (key.key == Key::BackSpace)) {
                ascii = 0x0C;
            }
            else if (kbd.KeyPressed(Key::LeftShift) && (key.key == Key::Escape)) {
                ascii = 0x13;
            }
            else {
                ascii = key.ascii;
            }
            break;
        }
    }
    this->kc.put_key(ascii);
}

//------------------------------------------------------------------------------
void
YakcApp::initRoms() {

    // standard roms required for initial booting are built-in
    this->kc.roms.add(kc85_roms::caos31, dump_caos31, sizeof(dump_caos31));
    this->kc.roms.add(kc85_roms::basic_rom, dump_basic_c0, sizeof(dump_basic_c0));

    // async-load optional ROMs
    IO::Load("rom:hc900.852", [this](IO::LoadResult ioRes) {
        this->kc.roms.add(kc85_roms::hc900, ioRes.Data.Data(), ioRes.Data.Size());
    });
    IO::Load("rom:caos22.852", [this](IO::LoadResult ioRes) {
        this->kc.roms.add(kc85_roms::caos22, ioRes.Data.Data(), ioRes.Data.Size());
    });
    IO::Load("rom:caos34.853", [this](IO::LoadResult ioRes) {
        this->kc.roms.add(kc85_roms::caos34, ioRes.Data.Data(), ioRes.Data.Size());
    });
    IO::Load("rom:caos42c.854", [this](IO::LoadResult ioRes) {
        this->kc.roms.add(kc85_roms::caos42c, ioRes.Data.Data(), ioRes.Data.Size());
    });
    IO::Load("rom:caos42e.854", [this](IO::LoadResult ioRes) {
        this->kc.roms.add(kc85_roms::caos42e, ioRes.Data.Data(), ioRes.Data.Size());
    });
}

//------------------------------------------------------------------------------
void
YakcApp::initModules() {
    this->kc.exp.register_none_module("NO MODULE", "Click to insert module!");
    if (!kc.exp.slot_occupied(0x08)) {
        kc.exp.insert_module(0x08, kc85_exp::none);
    }
    if (!kc.exp.slot_occupied(0x0C)) {
        kc.exp.insert_module(0x0C, kc85_exp::none);
    }

    // M022 EXPANDER RAM
    this->kc.exp.register_ram_module(kc85_exp::m022_16kbyte, 0xC0, 0x4000,
        "16 KByte RAM expansion module.\n\n"
        "SWITCH [SLOT] 43: map to address 0x4000\n"
        "SWITCH [SLOT] 83: map to address 0x8000\n"
        "SWITCH [SLOT] 00: switch module off\n\n"
        "...where [SLOT] is 08 or 0C");

    // M011 64 K RAM
    this->kc.exp.register_ram_module(kc85_exp::m011_64kbyte, 0xC0, 0x10000,
        "64 KByte RAM expansion module.\n\n"
        "SWITCH [SLOT] 03: map 1st block to 0x0000\n"
        "SWITCH [SLOT] 43: map 1st block to 0x4000\n"
        "SWITCH [SLOT] 83: map 1st block to 0x8000\n"
        "SWITCH [SLOT] C3: map 1st block to 0xC000\n"
        "...where [SLOT] is 08 or 0C.\n");

    // M026 FORTH
    IO::Load("rom:forth.853", [this](IO::LoadResult ioRes) {
        this->kc.roms.add(kc85_roms::forth, ioRes.Data.Data(), ioRes.Data.Size());
        this->kc.exp.register_rom_module(kc85_exp::m026_forth, 0xE0,
            kc.roms.ptr(kc85_roms::forth), kc.roms.size(kc85_roms::forth),
            "FORTH language expansion module.\n\n"
            "First deactivate the BASIC ROM with:\n"
            "SWITCH 02 00\n\n"
            "Then activate FORTH with:\n"
            "SWITCH [SLOT] C1\n\n"
            "...where [SLOT] is 08 or 0C");
    });

    // M027 DEVELOPMENT
    IO::Load("rom:develop.853", [this](IO::LoadResult ioRes) {
        this->kc.roms.add(kc85_roms::develop, ioRes.Data.Data(), ioRes.Data.Size());
        this->kc.exp.register_rom_module(kc85_exp::m027_development, 0xE0,
            kc.roms.ptr(kc85_roms::develop), kc.roms.size(kc85_roms::develop),
            "Assembler/disassembler expansion module.\n\n"
            "First deactivate the BASIC ROM with:\n"
            "SWITCH 02 00\n\n"
            "Then activate the module with:\n"
            "SWITCH [SLOT] C1\n\n"
            "...where [SLOT] is 08 or 0C");
    });

    // M006 BASIC (+ HC-CAOS 901)
    IO::Load("rom:m006.rom", [this](IO::LoadResult ioRes) {
        this->kc.roms.add(kc85_roms::basic_mod, ioRes.Data.Data(), ioRes.Data.Size());
        this->kc.exp.register_rom_module(kc85_exp::m006_basic, 0xC0,
            kc.roms.ptr(kc85_roms::basic_mod), kc.roms.size(kc85_roms::basic_mod),
            "BASIC + HC-901 CAOS for KC85/2.\n\n"
            "Activate with:\n"
            "JUMP [SLOT]\n\n"
            "...where [SLOT] is 08 or 0C");
    });

    // M012 TEXOR
    IO::Load("rom:texor.rom", [this](IO::LoadResult ioRes) {
        this->kc.roms.add(kc85_roms::texor, ioRes.Data.Data(), ioRes.Data.Size());
        this->kc.exp.register_rom_module(kc85_exp::m012_texor, 0xE0,
            kc.roms.ptr(kc85_roms::texor), kc.roms.size(kc85_roms::texor),
            "TEXOR text processing software.\n\n"
            "First deactivate the BASIC ROM with:\n"
            "SWITCH 02 00\n\n"
            "Then activate the module with:\n"
            "SWITCH [SLOT] C1\n\n"
            "...where [SLOT] is 08 or 0C");
    });
}
