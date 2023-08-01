
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
    float Damage = 0.025f;
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
    bool fat;
    bool invisibility;
    int tick;
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

    void Render(int Tick)
    {
        unsigned char color = (unsigned char)(int((float(Tick - tick) / 120.0f) * 255.0f));


        DrawRectangle(position.x - scale, position.y - scale, scale * 2.0f, scale * 2.0f, (Color) { (unsigned char)(100 + int((float(DamageCooldown) / TIMEOUT) * 155)), (unsigned char)(235 + int((float(DamageCooldown) / TIMEOUT) * -200)), (unsigned char)(150 + int(((float)DamageCooldown / TIMEOUT) * -100)), (invisibility ? (unsigned char) color : (unsigned char) (255)) });

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
        DrawLineEx(position.V(), (position + (float2){ cosf(angle), sinf(angle) } * 75.0f).V(), 5.0f, SKYBLUE);
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

