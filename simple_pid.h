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

int pid_init(simple_pid_t *pid, const char *name, float min, float max,
             float kp, float kd, float ki, uint32_t start_time);
float pid_update(simple_pid_t *pid, const float setpoint,
                 const float process_value, const uint32_t timestamp);

#endif
