#include <SDL.h>

class Display {
    private:
        SDL_Window* window{};       // The window to render to
        SDL_Renderer* renderer{};   // The object that will be rendering
        SDL_Texture* texture{};     // The texture to draw to the window

    public:
        // Constructor
        Display(const char* title, int windowWidth, int windowHeight, int textureWidth, int textureHeight);

        // Destructor
        ~Display();

        // Updates window at every game tick
        void updateScreen(const void* buffer, int pitch);

        // Handles keyboard input
        bool input(bool* keyPressedState);
};