#ifndef GAME_H_DEFINED
#define GAME_H_DEFINED
#include <iostream>
#include <vector>
#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

struct Game {
    bool is_running;
    bool is_menu;
    bool is_over;

    SDL_Window *window;
    SDL_Renderer *renderer;
    TTF_Font *font;

    ~Game() = default;
    Game() = default;

    void init(const char *title, int x, int y, int w, int h, bool fullscreen);

    void handle_events();
    void update();
    void render();
    void clean();

    bool running() { return is_running; }
};

struct Rect {
    float w, h;

    float x, y;

    float vx, vy;

    float ax, ay;
};

struct Obstacle {
    float w, h;

    float x, y;

    float vx, vy;

    int r = rand() % 254 + 1;;
    int g = rand() % 254 + 1;;
    int b = rand() % 254 + 1;;

    Obstacle(float w, float h, float x, float y, float vx, float vy) {
        this->w = w;
        this->h = h;
        this->x = x;
        this->y = y;
        this->vx = vx;
        this->vy = vy;
    }

    Obstacle(float w, float h, float x, float y) {
        this->w = w;
        this->h = h;
        this->x = x;
        this->y = y;
        this->vx = 4;
        this->vy = 3;
    }

    bool operator==(Obstacle rhs) {
        return this->w == rhs.w &&
            this->h == rhs.h &&
            this->x == rhs.x &&
            this->y == rhs.y &&
            this->vx == rhs.vx &&
            this->vy == rhs.vy;
    }
};

std::ostream &operator<<(std::ostream &os, const Obstacle obs) {
    os << obs.x << ", " << obs.y << "\n";
    return os;
}

#endif