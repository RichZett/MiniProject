
#ifndef RTDB_H
#define RTDB_H

#include <zephyr/kernel.h>

struct system_state_t {
    bool system_on;
    int set_temp;
    int current_temp;
};

extern struct system_state_t rtdb;
extern struct k_mutex rtdb_mutex;

#endif
