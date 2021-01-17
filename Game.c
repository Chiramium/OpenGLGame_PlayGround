#include <GL/glut.h>
#include <GL/glpng.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

// ウィンドウサイズの定義
#define WWIDTH 480
#define WHEIGHT 640

#define ENEMIES_MAX 5

enum MODE { TITLE, MANUAL, SETTING, RUN, PAUSE, GAMEOVER, CLEAR, FADEIN };
enum ETYPE { CIRCLE, CUBE, TRI, SHOT, BOSS };
enum BTYPE { BEAM, DOT, LINE };
enum DIRECTION { RIGHT, LEFT, STOP };

typedef struct PLAYER
{
  int x;
  int y;
  int shot;
  int speed;
  int life;
  int collision;
  int bomb;
  int graze;
} PLAYER;

typedef struct ENEMY
{
  int x;
  int y;
  int rx;
  int ry;
  int shot;
  int speed;
  int life;
  int direction;
  int type;
} ENEMY;

typedef struct e_Node
{
  ENEMY enemy;
  struct e_Node *prev;
  struct e_Node *next;
} enemy_Node;

typedef struct BULLET
{
  int x;
  int y;
  int dx;
  int dy;
  enum BTYPE type;
} BULLET;

typedef struct b_Node
{
  BULLET bullet;
  struct b_Node *prev;
  struct b_Node *next;
} bullet_Node;

GLuint img_pl, img_en[3], img_en_shot, img_en_bullet,  img_boss, img_bg, img_fl, img_title, img_pause, img_manual, img_gmovr, img_clr, img_start, img_gomanual, img_resume, img_quit, img_gotitle;
GLuint img_num[10];
pngInfo info_pl, info_en[3], info_en_shot, info_en_bullet, info_boss, info_bg, info_fl, info_title, info_pause, info_manual, info_gmovr, info_clr, info_start, info_gomanual, info_resume, info_quit, info_gotitle;
pngInfo info_num[10] = {0};
enum MODE mode = TITLE;
enum MODE nextMode;
int col = 0;
int score = 0;
char str[32];
int plx = 216, ply = 452;
int direction[4] = {0}; // 0:up, 1:right, 2:down, 3:left
int menu_select = 0;
int enemiesCnt = 0;
int killedEnemies = 0;
int fade = 255;
int bg1_pos = 0, bg2_pos = 0;
struct PLAYER player;
enemy_Node* enemies;
bullet_Node* bullets;

// Prototype
void Reshape(int, int);

void ImportImages();
void Initialize();

void Transition(int value);

enemy_Node *AddEnemy(enemy_Node* enemies, int x, int y, int rx, int ry, int shot, int speed, int life, int direction, enum ETYPE type);
void FreeEnemy(enemy_Node* enemy);
bullet_Node *AddBullet(bullet_Node* bullets, int x, int y, int dx, int dy, enum BTYPE type);
void FreeBullet(bullet_Node* bullet);
void MovePlayer();
void MoveEnemy();
void MoveBullet();
void ShotEnemyBullet(int value);
void isCollided(void);
void isEnemyCollided(void);

void Title(void);
void Manual(void);
void Run(void);
void Pause(void);
void GameOver(void);
void Clear(void);
void FadeIn(void);
void FadeOut(void);

void Display(void);
void PrintText(int x, int y, char *s);

void Mouse(int, int, int, int);
void PassiveMotion(int, int);
void Motion(int, int);
void Entry(int);
void Keyboard(unsigned char, int, int);
void KeyboardUp(unsigned char, int, int);
void SpecialKey(int, int, int);
void SpecialKeyUp(int, int, int);

void PutSprite(int num, int x, int y, pngInfo *info, int r, int g, int b, int a);
void PutImgNum(int x, int y, char str, int r, int g, int b, int a);
void PutImgNumbers(int x, int y, char *s, int r, int g, int b, int a);


int main(int argc, char **argv)
{
  int scrWidth = 0, scrHeight = 0; // 画面全体のサイズ

  // ウィンドウ初期化, 乱数初期化
  srandom((unsigned int)time(NULL));
  glutInit(&argc, argv);
  scrWidth = glutGet(GLUT_SCREEN_WIDTH);
  scrHeight = glutGet(GLUT_SCREEN_HEIGHT);
  glutInitWindowSize(WWIDTH, WHEIGHT);
  glutCreateWindow("Game [Test] ---DEBUG MODE---");
  glutPositionWindow((scrWidth - WWIDTH) / 2, (scrHeight - WHEIGHT) / 2);
  glutInitDisplayMode(GLUT_RGBA | GLUT_ALPHA);
  glClearColor(1.0, 1.0, 1.0, 1.0);
  Initialize();

  // アルファチャネルの有効化
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

  ImportImages();

  // コールバック関数の登録
  glutReshapeFunc(Reshape);
  glutTimerFunc(20, Transition, 0);
  glutTimerFunc(100, ShotEnemyBullet, 0);
  glutDisplayFunc(Display);
  glutPassiveMotionFunc(PassiveMotion);
  glutMouseFunc(Mouse);
  glutKeyboardFunc(Keyboard);
  glutKeyboardUpFunc(KeyboardUp);
  glutSpecialFunc(SpecialKey);
  glutSpecialUpFunc(SpecialKeyUp);

  // イベントループ突入
  glutMainLoop();

  return 0;
}

void Reshape(int w, int h)
{
  // ウィンドウサイズ固定
  glutReshapeWindow(WWIDTH, WHEIGHT);

  // 座標系の再設定
  glViewport(0, 0, w, h);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluOrtho2D(0, w, 0, h);
  glScaled(1, -1, 1);
  glTranslated(0, -h, 0);
}

void ImportImages()
{
  int i;

  sprintf(str, "./images/player/player.png");
  img_pl = pngBind(str, PNG_NOMIPMAP, PNG_ALPHA, &info_pl, GL_CLAMP, GL_NEAREST, GL_NEAREST);

  for (i = 0; i < 3; i++) {
    sprintf(str, "./images/enemy/enemy%d.png", i);
    img_en[i] = pngBind(str, PNG_NOMIPMAP, PNG_ALPHA, &info_en[i], GL_CLAMP, GL_NEAREST, GL_NEAREST);
  }

  sprintf(str, "./images/enemy/enemy_shot.png");
  img_en_shot = pngBind(str, PNG_NOMIPMAP, PNG_ALPHA, &info_en_shot, GL_CLAMP, GL_NEAREST, GL_NEAREST);

  sprintf(str, "./images/enemy/dot.png");
  img_en_bullet = pngBind(str, PNG_NOMIPMAP, PNG_ALPHA, &info_en_bullet, GL_CLAMP, GL_NEAREST, GL_NEAREST);

  sprintf(str, "./images/enemy/boss.png");
  img_boss = pngBind(str, PNG_NOMIPMAP, PNG_ALPHA, &info_boss, GL_CLAMP, GL_NEAREST, GL_NEAREST);

  sprintf(str, "./images/scr/background.png");
  img_bg = pngBind(str, PNG_NOMIPMAP, PNG_ALPHA, &info_bg, GL_CLAMP, GL_NEAREST, GL_NEAREST);

  sprintf(str, "./images/scr/field.png");
  img_fl = pngBind(str, PNG_NOMIPMAP, PNG_ALPHA, &info_fl, GL_CLAMP, GL_NEAREST, GL_NEAREST);

  sprintf(str, "./images/scr/title.png");
  img_title = pngBind(str, PNG_NOMIPMAP, PNG_ALPHA, &info_title, GL_CLAMP, GL_NEAREST, GL_NEAREST);

  sprintf(str, "./images/scr/pause.png");
  img_pause = pngBind(str, PNG_NOMIPMAP, PNG_ALPHA, &info_pause, GL_CLAMP, GL_NEAREST, GL_NEAREST);

  sprintf(str, "./images/scr/manual.png");
  img_manual = pngBind(str, PNG_NOMIPMAP, PNG_ALPHA, &info_manual, GL_CLAMP, GL_NEAREST, GL_NEAREST);

  sprintf(str, "./images/scr/gameover.png");
  img_gmovr = pngBind(str, PNG_NOMIPMAP, PNG_ALPHA, &info_gmovr, GL_CLAMP, GL_NEAREST, GL_NEAREST);

  sprintf(str, "./images/scr/clear.png");
  img_clr = pngBind(str, PNG_NOMIPMAP, PNG_ALPHA, &info_clr, GL_CLAMP, GL_NEAREST, GL_NEAREST);

  sprintf(str, "./images/menu/start.png");
  img_start = pngBind(str, PNG_NOMIPMAP, PNG_ALPHA, &info_start, GL_CLAMP, GL_NEAREST, GL_NEAREST);

  sprintf(str, "./images/menu/gomanual.png");
  img_gomanual = pngBind(str, PNG_NOMIPMAP, PNG_ALPHA, &info_gomanual, GL_CLAMP, GL_NEAREST, GL_NEAREST);

  sprintf(str, "./images/menu/resume.png");
  img_resume = pngBind(str, PNG_NOMIPMAP, PNG_ALPHA, &info_resume, GL_CLAMP, GL_NEAREST, GL_NEAREST);

  sprintf(str, "./images/menu/quit.png");
  img_quit = pngBind(str, PNG_NOMIPMAP, PNG_ALPHA, &info_quit, GL_CLAMP, GL_NEAREST, GL_NEAREST);

  sprintf(str, "./images/menu/gotitle.png");
  img_gotitle = pngBind(str, PNG_NOMIPMAP, PNG_ALPHA, &info_gotitle, GL_CLAMP, GL_NEAREST, GL_NEAREST);



  // テキストに対応する画像の読み込み
  for (i = 0; i < 10; i++) {
    sprintf(str, "./fonts/%d.png", i); // ファイル名はすべてASCIIコードに対応
    img_num[i] = pngBind(str, PNG_NOMIPMAP, PNG_ALPHA, &info_num[i], GL_CLAMP, GL_NEAREST, GL_NEAREST);
    printf("if = %d, W=%d, H=%d, D=%d, A=%d\n", img_num[i], info_num[i].Width, info_num[i].Height, info_num[i].Depth, info_num[i].Alpha);
  }
}

void Initialize()
{
  int i;

  bg1_pos = 0;
  bg2_pos= -960;

  player.x = 240;
  player.y = 476;
  player.speed = 12;
  player.collision = 0;
  player.life = 100;
  player.shot = 0;
  player.graze = 0;

  enemies = NULL;
  for (i = 0; i < ENEMIES_MAX-1; i++) {
    enemies = AddEnemy(enemies, random() % 368 + 40, (random() % 100), 0, 0, 0, (random() % 4 + 1), 20, random() % 2, random() % 4);
  }
  enemiesCnt = i;

  enemies = AddEnemy(enemies, 100, 300, 0, 0, 0, 0, 1000, STOP, CUBE);

  bullets = NULL;

  for (i = 0; i < 4; i++) {
    direction[i] = 0;
  }

  score = 0;
  killedEnemies = 0;
}

void Transition(int value)
{
  //printf("modeA = %d\n", mode);

  switch (mode) {
  case TITLE:
    Title();
    break;

  case MANUAL:
    Manual();
    break;

  case RUN:
    Run();
    break;

  case PAUSE:
    Pause();
    break;

  case GAMEOVER:
    GameOver();
    break;

  case CLEAR:
    Clear();
    break;

  case FADEIN:
    FadeIn();
    break;

  default:
    printf("mode error\n");
    mode = TITLE;
    break;
  }

  // Display関数を強制呼び出し
  glutPostRedisplay();

  glutTimerFunc(20, Transition, 0);
}

enemy_Node *AddEnemy(enemy_Node* enemies, int x, int y, int rx, int ry, int shot, int speed, int life, int direction, enum ETYPE type)
{
  // 新しいノードの作成
  enemy_Node *node;
  node = (enemy_Node *)malloc(sizeof(enemy_Node));
  if (type == CUBE || type == TRI || type == SHOT) {
    if (direction == LEFT) {
      node->enemy.x = 420;
    }
    else if (direction == RIGHT) {
      node->enemy.x = -10;
    }
    else {
      node->enemy.x = x;
    }
    node->enemy.y = 0;
  }
  else if (type == CIRCLE) {
    node->enemy.x = x;
    node->enemy.y = 0;
  }
  else {
    node->enemy.x = x;
    node->enemy.y = y;
  }
  node->enemy.rx = rx;
  node->enemy.ry = ry;
  node->enemy.shot = shot;
  node->enemy.speed = speed;
  node->enemy.life = life;
  node->enemy.direction = direction;
  node->enemy.type = type;

  printf("x=%d, y=%d, rx=%d, ry=%d, shot=%d, speed=%d, life=%d, direction=%d, type=%d\n", x, y, rx, ry, shot, speed, life, direction, type);

  // 次の要素をNULLに指定
  node->next = NULL;

  if (enemies == NULL) {  // リストが空の場合
    node->prev = NULL;
    return node;  // 登録したノードを返す
  }
  else {
    // リストの末尾のノードを探す
    enemy_Node *p = enemies;
    while (p->next != NULL) {
        p = p->next;
    }
    // 前のノードと登録したノードを連結
    p->next = node; // 前のノードの次を示す変数に登録したノードのポインタを渡す
    node->prev = p; // 登録したノードの前を示す変数に前のノードを渡す

    return enemies;
  }
}

void FreeEnemy(enemy_Node* enemy)
{
  if (enemy->next == NULL) { // 末尾のノードを消去するとき
    if (enemy->prev == NULL) { // 前のノードも存在しないとき
      enemies = NULL; // リストの参照をNULLに戻す
    }
    else {
      enemy->prev->next = NULL; // 前のノードの次を示す変数にNULLを渡す
    }
    free(enemy); // ノードを消去
  }
  else {
    if (enemy->prev == NULL) {
      enemy->next->prev = NULL;
      enemies = enemy->next;
      free(enemy); // ノードを消去
    }
    else {
      enemy->prev->next = enemy->next; // 前のノードの次を示す変数に消去するノードの次に登録されているノードのポインタを渡す
      enemy->next->prev = enemy->prev;
      free(enemy); // ノードを消去
    }
  }
}

bullet_Node *AddBullet(bullet_Node* bullets, int x, int y, int dx, int dy, enum BTYPE type)
{
  // 新しいノードの作成
  bullet_Node *node;
  node = (bullet_Node *)malloc(sizeof(bullet_Node));
  node->bullet.x = x;
  node->bullet.y = y;
  node->bullet.dx = dx;
  node->bullet.dy = dy;
  node->bullet.type = type;

  // 次の要素をNULLに指定
  node->next = NULL;

  if (bullets == NULL) {  // リストが空の場合
    node->prev = NULL;
    return node;  // 登録したノードを返す
  }
  else {
    // リストの末尾のノードを探す
    bullet_Node *p = bullets;
    while (p->next != NULL) {
        p = p->next;
    }
    // 前のノードと登録したノードを連結
    p->next = node; // 前のノードの次を示す変数に登録したノードのポインタを渡す
    node->prev = p; // 登録したノードの前を示す変数に前のノードを渡す

    return bullets;
  }
}

void FreeBullet(bullet_Node* bullet)
{
  if (bullet->next == NULL) { // 末尾のノードを消去するとき
    if (bullet->prev == NULL) { // 前のノードも存在しないとき
      bullets = NULL; // リストの参照をNULLに戻す
    }
    else {
      bullet->prev->next = NULL; // 前のノードの次を示す変数にNULLを渡す
    }
    free(bullet); // ノードを消去
  }
  else {
    if (bullet->prev == NULL) {
      bullet->next->prev = NULL;
      bullets = bullet->next;
      free(bullet); // ノードを消去
    }
    else {
      bullet->prev->next = bullet->next; // 前のノードの次を示す変数に消去するノードの次に登録されているノードのポインタを渡す
      bullet->next->prev = bullet->prev;
      free(bullet); // ノードを消去
    }
  }
}

void MovePlayer()
{
  if (direction[0] == 1) {
    if (player.y > 64) {
      player.y -= player.speed;
    }
    else {
      player.y = 64;
    }
  }
  if (direction[1] == 1) {
    if (player.x < 416) {
      player.x += player.speed;
    }
    else {
      player.x = 416;
    }
  }
  if (direction[2] == 1) {
    if (player.y < 476) {
      player.y += player.speed;
    }
    else {
      player.y = 476;
    }
  }
  if (direction[3] == 1) {
    if (player.x > 64) {
      player.x -= player.speed;
    }
    else {
      player.x = 64;
    }
  }
}

void MoveEnemy()
{
  enemy_Node *p = enemies;
  enemy_Node *temp;

  while (p != NULL) {
    temp = p->next;
    //p->enemy.x += p->enemy.speed;
    if ((p->enemy.x < -20 || p->enemy.x > 440) || (p->enemy.y < -20 || p->enemy.y > 500) || (p->enemy.life <= 0)) {
      enemiesCnt--;
      FreeEnemy(p);
    }
    else {
      if (p->enemy.type == CIRCLE) {
        p->enemy.y += p->enemy.speed;
        p->enemy.ry += p->enemy.speed;
        if (p->enemy.ry >= 100) {
          bullets = AddBullet(bullets, p->enemy.x+24, p->enemy.y+32, 0, 3, DOT);
          p->enemy.ry = 0;
        }
      }
      else if (p->enemy.type == CUBE || p->enemy.type == SHOT) {
        if (p->enemy.direction == LEFT) {
          p->enemy.x -= p->enemy.speed/2 + 1;
          p->enemy.y += p->enemy.speed;
          p->enemy.rx += p->enemy.speed/2 + 1;
          p->enemy.ry += p->enemy.speed;
        }
        else if (p->enemy.direction == RIGHT) {
          p->enemy.x += p->enemy.speed/2 + 1;
          p->enemy.y += p->enemy.speed;
          p->enemy.rx += p->enemy.speed/2 + 1;
          p->enemy.ry += p->enemy.speed;
        }
        if (p->enemy.type == SHOT) {
          p->enemy.shot = 1;
        }
        else {
          if (p->enemy.ry >= 100) {
            bullets = AddBullet(bullets, p->enemy.x+24, p->enemy.y+32, 0, 3, DOT);
            p->enemy.ry = 0;
          }
        }
      }
      else if (p->enemy.type == TRI) {
        if (p->enemy.direction == LEFT) {
          p->enemy.x -= p->enemy.speed;
          p->enemy.y += p->enemy.speed;
          p->enemy.rx += p->enemy.speed;
          p->enemy.ry += p->enemy.speed;
          if (p->enemy.ry >= 50) {
            bullets = AddBullet(bullets, p->enemy.x+24, p->enemy.y+32, 0, 3, DOT);
            p->enemy.ry = 0;
          }
          if (p->enemy.rx >= 200) {
            p->enemy.direction = RIGHT;
            p->enemy.rx = 0;
          }
        }
        else if (p->enemy.direction == RIGHT) {
          p->enemy.x += p->enemy.speed;
          p->enemy.y += p->enemy.speed;
          p->enemy.rx += p->enemy.speed;
          p->enemy.ry += p->enemy.speed;
          if (p->enemy.ry >= 50) {
            bullets = AddBullet(bullets, p->enemy.x+24, p->enemy.y+32, 0, 3, DOT);
            p->enemy.ry = 0;
          }
          if (p->enemy.rx >= 200) {
            p->enemy.direction = LEFT;
            p->enemy.rx = 0;
          }
        }
        else {
          p->enemy.y += p->enemy.speed;
          p->enemy.ry += p->enemy.speed;
        }
      }
    }
    p = temp;
  }
}

void MoveBullet()
{
  bullet_Node *p = bullets;
  bullet_Node *temp;

  while (p != NULL) {
    temp = p->next;
    p->bullet.x += p->bullet.dx;
    if (p->bullet.y > 0 && p->bullet.y <= 500) {
      p->bullet.y += p->bullet.dy;
    }
    else {
      FreeBullet(p);
    }
    p = temp;
  }
}

void ShotEnemyBullet(int value)
{
  enemy_Node *ep = enemies;

  while (ep != NULL) {
    if (ep->enemy.shot == 1) {
      bullets = AddBullet(bullets, ep->enemy.x+20, ep->enemy.y+48, (random()%5)-2, (random()%3)+1, DOT);
    }
    ep = ep->next;
  }


  glutTimerFunc(100, ShotEnemyBullet, 0);
}

void isCollided(void)
{
  int cflag = 0;
  enemy_Node *ep = enemies;
  bullet_Node *bp = bullets;
  bullet_Node *b_temp;

  while (ep != NULL) {
    if (ep->enemy.type == SHOT) {
      if (((player.x-3 >= ep->enemy.x+8 && player.x-3 <= ep->enemy.x+40) || (player.x+3 >= ep->enemy.x+8 && player.x+3 <= ep->enemy.x+40)) && ((player.y-3 >= ep->enemy.y+8 && player.y-3 <= ep->enemy.y+40) || (player.y+3 >= ep->enemy.y+8 && player.y+3 <= ep->enemy.y+40))) {
        cflag = 1;
      }
      else if ((player.x-3 >= ep->enemy.x && player.x-3 <= ep->enemy.x+48) || (player.x+3 >= ep->enemy.x && player.x+3 <= ep->enemy.x+48)) {
        if ((player.y-3 >= ep->enemy.y && player.y-3 <= ep->enemy.y+48) || (player.y+3 >= ep->enemy.y && player.y+3 <= ep->enemy.y+48)) {
          player.graze++;
          score += 1200;
        }
      }
    }
    else {
      if (((player.x-3 >= ep->enemy.x+8 && player.x-3 <= ep->enemy.x+24) || (player.x+3 >= ep->enemy.x+8 && player.x+3 <= ep->enemy.x+24)) && ((player.y-3 >= ep->enemy.y+8 && player.y-3 <= ep->enemy.y+24) || (player.y+3 >= ep->enemy.y+8 && player.y+3 <= ep->enemy.y+24))) {
        cflag = 1;
      }
      else if ((player.x-3 >= ep->enemy.x && player.x-3 <= ep->enemy.x+32) || (player.x+3 >= ep->enemy.x && player.x+3 <= ep->enemy.x+32)) {
        if ((player.y-3 >= ep->enemy.y && player.y-3 <= ep->enemy.y+32) || (player.y+3 >= ep->enemy.y && player.y+3 <= ep->enemy.y+32)) {
          player.graze++;
          score += 1200;
        }
      }
    }
    ep = ep->next;
  }

  if (cflag == 1) {
    player.collision = 1;
    if (player.life > 0) {
      player.life -= 30;
    }
    else {
      player.life = 0;
      mode = GAMEOVER;
    }
    cflag = 0;
  }
  else {
    player.collision = 0;
  }

  while (bp != NULL) {
    b_temp = bp->next;
    if (bp->bullet.type == DOT) {
      if (((player.x-3 >= bp->bullet.x+4 && player.x-3 <= bp->bullet.x+12) || (player.x+3 >= bp->bullet.x+4 && player.x+3 <= bp->bullet.x+12)) && ((player.y-3 >= bp->bullet.y+4 && player.y-3 <= bp->bullet.y+12) || (player.y+3 >= bp->bullet.y+4 && player.y+3 <= bp->bullet.y+12))) {
        if (player.life > 0) {
          player.life -= 5;
        }
        else {
          player.life = 0;
          mode = GAMEOVER;
        }
        FreeBullet(bp);
      }
      else if (((player.x-3 >= bp->bullet.x && player.x-3 <= bp->bullet.x+16) || (player.x+3 >= bp->bullet.x && player.x+3 <= bp->bullet.x+16)) && ((player.y-3 >= bp->bullet.y && player.y-3 <= bp->bullet.y+16) || (player.y+3 >= bp->bullet.y && player.y+3 <= bp->bullet.y+16))) {
          player.graze++;
          score += 500;
      }
    }
    bp = b_temp;
  }
}

void isEnemyCollided(void)
{
  bullet_Node *bp = bullets;
  bullet_Node *b_temp;
  enemy_Node *ep = enemies;
  enemy_Node *e_temp;

  while (bp != NULL) {
    b_temp = bp->next;
    ep = enemies;
    while (ep != NULL) {
      e_temp = ep->next;
      if (bp->bullet.type == BEAM && (ep->enemy.type == CIRCLE || ep->enemy.type == CUBE || ep->enemy.type == TRI || ep->enemy.type == SHOT || ep->enemy.type == BOSS)) {
        if (((bp->bullet.x >= ep->enemy.x && bp->bullet.x <= ep->enemy.x+32) || (bp->bullet.x+4 >= ep->enemy.x && bp->bullet.x+4 <= ep->enemy.x+32)) && ((bp->bullet.y >= ep->enemy.y && bp->bullet.y <= ep->enemy.y+32) || (bp->bullet.y+12 >= ep->enemy.x && bp->bullet.y+12 <= ep->enemy.y+32))) {
          ep->enemy.life--;
          if (ep->enemy.life <= 0) {
            switch (ep->enemy.type) {
            case CIRCLE:
              score += 100;
              break;

            case CUBE:
              score += 300;
              break;

            case TRI:
              score += 500;
              break;

            case SHOT:
              score += 1000;
              break;

            default:
              break;
            }
            enemiesCnt--;
            killedEnemies++;
            FreeBullet(bp);
            FreeEnemy(ep);
            break;
          }
          else {
            FreeBullet(bp);
            break;
          }
        }
      }
      ep = e_temp;
    }
    bp = b_temp;
  }
}

void Title(void)
{
  if (fade > 0) {
    fade -= 8;
  }
  else{}
}

void Manual(void)
{

}

void Run(void)
{
  bg1_pos+=2;
  bg2_pos+=2;
  if (bg1_pos >= 640) {
    bg1_pos = bg2_pos - 960;
  }
  else if (bg2_pos >= 640) {
    bg2_pos = bg1_pos - 960;
  }
  if (fade > 0) {
    fade -= 8;
  }
  else {
    score++;

    if (player.shot == 1) {
      bullets = AddBullet(bullets, player.x-12, player.y-8, 0, -20, BEAM);
      bullets = AddBullet(bullets, player.x+8, player.y-8, 0, -20, BEAM);
      //printf("%d, %d : %d\n", bullets->bullet.x, bullets->bullet.y, bullets->bullet.dy);
    }

    MovePlayer();
    MoveBullet();
    MoveEnemy();

    isCollided();
    isEnemyCollided();

    if (enemiesCnt < ENEMIES_MAX) {
      enemies = AddEnemy(enemies, random() % 368 + 40, (random() % 100) + 40, 0, 0, 0, (random() % 4 + 1), 20, random() % 2, random() % 4);
      enemiesCnt++;
    }

    if (killedEnemies >= 100) {
      mode = CLEAR;
    }
  }
}

void Pause(void)
{

}

void GameOver(void)
{

}

void Clear(void)
{

}

void FadeIn()
{
  if (fade < 255) {
    fade += 8;
  }
  else {
    mode = nextMode;
  }
}

void Display(void)
{
  //int x, y; // PNG画像をおく座標
  char str_buf[16];
  bullet_Node *bp = bullets;
  enemy_Node *ep = enemies;

  // ウィンドウの背景色
  glClear(GL_COLOR_BUFFER_BIT);

  if (mode == TITLE || (mode == MANUAL && nextMode == TITLE) || (mode == FADEIN && nextMode == RUN)) {
    PutSprite(img_title, 0, 0, &info_title, 255, 255, 255, 255);

    PutSprite(img_start, 54, 379, &info_start, 0, 155, 133, menu_select==0 ? 255 : 0);
    PutSprite(img_gomanual, 24, 474, &info_gomanual, 0, 155, 133, menu_select==1 ? 255 : 0);
    PutSprite(img_quit, 34, 564, &info_quit, 0, 155, 133, menu_select==2 ? 255 : 0);

    PutSprite(img_start, 50, 375, &info_start, 255, 255, 255, 255);
    PutSprite(img_gomanual, 20, 470, &info_gomanual, 255, 255, 255, 255);
    PutSprite(img_quit, 30, 560, &info_quit, 255, 255, 255, 255);
  }

  if (mode == RUN || mode == PAUSE || mode == GAMEOVER || mode == CLEAR || (mode == MANUAL && nextMode == PAUSE) || (mode == FADEIN && nextMode == TITLE)) {
    PutSprite(img_bg, 0, bg1_pos, &info_bg, 255, 255, 255, 255);
    PutSprite(img_bg, 0, bg2_pos, &info_bg, 255, 255, 255, 255);

    PutSprite(img_pl, player.x-24, player.y-24, &info_pl, 255, 255, 255, 255);

    while (ep != NULL) {
      //printf("%d, %d, %d\n", ep->enemy.x, ep->enemy.y, ep->enemy.direction);
      if (ep->enemy.type == CIRCLE) {
        PutSprite(img_en[0], ep->enemy.x, ep->enemy.y, &info_en[0], 255, 255, 255, 255);
      }
      else if (ep->enemy.type == CUBE) {
        PutSprite(img_en[1], ep->enemy.x, ep->enemy.y, &info_en[1], 255, 255, 255, 255);
      }
      else if (ep->enemy.type == TRI) {
        PutSprite(img_en[2], ep->enemy.x, ep->enemy.y, &info_en[2], 255, 255, 255, 255);
      }
      else if (ep->enemy.type == SHOT) {
        PutSprite(img_en_shot, ep->enemy.x, ep->enemy.y, &info_en_shot, 255, 255, 255, 255);
      }

      if (ep->enemy.type == CIRCLE || ep->enemy.type == CUBE || ep->enemy.type == TRI) {
        glColor4ub(0, 255, 0, 128);
        glBegin(GL_QUADS);
        glVertex2i(ep->enemy.x, ep->enemy.y);
        glVertex2i(ep->enemy.x, ep->enemy.y+32);
        glVertex2i(ep->enemy.x+32, ep->enemy.y+32);
        glVertex2i(ep->enemy.x+32, ep->enemy.y);
        glEnd();

        glColor4ub(255, 0, 0, 128);
        glBegin(GL_QUADS);
        glVertex2i(ep->enemy.x+8, ep->enemy.y+8);
        glVertex2i(ep->enemy.x+8, ep->enemy.y+24);
        glVertex2i(ep->enemy.x+24, ep->enemy.y+24);
        glVertex2i(ep->enemy.x+24, ep->enemy.y+8);
        glEnd();
      }
      else if (ep->enemy.type == SHOT) {
        glColor4ub(0, 255, 0, 128);
        glBegin(GL_QUADS);
        glVertex2i(ep->enemy.x, ep->enemy.y);
        glVertex2i(ep->enemy.x, ep->enemy.y+48);
        glVertex2i(ep->enemy.x+48, ep->enemy.y+48);
        glVertex2i(ep->enemy.x+48, ep->enemy.y);
        glEnd();

        glColor4ub(255, 0, 0, 128);
        glBegin(GL_QUADS);
        glVertex2i(ep->enemy.x+8, ep->enemy.y+8);
        glVertex2i(ep->enemy.x+8, ep->enemy.y+40);
        glVertex2i(ep->enemy.x+40, ep->enemy.y+40);
        glVertex2i(ep->enemy.x+40, ep->enemy.y+8);
        glEnd();
      }

      ep = ep->next;
    }

    glColor4ub(0, 0, 255, 255);
    glBegin(GL_QUADS);
    glVertex2i(player.x-3, player.y-3);
    glVertex2i(player.x-3, player.y+3);
    glVertex2i(player.x+3, player.y+3);
    glVertex2i(player.x+3, player.y-3);
    glEnd();

    glColor4ub(255, 0, 0, 255);
    glBegin(GL_POINTS);
    glVertex2i(player.x, player.y);
    glEnd();

    while (bp != NULL) {
      if (bp->bullet.type == BEAM) {
        glColor4ub(0, 0, 0, 255);
        glBegin(GL_QUADS);
        glVertex2i(bp->bullet.x, bp->bullet.y);
        glVertex2i(bp->bullet.x, bp->bullet.y+12);
        glVertex2i(bp->bullet.x+4, bp->bullet.y+12);
        glVertex2i(bp->bullet.x+4, bp->bullet.y);
        glEnd();

        glColor4ub(0, 157, 198, 255);
        glBegin(GL_QUADS);
        glVertex2i(bp->bullet.x+1, bp->bullet.y+1);
        glVertex2i(bp->bullet.x+1, bp->bullet.y+11);
        glVertex2i(bp->bullet.x+3, bp->bullet.y+11);
        glVertex2i(bp->bullet.x+3, bp->bullet.y+1);
        glEnd();
      }
      if (bp->bullet.type == DOT) {
        PutSprite(img_en_bullet, bp->bullet.x, bp->bullet.y, &info_en_bullet, 255, 255, 255, 255);

        glColor4ub(0, 255, 0, 128);
        glBegin(GL_QUADS);
        glVertex2i(bp->bullet.x, bp->bullet.y);
        glVertex2i(bp->bullet.x, bp->bullet.y+16);
        glVertex2i(bp->bullet.x+16, bp->bullet.y+16);
        glVertex2i(bp->bullet.x+16, bp->bullet.y);
        glEnd();

        glColor4ub(255, 0, 0, 128);
        glBegin(GL_QUADS);
        glVertex2i(bp->bullet.x+4, bp->bullet.y+4);
        glVertex2i(bp->bullet.x+4, bp->bullet.y+12);
        glVertex2i(bp->bullet.x+12, bp->bullet.y+12);
        glVertex2i(bp->bullet.x+12, bp->bullet.y+4);
        glEnd();
      }

      bp = bp->next;
    }
  }

  if (mode == PAUSE || (mode == MANUAL && nextMode == PAUSE)) {
    PutSprite(img_pause, 0, 0, &info_pause, 255, 255, 255, 255);

    PutSprite(img_resume, 179, 229, &info_resume, 0, 155, 133, menu_select==0 ? 255 : 0);
    PutSprite(img_gomanual, 169, 279, &info_gomanual, 0, 155, 133, menu_select==1 ? 255 : 0);
    PutSprite(img_gotitle, 199, 329, &info_gotitle, 0, 155, 133, menu_select==2 ? 255 : 0);

    PutSprite(img_resume, 175, 225, &info_resume, 255, 255, 255, 255);
    PutSprite(img_gomanual, 165, 275, &info_gomanual, 255, 255, 255, 255);
    PutSprite(img_gotitle, 195, 325, &info_gotitle, 255, 255, 255, 255);
  }

  if (mode == GAMEOVER || mode == CLEAR) {
    if (mode == GAMEOVER) {
      PutSprite(img_gmovr, 0, 0, &info_gmovr, 255, 255, 255, 255);
    }
    else {
      PutSprite(img_clr, 0, 0, &info_clr, 255, 255, 255, 255);
    }

    sprintf(str_buf, "%011d", score);
    PutImgNumbers(180, 231, str_buf, 255, 255, 255, 255);

    sprintf(str_buf, "%4d", player.graze);
    PutImgNumbers(306, 267, str_buf, 255, 255, 255, 255);

    sprintf(str_buf, "%03d", killedEnemies);
    PutImgNumbers(216, 305, str_buf, 255, 255, 255, 255);

  }

  if (mode == RUN || mode == PAUSE || mode == GAMEOVER || mode == CLEAR || (mode == MANUAL && nextMode == PAUSE) || (mode == FADEIN && nextMode == TITLE)) {

    PutSprite(img_fl, 0, 0, &info_fl, 255, 255, 255, 255);

    sprintf(str_buf, "%015d", score);
    PutImgNumbers(160, 545, str_buf, 255, 255, 255, 255);

    glColor4ub(40, 40, 40, 128);
    glBegin(GL_QUADS);
    glVertex2i(125, 593);
    glVertex2i(125, 605);
    glVertex2i(225, 605);
    glVertex2i(225, 593);
    glEnd();

    if (player.life <= 10) {
      glColor4ub(255, 0, 0, 255);
    }
    else if (player.life <= 30) {
      glColor4ub(255, 255, 0, 255);
    }
    else {
      glColor4ub(255, 255, 255, 255);
    }
    glBegin(GL_QUADS);
    glVertex2i(125, 593);
    glVertex2i(125, 605);
    glVertex2i(player.life+125, 605);
    glVertex2i(player.life+125, 593);
    glEnd();

    sprintf(str_buf, "%4d", player.graze);
    PutImgNumbers(358, 589, str_buf, 255, 255, 255, 255);

    glColor4ub(0, 0, 255, 200);
    PrintText(50, 60, "POS");
    sprintf(str_buf, "%d, %d", player.x, player.y);
    PrintText(120, 60, str_buf);

    PrintText(50, 80, "LIFE");
    sprintf(str_buf, "%d", player.life);
    PrintText(120, 80, str_buf);

    PrintText(50, 100, "SPEED");
    sprintf(str_buf, "%d", player.speed);
    PrintText(120, 100, str_buf);

    PrintText(50, 120, "SHOT");
    sprintf(str_buf, "%d", player.shot);
    PrintText(120, 120, str_buf);

    PrintText(50, 140, "GRAZE");
    sprintf(str_buf, "%d", player.graze);
    PrintText(120, 140, str_buf);

    PrintText(50, 160, "COLLISION");
    sprintf(str_buf, "%d", player.collision);
    PrintText(180, 160, str_buf);

    PrintText(50, 180, "ENEMIES");
    sprintf(str_buf, "%d", enemiesCnt);
    PrintText(160, 180, str_buf);
  }

  if (mode == MANUAL) {
    PutSprite(img_manual, 0, 0, &info_manual, 255, 255, 255, 255);
  }

  if (fade < 0) {
    fade = 0;
  }
  if (fade > 255) {
    fade = 255;
  }
  glColor4ub(0, 0, 0, fade);
  glBegin(GL_QUADS);
  glVertex2i(0, 0);
  glVertex2i(0, WHEIGHT);
  glVertex2i(WWIDTH, WHEIGHT);
  glVertex2i(WWIDTH, 0);
  glEnd();

  //w = glutGet(GLUT_WINDOW_WIDTH);
  //h = glutGet(GLUT_WINDOW_HEIGHT);

  glFlush();
}

void PrintText(int x, int y, char *s)
{
  int i = 0;

  glRasterPos2i(x, y);
  for (i = 0; i < strlen(s); i++)
  {
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, s[i]);
  }
}

void Mouse (int b, int s, int x, int y)
{
  if (b == GLUT_LEFT_BUTTON) {
    if (s == GLUT_UP) printf("[Left button up]");
    if (s == GLUT_DOWN) {
      printf("[Left button down]");
    }
  }

  if (b == GLUT_MIDDLE_BUTTON) {
    if (s == GLUT_UP) printf("[Middle button up]");
    if (s == GLUT_DOWN) printf("[Middle button down]");
  }

  if (b == GLUT_RIGHT_BUTTON) {
    if (s == GLUT_UP) printf("[Right button up]");
    if (s == GLUT_DOWN) printf("[Right button down]");
  }

  printf(" at (%d, %d)\n", x, y);

  // 再描画
  glutPostRedisplay();
}

void PassiveMotion(int x, int y)
{
  printf("PassiveMotion : (x, y)=(%d, %d)\n", x, y);
}

void Motion(int x, int y)
{
  printf("Motion : (x, y)=(%d, %d)\n", x, y);
  glutPostRedisplay();
}

void Entry(int s)
{
  if (s == GLUT_ENTERED) printf("Entered\n");
  if (s == GLUT_LEFT) printf("Left\n");
}

void Keyboard(unsigned char key, int x, int y)
{
  if ((key == 'q')) {
    printf("End\n");
    exit(0);
  }
  if (key == 27) {
    if (mode == PAUSE) {
      mode = RUN;
    }
    else if (mode == RUN) {
      mode = PAUSE;
    }
    else if (mode == GAMEOVER || mode == CLEAR) {
      Initialize();
      mode = FADEIN;
      nextMode = TITLE;
    }
  }
  if (key == 'z' || key == 'Z') {
    if (mode == TITLE) {
      switch (menu_select) {
      case 0:
        mode = FADEIN;
        nextMode = RUN;
        menu_select = 0;
        Initialize();
        break;

      case 1:
        mode = MANUAL;
        nextMode = TITLE;
        break;

      case 2:
        exit(0);
        break;

      default:
        break;
      }
    }
    else if (mode == PAUSE) {
      switch (menu_select) {
      case 0:
        mode = RUN;
        menu_select = 0;
        break;

      case 1:
        mode = MANUAL;
        nextMode = PAUSE;
        break;

      case 2:
        mode = FADEIN;
        nextMode = TITLE;
        menu_select = 0;
        break;

      default:
        break;
      }
    }
    else if (mode == MANUAL) {
      mode = nextMode;
    }
    else if (mode == RUN) {
      player.shot = 1;
    }
  }
}

void KeyboardUp(unsigned char key, int x, int y)
{
  if (key == 'z' || key == 'Z') {
    if (mode == RUN) {
      player.shot = 0;
    }
  }
}

void SpecialKey(int key, int x, int y)
{
  int mod = 0;

  if (mode == RUN) {
    mod = glutGetModifiers();
    if ((mod & GLUT_ACTIVE_SHIFT) != 0) {
      player.speed = 1;
    }
    else {
      player.speed = 8;
    }
  }

  switch (key) {
  case GLUT_KEY_RIGHT:
    if (mode == TITLE || mode == PAUSE) {
    }
    else if (mode == RUN) {
      direction[1] = 1;
    }
    glutPostRedisplay();
    break;

  case GLUT_KEY_LEFT:
    if (mode == TITLE || mode == PAUSE) {
    }
    else if (mode == RUN) {
      direction[3] = 1;
    }
    glutPostRedisplay();
    break;

  case GLUT_KEY_UP:
    if (mode == TITLE || mode == PAUSE) {
      if (menu_select> 0) {
        menu_select--;
      }
      else {
        menu_select= 2;
      }
    }
    else if (mode == RUN) {
      direction[0] = 1;
    }
    glutPostRedisplay();
    break;

  case GLUT_KEY_DOWN:
    if (mode == TITLE || mode == PAUSE) {
      if (menu_select< 2) {
        menu_select++;
      }
      else {
        menu_select= 0;
      }
    }
    else if (mode == RUN) {
      direction[2] = 1;
    }
    glutPostRedisplay();
    break;

  default:
    break;
  }
  //printf("%d, %d\n", px, py);
}

void SpecialKeyUp(int key, int x, int y)
{
  if (mode == RUN) {
    switch (key) {
    case GLUT_KEY_RIGHT:
      direction[1] = 0;
      glutPostRedisplay();
      break;

    case GLUT_KEY_LEFT:
      direction[3] = 0;
      glutPostRedisplay();
      break;

    case GLUT_KEY_UP:
      direction[0] = 0;
      glutPostRedisplay();
      break;

    case GLUT_KEY_DOWN:
      direction[2] = 0;
      glutPostRedisplay();
      break;

    default:
      break;
    }
  }
  //printf("%d, %d\n", px, py);
}

void PutSprite(int num, int x, int y, pngInfo *info, int r, int g, int b, int a)
{
  int w, h; // テクスチャの幅と高さ

  w = info->Width; // テクスチャの幅と高さを取得
  h = info->Height;

  // 描画した四角形にテクスチャを貼り付け
  glPushMatrix();
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, num);
  glColor4ub(r, g, b, a);

  glBegin(GL_QUADS); // 幅w, 高さhの四角形

  glTexCoord2i(0, 0);
  glVertex2i(x, y);

  glTexCoord2i(0, 1);
  glVertex2i(x, y + h);

  glTexCoord2i(1, 1);
  glVertex2i(x + w, y + h);

  glTexCoord2i(1, 0);
  glVertex2i(x + w, y);

  glEnd();

  glDisable(GL_TEXTURE_2D);
  glPopMatrix();
}

//
// ====== PutImgNum関数 ======
// ------ 機能 ------
// 数字1桁を対応する画像で表示
// -----------------
// ===========================
//
void PutImgNum(int x, int y, char str, int r, int g, int b, int a)
{
  if (((int)str) - 48 < 0 || ((int)str) - 48 > 9) {
    return;
  }
  PutSprite(img_num[((int)str) - 48], x, y, &info_num[((int)str) - 48], r, g, b, a);  // ASCIIコードに対応する画像を表示
}

//
// ====== PutImgNumbers関数 ======
// ------ 機能 ------
// 与えられた数字を順番に表示
// -----------------
// ==========================
//
void PutImgNumbers(int x, int y, char *s, int r, int g, int b, int a)
{
  int i = 0;
  int px = 0, py = 0; // 座標調整用変数

  // はじめの一文字を表示
  PutImgNum(x + (i * 18) + px, y + py, s[0], r, g, b, a);
  px = 0;
  py = 0;
  for (i = 1; i < strlen(s); i++) { // 残りの文字を表示
    PutImgNum(x + (i * 18) + px, y + py, s[i], r, g, b, a);
    py = 0;
  }
}