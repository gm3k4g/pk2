//#########################
//Pekka Kana 2
//Copyright (c) 2003 Janne Kivilahti
//#########################
#pragma once

#include "engine/platform.hpp"

#include <vector>

namespace PInput {

enum KEY {

	UNKNOWN,

	F1,	F2,	F3,
	F4,	F5,	F6,
	F7,	F8,	F9,
	F10,	F11,	F12,

	ESCAPE,	RETURN,
	BACK,	SPACE,
	DEL,	END,
	TAB,

	LALT,		RALT,
	LCONTROL,	RCONTROL,
	LSHIFT,		RSHIFT,

	LEFT,	RIGHT,
	UP,		DOWN,

	A,	B,	C,	D,
	E,	F,	G,	H,
	I,	J,	K,	L,
	M,	N,	O,	P,
	Q,	R,	S,	T,
	U,	V,	W,	X,
	Y,	Z

};

struct touch_t {

	int id;
	float pos_x, pos_y;

};

extern std::vector<touch_t> touchlist;

extern int mouse_x, mouse_y;

const char* KeyName(u8 key);
u8 GetKey();
bool Keydown(int key);

void StartKeyboard();
void EndKeyboard();
bool Is_Editing();
void InjectText(const char* text);
void InjectKey(int key);
int ReadKeyboard(char* c);

int Vibrate(int length);
void SetMousePosition(int x, int y);

std::vector<touch_t> GetTouch();
int GetTouchPos(float& x, float& y);

void GetMouse(int& x, int& y);
void UpdateMouse(bool keyMove, bool relative);
bool MouseLeft();
bool MouseRight();

int init();
int update();
int terminate();

}
