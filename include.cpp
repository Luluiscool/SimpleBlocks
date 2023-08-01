#include "struct.cpp"








CAM2D cam;


Bot bot;

float CursorAngle = PI * 0.5f;
float CursorDistanceScale = 150.0f;

std :: vector<Bullet> bullet;

std :: vector<Killer> killer;






















void start(float2 * _OFFSET)
{
    bot.isLocked = false;
    offset = _OFFSET;
    bot.teleport(center);
    bot.acceleration += 1.0f;
    bot.flash.Damage *= 4.0f;
}













void BetterLoop(int Tick, bool * hasBeenStarted, bool * RESUME)
{


    //////////////////////////////////////////////
    ///////////////       MENU       /////////////
    //////////////////////////////////////////////



    if(bot.health <= 0.0f)
    {
        ShowCursor();
        return;
    }

    if(IsKeyPressed(KEY_ESCAPE))
    {
        ShowCursor();
        *hasBeenStarted = false;
        *RESUME = true;
        return;
    }

    if(IsKeyDown(KEY_LEFT_ALT))
    {
        ShowCursor();
        return;
    }
    
    else
    {
        HideCursor();
    }






    //////////////////////////////////////////////////////////////////
    //////////////       [PLAYER / CURSOR] ANGLE       ///////////////
    //////////////////////////////////////////////////////////////////







    CursorAngle = atan2(GetMouseY() - center.y, GetMouseX() - center.x);

    float2 direction = (float2) { cosf(CursorAngle), sinf(CursorAngle) };

    float CursorDistFromCenter = distance2((float2) { (float)GetMouseX(), (float)GetMouseY() }, center);

    
    if(CursorDistFromCenter > CursorDistanceScale || CursorDistFromCenter < CursorDistanceScale - 10.0f)
    {
        SetMousePosition(center.x + direction.x * CursorDistanceScale, center.y + direction.y * CursorDistanceScale);
    }









    /////////////////////////////////////////
    //////////// PLAYER MOVEMENT ////////////
    /////////////////////////////////////////




    






    if(IsKeyDown(KEY_W))
    {
        bot.acceleration += direction * bot.speed;
    }  

    if(IsKeyDown(KEY_S))
    {
        bot.acceleration -= direction * bot.speed;
    }

    if(IsKeyDown(KEY_A))
    {
        bot.acceleration -= (float2) { -direction.y, direction.x } * bot.speed;
    }  

    if(IsKeyDown(KEY_D))
    {
        bot.acceleration += (float2) { -direction.y, direction.x } * bot.speed;
    }

    if(IsMouseButtonDown(0) && Tick % 6 == 0)
    {
        Bullet b;
        b.teleport(bot.position);
        b.acceleration = direction * 50.0f;

        bullet.push_back(b);
    }







    ////////////////////////////////////////////
    ////////////// PLAYER PHYSICS //////////////
    ////////////////////////////////////////////








    bot.update();

    bot.solvePals(&killer);

    bot.limitArea();






    ////////////////////////////////////////////
    /////////////// UPDATE BULLET //////////////
    ////////////////////////////////////////////


    


    int i = 0;

    for(auto &v : bullet)
    {
         // If bullet is too slow

         
        if(v.getVelocity().length() < 0.3f && v.acceleration.length() < 0.3f)
        {
             // Get rid of it

            bullet.erase(bullet.begin() + i);
            continue;
        }

         // Update the bullet

        v.update();

        i ++;
    }











    
    ///////////////////////////////////////////////
    ////////////// SPAWNING BAD BOIS //////////////
    ///////////////////////////////////////////////







    if((Tick + 1) % 120 == 0)
    {
        Killer k;

        if((bot.score + 1) % 10 == 0)
        {
            k.fat = true;
        }

        else if(GetRandomValue(0, 10) == 0)
        {
            k.scale = SCALE * 1.5f;
            k.Armor = 2.0f;
            k.boss = true;
            k.armed = false;
            k.cooldown = 300.0f;
            k.flash.active = false;
            k.flash.size = 0.0f;
        }

        else if(GetRandomValue(0, 5) == 0)
        {
            k.scale = SCALE * 1.25f;
            k.Armor = 1.5f;
            k.boss = false;
            k.armed = true;
            k.cooldown = 300.0f;
            k.flash.active = false;
            k.flash.size = 0.0f;
        }

        k.teleport(center);

        k.acceleration += normalized2(bot.position - k.position) * 10.0f;

        killer.push_back(k);
    }


    




    
    ///////////////////////////////////////////////
    ////////////// UPDATING BAD BOIS //////////////
    ///////////////////////////////////////////////










    int n = 0;

    for(auto &v : killer)
    {



        ////////////////////////////////////////////
        ///////////// UPDATING PHYSICS /////////////
        ////////////////////////////////////////////






        v.update();
        v.solvePals(&killer, n);
        v.acceleration += normalized2(bot.position - v.position) * (0.2 / v.Armor);
        v.limitArea();







        ///////////////  FAT BOI's ATTACK ///////////////

        if(v.fat)
        {
            if(GetRandomValue(0, 300) == 0) // 1 in 301 probability to become invisible
            {
                v.invisibility = true;
                v.tick = Tick;
            }

            if(v.invisibility) // if big fat bad boi is invisible
            {
                if(GetRandomValue(0, 10) == 0)
                {
                    v.acceleration += normalized2(bot.position - v.position) * 20.0f; // Accelerate towards player
                }

                if(Tick - v.tick > 120) // 120 frame timer to reset invisibility
                {
                    v.invisibility = false;
                }
            }
        }







        ////////////// BIG BAD BOI LASER ///////////////







        if(v.boss && !v.flash.active)   // if big bad boi is not using laser, there is a cooldown until they are able to use it.
        {
            v.cooldown -= 1.0f;
            if(v.cooldown <= 0.0f)
            {
                v.flash.active = true;
                v.flash.pastTick = Tick;
                v.flash.size = 0.0f;
                v.flash.dir = normalized2(bot.position - v.position);
                v.flash.position = v.position;
            }
        }




        

        if(v.flash.active && v.boss)   // If big bad boi is using laser, do what it should do.
        {
            v.flash.position = v.position; // Setting position of laser to big bad boi

            float damage = v.flash.check(bot.position, SCALE); // Calculating damage to PLAYER

            if(v.flash.size < 1000.0f)
            {
                v.flash.size += std :: max(v.flash.size * 0.5f, 1.0f); // Increase length of laser
            }

            if(damage > 0.0f) // If laser should deal damage to PLAYER
            {
                bot.health -= damage * v.Armor; // Deal damage
                bot.acceleration += v.flash.dir * 10.0f; // Add force to PLAYER
            }
            

            if(Tick - v.flash.pastTick > 50) // If big bad boi had enough fun with the laser
            {

                v.flash.active = false; // Deactivate it.
                if(GetRandomValue(0, 5) == 0) // But there is a small change that big bad boi uses it again
                {
                    v.cooldown = 300.0f;
                }
            }
        }









        /////////////////////////////////////
        //////////// PLAYER LASER ///////////
        /////////////////////////////////////









        if(IsMouseButtonDown(1) && bot.ability >= 1.0f)
        {
            bot.ability = 0.0f;
            bot.flash.pastTick = Tick;
            bot.flash.size = 0.0f;
            bot.isLocked = true;
        }









        //////////////////////////////////////
        //////////// LASER DAMAGE ////////////
        //////////////////////////////////////








        if(bot.isLocked)
        {
            float damage = bot.flash.check(v.position, v.scale); // Calculate damage
            

            // Deal damage to bad bois


            if(damage > 0.0f)
            {
                v.health -= damage / v.Armor;
                v.DamageCooldown = TIMEOUT;
                v.acceleration += bot.flash.dir * 10.0f;
            }


            // Limit use of laser for 50 frames or 5/6 seconds
            

            if(Tick - bot.flash.pastTick > 50)
            {
                bot.isLocked = false;
                bot.speed = 1.0f;
            }
        }






        ////////////////////////////////////
        //////////// BULLET DAMAGE /////////
        ////////////////////////////////////






        int m = 0;


        for(auto &b : bullet)
        {
            // If bullet hits bad boi


            if(b.position.x < v.position.x + v.scale + 4 && b.position.x > (v.position.x - 4) - v.scale && 
                b.position.y < v.position.y + v.scale + 4 && b.position.y > (v.position.y - 4) - v.scale)
                {

                    // Do all those stuff to "die" them

                    v.acceleration += b.getVelocity() * 0.5f;
                    v.DamageCooldown = TIMEOUT;
                    v.health -= 0.1f / v.Armor;
                    bullet.erase(bullet.begin() + m);

                    bot.ability = std :: min(bot.ability + 0.01f, 1.0f);
                }
            m ++ ;
        }







        if(v.health <= 0.0f) // If bad boi is already dead
        {
            killer.erase(killer.begin() + n); // D I E
            bot.score ++;
            return;
        }











        ////////////////////////////////////////////////
        ///////////// INCREASE SIZE OF BEAM ////////////
        ////////////////////////////////////////////////








        if(bot.isLocked)
        {
            bot.speed = 0.0f;
            bot.flash.dir = direction;

            bot.flash.position =  bot.position;

            if(bot.flash.size < 1000.0f)
            {
                bot.flash.size += std :: max(bot.flash.size * 0.5f, 1.0f);
            }
        }








        
        if(bot.position.x - SCALE < v.position.x + v.scale &&
        bot.position.x + SCALE > v.position.x - v.scale &&
        bot.position.y - SCALE < v.position.y + v.scale &&
        bot.position.y + SCALE > v.position.y - v.scale) // if bad boi collides with the PLAYER
        {
            bot.health -= 0.005f; // hurt em.
        }

        n ++;







        // SMALL CHANGE TO RUN TO THE PLAYER


        if(GetRandomValue(0, 1000) == 0)
        {
            v.acceleration *= 200.0f;
        }


    }

    if(IsKeyDown(KEY_F3) && Tick % 20 == 0)
    {
        if(IsKeyDown(49))
        {
            Killer kill;
            kill.teleport(center);
            
            killer.push_back(kill);
        }
        
        else if(IsKeyDown(50))
        {
            Killer kill;

            kill.scale = SCALE * 1.5f;
            kill.Armor = 2.0f;
            kill.boss = true;
            kill.armed = false;
            kill.cooldown = 300.0f;
            kill.flash.active = false;
            kill.flash.size = 0.0f;

            kill.teleport(center);
            
            killer.push_back(kill);
        }
    }

    if(IsKeyDown(51))
    {
        bot.health = 1.0f;
    }

}








//////////////////////////////////////
//////////// RESTART GAME ////////////
//////////////////////////////////////









void Restart()
{
    // Reset all stuff

    bullet.clear();
    killer.clear();
    bot = (Bot) {};
    bot.health = 1.0f;
    bot.flash = (Flash) { };
    start(offset);
    HideCursor();
}










//////////////////////////////////
////////// DEATH SCREEN //////////
//////////////////////////////////
////////// HOME SCREEN  //////////
//////////////////////////////////












void RenderDeath()
{
    bot.flash.size = 0;
    bot.isLocked = false;
    
    DrawRectangle(0, 0, resolution.x, resolution.y, (Color) { 0, 0, 0, 128 });

    float messageWidth = MeasureText("Good Luck Next Time Buddy...", resolution.y / 18);

    DrawText("Good Luck Next Time Buddy...", center.x + ((messageWidth * 0.5f) - messageWidth), resolution.y * 0.3f, resolution.y / 18, SKYBLUE);

    float messageRespawn = MeasureText("Try Again", resolution.y / 20);



    float2 MousePos = (float2) { (float)GetMouseX(), (float)GetMouseY() };

    float DY = MeasureTextEx(GetFontDefault(), "Try Again", resolution.y / 20, 0).y;

    if(MousePos.x > (center.x - messageRespawn * 0.5f) - 5.0f && MousePos.x < (center.x + messageRespawn * 0.5f) + 10.0f &&
    MousePos.y > (resolution.y * 0.5f) - 5.0f && MousePos.y < (resolution.y * 0.5f + DY) + 10.0f)
    {
        DrawText("Try Again", center.x + ((messageRespawn * 0.5f) - messageRespawn), resolution.y * 0.5f, resolution.y / 20, GREEN);

        if(IsMouseButtonPressed(0))
        {
            Restart();
        }
    }
    else
    {
        DrawText("Try Again", center.x + ((messageRespawn * 0.5f) - messageRespawn), resolution.y * 0.5f, resolution.y / 20, RED);
    }

    DrawRectangleLines(center.x + ((messageRespawn * 0.5f) - messageRespawn) - 5.0f, (resolution.y * 0.5f) - 5.0f, messageRespawn + 10.0f, DY + 10.0f, WHITE);
}
void RenderHome(bool * hasBeenStarted, std :: string t = "Start!", int Tick = 0)
{

    drawGird2D((float2){50, 50}, 2.0f, (float2) { (float)Tick, 0.0f });

    float messageWidth = MeasureText("SIMPLE BLOCKS", resolution.y / 16);

    DrawText("SIMPLE BLOCKS", center.x + ((messageWidth * 0.5f) - messageWidth), resolution.y * 0.1f, resolution.y / 16, SKYBLUE);

    float messageRespawn = MeasureText(t.c_str(), resolution.y / 20);


    float2 MousePos = (float2) { (float)GetMouseX(), (float)GetMouseY() };

    float DY = MeasureTextEx(GetFontDefault(), t.c_str(), resolution.y / 20, 0).y;

    if(MousePos.x > (center.x - messageRespawn * 0.5f) - 5.0f && MousePos.x < (center.x + messageRespawn * 0.5f) + 10.0f &&
    MousePos.y > (resolution.y * 0.25f) - 5.0f && MousePos.y < (resolution.y * 0.25f + DY) + 5.0f)
    {
        DrawText(t.c_str(), center.x + ((messageRespawn * 0.5f) - messageRespawn), resolution.y * 0.25f, resolution.y / 20, GREEN);

        if(IsMouseButtonPressed(0))
        {
            *hasBeenStarted = true;
            HideCursor();
        }
    }
    else
    {
        DrawText(t.c_str(), center.x + ((messageRespawn * 0.5f) - messageRespawn), resolution.y * 0.25f, resolution.y / 20, YELLOW);
    }

    DrawRectangleLines(center.x + ((messageRespawn * 0.5f) - messageRespawn) - 5.0f, (resolution.y * 0.25f) - 5.0f, messageRespawn + 10.0f, DY + 5.0f, WHITE);



    messageRespawn = MeasureText("Quit", resolution.y / 20);

    DY = MeasureTextEx(GetFontDefault(), "Quit", resolution.y / 20, 0).y;

    if(MousePos.x > (center.x - messageRespawn * 0.5f) - 5.0f && MousePos.x < (center.x + messageRespawn * 0.5f) + 10.0f &&
    MousePos.y > (resolution.y * 0.4f) - 5.0f && MousePos.y < (resolution.y * 0.4f + DY) + 5.0f)
    {
        DrawText("Quit", center.x + ((messageRespawn * 0.5f) - messageRespawn), resolution.y * 0.4f, resolution.y / 20, RED);

        if(IsMouseButtonPressed(0))
        {
            ShowCursor();
            exit(0);
        }
    }
    else
    {
        DrawText("Quit", center.x + ((messageRespawn * 0.5f) - messageRespawn), resolution.y * 0.4f, resolution.y / 20, ORANGE);
    }


    DrawRectangleLines(center.x + ((messageRespawn * 0.5f) - messageRespawn) - 5.0f, (resolution.y * 0.4f) - 5.0f, messageRespawn + 10.0f, DY + 5.0f, WHITE);


    float size = MeasureText("CREDITS", resolution.y / 20);

    float tex = (resolution.x - size) - 5.0f;


    if(MousePos.x > tex - 10.0f && MousePos.x < resolution.x - 5.0f &&
        MousePos.y > 5.0f && MousePos.y < DY + 10.0f)
    {
        DrawText("CREDITS", tex - 5.0f, 10.0f, resolution.y / 20, ORANGE);

        if(IsMouseButtonPressed(0))
        {
            system("start https://rusrs.netlify.app/credits");
        }
    }
    else
    {
        DrawText("CREDITS", tex - 5.0f, 10.0f, resolution.y / 20, GREEN);
    }

    DrawRectangleLines(tex - 10, 5, size + 10, DY + 5, WHITE);



    float m1 = MeasureText("Use [ W A S D ] to move", resolution.y / 22);

    DrawText("Use [ W A S D ] to move", center.x + ((m1 * 0.5f) - m1), resolution.y * 0.55f, resolution.y / 22, (Color) { 150, 199, 255, 255 });


    m1 = MeasureText("Mouse to look around", resolution.y / 22);

    DrawText("Mouse to look around", center.x + ((m1 * 0.5f) - m1), resolution.y * 0.65f, resolution.y / 22, (Color) { 150, 199, 255, 255 });

    
    m1 = MeasureText("LEFT CLICK to shoot", resolution.y / 22);

    DrawText("LEFT CLICK to shoot", center.x + ((m1 * 0.5f) - m1), resolution.y * 0.75f, resolution.y / 22, (Color) { 150, 199, 255, 255 });

    m1 = MeasureText("RIGHT CLICK to use Laser if Laser Bar is filled", resolution.y / 22);

    DrawText("RIGHT CLICK to use Laser if Laser Bar is filled", center.x + ((m1 * 0.5f) - m1), resolution.y * 0.85f, resolution.y / 22, (Color) { 150, 199, 255, 255 });
}









bool Render(bool * hasBeenStarted, bool * RESUME, int Tick)
{

    if(!*hasBeenStarted)
    {
        RenderHome(hasBeenStarted, *RESUME ? "Resume" : "Start!", Tick);
        return true;
    }


    // Drawing Grid
    drawGird2D((float2){50, 50}, 2.0f, *offset);

    
    for(auto &v : bullet)
    {
        // std :: cout << v.position.x << '\n';
        v.Render();
    }


    float active = 0.0f;
    float size = 0.0f;

    
    for(auto &k : killer)
    {
        if(k.flash.active && k.boss)
        {
            k.flash.Render();
            active += k.flash.size;
            size ++;
        }
        k.Render(Tick);
    }

    

    bot.Render(CursorAngle);

    DrawRectangleLinesEx((Rectangle) { center.x - 150.0f, 10.0f, 300.0f, 40.0f } , 2.0f, WHITE);

    if(bot.ability >= 1.0f)
    {
        DrawRectangleRec((Rectangle) { center.x - 148.0f, 12.0f, std :: max(bot.ability * 296.0f, 5.0f), 36.0f }, (Color) { (unsigned char)(int(float(102 + int(abs(sinf(float(Tick / 10) * 0.1f) * 153.0f))) * 0.7f)), (unsigned char)(int(float(191 + int(abs(sinf(float(Tick) * 0.1f) * 64.0f))) * 0.7f)), 255, 255 });
    }
    else
    {
        DrawRectangleRec((Rectangle) { center.x - 148.0f, 12.0f, std :: max(bot.ability * 296.0f, 5.0f), 36.0f }, (Color){ (unsigned char)(102 + (bot.boost * 100)), 191, 255, 255 });
    }





    if(bot.isLocked == true && bot.ability <= 0.0f && bot.flash.size > 0.0f)
    {
        DrawRectangle(0, 0, resolution.x, resolution.y, (Color) { 0, 0, 0, 128 });
        ShakeScreen(sinf(bot.flash.size) * std :: min(bot.flash.size, 1000.0f) * 0.05f);
        
        DrawText("FOCUSED", 30, 90, int(resolution.y / 28.0f), RED);
        bot.flash.Render();
        bot.Render(CursorAngle);
    }
    

    if(size > 0)
    {
        active /= size;
        ShakeScreen(sinf(active) * std :: min(active, 1000.0f) * 0.05f);
    }


    
    DrawText((std :: string("SCORE ") + std :: to_string(bot.score)).c_str(), 30, 60, int(resolution.y / 28.0f), GREEN);

    if(IsKeyDown(KEY_LEFT_ALT))
    {
        DrawText("GAME FREEZED", 30, 120, int(resolution.y / 28.0f), (Color) { 100, 149, 237, 255 });
    }

    if(!bot.isLocked && !active)
    {
        *offset = (float2) { 0.0f, 0.0f };
    }


    if(bot.health <= 0.0f)
    {
        RenderDeath();
        return true;
    }

    return false;
}


