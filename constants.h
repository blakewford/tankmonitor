#pragma once

#define INVALID -1

const float TANK_DEPTH = 70.0f; // cm
const int32_t PACE_MS = 2000;

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

static const char* LEVEL = "LIQUID";
static const char* RUN   = "RUN_GRINDER";
static const char* ESTOP = "STOP_GRINDER";
