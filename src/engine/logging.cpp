//
// Created by andy on 7/3/25.
//
#include "logging.hpp"

#include <utility>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace engine {
    static std::shared_ptr<logger_factory> s_logger_factory = std::make_shared<default_logger_factory>();

    void set_logger_factory(std::shared_ptr<logger_factory> factory) {
        s_logger_factory = std::move(factory);
    }

    const std::shared_ptr<logger_factory> &get_logger_factory() {
        return s_logger_factory;
    }

    std::shared_ptr<spdlog::logger> create_logger(const std::string &name) {
        return create_logger(name, logger_settings{.ephemeral_only = false});
    }

    std::shared_ptr<spdlog::logger> create_logger(const std::string &name, const logger_settings &settings) {
        return s_logger_factory->create_logger(name, settings);
    }

    std::shared_ptr<spdlog::logger> default_logger_factory::create_logger(const std::string     &name,
                                                                          const logger_settings &settings) {
        auto logger = spdlog::stdout_color_mt(name);
#ifndef NDEBUG
        logger->set_level(spdlog::level::debug);
#else
        logger->set_level(spdlog::level::info);
#endif
        return logger;
    }
} // namespace engine
