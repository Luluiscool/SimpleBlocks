



#define FRICTION 0.83
#define SCALE 20.0f
#define TIMEOUT 50.0f

float2 * offset;



void drawGird2D(float2 spacing, float thickness, float2 offset)
{
    for(int i = -1; i < resolution.x / spacing.x; i ++)
    {
        for(int j = -1; j < resolution.y / spacing.y; j ++)
        {
            DrawRectangle((i * spacing.x + (i * thickness)) + ((int)offset.x % (int)spacing.x), (j * spacing.y + (j * thickness)) + ((int)offset.y % (int)spacing.y), spacing.x, spacing.y, GRAY);
        }
    }
}


struct CAM2D
{
    float2 position;
};






struct Flash
{
    float2 position;
    float2 dir;
    float size;
    float Damage = 0.1f;
    int pastTick;

    bool active;

    float check(float2 vposition, float scale)
    {
        float2 A = vposition - scale;
        float2 D = vposition + scale;
        float2 B = (float2) { A.x, D.y };
        float2 C = (float2) { D.x, A.y };

        Vector2 re;

        bool t = CheckCollisionLines(A.V(), C.V(), position.V(), (position + dir * size).V(), &re);
        bool b = CheckCollisionLines(B.V(), D.V(), position.V(), (position + dir * size).V(), &re);
        bool l = CheckCollisionLines(A.V(), B.V(), position.V(), (position + dir * size).V(), &re);
        bool r = CheckCollisionLines(C.V(), D.V(), position.V(), (position + dir * size).V(), &re);

        if(t || b || l || r)
        {
            return Damage;
        }

        return 0.0f;
    }

    void Render()
    {
        DrawLineEx(position.V(), (position + dir * size).V(), 15.0f, WHITE);
    }
};








struct Killer
{
    float cooldown;
    bool boss;
    bool armed;
    float Armor = 1.0f;
    float health = 1.0f;
    float2 position;
    float2 oldPosition;
    float2 acceleration;
    int DamageCooldown = 0;
    float scale = SCALE;
    Flash flash;



    void update(float dt = 1.0f)
    {
        const float2 velocity = position - oldPosition;

        oldPosition = position;

        position = position + velocity * 0.95f + acceleration * dt * dt;

        acceleration = {};
    }

    void teleport(float2 v)
    {
        position = v;
        oldPosition = v;
    }

    void teleport(float x, float y)
    {
        position = (float2) { x, y };
        oldPosition = position;
    }

    float2 getVelocity()
    {
        return position - oldPosition;
    }

    void Render()
    {
        DrawRectangle(position.x - scale, position.y - scale, scale * 2.0f, scale * 2.0f, (Color) { (unsigned char)(100 + int((float(DamageCooldown) / TIMEOUT) * 155)), (unsigned char)(235 + int((float(DamageCooldown) / TIMEOUT) * -200)), (unsigned char)(150 + int(((float)DamageCooldown / TIMEOUT) * -100)), 255 });

        if(DamageCooldown > 0)
        {   
            DamageCooldown --;

            // DrawRectangleLines(std :: max(position.x - scale * 1.5f, 0.0f), std :: max(position.y - scale * 2.0f, 0.0f), scale * 3.0f, scale * 0.5f, WHITE);
            // DrawRectangle(std :: max(position.x - scale * 1.5f, 0.0f) + 1, std :: max((position.y - scale * 2.0f), 0.0f) + 1, ((scale * 3.0f) - 2) * health, (scale * 0.5f) - 2, RED);
            DrawRectangleLines(std :: min(std :: max(position.x - scale * 1.5f, 0.0f), (resolution.x - scale * 3.0f) - 1.0f), std :: min((double)std :: max(position.y - scale * 2.0f, 0.0f), (resolution.y - scale * 4.0) - 1.0f), scale * 3.0f, scale * 0.5f, WHITE);
            DrawRectangle(std :: min((double)std :: max(position.x - scale * 1.5f, 0.0f) + 1.0f, (resolution.x - scale * 3.0)), std :: min((double)std :: max((position.y - scale * 2.0f), 0.0f) + 1, resolution.y - scale * 4.0), ((scale * 3.0f) - 2) * health, (scale * 0.5f) - 2, RED);
        }
    }

    void solvePals(std :: vector<Killer> *k, int index)
    {
        int dx;
        for(dx = index + 1; dx < k -> size(); dx ++)
        {
            Killer * i = &k -> at(dx);
            // if(dx <= index) continue;

            Rectangle rec1 = (Rectangle) { position.x - scale, position.y - scale, scale * 2.0f, scale * 2.0f };
            Rectangle rec2 = (Rectangle) { i -> position.x - SCALE, i -> position.y - SCALE, SCALE * 2.0f, SCALE * 2.0f };

            if(!CheckCollisionRecs(rec1, rec2))
            {
                continue;
            }


            Rectangle rec = GetCollisionRec( rec1, rec2 );

            if(rec.width < rec.height)
            {
                position.x = (position.x < rec.x ? position.x - rec.width * 0.5f : position.x + rec.width * 0.5f); //- offset.height;
                i -> position.x = (i -> position.x < rec.x ? i -> position.x - rec.width * 0.5f : i -> position.x + rec.width * 0.5f); //- offset.height;
            }
            else
            {
                position.y = (position.y < rec.y ? position.y - rec.height * 0.5f: position.y + rec.height * 0.5f); //- offset.height;
                i -> position.y = (i -> position.y < rec.y ? i -> position.y - rec.height * 0.5f : i -> position.y + rec.height * 0.5f); //- offset.height;
            }
            

        }
    }

    
    void limitArea()
    {
        if(position.y > resolution.y - scale)
        {
            position.y = resolution.y - scale;
        }

        else if(position.y < scale)
        {
            position.y = scale;
        }


        if(position.x > resolution.x - scale)
        {
            position.x = resolution.x - scale;
        }

        else if(position.x < scale)
        {
            position.x = scale;
        }
    }
};









struct Bot
{
    bool boost;
    float ability;
    float speed = 1.0f;
    float health = 1.0f;
    int score;
    Flash flash;
    int lockedTarget; 
    bool isLocked;
    float2 position;
    float2 oldPosition;
    float2 acceleration;
    
    void update(float dt = 1.0f)
    {
        const float2 velocity = position - oldPosition;

        oldPosition = position;

        position = position + velocity * FRICTION + acceleration * dt * dt;

        acceleration = {};
    }

    void teleport(float2 v)
    {
        position = v;
        oldPosition = v;
    }

    void teleport(float x, float y)
    {
        position = (float2) { x, y };
        oldPosition = position;
    }

    void Render(float angle)
    {
        // DrawLineEx(position.V(), (position + (float2){ cosf(angle), sinf(angle) } * 75.0f).V(), 5.0f, SKYBLUE);
        DrawRectangle(position.x - SCALE, position.y - SCALE, SCALE * 2.0f, SCALE * 2.0f, WHITE);

        DrawRectangleLines(std :: min(std :: max(position.x - SCALE * 1.5f, 0.0f), (resolution.x - SCALE * 3.0f) - 1.0f), std :: min((double)std :: max(position.y - SCALE * 2.0f, 0.0f), (resolution.y - SCALE * 4.0) - 1.0f), SCALE * 3.0f, SCALE * 0.5f, WHITE);
        DrawRectangle(std :: min((double)std :: max(position.x - SCALE * 1.5f, 0.0f) + 1.0f, (resolution.x - SCALE * 3.0)), std :: min((double)std :: max((position.y - SCALE * 2.0f), 0.0f) + 1, resolution.y - SCALE * 4.0), ((SCALE * 3.0f) - 2) * health, (SCALE * 0.5f) - 2, BLUE);
    }

    void solvePals(std :: vector<Killer> *k)
    {
        int dx;
        for(dx = 0; dx < k -> size(); dx ++)
        {
            Killer * i = &k -> at(dx);

            Rectangle rec1 = (Rectangle) { position.x - SCALE, position.y - SCALE, SCALE * 2.0f, SCALE * 2.0f };
            Rectangle rec2 = (Rectangle) { i -> position.x - SCALE, i -> position.y - SCALE, SCALE * 2.0f, SCALE * 2.0f };

            if(!CheckCollisionRecs(rec1, rec2))
            {
                continue;
            }


            Rectangle rec = GetCollisionRec( rec1, rec2 );

            if(rec.width < rec.height)
            {
                position.x = (position.x < rec.x ? position.x - rec.width * 0.5f : position.x + rec.width * 0.5f); //- offset.height;
                i -> position.x = (i -> position.x < rec.x ? i -> position.x - rec.width * 0.5f : i -> position.x + rec.width * 0.5f); //- offset.height;
            }
            else
            {
                position.y = (position.y < rec.y ? position.y - rec.height * 0.5f : position.y + rec.height * 0.5f); //- offset.height;
                i -> position.y = (i -> position.y < rec.y ? i -> position.y - rec.height * 0.5f : i -> position.y + rec.height * 0.5f); //- offset.height;
            }
        }
    }

    void limitArea()
    {
        if(position.y > resolution.y - SCALE)
        {
            position.y = resolution.y - SCALE;
        }

        else if(position.y < SCALE)
        {
            position.y = SCALE;
        }


        if(position.x > resolution.x - SCALE)
        {
            position.x = resolution.x - SCALE;
        }

        else if(position.x < SCALE)
        {
            position.x = SCALE;
        }
    }
};







struct Bullet
{
    float2 position;
    float2 oldPosition;
    float2 acceleration;


    void update(float dt = 1.0f)
    {
        const float2 velocity = position - oldPosition;

        oldPosition = position;

        position = position + velocity + acceleration * dt * dt;

        acceleration = {};
    }

    void teleport(float2 v)
    {
        position = v;
        oldPosition = v;
    }

    void teleport(float x, float y)
    {
        position = (float2) { x, y };
        oldPosition = position;
    }

    float2 getVelocity()
    {
        return position - oldPosition;
    }

    void Render()
    {
        const float2 velocity = normalized2(getVelocity());
        DrawLineEx(position.V(), (position + velocity * 50.0f).V(), 7.0f, YELLOW);
    }
};





void ShakeScreen(float strength)
{
    if(IsKeyDown(KEY_LEFT_ALT)) return;

    *offset = (float2) { (GetRandomValue(0, 1) == 1) ? strength : -strength, (GetRandomValue(0, 1) == 1) ? strength : -strength };
}











CAM2D cam;


Bot bot;

float CursorAngle = PI * 0.5f;
float CursorDistanceScale = 150.0f;

std :: vector<Bullet> bullet;

std :: vector<Killer> killer;








void start(float2 * _OFFSET)
{
    // bot.health = 1000.0f;
    bot.isLocked = false;
    offset = _OFFSET;
    bot.teleport(center);
    bot.acceleration += 1.0f;
    // std :: cout << "eys?";
}






void BetterLoop(int Tick, bool * hasBeenStarted, bool * RESUME)
{
    //////////// BOT HEALTH ////////////



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
        return;
    }






    //////////// FOR BOT ////////////







    CursorAngle = atan2(GetMouseY() - center.y, GetMouseX() - center.x);

    float2 direction = (float2) { cosf(CursorAngle), sinf(CursorAngle) };

    float CursorDistFromCenter = distance2((float2) { (float)GetMouseX(), (float)GetMouseY() }, center);

    
    if(CursorDistFromCenter > CursorDistanceScale || CursorDistFromCenter < CursorDistanceScale - 10.0f)
    {
        SetMousePosition(center.x + direction.x * CursorDistanceScale, center.y + direction.y * CursorDistanceScale);
    }









    //////////// MOVEMENT ////////////




    






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









    ////////////// PHYSICS //////////////


    bot.update();

    bot.solvePals(&killer);

    bot.limitArea();








    /////////// BULLET ////////////








    int i = 0;

    for(auto &v : bullet)
    {
        if(v.getVelocity().length() < 0.3f && v.acceleration.length() < 0.3f)
        {
            bullet.erase(bullet.begin() + i);
            continue;
        }

        v.update();

        i ++;
    }




    //////// DIE KILLERS! /////////





    if((Tick + 1) % 120 == 0)
    {
        Killer k;

        if(GetRandomValue(0, 10) == 0)
        {
            k.scale = SCALE * 1.5f;
            k.Armor = 2.0f;
            k.boss = true;
            k.armed = false;
            k.cooldown = 300.0f;
            k.flash.active = false;
            k.flash.size = 0.0f;
        }

        if(GetRandomValue(0, 5) == 0)
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








    RayCollision worldRayCast;
    worldRayCast.distance = 2000.0f;







    ////////////// KILLER UPDATE /////////////








    int n = 0;

    for(auto &v : killer)
    {
        v.update();
        v.solvePals(&killer, n);
        v.acceleration += normalized2(bot.position - v.position) * (0.2 / v.Armor);
        v.limitArea();





        /////////// COOL PULL ///////////







        if(v.boss && GetRandomValue(0, 10) == 0)
        {
            bot.acceleration += normalized2(v.position - bot.position) * 0.1f;
        }







        /////////// SHOOT BAM (sort of) ////////////









        ////////////// THA BOSS ///////////////







        if(v.boss && !v.flash.active)
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




        if(v.flash.active && v.boss)
        {
            v.flash.position = v.position;
            float damage = v.flash.check(bot.position, SCALE);

            if(v.flash.size < 1000.0f)
            {
                v.flash.size += std :: max(v.flash.size * 0.5f, 1.0f);
            }

            if(damage > 0.0f)
            {
                bot.health -= damage / v.Armor;
                bot.acceleration += v.flash.dir * 10.0f;
            }
            

            if(Tick - v.flash.pastTick > 50)
            {
                v.flash.active = false;

                if(GetRandomValue(0, 5) == 0)
                {
                    v.cooldown = 300.0f;
                }
            }
        }







        //////////// BEAM ///////////


        RayCollision r = GetRayCollisionBox((Ray) { (Vector3){ bot.position.x, bot.position.y, 0.0f }, (Vector3){ direction.x, direction.y, 0.0f } }, (BoundingBox) { (Vector3){ v.position.x - v.scale, v.position.y - v.scale, 0 }, (Vector3){ v.position.x + v.scale, v.position.y + v.scale, 0 } });

        if(r.hit && worldRayCast.distance > r.distance)
        {
            if(IsMouseButtonDown(1))
            {
                bot.lockedTarget = n;
                bot.flash.pastTick = Tick;
                bot.flash.size = 0.0f;
            }
            worldRayCast = r;
        }








        //////////// DAMAGE BEAM //////////






        if(bot.isLocked)
        {
            float damage = bot.flash.check(v.position, v.scale);
            
            if(damage > 0.0f)
            {
                v.health -= damage / v.Armor;
                v.DamageCooldown = TIMEOUT;
                v.acceleration += bot.flash.dir * 10.0f;
            }
            

            if(Tick - bot.flash.pastTick > 50)
            {
                bot.isLocked = false;
                bot.speed = 1.0f;
            }
        }







        //////////// BULLET DAMAGE /////////






        int m = 0;


        for(auto &b : bullet)
        {
            if(b.position.x < v.position.x + v.scale + 4 && b.position.x > (v.position.x - 4) - v.scale && 
                b.position.y < v.position.y + v.scale + 4 && b.position.y > (v.position.y - 4) - v.scale)
                {
                    v.acceleration += b.getVelocity() * 0.5f;
                    v.DamageCooldown = TIMEOUT;
                    v.health -= 0.1f / v.Armor;
                    bullet.erase(bullet.begin() + m);

                    bot.ability = std :: min(bot.ability + 0.01f, 1.0f);
                }
            m ++ ;
        }

        if(v.health <= 0.0f)
        {
            killer.erase(killer.begin() + n);
            bot.score ++;
            return;
        }

        if(bot.position.x - SCALE < v.position.x + v.scale &&
        bot.position.x + SCALE > v.position.x - v.scale &&
        bot.position.y - SCALE < v.position.y + v.scale &&
        bot.position.y + SCALE > v.position.y - v.scale)
        {
            bot.health -= 0.005f;
        }

        n ++;





        ///////// BEAM STUFF /////////


        if(IsMouseButtonDown(1) && bot.ability >= 1.0f)
        {
            if(worldRayCast.distance >= 2000.0f)
            {
                bot.isLocked = false;
            }
            else
            {
                bot.ability = 0.0f;
                bot.isLocked = true;
            }
        }







        ///////////// SHOOT BEAM STUFF ////////////




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


        if(GetRandomValue(0, 1000) == 0)
        {
            v.acceleration *= 200.0f;
        }


    }

}







void Restart()
{
    bullet.clear();
    killer.clear();
    bot = (Bot) {};
    bot.health = 1.0f;
    bot.flash = (Flash) { };
    start(offset);
    HideCursor();
}




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
        k.Render();
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
        try
        {
            killer.at(bot.lockedTarget).Render();
        }
        catch(const std::exception& e)
        {

        }

        ShakeScreen(sinf(bot.flash.size) * std :: min(bot.flash.size, 1000.0f) * 0.05f);
        
        DrawText("FOCUSED", 30, 90, int(resolution.y / 28.0f), RED);
        bot.flash.Render();
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


