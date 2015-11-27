#pragma once
//------------------------------------------------------------------------------
/**
    @class Audio.h
    @brief implement audio playback callback
*/
#include "KC85Oryol.h"

class Audio {
public:
    /// setup audio playback
    void Setup(yakc::kc85& kc);
    /// shutdown audio playback
    void Discard();
    /// call per frame
    void Update();

private:
    /// ctc0 constant changed callback
    static void ctc0_const_changed(void* userdata);
    /// ctc1 constant changed callback
    static void ctc1_const_changed(void* userdata);
    /// update a sound voice
    void updateVoice(int channel);

    yakc::kc85* kc = nullptr;

    struct {
        yakc::ubyte mode = 0;
        yakc::ubyte constant = 0;
    } ctc_state[2];
};