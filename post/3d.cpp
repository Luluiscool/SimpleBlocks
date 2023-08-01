#define LIB_NAME "INTERNAL"


#define SPEED 0.1f
#define SENSITIVITY 0.001f
#define MAX_OBJ 10
#define FRICTION 0.8333f


struct GameCamera
{
    float3 position = (float3) { 0.0, 0.0, -10.0f };
    float2 rotation = (float2) { 0.0f, PI * 0.5f };
    float brightness = 1.0f;

    Camera3D RayCam;

    void init()
    {
        RayCam.fovy = 60;
        RayCam.projection = CAMERA_PERSPECTIVE;
        RayCam.position = position.V();
        RayCam.up = (Vector3) { 0.0f, 1.0f, 0.0f };
    }

    void update()
    {
        RayCam.position = position.V();
        RayCam.target = (Vector3) { position.x + cosf(rotation.y), position.y + sinf(rotation.x), position.z + sinf(rotation.y) };
    }
};

struct GameModel
{
    Model model;
    Shader shader;
    float3 position;
    float scale;
};

struct OBJMeshes
{
    int length;
    GameModel models[MAX_OBJ];
    GameCamera* Cam;

    void Load(std :: string uri[MAX_OBJ], float3 position[MAX_OBJ], float scale[MAX_OBJ], std :: string shaders[MAX_OBJ], std :: string vertex[MAX_OBJ], int _length)
    {
        length = _length;

        for(int i = 0; i < length; i ++)
        {
            models[i].model = LoadModel(uri[i].c_str());
            models[i].position = position[i];
            models[i].scale = scale[i];
            models[i].shader = LoadShader(TextFormat(vertex[i].c_str(), GLSL_VERSION), TextFormat(shaders[i].c_str(), GLSL_VERSION));

            int lightPosLocation = GetShaderLocation(models[i].shader, "lightPos");

            float light[3] = { 0.0f, 2.0f, -2.0f };

            SetShaderValue(models[i].shader, lightPosLocation, light, SHADER_UNIFORM_VEC3);
            
            int lightColLocation = GetShaderLocation(models[i].shader, "lightCol");

            float lightCol[3] = { 1.0f, 1.0f, 1.0f };

            SetShaderValue(models[i].shader, lightColLocation, lightCol, SHADER_UNIFORM_VEC3);
            
            int viewPosLocation = GetShaderLocation(models[i].shader, "viewPos");

            float viewPos[3] = { Cam -> position.x, Cam -> position.y, Cam -> position.z };

            SetShaderValue(models[i].shader, viewPosLocation, viewPos, SHADER_UNIFORM_VEC3);
            
            models[i].model.materials[1].shader = models[i].shader;
        }
    }

    void Render()
    {
        for(int i = 0; i < length; i++)
        {
            DrawModel(models[i].model, models[i].position.V(), models[i].scale, WHITE);
        }
    }
};

struct Flight
{
    float3 position;
    float3 acceleration;
    float3 oldPosition;
    float2 angle = (float2) { 0.0f, PI * 0.5f};
    
    void update(float dt = 1.0f)
    {
        const float3 velocity = position - oldPosition;

        oldPosition = position;

        position = position + velocity * FRICTION + acceleration * dt * dt;

        acceleration = (float3) { 0 };
    }

    void SetPosition(float x, float y, float z)
    {
        position.x = x;
        position.y = y;
        position.z = z;

        oldPosition = position;
    }

    void SetPosition(float3 v)
    {
        position = v;
        oldPosition = v;
    }
};




GameCamera cam;
OBJMeshes meshes;
Flight plane;
GameModel* PlaneMeshAddr;


int brightnessLocation[MAX_OBJ];


void start()
{
    cam.init();
    cam.update();
    std :: cout << cam.RayCam.target.x << std :: endl;
    std :: cout << cam.RayCam.target.y << std :: endl;
    std :: cout << cam.RayCam.target.z << std :: endl;

    plane.SetPosition(cam.position + (float3) { 0.0f, 2.0f, 2.0f });

    std :: string uri[MAX_OBJ] = 
    {
        "resources/level.gltf",
        "resources/scene2.gltf",
        "resources/box.gltf",
    };

    float3 position[MAX_OBJ] = 
    {
        (float3){ 0.0f, 2.0f, 0.0f },
        (float3){ 2.0f, 5.0f, 0.0f },
        plane.position
    };

    float scale[MAX_OBJ] = 
    {
        3.0f,
        1.0f,
        1.0f,
    };

    std :: string vertex[MAX_OBJ] = 
    {
        "resources/basic.vs",
        "resources/basic.vs",
        "resources/basic.vs",
    };

    std :: string shaders[MAX_OBJ] = 
    {
        "resources/basic.shader",
        "resources/basic.shader",
        "resources/basic.shader",
    };

    meshes.Cam = &cam;

    meshes.Load(uri, position, scale, shaders, vertex, 3);
    PlaneMeshAddr = &meshes.models[2];

    for(int i = 0; i < meshes.length; i ++)
    {
        brightnessLocation[i] = GetShaderLocation(meshes.models[i].shader, "brightness");
    }
}

void Loop(int Tick)
{
    cam.rotation.y += GetMouseDelta().x * SENSITIVITY;
    cam.rotation.x -= GetMouseDelta().y * SENSITIVITY;

    if(cam.rotation.x > PI * 0.5f) cam.rotation.x = PI * 0.5f;
    if(cam.rotation.x < -PI * 0.5f) cam.rotation.x = -PI * 0.5f;

    if(IsKeyDown(KEY_W))
    {
        cam.position.x += cosf(cam.rotation.y) * SPEED;
        cam.position.z += sinf(cam.rotation.y) * SPEED;
    }

    if(IsKeyDown(KEY_S))
    {
        cam.position.x -= cosf(cam.rotation.y) * SPEED;
        cam.position.z -= sinf(cam.rotation.y) * SPEED;
    }

    if(IsKeyDown(KEY_A))
    {
        cam.position.x += cosf(cam.rotation.y - (PI * 0.5f)) * SPEED;
        cam.position.z += sinf(cam.rotation.y - (PI * 0.5f)) * SPEED;
    }

    if(IsKeyDown(KEY_D))
    {
        cam.position.x += cosf(cam.rotation.y + (PI * 0.5f)) * SPEED;
        cam.position.z += sinf(cam.rotation.y + (PI * 0.5f)) * SPEED;
    }

    if(IsKeyDown(KEY_E))
    {
        cam.position.y += SPEED;
    }

    if(IsKeyDown(KEY_Q))
    {
        cam.position.y -= SPEED;
    }

    if(IsKeyDown(KEY_UP))
    {
        cam.brightness += 0.05;
    }

    if(IsKeyDown(KEY_DOWN))
    {
        cam.brightness -= 0.05;
    }
    cam.update();
    
    cam.position = plane.position - (float3) { 0.0f, -1.0f, 5.0f };
    PlaneMeshAddr -> position = plane.position;

    // for(int i = 0; i < meshes.length; i ++)
    // {
    //     SetShaderValue(meshes.models[i].shader, brightnessLocation[i], &cam.brightness, SHADER_UNIFORM_FLOAT);
    // }

    plane.acceleration += (float3) { cosf(plane.angle.y), sinf(plane.angle.x), sinf(plane.angle.y) } * 0.01f;
    plane.update();
}


void Render3D()
{
    DrawTriangle3D((Vector3) {1, 1, 0}, (Vector3) {0, 1, 1}, (Vector3) {0, 1, 0}, WHITE);
    DrawSphere((Vector3) { 0.0f, 2.0f, -2.0f }, 0.5, WHITE);
    meshes.Render();
}

void Render()
{
    DrawText(std :: to_string(cam.position.x).c_str(), 30, 60, 20, RED);
    DrawText(std :: to_string(cam.position.y).c_str(), 30, 90, 20, YELLOW);
    DrawText(std :: to_string(cam.position.z).c_str(), 30, 120, 20, BLUE);

    DrawText(std :: to_string(cam.rotation.x).c_str(), 30, 150, 20, SKYBLUE);
    DrawText(std :: to_string(cam.rotation.y).c_str(), 30, 180, 20, LIME);
    DrawText(std :: to_string(cam.brightness).c_str(), 30, 210, 20, ORANGE);
}