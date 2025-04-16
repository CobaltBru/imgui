#include "Chat.h"

void Chat::setTokens(string s)
{
    istringstream ss1(s);
    string buffer;
    queue<string> q;
    queue<string> nq;

    while (getline(ss1, buffer, '\n')) {
        q.push(buffer);
    }
    string current = q.front();
    q.pop();
    istringstream ss2(current);
    while (getline(ss2, buffer, '^')) {
        tokens.push_back(normalToken(buffer));
    }

    while (!q.empty())
    {
        current = q.front();
        q.pop();
        istringstream ss(current);
        vector<string>v;
        while (getline(ss, buffer, '^')) {
            v.push_back(buffer);
        }
        for (int i = 0; i < v.size(); i++)
        {
            if (i == 0)
            {
                tokens.push_back(enterToken(v[i]));
            }
            else
            {
                tokens.push_back(normalToken(v[i]));
            }
        }
    }


    for (auto tok : tokens)
    {
        boxWidth = max(boxWidth, tok.x + ImGui::CalcTextSize(tok.text.c_str()).x);
        boxHeight = max(boxHeight, tok.y + ImGui::CalcTextSize(tok.text.c_str()).y);
    }

}

Token Chat::normalToken(string s)
{
    Token tmp;
    if (tokens.empty())
    {
        tmp.x = 0;
        tmp.y = 0;
        tmp.delay = 0.0f;
    }
    else
    {
        tmp.x = tokens.back().x + ImGui::CalcTextSize(tokens.back().text.c_str()).x;
        tmp.y = tokens.back().y;
        tmp.delay = 0.3f;
    }
    
    tmp.text = s;
    tmp.appear = APPEAR::NORMAL;
    tmp.option = OPTION::STOP;
    tmp.colors = COLORS::WHITE;
    
    return tmp;
}

Token Chat::enterToken(string s)
{
    Token tmp;
    if (tokens.empty())
    {
        tmp.x = 0;
        tmp.y = 0;
    }
    else
    {
        tmp.x = 0;
        tmp.y = tokens.back().y + ImGui::CalcTextSize(tokens.back().text.c_str()).y;
    }
    tmp.text = s;
    tmp.appear = APPEAR::NORMAL;
    tmp.option = OPTION::STOP;
    tmp.colors = COLORS::WHITE;
    tmp.delay = 0;
    return tmp;
}
