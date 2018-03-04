//------------------------------------------------------------------------------
//  FileLoader.cc
//------------------------------------------------------------------------------
#include "FileLoader.h"
#include "Core/String/StringBuilder.h"
#include "yakc/util/filetypes.h"

using namespace Oryol;

namespace YAKC {

FileLoader* FileLoader::pointer = nullptr;

//------------------------------------------------------------------------------
void
FileLoader::Setup(yakc& emu_) {
    o_assert(nullptr == pointer);
    this->emu = &emu_;
    pointer = this;
    this->Items.Add("Pengo", "pengo.kcc", filetype::kcc, system::kc85_3);
    this->Items.Add("Pengo", "pengo4.kcc", filetype::kcc, system::kc85_4);
    this->Items.Add("Cave", "cave.kcc", filetype::kcc, system::kc85_3);
    this->Items.Add("Labyrinth", "labyrinth.kcc", filetype::kcc, system::kc85_3);
    this->Items.Add("House", "house.kcc", filetype::kcc, system::kc85_3);
    this->Items.Add("House", "house4.tap", filetype::kc_tap, system::kc85_4);
    this->Items.Add("Jungle", "jungle.kcc", filetype::kcc, system::kc85_3);
    this->Items.Add("Jungle", "jungle4.tap", filetype::kc_tap, system::kc85_4);
    this->Items.Add("Pacman", "pacman.kcc", filetype::kcc, system::kc85_3);
    this->Items.Add("Breakout", "breakout.kcc", filetype::kcc, system::kc85_3);
    this->Items.Add("Mad Breakin", "breakin.853", filetype::kcc, system::kc85_3);
    this->Items.Add("Boulderdash", "boulder3.tap", filetype::kc_tap, system::kc85_3);
    this->Items.Add("Boulderdash", "boulder4.tap", filetype::kc_tap, system::kc85_4);
    this->Items.Add("Digger", "digger3.tap", filetype::kc_tap, system::kc85_3);
    this->Items.Add("Digger", "digger4.tap", filetype::kc_tap, system::kc85_4);
    this->Items.Add("Tetris", "tetris.kcc", filetype::kcc, system::kc85_4);
    this->Items.Add("Ladder", "ladder-3.kcc", filetype::kcc, system::kc85_3);
    this->Items.Add("Enterprise", "enterpri.tap", filetype::kc_tap, system::any_kc85);
    this->Items.Add("Chess", "chess.kcc", filetype::kcc, system::any_kc85);
    this->Items.Add("Testbild", "testbild.kcc", filetype::kcc, system::kc85_3);
    this->Items.Add("Demo1", "demo1.kcc", filetype::kcc, system::kc85_4);
    this->Items.Add("Demo2", "demo2.kcc", filetype::kcc, system::kc85_4);
    this->Items.Add("Demo3", "demo3.kcc", filetype::kcc, system::kc85_4);
    this->Items.Add("Tiny-Basic 3.01", "tinybasic-3.01.z80", filetype::kc_z80, system::z1013_01);
    this->Items.Add("KC-Basic", "kc_basic.z80", filetype::kc_z80, system::any_z1013);
    this->Items.Add("Z1013 Forth", "z1013_forth.z80", filetype::kc_z80, system::any_z1013);
    this->Items.Add("Boulderdash", "boulderdash_1_0.z80", filetype::kc_z80, system(int(system::z1013_16)|int(system::z1013_64)));
    this->Items.Add("Demolation", "demolation.z80", filetype::kc_z80, system::any_z1013);
    this->Items.Add("Cosmic Ball", "cosmic_ball.z80", filetype::kc_z80, system::z1013_01);
    this->Items.Add("Galactica", "galactica.z80", filetype::kc_z80, system::any_z1013);
    this->Items.Add("Mazogs", "mazog_deutsch.z80", filetype::kc_z80, system::any_z1013);
    this->Items.Add("Monitor ZM30 (start with 'ZM')", "zm30.kcc", filetype::kcc, system::any_z9001);
    this->Items.Add("Forth 83 (start with 'F83')", "F83_COM.TAP", filetype::kc_tap, system::any_z9001);
    this->Items.Add("Arkanoid", "arkanoid.sna", filetype::cpc_sna, system::any_cpc, true);
    this->Items.Add("Ghosts'n'Goblins", "ghosts_n_goblins.sna", filetype::cpc_sna, system::any_cpc, true);
    this->Items.Add("Gryzor", "gryzor.sna", filetype::cpc_sna, system::cpc6128, true);
    this->Items.Add("Dragon Ninja", "dragon_ninja.sna", filetype::cpc_sna, system::any_cpc, true);
    this->Items.Add("Head over Heels", "head_over_heels.sna", filetype::cpc_sna, system::any_cpc, true);
    this->Items.Add("Boulderdash", "boulder_dash.sna", filetype::cpc_sna, system::any_cpc, true);
    this->Items.Add("Bomb Jack", "bomb_jack.sna", filetype::cpc_sna, system::any_cpc, true);
    this->Items.Add("Chase HQ", "chase_hq.sna", filetype::cpc_sna, system::cpc6128, true);
    this->Items.Add("Cybernoid", "cybernoid.sna", filetype::cpc_sna, system::any_cpc, true);
    this->Items.Add("Fruity Frank", "fruity_frank.sna", filetype::cpc_sna, system(int(system::kccompact)|int(system::cpc6128)), true);
    this->Items.Add("Ikari Warriors", "ikari_warriors.sna", filetype::cpc_sna, system::any_cpc, true);
    this->Items.Add("1943", "1943.sna", filetype::cpc_sna, system::any_cpc, true);
    this->Items.Add("Rick Dangerous", "rick_dangerous.sna", filetype::cpc_sna, system::any_cpc, true);
    this->Items.Add("Donkey Kong", "donkey_kong.sna", filetype::cpc_sna, system::cpc6128, true);
    this->Items.Add("DTC (Demo)", "dtc.sna", filetype::cpc_sna, system::cpc6128, true);
    this->Items.Add("Wolfenstrad (Demo)", "wolfenstrad.sna", filetype::cpc_sna, system::cpc6128, true);
    this->Items.Add("Ecole Buissonniere (Demo)", "ecole_buissonniere.sna", filetype::cpc_sna, system::cpc6128, true);
    this->Items.Add("Backtro (Demo, broken)", "backtro.sna", filetype::cpc_sna, system::cpc6128, true);
    this->Items.Add("From Scratch (Demo, broken)", "from_scratch.sna", filetype::cpc_sna, system::cpc6128, true);
    this->Items.Add("Tire Au Flan (Demo, broken)", "tire_au_flan.sna", filetype::cpc_sna, system::any_cpc, true);
    #if !ORYOL_EMSCRIPTEN
    this->Items.Add("Acid Test: Colours", "cpcacid_colours.bin", filetype::cpc_bin, system::any_cpc, true);
    this->Items.Add("Acid Test: Border", "cpcacid_cpcborder.bin", filetype::cpc_bin, system::any_cpc, true);
    this->Items.Add("Acid Test: Col", "cpcacid_cpccol.bin", filetype::cpc_bin, system::any_cpc, true);
    this->Items.Add("Acid Test: Pen", "cpcacid_cpcpen.bin", filetype::cpc_bin, system::any_cpc, true);
    this->Items.Add("Acid Test: Pen 2", "cpcacid_cpcpen2.bin", filetype::cpc_bin, system::any_cpc, true);
    this->Items.Add("Acid Test: CPC Test", "cpcacid_cpctest.bin", filetype::cpc_bin, system::any_cpc, true);
    this->Items.Add("Acid Test: HBlank", "cpcacid_hblank.bin", filetype::cpc_bin, system::any_cpc, true);
    this->Items.Add("Acid Test: IOCol", "cpcacid_iocol.bin", filetype::cpc_bin, system::any_cpc, true);
    this->Items.Add("Acid Test: ModeTrig", "cpcacid_modetrig.bin", filetype::cpc_bin, system::any_cpc, true);
    this->Items.Add("Acid Test: OnlyInc", "cpcacid_onlyincpc.bin", filetype::cpc_bin, system::any_cpc, true);
    this->Items.Add("Acid Test: VBlank", "cpcacid_vblank.bin", filetype::cpc_bin, system::any_cpc, true);
    this->Items.Add("Acid Test: VBlank2", "cpcacid_vblank2.bin", filetype::cpc_bin, system::any_cpc, true);
    this->Items.Add("Acid Test: Video Test", "cpcacid_videotest.bin", filetype::cpc_bin, system::any_cpc, true);
    this->Items.Add("Acid Test: CPU", "cpcacid_cpu.bin", filetype::cpc_bin, system::any_cpc, true);
    this->Items.Add("Acid Test: InOut", "cpcacid_inout.bin", filetype::cpc_bin, system::any_cpc, true);
    this->Items.Add("Acid Test: PPI", "cpcacid_ppi.bin", filetype::cpc_bin, system::any_cpc, true);
    this->Items.Add("Acid Test: PPI Audio", "cpcacid_ppi_audio.bin", filetype::cpc_bin, system::any_cpc, true);
    this->Items.Add("Acid Test: PPI VSyncOut", "cpcacid_vsyncout.bin", filetype::cpc_bin, system::any_cpc, true);
    this->Items.Add("Acid Test: HSyncLen", "cpcacid_hsynclen.bin", filetype::cpc_bin, system::any_cpc, true);
    #endif
    this->Items.Add("1942", "1942.tap", filetype::cpc_tap, system::cpc464, true);
    this->Items.Add("Ghosts'n'Goblins", "ghostsng.tap", filetype::cpc_tap, system::any_cpc, true);
    this->Items.Add("Tir Na Nog", "tirnanog.tap", filetype::cpc_tap, system::any_cpc, true);
    this->Items.Add("Back to Reality", "backtore.tap", filetype::cpc_tap, system::any_cpc, true);
    this->Items.Add("ASSMON", "assmon.tap", filetype::cpc_tap, system::any_cpc, true);
    this->Items.Add("KC Pascal", "kcpascal.tap", filetype::cpc_tap, system::any_cpc, true);
    this->Items.Add("Bombjack 2", "bombjac1.tap", filetype::cpc_tap, system::any_cpc, true);
    this->Items.Add("Beverly Hills Cop", "beverlyh.tap", filetype::cpc_tap, system::any_cpc, true);
    this->Items.Add("Biff", "biff.tap", filetype::cpc_tap, system::any_cpc, true);
    this->Items.Add("Bubbler", "bubbler.tap", filetype::cpc_tap, system::any_cpc, true);
    this->Items.Add("Boulderdash 4", "boulder1.tap", filetype::cpc_tap, system::any_cpc, true);
    this->Items.Add("Combat Zone", "combatzo.tap", filetype::cpc_tap, system::any_cpc, true);
    this->Items.Add("Commandos", "commando.tap", filetype::cpc_tap, system::any_cpc, true);
    this->Items.Add("Curse of Sherwood", "curseofs.tap", filetype::cpc_tap, system::any_cpc, true);
    this->Items.Add("Kingdom of Speldom", "kingdomo.tap", filetype::cpc_tap, system::any_cpc, true);
    this->Items.Add("Haunted Hedges", "hauntedh.tap", filetype::cpc_tap, system::any_cpc, true);
    this->Items.Add("2088", "2088.tap", filetype::cpc_tap, system::any_cpc, true);
    this->Items.Add("Exolon", "exolon.z80", filetype::zx_z80, system::zxspectrum48k, true);
    this->Items.Add("Cyclone", "cyclone.z80", filetype::zx_z80, system::zxspectrum48k, true);
    this->Items.Add("Boulderdash", "boulderdash_zx.z80", filetype::zx_z80, system::zxspectrum48k, true);
    this->Items.Add("Bomb Jack", "bombjack_zx.z80", filetype::zx_z80, system::zxspectrum48k, true);
    this->Items.Add("Arkanoid", "arkanoid_zx128k.z80", filetype::zx_z80, system::zxspectrum128k, true);
    this->Items.Add("Silkworm", "silkworm_zx128k.z80", filetype::zx_z80, system::zxspectrum128k, true);
    this->Items.Add("Hello World!", "atom_hello.txt", filetype::text, system::acorn_atom, true);
    this->Items.Add("Text Mode Test", "atom_alnum_test.txt", filetype::text, system::acorn_atom, true);
    this->Items.Add("Graphics Mode Test", "atom_graphics_test.txt", filetype::text, system::acorn_atom, true);
    this->Items.Add("Atomic Memory Checker", "atom_memcheck.txt", filetype::text, system::acorn_atom, true);
    this->Items.Add("Keyboard Joystick Test", "atom_kbdjoytest.txt", filetype::text, system::acorn_atom, true);
    this->Items.Add("AtoMMC Joystick Test", "atom_mmcjoytest.txt", filetype::text, system::acorn_atom, true);
    this->Items.Add("Jet Set Willy", "JSW.TAP", filetype::atom_tap, system::acorn_atom, true);
    this->Items.Add("Atomic Chuckie Egg", "CCHUCK.TAP", filetype::atom_tap, system::acorn_atom, true);
    this->Items.Add("Hard Hat Harry", "hardhatharry.tap", filetype::atom_tap, system::acorn_atom, true);
    this->Items.Add("Jet Set Miner", "cjetsetminer.tap", filetype::atom_tap, system::acorn_atom, true);
    this->Items.Add("Dormann 6502 Test", "dormann6502.tap", filetype::atom_tap, system::acorn_atom, true);
    this->Items.Add("Boulderdash", "boulderdash_c64.tap", filetype::c64_tap, system::any_c64, true);
}

//------------------------------------------------------------------------------
void
FileLoader::Discard() {
    o_assert(this == pointer);
    pointer = nullptr;
}

//------------------------------------------------------------------------------
void
FileLoader::Load(const Item& item) {
    this->load(item, false);
}

//------------------------------------------------------------------------------
void
FileLoader::LoadAndStart(const Item& item) {
    this->load(item, true);
}

//------------------------------------------------------------------------------
void
FileLoader::LoadTape(const Item& item) {
    StringBuilder strBuilder;
    strBuilder.Format(128, "kcc:%s", item.Filename.AsCStr());
    this->Url = strBuilder.GetString();
    this->State = Loading;
    IO::Load(strBuilder.GetString(),
        // load succeeded
        [this, item](IO::LoadResult ioResult) {
            this->FileData = std::move(ioResult.Data);
            this->Info = parseHeader(this->FileData, item);
            this->State = Ready;
            tape.insert_tape(item.Name.AsCStr(), item.Type, this->FileData.Data(), this->FileData.Size());
            this->emu->on_tape_inserted();
        },
        // load failed
        [this](const URL& url, IOStatus::Code ioStatus) {
            this->State = Failed;
            this->FailedStatus = ioStatus;
        });
}

//------------------------------------------------------------------------------
bool
FileLoader::Copy() {
    if (Ready == this->State) {
        this->quickload(this->emu, this->Info, this->FileData, false);
        return true;
    }
    else {
        return false;
    }
}

//------------------------------------------------------------------------------
bool
FileLoader::Start() {
    if (Ready == this->State) {
        this->quickload(this->emu, this->Info, this->FileData, true);
        return true;
    }
    else {
        return false;
    }
}

//------------------------------------------------------------------------------
Buffer&&
FileLoader::ObtainTextBuffer() {
    this->State = Waiting;
    return std::move(this->FileData);
}

//------------------------------------------------------------------------------
void
FileLoader::load(const Item& item, bool autostart) {
    StringBuilder strBuilder;
    strBuilder.Format(128, "kcc:%s", item.Filename.AsCStr());
    this->Url = strBuilder.GetString();
    this->State = Loading;
    IO::Load(strBuilder.GetString(),
        // load succeeded
        [this, item, autostart](IO::LoadResult ioResult) {
            this->FileData = std::move(ioResult.Data);
            this->Info = parseHeader(this->FileData, item);
            this->State = Ready;
            quickload(this->emu, this->Info, this->FileData, autostart);
        },
        // load failed
        [this](const URL& url, IOStatus::Code ioStatus) {
            this->State = Failed;
            this->FailedStatus = ioStatus;
        });
}

//------------------------------------------------------------------------------
FileLoader::FileInfo
FileLoader::parseHeader(const Buffer& data, const Item& item) {
    FileInfo info;
    info.Filename = item.Filename;
    info.EnableJoystick = item.EnableJoystick;
    if (filetype::none == item.Type) {
        // guess the file type
        StringBuilder strb(item.Filename);
        if (strb.Contains(".TXT") || strb.Contains(".txt")) {
            info.Type = filetype::text;
        }
        else if (strb.Contains(".TAP") || strb.Contains(".tap")) {
            if (this->emu->is_system(system::any_zx)) {
                info.Type = filetype::zx_tap;
            }
            else if (this->emu->is_system(system::any_cpc)) {
                info.Type = filetype::cpc_tap;
            }
            else if (this->emu->is_system(system::acorn_atom)) {
                info.Type = filetype::atom_tap;
            }
            else {
                info.Type = filetype::kc_tap;
            }
        }
        else if (strb.Contains(".KCC") || strb.Contains(".kcc")) {
            info.Type = filetype::kcc;
        }
        else if (strb.Contains(".Z80") || strb.Contains(".z80")) {
            if (this->emu->is_system(system::any_zx)) {
                info.Type = filetype::zx_z80;
            }
            else {
                info.Type = filetype::kc_z80;
            }
        }
        else if (strb.Contains(".SNA") || strb.Contains(".sna")) {
            info.Type = filetype::cpc_sna;
        }
        else if (strb.Contains(".BIN") || strb.Contains(".bin")) {
            info.Type = filetype::raw;
        }
    }
    else {
        info.Type = item.Type;
    }
    if (data.Empty()) {
        return info;
    }
    const uint8_t* start = data.Data();
    const uint8_t* ptr = start;
    if ((filetype::kc_tap == info.Type) || (filetype::kcc == info.Type)) {
        if (filetype::kc_tap == info.Type) {
            ptr = (const uint8_t*)&(((kctap_header*)ptr)->kcc);
        }
        const kcc_header* hdr = (const kcc_header*) ptr;
        info.Name = String((const char*)hdr->name, 0, 16);
    }
    else if (filetype::kc_z80 == info.Type) {
        const kcz80_header* hdr = (const kcz80_header*) ptr;
        info.Name = String((const char*)hdr->name, 0, 16);
    }
    else if (filetype::raw == info.Type) {
        info.Name = item.Name;
    }
    else if (filetype::zx_z80 == info.Type) {
        // http://www.worldofspectrum.org/faq/reference/z80format.htm
        const zxz80_header* hdr = (const zxz80_header*) ptr;
        const zxz80ext_header* ext_hdr = nullptr;
        uint16_t pc = (hdr->PC_h<<8 | hdr->PC_l) & 0xFFFF;
        if (0 == pc) {
            // Z80 version 2 or 3
            ext_hdr = (const zxz80ext_header*) (ptr + sizeof(zxz80_header));
            if (ext_hdr->hw_mode < 3) {
                info.RequiredSystem = system::zxspectrum48k;
            }
            else {
                info.RequiredSystem = system::zxspectrum128k;
            }
        }
        else {
            info.RequiredSystem = system::zxspectrum48k;
        }
    }
    else if (filetype::cpc_sna == info.Type) {
        // http://cpctech.cpc-live.com/docs/snapshot.html
        info.Name = item.Filename;
        info.RequiredSystem = system::any_cpc;  // FIXME
    }
    else if (filetype::cpc_tap == info.Type) {
        const cpctap_header* hdr = (const cpctap_header*) ptr;
        info.Name = String((const char*)hdr->name, 0, 16);
    }
    else if (filetype::atom_tap == info.Type) {
        const atomtap_header* hdr = (const atomtap_header*) ptr;
        info.Name = String((const char*)hdr->name, 0, 16);
        info.RequiredSystem = system::acorn_atom;
    }
    return info;
}

//------------------------------------------------------------------------------
void
FileLoader::quickload(yakc* emu, const FileInfo& info, const Buffer& data, bool autostart) {
    enum State newState = FileLoader::Waiting;
    if (filetype::text == info.Type) {
        // do nothing for text files, this will be checked
        // and caught from the outside
        newState = FileLoader::TextReady;
    }
    else {
        o_assert(info.Filename.IsValid());
        auto fp = emu->filesystem.open(info.Filename.AsCStr(), filesystem::mode::write);
        if (fp) {
            emu->filesystem.write(fp, data.Data(), data.Size());
            emu->filesystem.close(fp);
            emu->quickload(info.Filename.AsCStr(), info.Type, autostart);
            emu->enable_joystick(true);
            if (!autostart) {
                newState = FileLoader::Ready;
            }
        }
    }
    FileLoader::pointer->State = newState;
}

//------------------------------------------------------------------------------
bool
FileLoader::LoadAuto(const Item& item) {
    if (int(item.Compat) & int(this->emu->model)) {
        if (filetype_quickloadable(item.Type)) {
            this->LoadAndStart(item);
        }
        else {
            // load through tape deck
            this->LoadTape(item);

            // send the right load/run BASIC command to the emulator
            const char* cmd = this->emu->load_tape_cmd();
            if (cmd) {
                Buffer buf;
                buf.Add((const uint8_t*)cmd, strlen(cmd)+1);
                Keyboard::self->StartPlayback(std::move(buf));
            }
            this->emu->enable_joystick(true);
        }
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
bool
FileLoader::LoadAuto(const Item& item, const uint8_t* data, int size) {
    this->FileData.Clear();
    this->FileData.Add(data, size);
    this->Info = this->parseHeader(this->FileData, item);
    this->State = FileLoader::Ready;
    if (int(this->Info.RequiredSystem) & int(this->emu->model)) {
        if (filetype_quickloadable(this->Info.Type)) {
            this->quickload(this->emu, this->Info, this->FileData, true);
            return true;
        }
        else if (filetype::none != this->Info.Type){
            // load through tape deck
            tape.insert_tape(item.Filename.AsCStr(), this->Info.Type, data, size);
            this->emu->on_tape_inserted();

            // send the right load/run BASIC command to the emulator
            const char* cmd = this->emu->load_tape_cmd();
            if (cmd) {
                Buffer buf;
                buf.Add((const uint8_t*)cmd, strlen(cmd)+1);
                Keyboard::self->StartPlayback(std::move(buf));
            }
            this->emu->enable_joystick(true);
            return true;
        }
        else {
            Log::Warn("filetype not accepted\n");
        }
    }
    else {
        Log::Warn("filetype not compatible with current system\n");
    }
    return false;
}

} // namespace YAKC

