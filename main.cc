#include "game.cc"

int main(int argc, char const *argv[]) {
    Game *game = new Game();
    game->init("game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, false);
    while (game->running()) {
        game->handle_events();
        game->update();

        if (!game->is_menu && !game->is_over) {
            game->render();
        }
    }

    game->clean();
    delete game;

    return 0;
}