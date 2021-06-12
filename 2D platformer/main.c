#include "raylib.h"
#include "raymath.h"

#define G 400
#define PLAYER_JUMP_SPD 350.0f
#define PLAYER_HOR_SPD 200.0f

typedef struct Player {
    Vector2 position;
    float speed;
    bool canJump;
} Player;

typedef struct EnvItem {
    Rectangle rect;
    int blocking;
    Color color;
} EnvItem;
typedef struct Wall {
    Rectangle rect;
    int blocking;
    Color color;
} Wall;


typedef enum {
    STORAGE_POSITION_SCORE      = 0,
    STORAGE_POSITION_HISCORE    = 1
} StorageData;


void UpdatePlayer(Player *player, EnvItem *envItems, int envItemsLength, float delta,Sound jump);
void UpdateCameraPlayerBoundsPush(Camera2D *camera, Player *player, EnvItem *envItems, int envItemsLength, float delta, int width, int height);
void GenratePlatform(EnvItem *envItems,int number,int dis);

int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;
    int hiscore  = LoadStorageValue(STORAGE_POSITION_HISCORE);
    int score = 0;
    bool playState=false;
    bool GameOver = false;
    
    float pitch = 1.0f;
    
    InitWindow(screenWidth, screenHeight, "2D platformer");
    InitAudioDevice();
    
   //Texture import
    Image horizontalGradient = GenImageGradientH(screenWidth, screenHeight, BLACK, BLUE);
    Texture2D textures = LoadTextureFromImage(horizontalGradient);
    Texture2D grass = LoadTexture("resources/grass2.png");
    Texture2D fireWall = LoadTexture("resources/FireWall.png");
    UnloadImage(horizontalGradient);
    
    bool blink =false;
    int blinkframe = 0;
    int framesCounter = 0;
    int framesCounter2=0;
    int framesCounter3=0;
    int currentFrame = 0;
    
    Player player = { 0 };
    player.position = (Vector2){ 400, 280 };
    player.speed = 0;
    player.canJump = false;
   
    EnvItem envItems[100];
    GenratePlatform(envItems, 100,0);   
    Wall  wall={{-50,0,100,5000} ,1, RED}; 
     
    
    int envItemsLength = sizeof(envItems)/sizeof(envItems[0]);
    
    Camera2D camera = { 0 };
    camera.target = player.position;
    camera.offset = (Vector2){ screenWidth/2.0f, screenHeight/2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;
    
    //Sound import
    Music music = LoadMusicStream("resources/mini1111.xm");
    
    music.looping =true;
    PlayMusicStream(music);
    Sound jump = LoadSound("resources/Jump.wav");
    Sound change = LoadSound("resources/change.wav");
    Sound wallHit = LoadSound("resources/burnedByWall.wav");
    
    SetSoundVolume(jump, 0.5f);
    SetSoundVolume(change, 0.6f);
    
     
    
    SetTargetFPS(60);
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())
    {
        // Update
        //----------------------------------------------------------------------------------
        
        UpdateMusicStream(music); 
        SetMusicPitch(music, pitch);
        

        if(playState)
        {    
       float deltaTime = GetFrameTime();
        framesCounter++;
        framesCounter2++;
        framesCounter3++;
        UpdatePlayer(&player, envItems, envItemsLength, deltaTime,jump);
        
        if ((((framesCounter2/GetRandomValue(480,720))%2) == 1)&&!GameOver)
        {   
            PlaySound(change);
             
            blink = true;
            
        }

        
       if ((((framesCounter/60)%2) == 1)&&!GameOver)
        {   score++;
            wall.rect.x+=40;
            framesCounter = 0;
            
        }
        if(player.position.x-wall.rect.x >1000)
        {
            wall.rect.x=player.position.x-550;
        }
        wall.rect.y=player.position.y-1000;
        if (camera.zoom > 3.0f) camera.zoom = 3.0f;
        else if (camera.zoom < 0.25f) camera.zoom = 0.25f;

        if (IsKeyPressed(KEY_R))
        {
            PlaySound(wallHit);
            GameOver = false;
            camera.zoom = 1.0f;
             GenratePlatform(envItems, 100,0); 
            player.position = (Vector2){ 400, 280 };
            wall.rect.x=-50;
            wall.rect.y=0;
            score=0;
            framesCounter=0;
            framesCounter2=0;
            
        }
        
     
       
        UpdateCameraPlayerBoundsPush(&camera, &player, envItems, envItemsLength, deltaTime, screenWidth, screenHeight);
        }
        if(IsKeyPressed(KEY_ENTER))
            playState = true;
        //----------------------------------------------------------------------------------
        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(BLACK);
            
            DrawTexture(textures, 0, 0, WHITE);
            
            BeginMode2D(camera);
            
            if(playState){
                //blinkindg effect
                if(!blink){
                for (int i = 0; i < envItemsLength; i++) 
                {

                    Vector2 pos ={envItems[i].rect.x,envItems[i].rect.y};
                    DrawTextureRec(grass,envItems[i].rect,pos, GRAY);  
                
                }
                }
                else
                {
                    
                    if((((blinkframe/3)%2) == 1)&&!GameOver) {   
                     for (int i = 0; i < envItemsLength; i++) 
                        {
                            
                        Vector2 pos ={envItems[i].rect.x,envItems[i].rect.y};
                        DrawTextureRec(grass,envItems[i].rect,pos, WHITE);  
                        
                        }
                     }
                    
                    DrawText("blink", 30,120 , 20, WHITE); 
                  blinkframe++;
                  
                  if((((blinkframe/60)%2) == 1)&&!GameOver)
                  {
                  
                  blinkframe=0;
                  blink=false;
                  GenratePlatform(envItems, 100,player.position.x-500); //Change Platform position  after certain time
                  framesCounter2=0;
                  }
                }
                
              Vector2 posi = {wall.rect.x, wall.rect.y};
                DrawTextureRec(fireWall,wall.rect,posi, WHITE);
                
                
                
                Rectangle playerRect = { player.position.x - 20, player.position.y - 40, 40, 40 };
                DrawRectangleRec(playerRect, ORANGE);
               
            }
            EndMode2D();

            DrawText("Controls:", 20, 20, 10, BLACK);
            DrawText("- Right/Left to move", 40, 40, 10, DARKGRAY);
            DrawText("- Space to jump", 40, 60, 10, DARKGRAY);
            DrawText("- R to restart", 40, 80, 10, DARKGRAY);
           
           if(playState){ 
            DrawText(TextFormat("Score: %i", score), 300, 10, 30, WHITE);
            
           }
                        
            if(player.position.y>1000||wall.rect.x+125>=player.position.x)
            {
                
                if(score>hiscore){
                    hiscore =score;
                    SaveStorageValue(STORAGE_POSITION_HISCORE, hiscore);
                    DrawText("New Highscore", 350,180 , 20, WHITE);    
                }
                DrawText(TextFormat("Highscore: %i", hiscore), 550, 10, 30, WHITE);    
                DrawText("Game Over", 350,80 , 40, WHITE);
                GameOver =true;
            }
            if(!playState)
            {
                DrawText("Press Enter to Start", 200,200 , 30, WHITE);

            }
            
        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadMusicStream(music);          // Unload music stream buffers from RAM
    UnloadSound(jump);
    UnloadSound(change);
    UnloadSound(wallHit); 
    UnloadTexture(grass);
    UnloadTexture(fireWall);
    
    CloseAudioDevice();
    
    
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}

void UpdatePlayer(Player *player, EnvItem *envItems, int envItemsLength, float delta,Sound jump)
{
    if (IsKeyDown(KEY_LEFT)) player->position.x -= PLAYER_HOR_SPD*delta;
    if (IsKeyDown(KEY_RIGHT)) player->position.x += PLAYER_HOR_SPD*delta;
    if (IsKeyDown(KEY_SPACE) && player->canJump)
    {
        player->speed = -PLAYER_JUMP_SPD;
        player->canJump = false;
        PlaySound(jump);
    }

    int hitObstacle = 0;
    for (int i = 0; i < envItemsLength; i++)
    {
        EnvItem *ei = envItems + i;
        Vector2 *p = &(player->position);
        if (ei->blocking &&
            ei->rect.x <= p->x &&
            ei->rect.x + ei->rect.width >= p->x &&
            ei->rect.y >= p->y &&
            ei->rect.y < p->y + player->speed*delta)
        {
            hitObstacle = 1;
            player->speed = 0.0f;
            p->y = ei->rect.y;
        }
    }

    if (!hitObstacle)
    {
        player->position.y += player->speed*delta;
        player->speed += G*delta;
        player->canJump = false;
    }
    else player->canJump = true;
}


void UpdateCameraPlayerBoundsPush(Camera2D *camera, Player *player, EnvItem *envItems, int envItemsLength, float delta, int width, int height)
{
    static Vector2 bbox = { 0.2f, 0.2f };

    Vector2 bboxWorldMin = GetScreenToWorld2D((Vector2){ (1 - bbox.x)*0.5f*width, (1 - bbox.y)*0.5f*height }, *camera);
    Vector2 bboxWorldMax = GetScreenToWorld2D((Vector2){ (1 + bbox.x)*0.5f*width, (1 + bbox.y)*0.5f*height }, *camera);
    camera->offset = (Vector2){ (1 - bbox.x)*0.5f * width, (1 - bbox.y)*0.5f*height };

    if (player->position.x < bboxWorldMin.x) camera->target.x = player->position.x;
    if (player->position.y < bboxWorldMin.y) camera->target.y = player->position.y;
    if (player->position.x > bboxWorldMax.x) camera->target.x = bboxWorldMin.x + (player->position.x - bboxWorldMax.x);
    if (player->position.y > bboxWorldMax.y) camera->target.y = bboxWorldMin.y + (player->position.y - bboxWorldMax.y);
}



void GenratePlatform(EnvItem *envItems, int number,int dis)
{
 

    Rectangle block1 = {0, 400, 500, 200 };
    envItems[0].rect= block1;
    envItems[0].color=GRAY;
    envItems[0].blocking=1;
    int x=0+dis;
    int y=400;
    for(int i=1;i<number;i++)
    {
    if(i%3==0)
    {
    Rectangle block2 = {x-230,envItems[i-1].rect.y-GetRandomValue(100,250),GetRandomValue(90,210),15};
    envItems[i].rect= block2;
    envItems[i].color=GRAY;
    envItems[i].blocking=1;
    i++;
    }
    if(i%7==0)
    {
    Rectangle block2 = {x-100,envItems[i-1].rect.y+GetRandomValue(150,270),GetRandomValue(90,210),15};
    envItems[i].rect= block2;
    envItems[i].color=GRAY;
    envItems[i].blocking=1;
    i++;
    }    
    Rectangle block2 = {x,y,GetRandomValue(90,250),15};
    envItems[i].rect= block2;
    envItems[i].color=GRAY;
    envItems[i].blocking=1;

    x+=GetRandomValue(150,270);;       
    if(y<150)
        y=GetRandomValue(300,200);
    else if(y>400)
        y=GetRandomValue(380,300);
    else
        y= GetRandomValue(y-150,y+150); 
          
    } 

}

