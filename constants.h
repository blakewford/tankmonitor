#pragma once

#define INVALID -1

const float TANK_DEPTH = 70.0f; // cm

enum PUMP_STATE: int32_t
{
    UNKNOWN,
    NORMAL,
    RUNNING,
    INEFFECTIVE
};

static const char* STATE_STRINGS[] =
{
    "UNKNOWN",
    "NORMAL",
    "RUNNING",
    "INEFFECTIVE"
};

const int32_t DEBOUNCE_MS = 3000;

static const char* LEVEL = "LIQUID";
static const char* ESTOP = "STOP_GRINDER";
