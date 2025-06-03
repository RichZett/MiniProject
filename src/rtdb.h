/**
 * @file rtdb.h
 * @brief The Real-Time Database (RTDB) interface for the shared system
 */
#ifndef RTDB_H
#define RTDB_H

#include <zephyr/kernel.h>

/**
 * @struct system_state_t 
 * @brief Representation of the current state of the system. 
 * The struct is used to share and store key variables for the state between different parts of the system. 
 */
struct system_state_t {
    bool system_on;     /**< Indicates if system is on/off. */
    int set_temp;       /**< The set temperature that is wanted. */
    int current_temp;   /**< The current measured temperature. */
};

/**
 * @brief Variable that holds the current state of the system. Can be accessed in a thread safe manner. 
 */
extern struct system_state_t rtdb;

/**
 * @brief Mutex for synchronizing access to the system state.
 */
extern struct k_mutex rtdb_mutex;

#endif
