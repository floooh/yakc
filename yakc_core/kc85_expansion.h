#pragma once
//------------------------------------------------------------------------------
/**
    @class yakc::kc85_expansion
    @brief emulate the KC85 expansion slot system
*/
#include "yakc_core/common.h"
#include "yakc_core/kc85_module.h"

namespace yakc {

class kc85_expansion {
public:
    /// the module slots
    struct module_slot {
        ubyte slot_addr = 0x00;
        kc85_module mod;
        uword addr = 0x0000;
        ubyte control_byte = 0;
    };

    /// initialize the expansion system
    void init();
    /// reset the expansion system
    void reset();

    /// test if a slot exists
    bool slot_exists(ubyte slot_addr) const;
    /// get module slot by slot address (read/write)
    module_slot& slot_by_addr(ubyte slot_addr);
    /// get module slot by slot address (read-only)
    const module_slot& slot_by_addr(ubyte slot_addr) const;

    /// test if a slot is occupied
    bool slot_occupied(ubyte slot_addr) const;
    /// insert module into slot, slot must be free!
    void insert_module(ubyte slot_addr, const kc85_module& mod);
    /// remove an expansion module
    void remove_module(ubyte slot_addr, memory& mem);

    /// update module control byte
    void update_control_byte(ubyte slot_addr, ubyte ctrl_byte);
    /// get module type in slot (0xFF if slot doesn't exist or no module in slot)
    ubyte module_type(ubyte slot_addr) const;
    /// map or unmap a module based on its current control byte
    void update_memory_mappings(memory& mem);

    /// convert a slot address to a memory layer
    int memory_layer_by_slot_addr(ubyte slot_addr) const;

private:
    static const int max_num_slots = 2;
    module_slot slots[max_num_slots];
};

//------------------------------------------------------------------------------
inline void
kc85_expansion::init() {
    // base device module slots, order is important
    this->slots[0].slot_addr = 0x08;
    this->slots[1].slot_addr = 0x0C;
}

//------------------------------------------------------------------------------
inline void
kc85_expansion::reset() {
    // empty(?)
}

//------------------------------------------------------------------------------
inline bool
kc85_expansion::slot_exists(ubyte slot_addr) const {
    for (int i = 0; i < max_num_slots; i++) {
        if (this->slots[i].slot_addr == slot_addr) {
            return true;
        }
    }
    return false;
}

//------------------------------------------------------------------------------
inline kc85_expansion::module_slot&
kc85_expansion::slot_by_addr(ubyte slot_addr) {
    for (auto& slot : this->slots) {
        if (slot.slot_addr == slot_addr) {
            return slot;
        }
    }
    YAKC_ASSERT(false);
    return this->slots[0];
}

//------------------------------------------------------------------------------
inline const kc85_expansion::module_slot&
kc85_expansion::slot_by_addr(ubyte slot_addr) const {
    return const_cast<kc85_expansion*>(this)->slot_by_addr(slot_addr);
}

//------------------------------------------------------------------------------
inline int
kc85_expansion::memory_layer_by_slot_addr(ubyte slot_addr) const {
    for (int i = 0; i < max_num_slots; i++) {
        if (this->slots[i].slot_addr == slot_addr) {
            return i + 1;
        }
    }
    YAKC_ASSERT(false);
    return -1;
}

//------------------------------------------------------------------------------
inline bool
kc85_expansion::slot_occupied(ubyte slot_addr) const {
    return this->slot_by_addr(slot_addr).mod.type != 0xFF;
}

//------------------------------------------------------------------------------
inline void
kc85_expansion::insert_module(ubyte slot_addr, const kc85_module& mod) {
    YAKC_ASSERT(!this->slot_occupied(slot_addr));
    auto& slot = this->slot_by_addr(slot_addr);
    slot.mod = mod;
    slot.mod.on_insert();
}

//------------------------------------------------------------------------------
inline void
kc85_expansion::remove_module(ubyte slot_addr, memory& mem) {
    YAKC_ASSERT(this->slot_occupied(slot_addr));
    mem.unmap_layer(this->memory_layer_by_slot_addr(slot_addr));
    auto& slot = this->slot_by_addr(slot_addr);
    slot.addr = 0x0000;
    slot.mod.on_remove();
    slot.mod = kc85_module();
}

//------------------------------------------------------------------------------
inline void
kc85_expansion::update_control_byte(ubyte slot_addr, ubyte ctrl_byte) {
    if (this->slot_exists(slot_addr)) {
        this->slot_by_addr(slot_addr).control_byte = ctrl_byte;
    }
}

//------------------------------------------------------------------------------
inline ubyte
kc85_expansion::module_type(ubyte slot_addr) const {
    if (this->slot_exists(slot_addr)) {
        return this->slot_by_addr(slot_addr).mod.type;
    }
    else {
        return 0xFF;
    }
}

//------------------------------------------------------------------------------
inline void
kc85_expansion::update_memory_mappings(memory& mem) {
    for (auto& slot : this->slots) {
        if (0xFF != slot.mod.type) {
            const int memory_layer = this->memory_layer_by_slot_addr(slot.slot_addr);
            mem.unmap_layer(memory_layer);

            // compute module start address from control-byte
            slot.addr = (slot.control_byte & 0xF0)<<8;
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
