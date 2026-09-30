#define SDL_MAIN_USE_CALLBACKS /*Used instead of main in sdl3 application*/
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_rect.h>
#include <stdio.h>
#include <stdlib.h>

int ScreenWidth=1280;
int ScreenHeight=720;

typedef struct{ SDL_Texture *texture; int width,height; float x,y; }Player;

typedef struct{ SDL_FRect spr_src; }Frame;

typedef struct{ Frame frames [50]; int currentFrame; float timer; float frameTime; }Animation;

typedef struct {SDL_Window *window; SDL_Renderer *renderer; Player ChunLi;  Animation Idle; Animation Walk; Animation Jump; Animation Crouch; Uint64 lastFrame;} AppState;

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]){

    AppState *state = malloc(sizeof(AppState)); 
    
    SDL_Surface *surface = NULL;
    char *pngPath = NULL;
    
    if (!SDL_Init(SDL_INIT_VIDEO)){
        SDL_Log("Error: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    if (!SDL_CreateWindowAndRenderer("SF: Chun-Li trainer", ScreenWidth, ScreenHeight, SDL_WINDOW_RESIZABLE, &state->window, &state->renderer)){
        SDL_Log("Error: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    state->ChunLi.x = 30;
    state->ChunLi.y = 450;

    //Idle frame data
    state->Idle.currentFrame = 0;
    state->Idle.frameTime = 0.16f;
    state->Idle.timer = 0;
    
    state->Idle.frames[0].spr_src = (SDL_FRect){0,26,52,90};
    state->Idle.frames[1].spr_src = (SDL_FRect){52,26,52,90};
    state->Idle.frames[2].spr_src = (SDL_FRect){104,26,52,90};
    state->Idle.frames[3].spr_src = (SDL_FRect){156,26,52,90};

    //Walk frame data. Note SDL_FRect positioning needs to be checked for x position
    state->Walk.currentFrame = 0;
    state->Walk.frameTime = 0.16f;
    state->Walk.timer = 0;

    state->Walk.frames[0].spr_src = (SDL_FRect){200,26,52,90}; 
    state->Walk.frames[1].spr_src = (SDL_FRect){252,26,52,90}; 
    state->Walk.frames[2].spr_src = (SDL_FRect){304,26,52,90}; 
    state->Walk.frames[3].spr_src = (SDL_FRect){356,26,52,90}; 
    state->Walk.frames[4].spr_src = (SDL_FRect){408,26,52,90}; 
    state->Walk.frames[5].spr_src = (SDL_FRect){460,26,52,90}; 
    state->Walk.frames[6].spr_src = (SDL_FRect){512,26,52,90}; 
    state->Walk.frames[7].spr_src = (SDL_FRect){564,26,52,90}; 


    //Jump frame data. Note SDL_FRect positioning needs to be checked for x and y position.
    state->Jump.currentFrame = 0;
    state->Jump.frameTime = 0.16f;
    state->Jump.timer = 0;

    state->Jump.frames[0].spr_src = (SDL_FRect){590,26,52,60};
    state->Jump.frames[1].spr_src = (SDL_FRect){642,26,52,130};
    state->Jump.frames[2].spr_src = (SDL_FRect){694,-4,52,130};
    state->Jump.frames[3].spr_src = (SDL_FRect){746,26,52,130};

    //Crouch frame data. Note SDL_FRect positioning needs to be cehck for x and y position.
    state->Crouch.currentFrame = 0;
    state->Crouch.frameTime = 0.16f ;
    state->Crouch.timer = 0;

    state->Crouch.frames[0].spr_src = (SDL_FRect){1782,26,52,60};
    state->Crouch.frames[1].spr_src = (SDL_FRect){1824,26,52,60};

    SDL_asprintf(&pngPath,"%sChunLiSpriteSheet.png", SDL_GetBasePath());
    surface = SDL_LoadPNG(pngPath);

    if (!surface){
        SDL_Log("Error: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    SDL_free(pngPath);

    state->ChunLi.texture = SDL_CreateTextureFromSurface(state->renderer, surface);
    if(!state->ChunLi.texture){
        SDL_Log("Error: %s",SDL_GetError());
        return SDL_APP_FAILURE;
    }

    state->lastFrame = SDL_GetTicks();
    *appstate = state;
    SDL_DestroySurface(surface);
    return SDL_APP_CONTINUE;

}

SDL_AppResult SDL_AppEvent(void *appstate,SDL_Event *event){
    if (event->type == SDL_EVENT_QUIT){
        return SDL_APP_SUCCESS;
    }

    AppState *state = appstate;

    const bool *key_presses = SDL_GetKeyboardState(NULL);

    float speed = 22.0f;

    /*if (key_presses[SDL_SCANCODE_D]) state->ChunLi.x += speed ; 
    if (key_presses[SDL_SCANCODE_A]) state->ChunLi.x -= speed ;
    if (key_presses[SDL_SCANCODE_W]) state->ChunLi.y -= speed;
    if (key_presses[SDL_SCANCODE_S]) state->ChunLi.y += speed;*/

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate){

    AppState *state = appstate;
    SDL_FRect dst_rect;

    Uint64 currentTime = SDL_GetTicks();
    float deltaTime = (currentTime - state->lastFrame)/1000.0f;
    state->lastFrame = currentTime;
    
    Uint64 frameStart = SDL_GetTicks();
    state->Idle.timer += deltaTime;
    state->Walk.timer += deltaTime;

    SDL_SetRenderDrawColor(state->renderer,0,0,0,255);
    SDL_RenderClear(state->renderer);

    if(state->Idle.timer >= state->Idle.frameTime){
        state->Idle.timer -= state->Idle.frameTime;
        state->Idle.currentFrame ++;

        if(state->Idle.currentFrame >= 4){
            state->Idle.currentFrame = 0;
        }
    }

    Frame *frame_number = &state->Idle.frames[state->Idle.currentFrame];

    if (state->Walk.timer >= state->Walk.frameTime){
        state->Walk.timer -= state->Walk.frameTime;
        state->Walk.currentFrame ++;

        if(state->Walk.currentFrame >=8){
            state->Walk.currentFrame = 0;
        }
    }


    dst_rect.x = state->ChunLi.x;
    dst_rect.y = state->ChunLi.y;
    dst_rect.w = frame_number->spr_src.w*3;
    dst_rect.h = frame_number->spr_src.h*3; 

    SDL_RenderTexture(state->renderer, state->ChunLi.texture, &frame_number->spr_src, &dst_rect);

    SDL_RenderPresent(state->renderer);
    /*printf("Frame: %d\n", state->Idle.currentFrame);*/
    
    Uint64 frameTime = SDL_GetTicks() - frameStart;
    if (frameTime < 16){
        SDL_Delay(16-frameTime);
    }
    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result){
    AppState *state = appstate;
    SDL_DestroyRenderer(state->renderer);
    SDL_DestroyWindow(state->window);
    SDL_DestroyTexture(state->ChunLi.texture);
    free(state);
}
