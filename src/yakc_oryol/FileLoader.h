#pragma once
//------------------------------------------------------------------------------
/**
    @class YAKC::FileLoader
    @brief helper class to load KCC files
*/
#include "yakc/yakc.h"
#include "Core/String/String.h"
#include "Core/Containers/Array.h"
#include "IO/IO.h"

namespace YAKC {

class FileLoader {
public:
    static FileLoader* ptr;

    /// file types
    enum class FileType {
        None,
        UNKNOWN,
        KCC,
        TAP,
        Z80,
        BIN,        // raw file, requires start address
    };

    /// a load item
    struct Item {
        Item(const char* n, const char* fn, FileType t, device compat) :
            Name(n), Filename(fn), Type(t), Compat(compat), StartAddr(0), ExecAddr(0) {};
        Item(const char* n, const char* fn, FileType t, device compat, uword startAddr, uword execAddr) :
            Name(n), Filename(fn), Type(t), Compat(compat), StartAddr(startAddr), ExecAddr(execAddr) {};
        Oryol::String Name;
        Oryol::String Filename;
        FileType Type;
        device Compat;
        uword StartAddr;
        uword ExecAddr;
    };
    /// available items
    Oryol::Array<Item> Items;

    /// loading state
    enum State {
        Waiting,
        Loading,
        Ready,
        Failed,
    } State = Waiting;

    /// file info of last loaded file
    struct FileInfo {
        Oryol::String Name;
        uword StartAddr = 0;
        uword EndAddr = 0;
        uword ExecAddr = 0;
        bool HasExecAddr = false;
        bool FileSizeError = false;
        FileType Type = FileType::None;
        int PayloadOffset = 0;

        const char* TypeAsString() const {
            switch (this->Type) {
                case FileType::KCC: return "KCC";
                case FileType::TAP: return "TAP";
                case FileType::Z80: return "Z80";
                default:            return "???";
            }
        };
    } Info;
    /// url of last loaded file
    Oryol::URL Url;
    /// error code if loading has failed
    Oryol::IOStatus::Code FailedStatus = Oryol::IOStatus::OK;
    /// flips to true if externally provided data available
    bool ExtFileReady = false;

    /// setup the file loader object
    void Setup(yakc& emu);
    /// discard the loader object
    void Discard();
    /// load a file, and don't automatically start
    void Load(yakc& emu, const Item& item);
    /// load and auto-start a file
    void LoadAndStart(yakc& emu, const Item& item);
    /// copy the previously loaded file into Z80 memory
    bool Copy(yakc& emu);
    /// copy to memory and start the previously loaded file
    bool Start(yakc& emu);

    /// internal load method
    void load(yakc* emu, const Item& item, bool autostart);
    /// get file info from loaded file data
    static FileInfo parseHeader(const Oryol::Buffer& data, const Item& item);
    /// copy data from loaded stream object into KC memory
    static void copy(yakc* emu, const FileInfo& info, const Oryol::Buffer& data);
    /// special-case patch loaded files
    static void patch(yakc* emu, const FileInfo& info);
    /// auto-start the loaded program
    static void start(yakc* emu, const FileInfo& info);

    /// KCC file format header block
    #pragma pack(push,1)
    struct kcc_header {
        ubyte name[10];
        ubyte reserved[6];
        ubyte num_addr;
        ubyte load_addr_l;    // NOTE: odd offset!
        ubyte load_addr_h;
        ubyte end_addr_l;
        ubyte end_addr_h;
        ubyte exec_addr_l;
        ubyte exec_addr_h;
        ubyte pad[128 - 23];  // pad to 128 bytes
    };
    #pragma pack(pop)

    /// TAP file format header block
    #pragma pack(push,1)
    struct tap_header {
        ubyte sig[16];              // "\xC3KC-TAPE by AF. ";
        ubyte type;                 // 00: KCTAP_Z9001, 01: KCTAP_KC85, else: KCTAB_SYS
        kcc_header kcc;             // from here on identical with KCC
    };
    #pragma pack(pop)

    /// Z80 file format header block
    #pragma pack(push,1)
    struct z80_header {
        ubyte load_addr_l;
        ubyte load_addr_h;
        ubyte end_addr_l;
        ubyte end_addr_h;
        ubyte exec_addr_l;
        ubyte exec_addr_h;
        ubyte free[6];
        ubyte typ;
        ubyte d3[3];        // d3 d3 d3
        ubyte name[16];
    };
    #pragma pack(pop)

    Oryol::Buffer FileData;
};

} // namespace YAKC

