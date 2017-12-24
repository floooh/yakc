#pragma once
//------------------------------------------------------------------------------
/**
    @class MemoryMapWindow
    @brief visualize the current memory map configuration
*/
#include "yakc_ui/WindowBase.h"

namespace YAKC {

class MemoryMapWindow : public WindowBase {
    OryolClassDecl(MemoryMapWindow);
public:
    /// setup the window
    virtual void Setup(yakc& emu) override;
    /// draw method
    virtual bool Draw(yakc& emu) override;

    struct pageInfo {
        const char* name = nullptr;
        unsigned int addr = 0;
        unsigned int size = 0;
    };

    enum class type {
        off,
        mapped,
        hidden,
    };

    /// draw background grid
    void drawGrid(bool is_kc85_4);
    /// draw a 'memory rectangle'
    void drawRect(int layer, uint16_t addr, unsigned int len, const char* tooltip, type t);
    /// get name for a memory layer and page
    pageInfo getPageInfo(kc85_t& kc, int layer_index, int page_index) const;
};

} // namespace YAKC

