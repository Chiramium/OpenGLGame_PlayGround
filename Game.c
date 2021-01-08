#include <GL/glut.h>
#include <GL/glpng.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

// ウィンドウサイズの定義
#define WWIDTH 480
#define WHEIGHT 640



// Prototype
void Reshape(int, int);
void Initialize();
void Transition(int value);
void Title(void);
void Select(void);
void Run(void);
void Result(void);
void Display(void);
void PrintText(int x, int y, char *s);
void Mouse(int, int, int, int);
void PassiveMotion(int, int);
void Motion(int, int);
void Entry(int);
void Keyboard(unsigned char, int, int);
void SpecialKey(int, int, int);
void SpecialKeyUp(int, int, int);
void PutSprite(int num, int x, int y, pngInfo *info, int r, int g, int b, int a);

GLuint img;
pngInfo info;

struct PLAYER
{
  int x;
  int y;
  int shot;
  int speed;
  int life;
  int collision;
  int graze;
} PLAYER;

int mode = 2;
int col = 0;
int score = 0;
char str[32];
int plx = 216, ply = 452;
int direction[4] = {0}; // 0:up, 1:right, 2:down, 3:left
struct PLAYER player;

int main(int argc, char **argv)
{
  int scrWidth = 0, scrHeight = 0; // 画面全体のサイズ

  // ウィンドウ初期化, 乱数初期化
  srandom(12345);
  glutInit(&argc, argv);
  scrWidth = glutGet(GLUT_SCREEN_WIDTH);
  scrHeight = glutGet(GLUT_SCREEN_HEIGHT);
  glutInitWindowSize(WWIDTH, WHEIGHT);
  glutCreateWindow("Game [Test]");
  glutPositionWindow((scrWidth - WWIDTH) / 2, (scrHeight - WHEIGHT) / 2);
  glutInitDisplayMode(GLUT_RGBA | GLUT_ALPHA);
  glClearColor(1.0, 1.0, 1.0, 1.0);
  Initialize();

  // アルファチャネルの有効化
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

  sprintf(str, "./images/player.png");
  img = pngBind(str, PNG_NOMIPMAP, PNG_ALPHA, &info, GL_CLAMP, GL_NEAREST, GL_NEAREST);

  // コールバック関数の登録
  glutReshapeFunc(Reshape);
  glutTimerFunc(20, Transition, 0);
  glutDisplayFunc(Display);
  glutPassiveMotionFunc(PassiveMotion);
  glutMouseFunc(Mouse);
  glutKeyboardFunc(Keyboard);
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
  player.x = 240;
  player.y = 476;
  player.speed = 1;
  player.collision = 0;
  player.life = 100;
  player.shot = 0;
  player.graze = 0;
}

void Transition(int value)
{
  mode = mode % 4;
  //printf("modeA = %d\n", mode);

  switch (mode) {
  case 0:
    Title();
    break;

  case 1:
    Select();
    break;

  case 2:
    Run();
    break;

  case 3:
    Result();
    break;

  default:
    printf("mode error\n");
    mode = 0;
    break;
  }

  // Display関数を強制呼び出し
  glutPostRedisplay();

  glutTimerFunc(20, Transition, 0);
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
  if (direction[0] == 1) {
    if (player.y > 64) {
      player.y--;
    }
    else {
      player.y = 64;
    }
  }
  if (direction[1] == 1) {
    if (player.x < 416) {
      player.x++;
    }
    else {
      player.y = 64;
    }
  }
  if (direction[2] == 1) {
    if (player.y < 476) {
      player.y++;
    }
    else {
      player.y = 64;
    }
  }
  if (direction[3] == 1) {
    if (player.x > 64) player.x--;
    else {
      player.y = 64;
    }
  }
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

  glColor4ub(255, 255, 255, mode==0 ? 255 : 0);
  PrintText(100, 290, "START");

  glColor4ub(255, 255, 255, mode==2 ? 255 : 0);
  glBegin(GL_QUADS);
  glVertex2i(40, 40);
  glVertex2i(40, 40+height);
  glVertex2i(40+width, 40+height);
  glVertex2i(40+width, 40);
  glEnd();

  PutSprite(img, player.x-24, player.y-24, &info, 255, 255, 255, mode==2 ? 255 : 0);

  glColor4ub(0, 0, 255, mode==2 ? 255 : 0);
  glBegin(GL_QUADS);
  glVertex2i(player.x-3, player.y-3);
  glVertex2i(player.x-3, player.y+3);
  glVertex2i(player.x+3, player.y+3);
  glVertex2i(player.x+3, player.y-3);
  glEnd();

  glColor4ub(255, 0, 0, mode==2 ? 255 : 0);
  glBegin(GL_POINTS);
  glVertex2i(player.x, player.y);
  glEnd();

  glColor4ub(255, 255, 255, mode==2 ? 255 : 0);
  PrintText(40, 540, "SCORE");
  sprintf(str_buf, "%015d", score);
  PrintText(120, 540, str_buf);

  glColor4ub(255, 0, 0, mode==2 ? 200 : 0);
  PrintText(50, 60, "POS");
  sprintf(str_buf, "%d, %d", player.x, player.y);
  PrintText(120, 60, str_buf);

  glColor4ub(255, 0, 0, mode==2 ? 200 : 0);
  PrintText(50, 80, "LIFE");
  sprintf(str_buf, "%d", player.life);
  PrintText(120, 80, str_buf);

  glColor4ub(255, 0, 0, mode==2 ? 200 : 0);
  PrintText(50, 100, "SPEED");
  sprintf(str_buf, "%d", player.speed);
  PrintText(120, 100, str_buf);

  glColor4ub(255, 0, 0, mode==2 ? 200 : 0);
  PrintText(50, 120, "SHOT");
  sprintf(str_buf, "%d", player.shot);
  PrintText(120, 120, str_buf);

  glColor4ub(255, 0, 0, mode==2 ? 200 : 0);
  PrintText(50, 140, "GRAZE");
  sprintf(str_buf, "%d", player.graze);
  PrintText(120, 140, str_buf);

  glColor4ub(255, 0, 0, mode==2 ? 200 : 0);
  PrintText(50, 160, "COLLISION");
  sprintf(str_buf, "%d", player.collision);
  PrintText(180, 160, str_buf);


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
      mode++;
      printf("mode = %d\n", mode);
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
  if ((key == 'q') || (key == 27)) {
    printf("End\n");
    exit(0);
  }
}

void SpecialKey(int key, int x, int y)
{
  switch (key) {
  case GLUT_KEY_RIGHT:
    direction[1] = 1;
    glutPostRedisplay();
    break;

  case GLUT_KEY_LEFT:
    direction[3] = 1;
    glutPostRedisplay();
    break;

  case GLUT_KEY_UP:
    direction[0] = 1;
    glutPostRedisplay();
    break;

  case GLUT_KEY_DOWN:
    direction[2] = 1;
    glutPostRedisplay();
    break;

  default:
    break;
  }
  //printf("%d, %d\n", px, py);

}

void SpecialKeyUp(int key, int x, int y)
{
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