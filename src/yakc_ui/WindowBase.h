#pragma once
//------------------------------------------------------------------------------
/**
    @class WindowBase
    @brief simple window base class
*/
#include "Core/RefCounted.h"
#include "Core/String/String.h"
#include "yakc/yakc.h"

namespace YAKC {

class WindowBase : public Oryol::RefCounted {
    OryolClassDecl(WindowBase);
public:
    /// destructor
    virtual ~WindowBase();
    /// setup the window
    virtual void Setup(yakc& emu) = 0;
    /// draw method, return false if window closed
    virtual bool Draw(yakc& emu) = 0;
    /// false if window had been closed by user
    bool Visible = true;

protected:
    /// set the name, with unique id for imgui
    void setName(const char* str);

    static int globalUniqueId;
    Oryol::String title;
};

} // namespace YAKC
