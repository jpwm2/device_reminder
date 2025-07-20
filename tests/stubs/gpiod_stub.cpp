#include "gpiod_stub.h"
#include <string>

struct gpiod_chip { std::string name; };
struct gpiod_line { unsigned int number; };

static int fail_chip_open = 0;
static int fail_get_line = 0;
static int request_input_result = 0;
static int get_value_result = 0;

extern "C" {

void gpiod_stub_reset(void) {
    fail_chip_open = 0;
    fail_get_line = 0;
    request_input_result = 0;
    get_value_result = 0;
}

void gpiod_stub_set_fail_chip_open(int val) { fail_chip_open = val; }
void gpiod_stub_set_fail_get_line(int val) { fail_get_line = val; }
void gpiod_stub_set_request_input_result(int val) { request_input_result = val; }
void gpiod_stub_set_get_value_result(int val) { get_value_result = val; }

struct gpiod_chip* gpiod_chip_open(const char* chipname) {
    if (fail_chip_open) return nullptr;
    static gpiod_chip chip;
    chip.name = chipname ? chipname : "";
    return &chip;
}

void gpiod_chip_close(struct gpiod_chip* chip) {
    (void)chip;
}

struct gpiod_line* gpiod_chip_get_line(struct gpiod_chip* chip, unsigned int line) {
    (void)chip;
    if (fail_get_line) return nullptr;
    static gpiod_line line_obj;
    line_obj.number = line;
    return &line_obj;
}

int gpiod_line_request_input(struct gpiod_line* line, const char* consumer) {
    (void)line; (void)consumer;
    return request_input_result;
}

int gpiod_line_get_value(struct gpiod_line* line) {
    (void)line;
    return get_value_result;
}

void gpiod_line_release(struct gpiod_line* line) {
    (void)line;
}

} // extern "C"
