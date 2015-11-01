#pragma once
//------------------------------------------------------------------------------
/**
    @class mem_window
    @brief memory dump and edit window
*/
#include "ui_window.h"
#include "ui_hexinput.h"

class mem_window : public window {
    OryolClassDecl(mem_window);
public:
    /// setup the window
    virtual void setup(const yakc::kc85& kc) override;
    /// draw method
    virtual bool draw(yakc::kc85& kc) override;
    /// update the memory character buffer
    void update_membuffer(const yakc::kc85& kc);

    hexinput start_widget;
    hexinput len_widget;

    // mem dump line: '0000: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 |1234567890123456|'
    static const int line_len = 6 + 16*3 + 18;
    static const int buf_size = (((1<<16) / 16) * line_len);   // 16 values per line, max 80 chars per line
    char buf[buf_size];
};