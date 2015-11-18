#pragma once
//------------------------------------------------------------------------------
/**
    @class yakc::kc85_module.h
    @brief describe a KC85 expansion module
*/
#include "yakc_core/common.h"

namespace yakc {

class kc85_module {
public:
    /// create as 'empty' module
    static kc85_module create_empty(const char* name, const char* help);
    /// create as ROM module
    static kc85_module create_rom(ubyte type, ubyte* ptr, unsigned int size, const char* name, const char* help);
    /// create as RAM module
    static kc85_module create_ram(ubyte type, unsigned int size, const char* name, const char* help);
    /// destructor
    ~kc85_module();
    /// called when module is inserted
    void on_insert();
    /// called when module is removed
    void on_remove();

    /// the module type, see http://www.mpm-kc85.de/html/ModulListe.htm
    ubyte type = 0xFF;
    /// human-readable name, pointer to non-owned string
    const char* name = nullptr;
    /// human-readable help/description, pointer to non-owned string
    const char* help = nullptr;
    /// optional pointer to non-owned host-memory
    ubyte* mem_ptr = nullptr;
    /// optional size of memory
    unsigned int mem_size = 0;
    /// writable-flag for module memory
    bool writable = false;
    /// true if memory was allocated in on_insert
    bool mem_owned= false;
};

//------------------------------------------------------------------------------
inline
kc85_module::~kc85_module() {
    this->on_remove();
}

//------------------------------------------------------------------------------
inline kc85_module
kc85_module::create_empty(const char* name, const char* help) {
    kc85_module mod;
    mod.name = name;
    mod.help = help;
    return mod;
}

//------------------------------------------------------------------------------
inline kc85_module
kc85_module::create_rom(ubyte type, ubyte* ptr, unsigned int size, const char* name, const char* help) {
    YAKC_ASSERT(ptr);
    YAKC_ASSERT(size > 0);
    kc85_module mod;
    mod.type = type;
    mod.name = name;
    mod.help = help;
    mod.mem_ptr = ptr;
    mod.mem_size = size;
    mod.writable = false;
    mod.mem_owned = false;
    return mod;
}

//------------------------------------------------------------------------------
inline kc85_module
kc85_module::create_ram(ubyte type, unsigned int size, const char* name, const char* help) {
    YAKC_ASSERT(size > 0);
    kc85_module mod;
    mod.type = type;
    mod.name = name;
    mod.help = help;
    mod.mem_ptr = nullptr;
    mod.mem_size = size;
    mod.writable = true;
    mod.mem_owned = true;
    return mod;
}

//------------------------------------------------------------------------------
inline void
kc85_module::on_insert() {
    if (this->mem_owned && this->mem_size > 0) {
        YAKC_ASSERT(nullptr == this->mem_ptr);
        this->mem_ptr = (ubyte*) YAKC_MALLOC(this->mem_size);
        fill_random(this->mem_ptr, this->mem_size);
    }
}

//------------------------------------------------------------------------------
inline void
kc85_module::on_remove() {
    if (this->mem_owned && this->mem_ptr) {
        YAKC_FREE(this->mem_ptr);
        this->mem_ptr = 0;
    }
}

} // namespace yakc
