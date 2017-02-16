//------------------------------------------------------------------------------
//  main.cc
//  yakc app main source.
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/Main.h"
#include "Core/String/StringBuilder.h"
#include "Core/Time/Clock.h"
#include "Gfx/Gfx.h"
#include "Input/Input.h"
#include "IO/IO.h"
#include "HTTP/HTTPFileSystem.h"
#include "yakc/yakc.h"
#include "yakc_oryol/Draw.h"
#include "yakc_oryol/Audio.h"
#include "yakc_oryol/Keyboard.h"
#if YAKC_UI
#include "yakc_ui/UI.h"
#endif
#include "yakc/roms/rom_dumps.h"

using namespace Oryol;
using namespace YAKC;

class YakcApp : public App {
public:
    static YakcApp* self;
    YakcApp() { self = this; };
    ~YakcApp() { self = nullptr; };
    AppState::Code OnInit();
    AppState::Code OnRunning();
    AppState::Code OnCleanup();
    void initRoms();
    void initModules();

    yakc emu;
    Draw draw;
    Audio audio;
    Keyboard keyboard;
    #if YAKC_UI
    UI ui;
    #endif
    TimePoint lapTimePoint;
};
OryolMain(YakcApp);

YakcApp* YakcApp::self = nullptr;

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
    const int frameSizeX = 24;
    const int frameSizeY = 8;
    const int width = 2*frameSizeX + 2*320;
    const int height = 2*frameSizeY + 2*256;
    auto gfxSetup = GfxSetup::Window(width, height, "YAKC Emulator");
    gfxSetup.SetPoolSize(GfxResourceType::Mesh, 8);
    gfxSetup.SetPoolSize(GfxResourceType::Texture, 8);
    gfxSetup.SetPoolSize(GfxResourceType::Pipeline, 8);
    gfxSetup.SetPoolSize(GfxResourceType::Shader, 8);
    gfxSetup.HtmlTrackElementSize = true;
    gfxSetup.HtmlElement = "game-border";
    Gfx::Setup(gfxSetup);
    Input::Setup();

    // initialize Oryol platform wrappers
    this->draw.Setup(gfxSetup, frameSizeX, frameSizeY);
    this->audio.Setup(&this->emu);
    this->keyboard.Setup(this->emu);

    // initialize the emulator
    ext_funcs sys_funcs;
    sys_funcs.assertmsg_func = Oryol::Log::AssertMsg;
    sys_funcs.malloc_func = [] (size_t s) -> void* { return Oryol::Memory::Alloc((int)s); };
    sys_funcs.free_func = [] (void* p) { Oryol::Memory::Free(p); };
    this->emu.init(sys_funcs);

    // initialize the ROM dumps and modules
    this->initRoms();

    // switch the emulator on
    this->emu.poweron(YAKC::system::kc85_3, os_rom::caos_3_1);

    #if YAKC_UI
    this->ui.Setup(this->emu, &this->audio);
    #endif

    // on KC85/3 put a 16kByte module into slot 8 by default, CAOS will initialize
    // this automatically on startup
    this->initModules();
    if (this->emu.kc85.on) {
        this->emu.kc85.exp.insert_module(0x08, kc85_exp::m022_16kbyte);
    }

    this->lapTimePoint = Clock::Now();

    return AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
YakcApp::OnRunning() {
    #if ORYOL_DEBUG
    Duration frameTime = Duration::FromSeconds(1.0/60.0);
    #else
    Duration frameTime = Clock::LapTime(this->lapTimePoint);
    #endif

    #if YAKC_UI
    // toggle UI?
    if (Input::KeyDown(Key::Tab)) {
        this->ui.Toggle();
    }
    // check if a text dump is ready to feed into keyboard handler
    if (this->ui.FileLoader.State == FileLoader::TextReady) {
        this->keyboard.StartPlayback(this->ui.FileLoader.ObtainTextBuffer());
    }
    // don't handle KC input if IMGUI has the keyboard focus
    if (!ImGui::GetIO().WantCaptureKeyboard) {
        this->keyboard.HandleInput();
    }
    #else
    this->keyboard.HandleInput();
    #endif

    glm::vec4 clear;
    this->emu.border_color(clear.x, clear.y, clear.z);
    clear.w = 1.0f;
    Gfx::ApplyDefaultRenderTarget(ClearState::ClearColor(clear));
    int micro_secs = (int) frameTime.AsMicroSeconds();
    uint64_t processed_audio_cycles = this->audio.GetProcessedCycles();
    TimePoint emu_start_time = Clock::Now();
    #if YAKC_UI
        o_trace_begin(yakc_kc);
        // keep CPU synchronized to a small time window ahead of audio playback
        this->emu.step(micro_secs, processed_audio_cycles);
        o_trace_end();
        this->draw.UpdateParams(
            this->ui.Settings.crtEffect,
            this->ui.Settings.colorTV,
            glm::vec2(this->ui.Settings.crtWarp));
    #else
        o_trace_begin(yakc_kc);
        this->emu.onframe(micro_secs, processed_audio_cycles);
        o_trace_end();
        this->draw.UpdateParams(true, true, glm::vec2(1.0f/64.0f));
    #endif
    this->ui.EmulationTime = Clock::Since(emu_start_time);
    this->audio.Update();
    int width = 0;
    int height = 0;
    const void* fb = this->emu.framebuffer(width, height);
    if (fb) {
        this->draw.Render(fb, width, height);
    }
    #if YAKC_UI
    this->ui.OnFrame(this->emu);
    #endif
    Gfx::CommitFrame();
    return Gfx::QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
YakcApp::OnCleanup() {
    this->keyboard.Discard();
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
YakcApp::initRoms() {

    // only KC85/3 roms are 'built-in' to reeduce executable size
    this->emu.add_rom(rom_images::caos31, dump_caos31, sizeof(dump_caos31));
    this->emu.add_rom(rom_images::kc85_basic_rom, dump_basic_c0, sizeof(dump_basic_c0));

    // async-load optional ROMs
    IO::Load("rom:hc900.852", [this](IO::LoadResult ioRes) {
        this->emu.add_rom(rom_images::hc900, ioRes.Data.Data(), ioRes.Data.Size());
    });
    IO::Load("rom:caos22.852", [this](IO::LoadResult ioRes) {
        this->emu.add_rom(rom_images::caos22, ioRes.Data.Data(), ioRes.Data.Size());
    });
    IO::Load("rom:caos34.853", [this](IO::LoadResult ioRes) {
        this->emu.add_rom(rom_images::caos34, ioRes.Data.Data(), ioRes.Data.Size());
    });
    IO::Load("rom:caos42c.854", [this](IO::LoadResult ioRes) {
        this->emu.add_rom(rom_images::caos42c, ioRes.Data.Data(), ioRes.Data.Size());
    });
    IO::Load("rom:caos42e.854", [this](IO::LoadResult ioRes) {
        this->emu.add_rom(rom_images::caos42e, ioRes.Data.Data(), ioRes.Data.Size());
    });
    IO::Load("rom:z1013_mon202.bin", [this](IO::LoadResult ioRes) {
        this->emu.add_rom(rom_images::z1013_mon202, ioRes.Data.Data(), ioRes.Data.Size());
    });
    IO::Load("rom:z1013_mon_a2.bin", [this](IO::LoadResult ioRes) {
        this->emu.add_rom(rom_images::z1013_mon_a2, ioRes.Data.Data(), ioRes.Data.Size());
    });
    IO::Load("rom:z1013_font.bin", [this](IO::LoadResult ioRes) {
        this->emu.add_rom(rom_images::z1013_font, ioRes.Data.Data(), ioRes.Data.Size());
    });
    IO::Load("rom:z9001_os12_1.bin", [this](IO::LoadResult ioRes) {
        this->emu.add_rom(rom_images::z9001_os12_1, ioRes.Data.Data(), ioRes.Data.Size());
    });
    IO::Load("rom:z9001_os12_2.bin", [this](IO::LoadResult ioRes) {
        this->emu.add_rom(rom_images::z9001_os12_2, ioRes.Data.Data(), ioRes.Data.Size());
    });
    IO::Load("rom:z9001_font.bin", [this](IO::LoadResult ioRes) {
        this->emu.add_rom(rom_images::z9001_font, ioRes.Data.Data(), ioRes.Data.Size());
    });
    IO::Load("rom:z9001_basic.bin", [this](IO::LoadResult ioRes) {
        this->emu.add_rom(rom_images::z9001_basic, ioRes.Data.Data(), ioRes.Data.Size());
    });
    IO::Load("rom:kc87_os_2.bin", [this](IO::LoadResult ioRes) {
        this->emu.add_rom(rom_images::kc87_os_2, ioRes.Data.Data(), ioRes.Data.Size());
    });
    IO::Load("rom:z9001_basic_507_511.bin", [this](IO::LoadResult ioRes) {
        this->emu.add_rom(rom_images::z9001_basic_507_511, ioRes.Data.Data(), ioRes.Data.Size());
    });
    IO::Load("rom:kc87_font_2.bin", [this](IO::LoadResult ioRes) {
        this->emu.add_rom(rom_images::kc87_font_2, ioRes.Data.Data(), ioRes.Data.Size());
    });
    IO::Load("rom:amstrad_zx48k.bin", [this](IO::LoadResult ioRes) {
        this->emu.add_rom(rom_images::zx48k, ioRes.Data.Data(), ioRes.Data.Size());
    });
    IO::Load("rom:amstrad_zx128k_0.bin", [this](IO::LoadResult ioRes) {
        this->emu.add_rom(rom_images::zx128k_0, ioRes.Data.Data(), ioRes.Data.Size());
    });
    IO::Load("rom:amstrad_zx128k_1.bin", [this](IO::LoadResult ioRes) {
        this->emu.add_rom(rom_images::zx128k_1, ioRes.Data.Data(), ioRes.Data.Size());
    });
    IO::Load("rom:cpc464_os.bin", [this](IO::LoadResult ioRes) {
        this->emu.add_rom(rom_images::cpc464_os, ioRes.Data.Data(), ioRes.Data.Size());
    });
    IO::Load("rom:cpc464_basic.bin", [this](IO::LoadResult ioRes) {
        this->emu.add_rom(rom_images::cpc464_basic, ioRes.Data.Data(), ioRes.Data.Size());
    });
    IO::Load("rom:cpc6128_os.bin", [this](IO::LoadResult ioRes) {
        this->emu.add_rom(rom_images::cpc6128_os, ioRes.Data.Data(), ioRes.Data.Size());
    });
    IO::Load("rom:cpc6128_basic.bin", [this](IO::LoadResult ioRes) {
        this->emu.add_rom(rom_images::cpc6128_basic, ioRes.Data.Data(), ioRes.Data.Size());
    });
    IO::Load("rom:kcc_os.bin", [this](IO::LoadResult ioRes) {
        this->emu.add_rom(rom_images::kcc_os, ioRes.Data.Data(), ioRes.Data.Size());
    });
    IO::Load("rom:kcc_bas.bin", [this](IO::LoadResult ioRes) {
        this->emu.add_rom(rom_images::kcc_basic, ioRes.Data.Data(), ioRes.Data.Size());
    });
    IO::Load("rom:bbc_b_os12.rom", [this](IO::LoadResult ioRes) {
        this->emu.add_rom(rom_images::bbcmicro_b_os, ioRes.Data.Data(), ioRes.Data.Size());
    });
    IO::Load("rom:bbc_b_basic2.rom", [this](IO::LoadResult ioRes) {
        this->emu.add_rom(rom_images::bbcmicro_b_basic, ioRes.Data.Data(), ioRes.Data.Size());
    });
    IO::Load("rom:abasic.ic20", [this](IO::LoadResult ioRes) {
        this->emu.add_rom(rom_images::atom_basic, ioRes.Data.Data(), ioRes.Data.Size());
    });
    IO::Load("rom:afloat.ic21", [this](IO::LoadResult ioRes) {
        this->emu.add_rom(rom_images::atom_float, ioRes.Data.Data(), ioRes.Data.Size());
    });
    IO::Load("rom:dosrom.u15", [this](IO::LoadResult ioRes) {
        this->emu.add_rom(rom_images::atom_dos, ioRes.Data.Data(), ioRes.Data.Size());
    });
}

//------------------------------------------------------------------------------
void
YakcApp::initModules() {
    kc85& kc = this->emu.kc85;
    kc.exp.register_none_module("NO MODULE", "Click to insert module!");
    if (!kc.exp.slot_occupied(0x08)) {
        kc.exp.insert_module(0x08, kc85_exp::none);
    }
    if (!kc.exp.slot_occupied(0x0C)) {
        kc.exp.insert_module(0x0C, kc85_exp::none);
    }

    // M022 EXPANDER RAM
    kc.exp.register_ram_module(kc85_exp::m022_16kbyte, 0xC0, 0x4000,
        "16 KByte RAM expansion module.\n\n"
        "SWITCH [SLOT] 43: map to address 0x4000\n"
        "SWITCH [SLOT] 83: map to address 0x8000\n"
        "SWITCH [SLOT] 00: switch module off\n\n"
        "...where [SLOT] is 08 or 0C");

    // M011 64 K RAM
    kc.exp.register_ram_module(kc85_exp::m011_64kbyte, 0xC0, 0x10000,
        "64 KByte RAM expansion module.\n\n"
        "SWITCH [SLOT] 03: map 1st block to 0x0000\n"
        "SWITCH [SLOT] 43: map 1st block to 0x4000\n"
        "SWITCH [SLOT] 83: map 1st block to 0x8000\n"
        "SWITCH [SLOT] C3: map 1st block to 0xC000\n"
        "...where [SLOT] is 08 or 0C.\n");

    // M026 FORTH
    IO::Load("rom:forth.853", [this](IO::LoadResult ioRes) {
        this->emu.add_rom(rom_images::forth, ioRes.Data.Data(), ioRes.Data.Size());
        this->emu.kc85.exp.register_rom_module(kc85_exp::m026_forth, 0xE0,
            this->emu.roms.ptr(rom_images::forth), this->emu.roms.size(rom_images::forth),
            "FORTH language expansion module.\n\n"
            "First deactivate the BASIC ROM with:\n"
            "SWITCH 02 00\n\n"
            "Then activate FORTH with:\n"
            "SWITCH [SLOT] C1\n\n"
            "...where [SLOT] is 08 or 0C");
    });

    // M027 DEVELOPMENT
    IO::Load("rom:develop.853", [this](IO::LoadResult ioRes) {
        this->emu.add_rom(rom_images::develop, ioRes.Data.Data(), ioRes.Data.Size());
        this->emu.kc85.exp.register_rom_module(kc85_exp::m027_development, 0xE0,
            this->emu.roms.ptr(rom_images::develop), this->emu.roms.size(rom_images::develop),
            "Assembler/disassembler expansion module.\n\n"
            "First deactivate the BASIC ROM with:\n"
            "SWITCH 02 00\n\n"
            "Then activate the module with:\n"
            "SWITCH [SLOT] C1\n\n"
            "...where [SLOT] is 08 or 0C");
    });

    // M006 BASIC (+ HC-CAOS 901)
    IO::Load("rom:m006.rom", [this](IO::LoadResult ioRes) {
        this->emu.add_rom(rom_images::kc85_basic_mod, ioRes.Data.Data(), ioRes.Data.Size());
        this->emu.kc85.exp.register_rom_module(kc85_exp::m006_basic, 0xC0,
            this->emu.roms.ptr(rom_images::kc85_basic_mod), this->emu.roms.size(rom_images::kc85_basic_mod),
            "BASIC + HC-901 CAOS for KC85/2.\n\n"
            "Activate with:\n"
            "JUMP [SLOT]\n\n"
            "...where [SLOT] is 08 or 0C");
    });

    // M012 TEXOR
    IO::Load("rom:texor.rom", [this](IO::LoadResult ioRes) {
        this->emu.add_rom(rom_images::texor, ioRes.Data.Data(), ioRes.Data.Size());
        this->emu.kc85.exp.register_rom_module(kc85_exp::m012_texor, 0xE0,
            this->emu.roms.ptr(rom_images::texor), this->emu.roms.size(rom_images::texor),
            "TEXOR text processing software.\n\n"
            "First deactivate the BASIC ROM with:\n"
            "SWITCH 02 00\n\n"
            "Then activate the module with:\n"
            "SWITCH [SLOT] C1\n\n"
            "...where [SLOT] is 08 or 0C");
    });
}

//------------------------------------------------------------------------------
//  emscripten-specific website communication function to pass string
//  messages into the emscripten app
//
extern "C" {

void emsc_put_msg(const char* msg) {
    Log::Info("Msg from website: %s\n", msg);
    if (YakcApp::self) {
        YakcApp* app = YakcApp::self;
        StringBuilder strBuilder(msg);
        Array<String> tok;
        strBuilder.Tokenize(" ", tok);
        if (tok.Size() > 0) {
            if (tok[0] == "toggle-ui") {
                #if YAKC_UI
                app->ui.Toggle();
                #endif
            }
            else if (tok[0] == "toggle-keyboard") {
                #if YAKC_UI
                app->ui.ToggleKeyboard(YakcApp::self->emu);
                #endif
            }
            else if (tok[0] == "toggle-joystick") {
                app->emu.enable_joystick(!YakcApp::self->emu.is_joystick_enabled());
            }
            else if ((tok[0] == "boot") && (tok.Size() == 3)) {
                enum system sys = system_from_string(tok[1].AsCStr());
                os_rom os = os_from_string(tok[2].AsCStr());
                if (system::none != sys) {
                    app->emu.poweroff();
                    app->emu.poweron(sys, os);
                }
            }
        }
    }
}

} // extern "C"


