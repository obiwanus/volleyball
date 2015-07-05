#ifndef WIN32_VOLLEYBALL_H
#define WIN32_VOLLEYBALL_H


struct win32_game_code
{
    HMODULE GameCodeDLL;
    game_update_and_render *UpdateAndRender;
    bool32 IsValid;
};


#endif