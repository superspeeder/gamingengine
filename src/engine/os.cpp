//
// Created by andy on 7/3/25.
//

#include "os.hpp"
#include "logging.hpp"

#include <stdexcept>

#include <spdlog/spdlog.h>

namespace engine {
    static spdlog::logger *glfw_error_logger = nullptr;

    static bool check_glfw_version() {
        int major, minor;

        [[maybe_unused]] int rev;
        glfwGetVersion(&major, &minor, &rev);
        return major == 3 && minor == 4;
    }

    void os_init() {
        if (!glfwInit()) {
            throw std::runtime_error("Failed to initialize GLFW");
        }

        if (!check_glfw_version()) {
            glfwTerminate();
            throw std::runtime_error("Incorrect GLFW version (requires GLFW 3.4).");
        }

        glfw_error_logger = create_logger("glfw").get();

        glfwSetErrorCallback(+[](int code, const char *description) {
            if (glfw_error_logger)
                glfw_error_logger->error("({}) {}", code, description);
        });
    }

    void os_terminate() {
        spdlog::drop(glfw_error_logger->name());
        glfw_error_logger = nullptr;
        glfwTerminate();
    }

    void os_poll() {
        glfwPollEvents();
    }

    window::window(const attributes &attributes)
        : m_windowed_is_decorated(attributes.decorated), m_mode(attributes.mode),
          m_windowed_size(attributes.windowed_size),
          m_exclusive_fullscreen_resolution(attributes.exclusive_fullscreen_resolution_override),
          m_windowed_position(attributes.windowed_position.value_or(glm::ivec2(GLFW_ANY_POSITION, GLFW_ANY_POSITION))) {
        glfwDefaultWindowHints();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, attributes.resizable);
        glfwWindowHint(GLFW_VISIBLE, attributes.visible_on_open);

        int           monitor_count;
        GLFWmonitor **monitors = glfwGetMonitors(&monitor_count);

        if (attributes.target_monitor == -1 || attributes.target_monitor >= monitor_count) {
            m_fullscreen_monitor = monitors[monitor_count - 1];
        } else {
            m_fullscreen_monitor = monitors[attributes.target_monitor];
        }

        const GLFWvidmode *video_mode = glfwGetVideoMode(m_fullscreen_monitor);

        int width, height;

        int mx, my;
        glfwGetMonitorPos(m_fullscreen_monitor, &mx, &my);
        GLFWmonitor *fsm = nullptr;

        switch (attributes.mode) {
        case mode::exclusive_fullscreen:
            glfwWindowHint(GLFW_REFRESH_RATE, video_mode->refreshRate);
            {
                const auto res =
                    m_exclusive_fullscreen_resolution.value_or(glm::uvec2(video_mode->width, video_mode->height));
                width  = res.x;
                height = res.y;
                fsm    = m_fullscreen_monitor;
            }
            break;

        case mode::borderless_windowed_fullscreen:
            glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
            glfwWindowHint(GLFW_POSITION_X, mx);
            glfwWindowHint(GLFW_POSITION_Y, my);
            width  = video_mode->width;
            height = video_mode->height;
            break;

        case mode::windowed:
        default:
            glfwWindowHint(GLFW_DECORATED, m_windowed_is_decorated);
            glfwWindowHint(GLFW_POSITION_X, m_windowed_position.x);
            glfwWindowHint(GLFW_POSITION_Y, m_windowed_position.y);
            width  = attributes.windowed_size.x;
            height = attributes.windowed_size.y;
            break;
        }

        m_window = glfwCreateWindow(width, height, attributes.title.c_str(), fsm, nullptr);
        if (!m_window) {
            throw std::runtime_error("Failed to create GLFW window");
        }

        glfwSetWindowUserPointer(m_window, this);
    }

    window::~window() {
        glfwDestroyWindow(m_window);
    }

    bool window::should_close() const {
        return glfwWindowShouldClose(m_window);
    }


} // namespace engine
