//------------------------------------------------------------------------------
//  crt.cc
//------------------------------------------------------------------------------
#include "crt.h"

namespace YAKC {

//------------------------------------------------------------------------------
void
crt::init(int std, int x, int y, int w, int h) {
    // FIXME: support NTSC
    this->video_standard = std;
    this->visible = false;
    this->vis_area.x0 = this->h_disp_start + x;
    this->vis_area.y0 = this->v_disp_start + y;
    this->vis_area.x1 = this->vis_area.x0 + w;
    this->vis_area.y1 = this->vis_area.y0 + h;
}

//------------------------------------------------------------------------------
void
crt::step() {
    bool next_scanline = false;
    this->h_pos++;
    if (this->h_pos == this->h_disp_start) {
        this->h_black = false;
    }
    if (this->h_retrace > 0) {
        this->h_retrace--;
        if (this->h_retrace == 0) {
            this->h_pos = 0;
            next_scanline = true;
        }
    }
    if (next_scanline) {
        this->v_pos++;
        if (this->v_pos == this->v_disp_start) {
            this->v_black = false;
        }
        if (this->v_retrace > 0) {
            this->v_retrace--;
            if (this->v_retrace == 0) {
                this->v_pos = 0;
            }
        }
    }

    if ((this->h_pos >= this->vis_area.x0) &&
        (this->h_pos < this->vis_area.x1) &&
        (this->v_pos >= this->vis_area.y0) &&
        (this->v_pos < this->vis_area.y1))
    {
        this->visible = true;
        this->x = this->h_pos - this->vis_area.x0;
        this->y = this->v_pos - this->vis_area.y0;
    }
    else {
        this->visible = false;
    }
}

//------------------------------------------------------------------------------
void
crt::trigger_hsync() {
    if (0 == this->h_retrace) {
        this->h_retrace = 7;
        this->h_black = true;
    }
}

//------------------------------------------------------------------------------
void
crt::trigger_vsync() {
    if (0 == this->v_retrace) {
        this->v_retrace = 3;
        this->v_black = true;
    }
}

} // namespace YAKC
