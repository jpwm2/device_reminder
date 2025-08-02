#ifndef GPIOD_STUB_CTRL_H
#define GPIOD_STUB_CTRL_H

#include "gpiod.h"

#ifdef __cplusplus
extern "C" {
#endif

void gpiod_stub_reset(void);
void gpiod_stub_set_fail_chip_open(int val);
void gpiod_stub_set_fail_get_line(int val);
void gpiod_stub_set_request_input_result(int val);
void gpiod_stub_set_get_value_result(int val);
void gpiod_stub_set_request_output_result(int val);
void gpiod_stub_set_set_value_result(int val);
int gpiod_stub_get_last_set_value(void);
void gpiod_stub_set_request_rising_result(int val);
void gpiod_stub_set_request_falling_result(int val);
void gpiod_stub_set_request_both_result(int val);
void gpiod_stub_set_event_wait_result(int val);

#ifdef __cplusplus
}
#endif

#endif // GPIOD_STUB_CTRL_H
