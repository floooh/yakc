#pragma once
//------------------------------------------------------------------------------
/**
    @class LoadWindow
    @brief UI for loading KCC files from web server
*/
#include "yakc_ui/WindowBase.h"
#include "Core/String/String.h"
#include "IO/Core/IOQueue.h"

class LoadWindow : public WindowBase {
    OryolClassDecl(LoadWindow);
public:
    /// destructor
    ~LoadWindow();
    /// setup the window
    virtual void Setup(yakc::kc85& kc) override;
    /// draw method
    virtual bool Draw(yakc::kc85& kc) override;

    /// KCC file format header block
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
    /// file header translated from header
    struct fileInfo {
        Oryol::String name;
        yakc::uword startAddr = 0;
        yakc::uword endAddr = 0;
        yakc::uword execAddr = 0;
        bool hasExecAddr = false;
        bool fileSizeError = false;
    };

    /// get file info from loaded KCC file
    static fileInfo parseHeader(const Oryol::Ptr<Oryol::Stream>& data);
    /// copy data from loaded stream object into KC memory
    static void copy(yakc::kc85& kc, const fileInfo& info, const Oryol::Ptr<Oryol::Stream>& data);
    /// auto-start the loaded program
    static void start(yakc::kc85& kc, const fileInfo& info);

    Oryol::Array<Oryol::String> files;
    Oryol::IOQueue ioQueue;
    Oryol::URL url;
    Oryol::IOStatus::Code failedStatus = Oryol::IOStatus::OK;
    enum State {
        Waiting,
        Loading,
        Ready,
        Failed,
    } state = Waiting;
    Oryol::Ptr<Oryol::Stream> kccData;
};
