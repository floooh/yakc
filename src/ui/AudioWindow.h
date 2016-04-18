#pragma once
//------------------------------------------------------------------------------
/**
    @class AudioWindow
    @brief audio debugging window
*/
#include "yakc/KC85Oryol.h"
#include "ui/WindowBase.h"

class Audio;

class AudioWindow : public WindowBase {
    OryolClassDecl(AudioWindow);
public:
    /// constructor
    AudioWindow(Audio* audio);
    /// setup the window
    virtual void Setup(yakc::kc85& kc) override;
    /// draw method
    virtual bool Draw(yakc::kc85& kc) override;

    Audio* audio;
    bool paused;
    uint64_t sample_cycle_count;
    uint64_t cpu_cycle_count;
    int diffIndex;
    float wavBuffer[256];
    float diffBuffer[256];
};