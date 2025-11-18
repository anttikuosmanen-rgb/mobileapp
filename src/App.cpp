#include "App.h"
#include <iostream>
#include <SDL2/SDL_image.h>

App::App(const std::string& buildTool)
    : isRunning(false), buildTool(buildTool), window(nullptr),
      renderer(nullptr), guiManager(nullptr) {
}

App::~App() {
    cleanup();
}

bool App::initialize() {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
        return false;
    }

    // Initialize SDL_image for PNG loading
    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        std::cerr << "SDL_image initialization failed: " << IMG_GetError() << std::endl;
        std::cout << "Continuing without image loading support..." << std::endl;
    }

    // Create window
    window = SDL_CreateWindow("Multi-Platform GUI App",
                              SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED,
                              800, 600,
                              SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "Window creation failed: " << SDL_GetError() << std::endl;
        return false;
    }

    // Create renderer
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        std::cerr << "Renderer creation failed: " << SDL_GetError() << std::endl;
        return false;
    }

    // Create GUI manager
    guiManager = new GuiManager(renderer, buildTool);
    if (!guiManager->loadResources()) {
        std::cerr << "Failed to load GUI resources" << std::endl;
        return false;
    }

    std::cout << "Application initialized successfully" << std::endl;
    std::cout << "Built with: " << buildTool << std::endl;

    isRunning = true;
    return true;
}

void App::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                isRunning = false;
                break;

            case SDL_MOUSEBUTTONDOWN:
                if (event.button.button == SDL_BUTTON_LEFT) {
                    guiManager->handleClick(event.button.x, event.button.y);
                }
                break;

            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    isRunning = false;
                }
                break;
        }
    }

    // Check if exit was requested through GUI
    if (guiManager->shouldExit()) {
        isRunning = false;
    }
}

void App::update() {
    // Update logic here (if needed)
}

void App::render() {
    // Clear screen
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Render GUI
    guiManager->render();

    // Present
    SDL_RenderPresent(renderer);
}

void App::run() {
    while (isRunning) {
        handleEvents();
        update();
        render();

        // Small delay to prevent CPU overuse
        SDL_Delay(16); // ~60 FPS
    }
}

void App::cleanup() {
    if (guiManager) {
        delete guiManager;
        guiManager = nullptr;
    }

    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }

    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }

    IMG_Quit();
    SDL_Quit();

    std::cout << "Application cleaned up" << std::endl;
}
