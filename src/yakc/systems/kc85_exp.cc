//------------------------------------------------------------------------------
//  kc85_exp.cc
//------------------------------------------------------------------------------
#include "kc85_exp.h"
#include "kc85.h"
#include "yakc/util/breadboard.h"

namespace YAKC {

//------------------------------------------------------------------------------
void
kc85_exp::poweron() {
    // base device module slots, order is important
    this->slots[0].slot_addr = 0x08;
    this->slots[1].slot_addr = 0x0C;
}

//------------------------------------------------------------------------------
void
kc85_exp::reset() {
    // empty(?)
}

//------------------------------------------------------------------------------
const char*
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
uint8_t
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
void
kc85_exp::register_none_module(const char* name, const char* desc) {
    module& mod = this->registry[none];
    mod.registered = true;
    mod.type = none;
    mod.name = name;
    mod.desc = desc;
}

//------------------------------------------------------------------------------
void
kc85_exp::register_ram_module(module_type type, uint8_t addr_mask, unsigned int size, const char* desc) {
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
void
kc85_exp::register_rom_module(module_type type, uint8_t addr_mask, const uint8_t* ptr, unsigned int size, const char* desc) {
    YAKC_ASSERT(!this->is_module_registered(type));
    YAKC_ASSERT(ptr && size > 0 && desc);
    module& mod = this->registry[type];
    mod.registered = true;
    mod.type       = type;
    mod.id         = module_id(type);
    mod.name       = module_name(type);
    mod.desc       = desc;
    mod.mem_ptr    = (uint8_t*) ptr;
    mod.mem_size   = size;
    mod.writable   = false;
    mod.mem_owned  = false;
    mod.addr_mask  = addr_mask;
}

//------------------------------------------------------------------------------
bool
kc85_exp::is_module_registered(module_type type) const {
    YAKC_ASSERT((type >= 0) && (type < num_module_types));
    return this->registry[type].registered;
}

//------------------------------------------------------------------------------
const kc85_exp::module&
kc85_exp::module_template(module_type type) const {
    YAKC_ASSERT(is_module_registered(type));
    return this->registry[type];
}

//------------------------------------------------------------------------------
bool
kc85_exp::slot_exists(uint8_t slot_addr) const {
    for (int i = 0; i < num_slots; i++) {
        if (this->slots[i].slot_addr == slot_addr) {
            return true;
        }
    }
    return false;
}

//------------------------------------------------------------------------------
kc85_exp::module_slot&
kc85_exp::slot_by_addr(uint8_t slot_addr) {
    for (auto& slot : this->slots) {
        if (slot.slot_addr == slot_addr) {
            return slot;
        }
    }
    YAKC_ASSERT(false);
    return this->slots[0];
}

//------------------------------------------------------------------------------
const kc85_exp::module_slot&
kc85_exp::slot_by_addr(uint8_t slot_addr) const {
    return const_cast<kc85_exp*>(this)->slot_by_addr(slot_addr);
}

//------------------------------------------------------------------------------
bool
kc85_exp::module_in_slot_owns_pointer(uint8_t slot_addr, const uint8_t* ptr) const {
    const auto& slot = this->slot_by_addr(slot_addr);
    return (slot.mod.mem_ptr != 0) &&
        (ptr >= slot.mod.mem_ptr) &&
        (ptr < slot.mod.mem_ptr+slot.mod.mem_size);
}

//------------------------------------------------------------------------------
int
kc85_exp::memory_layer_by_slot_addr(uint8_t slot_addr) const {
    for (int i = 0; i < num_slots; i++) {
        if (this->slots[i].slot_addr == slot_addr) {
            return i + 1;
        }
    }
    YAKC_ASSERT(false);
    return -1;
}

//------------------------------------------------------------------------------
bool
kc85_exp::slot_occupied(uint8_t slot_addr) const {
    return this->slot_by_addr(slot_addr).mod.id != 0xFF;
}

//------------------------------------------------------------------------------
void
kc85_exp::insert_module(uint8_t slot_addr, module_type type) {
    YAKC_ASSERT(this->is_module_registered(type));
    YAKC_ASSERT(!this->slot_occupied(slot_addr));

    auto& slot = this->slot_by_addr(slot_addr);
    slot.mod = this->registry[type];
    if (slot.mod.mem_owned && slot.mod.mem_size > 0) {
        YAKC_ASSERT(nullptr == slot.mod.mem_ptr);
        slot.mod.mem_ptr = (uint8_t*) YAKC_MALLOC(slot.mod.mem_size);
        clear(slot.mod.mem_ptr, slot.mod.mem_size);
    }
}

//------------------------------------------------------------------------------
void
kc85_exp::remove_module(uint8_t slot_addr) {
    YAKC_ASSERT(this->slot_occupied(slot_addr));
    mem_unmap_layer(&kc85.mem, this->memory_layer_by_slot_addr(slot_addr));
    auto& slot = this->slot_by_addr(slot_addr);
    slot.addr = 0x0000;
    if (slot.mod.mem_owned && slot.mod.mem_ptr) {
        YAKC_FREE(slot.mod.mem_ptr);
        slot.mod.mem_ptr = 0;
    }
    slot.mod = module();
}

//------------------------------------------------------------------------------
void
kc85_exp::update_control_byte(uint8_t slot_addr, uint8_t ctrl_byte) {
    if (this->slot_exists(slot_addr)) {
        this->slot_by_addr(slot_addr).control_byte = ctrl_byte;
    }
}

//------------------------------------------------------------------------------
uint8_t
kc85_exp::module_type_in_slot(uint8_t slot_addr) const {
    if (this->slot_exists(slot_addr)) {
        return this->slot_by_addr(slot_addr).mod.id;
    }
    else {
        return 0xFF;
    }
}

//------------------------------------------------------------------------------
void
kc85_exp::update_memory_mappings() {
    for (auto& slot : this->slots) {
        if (0xFF != slot.mod.id) {
            const int memory_layer = this->memory_layer_by_slot_addr(slot.slot_addr);
            mem_unmap_layer(&kc85.mem, memory_layer);

            // compute module start address from control-byte
            slot.addr = (slot.control_byte & slot.mod.addr_mask)<<8;
            if (slot.mod.mem_size && slot.mod.mem_ptr) {
                if (slot.control_byte & 0x01) {
                    // activate the module
                    bool writable = (slot.control_byte & 0x02) && slot.mod.writable;
                    if (writable) {
                        mem_map_ram(&kc85.mem, memory_layer, slot.addr, slot.mod.mem_size, slot.mod.mem_ptr);
                    }
                    else {
                        mem_map_rom(&kc85.mem, memory_layer, slot.addr, slot.mod.mem_size, slot.mod.mem_ptr);
                    }
                }
                else {
                    // deactivate the module
                    mem_unmap_layer(&kc85.mem, memory_layer);
                }
            }
        }
    }
}

} // namespace YAKC
