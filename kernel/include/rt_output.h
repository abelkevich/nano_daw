#pragma once

namespace Playback
{
    status_t play();
    status_t pause();
    status_t stop();
    status_t init();
    status_t render_selection(uint32_t from_ms, uint32_t to_ms);
}