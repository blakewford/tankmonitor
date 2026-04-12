#pragma once

#define INVALID -1

const float TANK_DEPTH = 50.0f; // cm

enum PUMP_STATE: int32_t
{
    UNKNOWN,
    NORMAL,
    RUNNING,
    COOL_DOWN
};

static const char* STATE_STRINGS[] =
{
    "UNKNOWN",
    "NORMAL",
    "RUNNING",
    "COOL_DOWN"
};

static const char* LEVEL = "LIQUID";
static const char* HALT  = "STOP_GRINDER";
static const char* RUN   = "RUN_GRINDER";