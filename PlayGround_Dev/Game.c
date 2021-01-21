/* 【 名前 】 PlayGround - A Pop Shooting Game                                */
/* 【 制作者 】 長野高専電子情報工学科4年 岡田隼飛 (17405)                       */
/* 【 開発環境 】 cygwin ver.3.0.7-1 | gcc ver.7.4.0 | GNU make ver.4.2.1     */
/* 【 動作環境 】 Windows10 ver.1909 (OS Build : 18363.1198)                  */
/* 【 バージョン 】 ver.1.0.1                                                 */
/* 【 最終更新日 】 2021年 1月 21 日（ライバルが手を結ぶ日）                     */
/*===========================================================================*/
/*===========================================================================*/
/*                                     (,                                    */
/*                                   .MDMp                                   */
/*                                  .M^  TN.                                 */
/*                                 (B     JN,                                */
/*                               .MD       .Mp                               */
/*                              .M^          UN.                             */
/*                             (#             ?N,                            */
/*                           .MF               .Mp                           */
/*                          .M3                  UN.                         */
/*                         .#!                    ?N,                        */
/*                       .dF                       ,Mp                       */
/*                      .M=                          Uh.                     */
/*                     .#!                            ?N,                    */
/*                   .dF                               ,Me                   */
/*                  .M%                                  Wk.                 */
/*                 .#!                                    ?N,                */
/*               .dD                   .                   ,Me               */
/*              .Mt                   .MN.                   Wm              */
/*             .M'                   JD ,M,                   7N,            */
/*           .dD                   .M$    Wb                   ,M,           */
/*          .M$                   .M'      7N,                   Wm          */
/*         .M'                  .J#         ,M,                   TN,        */
/*       .J@                   .M"UN.      .M=Wm                   ,M,       */
/*      .MD                   .M'  ?N,    (#   7N,                  .Wb      */
/*     .M^                  .j@     ,Mp .MF     ,M,                   TN,    */
/*    J@                   .M"        UNM3       .Wb                   (M,   */
/*  .MD                   .M^         .MN,         TN,                  .Hb  */
/* .M^                   J@         .dF ,Me         (M,                   TN,*/
/* ?Me.                .MD         .M3    Wh.        .Hb                 .d@!*/
/*   ?Me.             .M^         .#!      ?N,         TN,             .d@!  */
/*     ?Me.          JB         .dF         ,Me         /N,          .d@!    */
/*      .MMa.      .MD         .Mt            Wk.        .Mp       .dMp      */
/*     .M3 ?Ma.   .M^         .H!              ?N,         TN.   .d@! Uh.    */
/*    .#!    ?Ma.(B         .dD                 ,Mx         ?N,.d@     ?N,   */
/*  .MF        ?MF         .M$                    Wm         .M@        ,Mp  */
/*   (Mm.                 .M'                      7N,                 .MD   */
/*     (Mm.             .d@                         ,M,              .MD     */
/*       (Hm.          .M$                           .Wm           .MD       */
/*         (Hm,       .M'                              TN,       .MD         */
/*           (Hm,    J@                                 (M,    .MD           */
/*             (Hm,.MD                                   .Wb .MD             */
/*               ,HM^                                      TMD               */
/*===========================================================================*/
/*===========================================================================*/
// __________________ヘッダーファイルのインクルード__________________ */
//
#include <GL/glut.h>
#include <GL/glpng.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
//
/* ______________________________________________________________ */


/* __________________________マクロ定義__________________________ */
//
// ウィンドウサイズの定義
#define WWIDTH 480
#define WHEIGHT 640
// 画面上に表示される敵の最大数を定義
#define ENEMIES_MAX 5
//
/* ______________________________________________________________ */


/* __________________________列挙型の定義__________________________ */
//
// ------ SCENE : ゲームの各シーンの実行を示す ------
// TITLE    ... タイトル画面
// MANUAL   ... マニュアル画面
// RUN      ... ゲーム画面
// PAUSE    ... ポーズ画面
// GAMEOVER ... ゲームオーバー画面
// CLEAR    ... クリア画面
// FADEIN   ... フェード画面
enum SCENE { TITLE, MANUAL, RUN, PAUSE, GAMEOVER, CLEAR, FADEIN };
// ------------------------------------------------
//
// ------ ETYPE : 敵キャラの種類を示す ------
// CIRCLE ... 円形の敵
// CUBE   ... 四角形の敵
// TRI    ... 三角形の敵
// SHOT   ... 弾幕を展開する敵
enum ETYPE { CIRCLE, CUBE, TRI, SHOT };
// ------------------------------------------------
//
// ------ BTYPE : 弾の種類を示す ------
// BEAM ... ビーム状の弾, プレイヤーから発射
// DOT  ... ドット状の弾, 敵キャラから発射
enum BTYPE { BEAM, DOT };
// ------------------------------------------------
//
// ------ DIRECTION : 敵が進む方向を示す ------
// RIGHT ... 右方向
// LEFT  ... 左方向
// STOP  ... 停止
enum DIRECTION { RIGHT, LEFT, STOP };
// ------------------------------------------------
/* ______________________________________________________________ */


/* __________________________構造体の定義__________________________ */
//
// ------ PLAYER : プレイヤーの情報を格納 ------
typedef struct PLAYER
{
  int x;  // x座標
  int y;  // y座標
  int shot;   // 弾発射フラグ
  int speed;  // 速度
  int life;   // HP
  int collision;  // 衝突フラグ
  int graze;  // グレイズ値
} PLAYER;
// ------------------------------------------------
//
// ------ ENEMY : 敵キャラの情報を格納 ------
typedef struct ENEMY
{
  int x;  // x座標
  int y;  // y座標
  int rx; // x方向の移動量カウント
  int ry; // y方向の移動量カウント
  int shot;   // 弾発射フラグ
  int speed;  // 速度
  int life;   // HP
  int direction;  // 移動方向
  enum ETYPE type;   // 敵の種類
} ENEMY;
// ------------------------------------------------
//
// ------ e_Node : 敵キャラのリスト構造の情報を格納 ------
typedef struct e_Node
{
  ENEMY enemy;  // 敵キャラの情報
  struct e_Node *prev;  // 前のリストのアドレス
  struct e_Node *next;  // 次のリストのアドレス
} enemy_Node;
// ------------------------------------------------
//
// ------ BULLET : 弾の情報を格納 ------
typedef struct BULLET
{
  int x;  // x座標
  int y;  // y座標
  int dx; // x方向のベクトル
  int dy; // y方向のベクトル
  enum BTYPE type;  // 弾の種類
} BULLET;
// ------------------------------------------------
//
// ------ b_Node : 弾のリスト構造の情報を格納 ------
typedef struct b_Node
{
  BULLET bullet;  //  弾の情報
  struct b_Node *prev;  // 前のリストのアドレス
  struct b_Node *next;  // 次のリストのアドレス
} bullet_Node;
// ------------------------------------------------
/* ______________________________________________________________ */


/* __________________________グローバル変数__________________________ */
//
// ====== シーン指定用列挙型変数 ======
// ------ SCENE(enum) ------
enum SCENE scene = TITLE; // 現在のシーン
enum SCENE nextScene;     // 次に遷移するシーン
// ------------------------------------------------
// ==============================================
//
// ====== 画面制御用変数 ======
// ------ int ------
int menu_select = 0;  // 選択されているメニュー
int fade = 255;   // フェード時の不透明度
int bg1_pos = 0, bg2_pos = 0; // 背景画像のy座標
// ------------------------------------------------
// ==============================================
//
// ====== 操作制御用変数 ======
// ------ int ------
// ctrlDirection : 方向キーの状態を格納
// [0]:上方向, [1]:右方向, [2]:下方向, [3]:左方向
// ctrlDirection[n] == 0 : 押されていない,  ctrlDirection[n] == 1 : 押されている
int ctrlDirection[4] = {0};
// ------------------------------------------------
// ==============================================
//
// ====== スコア, カウンタを示す変数 ======
// ------ int ------
int score = 0;  // ゲームのスコア
int enemiesCnt = 0; // 画面上の敵の数
int killedEnemies = 0;  // 倒した敵の数
// ------------------------------------------------
// ==============================================
//
// ====== ゲーム上のオブジェクトを示す変数 ======
// ------ PLAYER(struct) ------
struct PLAYER player; // プレイヤーの情報
// ------------------------------------------------
//
// ------ enemy_Node*(struct) ------
enemy_Node* enemies;  // 敵キャラのリスト構造の先頭アドレス
// ------------------------------------------------
//
// ------ bullet_Node*(struct) ------
bullet_Node* bullets; // 弾のリスト構造の先頭アドレス
// ------------------------------------------------
// ==============================================
//
// ====== 画像処理用変数 ======
// ------ GLuint ------
// img_pl        ... プレイヤーの画像ID
// img_en[3]     ... 円形, 四角形, 三角形の敵キャラの画像ID
// img_en_shot   ... 弾幕を展開する敵キャラの画像ID
// img_en_bullet ... 敵キャラの弾の画像ID
// img_bg        ... ゲーム画面の背景の画像ID
// img_fr        ... ゲーム画面のフレームの画像ID
// img_title     ... タイトル画面の画像ID
// img_pause     ... ポーズ画面の画像ID
// img_manual    ... マニュアル画面の画像ID
// img_gmovr     ... ゲームオーバー画面の画像ID
// img_clr       ... クリア画面の画像ID
// img_start     ... "START"メニューの画像ID
// img_gomanual  ... "MANUAL"メニューの画像ID
// img_resume    ... "RESUME"メニューの画像ID
// img_quit      ... "QUIT"メニューの画像ID
// img_gotitle   ... "TITLE"メニューの画像ID
GLuint img_pl, img_en[3], img_en_shot, img_en_bullet, img_bg, img_fr, img_title, img_pause, img_manual, img_gmovr, img_clr, img_start, img_gomanual, img_resume, img_quit, img_gotitle;
GLuint img_num[10]; // 数字フォントの画像ID
// ------------------------------------------------
//
// ------ pngInfo ------
// info_pl        ... プレイヤーの画像情報
// info_en[3]     ... 円形, 四角形, 三角形の敵キャラの画像情報
// info_en_shot   ... 弾幕を展開する敵キャラの画像情報
// info_en_bullet ... 敵キャラの弾の画像情報
// info_bg        ... ゲーム画面の背景の画像情報
// info_fr        ... ゲーム画面のフレームの画像情報
// info_title     ... タイトル画面の画像情報
// info_pause     ... ポーズ画面の画像情報
// info_manual    ... マニュアル画面の画像情報
// info_gmovr     ... ゲームオーバー画面の画像情報
// info_clr       ... クリア画面の画像情報
// info_start     ... "START"メニューの画像情報
// info_gomanual  ... "MANUAL"メニューの画像情報
// info_resume    ... "RESUME"メニューの画像情報
// info_quit      ... "QUIT"メニューの画像情報
// info_gotitle   ... "TITLE"メニューの画像情報
pngInfo info_pl, info_en[3], info_en_shot, info_en_bullet, info_bg, info_fr, info_title, info_pause, info_manual, info_gmovr, info_clr, info_start, info_gomanual, info_resume, info_quit, info_gotitle;
pngInfo info_num[10] = {0}; // 数字フォントの画像情報
// ------------------------------------------------
// ==============================================
/* ______________________________________________________________ */


/* __________________________プロトタイプ宣言__________________________ */
// ====== ウィンドウ更新 ======
void Reshape(int, int);
// ====== 画像読み込み ======
void ImportImages();
// ====== 初期化 ======
void Initialize();
// ====== シーン遷移処理 ======
void Transition(int value);
// ====== リスト構造管理 ======
enemy_Node *AddEnemy(enemy_Node* enemies, int x, int y, int rx, int ry, int shot, int speed, int life, int direction, enum ETYPE type);
void FreeEnemy(enemy_Node* enemy);
bullet_Node *AddBullet(bullet_Node* bullets, int x, int y, int dx, int dy, enum BTYPE type);
void FreeBullet(bullet_Node* bullet);
// ====== オブジェクト管理 ======
void MovePlayer();
void MoveEnemy();
void MoveBullet();
void ShotEnemyBullet(int value);
void isCollided(void);
void isEnemyCollided(void);
// ====== シーンごとの処理 ======
void Title(void);
void Manual(void);
void Run(void);
void Pause(void);
void GameOver(void);
void Clear(void);
void FadeIn(void);
// ====== 表示制御 ======
void Display(void);
// ====== 画像表示 ======
void PutSprite(int num, int x, int y, pngInfo *info, int r, int g, int b, int a);
void PutImgNum(int x, int y, char str, int r, int g, int b, int a);
void PutImgNumbers(int x, int y, char *s, int r, int g, int b, int a);
// ====== 操作制御 ======
void Keyboard(unsigned char, int, int);
void KeyboardUp(unsigned char, int, int);
void SpecialKey(int, int, int);
void SpecialKeyUp(int, int, int);
// ==============================================
/* ______________________________________________________________ */


/* __________________________関数__________________________ */
// ====== main関数 ======
// ウィンドウの初期化, ゲーム情報初期化, アルファチャネルの有効化, 画像読み込み, コールバック関数の登録, イベントループ突入
int main(int argc, char **argv)
{
  int scrWidth = 0, scrHeight = 0; // 画面全体のサイズ

  // ウィンドウ初期化, 乱数初期化
  srandom((unsigned int)time(NULL));
  glutInit(&argc, argv);
  scrWidth = glutGet(GLUT_SCREEN_WIDTH);
  scrHeight = glutGet(GLUT_SCREEN_HEIGHT);
  glutInitWindowSize(WWIDTH, WHEIGHT);
  glutCreateWindow("PlayGround [BETA] ---RELEASE MODE---");
  glutPositionWindow((scrWidth - WWIDTH) / 2, (scrHeight - WHEIGHT) / 2);
  glutInitDisplayMode(GLUT_RGBA | GLUT_ALPHA);
  glClearColor(1.0, 1.0, 1.0, 1.0);
  Initialize();

  // アルファチャネルの有効化
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

  // 画像読み込み
  ImportImages();

  // コールバック関数の登録
  glutReshapeFunc(Reshape);
  glutTimerFunc(20, Transition, 0);
  glutTimerFunc(100, ShotEnemyBullet, 0);
  glutDisplayFunc(Display);
  glutKeyboardFunc(Keyboard);
  glutKeyboardUpFunc(KeyboardUp);
  glutSpecialFunc(SpecialKey);
  glutSpecialUpFunc(SpecialKeyUp);

  // イベントループ突入
  glutMainLoop();

  return 0;
}
// ==============================================
//
// ====== Reshape関数 ======
// ウィンドウサイズ変更時にウィンドウを再描画（サイズは固定）
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
// ==============================================
//
// ====== ImportImages関数 ======
// 画像の読み込み
void ImportImages()
{
  int i;
  char str[32]; // 画像のファイル名・パスを格納

  // プレイヤー画像の読み込み
  sprintf(str, "./images/player/player.png");
  img_pl = pngBind(str, PNG_NOMIPMAP, PNG_ALPHA, &info_pl, GL_CLAMP, GL_NEAREST, GL_NEAREST);

  // 敵キャラ（円, 四角, 三角）画像の読み込み
  for (i = 0; i < 3; i++) {
    sprintf(str, "./images/enemy/enemy%d.png", i);
    img_en[i] = pngBind(str, PNG_NOMIPMAP, PNG_ALPHA, &info_en[i], GL_CLAMP, GL_NEAREST, GL_NEAREST);
  }

  // 敵キャラ（弾幕展開）画像の読み込み
  sprintf(str, "./images/enemy/enemy_shot.png");
  img_en_shot = pngBind(str, PNG_NOMIPMAP, PNG_ALPHA, &info_en_shot, GL_CLAMP, GL_NEAREST, GL_NEAREST);

  // 弾画像の読み込み
  sprintf(str, "./images/enemy/dot.png");
  img_en_bullet = pngBind(str, PNG_NOMIPMAP, PNG_ALPHA, &info_en_bullet, GL_CLAMP, GL_NEAREST, GL_NEAREST);

  // 背景画像の読み込み
  sprintf(str, "./images/scr/background.png");
  img_bg = pngBind(str, PNG_NOMIPMAP, PNG_ALPHA, &info_bg, GL_CLAMP, GL_NEAREST, GL_NEAREST);

  // フレーム画像の読み込み
  sprintf(str, "./images/scr/frame.png");
  img_fr = pngBind(str, PNG_NOMIPMAP, PNG_ALPHA, &info_fr, GL_CLAMP, GL_NEAREST, GL_NEAREST);

  // タイトル画面の画像の読み込み
  sprintf(str, "./images/scr/title.png");
  img_title = pngBind(str, PNG_NOMIPMAP, PNG_ALPHA, &info_title, GL_CLAMP, GL_NEAREST, GL_NEAREST);

  // ポーズ画面の画像の読み込み
  sprintf(str, "./images/scr/pause.png");
  img_pause = pngBind(str, PNG_NOMIPMAP, PNG_ALPHA, &info_pause, GL_CLAMP, GL_NEAREST, GL_NEAREST);

  // マニュアル画面の画像の読み込み
  sprintf(str, "./images/scr/manual.png");
  img_manual = pngBind(str, PNG_NOMIPMAP, PNG_ALPHA, &info_manual, GL_CLAMP, GL_NEAREST, GL_NEAREST);

  // ゲームオーバー画面の画像の読み込み
  sprintf(str, "./images/scr/gameover.png");
  img_gmovr = pngBind(str, PNG_NOMIPMAP, PNG_ALPHA, &info_gmovr, GL_CLAMP, GL_NEAREST, GL_NEAREST);

  // クリア画面の画像の読み込み
  sprintf(str, "./images/scr/clear.png");
  img_clr = pngBind(str, PNG_NOMIPMAP, PNG_ALPHA, &info_clr, GL_CLAMP, GL_NEAREST, GL_NEAREST);

  // "START"メニューの画像の読み込み
  sprintf(str, "./images/menu/start.png");
  img_start = pngBind(str, PNG_NOMIPMAP, PNG_ALPHA, &info_start, GL_CLAMP, GL_NEAREST, GL_NEAREST);

  // "MANUAL"メニューの画像の読み込み
  sprintf(str, "./images/menu/gomanual.png");
  img_gomanual = pngBind(str, PNG_NOMIPMAP, PNG_ALPHA, &info_gomanual, GL_CLAMP, GL_NEAREST, GL_NEAREST);

  // "RESUME"メニューの画像の読み込み
  sprintf(str, "./images/menu/resume.png");
  img_resume = pngBind(str, PNG_NOMIPMAP, PNG_ALPHA, &info_resume, GL_CLAMP, GL_NEAREST, GL_NEAREST);

  // "QUIT"メニューの画像の読み込み
  sprintf(str, "./images/menu/quit.png");
  img_quit = pngBind(str, PNG_NOMIPMAP, PNG_ALPHA, &info_quit, GL_CLAMP, GL_NEAREST, GL_NEAREST);

  // "TITLE"メニューの画像の読み込み
  sprintf(str, "./images/menu/gotitle.png");
  img_gotitle = pngBind(str, PNG_NOMIPMAP, PNG_ALPHA, &info_gotitle, GL_CLAMP, GL_NEAREST, GL_NEAREST);

  // 数字に対応する画像の読み込み
  for (i = 0; i < 10; i++) {
    sprintf(str, "./fonts/%d.png", i); // ファイル名はすべてその数字に対応
    img_num[i] = pngBind(str, PNG_NOMIPMAP, PNG_ALPHA, &info_num[i], GL_CLAMP, GL_NEAREST, GL_NEAREST);
  }
}
// ==============================================
//
// ====== Initialize関数 ======
// ゲーム情報の初期化
void Initialize()
{
  int i;
  struct e_Node *ep = enemies, *e_temp;
  struct b_Node *bp = bullets, *b_temp;

  bg1_pos = 0;    //
  bg2_pos= -960;  // 背景画像の座標を配置

  // プレイヤー情報の初期化
  player.x = 240;
  player.y = 476;
  player.speed = 12;
  player.collision = 0;
  player.life = 100;
  player.shot = 0;
  player.graze = 0;

  // 敵キャラのリスト構造を消去
  while (ep != NULL) {
    e_temp = ep->next;
    FreeEnemy(ep);
    ep = e_temp;
  }
  enemies = NULL;
  // 敵キャラのリスト構造を最大数構築
  for (i = 0; i < ENEMIES_MAX; i++) {
    enemies = AddEnemy(enemies, random() % 368 + 40, (random() % 100), 0, 0, 0, (random() % 4 + 1), 20, random() % 2, random() % 4);
  }
  enemiesCnt = i; // 敵キャラ数を記憶

  // 弾のリスト構造を消去
  while (bp != NULL) {
    b_temp = bp->next;
    FreeBullet(bp);
    bp = b_temp;
  }
  bullets = NULL;

  // 方向キー入力情報をリセット
  for (i = 0; i < 4; i++) {
    ctrlDirection[i] = 0;
  }

  score = 0;  // スコアリセット
  killedEnemies = 0;  // 敵撃破数リセット
}
// ==============================================
//
// ====== Transition関数 ======
// シーン遷移の処理
void Transition(int value)
{
  switch (scene) {
  // タイトル画面に移行
  case TITLE:
    Title();
    break;

  // マニュアル画面に移行
  case MANUAL:
    Manual();
    break;

  // ゲーム画面に移行
  case RUN:
    Run();
    break;

  // ポーズ画面に移行
  case PAUSE:
    Pause();
    break;

  // ゲームオーバー画面に移行
  case GAMEOVER:
    GameOver();
    break;

  // クリア画面に移行
  case CLEAR:
    Clear();
    break;

  // フェードイン画面に移行
  case FADEIN:
    FadeIn();
    break;

  // シーン遷移エラー
  default:
    printf("scene error\n");
    scene = TITLE;
    break;
  }

  // Display関数を強制呼び出し
  glutPostRedisplay();

  // 20ms秒後に自身をコールバック
  glutTimerFunc(20, Transition, 0);
}
// ==============================================
//
// ====== AddEnemy関数 ======
// 敵キャラのリスト構造に新しくノードを追加
enemy_Node *AddEnemy(enemy_Node* enemies, int x, int y, int rx, int ry, int shot, int speed, int life, int direction, enum ETYPE type)
{
  // 新しいノードの作成
  enemy_Node *node;
  node = (enemy_Node *)malloc(sizeof(enemy_Node));  // メモリ領域確保
  if (type == CUBE || type == TRI || type == SHOT) {  // 四角形, 三角形, 弾幕展開型キャラの場合
    if (direction == LEFT) {  // 左方向に進む場合
      node->enemy.x = 420;  // 画面右側に生成
    }
    else if (direction == RIGHT) {  // 右方向に進む場合
      node->enemy.x = -10;  // 画面左側に生成
    }
    else {  // それ以外
      node->enemy.x = x;  // 引数に対応するx座標上に生成
    }
    node->enemy.y = 0;  // 画面上側に生成
  }
  else if (type == CIRCLE) {  // 円形の敵キャラの場合
    node->enemy.x = x;  // 引数に対応するx座標上に生成
    node->enemy.y = 0;  // 画面上側に生成
  }
  else {  // それ以外
    node->enemy.x = x;  // 引数に対応するx座標上に生成
    node->enemy.y = y;  // 引数に対応するy座標上に生成
  }
  node->enemy.rx = rx;  // x方向の移動量
  node->enemy.ry = ry;  // y方向の移動量
  node->enemy.shot = shot;    // 発射フラグ
  node->enemy.speed = speed;  // 速度
  node->enemy.life = life;    // HP
  node->enemy.direction = direction;  // 移動方向
  node->enemy.type = type;  // 敵キャラの種類

  // 次の要素をNULLに指定
  node->next = NULL;

  if (enemies == NULL) {  // リストが空の場合
    node->prev = NULL; // 前のノードはないのでNULLに設定
    return node;  // 登録したノードのアドレスを返す
  }
  else {  // ノードが存在するとき
    // リストの末尾のノードを探す
    enemy_Node *p = enemies;
    while (p->next != NULL) {
        p = p->next;
    }
    // 前のノードと登録したノードを連結
    p->next = node; // 前のノードの次を示すポインタに登録したノードのアドレスを渡す
    node->prev = p; // 登録したノードの前を示すポインタに前のノードのアドレスを渡す

    // リスト構造の先頭アドレスを返す
    return enemies;
  }
}
// ==============================================
//
// ====== FreeEnemy関数 ======
// 指定した敵キャラのノードを消去
void FreeEnemy(enemy_Node* enemy)
{
  if (enemy->next == NULL) { // 末尾のノードを消去するとき
    if (enemy->prev == NULL) { // 前のノードも存在しないとき
      enemies = NULL; // リストの先頭アドレスをNULLに戻す
    }
    else {
      enemy->prev->next = NULL; // 前のノードの次を示すポインタにNULLを渡す
    }
    free(enemy); // ノードを消去
  }
  else {
    if (enemy->prev == NULL) {  // 先頭ノードを消去するとき
      enemy->next->prev = NULL; // 次ノードの前ノードを示すポインタに, NULLを設定
      enemies = enemy->next;  // 消去するノードの次ノードのアドレスをリスト構造の先頭に指定
      free(enemy); // ノードを消去
    }
    else {  // 先頭, 末尾以外のノードを消去するとき
      enemy->prev->next = enemy->next;  // 前ノードの次ノードを示すポインタに, 消去対象ノードの次ノードアドレスを渡す
      enemy->next->prev = enemy->prev;  // 次ノードの前ノードを示すポインタに, 消去対象ノードの前ノードアドレスを渡す
      free(enemy); // ノードを消去
    }
  }
}
// ==============================================
//
// ====== AddBullet関数 ======
// 弾のリスト構造に新しくノードを追加
bullet_Node *AddBullet(bullet_Node* bullets, int x, int y, int dx, int dy, enum BTYPE type)
{
  // 新しいノードの作成
  bullet_Node *node;
  node = (bullet_Node *)malloc(sizeof(bullet_Node));  // メモリ領域確保
  node->bullet.x = x; // x座標
  node->bullet.y = y; // y座標
  node->bullet.dx = dx; // x方向のベクトル
  node->bullet.dy = dy; // y方向のベクトル
  node->bullet.type = type; // 弾の種類

  // 次の要素をNULLに指定
  node->next = NULL;

  if (bullets == NULL) {  // リストが空の場合
    node->prev = NULL;  // 前ノードはないのでNULLに設定
    return node;  // 登録したノードのアドレスを返す
  }
  else {
    // リストの末尾のノードを探す
    bullet_Node *p = bullets;
    while (p->next != NULL) {
        p = p->next;
    }
    // 前のノードと登録したノードを連結
    p->next = node; // 前のノードの次を示すポインタに登録したノードのアドレスを渡す
    node->prev = p; // 登録したノードの前を示すポインタに前のノードのアドレスを渡す

    // リスト構造の先頭アドレスを返す
    return bullets;
  }
}
// ==============================================
//
// ====== FreeBullet関数 ======
// 指定した弾のノードを消去
void FreeBullet(bullet_Node* bullet)
{
  if (bullet->next == NULL) { // 末尾のノードを消去するとき
    if (bullet->prev == NULL) { // 前のノードも存在しないとき
      bullets = NULL; // リストの先頭アドレスをNULLに戻す
    }
    else {
      bullet->prev->next = NULL; // 前ノードの次ノードを示すポインタにNULLを設定
    }
    free(bullet); // ノードを消去
  }
  else {
    if (bullet->prev == NULL) {  // 先頭ノードを消去するとき
      bullet->next->prev = NULL; // 次ノードの前ノードを示すポインタに, NULLを設定
      bullets = bullet->next;  // 消去するノードの次ノードのアドレスをリスト構造の先頭に指定
      free(bullet); // ノードを消去
    }
    else {
      bullet->prev->next = bullet->next;  // 前ノードの次ノードを示すポインタに, 消去対象ノードの次ノードアドレスを渡す
      bullet->next->prev = bullet->prev;  // 次ノードの前ノードを示すポインタに, 消去対象ノードの前ノードアドレスを渡す
      free(bullet); // ノードを消去
    }
  }
}
// ==============================================
//
// ====== MovePlayer関数 ======
// プレイヤーの動作制御
void MovePlayer()
{
  if (ctrlDirection[0] == 1) {  // 上方向
    if (player.y > 64) {
      player.y -= player.speed;
    }
    else {
      player.y = 64;
    }
  }
  if (ctrlDirection[1] == 1) {  // 右方向
    if (player.x < 416) {
      player.x += player.speed;
    }
    else {
      player.x = 416;
    }
  }
  if (ctrlDirection[2] == 1) {  // 下方向
    if (player.y < 476) {
      player.y += player.speed;
    }
    else {
      player.y = 476;
    }
  }
  if (ctrlDirection[3] == 1) {  // 左方向
    if (player.x > 64) {
      player.x -= player.speed;
    }
    else {
      player.x = 64;
    }
  }
}
// ==============================================
//
// ====== MoveEnemy関数 ======
// 敵キャラの動作制御
void MoveEnemy()
{
  enemy_Node *p = enemies;  //
  enemy_Node *temp;         // 作業用ポインタ（敵キャラリストのノード）

  while (p != NULL) { // すべてのノードに対して実行
    temp = p->next; // 次ノードを保持
    if ((p->enemy.x < -20 || p->enemy.x > 440) || (p->enemy.y < -20 || p->enemy.y > 500) || (p->enemy.life <= 0)) { // 表示範囲外に出た場合
      enemiesCnt--; // 敵キャラ数を減らす
      FreeEnemy(p); // ノードを消去
    }
    else {
      if (p->enemy.type == CIRCLE) {  // 円形の敵キャラの場合
        p->enemy.y += p->enemy.speed; // 垂直方向にのみ移動
        p->enemy.ry += p->enemy.speed;  // 移動量を更新
        if (p->enemy.ry >= 100) { // y方向の移動量が100以上の場合
          bullets = AddBullet(bullets, p->enemy.x+24, p->enemy.y+32, 0, 3, DOT);  // 弾を発射
          p->enemy.ry = 0;  // 移動量リセット
        }
      }
      else if (p->enemy.type == CUBE || p->enemy.type == SHOT) {  // 四角形, 弾幕展開型の敵キャラの場合
        if (p->enemy.direction == LEFT) { // 左方向に移動する場合
          p->enemy.x -= p->enemy.speed/2 + 1; // 水平方向の移動
          p->enemy.y += p->enemy.speed; // 垂直方向の移動
          p->enemy.rx += p->enemy.speed/2 + 1;  //
          p->enemy.ry += p->enemy.speed;        // 移動量を更新
        }
        else if (p->enemy.direction == RIGHT) { // 右方向に移動する場合
          p->enemy.x += p->enemy.speed/2 + 1; // 水平方向の移動
          p->enemy.y += p->enemy.speed; // 垂直方向の移動
          p->enemy.rx += p->enemy.speed/2 + 1;  //
          p->enemy.ry += p->enemy.speed;        // 移動量を更新
        }
        if (p->enemy.type == SHOT) {  // 弾幕展開型の敵キャラの場合
          p->enemy.shot = 1;  // 発射フラグを立てる
        }
        else {  // 四角形の敵キャラの場合
          if (p->enemy.ry >= 100) { // y方向の移動量が100以上の場合
            bullets = AddBullet(bullets, p->enemy.x+24, p->enemy.y+32, 0, 3, DOT);  // 弾を発射
            p->enemy.ry = 0;  // 移動量リセット
          }
        }
      }
      else if (p->enemy.type == TRI) {  // 三角形の敵キャラの場合
        if (p->enemy.direction == LEFT) { // 左方向に移動する場合
          p->enemy.x -= p->enemy.speed; // 水平方向の移動
          p->enemy.y += p->enemy.speed; // 垂直方向の移動
          p->enemy.rx += p->enemy.speed;  //
          p->enemy.ry += p->enemy.speed;  // 移動量を更新
          if (p->enemy.ry >= 50) {  // y方向の移動量が50以上の場合
            bullets = AddBullet(bullets, p->enemy.x+24, p->enemy.y+32, 0, 3, DOT);  // 弾を発射
            p->enemy.ry = 0;  // 移動量リセット
          }
          if (p->enemy.rx >= 200) { // x方向の移動量が200以上の場合
            p->enemy.direction = RIGHT; // 右方向に移動
            p->enemy.rx = 0;  // 移動量リセット
          }
        }
        else if (p->enemy.direction == RIGHT) { // 右方向に移動する場合
          p->enemy.x += p->enemy.speed; // 水平方向の移動
          p->enemy.y += p->enemy.speed; // 垂直方向の移動
          p->enemy.rx += p->enemy.speed;  //
          p->enemy.ry += p->enemy.speed;  // 移動量を更新
          if (p->enemy.ry >= 50) {  // y方向の移動量が50以上の場合
            bullets = AddBullet(bullets, p->enemy.x+24, p->enemy.y+32, 0, 3, DOT);  // 弾を発射
            p->enemy.ry = 0;  // 移動量リセット
          }
          if (p->enemy.rx >= 200) { // x方向の移動量が200以上の場合
            p->enemy.direction = LEFT; // 左方向に移動
            p->enemy.rx = 0;  // 移動量リセット
          }
        }
        else {  // 移動方向が左右以外の場合
          p->enemy.y += p->enemy.speed;   // 垂直方向にのみ移動
          p->enemy.ry += p->enemy.speed;  // 移動量を更新
        }
      }
    }
    p = temp; // 保持しておいた次ノードに移行
  }
}
// ==============================================
//
// ====== MoveBullet関数 ======
// 弾の動作制御
void MoveBullet()
{
  bullet_Node *p = bullets;  //
  bullet_Node *temp;         // 作業用ポインタ（弾リストのノード）
  int b_free = 0; // 弾ノード消去フラグ

  while (p != NULL) { // すべてのノードに対して実行
    temp = p->next; // 次ノードを保持
    if (p->bullet.x > 0 && p->bullet.x <= 480) {  // 表示範囲内に弾がある場合
      p->bullet.x += p->bullet.dx;  // 水平方向の移動
    }
    else {  // 表示範囲外に出た場合
      b_free = 1; // フラグを立てる
    }
    if (p->bullet.y > 0 && p->bullet.y <= 500) {  // 表示範囲内に弾がある場合
      p->bullet.y += p->bullet.dy;  // 垂直方向の移動
    }
    else {  // 表示範囲外に出た場合
      b_free = 1; // フラグを立てる
    }

    if (b_free == 1) {  // ノード消去フラグが立っている場合
      FreeBullet(p);  // ノードを消去
      b_free = 0; // フラグリセット
    }
    p = temp; // 保持しておいた次ノードに移行
  }
}
// ==============================================
//
// ====== ShotEnemyBullet関数 ======
// 弾幕の制御
void ShotEnemyBullet(int value)
{
  enemy_Node *ep = enemies; // 作業用ポインタ（敵キャラリストのノード）

  while (ep != NULL) {  // すべてのノードに対して実行
    if (ep->enemy.shot == 1) {  // 発射フラグが立っている場合
      bullets = AddBullet(bullets, ep->enemy.x+20, ep->enemy.y+48, (random()%5)-2, (random()%3)+1, DOT);  // 弾を発射
    }
    ep = ep->next;  // 次ノードに移行
  }

  // 100msごとに再呼び出し
  glutTimerFunc(100, ShotEnemyBullet, 0);
}
// ==============================================
//
// ====== isCollided関数 ======
// プレイヤーの衝突, グレイズ判定
void isCollided(void)
{
  int cflag = 0;  // 衝突判定フラグ
  enemy_Node *ep = enemies; // 作業用ポインタ（敵キャラリストのノード）
  bullet_Node *bp = bullets;  //
  bullet_Node *b_temp;        // 作業用ポインタ（弾リストのノード）

  while (ep != NULL) {  // すべての敵キャラノードに対して実行
    if (ep->enemy.type == SHOT) { // 弾幕展開型敵キャラの場合
      if (((player.x-3 >= ep->enemy.x+8 && player.x-3 <= ep->enemy.x+40) || (player.x+3 >= ep->enemy.x+8 && player.x+3 <= ep->enemy.x+40)) && ((player.y-3 >= ep->enemy.y+8 && player.y-3 <= ep->enemy.y+40) || (player.y+3 >= ep->enemy.y+8 && player.y+3 <= ep->enemy.y+40))) { // 敵キャラの中心32x32ピクセルの衝突範囲にプレイヤーの6x6ピクセルの衝突範囲が存在するとき
        cflag = 1;  // 衝突判定フラグを立てる
      }
      else if ((player.x-3 >= ep->enemy.x && player.x-3 <= ep->enemy.x+48) || (player.x+3 >= ep->enemy.x && player.x+3 <= ep->enemy.x+48)) {
        if ((player.y-3 >= ep->enemy.y && player.y-3 <= ep->enemy.y+48) || (player.y+3 >= ep->enemy.y && player.y+3 <= ep->enemy.y+48)) { // 敵キャラの衝突範囲の周り4ピクセルにプレイヤーの6x6ピクセルの衝突範囲が存在するとき
          player.graze++; // グレイズ値を増加
          score += 1200;  // 弾幕展開型敵キャラのグレイズスコア : 1200点
        }
      }
    }
    else {  // 円形, 四角形, 三角形の敵キャラの場合
      if (((player.x-3 >= ep->enemy.x+8 && player.x-3 <= ep->enemy.x+24) || (player.x+3 >= ep->enemy.x+8 && player.x+3 <= ep->enemy.x+24)) && ((player.y-3 >= ep->enemy.y+8 && player.y-3 <= ep->enemy.y+24) || (player.y+3 >= ep->enemy.y+8 && player.y+3 <= ep->enemy.y+24))) { // 敵キャラの中心16x16ピクセルの衝突範囲にプレイヤーの6x6ピクセルの衝突範囲が存在するとき
        cflag = 1;  // 衝突判定フラグを立てる
      }
      else if ((player.x-3 >= ep->enemy.x && player.x-3 <= ep->enemy.x+32) || (player.x+3 >= ep->enemy.x && player.x+3 <= ep->enemy.x+32)) {
        if ((player.y-3 >= ep->enemy.y && player.y-3 <= ep->enemy.y+32) || (player.y+3 >= ep->enemy.y && player.y+3 <= ep->enemy.y+32)) { // 敵キャラの衝突範囲の周り8ピクセルにプレイヤーの6x6ピクセルの衝突範囲が存在するとき
          player.graze++; // グレイズ値を増加
          score += 1000;  // 弾幕展開型以外の敵キャラのグレイズスコア : 1000点
        }
      }
    }
    ep = ep->next;  // 次ノードに移行
  }

  if (cflag == 1) { // 衝突判定フラグが立っているとき
    player.collision = 1; // プレイヤーの衝突フラグを立てる
    if (player.life > 0) {  // プレイヤーのHPが0より多いとき
      player.life -= 30;  // HPを30減らす
    }
    else {  // プレイヤーのHPが0以下のとき
      player.life = 0;  // HPを0に再設定
      scene = GAMEOVER; // ゲームオーバー画面に移行
    }
    cflag = 0;  // 衝突判定フラグをリセット
  }
  else {  // 衝突判定フラグが立っていないとき
    player.collision = 0; // プレイヤーの衝突フラグをリセット
  }

  while (bp != NULL) {  // すべての弾ノードに対して実行
    b_temp = bp->next;  // 次ノードを保持
    if (bp->bullet.type == DOT) { // 敵キャラの弾の場合
      if (((player.x-3 >= bp->bullet.x+4 && player.x-3 <= bp->bullet.x+12) || (player.x+3 >= bp->bullet.x+4 && player.x+3 <= bp->bullet.x+12)) && ((player.y-3 >= bp->bullet.y+4 && player.y-3 <= bp->bullet.y+12) || (player.y+3 >= bp->bullet.y+4 && player.y+3 <= bp->bullet.y+12))) { // 弾の中心8x8ピクセルの衝突範囲にプレイヤーの6x6ピクセルの衝突範囲が存在するとき
        cflag = 1;  // 衝突判定フラグを立てる
        if (player.life > 0) {  // プレイヤーのHPが0を超えるとき
          player.life -= 5; // HPを5減らす
        }
        else {  // プレイヤーのHPが0以下のとき
          player.life = 0;  // HPを0に再設定
          scene = GAMEOVER; // ゲームオーバー画面に移行
        }
        FreeBullet(bp); // ノードを消去
      }
      else if (((player.x-3 >= bp->bullet.x && player.x-3 <= bp->bullet.x+16) || (player.x+3 >= bp->bullet.x && player.x+3 <= bp->bullet.x+16)) && ((player.y-3 >= bp->bullet.y && player.y-3 <= bp->bullet.y+16) || (player.y+3 >= bp->bullet.y && player.y+3 <= bp->bullet.y+16))) { // 弾の衝突範囲の周り4ピクセルにプレイヤーの6x6ピクセルの衝突範囲が存在するとき
          player.graze++; // グレイズ値を増加
          score += 500;   // 弾のグレイズスコア : 500点
      }
    }
    bp = b_temp;  // 保持しておいた次ノードに移行
  }
}
// ==============================================
//
// ====== isEnemyCollided関数 ======
// 敵キャラの衝突判定
void isEnemyCollided(void)
{
  enemy_Node *ep = enemies; //
  enemy_Node *e_temp;       // 作業用ポインタ（敵キャラリストのノード）
  bullet_Node *bp = bullets; //
  bullet_Node *b_temp;       // 作業用ポインタ（弾リストのノード）

  while (bp != NULL) {  // すべての弾ノードに対して実行
    b_temp = bp->next;  // 次ノードを保持
    ep = enemies; // 敵キャラリストの先頭を取得
    while (ep != NULL) {  // すべての敵キャラノードに対して実行
      e_temp = ep->next;  // 次ノードを保持
      if (bp->bullet.type == BEAM && (ep->enemy.type == CIRCLE || ep->enemy.type == CUBE || ep->enemy.type == TRI || ep->enemy.type == SHOT)) { // ノードがプレイヤーの弾かつ円形, 四角形, 三角形, 弾幕展開型敵キャラの場合
        if (((bp->bullet.x >= ep->enemy.x && bp->bullet.x <= ep->enemy.x+32) || (bp->bullet.x+4 >= ep->enemy.x && bp->bullet.x+4 <= ep->enemy.x+32)) && ((bp->bullet.y >= ep->enemy.y && bp->bullet.y <= ep->enemy.y+32) || (bp->bullet.y+12 >= ep->enemy.x && bp->bullet.y+12 <= ep->enemy.y+32))) { // 4x12ピクセルの衝突範囲に敵キャラの32x32ピクセルの衝突範囲が存在するとき
          ep->enemy.life--; // 敵キャラのHPを1減少
          if (ep->enemy.life <= 0) {  // HPが0以下の場合
            switch (ep->enemy.type) { // 敵キャラの種類ごとに分岐
            case CIRCLE:  // 円形の敵キャラの場合
              score += 100; // 撃破スコア : 100点
              break;

            case CUBE:  // 四角形の敵キャラの場合
              score += 300; // 撃破スコア : 300点
              break;

            case TRI:  // 三角形の敵キャラの場合
              score += 500; // 撃破スコア : 500点
              break;

            case SHOT:  // 弾幕展開型敵キャラの場合
              score += 1000; // 撃破スコア : 1000点
              break;

            default:
              break;
            }
            enemiesCnt--; // 敵キャラのカウントを減らす
            killedEnemies++;  // 敵キャラ撃破数を増加
            FreeBullet(bp); // 当たった弾ノードを消去
            FreeEnemy(ep);  // 撃破した敵ノードを消去
            break;  // ループを抜ける
          }
          else {  // 敵が生きている場合
            FreeBullet(bp); // 当たった弾ノードを消去
            break;  // ループを抜ける
          }
        }
      }
      ep = e_temp;  // 保持しておいた次の敵キャラノードへ移行
    }
    bp = b_temp;  // 保持しておいた次の弾ノードへ移行
  }
}
// ==============================================
//
// ====== Title関数 ======
// タイトル画面でのアニメーション, オブジェクト座標処理
void Title(void)
{
  // フェードイン後, フェードアウトを行う
  if (fade > 0) { // 不透明度が0より大きい間
    fade -= 8;  // 不透明度を8減少
  }
  else{}
}
// ==============================================
//
// ====== Manual関数 ======
// マニュアル画面でのアニメーション, オブジェクト座標処理
void Manual(void)
{
  // シーンごとの処理形式統一のため, 関数内が空でも関数を定義しておく
}
// ==============================================
//
// ====== Run関数 ======
// ゲーム画面でのアニメーション, オブジェクト座標処理
void Run(void)
{
  bg1_pos+=2; //
  bg2_pos+=2; // 背景のスクロール
  if (bg1_pos >= 640) { // 1枚目の背景がウィンドウ下まで移動した場合
    bg1_pos = bg2_pos - 960;  // 2枚目の背景の上に連結
  }
  else if (bg2_pos >= 640) { // 2枚目の背景がウィンドウ下まで移動した場合
    bg2_pos = bg1_pos - 960;  // 1枚目の背景の上に連結
  }

  // フェードイン後, フェードアウトを行う
  if (fade > 0) { // オーバーレイ画面（フェードイン）の不透明度が0より大きい場合
    fade -= 8;  // 不透明度を減少
  }
  else {  // オーバーレイ画面（フェードイン）が非表示の場合
    score++;  // スコアを加算（時間経過スコア : 1点/20m[sec]）

    if (player.shot == 1) { // プレイヤーの発射フラグが立っている場合
      bullets = AddBullet(bullets, player.x-12, player.y-8, 0, -20, BEAM);  //
      bullets = AddBullet(bullets, player.x+8, player.y-8, 0, -20, BEAM);   // プレイヤーから2つ弾を発射
    }

    MovePlayer(); // プレイヤーの動作処理
    MoveBullet(); // 弾の動作処理
    MoveEnemy();  // 敵キャラの動作処理

    isCollided(); // プレイヤーの衝突判定
    isEnemyCollided();  // 敵キャラの衝突判定

    if (enemiesCnt < ENEMIES_MAX) { // 画面上の敵キャラが最大数より少ない場合
      enemies = AddEnemy(enemies, random() % 368 + 40, (random() % 100) + 40, 0, 0, 0, (random() % 4 + 1), 20, random() % 2, random() % 4); // 敵キャラを追加
      enemiesCnt++; // 敵キャラのカウンタの値を増加
    }

    if (killedEnemies >= 100) { // 敵キャラを100体以上倒した場合
      score += 10000; // ゲームクリアスコア : 10000点
      scene = CLEAR;  // クリア画面に移行
    }
  }
}
// ==============================================
//
// ====== Pause関数 ======
// ポーズ画面でのアニメーション, オブジェクト座標処理
void Pause(void)
{
  // シーンごとの処理形式統一のため, 関数内が空でも関数を定義しておく
}
// ==============================================
//
// ====== GameOver関数 ======
// ゲームオーバー画面でのアニメーション, オブジェクト座標処理
void GameOver(void)
{
  // シーンごとの処理形式統一のため, 関数内が空でも関数を定義しておく
}
// ==============================================
//
// ====== Clear関数 ======
// クリア画面でのアニメーション, オブジェクト座標処理
void Clear(void)
{
  // シーンごとの処理形式統一のため, 関数内が空でも関数を定義しておく
}
// ==============================================
//
// ====== FadeIn関数 ======
// フェードイン画面でのアニメーション, オブジェクト座標処理
// フェードインの実行
void FadeIn()
{
  if (fade < 255) {  // オーバーレイ画面（フェードイン）の不透明度が255より小さい場合
    fade += 8;  // 不透明度を8増加
  }
  else {  // オーバーレイ画面（フェードイン）の不透明度が255以上の場合
    scene = nextScene;  //フェードイン後のシーンへ移行
  }
}
// ==============================================
//
// ====== Display関数 ======
// ウィンドウ上の描画処理
void Display(void)
{
  char str_buf[16];
  bullet_Node *bp = bullets;  // 作業用ポインタ（弾リストのノード）
  enemy_Node *ep = enemies;   // 作業用ポインタ（敵キャラリストのノード）

  // ウィンドウの背景色
  glClear(GL_COLOR_BUFFER_BIT);

  if (scene == TITLE || (scene == MANUAL && nextScene == TITLE) || (scene == FADEIN && nextScene == RUN)) { // タイトルシーン, タイトルシーン中のマニュアルシーン, ゲームシーンへのフェードインシーンの場合
    PutSprite(img_title, 0, 0, &info_title, 255, 255, 255, 255);  // タイトル画面の画像を描画

    PutSprite(img_start, 54, 379, &info_start, 0, 155, 133, menu_select==0 ? 255 : 0);  // "START"メニューの影を描画（選択時）
    PutSprite(img_gomanual, 24, 474, &info_gomanual, 0, 155, 133, menu_select==1 ? 255 : 0);  // "MANUAL"メニューの影を描画（選択時）
    PutSprite(img_quit, 34, 564, &info_quit, 0, 155, 133, menu_select==2 ? 255 : 0);  // "QUIT"メニューの影を描画（選択時）

    PutSprite(img_start, 50, 375, &info_start, 255, 255, 255, 255);  // "START"メニューの画像を描画
    PutSprite(img_gomanual, 20, 470, &info_gomanual, 255, 255, 255, 255);  // "MANUAL"メニューの画像を描画
    PutSprite(img_quit, 30, 560, &info_quit, 255, 255, 255, 255);  // "QUIT"メニューの画像を描画
  }

  if (scene == RUN || scene == PAUSE || scene == GAMEOVER || scene == CLEAR || (scene == MANUAL && nextScene == PAUSE) || (scene == FADEIN && nextScene == TITLE)) { // ゲームシーン, ポーズシーン, ゲームオーバーシーン, クリアシーン, ポーズシーン中のマニュアルシーン, タイトルシーンへのフェードインシーンの場合
    PutSprite(img_bg, 0, bg1_pos, &info_bg, 255, 255, 255, 255);  //
    PutSprite(img_bg, 0, bg2_pos, &info_bg, 255, 255, 255, 255);  // 2枚の背景画像を描画

    PutSprite(img_pl, player.x-24, player.y-24, &info_pl, 255, 255, 255, 255);  // プレイヤー画像を描画

    while (ep != NULL) {  // すべての敵キャラノードに対して実行
      if (ep->enemy.type == CIRCLE) { // 円形の敵キャラの場合
        PutSprite(img_en[0], ep->enemy.x, ep->enemy.y, &info_en[0], 255, 255, 255, 255);  // 円形の敵キャラ画像を描画
      }
      else if (ep->enemy.type == CUBE) { // 四角形の敵キャラの場合
        PutSprite(img_en[1], ep->enemy.x, ep->enemy.y, &info_en[1], 255, 255, 255, 255);  // 四角形の敵キャラ画像を描画
      }
      else if (ep->enemy.type == TRI) { // 三角形の敵キャラの場合
        PutSprite(img_en[2], ep->enemy.x, ep->enemy.y, &info_en[2], 255, 255, 255, 255);  // 三角形の敵キャラ画像を描画
      }
      else if (ep->enemy.type == SHOT) { // 弾幕展開型の敵キャラの場合
        PutSprite(img_en_shot, ep->enemy.x, ep->enemy.y, &info_en_shot, 255, 255, 255, 255);  // 弾幕展開型の敵キャラ画像を描画
      }
      ep = ep->next;  // 敵キャラの次ノードに移行
    }

    while (bp != NULL) {  // すべての弾ノードに対して実行
      if (bp->bullet.type == BEAM) {  // プレイヤーの弾の場合
        // 弾の描画
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
      if (bp->bullet.type == DOT) { // 敵キャラの弾の場合
        PutSprite(img_en_bullet, bp->bullet.x, bp->bullet.y, &info_en_bullet, 255, 255, 255, 255);  // 敵キャラの弾を描画
      }
      bp = bp->next;  // 弾の次ノードに移行
    }
  }

  if (scene == PAUSE || (scene == MANUAL && nextScene == PAUSE)) {  // ポーズシーン, ポーズシーン中のマニュアルシーンの場合
    PutSprite(img_pause, 0, 0, &info_pause, 255, 255, 255, 255);  // ポーズ画面の画像を描画

    PutSprite(img_resume, 179, 229, &info_resume, 0, 155, 133, menu_select==0 ? 255 : 0);   // "RESUME"メニューの影を描画（選択時）
    PutSprite(img_gomanual, 169, 279, &info_gomanual, 0, 155, 133, menu_select==1 ? 255 : 0); // "MANUAL"メニューの影を描画（選択時）
    PutSprite(img_gotitle, 199, 329, &info_gotitle, 0, 155, 133, menu_select==2 ? 255 : 0); // "TITLE"メニューの影を描画（選択時）

    PutSprite(img_resume, 175, 225, &info_resume, 255, 255, 255, 255);  // "RESUME"メニューの画像を描画
    PutSprite(img_gomanual, 165, 275, &info_gomanual, 255, 255, 255, 255);  // "MANUAL"メニューの画像を描画
    PutSprite(img_gotitle, 195, 325, &info_gotitle, 255, 255, 255, 255);  // "TITLE"メニューの画像を描画
  }

  if (scene == GAMEOVER || scene == CLEAR) {  // ゲームオーバーシーン, クリアシーンの場合
    if (scene == GAMEOVER) {  // ゲームオーバーシーンの場合
      PutSprite(img_gmovr, 0, 0, &info_gmovr, 255, 255, 255, 255);  // ゲームオーバー画面の画像を描画
    }
    else {  // クリアシーンの場合
      PutSprite(img_clr, 0, 0, &info_clr, 255, 255, 255, 255);  // クリア画面の画像を描画
    }

    // スコア, グレイズ, 敵キャラ撃破数の表示
    sprintf(str_buf, "%011d", score);
    PutImgNumbers(180, 231, str_buf, 255, 255, 255, 255);

    sprintf(str_buf, "%4d", player.graze);
    PutImgNumbers(306, 267, str_buf, 255, 255, 255, 255);

    sprintf(str_buf, "%03d", killedEnemies);
    PutImgNumbers(216, 305, str_buf, 255, 255, 255, 255);

  }

  if (scene == RUN || scene == PAUSE || scene == GAMEOVER || scene == CLEAR || (scene == MANUAL && nextScene == PAUSE) || (scene == FADEIN && nextScene == TITLE)) { // ゲームシーン, ポーズシーン, ゲームオーバーシーン, クリアシーン, ポーズシーン中のマニュアルシーン, タイトルシーンへのフェードインシーンの場合
    PutSprite(img_fr, 0, 0, &info_fr, 255, 255, 255, 255);  // フレーム画像を描画

    // スコアを描画
    sprintf(str_buf, "%015d", score);
    PutImgNumbers(160, 545, str_buf, 255, 255, 255, 255);

    // プレイヤーのゲージを表示（背景）
    glColor4ub(40, 40, 40, 128);
    glBegin(GL_QUADS);
    glVertex2i(125, 593);
    glVertex2i(125, 605);
    glVertex2i(225, 605);
    glVertex2i(225, 593);
    glEnd();
    // プレイヤーのゲージを表示
    if (player.life <= 10) {  // プレイヤーのHPが10以下の場合
      glColor4ub(255, 0, 0, 255); // ゲージを赤色に変更
    }
    else if (player.life <= 30) {  // プレイヤーのHPが30以下の場合
      glColor4ub(255, 255, 0, 255); // ゲージを黄色に変更
    }
    else {  // HPが30より多い場合
      glColor4ub(255, 255, 255, 255); // ゲージを白色に変更
    }
    // ゲージを描画
    glBegin(GL_QUADS);
    glVertex2i(125, 593);
    glVertex2i(125, 605);
    glVertex2i(player.life+125, 605);
    glVertex2i(player.life+125, 593);
    glEnd();

    // グレイズ値を表示
    sprintf(str_buf, "%4d", player.graze);
    PutImgNumbers(358, 589, str_buf, 255, 255, 255, 255);
  }

  if (scene == MANUAL) {  // マニュアルシーンの場合
    PutSprite(img_manual, 0, 0, &info_manual, 255, 255, 255, 255);  // マニュアル画面の画像を表示
  }

  // オーバーレイ画面（フェードイン）の不透明度調整
  if (fade < 0) {  // オーバーレイ画面（フェードイン）の不透明度が0より小さい場合
    fade = 0; // 0に変更
  }
  if (fade > 255) {  // オーバーレイ画面（フェードイン）の不透明度が255より大きい場合
    fade = 255; // 255に変更
  }
  // オーバーレイ画面（フェードイン）の描画
  glColor4ub(0, 0, 0, fade);
  glBegin(GL_QUADS);
  glVertex2i(0, 0);
  glVertex2i(0, WHEIGHT);
  glVertex2i(WWIDTH, WHEIGHT);
  glVertex2i(WWIDTH, 0);
  glEnd();

  glFlush();  // バッファ消去
}
// ==============================================
//
// ====== PutSprite関数 ======
// 画像の描画
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
// ==============================================
//
// ====== PutImgNum関数 ======
// 数字1桁を対応する画像で表示
void PutImgNum(int x, int y, char str, int r, int g, int b, int a)
{
  if (((int)str) - 48 < 0 || ((int)str) - 48 > 9) {
    return;
  }
  PutSprite(img_num[((int)str) - 48], x, y, &info_num[((int)str) - 48], r, g, b, a);  // ASCIIコードに対応する数字の画像を表示
}
// ==============================================
//
// ====== PutImgNum関数 ======
// 与えられた数字を順番に表示
void PutImgNumbers(int x, int y, char *s, int r, int g, int b, int a)
{
  int i = 0;

  // はじめの一桁を表示
  PutImgNum(x + (i * 18), y, s[0], r, g, b, a);
  for (i = 1; i < strlen(s); i++) { // 残りの桁を表示
    PutImgNum(x + (i * 18), y, s[i], r, g, b, a);
  }
}
// ==============================================
//
// ====== Keyboard関数 ======
// キーボード押下時の処理
void Keyboard(unsigned char key, int x, int y)
{
  if ((key == 'q')) {
    printf("End\n");
    exit(0);
  }
  if (key == 27) {
    if (scene == PAUSE) {
      scene = RUN;
    }
    else if (scene == RUN) {
      scene = PAUSE;
    }
    else if (scene == GAMEOVER || scene == CLEAR) {
      Initialize();
      scene = FADEIN;
      nextScene = TITLE;
    }
  }
  if (key == 'z' || key == 'Z' || key == 10 || key == 32|| key == 13) {
    if (scene == TITLE) {
      switch (menu_select) {
      case 0:
        scene = FADEIN;
        nextScene = RUN;
        menu_select = 0;
        Initialize();
        break;

      case 1:
        scene = MANUAL;
        nextScene = TITLE;
        break;

      case 2:
        exit(0);
        break;

      default:
        break;
      }
    }
    else if (scene == PAUSE) {
      switch (menu_select) {
      case 0:
        scene = RUN;
        menu_select = 0;
        break;

      case 1:
        scene = MANUAL;
        nextScene = PAUSE;
        break;

      case 2:
        scene = FADEIN;
        nextScene = TITLE;
        menu_select = 0;
        break;

      default:
        break;
      }
    }
    else if (scene == MANUAL) {
      scene = nextScene;
    }
    else if (scene == RUN) {
      player.shot = 1;
    }
  }
}
// ==============================================
//
// ====== KeyboardUp関数 ======
// キーボードを離したときの処理
void KeyboardUp(unsigned char key, int x, int y)
{
  if (key == 'z' || key == 'Z') {
    if (scene == RUN) {
      player.shot = 0;
    }
  }
}
// ==============================================
//
// ====== SpecialKey関数 ======
// 方向キー押下時の処理
void SpecialKey(int key, int x, int y)
{
  int mod = 0;

  if (scene == RUN) {
    mod = glutGetModifiers();
    if ((mod & GLUT_ACTIVE_SHIFT) != 0) { // 方向キーとShiftキーが押されている場合
      player.speed = 1; // 減速
    }
    else {
      player.speed = 8;
    }
  }

  switch (key) {
  case GLUT_KEY_RIGHT:
    if (scene == TITLE || scene == PAUSE) {
    }
    else if (scene == RUN) {
      ctrlDirection[1] = 1;
    }
    glutPostRedisplay();
    break;

  case GLUT_KEY_LEFT:
    if (scene == TITLE || scene == PAUSE) {
    }
    else if (scene == RUN) {
      ctrlDirection[3] = 1;
    }
    glutPostRedisplay();
    break;

  case GLUT_KEY_UP:
    if (scene == TITLE || scene == PAUSE) {
      if (menu_select> 0) {
        menu_select--;
      }
      else {
        menu_select= 2;
      }
    }
    else if (scene == RUN) {
      ctrlDirection[0] = 1;
    }
    glutPostRedisplay();
    break;

  case GLUT_KEY_DOWN:
    if (scene == TITLE || scene == PAUSE) {
      if (menu_select< 2) {
        menu_select++;
      }
      else {
        menu_select= 0;
      }
    }
    else if (scene == RUN) {
      ctrlDirection[2] = 1;
    }
    glutPostRedisplay();
    break;

  default:
    break;
  }
}
// ==============================================
//
// ====== SpecialKeyUp関数 ======
// 方向キーを離したときの処理
void SpecialKeyUp(int key, int x, int y)
{
  if (scene == RUN) {
    switch (key) {
    case GLUT_KEY_RIGHT:
      ctrlDirection[1] = 0;
      glutPostRedisplay();
      break;

    case GLUT_KEY_LEFT:
      ctrlDirection[3] = 0;
      glutPostRedisplay();
      break;

    case GLUT_KEY_UP:
      ctrlDirection[0] = 0;
      glutPostRedisplay();
      break;

    case GLUT_KEY_DOWN:
      ctrlDirection[2] = 0;
      glutPostRedisplay();
      break;

    default:
      break;
    }
  }
}
// ==============================================
/* ______________________________________________________________ */