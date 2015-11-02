//------------------------------------------------------------------------------
//  main.cc
//  yakc app main source.
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/App.h"
#include "Gfx/Gfx.h"
#include "Input/Input.h"
#include "kc85_oryol.h"
#include "ui.h"
#include "draw.h"

using namespace Oryol;
using namespace yakc;

class YakcApp : public App {
public:
    AppState::Code OnInit();
    AppState::Code OnRunning();
    AppState::Code OnCleanup();

    Id drawState;
    kc85 kc;
    class ui ui;
    class draw draw;
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
    this->kc.pause(true);
    this->ui.setup(this->kc);

    // a little test prog so we can actually see something
    ubyte prog[] = {
        // clear color memory
        0x21, 0x00, 0xA8,   // LD HL,0xA800
        0x11, 0x01, 0xA8,   // LD DE,0xA800
        0x01, 0xFF, 0x0B,   // LD BC,0x0A00
        0x36, 0x22,         // LD (HL),0x11
        0xED, 0xB0,         // LDIR

        // clear video memory
        0x3E, 0x01,         // LD A,0x01

        0x21, 0x00, 0x80,   // LD HL,0x8000
        0x11, 0x01, 0x80,   // LD DE,0x8001
        0x01, 0xFF, 0x27,   // LD BC,0x2800
        0x77,               // LD (HL),A
        0xED, 0xB0,         // LDIR

        // rotate clear-value and loop to clear vid mem
        0x07,               // RLCA
        0xC3, 0x0F, 0x02,   // JP 0x020F
    };
    this->kc.cpu.mem.write(0x0200, prog, sizeof(prog));
    this->kc.cpu.state.PC = 0x200;

    // setup the renderer
    this->draw.setup(gfxSetup);
    
    return AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
YakcApp::OnRunning() {
    Gfx::ApplyDefaultRenderTarget(ClearState::ClearColor(glm::vec4(0.5f,0.5f,0.5f,1.0f)));
    int micro_secs = 1000000 / 60;
    this->kc.onframe(micro_secs);
    this->draw.render(this->kc);
    this->ui.onframe(this->kc);
    Gfx::CommitFrame();
    return Gfx::QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
YakcApp::OnCleanup() {
    this->draw.discard();
    this->ui.discard();
    Input::Discard();
    Gfx::Discard();
    return App::OnCleanup();
}
