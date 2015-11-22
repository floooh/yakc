#pragma once
//------------------------------------------------------------------------------
/**
    @class Audio.h
    @brief implement audio playback callback
*/
#include "KC85Oryol.h"
#include "Sound/Sound.h"

class Audio {
public:
    /// setup audio playback
    void Setup(yakc::kc85& kc);
    /// shutdown audio playback
    void Discard();
};