#pragma once
//------------------------------------------------------------------------------
/**
    @class AudioWindow
    @brief audio debugging window
*/
#include "yakc_ui/WindowBase.h"

namespace YAKC {

class Audio;

class AudioWindow : public WindowBase {
    OryolClassDecl(AudioWindow);
public:
    /// constructor
    AudioWindow(Audio* audio);
    /// setup the window
    virtual void Setup(yakc& emu) override;
    /// draw method
    virtual bool Draw(yakc& emu) override;

    Audio* audio;
    bool paused;
    bool cpuAhead;
    bool cpuBehind;
    float wavBuffer[256];
};

} // namespace YAKC