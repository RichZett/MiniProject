
#include "rtdb.h"

struct system_state_t rtdb = {
    .system_on = false,
    .set_temp = 25,
    .current_temp = 25,
};

K_MUTEX_DEFINE(rtdb_mutex);
