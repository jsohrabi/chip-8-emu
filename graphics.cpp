#include "graphics.hpp"
#include <iostream>

Graphics::Graphics(const char* title, int windowWidth, int windowHeight, int textureWidth, int textureHeight) {
    SDL_Init(SDL_INIT_VIDEO);

    window = SDL_CreateWindow(title, 100, 100, windowWidth, windowHeight, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, textureWidth, textureHeight);
}

Graphics::~Graphics() {
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Graphics::updateScreen(const void* buffer, int pitch) {
    SDL_UpdateTexture(texture, nullptr, buffer, pitch);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
}

bool Graphics::input(bool* keyPressedState) {
    bool quit = false;
    SDL_Event e;

    while (SDL_PollEvent(&e)) {
        switch (e.type) {
            case SDL_QUIT:
                quit = true;
                break;
            
            case SDL_KEYDOWN:
                switch (e.key.keysym.sym) {
                    case SDLK_ESCAPE:
                        quit = true;
                        break;

                    case SDLK_x:
                        keyPressedState[0x0] = 1;
                        break;

                    case SDLK_1:
                        keyPressedState[0x1] = 1;
                        break;

                    case SDLK_2:
                        keyPressedState[0x2] = 1;
                        break;

                    case SDLK_3:
                        keyPressedState[0x3] = 1;
                        break;

                    case SDLK_q:
                        keyPressedState[0x4] = 1;
                        break;

                    case SDLK_w:
                        keyPressedState[0x5] = 1;
                        break;

                    case SDLK_e:
                        keyPressedState[0x6] = 1;
                        break;

                    case SDLK_a:
                        keyPressedState[0x7] = 1;
                        break;

                    case SDLK_s:
                        keyPressedState[0x8] = 1;
                        break;

                    case SDLK_d:
                        keyPressedState[0x9] = 1;
                        break;

                    case SDLK_z:
                        keyPressedState[0xA] = 1;
                        break;

                    case SDLK_c:
                        keyPressedState[0xB] = 1;
                        break;

                    case SDLK_4:
                        keyPressedState[0xC] = 1;
                        break;

                    case SDLK_r:
                        keyPressedState[0xD] = 1;
                        break;

                    case SDLK_f:
                        keyPressedState[0xE] = 1;
                        break;

                    case SDLK_v:
                        keyPressedState[0xF] = 1;
                        break;
                }
                break;

            case SDL_KEYUP:
                switch(e.key.keysym.sym) {
                    case SDLK_x:
                        keyPressedState[0x0] = 0;
                        break;

                    case SDLK_1:
                        keyPressedState[0x1] = 0;
                        break;

                    case SDLK_2:
                        keyPressedState[0x2] = 0;
                        break;

                    case SDLK_3:
                        keyPressedState[0x3] = 0;
                        break;

                    case SDLK_q:
                        keyPressedState[0x4] = 0;
                        break;

                    case SDLK_w:
                        keyPressedState[0x5] = 0;
                        break;

                    case SDLK_e:
                        keyPressedState[0x6] = 0;
                        break;

                    case SDLK_a:
                        keyPressedState[0x7] = 0;
                        break;

                    case SDLK_s:
                        keyPressedState[0x8] = 0;
                        break;

                    case SDLK_d:
                        keyPressedState[0x9] = 0;
                        break;

                    case SDLK_z:
                        keyPressedState[0xA] = 0;
                        break;

                    case SDLK_c:
                        keyPressedState[0xB] = 0;
                        break;

                    case SDLK_4:
                        keyPressedState[0xC] = 0;
                        break;

                    case SDLK_r:
                        keyPressedState[0xD] = 0;
                        break;

                    case SDLK_f:
                        keyPressedState[0xE] = 0;
                        break;

                    case SDLK_v:
                        keyPressedState[0xF] = 0;
                        break;
                }
                break;
        }
    }

    return quit;
}
