#pragma once
//------------------------------------------------------------------------------
/**
    @class yakc::kc85_exp
    @brief emulate the KC85 expansion slot system
*/
#include "core/common.h"

namespace yakc {

class kc85_exp {
public:
    enum module_type {
        none = 0,
        m006_basic,
        m011_64kbyte,
        m012_texor,
        m022_16kbyte,
        m026_forth,
        m027_development,
        m033_typestar,

        num_module_types,
    };

    struct module {
        bool registered = false;
        module_type type = none;
        ubyte id = 0xFF;
        const char* name = nullptr;
        const char* desc = nullptr;
        ubyte* mem_ptr = nullptr;
        unsigned int mem_size = 0;
        bool writable = false;
        bool mem_owned= false;
        ubyte addr_mask = 0xC0;
    };

    struct module_slot {
        ubyte slot_addr = 0x00;
        module mod;
        uword addr = 0x0000;
        ubyte control_byte = 0;
    };

    /// initialize the expansion system
    void init();
    /// reset the expansion system
    void reset();

    /// register the special 'none' module
    void register_none_module(const char* name, const char* desc);
    /// register a RAM module type
    void register_ram_module(module_type type, ubyte addr_mask, unsigned int size, const char* desc);
    /// register a ROM module type
    void register_rom_module(module_type type, ubyte addr_mask, const ubyte* ptr, unsigned int size, const char* desc);
    /// test if a module type is registered
    bool is_module_registered(module_type type) const;
    /// get module template by type
    const module& module_template(module_type type) const;
    /// get module name by type
    static const char* module_name(module_type type);
    /// get module id byte by type
    static ubyte module_id(module_type type);

    /// test if a slot exists
    bool slot_exists(ubyte slot_addr) const;
    /// get module slot by slot address (read/write)
    module_slot& slot_by_addr(ubyte slot_addr);
    /// get module slot by slot address (read-only)
    const module_slot& slot_by_addr(ubyte slot_addr) const;

    /// test if a slot is occupied
    bool slot_occupied(ubyte slot_addr) const;
    /// insert module into slot, slot must be free!
    void insert_module(ubyte slot_addr, module_type type);
    /// remove an expansion module
    void remove_module(ubyte slot_addr, memory& mem);

    /// update module control byte
    void update_control_byte(ubyte slot_addr, ubyte ctrl_byte);
    /// get module type in slot (0xFF if slot doesn't exist or no module in slot)
    ubyte module_type_in_slot(ubyte slot_addr) const;
    /// map or unmap a module based on its current control byte
    void update_memory_mappings(memory& mem);

    /// convert a slot address to a memory layer
    int memory_layer_by_slot_addr(ubyte slot_addr) const;

private:
    module registry[num_module_types];
    static const int num_slots = 2;
    module_slot slots[num_slots];
};

//------------------------------------------------------------------------------
inline void
kc85_exp::init() {
    // base device module slots, order is important
    this->slots[0].slot_addr = 0x08;
    this->slots[1].slot_addr = 0x0C;
}

//------------------------------------------------------------------------------
inline void
kc85_exp::reset() {
    // empty(?)
}

//------------------------------------------------------------------------------
inline const char*
kc85_exp::module_name(module_type type) {
    switch (type) {
        case none:              return "NONE";
        case m006_basic:        return "M006 BASIC";
        case m011_64kbyte:      return "M011 64 KBYTE RAM";
        case m012_texor:        return "M012 TEXOR";
        case m022_16kbyte:      return "M022 EXPANDER RAM";
        case m026_forth:        return "M026 FORTH";
        case m027_development:  return "M027 DEVELOPMENT";
        case m033_typestar:     return "M033 TYPESTAR";
        default:                return "UNKNOWN";
    }
}

//------------------------------------------------------------------------------
inline ubyte
kc85_exp::module_id(module_type type) {
    switch (type) {
        case none:              return 0xFF;
        case m006_basic:        return 0xFC;
        case m011_64kbyte:      return 0xF6;
        case m012_texor:        return 0xFB;
        case m022_16kbyte:      return 0xF4;
        case m026_forth:        return 0xFB;
        case m027_development:  return 0xFB;
        case m033_typestar:     return 0x01;
        default:                return 0xFF;
    }
}

//------------------------------------------------------------------------------
inline void
kc85_exp::register_none_module(const char* name, const char* desc) {
    module& mod = this->registry[none];
    mod.registered = true;
    mod.type = none;
    mod.name = name;
    mod.desc = desc;
}

//------------------------------------------------------------------------------
inline void
kc85_exp::register_ram_module(module_type type, ubyte addr_mask, unsigned int size, const char* desc) {
    YAKC_ASSERT(!this->is_module_registered(type));
    YAKC_ASSERT(size > 0 && desc);
    module& mod = this->registry[type];
    mod.registered = true;
    mod.type       = type;
    mod.id         = module_id(type);
    mod.name       = module_name(type);
    mod.desc       = desc;
    mod.mem_ptr    = nullptr;
    mod.mem_size   = size;
    mod.writable   = true;
    mod.mem_owned  = true;
    mod.addr_mask  = addr_mask;
}

//------------------------------------------------------------------------------
inline void
kc85_exp::register_rom_module(module_type type, ubyte addr_mask, const ubyte* ptr, unsigned int size, const char* desc) {
    YAKC_ASSERT(!this->is_module_registered(type));
    YAKC_ASSERT(ptr && size > 0 && desc);
    module& mod = this->registry[type];
    mod.registered = true;
    mod.type       = type;
    mod.id         = module_id(type);
    mod.name       = module_name(type);
    mod.desc       = desc;
    mod.mem_ptr    = (ubyte*) ptr;
    mod.mem_size   = size;
    mod.writable   = false;
    mod.mem_owned  = false;
    mod.addr_mask  = addr_mask;
}

//------------------------------------------------------------------------------
inline bool
kc85_exp::is_module_registered(module_type type) const {
    YAKC_ASSERT((type >= 0) && (type < num_module_types));
    return this->registry[type].registered;
}

//------------------------------------------------------------------------------
inline const kc85_exp::module&
kc85_exp::module_template(module_type type) const {
    YAKC_ASSERT(is_module_registered(type));
    return this->registry[type];
}

//------------------------------------------------------------------------------
inline bool
kc85_exp::slot_exists(ubyte slot_addr) const {
    for (int i = 0; i < num_slots; i++) {
        if (this->slots[i].slot_addr == slot_addr) {
            return true;
        }
    }
    return false;
}

//------------------------------------------------------------------------------
inline kc85_exp::module_slot&
kc85_exp::slot_by_addr(ubyte slot_addr) {
    for (auto& slot : this->slots) {
        if (slot.slot_addr == slot_addr) {
            return slot;
        }
    }
    YAKC_ASSERT(false);
    return this->slots[0];
}

//------------------------------------------------------------------------------
inline const kc85_exp::module_slot&
kc85_exp::slot_by_addr(ubyte slot_addr) const {
    return const_cast<kc85_exp*>(this)->slot_by_addr(slot_addr);
}

//------------------------------------------------------------------------------
inline int
kc85_exp::memory_layer_by_slot_addr(ubyte slot_addr) const {
    for (int i = 0; i < num_slots; i++) {
        if (this->slots[i].slot_addr == slot_addr) {
            return i + 1;
        }
    }
    YAKC_ASSERT(false);
    return -1;
}

//------------------------------------------------------------------------------
inline bool
kc85_exp::slot_occupied(ubyte slot_addr) const {
    return this->slot_by_addr(slot_addr).mod.id != 0xFF;
}

//------------------------------------------------------------------------------
inline void
kc85_exp::insert_module(ubyte slot_addr, module_type type) {
    YAKC_ASSERT(this->is_module_registered(type));
    YAKC_ASSERT(!this->slot_occupied(slot_addr));

    auto& slot = this->slot_by_addr(slot_addr);
    slot.mod = this->registry[type];
    if (slot.mod.mem_owned && slot.mod.mem_size > 0) {
        YAKC_ASSERT(nullptr == slot.mod.mem_ptr);
        slot.mod.mem_ptr = (ubyte*) YAKC_MALLOC(slot.mod.mem_size);
        clear(slot.mod.mem_ptr, slot.mod.mem_size);
    }
}

//------------------------------------------------------------------------------
inline void
kc85_exp::remove_module(ubyte slot_addr, memory& mem) {
    YAKC_ASSERT(this->slot_occupied(slot_addr));
    mem.unmap_layer(this->memory_layer_by_slot_addr(slot_addr));
    auto& slot = this->slot_by_addr(slot_addr);
    slot.addr = 0x0000;
    if (slot.mod.mem_owned && slot.mod.mem_ptr) {
        YAKC_FREE(slot.mod.mem_ptr);
        slot.mod.mem_ptr = 0;
    }
    slot.mod = module();
}

//------------------------------------------------------------------------------
inline void
kc85_exp::update_control_byte(ubyte slot_addr, ubyte ctrl_byte) {
    if (this->slot_exists(slot_addr)) {
        this->slot_by_addr(slot_addr).control_byte = ctrl_byte;
    }
}

//------------------------------------------------------------------------------
inline ubyte
kc85_exp::module_type_in_slot(ubyte slot_addr) const {
    if (this->slot_exists(slot_addr)) {
        return this->slot_by_addr(slot_addr).mod.id;
    }
    else {
        return 0xFF;
    }
}

//------------------------------------------------------------------------------
inline void
kc85_exp::update_memory_mappings(memory& mem) {
    for (auto& slot : this->slots) {
        if (0xFF != slot.mod.id) {
            const int memory_layer = this->memory_layer_by_slot_addr(slot.slot_addr);
            mem.unmap_layer(memory_layer);

            // compute module start address from control-byte
            slot.addr = (slot.control_byte & slot.mod.addr_mask)<<8;
            if (slot.mod.mem_size && slot.mod.mem_ptr) {
                if (slot.control_byte & 0x01) {
                    // activate the module
                    bool writable = (slot.control_byte & 0x02) && slot.mod.writable;
                    mem.map(memory_layer, slot.addr, slot.mod.mem_size, slot.mod.mem_ptr, writable);
                }
                else {
                    // deactivate the module
                    mem.unmap_layer(memory_layer);
                }
            }
        }
    }
}

} // namespace yakc
