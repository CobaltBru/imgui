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
#include "Chat.h"

enum class LinkType { NodeSelf, RedSelect, NormalSelect };
extern bool       g_IsLinking;
extern LinkType   g_LinkType;
extern Node* g_LinkSourceNode;
extern int        g_LinkSourceIndex;

class Control
{
public:
    bool createNodeButton = false;
    char savePath[256] = "graph_save.json";
    char loadPath[256] = "graph_save.json";
    void Menu(std::vector<Node>& nodes);

    void SaveGraph(const std::vector<Node>& nodes, const std::string& path);
    void LoadGraph(std::vector<Node>&nodes, const std::string & path);
};

