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
#include "yakc_oryol/Keyboard.h"

namespace YAKC {

class FileLoader {
public:
    static FileLoader* pointer;

    /// a load item
    struct Item {
        Item(const char* n, const char* fn, filetype t, system compat, bool enableJoystick=false) :
            Name(n), Filename(fn), Type(t), Compat(compat), OptStartAddr(0), OptExecAddr(0), EnableJoystick(true) {};
        Item(const char* n, const char* fn, filetype t, system compat, uint16_t startAddr, uint16_t execAddr) :
            Name(n), Filename(fn), Type(t), Compat(compat), OptStartAddr(startAddr), OptExecAddr(execAddr) {};
        Oryol::String Name;
        Oryol::String Filename;
        filetype Type;
        system Compat;
        uint16_t OptStartAddr;
        uint16_t OptExecAddr;
        bool EnableJoystick;
    };
    /// available items
    Oryol::Array<Item> Items;

    /// loading state
    enum State {
        Waiting,
        Loading,
        Ready,
        TextReady,      // special case external text stream ready
        Failed,
    } State = Waiting;

    /// file info of last loaded file
    struct FileInfo {
        Oryol::String Filename;
        Oryol::String Name;
        filetype Type = filetype::none;
        system RequiredSystem = system::any;
        bool EnableJoystick = false;
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
    /// automatically load and start a loader item
    bool LoadAuto(const Item& item);
    /// automatically load and start from a data block
    bool LoadAuto(const Item& item, const uint8_t* data, int size);
    /// load a file, and don't automatically start
    void Load(const Item& item);
    /// load and auto-start a file
    void LoadAndStart(const Item& item);
    /// load and insert into tape deck
    void LoadTape(const Item& item);
    /// copy the previously loaded file into Z80 memory
    bool Copy();
    /// copy to memory and start the previously loaded file
    bool Start();
    /// obtain text buffer (if a text file has been loaded)
    Oryol::Buffer&& ObtainTextBuffer();

    /// get file info from loaded file data
    FileInfo parseHeader(const Oryol::Buffer& data, const Item& item);
    /// quickload the provided file data
    static void quickload(yakc* emu, const FileInfo& info, const Oryol::Buffer& data, bool autostart);
    /// start loading, and then call quickload
    void load(const Item& item, bool autostart);

    Oryol::Buffer FileData;
private:
    yakc* emu = nullptr;
};

} // namespace YAKC

