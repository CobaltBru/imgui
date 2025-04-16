#pragma once

#include <d3d11.h>
#include <tchar.h>
#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <sstream>
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "imgui_stdlib.h"

using namespace std;
enum APPEAR { NORMAL, DOOM, EXPLODE, END };
enum OPTION { STOP, SHAKE, WAVE };
enum COLORS { WHITE, RED, YELLOW, PURPLE, ORANGE, GREEN, SKY };


struct Token
{
    float x, y;

    string text;
    APPEAR appear;
    OPTION option;
    COLORS colors;
    float delay;
};

class Chat
{
public:
    string origin;
    vector<Token> tokens;
    float boxWidth = 0;
    float boxHeight = 0;
    void setTokens(string s);
    Token normalToken(string s);
    Token enterToken(string s);
    void PrintTokens();
};

