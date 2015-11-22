//------------------------------------------------------------------------------
//  Audio.cc
//------------------------------------------------------------------------------
#include "Audio.h"

using namespace Oryol;
using namespace yakc;

//------------------------------------------------------------------------------
void
Audio::Setup() {
    // FIXME
}

//------------------------------------------------------------------------------
void
Audio::Discard() {
    // FIXME
}

//------------------------------------------------------------------------------
int
Audio::channelFreq(const kc85& kc, z80ctc::channel c) {
    auto& ctc0 = kc.ctc.channels[c];
    int f = ctc0.constant;
    if ((ctc0.mode & z80ctc::PRESCALER) == z80ctc::PRESCALER_256) {
        f *= 256;
    }
    else {
        f *= 16;
    }
    return f;
}

//------------------------------------------------------------------------------
void
Audio::Update(kc85& kc) {

    // check if channel on/off
    const bool c0Active = !(kc.ctc.channels[z80ctc::CTC0].mode & z80ctc::RESET);
    if (c0Active != this->chn0Active) {
        this->chn0Active = c0Active;
        if (c0Active) {
            //Log::Info("CTC0 sound ON\n");
        }
        else {
            //Log::Info("CTC0 sound OFF\n");
        }
    }
    const bool c1Active = !(kc.ctc.channels[z80ctc::CTC1].mode & z80ctc::RESET);
    if (c1Active != this->chn1Active) {
        this->chn1Active = c1Active;
        if (c1Active) {
            //Log::Info("CTC1 sound ON\n");
        }
        else {
            //Log::Info("CTC1 sound OFF\n");
        }
    }

    // check if frequencies have changed
    const int f0 = channelFreq(kc, z80ctc::CTC0);
    if (f0 != this->curFreq0) {
        //Log::Info("CTC0 freq changed!\n");
        this->curFreq0 = f0;
    }
    const int f1 = channelFreq(kc, z80ctc::CTC1);
    if (f1 != this->curFreq1) {
        //Log::Info("CTC1 freq changed!\n");
        this->curFreq1 = f1;
    }

    // check if volume has changed
    const int v = kc.pio.channel_data[z80pio::B] & kc85::PIO_B_VOLUME_MASK;
    if (v != this->curVolume) {
        //Log::Info("Volume changed!\n");
        this->curVolume = v;
    }
}
