#pragma once
//------------------------------------------------------------------------------
/**
    @class YAKC::Keyboard
    @brief get keyboard input from Oryol and forward to emulator
*/
#include "yakc/yakc.h"
#include "Input/Input.h"
#include "Core/Containers/Buffer.h"

namespace YAKC {

class Keyboard {
public:
    /// setup the keyboard handler
    void Setup(yakc& emu);
    /// discard the keyboard handler
    void Discard();
    /// handle keyboard input, call this once per frame
    void HandleInput();
    /// set a text stream for playback
    void StartPlayback(Oryol::Buffer&& buffer);

    /// handle text playback (called from HandleInput)
    void handleTextPlayback();

    bool hasInputFocus = true;
    yakc* emu = nullptr;
    uint8_t cur_char = 0;
    uint8_t cur_joystick = 0;
    Oryol::Input::CallbackId callbackId = 0;
    Oryol::Set<Oryol::Key::Code> pressedKeys;

    int playbackCounter = 0;
    bool playbackFlipFlop = 0;
    int playbackPos = 0;
    uint8_t playbackChar = 0;
    Oryol::Buffer playbackBuffer;
};

} // namespace YAKC
