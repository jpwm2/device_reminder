#include "infra/process/process.hpp"

#include <chrono>
#include <stdexcept>
#include <string>
#include <thread>
#include <unistd.h>

namespace device_reminder {

Process::Process(std::shared_ptr<IMessageReceiver> receiver,
                 std::shared_ptr<ILogger> logger,
                 std::shared_ptr<MessageInbox> inbox_dep,
                 MessageInbox inbox)
    : receiver_(std::move(receiver))
    , logger_(std::move(logger))
    , inbox_dep_(std::move(inbox_dep))
    , inbox_(std::move(inbox)) {}

pid_t Process::spawn() {
    pid_t pid = fork();
    if (pid < 0) {
        if (logger_) {
            logger_->error("Process spawn failed: fork error");
        }
        throw std::runtime_error("fork failed");
    }
    if (pid == 0) {
        try {
            run();
            _exit(0);
        } catch (...) {
            _exit(1);
        }
    }
    return pid;
}

void Process::run() {
    if (logger_) {
        logger_->info("Process run start");
    }

    if (!receiver_ || !logger_ || !inbox_dep_) {
        if (logger_) {
            logger_->error("Process run failed: invalid dependency");
        }
        throw std::runtime_error("invalid dependency");
    }

    try {
        running_.store(true);
        receiver_->run();
        while (running_.load()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        if (logger_) {
            logger_->info("Process run succeeded");
        }
    } catch (...) {
        if (logger_) {
            try {
                throw;
            } catch (const std::exception& e) {
                logger_->error(std::string("Process run failed: ") + e.what());
            } catch (...) {
                logger_->error("Process run failed: unknown error");
            }
        }
        throw;
    }
}

void Process::stop() {
    if (logger_) {
        logger_->info("Process stop start");
    }

    try {
        running_.store(false);
        if (receiver_) {
            receiver_->stop();
        }
        if (logger_) {
            logger_->info("Process stop succeeded");
        }
    } catch (...) {
        if (logger_) {
            try {
                throw;
            } catch (const std::exception& e) {
                logger_->error(std::string("Process stop failed: ") + e.what());
            } catch (...) {
                logger_->error("Process stop failed: unknown error");
            }
        }
        throw;
    }
}

} // namespace device_reminder

