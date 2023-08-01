
#include <raylib.h>
#include <iostream>
#include <vector>

#if defined(PLATFORM_DESKTOP)
    #define GLSL_VERSION            330
#else   // PLATFORM_RPI, PLATFORM_ANDROID, PLATFORM_WEB
    #define GLSL_VERSION            100
#endif

#include "vector.cpp"
#include "include.cpp"

const float2 Center = resolution / 2;

float FrameTime = 0.0f;
int Tick = 0;

bool hasBeenStarted;

bool RESUME;

float2 _OFFSET = (float2) { 0.0f, 0.0f };

bool musicStatus = false;


void ToggleMusic(Music * music)
{
    musicStatus = !musicStatus;

    if(musicStatus)
    {
        ResumeMusicStream(*music);
    }
    else
    {
        PauseMusicStream(*music);
    }
}


bool twoStarted = hasBeenStarted;



Shader bloom;


int main () {
    // SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(resolution.x, resolution.y, "");
    SetConfigFlags(FLAG_MSAA_4X_HINT); // Anti-aliasing x4 (if available)
    SetExitKey(KEY_NULL);

    InitAudioDevice();

    bloom = LoadShader(0, TextFormat("bloom.fs", GLSL_VERSION));

    Music music = LoadMusicStream("mini1111.xm");
    music.looping = true;

    PlayMusicStream(music);

    if(!hasBeenStarted)
    {
        PauseMusicStream(music);
    }


    RenderTexture2D target = LoadRenderTexture(resolution.x, resolution.y);
    
    SetTargetFPS(60);


    start(&_OFFSET);
    // Loop(0);

    while (WindowShouldClose() == false){
        FrameTime = GetFrameTime(); // Get time took to render last frame


        if(hasBeenStarted)BetterLoop(Tick, &hasBeenStarted, &RESUME);

        UpdateMusicStream(music);

        if(twoStarted != hasBeenStarted)
        {
            ToggleMusic(&music);
            twoStarted = hasBeenStarted;
        }

        if(IsWindowResized())
        {
            resolution = (float2) { (float)GetScreenWidth(), (float)GetScreenHeight() };
            center = resolution * 0.5f;

            std :: cout << resolution.x << std :: endl;
        }

        bool isDead;


        BeginTextureMode(target);
            ClearBackground(BLACK);


                isDead = Render(&hasBeenStarted, &RESUME, Tick);

                if(isDead)
                {
                    *offset = (float2) { 0.0f, 0.0f };
                }


            DrawText(std :: to_string(GetFPS()).c_str(), 30, 30, 20, SKYBLUE);

        EndTextureMode();


        BeginDrawing();
            ClearBackground(BLACK);

            BeginShaderMode(bloom);
                DrawTextureRec(target.texture, (Rectangle) { 0.0f, 0.0f, (float)target.texture.width, (float)-target.texture.height }, (hasBeenStarted && !isDead) ? _OFFSET.V() : (Vector2) { 0.0f, 0.0f }, WHITE);
            EndShaderMode();
        EndDrawing();


        Tick ++;
    }

    CloseWindow();

    system("cls");
    return 0;
}

