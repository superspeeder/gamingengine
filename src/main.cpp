#include "engine/os.hpp"


#include <GLFW/glfw3.h>
#include <cstdlib>

#if GLFW_VERSION_MAJOR != 3 || GLFW_VERSION_MINOR != 4
#error "Invalid GLFW header version (requires 3.4)"
#endif


int main() {
    engine::os_init();
    {
        const engine::window window(engine::window::attributes{.title = "Hello!", .windowed_size = {800, 600}});

        while (!window.should_close()) {
            engine::os_poll();
        }
    }

    engine::os_terminate();
    return EXIT_SUCCESS;
}
