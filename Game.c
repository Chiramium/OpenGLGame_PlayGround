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

enum MODE { TITLE, SELECT, SETTING, RUN, PAUSE, RESULT };
enum ETYPE { CUBE, BOSS };
enum BTYPE { BEAM, DOT, LINE };

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
  int shot;
  int speed;
  int life;
  int collision;
  int active;
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

GLuint img_pl, img_en, img_fl;
GLuint img_num[10];
pngInfo info_pl, info_en, info_fl;
pngInfo info_num[10] = {0};
int mode = TITLE;
int col = 0;
int score = 0;
char str[32];
int plx = 216, ply = 452;
int direction[4] = {0}; // 0:up, 1:right, 2:down, 3:left
int menu_select = 0;
int enemiesCnt = 0;
struct PLAYER player;
enemy_Node* enemies;
bullet_Node* bullets;

// Prototype
void Reshape(int, int);

void Initialize();

void Transition(int value);

enemy_Node *AddEnemy(enemy_Node* enemies, int x, int y, int shot, int speed, int life, int collision, int active, enum ETYPE type);
void FreeEnemy(enemy_Node* enemy);
bullet_Node *AddBullet(bullet_Node* bullets, int x, int y, int dx, int dy, enum BTYPE type);
void FreeBullet(bullet_Node* bullet);
void MovePlayer();
void MoveEnemy();
void MoveBullet();
void isCollided(void);
void isEnemyCollided(void);

void Title(void);
void Select(void);
void Run(void);
void Pause(void);
void Result(void);

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
  int i;
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

  sprintf(str, "./images/player.png");
  img_pl = pngBind(str, PNG_NOMIPMAP, PNG_ALPHA, &info_pl, GL_CLAMP, GL_NEAREST, GL_NEAREST);

  sprintf(str, "./images/enemy.png");
  img_en = pngBind(str, PNG_NOMIPMAP, PNG_ALPHA, &info_en, GL_CLAMP, GL_NEAREST, GL_NEAREST);

  sprintf(str, "./images/field.png");
  img_fl = pngBind(str, PNG_NOMIPMAP, PNG_ALPHA, &info_fl, GL_CLAMP, GL_NEAREST, GL_NEAREST);

  // テキストに対応する画像の読み込み
  for (i = 0; i < 10; i++) {
    sprintf(str, "./fonts/%d.png", i); // ファイル名はすべてASCIIコードに対応
    img_num[i] = pngBind(str, PNG_NOMIPMAP, PNG_ALPHA, &info_num[i], GL_CLAMP, GL_NEAREST, GL_NEAREST);
    printf("if = %d, W=%d, H=%d, D=%d, A=%d\n", img_num[i], info_num[i].Width, info_num[i].Height, info_num[i].Depth, info_num[i].Alpha);
  }

  // コールバック関数の登録
  glutReshapeFunc(Reshape);
  glutTimerFunc(20, Transition, 0);
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

void Initialize()
{
  int i;

  player.x = 240;
  player.y = 476;
  player.speed = 12;
  player.collision = 0;
  player.life = 100;
  player.shot = 0;
  player.graze = 0;

  enemies = NULL;
  for (i = 0; i < ENEMIES_MAX-1; i++) {
    enemies = AddEnemy(enemies, (random() % 400) + 40, (random() % 300) + 40, 0, (random() % 10 + 1), 15, 0, 1, CUBE);
  }
  enemiesCnt = i;

  enemies = AddEnemy(enemies, 50, 200, 0, 0, 1000, 0, 1, CUBE);

  bullets = NULL;

  for (i = 0; i < 4; i++) {
    direction[i] = 0;
  }

  score = 0;
}

void Transition(int value)
{
  //printf("modeA = %d\n", mode);

  switch (mode) {
  case TITLE:
    Title();
    break;

  case SELECT:
    Select();
    break;

  case RUN:
    Run();
    break;

  case PAUSE:
    Pause();
    break;

  case RESULT:
    Result();
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

enemy_Node *AddEnemy(enemy_Node* enemies, int x, int y, int shot, int speed, int life, int collision, int active, enum ETYPE type)
{
  // 新しいノードの作成
  enemy_Node *node;
  node = (enemy_Node *)malloc(sizeof(enemy_Node));
  node->enemy.x = x;
  node->enemy.y = y;
  node->enemy.shot = shot;
  node->enemy.speed = speed;
  node->enemy.life = life;
  node->enemy.collision = collision;
  node->enemy.active = active;
  node->enemy.type = type;

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
    if ((p->enemy.x < 0 || p->enemy.x > 420) || (p->enemy.y < 0 || p->enemy.y > 500) || (p->enemy.life <= 0)) {
      enemiesCnt--;
      FreeEnemy(p);
    }
    else {
      p->enemy.y += p->enemy.speed;
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

void isCollided(void)
{
  int cflag = 0;
  enemy_Node *p = enemies;

  while (p != NULL) {
    if (((player.x-3 >= p->enemy.x+8 && player.x-3 <= p->enemy.x+24) || (player.x+3 >= p->enemy.x+8 && player.x+3 <= p->enemy.x+24)) && ((player.y-3 >= p->enemy.y+8 && player.y-3 <= p->enemy.y+24) || (player.y+3 >= p->enemy.y+8 && player.y+3 <= p->enemy.y+24))) {
      cflag = 1;
    }
    else if ((player.x-3 >= p->enemy.x && player.x-3 <= p->enemy.x+32) || (player.x+3 >= p->enemy.x && player.x+3 <= p->enemy.x+32)) {
      if ((player.y-3 >= p->enemy.y && player.y-3 <= p->enemy.y+32) || (player.y+3 >= p->enemy.y && player.y+3 <= p->enemy.y+32)) {
        player.graze++;
        score += 100;
      }
    }
    else if ((player.y-3 >= p->enemy.y && player.y-3 <= p->enemy.y+32) || (player.y+3 >= p->enemy.y && player.y+3 <= p->enemy.y+32)) {
      if ((player.x-3 >= p->enemy.x && player.x-3 <= p->enemy.x+32) || (player.x+3 >= p->enemy.x && player.x+3 <= p->enemy.x+32)) {
        player.graze++;
        score += 100;
      }
    }
    p = p->next;
  }

  if (cflag == 1) {
    player.collision = 1;
    if (player.life > 0) {
      player.life--;
    }
    else {
      mode = RESULT;
    }
    cflag = 0;
  }
  else {
    player.collision = 0;
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
      if (bp->bullet.type == BEAM && ep->enemy.type == CUBE) {
        if (((bp->bullet.x >= ep->enemy.x && bp->bullet.x <= ep->enemy.x+32) || (bp->bullet.x+4 >= ep->enemy.x && bp->bullet.x+4 <= ep->enemy.x+32)) && ((bp->bullet.y >= ep->enemy.y && bp->bullet.y <= ep->enemy.y+32) || (bp->bullet.y+12 >= ep->enemy.x && bp->bullet.y+12 <= ep->enemy.y+32))) {
          ep->enemy.life--;
          if (ep->enemy.life <= 0) {
            enemiesCnt--;
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
  col = 0;
}

void Select(void)
{
  col = 1;
}

void Run(void)
{
  col = 2;
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
    enemies = AddEnemy(enemies, (random() % 400) + 40, (random() % 300) + 40, 0, (random() % 10 + 1), 15, 0, 1, CUBE);
    enemiesCnt++;
  }
}

void Pause(void)
{

}

void Result(void)
{
  col = 3;
}

void Display(void)
{
  //int x, y; // PNG画像をおく座標
  int width = 400, height = 460; // ゲームマップのサイズ
  char str_buf[16];
  bullet_Node *bp = bullets;
  enemy_Node *ep = enemies;

  // ウィンドウの背景色
  glClear(GL_COLOR_BUFFER_BIT);

  switch (col) {
  case 0:
    glColor3ub(255, 0, 0);
    break;

  case 1:
    glColor3ub(0, 255, 0);
    break;

  case 2:
    glColor3ub(0, 0, 255);
    break;

  case 3:
    glColor3ub(200, 30, 150);
    break;

  default:
    break;
  }

  glBegin(GL_QUADS);
  glVertex2i(0, 0);
  glVertex2i(0, WHEIGHT);
  glVertex2i(WWIDTH, WHEIGHT);
  glVertex2i(WWIDTH, 0);
  glEnd();

  if (mode == TITLE) {
    glColor4ub(0, 0, 0, menu_select==0 ? 255 : 0);
    PrintText(101, 291, "START");
    glColor4ub(0, 0, 0, menu_select==1 ? 255 : 0);
    PrintText(101, 311, "SETTING");
    glColor4ub(0, 0, 0, menu_select==2 ? 255 : 0);
    PrintText(101, 331, "QUIT");

    glColor4ub(255, 255, 255, 255);
    PrintText(100, 290, "START");
    PrintText(100, 310, "SETTING");
    PrintText(100, 330, "QUIT");
  }

  if (mode == RUN || mode == PAUSE || mode == RESULT) {
    glColor4ub(255, 255, 255, 255);
    glBegin(GL_QUADS);
    glVertex2i(40, 40);
    glVertex2i(40, 40+height+1);
    glVertex2i(40+width+1, 40+height+1);
    glVertex2i(40+width+1, 40);
    glEnd();

    while (bp != NULL) {
      if (bp->bullet.type == BEAM) {
        glColor4ub(255, 0, 0, 255);
        glBegin(GL_QUADS);
        glVertex2i(bp->bullet.x, bp->bullet.y);
        glVertex2i(bp->bullet.x, bp->bullet.y+12);
        glVertex2i(bp->bullet.x+4, bp->bullet.y+12);
        glVertex2i(bp->bullet.x+4, bp->bullet.y);
        glEnd();
      }

      bp = bp->next;
    }

    PutSprite(img_pl, player.x-24, player.y-24, &info_pl, 255, 255, 255, 255);

    while (ep != NULL) {
      //printf("%d, %d, %d\n", ep->enemy.x, ep->enemy.y, ep->enemy.active);
      PutSprite(img_en, ep->enemy.x, ep->enemy.y, &info_en, 255, 255, 255, 255);

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

  if (mode == PAUSE) {
    glColor4ub(0, 0, 0, 128);
    glBegin(GL_QUADS);
    glVertex2i(0, 0);
    glVertex2i(0, WHEIGHT);
    glVertex2i(WWIDTH, WHEIGHT);
    glVertex2i(WWIDTH, 0);
    glEnd();

    glColor4ub(255, 0, 0, menu_select==0 ? 255 : 0);
    PrintText(101, 291, "RESUME");
    glColor4ub(255, 0, 0, menu_select==1 ? 255 : 0);
    PrintText(101, 311, "SETTING");
    glColor4ub(255, 0, 0, menu_select==2 ? 255 : 0);
    PrintText(101, 331, "TITLE");

    glColor4ub(255, 255, 255, 255);
    PrintText(100, 290, "RESUME");
    PrintText(100, 310, "SETTING");
    PrintText(100, 330, "TITLE");
  }

  if (mode == RESULT) {
    glColor4ub(0, 0, 0, 128);
    glBegin(GL_QUADS);
    glVertex2i(0, 0);
    glVertex2i(0, WHEIGHT);
    glVertex2i(WWIDTH, WHEIGHT);
    glVertex2i(WWIDTH, 0);
    glEnd();

    glColor4ub(255, 0, 0, menu_select==0 ? 255 : 0);
    PrintText(180, 200, "GAME OVER");
    PrintText(120, 300, "SCORE: ");
    sprintf(str_buf, "%015d", score);
    PrintText(200, 300, str_buf);
  }

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
  }
  if (key == 'z' || key == 'Z') {
    if (mode == TITLE) {
      switch (menu_select) {
      case 0:
        mode = RUN;
        break;

      case 1:
        mode = SETTING;
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
        break;

      case 1:
        mode = SETTING;
        break;

      case 2:
        mode = TITLE;
        break;

      default:
        break;
      }
    }
    else if (mode == RESULT) {
      Initialize();
      mode = TITLE;
    }
    else if (mode == RUN) {
      printf("zDown\n");
      player.shot = 1;
    }
  }
}

void KeyboardUp(unsigned char key, int x, int y)
{
  if (key == 'z' || key == 'Z') {
    printf("zUp\n");
    player.shot = 0;
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
      player.speed = 12;
    }
  }

  switch (key) {
  case GLUT_KEY_RIGHT:
    if (mode == TITLE || mode == PAUSE) {
      printf("RIGHT\n");
    }
    else if (mode == RUN) {
      direction[1] = 1;
    }
    glutPostRedisplay();
    break;

  case GLUT_KEY_LEFT:
    if (mode == TITLE || mode == PAUSE) {
      printf("LEFT\n");
    }
    else if (mode == RUN) {
      direction[3] = 1;
    }
    glutPostRedisplay();
    break;

  case GLUT_KEY_UP:
    if (mode == TITLE || mode == PAUSE) {
      printf("UP\n");
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
      printf("DOWN\n");
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
    // 特定の文字の場合は座標を調整
    // if (s[i - 1] >= 'A' && s[i - 1] <= 'Z') { // 直前の文字が大文字
    //   if (s[i - 1] == 'M' || s[i - 1] == 'W') {
    //     px += 8;
    //   }
    //   if (s[i - 1] == 'C') {
    //     px += 6;
    //   }
    //   px += 2;
    // }
    // if (s[i] == 'y') {
    //   py += 10;
    // }
    // if (s[i - 1] == 'u') {
    //   px -= 2;
    // }
    // if (s[i - 1] == 'i') {
    //   px -= 8;
    // }
    // if (s[i - 1] == 'r') {
    //   px -= 3;
    // }
    // if (s[i - 1] == 's') {
    //   px -= 3;
    // }
    // if (s[i] == 's') {
    //   px -= 3;
    // }
    PutImgNum(x + (i * 18) + px, y + py, s[i], r, g, b, a);
    py = 0;
  }
}