#include <iostream>
#include <vector>
#include <cstdint>
#include <cstring>
#include <string>

#ifdef HAS_SDL
#include <SDL2/SDL.h>
#endif

const int BOARD_SIZE = 8;

class ChessboardRenderer {
private:
    std::vector<std::vector<char>> board;
    
public:
    ChessboardRenderer() {
        board.resize(BOARD_SIZE, std::vector<char>(BOARD_SIZE, ' '));
    }
    
    void renderChessboard() {
        // Render chessboard pattern
        for (int row = 0; row < BOARD_SIZE; ++row) {
            for (int col = 0; col < BOARD_SIZE; ++col) {
                // Alternate colors for chessboard pattern
                if ((row + col) % 2 == 0) {
                    board[row][col] = '#';  // White square
                } else {
                    board[row][col] = '.';  // Black square
                }
            }
        }
        
        // Add pieces (simplified representation)
        // Pawns
        for (int col = 0; col < BOARD_SIZE; ++col) {
            board[1][col] = 'P';  // Black pawns
            board[6][col] = 'p';  // White pawns
        }
        
        // Major pieces
        board[0][0] = board[0][7] = 'R';  // Black rooks
        board[0][1] = board[0][6] = 'N';  // Black knights
        board[0][2] = board[0][5] = 'B';  // Black bishops
        board[0][3] = 'Q';                // Black queen
        board[0][4] = 'K';                // Black king
        
        board[7][0] = board[7][7] = 'r';  // White rooks
        board[7][1] = board[7][6] = 'n';  // White knights
        board[7][2] = board[7][5] = 'b';  // White bishops
        board[7][3] = 'q';                // White queen
        board[7][4] = 'k';                // White king
    }
    
    void display() {
        std::cout << "\n  a b c d e f g h" << std::endl;
        for (int row = 0; row < BOARD_SIZE; ++row) {
            std::cout << 8 - row << " ";
            for (int col = 0; col < BOARD_SIZE; ++col) {
                std::cout << board[row][col] << " ";
            }
            std::cout << 8 - row << std::endl;
        }
        std::cout << "  a b c d e f g h\n" << std::endl;
    }
};

#ifdef HAS_SDL
class SDLChessboardRenderer {
private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;
    std::vector<uint32_t> framebuffer;
    const int WINDOW_WIDTH = 320;
    const int WINDOW_HEIGHT = 320;
    
public:
    SDLChessboardRenderer() : window(nullptr), renderer(nullptr), texture(nullptr) {
        framebuffer.resize(WINDOW_WIDTH * WINDOW_HEIGHT);
    }
    
    ~SDLChessboardRenderer() {
        if (texture) SDL_DestroyTexture(texture);
        if (renderer) SDL_DestroyRenderer(renderer);
        if (window) SDL_DestroyWindow(window);
        SDL_Quit();
    }
    
    bool initialize() {
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
            return false;
        }
        
        window = SDL_CreateWindow("Chessboard",
                                 SDL_WINDOWPOS_UNDEFINED,
                                 SDL_WINDOWPOS_UNDEFINED,
                                 WINDOW_WIDTH,
                                 WINDOW_HEIGHT,
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
                                   WINDOW_WIDTH,
                                   WINDOW_HEIGHT);
        if (!texture) {
            std::cerr << "Texture could not be created! SDL_Error: " << SDL_GetError() << std::endl;
            return false;
        }
        
        return true;
    }
    
    void renderChessboard() {
        // Clear framebuffer
        std::memset(framebuffer.data(), 0, framebuffer.size() * sizeof(uint32_t));
        
        int squareSize = WINDOW_WIDTH / BOARD_SIZE;
        
        for (int row = 0; row < BOARD_SIZE; ++row) {
            for (int col = 0; col < BOARD_SIZE; ++col) {
                uint32_t color;
                if ((row + col) % 2 == 0) {
                    color = 0xFFDDBB88;  // Light brown
                } else {
                    color = 0xFFAA7744;  // Dark brown
                }
                
                for (int y = 0; y < squareSize; ++y) {
                    for (int x = 0; x < squareSize; ++x) {
                        int pixelX = col * squareSize + x;
                        int pixelY = row * squareSize + y;
                        if (pixelX < WINDOW_WIDTH && pixelY < WINDOW_HEIGHT) {
                            framebuffer[pixelY * WINDOW_WIDTH + pixelX] = color;
                        }
                    }
                }
            }
        }
    }
    
    void display() {
        SDL_UpdateTexture(texture, nullptr, framebuffer.data(), WINDOW_WIDTH * sizeof(uint32_t));
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, nullptr, nullptr);
        SDL_RenderPresent(renderer);
        
        std::cout << "SDL Chessboard displayed. Press any key or close window to exit." << std::endl;
        
        // Wait for user to close window
        bool running = true;
        SDL_Event e;
        while (running) {
            while (SDL_PollEvent(&e)) {
                if (e.type == SDL_QUIT || e.type == SDL_KEYDOWN) {
                    running = false;
                }
            }
            SDL_Delay(16);
        }
    }
};
#endif

void printUsage(const char* programName) {
    std::cout << "Usage: " << programName << " [OPTIONS]" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --sdl     Display chessboard using SDL2 (graphical)" << std::endl;
    std::cout << "  --chars   Display chessboard using ASCII characters (default)" << std::endl;
    std::cout << "  --help    Show this help message" << std::endl;
}

int main(int argc, char* argv[]) {
    bool useSDL = false;
    
    // Parse command line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--sdl") {
            useSDL = true;
        } else if (arg == "--chars") {
            useSDL = false;
        } else if (arg == "--help") {
            printUsage(argv[0]);
            return 0;
        } else {
            std::cerr << "Unknown option: " << arg << std::endl;
            printUsage(argv[0]);
            return 1;
        }
    }
    
    if (useSDL) {
#ifdef HAS_SDL
        SDLChessboardRenderer renderer;
        if (!renderer.initialize()) {
            std::cerr << "Failed to initialize SDL. Falling back to ASCII mode." << std::endl;
            ChessboardRenderer fallbackRenderer;
            fallbackRenderer.renderChessboard();
            fallbackRenderer.display();
            return 1;
        }
        renderer.renderChessboard();
        renderer.display();
#else
        std::cerr << "SDL support not compiled in. Falling back to ASCII mode." << std::endl;
        ChessboardRenderer renderer;
        renderer.renderChessboard();
        renderer.display();
#endif
    } else {
        ChessboardRenderer renderer;
        renderer.renderChessboard();
        renderer.display();
    }
    
    return 0;
}
