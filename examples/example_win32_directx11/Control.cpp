#include "Control.h"
#include "json.hpp"
#include <fstream>
using json = nlohmann::json;

bool       g_IsLinking = false;
LinkType   g_LinkType = LinkType::NodeSelf;
Node* g_LinkSourceNode = nullptr;
int        g_LinkSourceIndex = -1;
static ImVec2 g_PanOffset = ImVec2(0.0f, 0.0f);

void Control::Menu(std::vector<Node>& nodes)
{
    ImGui::SetNextWindowPos({ 800,500 }, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(200, 200), ImGuiCond_FirstUseEver);
    ImGui::Begin("Control pannel");

    ImGui::Checkbox("Create Node Mode", &createNodeButton);

    if (ImGui::IsMouseDragging(ImGuiMouseButton_Right))
    {
        ImGuiIO& io = ImGui::GetIO();
        g_PanOffset.x += io.MouseDelta.x;
        g_PanOffset.y += io.MouseDelta.y;
    }

    if (createNodeButton)
    {
        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
        {
            createNodeButton = false;
            ImGuiIO& io = ImGui::GetIO();
            Node node;
            node.g_pos = { io.MousePos.x - g_PanOffset.x,io.MousePos.y - g_PanOffset.y };
            nodes.push_back(node);
        }
    }

    for (int i = nodes.size() - 1; i >= 0; --i)
    {
        if (nodes[i].Screen(g_PanOffset))
        {
            nodes.erase(nodes.begin() + i);
        }
    }

    // ── Save/Load 섹션 ──
    ImGui::Separator();
    ImGui::Text("Save / Load");
    ImGui::InputText("##savepath", savePath, IM_ARRAYSIZE(savePath));
    if (ImGui::Button("Save Graph")) {
        SaveGraph(nodes, savePath);
    }
    ImGui::InputText("##loadpath", loadPath, IM_ARRAYSIZE(loadPath));
    ImGui::SameLine();
    if (ImGui::Button("Load Graph")) {
        LoadGraph(nodes, loadPath);
    }

    ImGui::End();
    if (g_IsLinking && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
    {
        ImVec2 mouse = ImGui::GetIO().MousePos;
        for (auto& target : nodes)
        {
            // 각 노드의 화면상 영역
            ImVec2 min = target.windowPos;
            ImVec2 max = ImVec2(min.x + target.windowSize.x,
                min.y + target.windowSize.y);
            if (mouse.x >= min.x && mouse.x < max.x &&
                mouse.y >= min.y && mouse.y < max.y)
            {
                // 클릭된 대상에 따라 next를 설정
                switch (g_LinkType)
                {
                case LinkType::NodeSelf:
                    g_LinkSourceNode->next = target.key;
                    break;
                case LinkType::RedSelect:
                    g_LinkSourceNode->redSelects[g_LinkSourceIndex].next = target.key;
                    break;
                case LinkType::NormalSelect:
                    g_LinkSourceNode->normalSelects[g_LinkSourceIndex].next = target.key;
                    break;
                }
                break;
            }
        }
        g_IsLinking = false;
    }

    {
        ImDrawList* fg = ImGui::GetForegroundDrawList();
        for (auto& node : nodes)
        {
            for (auto& c : node.connections)
            {
                // 목적지 노드 찾기
                auto it = std::find_if(nodes.begin(), nodes.end(),
                    [&](auto& n) { return n.key == c.destKey; });
                if (it == nodes.end()) continue;
                // 시작점과 목적지 중심 계산
                ImVec2 src = c.start;
                ImVec2 dst = ImVec2(
                    it->windowPos.x + it->windowSize.x * 0.5f,
                    it->windowPos.y + it->windowSize.y * 0.5f
                );
                fg->AddLine(src, dst, IM_COL32(255, 0, 0, 255), 2.0f);
            }
        }
    }


}

void Control::SaveGraph(const std::vector<Node>& nodes, const std::string& path) {
    json jnodes = json::array();
    for (auto& n : nodes) {
        json jn;
        jn["key"] = n.key;
        jn["pos"] = { n.g_pos.x, n.g_pos.y };
        jn["next"] = n.next;
        jn["chat"] = n.chat.to_json();
        // redSelects
        jn["red"] = json::array();
        for (auto& s : n.redSelects)
            jn["red"].push_back({ {"text", s.text}, {"next", s.next} });
        // normalSelects
        jn["norm"] = json::array();
        for (auto& s : n.normalSelects)
            jn["norm"].push_back({ {"text", s.text}, {"next", s.next} });
        jnodes.push_back(jn);
    }
    std::ofstream ofs(path);
    ofs << jnodes.dump(2);
}

void Control::LoadGraph(std::vector<Node>& nodes, const std::string& path) {
    std::ifstream ifs(path);
    if (!ifs.is_open()) return;
    json jnodes; ifs >> jnodes;
    nodes.clear();
    for (auto& jn : jnodes) {
        Node n;
        n.key = jn.at("key").get<string>();
        auto pos = jn.at("pos");
        n.g_pos.x = pos[0].get<float>();
        n.g_pos.y = pos[1].get<float>();
        n.next = jn.at("next").get<string>();
        n.chat.from_json(jn.at("chat"));
        // selects
        for (auto& jr : jn.at("red"))
            n.redSelects.push_back({ jr.at("text").get<string>(),
                                     jr.at("next").get<string>() });
        for (auto& jnorm : jn.at("norm"))
            n.normalSelects.push_back({ jnorm.at("text").get<string>(),
                                        jnorm.at("next").get<string>() });
        nodes.push_back(std::move(n));
    }
}
