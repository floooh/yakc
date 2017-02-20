//------------------------------------------------------------------------------
//  tapedeck.cc
//------------------------------------------------------------------------------
#if _MSC_VER && !defined(_CRT_SECURE_NO_WARNINGS)
#define _CRT_SECURE_NO_WARNINGS
#endif
#include "tapedeck.h"

namespace YAKC {

//------------------------------------------------------------------------------
bool
tapedeck::insert_tape(const char* name_, filetype type_, const void* data, int num_bytes) {
    YAKC_ASSERT(name_ && data && num_bytes > 0);

    name[0] = 0;
    type = filetype::none;
    playing = false;
    count = 0;

    // remove previous tape?
    if (fp) {
        fs.close_rm(fp);
        fp = filesystem::invalid_file;
    }

    // store the new tape data into the memory filesystem
    strncpy(name, name_, sizeof(name));
    name[sizeof(name)-1] = 0;
    type = type_;
    playing = false;
    fp = fs.open(name, filesystem::mode::write);
    if (fp) {
        fs.write(fp, data, num_bytes);
        fs.close(fp);

        // open the file for reading
        fs.open(name, filesystem::mode::read);

        return true;
    }
    else {
        return false;
    }
}

//------------------------------------------------------------------------------
int
tapedeck::read(void* ptr, int num_bytes) {
    if (fp) {
        return fs.read(fp, ptr, num_bytes);
    }
    else {
        return 0;
    }
}

//------------------------------------------------------------------------------
bool
tapedeck::eof() {
    if (fp) {
        return fs.eof(fp);
    }
    else {
        return true;
    }
}

//------------------------------------------------------------------------------
void
tapedeck::play() {
    playing = true;
}

//------------------------------------------------------------------------------
void
tapedeck::stop() {
    playing = false;
}

//------------------------------------------------------------------------------
void
tapedeck::stop_rewind() {
    playing = false;
    count = 0;
    if (fp) {
        fs.set_pos(fp, 0);
    }
}

//------------------------------------------------------------------------------
const char*
tapedeck::tape_name() const {
    return name;
}

//------------------------------------------------------------------------------
filetype
tapedeck::tape_filetype() const {
    return type;
}

//------------------------------------------------------------------------------
bool
tapedeck::is_playing() const {
    return playing;
}

//------------------------------------------------------------------------------
void
tapedeck::inc_counter(int val) {
    count += val;
}

//------------------------------------------------------------------------------
int
tapedeck::counter() const {
    return count;
}

} // namespace YAKC

