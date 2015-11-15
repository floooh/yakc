#pragma once
//------------------------------------------------------------------------------
/**
    @class MemoryMapWindow
    @brief visualize the current memory map configuration
*/
#include "yakc_ui/WindowBase.h"

class MemoryMapWindow : public WindowBase {
    OryolClassDecl(MemoryMapWindow);
public:
    /// setup the window
    virtual void Setup(yakc::kc85& kc) override;
    /// draw method
    virtual bool Draw(yakc::kc85& kc) override;

    /// get name for a memory layer and page
    const char* getPageName(yakc::kc85& kc, int layer_index, int page_index) const;
};
