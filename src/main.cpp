#include "App/App.h"

#ifdef _WIN32
#include <windows.h>
#endif

int main()
{
#ifdef _WIN32 // Hide the stupid console window
    ShowWindow(GetConsoleWindow(), SW_HIDE);
#endif
    if (App::Load() != 0) { return -1; }
    App::Play();
    App::Free();
    return 0;
}