//------------------------------------------------------------------------------
//  main.cc
//  yakc app main source.
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/Main.h"
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

using namespace Oryol;
using namespace YAKC;

class YakcApp : public App {
public:
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
    const int frameSizeX = 32;
    const int frameSizeY = 16;
    const int width = 2*frameSizeX + 2*320;
    const int height = 2*frameSizeY + 2*256;
    auto gfxSetup = GfxSetup::Window(width, height, "YAKC Emulator");
    gfxSetup.SetPoolSize(GfxResourceType::Mesh, 8);
    gfxSetup.SetPoolSize(GfxResourceType::Texture, 8);
    gfxSetup.SetPoolSize(GfxResourceType::Pipeline, 8);
    gfxSetup.SetPoolSize(GfxResourceType::Shader, 8);
    Gfx::Setup(gfxSetup);
    Input::Setup();

    // initialize the emulator
    ext_funcs sys_funcs;
    sys_funcs.assertmsg_func = Log::AssertMsg;
    sys_funcs.malloc_func = [] (size_t s) -> void* { return Oryol::Memory::Alloc((int)s); };
    sys_funcs.free_func = [] (void* p) { Oryol::Memory::Free(p); };
    sound_funcs snd_funcs;
    snd_funcs.userdata = &this->audio;
    snd_funcs.sound = Audio::cb_sound;
    snd_funcs.volume = Audio::cb_volume;
    snd_funcs.stop = Audio::cb_stop;
    this->emu.init(sys_funcs, snd_funcs);

    // initialize the ROM dumps and modules
    this->initRoms();

    // switch the emulator on
    this->emu.poweron(device::kc85_3, os_rom::caos_3_1);

    this->draw.Setup(gfxSetup, frameSizeX, frameSizeY);
    this->audio.Setup(this->emu.board.clck);
    this->keyboard.Setup(this->emu);
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
    uint64_t min_cycle_count = 0;
    uint64_t max_cycle_count = 0;
    const uint64_t audio_cycle_count = this->audio.GetProcessedCycles();
    if (audio_cycle_count > 0) {
        const uint64_t cpu_min_ahead_cycles = (this->emu.board.clck.base_freq_khz*1000)/100;
        const uint64_t cpu_max_ahead_cycles = (this->emu.board.clck.base_freq_khz*1000)/25;
        min_cycle_count = audio_cycle_count + cpu_min_ahead_cycles;
        max_cycle_count = audio_cycle_count + cpu_max_ahead_cycles;
    }

    #if YAKC_UI
        o_trace_begin(yakc_kc);
        // keep CPU synchronized to a small time window ahead of audio playback
        this->emu.onframe(this->ui.Settings.cpuSpeed, micro_secs, min_cycle_count, max_cycle_count);
        o_trace_end();
        this->draw.UpdateParams(
            this->ui.Settings.crtEffect,
            this->ui.Settings.colorTV,
            glm::vec2(this->ui.Settings.crtWarp));
    #else
        o_trace_begin(yakc_kc);
        this->emu.onframe(2, micro_secs, min_cycle_count, max_cycle_count);
        o_trace_end();
        this->draw.UpdateParams(true, true, glm::vec2(1.0f/64.0f));
    #endif
    this->audio.Update(this->emu.board.clck);
    if (this->emu.kc85.on) {
        this->draw.Render(this->emu.kc85.video.rgba8_buffer, 320, 256);
    }
    else if (this->emu.z9001.on) {
        this->draw.Render(this->emu.z9001.rgba8_buffer, 320, 192);
    }
    else if (this->emu.z1013.on) {
        this->draw.Render(this->emu.z1013.rgba8_buffer, 256, 256);
    }
    else if (this->emu.zx.on) {
        // NOTE: ZX only has 256x192 framebuffer, but we put
        // it into a 320x256 buffer and include the border color there
        this->draw.Render(this->emu.zx.rgba8_buffer, 320, 256);
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

    // standard roms required for initial booting are built-in
    this->emu.kc85.roms.add(kc85_roms::caos31, dump_caos31, sizeof(dump_caos31));
    this->emu.kc85.roms.add(kc85_roms::basic_rom, dump_basic_c0, sizeof(dump_basic_c0));
    this->emu.kc85.roms.add(kc85_roms::caos42c, dump_caos42c, sizeof(dump_caos42c));
    this->emu.kc85.roms.add(kc85_roms::caos42e, dump_caos42e, sizeof(dump_caos42e));

    // async-load optional ROMs
    IO::Load("rom:hc900.852", [this](IO::LoadResult ioRes) {
        this->emu.kc85.roms.add(kc85_roms::hc900, ioRes.Data.Data(), ioRes.Data.Size());
    });
    IO::Load("rom:caos22.852", [this](IO::LoadResult ioRes) {
        this->emu.kc85.roms.add(kc85_roms::caos22, ioRes.Data.Data(), ioRes.Data.Size());
    });
    IO::Load("rom:caos34.853", [this](IO::LoadResult ioRes) {
        this->emu.kc85.roms.add(kc85_roms::caos34, ioRes.Data.Data(), ioRes.Data.Size());
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
        kc85& kc = this->emu.kc85;
        kc.roms.add(kc85_roms::forth, ioRes.Data.Data(), ioRes.Data.Size());
        kc.exp.register_rom_module(kc85_exp::m026_forth, 0xE0,
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
        kc85& kc = this->emu.kc85;
        kc.roms.add(kc85_roms::develop, ioRes.Data.Data(), ioRes.Data.Size());
        kc.exp.register_rom_module(kc85_exp::m027_development, 0xE0,
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
        kc85& kc = this->emu.kc85;
        kc.roms.add(kc85_roms::basic_mod, ioRes.Data.Data(), ioRes.Data.Size());
        kc.exp.register_rom_module(kc85_exp::m006_basic, 0xC0,
            kc.roms.ptr(kc85_roms::basic_mod), kc.roms.size(kc85_roms::basic_mod),
            "BASIC + HC-901 CAOS for KC85/2.\n\n"
            "Activate with:\n"
            "JUMP [SLOT]\n\n"
            "...where [SLOT] is 08 or 0C");
    });

    // M012 TEXOR
    IO::Load("rom:texor.rom", [this](IO::LoadResult ioRes) {
        kc85& kc = this->emu.kc85;
        kc.roms.add(kc85_roms::texor, ioRes.Data.Data(), ioRes.Data.Size());
        kc.exp.register_rom_module(kc85_exp::m012_texor, 0xE0,
            kc.roms.ptr(kc85_roms::texor), kc.roms.size(kc85_roms::texor),
            "TEXOR text processing software.\n\n"
            "First deactivate the BASIC ROM with:\n"
            "SWITCH 02 00\n\n"
            "Then activate the module with:\n"
            "SWITCH [SLOT] C1\n\n"
            "...where [SLOT] is 08 or 0C");
    });
}

