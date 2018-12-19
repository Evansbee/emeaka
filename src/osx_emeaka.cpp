#include <SDL.h>

#include "emeaka.h"
#include "osx_emeaka.h"

global_variable bool IsRunning;

internal void OSXLoadGameAPIFunctions(GameAPI *apiFunctions)
{
dlopen("emeaka_game.dylib");
}

internal void OSXHandleEvent(SDL_Event *event)
{

    switch(event->type)
    {
        case SDL_QUIT:
        {
            IsRunning = false;
        }break;

        case SDL_WINDOWEVENT:
        {
            switch(event->window.event)
            {
                case SDL_WINDOWEVENT_RESIZED:
                {
                    printf("SDL_WINDOWEVENT_RESIZED\n");
                }break;
                case SDL_WINDOWEVENT_FOCUS_GAINED:
                {
                    printf("SDL_WINDOWEVENT_FOCUS_GAINED\n");
                }break;
                case SDL_WINDOWEVENT_EXPOSED:
                {
                    SDL_Window *window = SDL_GetWindowFromID(event->window.windowID);
                    SDL_Renderer *renderer = SDL_GetRenderer(window);
                    local_persist bool isWhite = false;
                    if(isWhite)
                    {
                        SDL_SetRenderDrawColor(renderer, 255,255,255,255);
                    }
                    else
                    {
                        SDL_SetRenderDrawColor(renderer,0,0,0,255);
                    }
                    isWhite = !isWhite;
                    SDL_RenderClear(renderer);
                    SDL_RenderPresent(renderer);
                }break;
            }
        }break;        
    }

}

int main(int argc, char** argv)
{   
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *window = SDL_CreateWindow("EMEAKA", 
                                          SDL_WINDOWPOS_UNDEFINED,
                                          SDL_WINDOWPOS_UNDEFINED,
                                          960,
                                          540,
                                          SDL_WINDOW_RESIZABLE);

    if(window)
    {
        SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
        
        if(renderer)
        {   SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, 960, 540);
            oid *pixels = malloc(960*540*4);

            GameAPI apiFunctions = {};
            
            OSXLoadGameAPIFunctions(&apiFunctions);

            IsRunning = true;
            while(IsRunning)
            {
                SDL_Event ev;
                SDL_WaitEvent(&ev);
                OSXHandleEvent(&ev);
            }
        }
    }
    SDL_Quit();
    return 0;
}