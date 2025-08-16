#ifndef SETPRIORITY_STUB_H
#define SETPRIORITY_STUB_H
#include <sys/resource.h>
#include <gmock/gmock.h>

class SetPriorityMock {
public:
    MOCK_METHOD(int, call, (int which, id_t who, int prio));
};

extern SetPriorityMock* g_setpriority_mock;

#endif
