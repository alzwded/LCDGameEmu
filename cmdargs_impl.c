#include <SDL/SDL.h>
#include <stdlib.h>
#include "cmdargs.h"
#include "log.h"

void args_list_joysticks(char const* _)
{
    extern char const* _GetVersion(void);
    printf("lcdgameemu %s\n", _GetVersion());
    if(0 > SDL_Init(SDL_INIT_JOYSTICK)) {
        jaklog(FATAL, JAK_STR|JAK_LN, "failed to initialize SDL");
        exit(255);
    }

    {
        int i = 0, num = SDL_NumJoysticks();
        printf("%d joystick%s detected\n", num, (num != 1) ? "s" : "");
        for(; i < num; ++i) {
            printf("%3d. %s\n", i + 1, SDL_JoystickName(i));
        }
    }

    SDL_Quit();
    exit(1);
}
