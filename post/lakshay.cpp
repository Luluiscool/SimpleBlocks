// internal.cpp

float2 MousePos;
float2 Gravity = (float2) { 0.0f, 1.5f };
float Friction = 0.87f;
float2 TEST_SCALE = (float2) { 738.0f, 541.0f };

struct Sprite 
{
    float2 pos;
    float scale = 1;

    Texture texture;

    void Init(std :: string path)
    {
        texture = LoadTexture(path.c_str());
    }

    void Render()
    {
        DrawTextureEx(texture, pos.V(), 0, scale, WHITE);
    }
};

struct Ability
{
    float JumpStrength;
    float Speed;
};

struct Player
{
    float2 position;
    float2 oldPosition;
    float2 acceleration;
    bool onGround;

    Sprite sprite;

    float2 GetVelocity()
    {
        return position - oldPosition;
    }

    void update()
    {
        const float2 velocity = position - oldPosition;

        oldPosition = position;

        position = position + (velocity * Friction) + acceleration;

        acceleration = (float2) { 0, 0 };
    }

    void SetPosition(float x, float y)
    {
        position = (float2) { x, y};
        oldPosition = position;
    }

    void SetPosition(float2 _pos)
    {
        position = _pos;
        oldPosition = _pos;
    }

    void ApplyExternalForces()
    {
        acceleration += Gravity;
    }

    void Evaluate()
    {
        update();
        ApplyExternalForces();

        if(position.y > resolution.y * 0.6f)
        {
            position.y = resolution.y * 0.6f;
            onGround = true;
        }
    }

    void Control()
    {
        if(IsKeyDown(KEY_SPACE) && onGround)
        {
            acceleration.y -= ability.JumpStrength;
        }
        if(IsKeyDown(KEY_A))
        {
            acceleration.x -= ability.Speed;
        }
        if(IsKeyDown(KEY_D))
        {
            acceleration.x += ability.Speed;
        }
    }

    Ability ability;
};

struct Block
{
    Sprite sprite;
    float2 position;
    Rectangle CollisionBound;
    unsigned int TYPE;

    void UpdateEntityPosition(Player* player, float2 scale)
    {
        float2* pos = &player -> position;
        Rectangle rec = (Rectangle) { pos -> x, pos -> y, scale.x, scale.y };

        if(!CheckCollisionRecs(rec, CollisionBound))return;

        Rectangle offset = GetCollisionRec(CollisionBound, rec);
        

        if(offset.width < offset.height)
        {
            pos -> x = (pos -> x < offset.x ? pos -> x - offset.width : pos -> x + offset.width); //- offset.height;

            if(pos -> x < offset.x)
            {
                player -> onGround = true;
            }
        }
        else
        {
            pos -> y = (pos -> y < offset.y ? pos -> y - offset.height : pos -> y + offset.height); //- offset.height;
        }
    }
};

Player player;
Block block;

void start()
{
    player.ability.JumpStrength = 30.0f;
    player.ability.Speed = 1.0f;
    player.SetPosition(resolution * 0.5f);
    player.sprite.Init("resources/l1.png");
    player.sprite.scale = 0.2f;

    block.sprite.Init("resources/l1.png");
    block.sprite.scale = 0.1f;

    block.position = (float2) { resolution.x * 0.5f, resolution.y * 0.45f };
    block.sprite.pos = block.position;
    block.CollisionBound = (Rectangle) { block.position.x, block.position.y, TEST_SCALE.x * 0.1f, TEST_SCALE.y * 0.1f};
}

void Loop(int Tick)
{
    MousePos = (float2) { (float)GetMouseX(), (float)GetMouseY() };

    player.onGround = false;

    player.Evaluate();
    block.UpdateEntityPosition(&player, TEST_SCALE * 0.2f);

    player.Control();

    player.sprite.pos = player.position;
}

void Render()
{
    player.sprite.Render();
    block.sprite.Render();
}