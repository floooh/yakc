#pragma once
//------------------------------------------------------------------------------
/**
    @class FileLoader
    @brief helper class to load KCC files
*/
#include "yakc/KC85Oryol.h"
#include "Core/String/String.h"
#include "Core/Containers/Array.h"
#include "IO/IO.h"

class FileLoader {
public:
    /// a load item
    struct Item {
        Item(const char* n, const char* fn, yakc::device compat) : Name(n), Filename(fn), Compat(compat) {};
        Oryol::String Name;
        Oryol::String Filename;
        yakc::device Compat;
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

    /// file types
    enum class FileType {
        None,
        KCC,
        TAP,
    };

    /// file info of last loaded file
    struct FileInfo {
        Oryol::String Name;
        yakc::uword StartAddr = 0;
        yakc::uword EndAddr = 0;
        yakc::uword ExecAddr = 0;
        bool HasExecAddr = false;
        bool FileSizeError = false;
        FileType Type = FileType::None;
        int PayloadOffset = 0;
    } Info;
    /// url of last loaded file
    Oryol::URL Url;
    /// error code if loading has failed
    Oryol::IOStatus::Code FailedStatus = Oryol::IOStatus::OK;

    /// setup the file loader object
    void Setup(yakc::kc85& kc);
    /// discard the loader object
    void Discard();
    /// load a file, and don't automatically start
    void Load(yakc::kc85& kc, const Item& item);
    /// load and auto-start a file
    void LoadAndStart(yakc::kc85& kc, const Item& item);
    /// copy the previously loaded file into Z80 memory
    bool Copy(yakc::kc85& kc);
    /// copy to memory and start the previously loaded file
    bool Start(yakc::kc85& kc);

private:
    /// internal load method
    void load(yakc::kc85* kc, const Item& item, bool autostart);
    /// get file info from loaded file data
    static FileInfo parseHeader(const Oryol::Buffer& data);
    /// copy data from loaded stream object into KC memory
    static void copy(yakc::kc85* kc, const FileInfo& info, const Oryol::Buffer& data);
    /// special-case patch loaded files
    static void patch(yakc::kc85* kc, const FileInfo& info);
    /// auto-start the loaded program
    static void start(yakc::kc85* kc, const FileInfo& info);

    /// KCC file format header block
    #pragma pack(push,1)
    struct kcc_header {
        yakc::ubyte name[10];
        yakc::ubyte reserved[6];
        yakc::ubyte num_addr;
        yakc::ubyte load_addr_l;    // NOTE: odd offset!
        yakc::ubyte load_addr_h;
        yakc::ubyte end_addr_l;
        yakc::ubyte end_addr_h;
        yakc::ubyte exec_addr_l;
        yakc::ubyte exec_addr_h;
        yakc::ubyte pad[128 - 23];  // pad to 128 bytes
    };
    #pragma pack(pop)

    /// TAP file format header block
    #pragma pack(push,1)
    struct tap_header {
        yakc::ubyte sig[16];        // "\xC3KC-TAPE by AF. ";
        yakc::ubyte type;           // 00: KCTAP_Z9001, 01: KCTAP_KC85, else: KCTAB_SYS
        kcc_header kcc;             // from here on identical with KCC
    };
    #pragma pack(pop)

    Oryol::Buffer kccData;
};
