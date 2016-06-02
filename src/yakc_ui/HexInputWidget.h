#pragma once
//------------------------------------------------------------------------------
/**
    @class HexInputWidget
    @brief wrapper class for a hex-input widget
*/
#include "yakc_ui/Util.h"
#include "Core/String/StringAtom.h"
#include "IMUI/IMUI.h"

namespace YAKC {

class HexInputWidget {
public:
    /// constructor
    HexInputWidget() : value(0), modeUWord(true) {
        memset(this->buf, 0, sizeof(this->buf));
    }
    /// configure with 16-bit value
    void Configure16(const Oryol::StringAtom& label_, uword value_) {
        this->label = label_;
        this->Set16(value_);
    }
    /// configure with 8-bit value
    void Configure8(const Oryol::StringAtom& label_, ubyte value_) {
        this->label = label_;
        this->Set8(value_);
    }
    /// set 16-bit value (updates text buffers)
    void Set16(uword value) {
        this->modeUWord = true;
        this->value = value;
        Util::UWordToStr(value, this->buf, sizeof(this->buf));
    }
    /// get 16-bit value
    uword Get16() const {
        return this->value;
    }
    /// set 8-bit value
    void Set8(ubyte value) {
        this->modeUWord = false;
        this->value = (uword) value;
        Util::UByteToStr(value, this->buf, sizeof(this->buf));
    }
    /// get 8-bit value
    ubyte Get8() const {
        return (ubyte) this->value;
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
    uword value;
    bool modeUWord;
};

} // namespace YAKC
