#include "infra/message/process_sender.hpp"

#include "infra/logger.hpp"
#include "infra/message/message_codec.hpp"

#include <algorithm>
#include <cctype>
#include <stdexcept>
#include <utility>

namespace device_reminder {

IProcessSender::IProcessSender(std::shared_ptr<ILogger> logger,
                               std::shared_ptr<IMessageCodec> codec)
    : logger_(std::move(logger)), codec_(std::move(codec)) {}

ProcessSender::ProcessSender(std::shared_ptr<ILogger> logger,
                             std::shared_ptr<IMessageCodec> codec)
    : IProcessSender(std::move(logger), std::move(codec)) {}

void ProcessSender::send(const std::string& endpoint,
                         std::shared_ptr<IMessage> message) {
    try {
        if (endpoint.empty() ||
            std::all_of(endpoint.begin(), endpoint.end(), [](unsigned char c) {
                return std::isspace(c) != 0;
            })) {
            if (logger_) logger_->error("send: endpoint is invalid");
            throw std::invalid_argument("endpoint is invalid");
        }
        if (!message) {
            if (logger_) logger_->error("send: message is null");
            throw std::invalid_argument("message is null");
        }

        if (logger_) {
            logger_->info("send: start endpoint=" + endpoint);
        }

        if (!codec_) {
            if (logger_) logger_->error("send: codec is null");
            throw std::runtime_error("codec is null");
        }

        auto data = codec_->encode(message);
        if (data.empty()) {
            if (logger_) logger_->error("send: encoded data is empty");
            throw std::runtime_error("encoded data is empty");
        }

        if (logger_) {
            logger_->info("send: succeeded endpoint=" + endpoint +
                          ", size=" + std::to_string(data.size()));
        }
    } catch (const std::exception& e) {
        if (logger_) {
            logger_->error("send failed: " + std::string(e.what()));
        }
        throw;
    } catch (...) {
        if (logger_) {
            logger_->error("send failed: unknown exception");
        }
        throw;
    }
}

} // namespace device_reminder

