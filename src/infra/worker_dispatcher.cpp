#include "worker_dispatcher/worker_dispatcher.hpp"

#include <chrono>
#include <thread>

namespace device_reminder {

WorkerDispatcher::WorkerDispatcher(key_t, MessageHandler handler, long)
    : handler_(std::move(handler)) {}

WorkerDispatcher::~WorkerDispatcher() {
    stop();
    join();
}

void WorkerDispatcher::start() {
    if (running_) return;
    running_ = true;
    thread_ = std::thread(&WorkerDispatcher::loop, this);
}

void WorkerDispatcher::stop() {
    running_ = false;
}

void WorkerDispatcher::join() {
    if (thread_.joinable()) thread_.join();
}

void WorkerDispatcher::loop() {
    while (running_) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        if (handler_) handler_(0);
    }
}

} // namespace device_reminder
