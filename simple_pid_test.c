#include <stdbool.h>
#include <stdlib.h>

#include "acutest.h"
#include "simple_pid.h"

static bool float_close(const float v1, const float v2, const float epsilon)
{
    return (v1 + epsilon > v2) && (v1 - epsilon < v2);
}

static void simple_test(void)
{
    simple_pid_t pid;

    TEST_ASSERT(pid_init(&pid, "simple", 0, 100, 0.1, 0.1, 0.1, 0) >= 0);

    /* Run a simple PID where the output value is the same as the input value
     */
    float setpoint = 100.0;
    bool good = false;
    float output = 0.0;
    for (int i = 0; i < 1000 && !good; i++) {
        output = pid_update(&pid, setpoint, output, i);
        // Check we're in range
        good = float_close(output, setpoint, 0.2);
    }
    TEST_ASSERT_(good, "Output never stabilised to %f, reached %f", setpoint,
                 output);
}

/* Returns a non-zero float up to & including max */
static float rand_float(float max)
{
    return ((float)((rand() % 100000) + 1) / 100000) * max;
}

static void random_test(void)
{
    for (int t = 0; t < 1000; t++) {
        simple_pid_t pid;
        float kp = rand_float(0.1);
        float kd = rand_float(0.1);
        float ki = rand_float(0.1);
        float setpoint = rand_float(200) - 100;
        float output = 0.0;
        bool good = false;
        int i;

        TEST_ASSERT(pid_init(&pid, "random", -100, 100, kp, kd, ki, 0) >= 0);

        for (i = 0; i < 10000000 && !good; i++) {
            output = pid_update(&pid, setpoint, output, i);
            good = float_close(output, setpoint, 0.2);
        }
        TEST_CHECK_(good,
                    "%d: Output never stabilised to %f, reached %f. "
                    "Integral=%f last_error=%f",
                    t, setpoint, output, pid.integral, pid.last_error);
        // printf("%d: Took %d iterations to reach %f, kp=%f kd=%f ki=%f\n",
        // t, i, setpoint, kp, kd, ki);
    }
}

// returns speed in m/s given previous speed in m/s, acceleration in m/s^2 &
// time delta in second
static float calculate_car_pos(float prev_speed, float acceleration, float dt)
{
    return prev_speed + acceleration * dt;
}

static void cruise_control_test(void)
{
    for (int t = 0; t < 100; t++) {
        float speed = rand_float(100) - 50;
        float target = rand_float(100);
        float acceleration = 0.0;
        simple_pid_t pid;
        bool good = false;

        TEST_ASSERT(pid_init(&pid, "cruise control", -100, 100,
                             rand_float(0.2), rand_float(0.01),
                             rand_float(0.1), 0) >= 0);

        for (int i = 0; i < 10000 && !good; i++) {
            float dt = 0.01;
            float accel_change = pid_update(&pid, target, speed, i);
            acceleration += accel_change / 100;
            if (acceleration > 10)
                acceleration = 10;
            if (acceleration < -10)
                acceleration = -10;
            speed = calculate_car_pos(speed, acceleration, dt);
            // printf("Iteration %d, acceleration %f, speed %f, target %f\n",
            // i, acceleration, speed, target);
            good = float_close(speed, target, 0.2);
        }
        TEST_CHECK_(good,
                    "%d: Output never stabilised to %f, reached %f. "
                    "Integral=%f last_error=%f",
                    t, target, speed, pid.integral, pid.last_error);
    }
}

TEST_LIST = {
    {"simple", simple_test},
    {"multiple", random_test},
    {"cruise", cruise_control_test},
    {NULL, NULL} // zeroed record marking the end of the list
};