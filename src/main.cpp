#include "chip8.hpp"
#include "graphics.hpp"

int main(int argc, char **argv) {
    const unsigned int VIDEO_WIDTH = 64;
    const unsigned int VIDEO_HEIGHT = 32;
    int pitch;
    bool quit = false;

    if (argc != 2) {
        std::cout << "Insufficient arguments. Usage: " << argv[0] << " <path to rom>\n";
        std::exit(0);
    }

    const char* filename = argv[1];
    Chip8 chip8;
    Graphics* graphics = new Graphics("CHIP-8 Emulator by Jonathan Sohrabi", VIDEO_WIDTH*4, VIDEO_HEIGHT*4, VIDEO_WIDTH, VIDEO_HEIGHT);

    chip8.loadROM(filename);

    pitch = sizeof(chip8.display[0]) * VIDEO_WIDTH;

    auto lastCycleTime = std::chrono::high_resolution_clock::now();

    while (!quit) {
        quit = graphics->input(chip8.keyPressedState);
        auto currentTime = std::chrono::high_resolution_clock::now();
        float timeElapsedInCycle = std::chrono::duration<float, std::chrono::milliseconds::period>(currentTime - lastCycleTime).count();
        
        if (timeElapsedInCycle > 10) {
            lastCycleTime = currentTime;
            chip8.cycle();
            graphics->updateScreen(chip8.display, pitch);
        }
    }

    delete graphics;

    return 0;
}