#pragma once
//------------------------------------------------------------------------------
/**
    @class YAKC::tapedeck
    @brief a sort-of-tapedeck abstraction
*/
#include "yakc/core/core.h"
#include "yakc/core/filesystem.h"

namespace YAKC {

class tapedeck {
public:
    /// insert a new 'tape', remove the previous 'tape'
    bool insert_tape(const char* name, filetype type, const void* data, int num_bytes);
    /// press the play button
    void play();
    /// press the stop button
    void stop();

    /// read data from the tape
    int read(void* ptr, int num_bytes);
    /// test if there's more data on the tape
    bool eof();

    /// get name of current tape
    const char* tape_name() const;
    /// get filetype of current tape
    filetype tape_filetype() const;
    /// return true if the tape is currently playing
    bool is_playing() const;

    /// update the tape counter (can be used as progress indicator)
    void inc_counter(int val);
    /// current counter value
    int counter() const;

private:
    filesystem fs;
    filesystem::file fp = filesystem::invalid_file;
    static const int max_name_len = filesystem::file_item::max_name_size;
    char name[max_name_len] = { };
    filetype type = filetype::none;
    bool playing = false;
    int count = 0;
};

} // namespace YAKC

