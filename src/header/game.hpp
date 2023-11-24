#ifndef GAME_HPP
#define GAME_HPP

struct Game
{
    Game();

    bool setup();
    void update();
    void render();
    void handleInput();
};

#endif