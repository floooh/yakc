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
        RAW = 0,
        KCC,
        KC_TAP,
        KC_Z80,
        ZX_TAP,
        ZX_Z80,

        Num,
        None,
    };

    /// a load item
    struct Item {
        Item(const char* n, const char* fn, FileType t, device compat) :
            Name(n), Filename(fn), Type(t), Compat(compat), OptStartAddr(0), OptExecAddr(0) {};
        Item(const char* n, const char* fn, FileType t, device compat, uword startAddr, uword execAddr) :
            Name(n), Filename(fn), Type(t), Compat(compat), OptStartAddr(startAddr), OptExecAddr(execAddr) {};
        Oryol::String Name;
        Oryol::String Filename;
        FileType Type;
        device Compat;
        uword OptStartAddr;
        uword OptExecAddr;
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
        Oryol::String Filename;
        Oryol::String Name;
        FileType Type = FileType::None;
        uword StartAddr = 0;
        uword EndAddr = 0;
        uword ExecAddr = 0;
        bool HasExecAddr = false;
        bool FileSizeError = false;
        int PayloadOffset = 0;
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
    void Load(const Item& item);
    /// load and auto-start a file
    void LoadAndStart(const Item& item);
    /// copy the previously loaded file into Z80 memory
    bool Copy();
    /// copy to memory and start the previously loaded file
    bool Start();

    /// internal load method
    void load(const Item& item, bool autostart);
    /// get file info from loaded file data
    FileInfo parseHeader(const Oryol::Buffer& data, const Item& item);
    /// copy data from loaded stream object into KC memory
    static void copy(yakc* emu, const FileInfo& info, const Oryol::Buffer& data);
    /// special-case patch loaded files
    static void patch(yakc* emu, const FileInfo& info);
    /// auto-start the loaded program
    static void start(yakc* emu, const FileInfo& info);

    Oryol::Buffer FileData;
private:
    yakc* emu = nullptr;
};

} // namespace YAKC

