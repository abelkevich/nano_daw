#pragma once
#include "common.h"
#include "session.h"

status_t render(Session ses, std::string mix_path);
status_t renderAll(std::string mix_path);