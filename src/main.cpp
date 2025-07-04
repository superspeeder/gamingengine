#include "engine/os.hpp"
#include "engine/render/render_device.hpp"


#include <GLFW/glfw3.h>
#include <cstdlib>

int main() {
    engine::os_init();
    {
        const auto window = std::make_shared<engine::window>(
            engine::window::attributes{.title = "Hello!", .windowed_size = {800, 600}}
        );
        const auto render_device = std::make_shared<engine::render_device>(window);

        while (!window->should_close()) {
            engine::os_poll();
        }
    }

    engine::os_terminate();
    return EXIT_SUCCESS;
}
