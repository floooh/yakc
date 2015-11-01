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
        0x21, 0x00, 0xA8,
        0x36, 0x11,         // red on black
        0x23,
        0x36, 0x11,
        0x21, 0x00, 0x80,
        0x36, 0xAA,             // stripes
        0x23,
        0x36, 0xAA
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
