#include "setpriority_stub.h"

SetPriorityMock* g_setpriority_mock = nullptr;

extern "C" int setpriority(int which, id_t who, int prio) {
    if (g_setpriority_mock) {
        return g_setpriority_mock->call(which, who, prio);
    }
    return 0;
}
