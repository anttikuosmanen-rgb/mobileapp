#ifndef GUIMANAGER_H
#define GUIMANAGER_H

#include <string>
#include <SDL2/SDL.h>

struct Button {
    SDL_Rect rect;
    std::string label;
    SDL_Color color;
};

class GuiManager {
public:
    GuiManager(SDL_Renderer* renderer, const std::string& buildTool);
    ~GuiManager();

    bool loadResources();
    void render();
    void handleClick(int x, int y);

    bool shouldExit() const { return exitRequested; }
    bool isShowingAbout() const { return showAbout; }

private:
    SDL_Renderer* renderer;
    SDL_Texture* backgroundTexture;
    SDL_Surface* backgroundSurface;

    Button exitButton;
    Button infoButton;

    bool exitRequested;
    bool showAbout;
    std::string buildTool;

    void renderButton(const Button& button);
    void renderAboutDialog();
    bool createDefaultBackground();
    void createDefaultButtons();
};

#endif // GUIMANAGER_H
