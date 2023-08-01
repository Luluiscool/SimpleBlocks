// 3D Plane Simulator

#define MAX_POINTS 64
#define AMPLIFIER 100.0f



struct GameCamera
{
    float3 position;
    float3 rotation = (float3) { 0.0f, 0.0f, 0.0f};

    void RenderWireframe()
    {

    }
};

struct Wireframe
{
    float3 points[MAX_POINTS];
    float2 screenpoints[MAX_POINTS];

    int connections[MAX_POINTS][2];

    int point_length;
    int joint_length;

    void Init(float3 _points[MAX_POINTS], int _connections[MAX_POINTS][2], int _point_length, int _joint_length)
    {
        point_length = _point_length;
        joint_length = _joint_length;

        for(int i = 0; i < point_length; i ++)
        {
            points[i] = _points[i];
        }

        for(int i = 0; i < joint_length; i ++)
        {
            connections[i][0] = _connections[i][0];
            connections[i][1] = _connections[i][1];
        }
    }

    void TransformMatrix(GameCamera cam)
    {
        for(int i = 0; i < point_length; i ++)
        {
            if(points[i].z < 0.0f)
            {
                screenpoints[i].x = points[i].x / 0.01;
                screenpoints[i].y = points[i].y / 0.01;
                continue;
            }

            screenpoints[i].x = (points[i].x + cam.position.x) / (points[i].z / 1);
            screenpoints[i].y = (points[i].y + cam.position.y) / (points[i].z / 1);
        }
    }
};

GameCamera Cam;
Wireframe wireframe;




void Render()
{
    Wireframe transformed = wireframe;

    for(int i = 0; i < transformed.point_length; i ++)
    {

        float3* point = &transformed.points[i];

        point -> x -= Cam.position.x;
        point -> y -= Cam.position.y;
        point -> z -= Cam.position.z;

        float2 xz = (float2) { point -> x, point -> z };
        xz = rotateVector(xz, Cam.rotation.y);

        point -> x = xz.x;
        point -> z = xz.y;


        float2 yz = (float2) { point -> y, point -> z };
        yz = rotateVector(yz, Cam.rotation.x);

        point -> y = yz.x;
        point -> z = yz.y;

    }

    transformed.TransformMatrix(Cam);

    for(int i = 0; i < transformed.point_length; i ++)
    {
        if(transformed.points[i].z < 0.0)
        {
            continue;
        }
        DrawCircle(transformed.screenpoints[i].x * AMPLIFIER + resolution.x * 0.5f, transformed.screenpoints[i].y * AMPLIFIER + resolution.y * 0.5f, 5.0f, WHITE);
    }

    for(int i = 0; i < transformed.joint_length; i ++)
    {

        float2 A = transformed.screenpoints[transformed.connections[i][0]];
        float2 B = transformed.screenpoints[transformed.connections[i][1]];

        // if(transformed.points[transformed.connections[i][0]].z < 0.0)
        // {
        //     continue;
        // }
        
        // if(transformed.points[transformed.connections[i][1]].z <= 0.0)
        // {
        //     continue;
        // }

        DrawLine(
            A.x * AMPLIFIER + resolution.x * 0.5f,
            A.y * AMPLIFIER + resolution.y * 0.5f,
            B.x * AMPLIFIER + resolution.x * 0.5f,
            B.y * AMPLIFIER + resolution.y * 0.5f,
            WHITE);
    }

    DrawText(std :: to_string(Cam.rotation.x).c_str(), 30, 60, 20, RED);
    DrawText(std :: to_string(Cam.rotation.y).c_str(), 30, 90, 20, YELLOW);
    DrawText(std :: to_string(Cam.rotation.z).c_str(), 30, 120, 20, GREEN);
    
    DrawText(std :: to_string(Cam.position.x).c_str(), 30, 150, 20, RED);
    DrawText(std :: to_string(Cam.position.y).c_str(), 30, 180, 20, YELLOW);
    DrawText(std :: to_string(Cam.position.z).c_str(), 30, 210, 20, GREEN);
}

void Loop(int Tick)
{
    Cam.rotation.y += GetMouseDelta().x * 0.001f;
    Cam.rotation.x += GetMouseDelta().y * 0.001f;

    if(Cam.rotation.x > PI * 0.5f)
    {
        Cam.rotation.x = PI * 0.5f;
    }

    if(Cam.rotation.x < -PI * 0.5f)
    {
        Cam.rotation.x = -PI * 0.5f;
    }

    if(IsKeyDown(KEY_W))
    {
        Cam.position.x += sinf(Cam.rotation.y) * 0.1f;
        Cam.position.z += cosf(Cam.rotation.y) * 0.1f;
    }
    
    if(IsKeyDown(KEY_S))
    {
        Cam.position.x -= sinf(Cam.rotation.y) * 0.1f;
        Cam.position.z -= cosf(Cam.rotation.y) * 0.1f;
    }


    // if(absf(Cam.rotation.y) > PI * 2.0f )
    // {
    //     Cam.rotation.y = 0;
    // }

    // if(absf(Cam.rotation.x) > PI * 2.0f)
    // {
    //     Cam.rotation.x = 0;
    // }

    // if(absf(Cam.rotation.z) > PI * 2.0f)
    // {
    //     Cam.rotation.z = 0;
    // }
}

void start()
{
    std :: cout << Cam.rotation.x << std :: endl;
    std :: cout << Cam.rotation.y << std :: endl;
    std :: cout << Cam.rotation.z << std :: endl;

    float3 points[8] = 
    {
        (float3) { -1.0f, -1.0f, 1.0f },
        (float3) {  1.0f, -1.0f, 1.0f },
        (float3) {  1.0f,  1.0f, 1.0f },
        (float3) { -1.0f,  1.0f, 1.0f },
        
        (float3) { -1.0f, -1.0f, 2.0f },
        (float3) {  1.0f, -1.0f, 2.0f },
        (float3) {  1.0f,  1.0f, 2.0f },
        (float3) { -1.0f,  1.0f, 2.0f },
    };


    int joints[12][2] = 
    {
        {0, 1},
        {1, 2},
        {2, 3},
        {3, 0},

        {4, 5},
        {5, 6},
        {6, 7},
        {7, 4},

        {0, 4},
        {1, 5},
        {2, 6},
        {3, 7},
    };

    wireframe.Init(points, joints, 8, 12);

    // std :: exit(0);
}