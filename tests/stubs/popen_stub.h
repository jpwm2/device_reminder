#ifndef POPEN_STUB_H
#define POPEN_STUB_H
#include <stdio.h>
#ifdef __cplusplus
extern "C" {
#endif
void popen_stub_reset(void);
void popen_stub_set_fail(int v);
void popen_stub_set_output(const char* str);
#ifdef __cplusplus
}
#endif
#endif // POPEN_STUB_H
