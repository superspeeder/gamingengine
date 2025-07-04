#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <optional>
#include <string>

#include <vulkan/vulkan_raii.hpp>

#if GLFW_VERSION_MAJOR != 3 || GLFW_VERSION_MINOR != 4
#error "Invalid GLFW header version (requires 3.4)"
#endif


namespace engine {
    void os_init();
    void os_terminate();
    void os_poll();

    class window {
      public:
        enum class mode {
            windowed,
            exclusive_fullscreen,
            borderless_windowed_fullscreen,
        };

        struct attributes {
            std::string               title;
            glm::uvec2                windowed_size;
            std::optional<glm::uvec2> exclusive_fullscreen_resolution_override = std::nullopt;
            ::engine::window::mode    mode                                     = ::engine::window::mode::windowed;
            int                       target_monitor                           = 0;
            bool                      resizable                                = false;
            bool                      visible_on_open                          = true;
            bool                      decorated                                = true;
            bool                      focused_on_open                          = true;
            bool                      auto_iconify                             = true;
            bool                      always_on_top                            = false;
            bool                      maximized_on_open                        = false;
            bool                      center_cursor_on_open                    = true;
            bool                      transparent_framebuffer                  = false;

            // not the same as focused_on_open (this applies to every time the window is shown)
            bool                      focus_on_show     = true;
            bool                      scale_to_monitor  = false;
            bool                      scale_framebuffer = true;
            bool                      mouse_passthrough = false;
            std::optional<glm::ivec2> windowed_position = std::nullopt;
        };

        explicit window(const attributes &attributes);
        ~window();

        window(const window &other)                = delete;
        window(window &&other) noexcept            = delete;
        window &operator=(const window &other)     = delete;
        window &operator=(window &&other) noexcept = delete;

        [[nodiscard]] bool should_close() const;

        vk::Extent2D get_extent() const;

        glm::ivec2 get_position() const;

        vk::raii::SurfaceKHR create_surface(const vk::raii::Instance& instance) const;

      private:
        bool                      m_windowed_is_decorated;
        mode                      m_mode;
        glm::uvec2                m_windowed_size;
        std::optional<glm::uvec2> m_exclusive_fullscreen_resolution;
        glm::ivec2                m_windowed_position;
        GLFWmonitor              *m_fullscreen_monitor;

        GLFWwindow *m_window;
    };

} // namespace engine
