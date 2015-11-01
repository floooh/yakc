//------------------------------------------------------------------------------
//  ui_window.cc
//------------------------------------------------------------------------------
#include "ui_window.h"
#include "Core/String/StringBuilder.h"

OryolClassImpl(window);

using namespace Oryol;
using namespace yakc;

int window::global_unique_id = 0;

//------------------------------------------------------------------------------
window::~window() {
    // empty
}

//------------------------------------------------------------------------------
void
window::setup(const kc85&) {
    // empty
}

//------------------------------------------------------------------------------
bool
window::draw(kc85&) {
    return false;
}

//------------------------------------------------------------------------------
void
window::set_name(const char* str) {
    o_assert(str);
    StringBuilder strBuilder;
    strBuilder.Format(32, "%s##%d", str, ++global_unique_id);
    this->title = strBuilder.GetString();
}
