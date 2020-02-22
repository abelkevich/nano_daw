#pragma once

namespace Playback
{
    status_t play(uint32_t from_ms, uint32_t to_ms);
    status_t pause();
    status_t stop();
    status_t init();
}