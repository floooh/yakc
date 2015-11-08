//------------------------------------------------------------------------------
//  main.cc
//  yakc app main source.
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/App.h"
#include "Gfx/Gfx.h"
#include "Input/Input.h"
#include "kc85_oryol.h"
#include "draw.h"
#if YAKC_UI
#include "yakc_ui/UI.h"
#endif

using namespace Oryol;
using namespace yakc;

class YakcApp : public App {
public:
    AppState::Code OnInit();
    AppState::Code OnRunning();
    AppState::Code OnCleanup();

    Id drawState;
    kc85 kc;
    class draw draw;
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
    #if YAKC_UI
    this->ui.OnFrame(this->kc);
    #endif
    Gfx::CommitFrame();
    return Gfx::QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
YakcApp::OnCleanup() {
    this->draw.discard();
    #if YAKC_UI
    this->ui.Discard();
    #endif
    Input::Discard();
    Gfx::Discard();
    return App::OnCleanup();
}
