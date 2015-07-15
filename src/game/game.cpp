#include "game.h"
#include "appconfig.h"
#include "engine/engine.h"
#include "game_state/battle.h"
#include "game_state/menu.h"

#include <ctime>
#include <iostream>
#include <stdlib.h>

Game::Game()
{
    m_window = nullptr;
}

Game::~Game()
{
    if(m_game_state != nullptr)
        delete m_game_state;
}

void Game::run()
{
    is_running = true;

    //utworzenie okna
    m_window = SDL_CreateWindow("TANKS", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                AppConfig::windows_rect.w, AppConfig::windows_rect.h, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

    if(m_window == nullptr) return;

    Engine::getEngine().getRenderer()->loadTexture(m_window);
    Engine::getEngine().getRenderer()->loadFont();

    srand(time(NULL)); //inicjowanie generatora pseudolosowego

    m_game_state = new Menu;

    double FPS;
    Uint32 time1, time2, dt, fps_time = 0, fps_count = 0, delay = 15;
    time1 = SDL_GetTicks();
    while(is_running)
    {
        time2 = SDL_GetTicks();
        dt = time2 - time1;
        time1 = time2;

        if(m_game_state->finished())
        {
            GameState* new_state = m_game_state->nextState();
            delete m_game_state;
            m_game_state = new_state;
        }
        if(m_game_state == nullptr) break;

        eventProces();

        m_game_state->update(dt);
        m_game_state->draw();

        SDL_Delay(delay);

        //FPS
        fps_time += dt;
        fps_count++;
        if(fps_time > 200)
        {
            FPS = (double)fps_count / fps_time * 1000;
            if(FPS > 60) delay++;
            else if(delay > 0) delay--;
            fps_time = 0; fps_count = 0;
        }
    }

    SDL_DestroyWindow(m_window);
    m_window = nullptr;
}

void Game::eventProces()
{
    SDL_Event event;
    while(SDL_PollEvent(&event))
    {
        if(event.type == SDL_QUIT)
        {
            is_running = false;
        }
        else if(event.type == SDL_WINDOWEVENT)
        {
            if(event.window.event == SDL_WINDOWEVENT_RESIZED ||
               event.window.event == SDL_WINDOWEVENT_MAXIMIZED ||
               event.window.event == SDL_WINDOWEVENT_RESTORED ||
               event.window.event == SDL_WINDOWEVENT_SHOWN)
            {

                AppConfig::windows_rect.w = event.window.data1;
                AppConfig::windows_rect.h = event.window.data2;
                Engine::getEngine().getRenderer()->setScale((float)AppConfig::windows_rect.w / (AppConfig::map_rect.w + AppConfig::status_rect.w),
                                                            (float)AppConfig::windows_rect.h / AppConfig::map_rect.h);
            }
        }

        m_game_state->eventProcess(&event);
    }
}
