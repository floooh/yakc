//------------------------------------------------------------------------------
//  WindowBase.cc
//------------------------------------------------------------------------------
#include "WindowBase.h"
#include "Core/String/StringBuilder.h"

using namespace Oryol;

namespace yakc {

int WindowBase::globalUniqueId = 0;

//------------------------------------------------------------------------------
WindowBase::~WindowBase() {
    // empty
}

//------------------------------------------------------------------------------
void
WindowBase::setName(const char* str) {
    o_assert(str);
    StringBuilder strBuilder;
    strBuilder.Format(32, "%s##%d", str, ++globalUniqueId);
    this->title = strBuilder.GetString();
}

} // namespace yakc

