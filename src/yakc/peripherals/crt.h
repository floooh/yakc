#pragma once
//------------------------------------------------------------------------------
/**
    @class YAKC::crt
    @brief cathode-ray-tube emulation helper class
    
    NOTE: currently only PAL is supported
    
    https://en.wikipedia.org/wiki/Horizontal_blanking_interval
    http://www.batsocks.co.uk/readme/video_timing.htm
    
    For PAL, a 64us scanline has the following timings:
    
    Back Porch:         5us
    Display Area:       52us
    Front Porch:        2us
    Retrace:            5us

    At the start of Front Porch, the ray will wait until HSYNC arrives.
    
    A PAL frame has the following timings:
    
    Overall scanlines:  312 @ 64us
    First visible line: 5
    Visible scanlines:  304
*/
#include "yakc/core/core.h"

namespace YAKC {

class crt {
public:
    /// video standards
    enum {
        PAL = 0,
        NTSC,
    };

    /// initialize with PAL or NTSC and visible area
    void init(int video_standard, int x, int y, int w, int h);
    /// reset the object
    void reset();
    /// step the crt once per micro-second
    void step();
    /// trigger the HSYNC signal (start horizontal retrace)
    void trigger_hsync();
    /// trigger the VSYNC signal (start vertical retrace)
    void trigger_vsync();

    /// currently inside visible area
    bool visible = false;
    /// current framebuffer x/y position
    int x = 0;
    int y = 0;

    /// which video standard
    int video_standard = PAL;
    /// current horizontal position (0..63)
    int h_pos = 0;
    /// current vertical position (0..312)
    int v_pos = 0;
    /// horizontal blanking underway (retrace + porches)
    bool h_black = false;
    /// vertical blanking underdway
    bool v_black = false;
    /// horizontal retrace counter, this is >0 during horizontal retrace
    int h_retrace = 0;
    /// vertical retrace counter, this is >0 during vertical retrace
    int v_retrace = 0;

    /// horizontal markers
    int h_disp_start = 6;
    int h_disp_end = 6 + 52;

    /// vertical markers
    int v_disp_start = 5;
    int v_disp_end   = 5 + 304;

    // visible area
    struct {
        int x0 = 0;
        int y0 = 0;
        int x1 = 0;
        int y1 = 0;
    } vis_area;
};

} // namespace YAKC
