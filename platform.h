#pragma once

int get_active_gpio();
void update_status(float percentage_full, PUMP_STATE state, const char* detail);