#pragma once
//------------------------------------------------------------------------------
/**
    @class mem_window
    @brief memory dump and edit window
*/
#include "ui_window.h"
#include "ui_hexinput.h"
#include "imgui_memoryeditor.h"

class mem_window : public window {
    OryolClassDecl(mem_window);
public:
    /// configure the memory bank (0..3) before setup() is called
    int memory_bank_index = 0;

    /// setup the window
    virtual void setup(const yakc::kc85& kc) override;
    /// draw method
    virtual bool draw(yakc::kc85& kc) override;

    MemoryEditor edit;
};