#pragma once
//------------------------------------------------------------------------------
/**
    @class AudioWindow
    @brief audio debugging window
*/
#include "yakcapp/KC85Oryol.h"
#include "ui/WindowBase.h"

namespace yakc {

class Audio;

class AudioWindow : public WindowBase {
    OryolClassDecl(AudioWindow);
public:
    /// constructor
    AudioWindow(Audio* audio);
    /// setup the window
    virtual void Setup(yakc::emu& emu) override;
    /// draw method
    virtual bool Draw(yakc::emu& emu) override;

    Audio* audio;
    bool paused;
    bool cpuAhead;
    bool cpuBehind;
    float wavBuffer[256];
};

} // namespace yakc