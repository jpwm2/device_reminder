#include "infra/process/process.hpp"
#include "infra/message/message_inbox.hpp"

#include <cerrno>
#include <stdexcept>
#include <string>
#include <utility>
#include <sys/resource.h>

namespace device_reminder {
namespace {
constexpr const char* kConfigFile = "process.conf";
constexpr const char* kPriorityKey = "priority";
} // namespace

Process::Process(std::shared_ptr<IMessageReceiver> receiver,
                 std::shared_ptr<IFileLoader> file_loader,
                 std::shared_ptr<ILogger> logger)
    : receiver_(std::move(receiver))
    , file_loader_(std::move(file_loader))
    , logger_(std::move(logger))
{}

void Process::run() {
    if (logger_) {
        logger_->info("Process run start");
    }

    if (!receiver_ || !file_loader_ || !logger_) {
        if (logger_) {
            logger_->error("Process run failed: invalid dependency");
        }
        throw std::runtime_error("invalid dependency");
    }

    try {
        int priority = 0;
        try {
            priority = file_loader_->load_int(kConfigFile, kPriorityKey);
        } catch (const std::exception& e) {
            if (logger_) {
                logger_->warn(std::string("Process priority load failed: ") + e.what());
            }
        }
        if (priority != 0) {
            if (setpriority(PRIO_PROCESS, 0, priority) != 0) {
                throw std::runtime_error("failed to set priority");
            }
        }

        inbox_ = std::make_shared<MessageInbox>(logger_, nullptr, nullptr, nullptr);
        // Connection between receiver_ and inbox_ is implementation-specific

        receiver_->run();
        running_.store(true);
        if (logger_) {
            logger_->info("Process run succeeded");
        }
    } catch (...) {
        running_.store(false);
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
        if (receiver_) {
            receiver_->stop();
        }
        if (inbox_) {
            inbox_.reset();
        }
        running_.store(false);
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

