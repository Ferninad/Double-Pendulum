#include "common.h"
#include "cmath"
#include "vector"
#include "string"

bool Init();
void CleanUp();
void Run();
void Draw();
void DrawCircle(SDL_Point center, int radius);

SDL_Window *window;
SDL_GLContext glContext;
SDL_Surface *gScreenSurface = nullptr;
SDL_Renderer *renderer = nullptr;
SDL_Rect pos;

int screenWidth = 500;
int screenHeight = 500;
int xoff = 250;
int yoff = 100;
double timestep = 1;
double g = 9.8 / 60;

vector<vector<double>> Pendulums;
vector<vector<int>> trace;

bool Init()
{
    if (SDL_Init(SDL_INIT_NOPARACHUTE & SDL_INIT_EVERYTHING) != 0)
    {
        SDL_Log("Unable to initialize SDL: %s\n", SDL_GetError());
        return false;
    }
    else
    {
        //Specify OpenGL Version (4.2)
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_Log("SDL Initialised");
    }

    //Create Window Instance
    window = SDL_CreateWindow(
        "Game Engine",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        screenWidth,
        screenHeight,   
        SDL_WINDOW_OPENGL);

    //Check that the window was succesfully created
    if (window == NULL)
    {
        //Print error, if null
        printf("Could not create window: %s\n", SDL_GetError());
        return false;
    }
    else{
        gScreenSurface = SDL_GetWindowSurface(window);
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        SDL_Log("Window Successful Generated");
    }
    //Map OpenGL Context to Window
    glContext = SDL_GL_CreateContext(window);

    return true;
}

int main()
{
    //Error Checking/Initialisation
    if (!Init())
    {
        printf("Failed to Initialize");
        return -1;
    }

    // Clear buffer with black background
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    //Swap Render Buffers
    SDL_GL_SwapWindow(window);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    Run();

    CleanUp();
    return 0;
}

void CleanUp()
{
    //Free up resources
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Run()
{
    bool gameLoop = true;
    srand(time(NULL));

    Pendulums.push_back({100, 50, M_PI/2, 0}); //length, mass, angle, vel
    Pendulums.push_back({150, 50, M_PI/2, 0}); //length, mass, angle, vel

    double x1 = Pendulums[0][0] * sin(Pendulums[0][2]);
    double y1 = Pendulums[0][0] * cos(Pendulums[0][2]);
    SDL_Point p1 = {static_cast<int>(round(x1)) + xoff, static_cast<int>(round(y1)) + yoff};

    double x2 = x1 + Pendulums[1][0] * sin(Pendulums[1][2]);
    double y2 = y1 + Pendulums[1][0] * cos(Pendulums[1][2]);
    SDL_Point p2 = {static_cast<int>(round(x2)) + xoff, static_cast<int>(round(y2)) + yoff};
    trace.push_back({p2.x, p2.y});
    int time = clock();
    
    while (gameLoop)
    {   
        int ctime = clock();
        
        if(ctime - time > 17){
            pos.x = 0;
            pos.y = 0;
            pos.w = screenWidth;
            pos.h = screenHeight;
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderFillRect(renderer, &pos);
            time = clock();

            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            Draw();
            SDL_RenderPresent(renderer);
        }

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                gameLoop = false;
            }
            if (event.type == SDL_KEYDOWN)
            {
                switch (event.key.keysym.sym){
                    case SDLK_ESCAPE:
                        gameLoop = false;
                        break;
                    default:
                        break;
                }
            }

            if (event.type == SDL_KEYUP)
            {
                switch (event.key.keysym.sym){
                    default:
                        break;
                }
            }
        }
    }
}

void Draw(){ //length, mass, angle, velocity
    double m1 = Pendulums[0][1];
    double m2 = Pendulums[1][1];
    double a1 = Pendulums[0][2];
    double a2 = Pendulums[1][2];
    double v1 = Pendulums[0][3];
    double v2 = Pendulums[1][3];
    double r1 = Pendulums[0][0];
    double r2 = Pendulums[1][0];

    double anga1 = -1*g*(2*m1 + m2)*sin(a1);
    anga1 -= m2*g*sin(a1 - 2*a2);
    anga1 -= 2*sin(a1 - a2)*m2*(pow(v2, 2)*r2 + pow(v1, 2)*r1*cos(a1 - a2));
    anga1 /= r1*(2*m1 + m2 - m2*cos(2*a1 - 2*a2));
    
    double anga2 = 2*sin(a1 - a2);
    anga2 *= pow(v1, 2)*r1*(m1 + m2) + g*(m1 + m2)*cos(a1) + pow(v2, 2)*r2*m2*cos(a1 - a2);
    anga2 /= r2*(2*m1 + m2 - m2*cos(2*a1 - 2*a2));

    Pendulums[0][3] += anga1;
    Pendulums[1][3] += anga2;
    Pendulums[0][2] += Pendulums[0][3] * timestep * 1.01;
    Pendulums[1][2] += Pendulums[1][3] * timestep * 1.01;

    double x1 = Pendulums[0][0] * sin(Pendulums[0][2]);
    double y1 = Pendulums[0][0] * cos(Pendulums[0][2]);
    SDL_Point p1 = {static_cast<int>(round(x1)) + xoff, static_cast<int>(round(y1)) + yoff};

    double x2 = x1 + Pendulums[1][0] * sin(Pendulums[1][2]);
    double y2 = y1 + Pendulums[1][0] * cos(Pendulums[1][2]);
    SDL_Point p2 = {static_cast<int>(round(x2)) + xoff, static_cast<int>(round(y2)) + yoff};
    trace.push_back({p2.x, p2.y});

    DrawCircle(p1, 10);
    DrawCircle(p2, 10);

    SDL_RenderDrawLine(renderer, static_cast<int>(round(x1)) + xoff, static_cast<int>(round(y1)) + yoff, 0 + xoff, 0 + yoff);
    SDL_RenderDrawLine(renderer, static_cast<int>(round(x2)) + xoff, static_cast<int>(round(y2)) + yoff, static_cast<int>(round(x1)) + xoff, static_cast<int>(round(y1)) + yoff);
    
    SDL_RenderDrawLine(renderer, trace[0][0], trace[0][1], trace[1][0], trace[1][1]);
    if(trace.size() > 2){
        for(int i = 2; i < trace.size(); i++){
            SDL_RenderDrawLine(renderer, trace[i][0], trace[i][1], trace[i-1][0], trace[i-1][1]);
        }
    }
    if(trace.size() > 1000){
        trace.erase(trace.begin());
    }
}

void DrawCircle(SDL_Point center, int radius){
    double x = center.x - radius;
    double y = center.y;
    if(x < 0)
        x = 0;
    double endx = center.x + radius;
    if(endx >= screenWidth)
        endx = screenWidth - 1;
    for(x; x < endx; x+=1){
        double h = sqrt((radius + x - center.x) * (radius - x + center.x)) + center.y;
        pos.x = round(x);
        pos.y = y - (round(h) - y);
        pos.w = 1;
        pos.h = (round(h) - y) * 2;
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderFillRect(renderer, &pos);
    }
}