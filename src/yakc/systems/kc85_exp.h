#pragma once
//------------------------------------------------------------------------------
/**
    @class YAKC::kc85_exp
    @brief emulate the KC85 expansion slot system
*/
#include "yakc/core/core.h"

namespace YAKC {

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
        bool registered     = false;
        module_type type    = none;
        uint8_t id            = 0xFF;
        const char* name    = nullptr;
        const char* desc    = nullptr;
        uint8_t* mem_ptr      = nullptr;
        unsigned int mem_size = 0;
        bool writable       = false;
        bool mem_owned      = false;
        uint8_t addr_mask     = 0xC0;
    };

    struct module_slot {
        uint8_t slot_addr = 0x00;
        module mod;
        uint16_t addr = 0x0000;
        uint8_t control_byte = 0;
    };

    /// initialize the expansion system
    void poweron();
    /// reset the expansion system
    void reset();

    /// register the special 'none' module
    void register_none_module(const char* name, const char* desc);
    /// register a RAM module type
    void register_ram_module(module_type type, uint8_t addr_mask, unsigned int size, const char* desc);
    /// register a ROM module type
    void register_rom_module(module_type type, uint8_t addr_mask, const uint8_t* ptr, unsigned int size, const char* desc);
    /// test if a module type is registered
    bool is_module_registered(module_type type) const;
    /// get module template by type
    const module& module_template(module_type type) const;
    /// get module name by type
    static const char* module_name(module_type type);
    /// get module id byte by type
    static uint8_t module_id(module_type type);

    /// test if a slot exists
    bool slot_exists(uint8_t slot_addr) const;
    /// get module slot by slot address (read/write)
    module_slot& slot_by_addr(uint8_t slot_addr);
    /// get module slot by slot address (read-only)
    const module_slot& slot_by_addr(uint8_t slot_addr) const;
    /// test if module in slot 'owns' a host memory address
    bool module_in_slot_owns_pointer(uint8_t slot_addr, const uint8_t* ptr) const;

    /// test if a slot is occupied
    bool slot_occupied(uint8_t slot_addr) const;
    /// insert module into slot, slot must be free!
    void insert_module(uint8_t slot_addr, module_type type);
    /// remove an expansion module
    void remove_module(uint8_t slot_addr);

    /// update module control byte
    void update_control_byte(uint8_t slot_addr, uint8_t ctrl_byte);
    /// get module type in slot (0xFF if slot doesn't exist or no module in slot)
    uint8_t module_type_in_slot(uint8_t slot_addr) const;
    /// map or unmap a module based on its current control byte
    void update_memory_mappings();

    /// convert a slot address to a memory layer
    int memory_layer_by_slot_addr(uint8_t slot_addr) const;

    module registry[num_module_types];
    static const int num_slots = 2;
    module_slot slots[num_slots];
};

} // namespace YAKC
