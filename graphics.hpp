#include <SDL.h>

class Graphics {
    private:
        SDL_Window* window{};       // The window to render to
        SDL_Renderer* renderer{};   // The object that will be rendering
        SDL_Texture* texture{};     // The texture to draw to the window

    public:
        // Constructor
        Graphics(const char* title, int windowWidth, int windowHeight, int textureWidth, int textureHeight);

        // Destructor
        ~Graphics();

        // Updates window at every game tick
        void updateScreen(const void* buffer, int pitch);

        // Handles keyboard input
        bool input(bool* keyPressedState);
};