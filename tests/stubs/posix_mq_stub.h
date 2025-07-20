#ifndef POSIX_MQ_STUB_H
#define POSIX_MQ_STUB_H
#include <mqueue.h>
#ifdef __cplusplus
extern "C" {
#endif
void mq_stub_reset(void);
void mq_stub_set_fail_open(int v);
void mq_stub_set_fail_send(int v);
void mq_stub_set_fail_receive(int v);
#ifdef __cplusplus
}
#endif
#endif
