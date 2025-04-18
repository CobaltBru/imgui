#include "Chat.h"
#include "CommonFunction.h"
#include "Control.h"

static Token* selectedToken = nullptr;

void Chat::setTokens(string s)
{
    /*if (s == origin) return;
    else tokens.clear();*/
    tokens.clear();
    boxWidth = 0;
    boxHeight = 0;
    origin = s;
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
        //if (buffer == "")continue;
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
            //if (v[i] == "")continue;
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


    for (auto& tok : tokens)
    {
        tok.width = ImGui::CalcTextSize(tok.text.c_str()).x;
        tok.height = ImGui::CalcTextSize(tok.text.c_str()).y;
        boxWidth = max(boxWidth, tok.x + tok.width);
        boxHeight = max(boxHeight, tok.y + tok.height);
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
        tmp.delay = 0.5f;
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

void Chat::PrintTokens()
{
    ImVec2 p = ImGui::GetCursorScreenPos();
    ImVec2 bottomRight = ImVec2(p.x + boxWidth, p.y + boxHeight);
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    drawList->AddRectFilled(p, bottomRight, IM_COL32(0, 255, 0, 125));

    int flag = 0;
    for (int i = 0; i < tokens.size(); i++)
    {
        Token& token = tokens[i];
        if (token.width == 0 || token.height == 0) continue;
        if (token.delay >= 0.0001f)
        {
            if (flag == 0)flag = 1;
            else flag = 0;
        }

        ImVec2 pos = ImGui::GetWindowContentRegionMin();
        PrintTokenBox(token, flag);
        ImGui::SetCursorPosX(pos.x + token.x);
        ImGui::SetCursorPosY(pos.y + token.y);
        ImGui::Text(token.text.c_str());
        ImGui::SetCursorPos(pos);
        if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0))
        {
            selectedToken = &tokens[i];
            ImGui::OpenPopup("TokenEditor");
        }
    }
    if (ImGui::BeginPopup("TokenEditor"))
    {
        if (selectedToken)
        {
            ImGui::Text("편집 중: %s", selectedToken->text.c_str());

            int appear = selectedToken->appear;
            const char* appearNames[] = { "NORMAL", "DOOM", "EXPLODE", "END" };
            ImGui::Combo("Appear", &appear, appearNames, IM_ARRAYSIZE(appearNames));
            selectedToken->appear = static_cast<APPEAR>(appear);

            int option = selectedToken->option;
            const char* optionNames[] = { "STOP", "SHAKE", "WAVE" };
            ImGui::Combo("Option", &option, optionNames, IM_ARRAYSIZE(optionNames));
            selectedToken->option = static_cast<OPTION>(option);

            int color = selectedToken->colors;
            const char* colorNames[] = { "WHITE", "RED", "YELLOW", "PURPLE", "ORANGE", "GREEN", "SKY" };
            ImGui::Combo("Color", &color, colorNames, IM_ARRAYSIZE(colorNames));
            selectedToken->colors = static_cast<COLORS>(color);
        }

        if (ImGui::Button("Close"))
            ImGui::CloseCurrentPopup();

        ImGui::EndPopup();
    }
}

void Chat::PrintTokenBox(Token tok,int flag)
{
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImVec2 windowPos = ImGui::GetCursorScreenPos();
    ImVec2 p = windowPos;
    p.x += tok.x;
    p.y += tok.y;
    ImVec2 bottomRight = ImVec2(p.x + tok.width, p.y + tok.height);
    if(flag == 0)
        drawList->AddRectFilled(p, bottomRight, IM_COL32(255, 140, 0, 255));
    else
        drawList->AddRectFilled(p, bottomRight, IM_COL32(65, 105, 225, 255));

}


json Chat::to_json() const {
    json j;
    j["origin"] = origin;

    j["boxWidth"] = boxWidth;
    j["boxHeight"] = boxHeight;
    
    j["tokens"] = json::array();
    for (auto& tok : tokens) {
        j["tokens"].push_back({
            {"x",      tok.x},
            {"y",      tok.y},
            {"text",   tok.text},
            {"appear", (int)tok.appear},
            {"option", (int)tok.option},
            {"colors", (int)tok.colors},
            {"delay",  tok.delay}
            });
    }
    return j;
}

// JSON 역직렬화: tokens와 옵션 복원
void Chat::from_json(const json& j) {
    origin = j.at("origin").get<string>();

    boxWidth = j.at("boxWidth").get<float>();
    boxHeight = j.at("boxHeight").get<float>();


    tokens.clear();
    for (auto& jt : j.at("tokens")) {
        Token tok;
        tok.x = jt.at("x").get<float>();
        tok.y = jt.at("y").get<float>();
        tok.text = jt.at("text").get<string>();
        tok.appear = static_cast<APPEAR>(jt.at("appear").get<int>());
        tok.option = static_cast<OPTION>(jt.at("option").get<int>());
        tok.colors = static_cast<COLORS>(jt.at("colors").get<int>());
        tok.delay = jt.at("delay").get<float>();
        // 폭/높이 재계산
        ImVec2 sz = ImGui::CalcTextSize(tok.text.c_str());
        tok.width = sz.x;
        tok.height = sz.y;
        tokens.push_back(tok);
    }
    // 박스 크기 갱신
    boxWidth = boxHeight = 0;
    for (auto& tok : tokens) {
        boxWidth = max(boxWidth, tok.x + tok.width);
        boxHeight = max(boxHeight, tok.y + tok.height);
    }
}


Node::Node()
{
    activeTextEditor = false;
    key = random_string(10);
}

bool Node::Screen(ImVec2& panOffset)
{
    ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos({ g_pos.x + panOffset.x,g_pos.y + panOffset.y }, ImGuiCond_Always);

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove;
    ImGui::Begin(key.c_str());

    if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup) &&
        ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
    {
        ImGui::SetClipboardText(key.c_str());
        // (선택) 복사가 됐다는 작은 툴팁
        ImGui::BeginTooltip();
        ImGui::Text("Key copied!");
        ImGui::EndTooltip();
    }

    windowPos = ImGui::GetWindowPos();
    windowSize = ImGui::GetWindowSize();
    connections.clear();
    if (ImGui::IsWindowHovered(ImGuiHoveredFlags_RootAndChildWindows) &&
        ImGui::IsMouseDragging(ImGuiMouseButton_Left))
    {
        ImGuiIO& io = ImGui::GetIO();
        // 논리적 pos 에 MouseDelta 를 더해주면
        // 다음 프레임부터 SetNextWindowPos 에 반영됩니다
        g_pos.x += io.MouseDelta.x;
        g_pos.y += io.MouseDelta.y;
    }

    if (chat.boxWidth < 200.f)
    {
        TextBoxSize.x = 200.f;
    }
    else
    {
        TextBoxSize.x = chat.boxWidth * 1.5f;
    }
    if (chat.boxHeight < 100.f)
    {
        TextBoxSize.y = 100;
    }
    else
    {
        TextBoxSize.y = chat.boxHeight * 1.5f;
    }
    editButtonPos = ImVec2(0.f, chat.boxHeight + 5.f);
    saveButtonPos = ImVec2(50.f, TextBoxSize.y + 5.f);
    
    ImVec2 pos = ImGui::GetWindowContentRegionMin();
    if (!activeTextEditor)
    {
        chat.PrintTokens();
        ImGui::SetCursorPos({ editButtonPos.x + pos.x, editButtonPos.y + pos.y });
        if (ImGui::Button("Edit"))
        {
            activeTextEditor = true;
            buffer = chat.origin;
        }

        if (redSelects.empty() && normalSelects.empty())
        {
            if (ImGui::Button("Link Next"))
            {
                g_IsLinking = true;
                g_LinkType = LinkType::NodeSelf;
                g_LinkSourceNode = this;
            }

        }
    }
    
    
    if (activeTextEditor)
    {
        ImGui::SetCursorPos({ pos });
        ImGui::InputTextMultiline(
            "##inline_editor",
            &buffer,
            TextBoxSize,
            ImGuiInputTextFlags_AllowTabInput
        );
        ImGui::SetCursorPos({ 10.f, saveButtonPos.y + pos.y });
        if (ImGui::Button("Save"))
        {
            chat.setTokens(buffer);
            activeTextEditor = false;
            buffer = "";
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel"))
        {
            activeTextEditor = false;
            buffer = "";
        }
    }


    ImGui::SeparatorText("Red 선택지");

    if (!isAddingRedSelect)
    {
        if (ImGui::Button("Red 선택지 추가"))
            isAddingRedSelect = true;
    }
    else
    {
        ImGui::InputText("##red_input", &redInputBuffer);
        ImGui::SameLine();
        if (ImGui::Button("확인"))
        {
            if (!redInputBuffer.empty())
            {
                redSelects.push_back({ redInputBuffer, "" });
            }
            redInputBuffer.clear();
            isAddingRedSelect = false;
        }
        ImGui::SameLine();
        if (ImGui::Button("취소"))
        {
            redInputBuffer.clear();
            isAddingRedSelect = false;
        }
    }

    // 기존 선택지 출력
    for (int i = 0; i < redSelects.size(); ++i)
    {
        auto& sel = redSelects[i];
        ImGui::Text("%s", redSelects[i].text.c_str());

        ImVec2 tmin = ImGui::GetItemRectMin();
        ImVec2 tmax = ImGui::GetItemRectMax();
        ImVec2 start = ImVec2((tmin.x + tmax.x) * 0.5f, (tmin.y + tmax.y) * 0.5f);


        ImGui::SameLine();
        // Link 버튼
        if (ImGui::SmallButton(("Link##red" + std::to_string(i)).c_str()))
        {
            g_IsLinking = true;
            g_LinkType = LinkType::RedSelect;
            g_LinkSourceNode = this;
            g_LinkSourceIndex = i;
        }
        ImGui::SameLine();
        if (ImGui::Button(("삭제##red" + std::to_string(i)).c_str()))
        {
            redSelects.erase(redSelects.begin() + i);
            --i; // 안전하게 인덱스 보정
            continue;
        }
        // 3) next가 설정되어 있으면 이 start를 사용
        if (!sel.next.empty())
            connections.push_back({ start, sel.next });

    }

    // ─ Normal 선택지 ─
    ImGui::SeparatorText("Normal 선택지");

    if (!isAddingNormalSelect)
    {
        if (ImGui::Button("Normal 선택지 추가"))
            isAddingNormalSelect = true;
    }
    else
    {
        ImGui::InputText("##normal_input", &normalInputBuffer);
        ImGui::SameLine();
        if (ImGui::Button("확인##normal"))
        {
            if (!normalInputBuffer.empty())
            {
                normalSelects.push_back({ normalInputBuffer, "" });
            }
            normalInputBuffer.clear();
            isAddingNormalSelect = false;
        }
        ImGui::SameLine();
        if (ImGui::Button("취소##normal"))
        {
            normalInputBuffer.clear();
            isAddingNormalSelect = false;
        }
    }

    for (int i = 0; i < normalSelects.size(); ++i)
    {
        auto& sel = normalSelects[i];
        ImGui::Text("%s", normalSelects[i].text.c_str());
        ImVec2 tmin = ImGui::GetItemRectMin();
        ImVec2 tmax = ImGui::GetItemRectMax();
        ImVec2 start = ImVec2((tmin.x + tmax.x) * 0.5f, (tmin.y + tmax.y) * 0.5f);

        ImGui::SameLine();
        if (ImGui::SmallButton(("Link##norm" + std::to_string(i)).c_str()))
        {
            g_IsLinking = true;
            g_LinkType = LinkType::NormalSelect;
            g_LinkSourceNode = this;
            g_LinkSourceIndex = i;
        }
        ImGui::SameLine();
        if (ImGui::Button(("삭제##normal" + std::to_string(i)).c_str()))
        {
            normalSelects.erase(normalSelects.begin() + i);
            --i;
            continue;
        }
        if (!sel.next.empty())
            connections.push_back({ start, sel.next });
    }

    ImVec2 contentMin = ImGui::GetWindowContentRegionMin();
    ImVec2 contentMax = ImGui::GetWindowContentRegionMax();
    ImVec2 deleteBtnSize = ImVec2(60, 20);
    ImVec2 deleteBtnPos = ImVec2(contentMax.x - deleteBtnSize.x - 10, contentMax.y - deleteBtnSize.y - 10);
    ImGui::InputInt("Pos Index", &posIndex);
    ImGui::SetCursorPos(deleteBtnPos);
    bool shouldDelete = ImGui::Button("Delete", deleteBtnSize);
    // (3) 노드 자체 연결: next 가 비어있지 않으면
    if (!next.empty())
    {
        ImVec2 center = { windowPos.x + windowSize.x * 0.5f , windowPos.y + windowSize.y * 0.5f };
        connections.push_back({ center, next });
    }

    
    ImGui::End();
    return shouldDelete;
}
