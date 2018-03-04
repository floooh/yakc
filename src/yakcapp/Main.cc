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
#include "HttpFS/HTTPFileSystem.h"
#include "yakc/yakc.h"
#include "yakc/systems/kc85.h"
#include "yakc_oryol/Draw.h"
#include "yakc_oryol/Audio.h"
#include "yakc_oryol/Keyboard.h"
#if YAKC_UI
#include "yakc_ui/UI.h"
#endif
#include "yakc/roms/rom_dumps.h"
#if ORYOL_EMSCRIPTEN
#include <emscripten/emscripten.h>
#endif

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
    gfxSetup.ResourcePoolSize[GfxResourceType::Mesh] = 8;
    gfxSetup.ResourcePoolSize[GfxResourceType::Texture] = 8;
    gfxSetup.ResourcePoolSize[GfxResourceType::Pipeline] = 8;
    gfxSetup.ResourcePoolSize[GfxResourceType::Shader] = 8;
    gfxSetup.HtmlTrackElementSize = true;
    gfxSetup.HtmlElement = "canvas";
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
    if (kc85.on) {
        kc85.exp.insert_module(0x08, kc85_exp::m022_16kbyte);
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
    this->keyboard.hasInputFocus = !ImGui::GetIO().WantCaptureKeyboard;
    #endif
    this->keyboard.HandleInput();

    int micro_secs = (int) frameTime.AsMicroSeconds();
    uint64_t processed_audio_cycles = this->audio.GetProcessedCycles();
    TimePoint emu_start_time = Clock::Now();
    #if YAKC_UI
        // keep CPU synchronized to a small time window ahead of audio playback
        this->emu.exec(micro_secs, processed_audio_cycles);
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
    Gfx::BeginPass(PassAction::Clear(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)));
    const void* fb = this->emu.framebuffer(width, height);
    if (fb) {
        this->draw.Render(fb, width, height);
    }
    #if YAKC_UI
    this->ui.OnFrame(this->emu);
    #endif
    Gfx::EndPass();
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
    IO::Load("rom:cpc6128_amsdos.bin", [this](IO::LoadResult ioRes) {
        this->emu.add_rom(rom_images::cpc6128_amsdos, ioRes.Data.Data(), ioRes.Data.Size());
    });
    IO::Load("rom:kcc_os.bin", [this](IO::LoadResult ioRes) {
        this->emu.add_rom(rom_images::kcc_os, ioRes.Data.Data(), ioRes.Data.Size());
    });
    IO::Load("rom:kcc_bas.bin", [this](IO::LoadResult ioRes) {
        this->emu.add_rom(rom_images::kcc_basic, ioRes.Data.Data(), ioRes.Data.Size());
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
    IO::Load("rom:c64_kernalv3.bin", [this](IO::LoadResult ioRes) {
        this->emu.add_rom(rom_images::c64_kernalv3, ioRes.Data.Data(), ioRes.Data.Size());
    });
    IO::Load("rom:c64_char.bin", [this](IO::LoadResult ioRes) {
        this->emu.add_rom(rom_images::c64_char, ioRes.Data.Data(), ioRes.Data.Size());
    });
    IO::Load("rom:c64_basic.bin", [this](IO::LoadResult ioRes) {
        this->emu.add_rom(rom_images::c64_basic, ioRes.Data.Data(), ioRes.Data.Size());
    });
}

//------------------------------------------------------------------------------
void
YakcApp::initModules() {
    kc85.exp.register_none_module("NO MODULE", "Click to insert module!");
    if (!kc85.exp.slot_occupied(0x08)) {
        kc85.exp.insert_module(0x08, kc85_exp::none);
    }
    if (!kc85.exp.slot_occupied(0x0C)) {
        kc85.exp.insert_module(0x0C, kc85_exp::none);
    }

    // M022 EXPANDER RAM
    kc85.exp.register_ram_module(kc85_exp::m022_16kbyte, 0xC0, 0x4000,
        "16 KByte RAM expansion module.\n\n"
        "SWITCH [SLOT] 43: map to address 0x4000\n"
        "SWITCH [SLOT] 83: map to address 0x8000\n"
        "SWITCH [SLOT] 00: switch module off\n\n"
        "...where [SLOT] is 08 or 0C");

    // M011 64 K RAM
    kc85.exp.register_ram_module(kc85_exp::m011_64kbyte, 0xC0, 0x10000,
        "64 KByte RAM expansion module.\n\n"
        "SWITCH [SLOT] 03: map 1st block to 0x0000\n"
        "SWITCH [SLOT] 43: map 1st block to 0x4000\n"
        "SWITCH [SLOT] 83: map 1st block to 0x8000\n"
        "SWITCH [SLOT] C3: map 1st block to 0xC000\n"
        "...where [SLOT] is 08 or 0C.\n");

    // M026 FORTH
    IO::Load("rom:forth.853", [this](IO::LoadResult ioRes) {
        this->emu.add_rom(rom_images::forth, ioRes.Data.Data(), ioRes.Data.Size());
        kc85.exp.register_rom_module(kc85_exp::m026_forth, 0xE0,
            roms.ptr(rom_images::forth), roms.size(rom_images::forth),
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
        kc85.exp.register_rom_module(kc85_exp::m027_development, 0xE0,
            roms.ptr(rom_images::develop), roms.size(rom_images::develop),
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
        kc85.exp.register_rom_module(kc85_exp::m006_basic, 0xC0,
            roms.ptr(rom_images::kc85_basic_mod), roms.size(rom_images::kc85_basic_mod),
            "BASIC + HC-901 CAOS for KC85/2.\n\n"
            "Activate with:\n"
            "JUMP [SLOT]\n\n"
            "...where [SLOT] is 08 or 0C");
    });

    // M012 TEXOR
    IO::Load("rom:texor.rom", [this](IO::LoadResult ioRes) {
        this->emu.add_rom(rom_images::texor, ioRes.Data.Data(), ioRes.Data.Size());
        kc85.exp.register_rom_module(kc85_exp::m012_texor, 0xE0,
            roms.ptr(rom_images::texor), roms.size(rom_images::texor),
            "TEXOR text processing software.\n\n"
            "First deactivate the BASIC ROM with:\n"
            "SWITCH 02 00\n\n"
            "Then activate the module with:\n"
            "SWITCH [SLOT] C1\n\n"
            "...where [SLOT] is 08 or 0C");
    });
}

//------------------------------------------------------------------------------
//  Javascript interface functions
//
#if ORYOL_EMSCRIPTEN
extern "C" {

EMSCRIPTEN_KEEPALIVE void yakc_boot(const char* sys_str, const char* os_str) {
    auto* app = YakcApp::self;
    if (app) {
        auto sys = system_from_string(sys_str);
        os_rom os = os_from_string(os_str);
        if (system::none != sys) {
            app->emu.poweroff();
            app->emu.poweron(sys, os);
        }
    }
}

EMSCRIPTEN_KEEPALIVE int yakc_toggle_ui() {
    auto* app = YakcApp::self;
    if (app) {
        app->ui.Toggle();
        return app->ui.uiEnabled ? 1:0;
    }
    return 0;
}

EMSCRIPTEN_KEEPALIVE int yakc_toggle_keyboard() {
    auto* app = YakcApp::self;
    if (app) {
        app->ui.ToggleKeyboard(app->emu);
        return app->ui.keyboardWindow ? 1:0;
    }
    return 0;
}

EMSCRIPTEN_KEEPALIVE int yakc_toggle_joystick() {
    auto* app = YakcApp::self;
    if (app) {
        app->emu.enable_joystick(!app->emu.is_joystick_enabled());
        return app->emu.is_joystick_enabled() ? 1:0;
    }
    return 0;
}

EMSCRIPTEN_KEEPALIVE int yakc_toggle_crt() {
    auto* app = YakcApp::self;
    if (app) {
        app->ui.Settings.crtEffect = !app->ui.Settings.crtEffect;
        return app->ui.Settings.crtEffect ? 1:0;
    }
    return 0;
}

EMSCRIPTEN_KEEPALIVE void yakc_power() {
    auto* app = YakcApp::self;
    if (app) {
        app->emu.poweroff();
        app->emu.poweron(app->emu.model, app->emu.os);
    }
}

EMSCRIPTEN_KEEPALIVE void yakc_reset() {
    if (YakcApp::self) {
        YakcApp::self->emu.reset();
    }
}

EMSCRIPTEN_KEEPALIVE const char* yakc_get_system() {
    if (YakcApp::self) {
        return string_from_system(YakcApp::self->emu.model);
    }
    else {
        return "";
    }
}

EMSCRIPTEN_KEEPALIVE void yakc_quickload(const char* name, const char* filename, const char* filetype_str, const char* sys_str) {
    Log::Info("yakc_quickload(%s, %s, %s)\n", filename, filetype_str, sys_str);
    if (YakcApp::self) {
        auto* app = YakcApp::self;
        auto sys = system_from_string(sys_str);
        filetype type = filetype_from_string(filetype_str);
        FileLoader::Item item(name, filename, type, sys, false);
        if (int(item.Compat) & int(app->emu.model)) {
            if (filetype_quickloadable(item.Type)) {
                app->ui.FileLoader.LoadAndStart(item);
            }
            else {
                // load through tape deck
                app->ui.FileLoader.LoadTape(item);

                // send the right load/run BASIC command to the emulator
                String cmd;
                if (app->emu.is_system(system::any_cpc)) {
                    cmd = "run\"\n\n";
                }
                else if (app->emu.is_system(system::acorn_atom)) {
                    cmd = "*LOAD\n\n";
                }
                Buffer buf;
                buf.Add((const uint8_t*)cmd.AsCStr(), cmd.Length()+1);
                app->keyboard.StartPlayback(std::move(buf));
            }
        }
    }
}

EMSCRIPTEN_KEEPALIVE int yakc_loadfile(const char* filename, const uint8_t* data, int size) {
    Log::Info("yakc_loadfile(%s, %p, %d)\n", filename, data, size);
    if (YakcApp::self) {
        auto* app = YakcApp::self;
        FileLoader::Item item(filename, filename, filetype::none, system::any);
        FileLoader* loader = FileLoader::pointer;
        loader->FileData.Clear();
        loader->FileData.Add(data, size);
        loader->Info = loader->parseHeader(loader->FileData, item);
        loader->State = FileLoader::Ready;
        if (int(loader->Info.RequiredSystem) & int(app->emu.model)) {
            if (filetype_quickloadable(loader->Info.Type)) {
                loader->quickload(&app->emu, loader->Info, loader->FileData, true);
                return 1;
            }
            else if (filetype::none != loader->Info.Type){
                // load through tape deck
                tape.insert_tape(filename, loader->Info.Type, data, size);

                // send the right load/run BASIC command to the emulator
                String cmd;
                if (app->emu.is_system(system::any_cpc)) {
                    cmd = "run\"\n\n";
                }
                else if (app->emu.is_system(system::acorn_atom)) {
                    cmd = "*LOAD\n\n";
                }
                Buffer buf;
                buf.Add((const uint8_t*)cmd.AsCStr(), cmd.Length()+1);
                app->keyboard.StartPlayback(std::move(buf));
                return 1;
            }
            else {
                Log::Warn("filetype not accepted\n");
            }
        }
        else {
            Log::Warn("filetype not compatible with current system\n");
        }
    }
    return 0;
}

} // extern "C"
#endif

