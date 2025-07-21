#ifndef GPIOD_STUB_H
#define GPIOD_STUB_H

#ifdef __cplusplus
extern "C" {
#endif

struct gpiod_chip;
struct gpiod_line;

struct gpiod_chip* gpiod_chip_open(const char* chipname);
void gpiod_chip_close(struct gpiod_chip* chip);
struct gpiod_line* gpiod_chip_get_line(struct gpiod_chip* chip, unsigned int line);
int gpiod_line_request_input(struct gpiod_line* line, const char* consumer);
int gpiod_line_request_output(struct gpiod_line* line, const char* consumer, int default_val);
int gpiod_line_set_value(struct gpiod_line* line, int value);
int gpiod_line_request_rising_edge_events(struct gpiod_line* line, const char* consumer);
int gpiod_line_request_falling_edge_events(struct gpiod_line* line, const char* consumer);
int gpiod_line_request_both_edges_events(struct gpiod_line* line, const char* consumer);
int gpiod_line_event_wait(struct gpiod_line* line, const struct timespec* timeout);
int gpiod_line_get_value(struct gpiod_line* line);
void gpiod_line_release(struct gpiod_line* line);

#ifdef __cplusplus
}
#endif

#endif // GPIOD_STUB_H
