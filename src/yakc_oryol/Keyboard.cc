//------------------------------------------------------------------------------
//  Keyboard.cc
//------------------------------------------------------------------------------
#include "Keyboard.h"

namespace YAKC {

using namespace Oryol;

//------------------------------------------------------------------------------
static bool
is_modifier(Key::Code key) {
    switch (key) {
        case Key::LeftShift:
        case Key::RightShift:
        case Key::LeftControl:
        case Key::RightControl:
        case Key::LeftAlt:
        case Key::RightAlt:
        case Key::LeftSuper:
        case Key::RightSuper:
            return true;
        default:
            return false;
    }
}

//------------------------------------------------------------------------------
static uint8_t
translate_special_key(Key::Code key, bool shift_pressed) {

    uint8_t ascii = 0;

    // check for special case (Enter, Esc, cursor keys, etc) and
    // translate them to ascii
    struct toAscii {
        Key::Code key;
        uint8_t ascii;
    };
    const static toAscii keyTable[] = {
        // FIXME:
        // HOME, PAGE UP/DOWN, START/END of line, INSERT,
        // also, some of these (e.g. the function keys) are KC85/2 specific!
        { Key::Left, 0x08 },
        { Key::Right, 0x09 },
        { Key::Down, 0x0A },
        { Key::Up, 0x0B },
        { Key::Enter, 0x0D },
        { Key::BackSpace, 0x01 },
        { Key::Escape, 0x03 },
        { Key::F1, 0xF1 },
        { Key::F2, 0xF2 },
        { Key::F3, 0xF3 },
        { Key::F4, 0xF4 },
        { Key::F5, 0xF5 },
        { Key::F6, 0xF6 },
        { Key::F7, 0xF7 },
        { Key::F8, 0xF8 },
        { Key::F9, 0xF9 },
        { Key::F10, 0xFA },
        { Key::F11, 0xFB },
        { Key::F12, 0xFC },
    };
    for (const auto& item : keyTable) {
        if (item.key == key) {
            if (shift_pressed && (key == Key::BackSpace)) {
                // shift+bs: clear screen
                ascii = 0x0C;
            }
            else if (shift_pressed && (key == Key::Escape)) {
                // shift+esc: break
                ascii = 0x13;
            }
            else {
                ascii = item.ascii;
            }
            break;
        }
    }
    return ascii;
}

//------------------------------------------------------------------------------
void
Keyboard::Setup(yakc& emu_) {
    o_assert_dbg(!this->emu);
    this->emu = &emu_;
    Input::SubscribeEvents([this](const InputEvent& e) {
        if (e.Type == InputEvent::WChar) {
            if ((e.WCharCode >= 32) && (e.WCharCode < 127)) {
                uint8_t ascii = (uint8_t) e.WCharCode;
                // invert case (FIXME: this depends on the emulated system)
                if (islower(ascii)) {
                    ascii = toupper(ascii);
                }
                else if (isupper(ascii)) {
                    ascii = tolower(ascii);
                }
                this->cur_char = ascii;
            }
        }
        else if (e.Type == InputEvent::KeyDown) {
            // keep track of pressed keys
            if (!is_modifier(e.KeyCode) && !this->pressedKeys.Contains(e.KeyCode)) {
                this->pressedKeys.Add(e.KeyCode);
            }
            // translate any non-alphanumeric keys (Enter, Esc, cursor keys etc...)
            uint8_t special_ascii = translate_special_key(e.KeyCode, Input::KeyPressed(Key::LeftShift));
            if (0 != special_ascii) {
                this->cur_char = special_ascii;
            }
        }
        else if (e.Type == InputEvent::KeyUp) {
            // keep track of pressed keys
            if (!is_modifier(e.KeyCode) && this->pressedKeys.Contains(e.KeyCode)) {
                this->pressedKeys.Erase(e.KeyCode);
                if (this->pressedKeys.Empty()) {
                    this->cur_char = 0;
                }
            }
        }
    });
}

//------------------------------------------------------------------------------
void
Keyboard::Discard() {
    o_assert_dbg(this->emu);
    Input::UnsubscribeEvents(this->callbackId);
}

//------------------------------------------------------------------------------
void
Keyboard::HandleInput() {
    o_assert_dbg(this->emu);
    this->emu->put_key(this->cur_char);
}

} // namespace YAKC
