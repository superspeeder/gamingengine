//
// Created by andy on 7/4/25.
//

#include "render_device.hpp"

#include "engine/logging.hpp"

namespace engine {

    render_device::render_device(const std::shared_ptr<window> &window)
        : m_window(window), m_instance(nullptr), m_surface(nullptr), m_physical_device(nullptr) {

        m_logger = create_logger("render");

        _create_instance();
        m_surface = m_window->create_surface(m_instance);
        _select_physical_device();

        {
            const auto &props = m_physical_device.getProperties();
            m_logger->info("Selected Physical Device: {}", props.deviceName.data());
        }
    }

    render_device::~render_device() {}

    const std::shared_ptr<spdlog::logger> &render_device::logger() const {
        return m_logger;
    }

    void render_device::_create_instance() {
        constexpr vk::ApplicationInfo app_info = vk::ApplicationInfo().setApiVersion(vk::ApiVersion14);

        uint32_t     count;
        const char **required_extensions = glfwGetRequiredInstanceExtensions(&count);
        std::vector  extensions(required_extensions, required_extensions + count);

        vk::InstanceCreateInfo instance_create_info{};
        instance_create_info.setPApplicationInfo(&app_info);
        instance_create_info.setPEnabledExtensionNames(extensions);
        m_instance = vk::raii::Instance(m_context, instance_create_info);
    }

    void render_device::_select_physical_device() {
        m_physical_device = m_instance.enumeratePhysicalDevices()[0];
    }
} // namespace engine
