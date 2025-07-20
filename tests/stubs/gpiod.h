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
int gpiod_line_get_value(struct gpiod_line* line);
void gpiod_line_release(struct gpiod_line* line);

#ifdef __cplusplus
}
#endif

#endif // GPIOD_STUB_H
