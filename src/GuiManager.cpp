#include "GuiManager.h"
#include <iostream>
#include <SDL2/SDL_image.h>

GuiManager::GuiManager(SDL_Renderer* renderer, const std::string& buildTool)
    : renderer(renderer), backgroundTexture(nullptr), backgroundSurface(nullptr),
      exitRequested(false), showAbout(false), buildTool(buildTool) {
    createDefaultButtons();
}

GuiManager::~GuiManager() {
    if (backgroundTexture) {
        SDL_DestroyTexture(backgroundTexture);
    }
    if (backgroundSurface) {
        SDL_FreeSurface(backgroundSurface);
    }
}

void GuiManager::createDefaultButtons() {
    // Exit button (X) in top-right corner
    exitButton.rect = {750, 10, 40, 40};
    exitButton.label = "X";
    exitButton.color = {255, 0, 0, 255};

    // Info button (i) in top-right corner, next to exit
    infoButton.rect = {700, 10, 40, 40};
    infoButton.label = "i";
    infoButton.color = {0, 120, 255, 255};
}

bool GuiManager::createDefaultBackground() {
    // Create a simple gradient background as default
    const int width = 800;
    const int height = 600;

    backgroundSurface = SDL_CreateRGBSurface(0, width, height, 32,
                                             0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
    if (!backgroundSurface) {
        std::cerr << "Failed to create background surface: " << SDL_GetError() << std::endl;
        return false;
    }

    // Fill with a gradient (top: dark blue, bottom: light blue)
    SDL_LockSurface(backgroundSurface);
    Uint32* pixels = (Uint32*)backgroundSurface->pixels;
    for (int y = 0; y < height; y++) {
        Uint8 blue = 50 + (y * 150 / height);
        Uint8 green = 30 + (y * 100 / height);
        for (int x = 0; x < width; x++) {
            pixels[y * width + x] = SDL_MapRGB(backgroundSurface->format, 20, green, blue);
        }
    }
    SDL_UnlockSurface(backgroundSurface);

    backgroundTexture = SDL_CreateTextureFromSurface(renderer, backgroundSurface);
    if (!backgroundTexture) {
        std::cerr << "Failed to create background texture: " << SDL_GetError() << std::endl;
        return false;
    }

    return true;
}

bool GuiManager::loadResources() {
    // Try to load background image from resources
    std::string backgroundPath = "resources/background.png";
    SDL_Surface* loadedSurface = IMG_Load(backgroundPath.c_str());

    if (loadedSurface) {
        backgroundSurface = loadedSurface;
        backgroundTexture = SDL_CreateTextureFromSurface(renderer, backgroundSurface);
        if (!backgroundTexture) {
            std::cerr << "Failed to create texture from " << backgroundPath << ": " << SDL_GetError() << std::endl;
            SDL_FreeSurface(loadedSurface);
            return createDefaultBackground();
        }
        std::cout << "Loaded background image: " << backgroundPath << std::endl;
        return true;
    } else {
        std::cout << "Background image not found, creating default background" << std::endl;
        return createDefaultBackground();
    }
}

void GuiManager::render() {
    // Render background
    if (backgroundTexture) {
        SDL_RenderCopy(renderer, backgroundTexture, nullptr, nullptr);
    }

    // Render buttons
    renderButton(exitButton);
    renderButton(infoButton);

    // Render about dialog if showing
    if (showAbout) {
        renderAboutDialog();
    }
}

void GuiManager::renderButton(const Button& button) {
    // Draw button background
    SDL_SetRenderDrawColor(renderer, button.color.r, button.color.g, button.color.b, button.color.a);
    SDL_RenderFillRect(renderer, &button.rect);

    // Draw button border
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawRect(renderer, &button.rect);

    // Note: For simplicity, we're not rendering text here. In a full implementation,
    // you would use SDL_ttf to render the button label text.
}

void GuiManager::renderAboutDialog() {
    // Draw semi-transparent overlay
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 180);
    SDL_Rect overlay = {0, 0, 800, 600};
    SDL_RenderFillRect(renderer, &overlay);

    // Draw dialog box
    SDL_SetRenderDrawColor(renderer, 240, 240, 240, 255);
    SDL_Rect dialog = {200, 200, 400, 200};
    SDL_RenderFillRect(renderer, &dialog);

    // Draw dialog border
    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
    SDL_RenderDrawRect(renderer, &dialog);

    // Note: In a full implementation, you would render text here showing:
    // "Multi-Platform GUI App v1.0"
    // "Built with: [CMake/Ant/Gradle]"
    // For now, the dialog box is displayed. Text rendering would require SDL_ttf.

    std::cout << "About Dialog: Multi-Platform GUI App v1.0 - Built with " << buildTool << std::endl;
}

void GuiManager::handleClick(int x, int y) {
    SDL_Point point = {x, y};

    // Check exit button
    if (SDL_PointInRect(&point, &exitButton.rect)) {
        std::cout << "Exit button clicked" << std::endl;
        exitRequested = true;
        return;
    }

    // Check info button
    if (SDL_PointInRect(&point, &infoButton.rect)) {
        std::cout << "Info button clicked" << std::endl;
        showAbout = !showAbout;
        return;
    }

    // Click anywhere else closes about dialog
    if (showAbout) {
        showAbout = false;
    }
}
