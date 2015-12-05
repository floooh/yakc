#pragma once
//------------------------------------------------------------------------------
/**
    @class MemoryMapWindow
    @brief visualize the current memory map configuration
*/
#include "ui/WindowBase.h"

class MemoryMapWindow : public WindowBase {
    OryolClassDecl(MemoryMapWindow);
public:
    /// setup the window
    virtual void Setup(yakc::kc85& kc) override;
    /// draw method
    virtual bool Draw(yakc::kc85& kc) override;

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

    /// draw a 'memory rectangle'
    void drawRect(int layer, yakc::uword addr, yakc::uword len, type t);

    /// get name for a memory layer and page
    pageInfo getPageInfo(yakc::kc85& kc, int layer_index, int page_index) const;
};
