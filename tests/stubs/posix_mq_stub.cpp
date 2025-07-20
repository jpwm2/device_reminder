#include "posix_mq_stub.h"
#include <queue>
#include <string>
#include <cstring>
#include <errno.h>

struct StubMQ {
    std::queue<std::string> q;
};
static StubMQ g_mq;
static int fail_open = 0;
static int fail_send = 0;
static int fail_receive = 0;

extern "C" {

void mq_stub_reset(void) {
    while(!g_mq.q.empty()) g_mq.q.pop();
    fail_open = fail_send = fail_receive = 0;
}
void mq_stub_set_fail_open(int v) { fail_open = v; }
void mq_stub_set_fail_send(int v) { fail_send = v; }
void mq_stub_set_fail_receive(int v) { fail_receive = v; }

mqd_t mq_open(const char*, int, ...) {
    if(fail_open) { errno = EINVAL; return (mqd_t)-1; }
    return 1; // dummy descriptor
}
int mq_close(mqd_t) { return 0; }
int mq_unlink(const char*) { return 0; }
int mq_send(mqd_t, const char* msg_ptr, size_t msg_len, unsigned int) {
    if(fail_send) { errno = EINVAL; return -1; }
    g_mq.q.emplace(msg_ptr, msg_len);
    return 0;
}
ssize_t mq_receive(mqd_t, char* msg_ptr, size_t msg_len, unsigned int*) {
    if(fail_receive || g_mq.q.empty()) { errno = EAGAIN; return -1; }
    auto& s = g_mq.q.front();
    size_t n = std::min(msg_len, s.size());
    std::memcpy(msg_ptr, s.data(), n);
    g_mq.q.pop();
    return static_cast<ssize_t>(n);
}

} // extern "C"
