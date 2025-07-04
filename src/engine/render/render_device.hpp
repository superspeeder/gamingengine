//
// Created by andy on 7/4/25.
//

#pragma once

#include "engine/os.hpp"

#include <spdlog/spdlog.h>
#include <vulkan/vulkan_raii.hpp>

namespace engine {

    class render_device {
      public:
        explicit render_device(const std::shared_ptr<window> &window);
        ~render_device();

        const std::shared_ptr<spdlog::logger> &logger() const;

      private:
        std::shared_ptr<window>         m_window;
        std::shared_ptr<spdlog::logger> m_logger;

        vk::raii::Context        m_context;
        vk::raii::Instance       m_instance;
        vk::raii::SurfaceKHR     m_surface;
        vk::raii::PhysicalDevice m_physical_device;

        void _create_instance();
        void _select_physical_device();
    };

} // namespace engine
