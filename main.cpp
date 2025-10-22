#include <SDL2/SDL.h>
#include <iostream>
#include <vector>
#include <cstdint>
#include <cstring>

const int WINDOW_WIDTH = 320;
const int WINDOW_HEIGHT = 320;
const int BOARD_SIZE = 8;

class ChessboardRenderer {
private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;
    std::vector<uint32_t> framebuffer;
    
public:
    ChessboardRenderer() : window(nullptr), renderer(nullptr), texture(nullptr) {
        framebuffer.resize(WINDOW_WIDTH * WINDOW_HEIGHT);
    }
    
    bool initialize() {
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
            return false;
        }
        
        window = SDL_CreateWindow("SDL2 Chessboard - Framebuffer",
                                 SDL_WINDOWPOS_CENTERED,
                                 SDL_WINDOWPOS_CENTERED,
                                 WINDOW_WIDTH, WINDOW_HEIGHT,
                                 SDL_WINDOW_SHOWN);
        
        if (!window) {
            std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
            return false;
        }
        
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        if (!renderer) {
            std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
            return false;
        }
        
        texture = SDL_CreateTexture(renderer,
                                   SDL_PIXELFORMAT_ARGB8888,
                                   SDL_TEXTUREACCESS_STREAMING,
                                   WINDOW_WIDTH, WINDOW_HEIGHT);
        
        if (!texture) {
            std::cerr << "Texture could not be created! SDL_Error: " << SDL_GetError() << std::endl;
            return false;
        }
        
        return true;
    }
    
    void renderChessboard() {
        // Clear framebuffer to black
        std::fill(framebuffer.begin(), framebuffer.end(), 0xFF000000);
        
        int cellWidth = WINDOW_WIDTH / BOARD_SIZE;
        int cellHeight = WINDOW_HEIGHT / BOARD_SIZE;
        
        // Render chessboard pattern
        for (int row = 0; row < BOARD_SIZE; ++row) {
            for (int col = 0; col < BOARD_SIZE; ++col) {
                uint32_t color;
                
                // Alternate colors for chessboard pattern
                if ((row + col) % 2 == 0) {
                    color = 0xFFFFFFFF; // White
                } else {
                    color = 0xFF404040; // Dark gray
                }
                
                // Fill the cell in framebuffer
                for (int y = row * cellHeight; y < (row + 1) * cellHeight; ++y) {
                    for (int x = col * cellWidth; x < (col + 1) * cellWidth; ++x) {
                        if (x < WINDOW_WIDTH && y < WINDOW_HEIGHT) {
                            framebuffer[y * WINDOW_WIDTH + x] = color;
                        }
                    }
                }
            }
        }
        
        // Update texture with framebuffer data
        void* pixels;
        int pitch;
        SDL_LockTexture(texture, nullptr, &pixels, &pitch);
        std::memcpy(pixels, framebuffer.data(), framebuffer.size() * sizeof(uint32_t));
        SDL_UnlockTexture(texture);
        
        // Render texture to screen
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, nullptr, nullptr);
        SDL_RenderPresent(renderer);
    }
    
    void run() {
        bool running = true;
        SDL_Event event;
        
        while (running) {
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT) {
                    running = false;
                } else if (event.type == SDL_KEYDOWN) {
                    if (event.key.keysym.sym == SDLK_ESCAPE) {
                        running = false;
                    }
                }
            }
            
            renderChessboard();
            SDL_Delay(16); // ~60 FPS
        }
    }
    
    ~ChessboardRenderer() {
        if (texture) SDL_DestroyTexture(texture);
        if (renderer) SDL_DestroyRenderer(renderer);
        if (window) SDL_DestroyWindow(window);
        SDL_Quit();
    }
};

int main(int argc, char* argv[]) {
    ChessboardRenderer chessboard;
    
    if (!chessboard.initialize()) {
        return -1;
    }
    
    chessboard.run();
    
    return 0;
}
