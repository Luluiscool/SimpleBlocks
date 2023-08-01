// internal.cpp

float2 MousePos;
float2 Gravity = (float2) { 0.0f, 3.0f };
float Friction = 0.8f;
float rest = 100.0f;
#define MAX_POINT_LIMIT 16

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


struct Point
{
    float2 position;
    float2 oldPosition;
    float2 acceleration;

    float2 GetVelocity()
    {
        return position - oldPosition;
    }

    void update(float dt = 1)
    {
        const float2 velocity = position - oldPosition;

        oldPosition = position;

        position = position + (velocity * Friction) + acceleration * dt * dt;

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

        if(position.y > resolution.y * 0.7f)
        {
            position.y = resolution.y * 0.7f;
        }
    }

    void Render()
    {
        for(int i = 0; i < 10; i ++)
        {
            DrawCircle(position.x, position.y, 5.0f + float(i / 2), (Color){ WHITE.r, WHITE.g, WHITE.b, (unsigned char)(255 - ((i / 10) * 255)) });
        }
    }
};

struct VerletsPolygon
{
    Point points[MAX_POINT_LIMIT];
    int pointCount;

    int Connections[MAX_POINT_LIMIT][2];
    int connectionCount;

    int RestLength[MAX_POINT_LIMIT];

    void Init(Point _points[MAX_POINT_LIMIT], int count, int _connections[MAX_POINT_LIMIT][2], int ConnectionCount, float RestLengths[MAX_POINT_LIMIT])
    {
        pointCount = count;

        for(int i = 0; i < count; i ++)
        {
            points[i] = _points[i];
        }

        connectionCount = ConnectionCount;

        for(int i = 0; i < connectionCount; i ++)
        {
            Connections[i][0] = _connections[i][0];
            Connections[i][1] = _connections[i][1];
        }

        for(int i = 0; i < connectionCount; i ++)
        {
            RestLength[i] = RestLengths[i];
        }
    }

    void Render()
    {
        for(int i = 0; i < pointCount; i ++)
        {
            points[i].Render();
        }

        for(int i = 0; i < connectionCount; i ++)
        {
            Vector2 A = points[Connections[i][0]].position.V();
            Vector2 B = points[Connections[i][1]].position.V();

            DrawLineEx(A, B, 5.0f, WHITE);
        }
    }

    void update()
    {
        for(int i = 0; i < pointCount; i ++)
        {
            points[i].Evaluate();
        }

        for(int i = 0; i < connectionCount; i ++)
        {
            Point *A = &points[Connections[i][0]];
            Point *B = &points[Connections[i][1]];
            float rest = RestLength[i];

            float2 delta = A -> position - B -> position;
            float dist = length2(delta);
            float2 dir = delta / dist;

            float force = (dist - rest);

            A -> acceleration -= dir * force * 0.5f;
            B -> acceleration += dir * force * 0.5f;
        }
    }

    int Control(int Lock)
    {
        if(IsMouseButtonPressed(0))
        {
            if(Lock != 0)
            {
                return 0;
            }

            for(int i = 0; i < pointCount; i ++)
            {
                if(distance2(points[i].position, MousePos) < 50.0f)
                {
                    return i + 1;
                }
            }
        }
        else if(Lock != 0)
        {
            points[Lock - 1].position = MousePos;
        }

        return Lock;
    }
};


VerletsPolygon polygon;
Point pointA;
Point pointB;
Point pointC;
Point pointD;

void start()
{
    pointA.SetPosition(resolution * 0.4f);
    pointB.SetPosition(resolution.x * 0.6f, resolution.y * 0.4f);
    pointC.SetPosition(resolution * 0.6f);
    pointD.SetPosition(resolution.x * 0.4f, resolution.y * 0.6f);

    Point points[4] =
    {
        pointA,
        pointB,
        pointC,
        pointD,
    };

    int connections[6][2] =
    {
        {0, 1},
        {1, 2},
        {2, 3},
        {3, 0},
        {0, 2},
        {1, 3},
    };

    float lengths[6] =
    {
        100.0f,
        100.0f,
        100.0f,
        100.0f,
        141.0f,
        141.0f,
    };

    polygon.Init(points, 4, connections, 6, lengths);
}

int LockPoint = 0;

void Loop(int Tick)
{
    MousePos = (float2) { (float)GetMouseX(), (float)GetMouseY() };

    polygon.update();
    LockPoint = polygon.Control(LockPoint);
}

void Render()
{
    polygon.Render();

    DrawRectangle(0, (resolution.y * 0.7f) + 10.0f, resolution.x, resolution.y * 0.3f, SKYBLUE);
}