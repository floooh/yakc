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
    this->ui.setup(this->kc);

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
