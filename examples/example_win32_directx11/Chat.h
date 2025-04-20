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
#include "json.hpp"
using json = nlohmann::json;

using namespace std;
enum APPEAR { NORMAL, DOOM, EXPLODE, END };
enum OPTION { STOP, SHAKE, WAVE };
enum COLORS { WHITE, RED, YELLOW, PURPLE, ORANGE, GREEN, SKY };


struct Token
{
    float x, y;
    float yIdx;
    float width, height;
    string text;
    APPEAR appear;
    OPTION option;
    COLORS colors;
    float delay;
};

struct Select
{
    string text;
    string next;
};


class Chat
{
public:
    string origin;
    vector<Token> tokens;
    
    float boxWidth = 0;
    float boxHeight = 0;
    void setTokens(string s);
    Token normalToken(string s, int yidx);
    Token enterToken(string s, int yidx);
    void PrintTokens();
    void PrintTokenBox(Token tok, int flag);

   

    json to_json() const;
    void from_json(const json& j);
};

struct Connection {
    ImVec2 start;     // 시작점(스크린 좌표)
    string destKey;   // 연결될 노드 키
};

class Node
{
public:
    string key;
    string next;
    int posIndex = 0;
    Chat chat;
    bool activeTextEditor = false;

    vector<Select> redSelects;
    vector<Select> normalSelects;

    bool isAddingRedSelect = false;
    bool isAddingNormalSelect = false;
    string redInputBuffer;
    string normalInputBuffer;

    string buffer;
    ImVec2 editButtonPos;
    ImVec2 saveButtonPos;
    ImVec2 TextBoxSize;
    ImVec2 g_pos;

    ImVec2 windowPos;
    ImVec2 windowSize;
    vector<Connection> connections;
public:
    Node();
    bool Screen(ImVec2& panOffset);
};
