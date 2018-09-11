#pragma once
//------------------------------------------------------------------------------
/**
    @class YAKC::kc85
    @brief wrapper class for the KC85/2, /3, /4
*/
#include "yakc/util/breadboard.h"
#include "yakc/util/rom_images.h"
#include "yakc/util/filesystem.h"
#include "yakc/util/filetypes.h"
#include "systems/kc85.h"

namespace YAKC {

class kc85_t {
public:
    /// check if required roms are loaded
    static bool check_roms(system model, os_rom os);

    /// power-on the device
    void poweron(system m, os_rom os);
    /// power-off the device
    void poweroff();
    /// reset the device
    void reset();
    /// get info about emulated system
    const char* system_info() const;
    /// return number of supported joysticks
    int num_joysticks() const { return 0; };
    /// process a number of cycles, return final processed tick
    void exec(uint32_t micro_seconds);
    /// decode audio data
    void decode_audio(float* buffer, int num_samples);
    /// get pointer to framebuffer, width and height
    const void* framebuffer(int& out_width, int& out_height);
    /// called when alpha-numeric key has been pressed
    void on_ascii(uint8_t ascii);
    /// called when non-alnum key has been pressed down
    void on_key_down(uint8_t key);
    /// called when non-alnum key has been released
    void on_key_up(uint8_t key);
    /// file quickloading
    bool quickload(filesystem* fs, const char* name, filetype type, bool start);
    /// audio callback
    static void audio_cb(const float* samples, int num_samples, void* user_data);
    /// callback to apply patches after a snapshot is loaded
    static void patch_cb(const char* snapshot_name, void* user_data);

    system cur_model = system::kc85_3;
    bool on = false;
    ::kc85_t sys;

    struct module {
        bool registered  = false;
        kc85_module_type_t type = KC85_MODULE_NONE;
        uint8_t id       = 0xFF;
        const char* name = nullptr;
        const char* desc = nullptr;
        uint8_t* mem_ptr = nullptr;
        unsigned int mem_size = 0;
    };
    module mod_registry[KC85_MODULE_NUM];

    /// register the special 'none' module
    void register_none_module(const char* name, const char* desc);
    /// register a RAM module type
    void register_ram_module(kc85_module_type_t type, const char* desc);
    /// register a ROM module type
    void register_rom_module(kc85_module_type_t type, const uint8_t* ptr, unsigned int size, const char* desc);
    /// test if a module type is registered
    bool is_module_registered(kc85_module_type_t type) const;
    /// get module template by type
    const module& module_template(kc85_module_type_t type) const;
    /// get module name by type
    static const char* module_name(kc85_module_type_t type);
    /// get module id byte by type
    static uint8_t module_id(kc85_module_type_t type);
    /// check if a slot contains a module
    bool slot_occupied(uint8_t slot_addr);
    /// get module by slot address
    const module& mod_by_slot_addr(uint8_t slot_addr) const;
    /// insert module into slot, slot must be free!
    void insert_module(uint8_t slot_addr, kc85_module_type_t type);
    /// remove an expansion module
    void remove_module(uint8_t slot_addr);
};
extern YAKC::kc85_t kc85;

} // namespace YAKC

