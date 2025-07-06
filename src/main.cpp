#include "engine/os.hpp"
#include "engine/render/render_device.hpp"
#include "engine/scene/scene.hpp"


#include <GLFW/glfw3.h>
#include <cstdlib>
#include <iostream>

class test_object : public engine::scene::scene_object {
  public:
    test_object(const std::weak_ptr<engine::scene::scene> &scene, const uint64_t id) : scene_object(scene, id) {}

    void update(const double delta) override { spdlog::info("update {}", 1.0 / delta); }
};

int main() {
    engine::os_init();
    {
        const auto window = std::make_shared<engine::window>(
            engine::window::attributes{.title = "Hello!", .windowed_size = {800, 600}}
        );
        const auto render_device = std::make_shared<engine::render_device>(window);

        const auto scene        = std::make_shared<engine::scene::scene>();
        const auto update_group = scene->push_front_new_update_group();

        const auto [_, _2] = scene->emplace_object_named_ug<test_object>("test_object", update_group);

        double this_frame = glfwGetTime();
        double delta_time = 1.0 / 60.0;
        double last_frame = this_frame - delta_time;

        while (!window->should_close()) {
            engine::os_poll();
            scene->update(delta_time);

            last_frame = this_frame;
            this_frame = glfwGetTime();
            delta_time = this_frame - last_frame;
        }
    }

    engine::os_terminate();
    return EXIT_SUCCESS;
}
