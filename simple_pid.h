/**
 * Simple standalone implementation of a PID control loop.
 * See https://en.wikipedia.org/wiki/PID_controller for details
 *
 * (c) Andre Renaud, 2020
 *
 */
#ifndef SIMPLE_PID_H
#define SIMPLE_PID_H

#include <stdint.h>
#include <stdio.h>

typedef struct {
    char name[20];
    float last_val;
    float min;
    float max;
    float kp;
    float kd;
    float ki;
    uint32_t last_time;
    float integral;
    float last_error;
    FILE *csv_output;
} simple_pid_t;

/**
 * Initialise a new PID controller
 * @param name: Name to use for debugging messages (see SIMPLE_PID_DEBUG)
 * @param min: Minimum output value from the PID
 * @param max: Maximum output value for the pid
 * @param kp: Proportional gain multiplier
 * @param ki: Integral gain multiplier
 * @param kd: Derivative gain multiplier
 * @param start_time: Current time (to calculate the first time delta from)
 * @return < 0 on failure, >= 0 on success
 */
int pid_init(simple_pid_t *pid, const char *name, float min, float max,
             float kp, float ki, float kd, uint32_t start_time);

/**
 * Update the PID to obtain a new output value
 * @param pid: PID to update
 * @param setpoint: Desired outcome
 * @param process_value: Current state of teh system
 * @param timestamp: Monotonically increasing time stamp for speed control
 * @return New output value to be applied
 */
float pid_update(simple_pid_t *pid, const float setpoint,
                 const float process_value, const uint32_t timestamp);

/**
 * Set the pid controller to output all calculated values to the given FILE
 * as a CSV
 * These values will be output each time pid_update is called, and written
 * in the following order:
 * timestamp, setpoint, process_value, p_out, i_out, d_out, output, integral,
 * error
 */
int pid_set_csv(simple_pid_t *pid, FILE *output);

#endif
