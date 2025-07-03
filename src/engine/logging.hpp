//
// Created by andy on 7/3/25.
//

#pragma once

#include <spdlog/spdlog.h>

namespace engine {

    struct logger_settings {
        bool ephemeral_only; // set to hint that the log should not be written to a file.
    };

    class logger_factory {
      public:
        virtual ~logger_factory()                                                              = default;
        virtual std::shared_ptr<spdlog::logger> create_logger(const std::string     &name,
                                                              const logger_settings &settings) = 0;
    };

    void set_logger_factory(std::shared_ptr<logger_factory> factory);

    const std::shared_ptr<logger_factory> &get_logger_factory();

    std::shared_ptr<spdlog::logger> create_logger(const std::string &name);
    std::shared_ptr<spdlog::logger> create_logger(const std::string &name, const logger_settings &settings);

    class default_logger_factory final : public logger_factory {
      public:
        std::shared_ptr<spdlog::logger> create_logger(const std::string     &name,
                                                      const logger_settings &settings) override;
    };
} // namespace engine
