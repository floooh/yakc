#pragma once
//------------------------------------------------------------------------------
/**
    @class YAKC::Keyboard
    @brief get keyboard input from Oryol and forward to emulator
*/
#include "yakc/yakc.h"
#include "Input/Input.h"

namespace YAKC {

class Keyboard {
public:
    /// setup the keyboard handler
    void Setup(yakc& emu);
    /// discard the keyboard handler
    void Discard();
    /// handle keyboard input, call this once per frame
    void HandleInput();

    bool hasInputFocus = true;
    yakc* emu = nullptr;
    uint8_t cur_char = 0;
    Oryol::Input::CallbackId callbackId = 0;
    Oryol::Set<Oryol::Key::Code> pressedKeys;
};

} // namespace YAKC
