#include "App.h"
#include <iostream>

// Build tool detection through preprocessor defines
// These will be defined by each build system
#ifndef BUILD_TOOL
    #define BUILD_TOOL "Unknown"
#endif

int main(int argc, char* argv[]) {
    std::cout << "=== Multi-Platform GUI Application ===" << std::endl;
    std::cout << "Build Tool: " << BUILD_TOOL << std::endl;
    std::cout << "=======================================" << std::endl;

    App app(BUILD_TOOL);

    if (!app.initialize()) {
        std::cerr << "Failed to initialize application" << std::endl;
        return 1;
    }

    app.run();
    app.cleanup();

    std::cout << "Application exited successfully" << std::endl;

#ifdef PLATFORM_IOS
    exit(0);
#endif
    return 0;
}
