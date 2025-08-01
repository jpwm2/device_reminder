#include "popen_stub.h"
#include <cstdio>
#include <string>
#include <cerrno>

static std::string g_output;
static int g_fail = 0;

extern "C" FILE* popen(const char* command, const char* type) {
    (void)command; (void)type;
    if (g_fail) { errno = EINVAL; return nullptr; }
    FILE* fp = tmpfile();
    if (!fp) return nullptr;
    fwrite(g_output.c_str(), 1, g_output.size(), fp);
    rewind(fp);
    return fp;
}

extern "C" int pclose(FILE* stream) {
    return fclose(stream);
}

extern "C" void popen_stub_reset(void) {
    g_output.clear();
    g_fail = 0;
}

extern "C" void popen_stub_set_fail(int v) {
    g_fail = v;
}

extern "C" void popen_stub_set_output(const char* str) {
    g_output = str ? str : "";
}

