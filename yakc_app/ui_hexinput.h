#pragma once
//------------------------------------------------------------------------------
/**
    @class hexinput
    @brief wrapper class for a hex-input widget
*/
#include "kc85_oryol.h"
#include "util.h"
#include "Core/String/StringAtom.h"
#include "imgui.h"

class hexinput {
public:
    /// configure with 16-bit value
    void configure16(const Oryol::StringAtom& label_, yakc::uword value_) {
        this->label = label_;
        this->set16(value_);
    }
    /// configure with 8-bit value
    void configure8(const Oryol::StringAtom& label_, yakc::ubyte value_) {
        this->label = label_;
        this->set8(value_);
    }
    /// set 16-bit value (updates text buffers)
    void set16(yakc::uword value) {
        this->mode_uword = true;
        this->value = value;
        util::uword_to_str(value, this->buf, sizeof(this->buf));
    }
    /// get 16-bit value
    yakc::uword get16() const {
        return this->value;
    }
    /// set 8-bit value
    void set8(yakc::ubyte value) {
        this->mode_uword = false;
        this->value = (yakc::uword) value;
        util::ubyte_to_str(value, this->buf, sizeof(this->buf));
    }
    /// get 8-bit value
    yakc::ubyte get8() const {
        return (yakc::ubyte) this->value;
    }
    /// draw the widget, return if value was updated
    bool draw() {
        const int editFlags = ImGuiInputTextFlags_CharsHexadecimal|ImGuiInputTextFlags_CharsUppercase|ImGuiInputTextFlags_EnterReturnsTrue;
        if (ImGui::InputText(this->label.AsCStr(), this->buf, buf_size, editFlags)) {
            if (this->mode_uword) {
                this->set16(util::parse_uword(this->buf, this->get16()));
            }
            else {
                this->set8(util::parse_uword(this->buf, this->get8()));
            }
            return true;
        }
        return false;
    }

    static const int buf_size = 5;
    Oryol::StringAtom label;
    char buf[buf_size] = { 0 };
    yakc::uword value = 0;
    bool mode_uword = true;     // 16- or 8-bit mode
};
