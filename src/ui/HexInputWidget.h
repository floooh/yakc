#pragma once
//------------------------------------------------------------------------------
/**
    @class HexInputWidget
    @brief wrapper class for a hex-input widget
*/
#include "yakc/KC85Oryol.h"
#include "ui/Util.h"
#include "Core/String/StringAtom.h"
#include "IMUI/IMUI.h"

namespace yakc {

class HexInputWidget {
public:
    /// constructor
    HexInputWidget() : value(0), modeUWord(true) {
        YAKC_MEMSET(this->buf, 0, sizeof(this->buf));
    }
    /// configure with 16-bit value
    void Configure16(const Oryol::StringAtom& label_, yakc::uword value_) {
        this->label = label_;
        this->Set16(value_);
    }
    /// configure with 8-bit value
    void Configure8(const Oryol::StringAtom& label_, yakc::ubyte value_) {
        this->label = label_;
        this->Set8(value_);
    }
    /// set 16-bit value (updates text buffers)
    void Set16(yakc::uword value) {
        this->modeUWord = true;
        this->value = value;
        Util::UWordToStr(value, this->buf, sizeof(this->buf));
    }
    /// get 16-bit value
    yakc::uword Get16() const {
        return this->value;
    }
    /// set 8-bit value
    void Set8(yakc::ubyte value) {
        this->modeUWord = false;
        this->value = (yakc::uword) value;
        Util::UByteToStr(value, this->buf, sizeof(this->buf));
    }
    /// get 8-bit value
    yakc::ubyte Get8() const {
        return (yakc::ubyte) this->value;
    }
    /// draw the widget, return if value was updated
    bool Draw() {
        bool retval = false;
        if (this->modeUWord) {
            ImGui::PushItemWidth(38);
        }
        else {
            ImGui::PushItemWidth(22);
        }
        const int editFlags = ImGuiInputTextFlags_CharsHexadecimal|ImGuiInputTextFlags_CharsUppercase|ImGuiInputTextFlags_EnterReturnsTrue;
        if (ImGui::InputText(this->label.AsCStr(), this->buf, bufSize, editFlags)) {
            if (this->modeUWord) {
                this->Set16(Util::ParseUWord(this->buf, this->Get16()));
            }
            else {
                this->Set8(Util::ParseUByte(this->buf, this->Get8()));
            }
            retval = true;
        }
        ImGui::PopItemWidth();
        return retval;
    }

    static const int bufSize = 5;
    Oryol::StringAtom label;
    char buf[bufSize];
    yakc::uword value;
    bool modeUWord;
};

} // namespace yakc
