// Sample shooting game "Destroid"

#include "DxLib.h"                  //includes DX library
#include "stdlib.h"
#define _USE_MATH_DEFINES
#include "math.h"
#include "stdbool.h"

// vector functions and initializations －　ベクトルの宣言とベクトルの関数の宣言
// vector class　ー　ベクトルのオブジェクトの宣言
typedef struct Vector {
	float x, y;
} Vector; 
//　create a vector　－　新しいベクトルを作る関数
Vector new_vector() {
	Vector c;
	c.x = 800;
	c.y = 300;
	return c;
}
//　create a random position vector　－　新しい乱数位置ベクトルを作る関数
Vector rand_vectorp() {
	Vector r;
	r.x = GetRand(800);
	r.y = GetRand(600);
	return r;
}
//　create a random velocity vector　－　新しい乱数速度ベクトルを作る関数
Vector rand_vectorv() {
	Vector r;
	int random;
	random = GetRand(1);
	if (random == 0) {
		r.x = GetRand(400);
		r.y = GetRand(300);
	}
	else {
		r.x = -GetRand(400);
		r.y = -GetRand(300);
	}
	return r;
}
// add vectors　－　ベクトルの足し算をする関数
Vector vec_add(Vector a, Vector b) {
	Vector c = new_vector();
	c.x = a.x + b.x;
	c.y = a.y + b.y;
	return c;
}
// substracts vector　－　ベクトルの引き算をする関数
Vector vec_sub(Vector a, Vector b) {
	Vector c = new_vector();
	c.x = a.x - b.x;
	c.y = a.y - b.y;
	return c;
}
// multiply vectors by scalar　ー　ベクトルの掛け算をする関数
Vector vec_mul(Vector a, float sf) {
	Vector c = new_vector();
	c.x = a.x * sf;
	c.y = a.y * sf;
	return c;
}
// make things return in windows　ー　オブジェクトは画面の限り存在する
Vector wrap(Vector size, Vector p) {
	Vector c;
	c.x = fmod(p.x + size.x, size.x);
	c.y = fmod(p.y + size.y, size.y);
	return c;
}
// calculate distance between 2 vectors ー　2つのベクトルの間の距離を計算する関数
float dist(Vector a, Vector b) {
	return sqrt(((b.x-a.x)*(b.x - a.x))+((b.y-a.y)*(b.y - a.y)));
}
// time delay ー　時間を待つする関数
void delay(int x)
{
	int i = 0, j = 0;
	for (i = 0; i<x; i++) { for (j = 0; j<200000; j++) {} }
}

//　ship functions and initializations　－　宇宙船の宣言と宇宙船の関数の宣言

#define MAX_SPEED 50
#define ACCEL_CONST 1
#define SHIP_ROTATION_SPEED 0.05
#define SHIP_FRICTION 0.4

//　class ship ー　宇宙船のオブジェクトの宣言
typedef struct Ship {
	// The position of the ship
	Vector position;
	// The velocity of the ship
	Vector velocity;
	// The angle the ship is pointing in radians
	double angle;
	//ship's lives
	int lives;
	// sprite
	int e;
} Ship;
//　create a ship at given position an velocity ー　新しい宇宙船を作る関数
Ship new_ship(Vector p, Vector v) {
	Ship s;
	s.position = p;
	s.velocity = v;
	s.angle = 0;
	s.lives = 5;
	s.e = 0;
	return s;
}
//　rotates ship to left　ー　宇宙船を左に回転する関数
void rotate_ship_left(Ship *ship) {
	ship->angle -= SHIP_ROTATION_SPEED;
}
//　rotates ship to right　ー　宇宙船を右に回転する関数
void rotate_ship_right(Ship *ship) {
	ship->angle += SHIP_ROTATION_SPEED;
}
//　limitation of speed　－　宇宙船の速度の限界関数
void bound_ship_speeds(Ship *ship) {
	Vector *v = &ship->velocity;
	if (v->x < -MAX_SPEED)
		v->x = -MAX_SPEED;
	if (v->x > MAX_SPEED)
		v->x = MAX_SPEED;
	if (v->y < -MAX_SPEED)
		v->y = -MAX_SPEED;
	if (v->y > MAX_SPEED)
		v->y = MAX_SPEED;
}
//　accelerate ships　－　宇宙船を前に進む関数
void accelerate_ship(Ship *ship) {
	Vector delta;
	delta.x = (sin(-ship->angle) * ACCEL_CONST);
	delta.y = (cos(-ship->angle) * ACCEL_CONST);
	ship->velocity = vec_sub(ship->velocity, delta);
	bound_ship_speeds(ship);
}
//　decelerates ships　－　宇宙船を後ろに戻る関数
void deccelerate_ship(Ship *ship) {
	Vector delta;
	delta.x = (sin(-ship->angle) * ACCEL_CONST);
	delta.y = (cos(-ship->angle) * ACCEL_CONST);
	ship->velocity = vec_add(ship->velocity, delta);
	bound_ship_speeds(ship);
}
// updates ship position and velocity ー　宇宙船の位置と速度をアップデートする関数
void update_ship(Ship *ship, float dt, Vector size){
	ship->position = wrap(size, vec_add(ship->position, vec_mul(ship->velocity, dt)));
	ship->velocity = vec_mul(ship->velocity, (pow(1 - SHIP_FRICTION, dt)));
}
// draw ship ー　宇宙船を画面に確描く
void render_ship(Ship *ship, int k) {
DrawRotaGraph(ship->position.x, ship->position.y, 1, ship->angle, k, TRUE);
}

//　asteroid functions and initializations　－　アストロイドの宣言とアストロイドの関数の宣言
//　class asteroid ー　アストロイドのオブジェクトの宣言
typedef struct Asteroid {
	Vector position;
	Vector velocity;
	int radius;
	int random;
	int e;
	int destroyed;
} Asteroid;
//　create an asteroid at random position and random velocity ー　新しいアストロイドを作る関数
Asteroid new_asteroid() {
	Asteroid a;
	a.random = GetRand(3);
	if (a.random == 3) {
		a.position.x = GetRand(800);
		a.position.y = 0;
	}
	else if (a.random == 2) {
		a.position.x = GetRand(800);
		a.position.y = 600;
	}
	else if (a.random == 1) {
		a.position.x = 0;
		a.position.y = GetRand(600);
	}
	else {
		a.position.x = 800;
		a.position.y = GetRand(600);
	}
	a.velocity = rand_vectorv();
	a.radius = GetRand(15) + 10;
	a.destroyed = 0;
	a.e = 0;
	return a;
}

//　bullet functions and initializations　－　弾丸の宣言と弾丸の関数の宣言
//　class bullet ー　弾丸のオブジェクトの宣言
typedef struct Bullet{
	Vector position;
	Vector velocity;
	int launch;
}Bullet;
// create a bullet ー　新しい弾丸を作る関数
Bullet new_Bullet(Vector p, Vector v) {
	Bullet b;
	b.position = p;
	b.velocity = v;
	b.launch = 0;
	return b;
}
// launch a bullet ー　弾丸をシュートする関数
void launch_bullet(Bullet *bullet, Ship *ship ) {
	Vector delta;
	delta.x = (sin(-ship->angle));
	delta.y = (cos(-ship->angle));
	bullet->velocity = vec_sub(bullet->velocity, delta);
	bullet->launch = 1;
}

//　star functions and initializations　－　星の宣言と星の関数の宣言
//　class star ー　星のオブジェクトの宣言
typedef struct Star {
	Vector position;
	Vector velocity;
} Star;
// create a star ー　新しい星を作る関数
Star new_star() {
	Star s;
	s.position = rand_vectorp();
	s.velocity = {-4 , 3};
	return s;
}
// updates star position and velocity ー 星の位置と速度をアップデートする関数
void update_star(Star *star, Vector size) {
	star->position = wrap(size, vec_add(star->position, star->velocity));
}
// draw star ー　星を画面に確描く
void render_star(Star *star) {
	DrawPixel(star->position.x, star->position.y, 0xFFFFFF);
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// global variable declaration　－　変数の宣言
	Vector size = { 800, 600 };
	Star stars[100];
	Vector playerShipPos = new_vector();
	Vector playerShipVel = { 0.0, 0.0 };
	Ship playerShip = new_ship(playerShipPos, playerShipVel);
	Asteroid asteroids[10];
	Vector bulletPos = new_vector();
	Vector bulletVel = { 0.0, 0.0 };
	Bullet bullets[1000];
	int key = 0, oldkey;
	float dt = 0.1;
	int score = 0, highscore=0;
	int flgEnd = 1, flgAsteroid = 0, flgShipDestroyed = 0, flgStar = 0;
	int a, b, rb, st, e = 0;
	for (b = 0; b < 100; b++) {
		bullets[b] = new_Bullet(bulletPos, bulletVel);
	}

	ChangeWindowMode(TRUE);			// change windows mode　－　ウィンドウズを変更する
	SetGraphMode(800, 600, 32);
	DxLib_Init();					// starts DX library　－　DXライブラリーをスタート
	SetDrawScreen(DX_SCREEN_BACK);	// sets draw screen to the back

	// load sound n image files　－　音楽と画像をロードする
	int shipSprite = LoadGraph("ship1.png");
	int title = LoadGraph("Destroid.png");
	int title2 = LoadGraph("title2.png");
	int start = LoadGraph("start.png");
	int copyright = LoadGraph("copyright.png");
	int gameOver = LoadGraph("gameOver.png");
	int playAgain = LoadGraph("playAgain.png");
	int quit = LoadGraph("quit.png");
	int scoreHighscore = LoadGraph("scoreHighscore.png");
	int instruction = LoadGraph("instruction.png");
	int explosion2[74]; 
	LoadDivGraph("explosion2.png", 74, 9, 9, 100, 100, explosion2);
	int explosion[72];
	LoadDivGraph("explosion.png", 72, 8, 9, 50, 50, explosion);
	int music1 = LoadSoundMem("music1.mp3");
	int shot = LoadSoundMem("shot.mp3");
	int explosionSound1 = LoadSoundMem("explosionSound1.mp3");
	int explosionSound2 = LoadSoundMem("explosionSound2.mp3");

	PlaySoundMem(music1, DX_PLAYTYPE_LOOP, TRUE);
	WaitTimer(1000);
	
	// scene 1 : start screen
	while (ProcessMessage() == 0) {
		
		if (flgStar==0) {
			for (st = 0; st < 100; st++) {
				stars[st] = new_star();
			}
			flgStar = 1;
		}
		if (CheckHitKey(KEY_INPUT_RETURN) == 1) break;
		for (st = 0; st < 100; st++) {
			update_star(&stars[st], size);
		}

		ClearDrawScreen();

		DrawExtendGraph(110, 35, 670, 175, title2, TRUE);
		DrawExtendGraph(45, 55, 760, 355, title, TRUE);
		//DrawExtendGraph(160, 220, 600, 350, copyright, TRUE);
		DrawLine(45, 255, 729, 255, 0xFFFFFF);
		DrawLine(45, 300, 729, 300, 0xFFFFFF);
		DrawExtendGraph(100, 310, 700, 510, start, TRUE);

		// render stars
		for (st = 0; st < 100; st++) {
			render_star(&stars[st]);
		}

		ScreenFlip();
	}

	// scene 2 : instruction
	while (ProcessMessage() == 0) {
		
		if (flgStar == 0) {
			for (st = 0; st < 100; st++) {
				stars[st] = new_star();
			}
			flgStar = 1;
		}
		for (st = 0; st < 100; st++) {
			update_star(&stars[st], size);
		}
		if (CheckHitKey(KEY_INPUT_SPACE) == 1) break;

		ClearDrawScreen();
		
		DrawGraph(0,0,instruction,TRUE);
		for (st = 0; st < 100; st++) {
			render_star(&stars[st]);
		}

		ScreenFlip();
	}

	// scene 3 : main game
	game:
	while (ProcessMessage() == 0) {
		
		ChangeVolumeSoundMem(240, music1);
		ChangeVolumeSoundMem(255, explosionSound1);
		ChangeVolumeSoundMem(255, explosionSound2);
		if (flgAsteroid == 0) {
				for (a = 0; a < 10; a++) {
					asteroids[a] = new_asteroid();
				}
				flgAsteroid = 1;
			}
		oldkey = key;
		key = CheckHitKey(KEY_INPUT_SPACE);
		if (CheckHitKey(KEY_INPUT_UP) == 1) {
				accelerate_ship(&playerShip);
			}
		else if (CheckHitKey(KEY_INPUT_DOWN) == 1) {
				deccelerate_ship(&playerShip);
			}
		if (CheckHitKey(KEY_INPUT_RIGHT) == 1) {
				rotate_ship_right(&playerShip);
			}
		else if (CheckHitKey(KEY_INPUT_LEFT) == 1) {
				rotate_ship_left(&playerShip);
			}
		if (((key &~oldkey)&CheckHitKey(KEY_INPUT_SPACE) == 1)&&(flgShipDestroyed==0)) {
				rb = GetRand(99);
				if (bullets[rb].launch == 0) {
					launch_bullet(&bullets[rb], &playerShip);
				}
				PlaySoundMem(shot, DX_PLAYTYPE_BACK, TRUE);
			}

		// update
		update_ship(&playerShip, dt, size);
		
		for (st = 0; st < 100; st++) {
				stars[st].position = wrap(size, vec_add(stars[st].position, vec_mul(playerShip.velocity, 1)));
			}
		for (b = 0; b < 100; b++) {
				if (bullets[b].launch == 1) {
					bullets[b].position = vec_add(bullets[b].position, vec_mul(bullets[b].velocity, 10));
					if ((bullets[b].position.x < 0) || (bullets[b].position.x > 800) || (bullets[b].position.y < 0) || (bullets[b].position.y > 600)) {
						bullets[b].launch = 0;
					}
				}
				else {
					bullets[b].position = playerShip.position;
					bullets[b].velocity = { 0,0 };
				}
			}
		for (a = 0; a < 10; a++) {
				if ((flgShipDestroyed == 0) && ((dist(playerShip.position, asteroids[a].position) < 25 + asteroids[a].radius))) {
					playerShip.lives = playerShip.lives - 1;
					flgShipDestroyed = 1;
					PlaySoundMem(explosionSound2, DX_PLAYTYPE_BACK, TRUE);
					asteroids[a].destroyed = 1;
				}
				for (b = 0; b < 100; b++) {
					if ((dist(bullets[b].position, asteroids[a].position) < 2 + asteroids[a].radius)&&(flgShipDestroyed == 0)) {
						PlaySoundMem(explosionSound1, DX_PLAYTYPE_BACK, TRUE);
						asteroids[a].destroyed = 1;
						score = score + 1;
						bullets[b].launch = 0;
					}
				}
				asteroids[a].position = wrap(size, vec_add(asteroids[a].position, vec_mul(asteroids[a].velocity, 0.005)));
			}

		//render
		ClearDrawScreen();

		//screen gui (score etc..)
		DrawFormatString(120, 10, 0xFFFFFF, "Lives = %d", playerShip.lives);
		DrawFormatString(10, 10, 0xFFFFFF, "Score = %d", score);

		//bullet
		for (b = 0; b < 100; b++) {
				DrawCircle(bullets[b].position.x, bullets[b].position.y, 2, 0x33FFFA);
			}

		//ship
		if (flgShipDestroyed == 1) {
					playerShip.velocity = { 0,0 },
					DrawGraph(playerShip.position.x - 40, playerShip.position.y - 40, explosion2[e], TRUE);
					e = e + 1;
				if (e >74) {
					playerShip.position = { 400, 300 };
					playerShip.velocity = { 0, 0 };
					flgShipDestroyed = 0;
					e = 0;
					for (a = 0; a < 10; a++) {
						asteroids[a] = new_asteroid();
					}
					if (playerShip.lives == 0)break;
				}
			}
		else {
				render_ship(&playerShip, shipSprite);
			}

		//asteroid
		for (a = 0; a < 10; a++) {
				if (asteroids[a].destroyed == 0) {
					DrawCircle(asteroids[a].position.x, asteroids[a].position.y, asteroids[a].radius, 0xCCCCCC);
				}
				else {
					asteroids[a].velocity = {0,0},
					DrawGraph(asteroids[a].position.x-20, asteroids[a].position.y-20, explosion[asteroids[a].e], TRUE);
					asteroids[a].e = asteroids[a].e + 3;
					if (asteroids[a].e >71) {
						asteroids[a] = new_asteroid();
					}
				}
			}

		//stars
		for (st = 0; st < 100; st++) {
				render_star(&stars[st]);
			}
				
			ScreenFlip();
	}

	// scene 4 : score & highscore
	while (ProcessMessage() == 0) {
		if (flgStar == 0) {
			for (st = 0; st < 100; st++) {
				stars[st] = new_star();
			}
			flgStar = 1;
		}
		for (st = 0; st < 100; st++) {
			update_star(&stars[st], size);
		}
		if (score>highscore) {
			highscore = score;
		}
		if (CheckHitKey(KEY_INPUT_RETURN) == 1) { 
			playerShip.lives = 5;
			score = 0;
			SetFontSize(16);
			goto game;
		}
		if (CheckHitKey(KEY_INPUT_ESCAPE) == 1) break;
		
		ClearDrawScreen();
		
		DrawExtendGraph(45, 35, 760, 335, gameOver, TRUE);
		DrawExtendGraph(45, 210, 670, 450, scoreHighscore, TRUE);
		DrawLine(55, 225, 729, 225, 0xFFFFFF);
		DrawLine(55, 300, 729, 300, 0xFFFFFF);
		DrawExtendGraph(45, 350, 400, 500, playAgain, TRUE);
		DrawExtendGraph(400, 350, 755, 500, quit, TRUE);
		SetFontSize(50);
		DrawFormatString(55, 240, 0xFFFFFF, "        %d             %d", score, highscore);
		for (st = 0; st < 100; st++) {
			render_star(&stars[st]);
		}

		ScreenFlip();
	}
	
	DxLib_End();    // end DX library
	return 0;
}

