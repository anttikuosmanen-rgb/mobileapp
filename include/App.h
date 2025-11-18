#ifndef APP_H
#define APP_H

#include <string>
#include <SDL2/SDL.h>
#include "GuiManager.h"

class App {
public:
    App(const std::string& buildTool);
    ~App();

    bool initialize();
    void run();
    void cleanup();

private:
    bool isRunning;
    std::string buildTool;
    SDL_Window* window;
    SDL_Renderer* renderer;
    GuiManager* guiManager;

    void handleEvents();
    void update();
    void render();
};

#endif // APP_H
