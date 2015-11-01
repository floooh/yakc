#pragma once
//------------------------------------------------------------------------------
/**
    @class window
    @brief simple window base class
*/
#include "Core/RefCounted.h"
#include "Core/String/String.h"
#include "kc85_oryol.h"

class window : public Oryol::RefCounted {
    OryolClassDecl(window);
public:
    /// destructor
    virtual ~window();
    /// setup the window
    virtual void setup(const yakc::kc85& kc);
    /// draw method, return false if window closed
    virtual bool draw(yakc::kc85& kc);
    /// false if window had been closed by user
    bool visible = true;

protected:
    /// set the name, with unique id for imgui
    void set_name(const char* str);

    static int global_unique_id;
    Oryol::String title;
};