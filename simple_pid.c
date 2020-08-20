#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "simple_pid.h"

#define SIMPLE_PID_DEBUG 0

static void pdebug(const char *fmt, ...)
    __attribute__((format(printf, 1, 2)));

static void pdebug(const char *fmt, ...)
{
#if SIMPLE_PID_DEBUG
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stdout, fmt, ap);
    va_end(ap);
#endif
}

int pid_init(simple_pid_t *pid, const char *name, float min, float max,
             float kp, float ki, float kd, uint32_t start_time)
{
    memset(pid, 0, sizeof(*pid));
    if (min > max)
        return -1;

    if (name) {
        strncpy(pid->name, name, sizeof(pid->name));
        pid->name[sizeof(pid->name) - 1] = '\0';
    }
    pid->min = min;
    pid->max = max;
    pid->kp = kp;
    pid->kd = kd;
    pid->ki = ki;
    pid->last_time = start_time;
    pid->integral = 0.0;
    pid->last_error = 0.0;

    return 0;
}

static float clamp(float var, float min, float max)
{
    if (var < min)
        return min;
    if (var > max)
        return max;
    return var;
}

float pid_update(simple_pid_t *pid, const float setpoint,
                 const float process_value, const uint32_t timestamp)
{
    float error = setpoint - process_value;
    uint32_t dt = timestamp - pid->last_time;
    float p_out, i_out, d_out, output;
    float derivative;

    if (dt == 0)
        derivative = 0;
    else
        derivative = (error - pid->last_error) / dt;

    pdebug(
        "pid_update %s: sp=%f pv=%f time=%d error=%f dt=%u derivative=%f\n",
        pid->name, setpoint, process_value, timestamp, error, dt, derivative);

    // Limit the integral so that it cannot contribute over the full range of
    // the output
    pid->integral = clamp(pid->integral + error * dt, pid->min / pid->ki,
                          pid->max / pid->ki);

    p_out = pid->kp * error;
    i_out = pid->ki * pid->integral;
    d_out = pid->kd * derivative;

    pid->last_time = timestamp;
    pid->last_error = error;

    output = clamp(p_out + i_out + d_out, pid->min, pid->max);

    if (pid->csv_output)
        fprintf(pid->csv_output, "%d,%f,%f,%f,%f,%f,%f,%f,%f\n", timestamp,
                setpoint, process_value, p_out, i_out, d_out, output,
                pid->integral, pid->last_error);

    pdebug("pid_update %s: p_out=%f i_out=%f d_out=%f output=%f\n", pid->name,
           p_out, i_out, d_out, output);

    return output;
}

int pid_set_csv(simple_pid_t *pid, FILE *output)
{
    if (!pid)
        return -1;
    pid->csv_output = output;
    if (pid->csv_output)
        fprintf(pid->csv_output, "timestamp,setpoint,processvalue,p_out,i_"
                                 "out,d_out,output,integral,error\n");
    return 0;
}