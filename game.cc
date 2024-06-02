#include "game.h"

const float velocity = 8.0f;
const float acceleration = 4.0f;
const int size = 10;

int count = 0;
int time_alive = 0;
int lives = 3;

int r = rand() % 254 + 1;
int g = rand() % 254 + 1;
int b = rand() % 254 + 1;

bool at_borderL, at_borderR, at_borderU, at_borderD;
bool spawn;

Rect rect = { size, size, float(rand() % 750 + 25), float(rand() % 550 + 25), 0, 0, 0, 0 };
Rect menu_rect = { 40, 40, 200, 200, 10, 10, 0, 0 };

std::vector<Obstacle *> obstacles;

void reset_pos(Rect &rect) {
    rect.x = (rand() % 700) + 50;
    rect.y = (rand() % 500) + 50;

    for (auto obs : obstacles) {
        while (rect.x > obs->x && rect.x < obs->x + obs->w
            && rect.y > obs->y && rect.y < obs->y + obs->h) {
            rect.x = (rand() % 700) + 50;
            rect.y = (rand() % 500) + 50;
        }
    }
}

void reset_pos(Obstacle &o) {
    o.x = (rand() % 700) + 50;
    o.y = (rand() % 500) + 50;

    for (auto obs : obstacles) {
        while (o.x > obs->x && o.x < obs->x + obs->w
            && o.y > obs->y && o.y < obs->y + obs->h) {
            o.x = (rand() % 700) + 50;
            o.y = (rand() % 500) + 50;
        }
    }
}

void check_collision(const Obstacle obs, Rect &rect) {
    if (rect.x > obs.x && rect.x < obs.x + obs.w &&
        rect.y > obs.y && rect.y < obs.y + obs.h) {

        if (!(abs(rect.vx) < 0.01f && abs(rect.vy) < 0.01f)) {
            reset_pos(rect);
            --lives;
        }
    }
}

void check_collision(Obstacle &colliding, Obstacle &collision) {
    if (collision.x > colliding.x && collision.x < colliding.x + colliding.w &&
        collision.y > colliding.y && collision.y < colliding.y + colliding.h) {
        if (colliding.vx > 0 && collision.vx < 0) {
            colliding.x = collision.x;
            colliding.vx *= -1;
            collision.vx *= -1;
        } else if (colliding.vx > 0 && collision.vx > 0) {
            colliding.x = collision.x;
            colliding.vx *= -1;
        } else if (colliding.vx < 0 && collision.vx > 0) {
            colliding.x = collision.x + collision.w;
            colliding.vx *= -1;
            collision.vx *= -1;
        } else if (colliding.vx < 0 && collision.vx < 0) {
            colliding.x = collision.x + collision.w;
            colliding.vx *= -1;
        }

        if (colliding.vy > 0 && collision.vy < 0) {
            colliding.y = collision.y;
            colliding.vy *= -1;
            collision.vy *= -1;
        } else if (colliding.vy > 0 && collision.vy > 0) {
            colliding.y = collision.y;
            colliding.vy *= -1;
        } else if (colliding.vy < 0 && collision.vy > 0) {
            colliding.y = collision.y + collision.h;
            colliding.vy *= -1;
            collision.vy *= -1;
        } else if (colliding.vy < 0 && collision.vy < 0) {
            colliding.y = collision.y + collision.h;
            colliding.vy *= -1;
        }
    }
}

void render_object(SDL_Renderer *renderer, const Obstacle obj) {
    for (int i = 0; i < obj.w; ++i) {
        for (int j = 0; j < obj.h; ++j) {
            SDL_RenderDrawPointF(renderer, obj.x + i, obj.y + j);
        }
    }
}

void render_object(SDL_Renderer *renderer, const Rect obj) {
    for (int i = 0; i < obj.w; ++i) {
        for (int j = 0; j < obj.h; ++j) {
            SDL_RenderDrawPointF(renderer, obj.x + i, obj.y + j);
        }
    }
}

void render_text(SDL_Renderer *renderer, const std::string &text, TTF_Font *font, SDL_Color color, int x, int y) {
    SDL_Surface *surfaceMessage = TTF_RenderText_Solid(font, text.c_str(), color);
    if (!surfaceMessage) {
        std::cerr << "Failed to create surface: " << TTF_GetError() << std::endl;
        return;
    }

    SDL_Texture *message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
    if (!message) {
        std::cerr << "Failed to create texture: " << SDL_GetError() << std::endl;
        SDL_FreeSurface(surfaceMessage);
        return;
    }

    int textWidth = surfaceMessage->w;
    int textHeight = surfaceMessage->h;

    SDL_FreeSurface(surfaceMessage);

    SDL_Rect renderQuad = { x - textWidth / 2, y - textHeight / 2, textWidth, textHeight };

    SDL_RenderCopy(renderer, message, nullptr, &renderQuad);

    SDL_DestroyTexture(message);
}

void render_menu(SDL_Renderer *renderer) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    TTF_Font *font = TTF_OpenFont("JMH_Typewriter.ttf", 24);
    SDL_Color color = { 255, 255, 255, 255 };

    render_text(renderer, "Bullet Hell", font, color, 400, 300);
    render_text(renderer, "Press Enter to Start", font, color, 400, 500);

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    render_object(renderer, menu_rect);

    SDL_RenderPresent(renderer);

    SDL_Delay(32);
}

void render_end(SDL_Renderer *renderer) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    TTF_Font *font = TTF_OpenFont("JMH_Typewriter.ttf", 24);
    SDL_Color color = { 255, 255, 255, 255 };

    render_text(renderer, "YOU LOSE!", font, color, 400, 300);
    render_text(renderer, "Score: " + std::to_string(count), font, color, 400, 350);
    render_text(renderer, "Time: " + std::to_string(time_alive / 32), font, color, 400, 400);
    render_text(renderer, "Score Per Second: " + std::to_string(count / (time_alive / 32.0)), font, color, 400, 450);

    SDL_RenderPresent(renderer);

    SDL_Delay(100);
}

bool in_bounds_x(Obstacle &obs) {
    if (obs.x <= 0 || obs.x + obs.w >= 800) {
        return false;
    } else {
        return true;
    }
}

bool in_bounds_y(Obstacle &obs) {
    if (obs.y <= 0 || obs.y + obs.h >= 600) {
        return false;
    } else {
        return true;
    }
}

void Game::init(const char *title, int x, int y, int w, int h, bool fullscreen) {
    int flags = 0;
    if (fullscreen) {
        flags = SDL_WINDOW_FULLSCREEN;
    }

    if (SDL_Init(SDL_INIT_EVERYTHING) == 0 && TTF_Init() == 0) {
        std::cout << "Subsystems Initialized...\n";

        window = SDL_CreateWindow(title, x, y, w, h, flags);
        if (window) {
            std::cout << "Window Created...\n";
        }

        renderer = SDL_CreateRenderer(window, -1, 0);
        if (renderer) {
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            std::cout << "Renderer Created...\n";
        }

        is_running = true;
        is_over = false;
        std::cout << "Game Started...\n";
        is_menu = true;
        std::cout << "Loading Menu...\n";
    } else {
        is_running = false;
        std::cout << "Game Failed to Start!\n";
    }
}

void Game::handle_events() {
    SDL_Event e;
    SDL_PollEvent(&e);

    switch (e.type) {
    case SDL_QUIT:
        is_running = false;
        break;
    case SDL_KEYDOWN:
        spawn = true;
        if (!is_over) ++count;
        switch (e.key.keysym.sym) {
        case SDLK_DOWN:
            rect.ay += acceleration;
            break;
        case SDLK_UP:
            rect.ay += -acceleration;
            break;
        case SDLK_RIGHT:
            rect.ax += acceleration;
            break;
        case SDLK_LEFT:
            rect.ax += -acceleration;
            break;
        case SDLK_RETURN2:
        case SDLK_RETURN:
        case SDLK_ESCAPE:
            is_menu = false;
            obstacles.push_back(new Obstacle(rand() % 40 + 10, rand() % 40 + 10, rand() % 760 + 10, rand() % 560 + 10));
        default:
            break;
        }
        break;
    case SDL_KEYUP:
        spawn = false;
        switch (e.key.keysym.sym) {
        case SDLK_UP:
        case SDLK_DOWN:
            rect.ay = 0.0f;
            break;
        case SDLK_LEFT:
        case SDLK_RIGHT:
            rect.ax = 0.0f;
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
}

void Game::update() {
    if (is_menu) {
        menu_rect.x += menu_rect.vx;
        menu_rect.y += menu_rect.vy;

        if (menu_rect.x <= 0 || menu_rect.x >= 800 - menu_rect.w) {
            menu_rect.vx *= -1;
        }

        if (menu_rect.y <= 0 || menu_rect.y >= 600 - menu_rect.h) {
            menu_rect.vy *= -1;
        }

        render_menu(renderer);
    } else if (!is_menu && lives >= 0) {
        if (lives > 0 && spawn && count % 10 == 0) {
            obstacles.push_back(new Obstacle(rand() % 40 + 10, rand() % 40 + 10, rand() % 760 + 10, rand() % 560 + 10));
            spawn = false;
        }

        rect.vx += rect.ax;
        rect.vy += rect.ay;

        rect.vx *= 0.7f;
        rect.vy *= 0.7f;

        if (rect.vx >= velocity) {
            rect.vx = velocity;
        }

        if (rect.vx <= -velocity) {
            rect.vx = -velocity;
        }

        if (rect.vy >= velocity) {
            rect.vy = velocity;
        }

        if (rect.vy <= -velocity) {
            rect.vy = -velocity;
        }

        rect.x += rect.vx;
        rect.y += rect.vy;

        if (rect.x <= 0) {
            rect.x = 800 - rect.w;
        } else if (rect.x >= 800 - rect.w) {
            rect.x = 0;
        }

        if (rect.y <= 0) {
            rect.y = 600 - rect.h;
        } else if (rect.y >= 600 - rect.h) {
            rect.y = 0;
        }

        for (auto &obs : obstacles) {
            obs->x += obs->vx;
            obs->y += obs->vy;

            check_collision(*obs, rect);

            for (auto &o : obstacles) {
                check_collision(*obs, *o);
            }

            if (!in_bounds_x(*obs)) {
                obs->w = rand() % 20 + 20;
                obs->h = rand() % 20 + 20;
                obs->vx *= -(rand() % 50 + 75) / 100.0;
                obs->x = (obs->x <= 0) ? 0 : 800 - obs->w;

                obs->r = rand() % 235 + 20;
                obs->g = rand() % 235 + 20;
                obs->b = rand() % 235 + 20;
            }

            if (!in_bounds_y(*obs)) {
                obs->w = rand() % 20 + 20;
                obs->h = rand() % 20 + 20;
                obs->vy *= -(rand() % 50 + 75) / 100.0;
                obs->y = (obs->y <= 0) ? 0 : 600 - obs->h;

                obs->r = rand() % 235 + 20;
                obs->g = rand() % 235 + 20;
                obs->b = rand() % 235 + 20;
            }
        }
    } else {
        is_over = true;
        render_end(renderer);
    }

    SDL_Delay(16);
}

void Game::render() {
    // background
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // obstacles
    for (auto obs : obstacles) {
        SDL_SetRenderDrawColor(renderer, obs->r, obs->g, obs->b, 255);
        render_object(renderer, *obs);
    }

    // rect
    if (abs(rect.vx) < 0.01f && abs(rect.vy) < 0.01f) {
        SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
    } else {
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    }

    render_object(renderer, rect);

    // score + lives
    SDL_Color color = { 255, 255, 255, 255 };
    TTF_Font *font = TTF_OpenFont("JMH_Typewriter.ttf", 24);
    render_text(renderer, "Score: " + std::to_string(count / 10) + "." + std::to_string(count % 10), font, color, 65, 15);
    render_text(renderer, "Lives: " + std::to_string(lives), font, color, 55, 60);
    render_text(renderer, "Time: " + std::to_string(++time_alive / 32), font, color, 730, 15);

    // present
    SDL_RenderPresent(renderer);
}

void Game::clean() {
    TTF_CloseFont(font);
    TTF_Quit();
    font = nullptr;

    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();

    std::cout << "Game Ended!" << std::endl;
}