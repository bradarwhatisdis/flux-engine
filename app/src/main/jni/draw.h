#pragma once

#include <Vector/Vectors.h>
#include <imgui/imgui.h>

#include "resources.h"

using namespace ImGui;
using namespace std;

#include "include/includes.h"

#include "8bp.h"
#include "8bp/Ruleset.h"
#include "imgui/inc/8bp.h"
#include "keylogin.h"
#include "oxorany/oxorany.h"

#include "imgui/inc/custom_theme.h"
#include "imgui/inc/typography.h"
#include "imgui/inc/draw_helpers.h"
#include "imgui/inc/status_logger.h"
#include "imgui/inc/color_picker.h"

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES3/gl3.h>

struct MenuState {
    bool isOpen = true; // DEBUG: force open to test rendering
    int currentTab = 0;
    int prevTab = 0;
    float tabTransition = 0.0f;  // 0=idle, animating to 1
    float sidebarWidth = 320.0f;
    float animProgress = 0.0f;
    float menuAlpha = 1.0f; // DEBUG: force visible
    float menuScale = 0.9f;
    ImVec4 accentColor = FluxPalette::PrimaryV();
};
static MenuState g_menu;

static float EaseOutBack(float x) {
    const float c1 = 1.70158f;
    const float c3 = c1 + 1.0f;
    return 1.0f + c3 * powf(x - 1.0f, 3.0f) + c1 * powf(x - 1.0f, 2.0f);
}

static float EaseOutQuart(float x) {
    return 1.0f - powf(1.0f - x, 4.0f);
}

static void DrawGradientRect(ImDrawList* dl, ImVec2 p1, ImVec2 p2, ImU32 col1, ImU32 col2, bool horizontal = true) {
    if (horizontal) {
        dl->AddRectFilledMultiColor(p1, p2, col1, col2, col2, col1);
    } else {
        dl->AddRectFilledMultiColor(p1, p2, col1, col1, col2, col2);
    }
}

static bool SidebarButton(const char* label, const char* icon, bool selected, float width) {
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems) return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);

    ImVec2 pos = window->DC.CursorPos;
    ImVec2 size = ImVec2(width - 12.0f, 68.0f);

    const ImRect bb(pos, pos + size);
    ItemSize(size, style.FramePadding.y);
    if (!ItemAdd(bb, id)) return false;

    bool hovered, held;
    bool pressed = ButtonBehavior(bb, id, &hovered, &held);

    static std::map<ImGuiID, float> hoverAnim;
    float& animT = hoverAnim[id];
    float targetT = (selected || hovered) ? 1.0f : 0.0f;
    animT += (targetT - animT) * g.IO.DeltaTime * 12.0f;

    ImDrawList* dl = window->DrawList;
    
    if (selected) {
        dl->AddRectFilled(bb.Min, bb.Max, FluxPalette::Primary(220), 14.0f);
        dl->AddRectFilled(ImVec2(bb.Min.x, bb.Min.y + 4), ImVec2(bb.Min.x + 4, bb.Max.y - 4),
                          FluxPalette::PrimaryGlow(), 2.0f);
    } else if (animT > 0.01f) {
        ImU32 hoverCol = FluxPalette::Primary((int)(40 * animT));
        dl->AddRectFilled(bb.Min, bb.Max, hoverCol, 14.0f);
    }

    float iconOffset = 6.0f * animT;
    float iconX = bb.Min.x + 20.0f + iconOffset;
    float iconY = bb.Min.y + (size.y - FluxFont::Small) * 0.5f;
    FluxFont::RenderText(dl, icon, ImVec2(iconX, iconY),
                       selected ? FluxPalette::TextPrimary() : FluxPalette::TextMuted((int)(180 + 75 * animT)),
                       FluxFont::Small);
    
    float textX = bb.Min.x + 46.0f + iconOffset;
    float textY = bb.Min.y + (size.y - FluxFont::Small) * 0.5f;
    FluxFont::RenderText(dl, label, ImVec2(textX, textY),
                       selected ? FluxPalette::TextPrimary() : FluxPalette::TextSecondary((int)(200 + 55 * animT)),
                       FluxFont::Small);

    return pressed;
}

static bool ToggleSwitch(const char* label, bool* v) {
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems) return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);

    float height = 52.0f;
    float width = 76.0f;
    float radius = height * 0.5f;

    ImVec2 textSize = CalcTextSize(label);
    ImVec2 pos = window->DC.CursorPos;
    ImVec2 size = ImVec2(GetContentRegionAvail().x, ImMax(height, textSize.y) + style.FramePadding.y * 2 + 10.0f);

    const ImRect bb(pos, pos + size);
    ItemSize(size, style.FramePadding.y);
    if (!ItemAdd(bb, id)) return false;

    bool hovered, held;
    bool pressed = ButtonBehavior(bb, id, &hovered, &held);
    if (pressed) *v = !*v;

    static std::map<ImGuiID, float> switchAnim;
    float& animT = switchAnim[id];
    float targetT = *v ? 1.0f : 0.0f;
    animT += (targetT - animT) * g.IO.DeltaTime * 14.0f;

    ImDrawList* dl = window->DrawList;
    
    if (hovered) {
        dl->AddRectFilled(bb.Min, bb.Max, FluxPalette::SurfaceElevated(80), 10.0f);
    }
    
    ImVec2 togglePos = ImVec2(bb.Max.x - width - 15.0f, bb.Min.y + (size.y - height) * 0.5f);
    ImVec2 toggleEnd = ImVec2(togglePos.x + width, togglePos.y + height);
    
    // Track: interpolate from dark grey to orange
    ImVec4 off4 = FluxPalette::SurfaceElevatedV();
    ImVec4 on4 = FluxPalette::PrimaryV();
    ImVec4 track4;
    track4.x = off4.x + (on4.x - off4.x) * animT;
    track4.y = off4.y + (on4.y - off4.y) * animT;
    track4.z = off4.z + (on4.z - off4.z) * animT;
    track4.w = off4.w + (on4.w - off4.w) * animT;
    ImU32 trackCol = ImGui::ColorConvertFloat4ToU32(track4);
    dl->AddRectFilled(togglePos, toggleEnd, (ImU32)trackCol, radius);
    
    // Glow ring behind knob when ON
    if (animT > 0.01f) {
        float glowX = togglePos.x + radius + (width - height) * animT;
        float glowY = togglePos.y + radius;
        dl->AddCircleFilled(ImVec2(glowX, glowY), radius - 2.0f,
                           FluxPalette::Primary((int)(60 * animT)), 32);
    }
    
    // Knob
    float knobX = togglePos.x + radius + (width - height) * animT;
    float knobY = togglePos.y + radius;
    float knobR = radius - 4.0f;
    
    dl->AddCircleFilled(ImVec2(knobX, knobY + 1), knobR + 1.0f, IM_COL32(0, 0, 0, 30), 32);
    dl->AddCircleFilled(ImVec2(knobX, knobY), knobR, FluxPalette::TextPrimary(), 32);

    // Label
    dl->AddText(ImVec2(bb.Min.x + 15.0f, bb.Min.y + (size.y - textSize.y) * 0.5f),
                FluxPalette::TextPrimary(), label);

    return pressed;
}

INLINE void DrawAutoQueue() {
    if (!g_Token.empty() && !g_Auth.empty() && g_Token == g_Auth) {
        static std::chrono::steady_clock::time_point last_call_time;
        static std::chrono::steady_clock::time_point countdown_start;
        static bool counting = false;
        static float glowPhase = 0.0f;

        auto now = std::chrono::steady_clock::now();

        if (std::chrono::duration_cast<std::chrono::milliseconds>(now - last_call_time).count() > 500) {
            counting = false;
        }
        last_call_time = now;

        if (!counting) {
            counting = true;
            countdown_start = now;
        }

        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - countdown_start).count();
        int remaining_ms = 3000 - elapsed;

        if (remaining_ms <= 0) {
            if (sharedMenuManager.getMenuStateId() == 13) PopMenuState(13);
            StartLastMatch();
            counting = false;
            return;
        }

        glowPhase = fmod(glowPhase + ImGui::GetIO().DeltaTime * 1.25f, 1.0f);

        SetNextWindowPos(ImVec2(Width / 2.0f, Height / 2.0f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
        SetNextWindowSize(ImVec2(360, 260), ImGuiCond_Always);
        PushStyleColor(ImGuiCol_WindowBg, FluxPalette::SurfaceCardV(0.98f));
        PushStyleVar(ImGuiStyleVar_WindowRounding, 20.0f);

        if (Begin(O("##AutoQueue"), nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings)) {
            ImDrawList* dl = GetWindowDrawList();
            ImVec2 winPos = GetWindowPos();
            ImVec2 winSize = GetWindowSize();
            
            // Orange header gradient
            FluxPalette::GradientHPrimary(dl, winPos, ImVec2(winPos.x + winSize.x, winPos.y + 70));
            dl->AddRectFilled(winPos, ImVec2(winPos.x + winSize.x, winPos.y + 20),
                              FluxPalette::Primary(), 20.0f, ImDrawFlags_RoundCornersTop);
            
            ImVec2 titleSize = CalcTextSize(O("Auto Queue"));
            dl->AddText(ImGui::GetFont(), 16.0f,
                        ImVec2(winPos.x + (winSize.x - titleSize.x) * 0.5f, winPos.y + 24),
                        FluxPalette::TextPrimary(), O("Auto Queue"));

            SetCursorPosY(90);
            float font_scale = 3.5f;
            SetWindowFontScale(font_scale);

            std::string count_str = std::to_string((remaining_ms / 1000) + 1);
            auto text_size = CalcTextSize(count_str.c_str());
            float textX = (winSize.x - text_size.x) * 0.5f;
            SetCursorPosX(textX);

            float pulseA = 0.3f + 0.35f * (0.5f - 0.5f * cosf(glowPhase * 2.0f * 3.14159f));
            ImVec2 glowCenter(winPos.x + winSize.x * 0.5f, GetCursorScreenPos().y + text_size.y * 0.5f);
            float glowRadius = 60.0f + 10.0f * (0.5f - 0.5f * cosf(glowPhase * 2.0f * 3.14159f));
            dl->AddCircleFilled(glowCenter, glowRadius,
                                IM_COL32(255, 107, 0, (int)(pulseA * 40)), 32);
            dl->AddCircleFilled(glowCenter, glowRadius * 0.6f,
                                IM_COL32(255, 107, 0, (int)(pulseA * 25)), 24);
            
            TextColored(FluxPalette::PrimaryGlowV(), "%s", count_str.c_str());

            SetWindowFontScale(1.0f);

            SetCursorPosY(winSize.y - 75);
            SetCursorPosX(25);
            PushStyleColor(ImGuiCol_Button, FluxPalette::RedV());
            PushStyleColor(ImGuiCol_ButtonHovered, FluxPalette::RedGlowV());
            PushStyleVar(ImGuiStyleVar_FrameRounding, 12.0f);
            
            if (Button(O("Cancel"), ImVec2(winSize.x - 50, 50))) {
                persistent_bool[O("bAutoQueue")] = false;
                counting = false;
            }
            
            PopStyleVar();
            PopStyleColor(2);
            End();
        }
        PopStyleVar();
        PopStyleColor();
    }
}

INLINE void DrawESP(ImDrawList* draw) {
    if (!g_Token.empty() && !g_Auth.empty() && g_Token == g_Auth) {
        if (!sharedGameManager) return;

        UpdateScreenTable();

        sharedDirector = F(ptr, libmain + O(0x4f06288));
        if (!sharedDirector) return;

        sharedUserInfo = F(ptr, libmain + O(0x4e9feb8));
        if (!sharedUserInfo) return;

        F(bool, sharedUserInfo + 0x340) = true;

        sharedMainManager = F(ptr, libmain + O(0x4dde3e0));
        if (!sharedMainManager) return;

        sharedMenuManager = F(ptr, libmain + O(0x4dfe838));
        if (!sharedMenuManager) return;

        MainStateManager mainStateManager = sharedMainManager.mStateManager;
        if (!mainStateManager) return;
        if (!mainStateManager.isInGame()) {
        if (persistent_bool[O("bAutoQueue")]) {
            if (!sharedMenuManager.isInQueue()) DrawAutoQueue();
        } return;
        }

        auto visualCue = sharedGameManager.mVisualCue();

        Ball::Classification myclass = sharedGameManager.getPlayerClassification();

        Table table = sharedGameManager.mTable;
        if (!table) return;

        auto tableProperties = table.mTableProperties();
        if (!tableProperties) return;

        auto& pockets = tableProperties.mPockets();

        if (persistent_bool[O("bESP_DrawPockets")]) {
            for (int i = 0; i < 6; i++) {
                auto screenPos = WorldToScreen(pockets[i]);
                draw->AddCircle(ImVec2(screenPos.x, screenPos.y), 40, WHITE, 0, 3.f);
            }
        }

        GameStateManager gameStateManager = sharedGameManager.mStateManager;
        if (!gameStateManager) return;

        if (persistent_bool[O("bAutoPlay")]) AutoPlay::Update();

        auto stateId = gameStateManager.getCurrentStateId();
        if (stateId == 4) gPrediction->determineShotResult(false);
        if (stateId == 6 || stateId == 7 || stateId == 8) return;

        if (persistent_bool[O("bESP_DrawPocketsShotState")]) {
            for (int i = 0; i < 6; i++) {
                if (Prediction::pocketStatus[i]) {
                    auto screenPos = WorldToScreen(pockets[i]);
                    draw->AddCircle(ImVec2(screenPos.x, screenPos.y), 40, GREEN, 0, 5.f);
                }
            }
        }

        if (persistent_bool[O("bESP_DrawPredictionLine")]) {
            for (int i = 0; i < gPrediction->guiData.ballsCount; i++) {
                auto& ball = gPrediction->guiData.balls[i];

                if (ball.initialPosition != ball.predictedPosition) {
                    ImVec2 lastPos{};
                    for (int j = 1; j < ball.positions.size(); j++) {
                        auto point = WorldToScreen(ball.positions[j]);
                        if (lastPos.x || lastPos.y) draw->AddLine(lastPos, point, colors[i], 10.f);
                        lastPos = point;
                    }
                }
            }
        }

        if (persistent_bool[O("bESP_DrawPredictionLine")]) {
            for (int i = 0; i < gPrediction->guiData.ballsCount; i++) {
                auto& ball = gPrediction->guiData.balls[i];

                if (ball.initialPosition != ball.predictedPosition) {
                    draw->AddCircle(WorldToScreen(ball.initialPosition), 20, colors[i], 0, 6.f);
                    draw->AddCircleFilled(WorldToScreen(ball.predictedPosition), 20, colors[i]);
                }
            }
        }
    }
}

#include "ButtonClicker.h"

static void DrawSidebar(float sidebarW, float winH) {
    ImDrawList* dl = GetWindowDrawList();
    ImVec2 winPos = GetWindowPos();
    
    // Solid black sidebar with subtle right separator
    dl->AddRectFilled(winPos, ImVec2(winPos.x + sidebarW, winPos.y + winH), FluxPalette::SurfaceBg());
    dl->AddLine(ImVec2(winPos.x + sidebarW - 1, winPos.y), ImVec2(winPos.x + sidebarW - 1, winPos.y + winH),
                FluxPalette::Primary(60), 1.0f);
    
    SetCursorPos(ImVec2(0, 25));
    
    BeginGroup();
    
    Dummy(ImVec2(sidebarW, 5));
    SetCursorPosX(20);
    
    // Brand: FLUX in orange
    SetWindowFontScale(1.3f);
    TextColored(FluxPalette::PrimaryV(), O("INFERNO"));
    SetWindowFontScale(1.0f);
    
    SetCursorPosX(20);
    TextColored(FluxPalette::TextMutedV(), O("Engine v1.0"));
    
    Dummy(ImVec2(sidebarW, 35));
    
    SetCursorPosX(10);
    if (SidebarButton(O("ESP"), O("\xE2\x96\xA0"), g_menu.currentTab == 0, sidebarW) && g_menu.currentTab != 0) {
        g_menu.prevTab = g_menu.currentTab; g_menu.currentTab = 0; g_menu.tabTransition = 0.001f;
    }
    
    Dummy(ImVec2(0, 4));
    SetCursorPosX(10);
    if (SidebarButton(O("Auto Play"), O("\xE2\x97\x86"), g_menu.currentTab == 1, sidebarW) && g_menu.currentTab != 1) {
        g_menu.prevTab = g_menu.currentTab; g_menu.currentTab = 1; g_menu.tabTransition = 0.001f;
    }
    
    Dummy(ImVec2(0, 4));
    SetCursorPosX(10);
    if (SidebarButton(O("Auto Queue"), O("\xE2\x97\x8B"), g_menu.currentTab == 2, sidebarW) && g_menu.currentTab != 2) {
        g_menu.prevTab = g_menu.currentTab; g_menu.currentTab = 2; g_menu.tabTransition = 0.001f;
    }
    
    Dummy(ImVec2(0, 4));
    SetCursorPosX(10);
    if (SidebarButton(O("Stats"), O("\xE2\x96\xB2"), g_menu.currentTab == 3, sidebarW) && g_menu.currentTab != 3) {
        g_menu.prevTab = g_menu.currentTab; g_menu.currentTab = 3; g_menu.tabTransition = 0.001f;
    }
    
    // Theme switcher at sidebar bottom
    Dummy(ImVec2(0, winH - 340));
    SetCursorPosX(15);
    SetWindowFontScale(0.85f);
    TextColored(FluxPalette::TextMutedV(), O("Theme"));
    SetWindowFontScale(1.0f);
    SetCursorPosX(10);
    PushStyleVar(ImGuiStyleVar_FrameRounding, 8.0f);
    PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 6));
    SetNextItemWidth(sidebarW - 20);
    if (Combo("##theme", &current_theme, "Inferno\0Dark\0Light\0Classic\0")) {
        switch_theme(current_theme);
    }
    PopStyleVar(2);
    
    EndGroup();
}

static void DrawContentArea(float sidebarW, float winW, float winH) {
    bool need_save = false;
    
    ImDrawList* dl = GetWindowDrawList();
    ImVec2 winPos = GetWindowPos();
    
    // Content area header strip
    dl->AddRectFilled(ImVec2(winPos.x + sidebarW, winPos.y), ImVec2(winPos.x + winW, winPos.y + 60),
                      FluxPalette::SurfaceCard());
    dl->AddRectFilled(ImVec2(winPos.x + sidebarW, winPos.y + 59), ImVec2(winPos.x + winW, winPos.y + 60),
                      FluxPalette::Primary(100));
    
    const char* tabTitles[] = { "ESP Settings", "Auto Play", "Auto Queue", "Stats" };
    
    // Section header with orange accent bar
    dl->AddRectFilled(ImVec2(winPos.x + sidebarW + 15, winPos.y + 18),
                      ImVec2(winPos.x + sidebarW + 19, winPos.y + 42),
                      FluxPalette::Primary(), 2.0f);
    
    SetCursorPos(ImVec2(sidebarW + 28, 20));
    SetWindowFontScale(1.2f);
    TextColored(FluxPalette::TextPrimaryV(), "%s", tabTitles[g_menu.currentTab]);
    SetWindowFontScale(1.0f);
    
    SetCursorPos(ImVec2(sidebarW + 28, 44));
    TextColored(FluxPalette::TextMutedV(), O("Configure your settings below"));
    
    SetCursorPos(ImVec2(sidebarW + 15, 75));
    
    // Tab transition slide effect
    float contentOffset = 0.0f;
    float contentAlpha = 1.0f;
    if (g_menu.tabTransition < 1.0f && g_menu.tabTransition > 0.001f) {
        float eased = EaseOutQuart(g_menu.tabTransition);
        contentOffset = (1.0f - eased) * 40.0f;  // slide in from right
        contentAlpha = eased;
    }
    
    PushStyleColor(ImGuiCol_ChildBg, ImVec4(0, 0, 0, 0));
    PushStyleVar(ImGuiStyleVar_ScrollbarSize, 4.0f);
    SetCursorPosX(GetCursorPosX() + contentOffset);
    BeginChild(O("##ContentArea"), ImVec2(winW - sidebarW - 30, winH - 90), false, ImGuiWindowFlags_NoScrollbar);
    
    switch (g_menu.currentTab) {
        case 0: {
            Dummy(ImVec2(0, 15));
            need_save |= ToggleSwitch(O("Draw Prediction Lines"), &persistent_bool[O("bESP_DrawPredictionLine")]);
            Dummy(ImVec2(0, 4));
            need_save |= ToggleSwitch(O("Draw Pockets"), &persistent_bool[O("bESP_DrawPockets")]);
            Dummy(ImVec2(0, 4));
            need_save |= ToggleSwitch(O("Draw Shot State"), &persistent_bool[O("bESP_DrawPocketsShotState")]);
            break;
        }
        
        case 1: {
            Dummy(ImVec2(0, 15));
            need_save |= ToggleSwitch(O("Enable Auto Play"), &persistent_bool[O("bAutoPlay")]);
            
            Dummy(ImVec2(0, 25));
            TextColored(FluxPalette::TextMutedV(), O("Auto play will automatically"));
            TextColored(FluxPalette::TextMutedV(), O("aim and shoot for you"));
            break;
        }
        
        case 2: {
            Dummy(ImVec2(0, 15));
            need_save |= ToggleSwitch(O("Enable Auto Queue"), &persistent_bool[O("bAutoQueue")]);
            Dummy(ImVec2(0, 25));
            
            TextColored(FluxPalette::TextSecondaryV(), O("Mode"));
            Dummy(ImVec2(0, 8));
            SetNextItemWidth(GetContentRegionAvail().x);
            need_save |= Combo("##mode", &persistent_int["iAutoQueue_Mode"], "Last Selected\0Smart\0");
            
            if (persistent_int["iAutoQueue_Mode"] == 1) {
                Dummy(ImVec2(0, 20));
                TextColored(FluxPalette::TextSecondaryV(), O("Bet Percent"));
                Dummy(ImVec2(0, 8));
                SetNextItemWidth(GetContentRegionAvail().x);
                need_save |= SliderInt("##betpercent", &persistent_int["iAutoQueue_BetPercent"], 1, 100, "%d%%");
            }
            
            Dummy(ImVec2(0, 30));
            TextColored(FluxPalette::TextMutedV(), O("You will be auto queued to"));
            TextColored(FluxPalette::TextMutedV(), O("the last game mode you played"));
            break;
        }
        
        case 3: {
            Dummy(ImVec2(0, 15));
            TextColored(FluxPalette::TextSecondaryV(), O("Game Statistics"));
            Dummy(ImVec2(0, 12));
            
            struct StatRow { const char* label; int* value; };
            static int gamesPlayed = 0, wins = 0, losses = 0, winStreak = 0, bestWinStreak = 0;
            StatRow stats[] = {
                { O("Games Played"), &gamesPlayed },
                { O("Wins"), &wins },
                { O("Losses"), &losses },
                { O("Win Streak"), &winStreak },
                { O("Best Streak"), &bestWinStreak },
            };
            for (int i = 0; i < 5; i++) {
                float rowAlpha = 0.03f * (i % 2 == 0 ? 1 : 0);
                ImVec2 rowMin = GetCursorScreenPos();
                ImVec2 rowMax(rowMin.x + GetContentRegionAvail().x, rowMin.y + 40);
                dl->AddRectFilled(rowMin, rowMax, FluxPalette::SurfaceCard((int)(rowAlpha * 255)));
                Dummy(ImVec2(0, 12));
                SetCursorPosX(10);
                TextColored(FluxPalette::TextMutedV(), "%s", stats[i].label);
                SameLine(GetContentRegionAvail().x - 50);
                TextColored(FluxPalette::PrimaryV(), "%d", *stats[i].value);
                Dummy(ImVec2(0, 4));
            }
            
            Dummy(ImVec2(0, 20));
            SetCursorPosX(10);
            if (Button(O("Reset Stats"), ImVec2(GetContentRegionAvail().x - 20, 44))) {
                gamesPlayed = wins = losses = winStreak = bestWinStreak = 0;
            }
            break;
        }
    }
    
    if (need_save) save_persistence();
    
    EndChild();
    PopStyleVar();
    PopStyleColor();
}

// ── Background Particles ──────────────────────────────────────
static const int MAX_PARTICLES = 30;
struct Particle {
    ImVec2 pos;
    ImVec2 vel;
    float size;
    float alpha;
    float speed;
};
static Particle g_particles[MAX_PARTICLES];
static bool g_particles_init = false;

static void InitParticles() {
    for (int i = 0; i < MAX_PARTICLES; i++) {
        g_particles[i].pos = ImVec2(
            (float)(rand() % 800),
            (float)(rand() % 580)
        );
        g_particles[i].vel = ImVec2(
            (float)(rand() % 20 - 10) * 0.5f,
            -(float)(rand() % 30 + 10) * 0.3f
        );
        g_particles[i].size = (float)(rand() % 6 + 2);
        g_particles[i].alpha = (float)(rand() % 60 + 20) / 255.0f;
        g_particles[i].speed = (float)(rand() % 20 + 10) * 0.1f;
    }
    g_particles_init = true;
}

static void DrawParticles(ImDrawList* dl, float winX, float winY, float winW, float winH, float alpha) {
    if (!g_particles_init) InitParticles();
    
    for (int i = 0; i < MAX_PARTICLES; i++) {
        auto& p = g_particles[i];
        p.pos.x += p.vel.x * ImGui::GetIO().DeltaTime * p.speed;
        p.pos.y += p.vel.y * ImGui::GetIO().DeltaTime * p.speed;
        
        // Slow drift
        p.vel.x += (float)(sinf(ImGui::GetTime() * 0.5f + i) * 0.1f) * ImGui::GetIO().DeltaTime;
        
        // Wrap around
        if (p.pos.y < -20) {
            p.pos.y = winH + 20;
            p.pos.x = (float)(rand() % (int)winW);
        }
        if (p.pos.x < -20) p.pos.x = winW + 20;
        if (p.pos.x > winW + 20) p.pos.x = -20;
        
        float fade = p.alpha * alpha;
        if (fade <= 0.0f) continue;
        
        ImVec2 screenPos(winX + p.pos.x, winY + p.pos.y);
        ImU32 col = FluxPalette::Primary((int)(fade * 255));
        dl->AddCircleFilled(screenPos, p.size, col, 16);
    }
}

INLINE void DrawMenu(ImGuiIO& io) {
    if (true) { // DEBUG: auth bypass — outer Draw() already guards this
        if (is_segv_handler_active()) {
            jump_buffer_active = 1;
            if (!sigsetjmp(jump_buffer, 1)) DrawESP(GetBackgroundDrawList());
            jump_buffer_active = 0;
        }

        float targetAlpha = g_menu.isOpen ? 1.0f : 0.0f;
        if (g_menu.isOpen) {
            g_menu.menuAlpha += (1.0f - g_menu.menuAlpha) * io.DeltaTime * 12.0f;
        } else {
            g_menu.menuAlpha = 0.0f;
        }

        // Animate tab transition
        if (g_menu.tabTransition > 0.001f && g_menu.tabTransition < 1.0f) {
            g_menu.tabTransition += io.DeltaTime * 6.0f;
            if (g_menu.tabTransition >= 1.0f) g_menu.tabTransition = 1.0f;
        } else if (g_menu.tabTransition < 0.001f) {
            g_menu.tabTransition = 1.0f;
        }

        if (g_menu.menuAlpha > 0.01f) {
            float winW = 860.0f;
            float winH = 620.0f;
            
            SetNextWindowSize(ImVec2(winW, winH), ImGuiCond_Always);
            SetNextWindowPos(ImVec2(Width / 2.0f, Height / 2.0f), ImGuiCond_FirstUseEver, ImVec2(0.5f, 0.5f));
            
            PushStyleColor(ImGuiCol_WindowBg, FluxPalette::SurfaceCardV(g_menu.menuAlpha * 0.92f));
            PushStyleVar(ImGuiStyleVar_WindowRounding, 16.0f);
            PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
            PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
            PushStyleVar(ImGuiStyleVar_Alpha, g_menu.menuAlpha);
            
            ImGuiWindowFlags winFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
            
            if (Begin(O("##MainMenu"), &g_menu.isOpen, winFlags)) {
                ImDrawList* dl = GetWindowDrawList();
                ImVec2 winPos = GetWindowPos();
                
                // Background particles
                DrawParticles(dl, winPos.x, winPos.y, winW, winH, g_menu.menuAlpha * 0.6f);
                
                // Glass panel overlay (subtle)
                DrawGlassPanel(dl, winPos, ImVec2(winPos.x + winW, winPos.y + winH),
                               FluxPalette::SurfaceBg(), 16.0f, 0.08f);
                
                dl->AddRect(winPos, ImVec2(winPos.x + winW, winPos.y + winH), FluxPalette::Primary((int)(150 * g_menu.menuAlpha)), 16.0f, 0, 1.5f);
                
                DrawSidebar(g_menu.sidebarWidth, winH);
                DrawContentArea(g_menu.sidebarWidth, winW, winH);
            }
            End();
            
            PopStyleVar(4);
            PopStyleColor();
        }
    }
}

static void DrawFloatingButton(ImGuiIO& io) {
    static ImVec2 buttonPos = ImVec2(80, 60);
    static bool isDragging = false;
    static float hoverAnim = 0.0f;
    static float pulseTime = 0.0f;
    
    float buttonRadius = 40.0f;
    float buttonSize = buttonRadius * 2.0f;
    float textWidth = 140.0f;
    float totalWidth = buttonSize + textWidth + 15.0f;
    float totalHeight = buttonSize + 4.0f;
    
    bool isHovered = false;
    pulseTime += io.DeltaTime;
    
    SetNextWindowPos(buttonPos, ImGuiCond_Always);
    SetNextWindowSize(ImVec2(totalWidth, totalHeight), ImGuiCond_Always);
    PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));
    PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    
    if (Begin(O("##FloatBtn"), nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings)) {
        ImDrawList* dl = GetWindowDrawList();
        
        ImVec2 center = ImVec2(buttonPos.x + buttonRadius + 2, buttonPos.y + buttonRadius + 2);
        
        SetCursorPos(ImVec2(0, 0));
        InvisibleButton(O("##FloatBtnHit"), ImVec2(totalWidth, totalHeight));
        isHovered = IsItemHovered();
        
        float targetHover = isHovered ? 1.0f : 0.0f;
        hoverAnim += (targetHover - hoverAnim) * io.DeltaTime * 10.0f;
        
        float currentRadius = buttonRadius + hoverAnim * 4.0f;
        float pulse = sinf(pulseTime * 2.0f) * 0.04f + 1.0f;  // subtle breathing
        
        // Outer glow ring (animated with pulse + hover)
        int glowAlpha = (int)(80 + 60 * hoverAnim + 40 * (pulse - 1.0f));
        dl->AddCircle(center, currentRadius * 1.3f * pulse,
                      FluxPalette::Primary(glowAlpha), 48, 4.0f);
        dl->AddCircleFilled(ImVec2(center.x + 2, center.y + 3),
                            currentRadius, IM_COL32(0, 0, 0, 60), 48);
        
        // Main circle: gradient outer (dark orange) → inner (bright orange)
        dl->AddCircleFilled(center, currentRadius,
                            FluxPalette::PrimaryDark(), 48);
        dl->AddCircleFilled(center, currentRadius - 3,
                            FluxPalette::Primary(), 48);
        dl->AddCircleFilled(center, currentRadius - 6,
                            FluxPalette::PrimaryGlow(), 48);
        
        // Glow border ring
        dl->AddCircle(center, currentRadius,
                      FluxPalette::PrimaryGlow((int)(180 + 75 * hoverAnim)),
                      48, 2.5f);
        
        // Icon (white)
        float iconSize = 14.0f;
        ImU32 iconColor = FluxPalette::TextPrimary();
        
        if (g_menu.isOpen) {
            // Close (X)
            dl->AddLine(ImVec2(center.x - iconSize, center.y - iconSize),
                        ImVec2(center.x + iconSize, center.y + iconSize),
                        iconColor, 3.0f);
            dl->AddLine(ImVec2(center.x + iconSize, center.y - iconSize),
                        ImVec2(center.x - iconSize, center.y + iconSize),
                        iconColor, 3.0f);
        } else {
            // Hamburger (3 bars)
            float barW = 16.0f;
            float barH = 2.5f;
            float gap = 5.5f;
            float r = 2.0f;
            dl->AddRectFilled(ImVec2(center.x - barW, center.y - gap - barH),
                              ImVec2(center.x + barW, center.y - gap),
                              iconColor, r);
            dl->AddRectFilled(ImVec2(center.x - barW, center.y - barH * 0.5f),
                              ImVec2(center.x + barW, center.y + barH * 0.5f),
                              iconColor, r);
            dl->AddRectFilled(ImVec2(center.x - barW, center.y + gap),
                              ImVec2(center.x + barW, center.y + gap + barH),
                              iconColor, r);
        }
        
        // Text
        float textX = buttonPos.x + buttonSize + 12.0f;
        float textY = buttonPos.y + (totalHeight - 38.0f) * 0.5f;
        dl->AddText(ImVec2(textX, textY), FluxPalette::TextPrimary(), O("FLUX"));
        dl->AddText(ImVec2(textX, textY + 24.0f), FluxPalette::TextMuted(), O("Engine"));
        
        // Drag
        if (IsItemActive() && IsMouseDragging(0)) {
            isDragging = true;
            buttonPos.x += io.MouseDelta.x;
            buttonPos.y += io.MouseDelta.y;
            buttonPos.x = ImClamp(buttonPos.x, 0.0f, (float)Width - totalWidth);
            buttonPos.y = ImClamp(buttonPos.y, 0.0f, (float)Height - totalHeight);
        }
        
        if (IsItemHovered() && IsMouseReleased(0) && !isDragging) {
            g_menu.isOpen = !g_menu.isOpen;
        }
        
        if (!IsItemActive()) isDragging = false;
    }
    End();
    
    PopStyleVar(2);
    PopStyleColor();
}


static bool first_time = true;
INLINE void DrawLogin(ImGuiIO& io) {
    if (logged_in) return DrawMenu(io);

    SetNextWindowPos(ImVec2(0, 0));
    SetNextWindowSize(io.DisplaySize);
    PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.00f, 0.00f, 0.00f, 0.88f));
    Begin(O("##Overlay"), nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoBringToFrontOnFocus);
    PopStyleColor();

    float cardW = 520;
    float cardH = 440;

    SetNextWindowSize(ImVec2(cardW, cardH), ImGuiCond_Always);
    SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));

    PushStyleColor(ImGuiCol_WindowBg, FluxPalette::SurfaceCardV());
    PushStyleVar(ImGuiStyleVar_WindowRounding, 20.0f);
    PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

    Begin(O("##LoginCard"), nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar);

    ImDrawList* dl = GetWindowDrawList();
    ImVec2 winPos = GetWindowPos();
    float headerH = 120.0f;

    // Glass panel border
    ImU32 borderCol = FluxPalette::Primary(30);
    dl->AddRect(winPos, ImVec2(winPos.x + cardW, winPos.y + cardH), borderCol, 20.0f, ImDrawFlags_RoundCornersAll, 1.0f);

    // Header: orange gradient with decorative arc
    FluxPalette::GradientHPrimary(dl, winPos, ImVec2(winPos.x + cardW, winPos.y + headerH));
    dl->AddRectFilled(winPos, ImVec2(winPos.x + cardW, winPos.y + 20), FluxPalette::Primary(), 20.0f, ImDrawFlags_RoundCornersTop);

    // Decorative arc at header bottom
    ImVec2 arcCenter(winPos.x + cardW * 0.5f, winPos.y + headerH);
    dl->PathClear();
    float arcR = 80.0f;
    float arcW = 3.0f;
    dl->PathArcTo(arcCenter, arcR, PI, 0.0f, 40);
    dl->PathStroke(FluxPalette::Amber(60), ImDrawFlags_None, arcW);
    dl->PathArcTo(arcCenter, arcR + 12.0f, PI * 1.1f, -0.1f, 40);
    dl->PathStroke(FluxPalette::Primary(40), ImDrawFlags_None, arcW);

    // Title
    ImVec2 titleSize = FluxFont::CalcTextBox(O("FLUX ENGINE"), FluxFont::H2);
    dl->AddText(ImGui::GetFont(), FluxFont::H2,
                ImVec2(winPos.x + (cardW - titleSize.x) * 0.5f, winPos.y + 30),
                FluxPalette::TextPrimary(), O("FLUX ENGINE"));

    // Subtitle
    ImVec2 subSize = FluxFont::CalcTextBox(O("8 Ball Pool Mod"), FluxFont::Small);
    dl->AddText(ImGui::GetFont(), FluxFont::Small,
                ImVec2(winPos.x + (cardW - subSize.x) * 0.5f, winPos.y + 72),
                FluxPalette::TextSecondary(200), O("8 Ball Pool Mod"));

    SetCursorPosY(135);

    // Error message
    if (!ERROR_MESSAGE.empty()) {
        SetCursorPosX(30);
        PushTextWrapPos(cardW - 30);
        TextColored(ImVec4(1.0f, 0.2f, 0.2f, 1.0f), "%s", ERROR_MESSAGE.c_str());
        PopTextWrapPos();
        Dummy(ImVec2(0, 15));
    }

    if (is_logging_in) {
        SetCursorPosY(180);
        
        static float spinner_angle = 0.0f;
        spinner_angle += io.DeltaTime * 4.0f;

        // Arc spinner (2 arcs rotating)
        ImVec2 spinCenter(winPos.x + cardW * 0.5f, winPos.y + 210);
        float spinR = 36.0f;
        float arcLen = PI * 0.6f;

        dl->PathClear();
        dl->PathArcTo(spinCenter, spinR, spinner_angle, spinner_angle + arcLen, 24);
        dl->PathStroke(FluxPalette::Primary(), ImDrawFlags_None, 5.0f);

        dl->PathClear();
        dl->PathArcTo(spinCenter, spinR, spinner_angle + PI, spinner_angle + PI + arcLen, 24);
        dl->PathStroke(FluxPalette::PrimaryGlow(180), ImDrawFlags_None, 4.0f);

        // Center dot
        dl->AddCircleFilled(spinCenter, 4.0f, FluxPalette::PrimaryGlow());

        // Loading text
        SetCursorPosX((cardW - 150.0f) * 0.5f);
        SetCursorPosY(280);
        TextColored(FluxPalette::TextMutedV(), O("Authenticating..."));
    } else {
        SetCursorPosY(165);
        
        // Instruction text
        ImVec2 infoSize = FluxFont::CalcTextBox(O("Paste your license key"), FluxFont::Small);
        SetCursorPosX((cardW - infoSize.x) * 0.5f);
        TextColored(FluxPalette::TextMutedV(), O("Paste your license key"));

        Dummy(ImVec2(0, 40));
        
        bool AutoLogin = first_time && !persistent_string[O("key")].empty();
        
        SetCursorPosX(40);
        PushStyleColor(ImGuiCol_Button, FluxPalette::PrimaryV());
        PushStyleColor(ImGuiCol_ButtonHovered, FluxPalette::PrimaryGlowV());
        PushStyleColor(ImGuiCol_ButtonActive, FluxPalette::PrimaryDarkV());
        PushStyleVar(ImGuiStyleVar_FrameRounding, 14.0f);
        
        if (AutoLogin || Button(O("LOGIN WITH CLIPBOARD"), ImVec2(cardW - 80, 64))) {
            JNIEnv* env;
            jint getEnvResult = VM->GetEnv((void**)&env, JNI_VERSION_1_6);
            if (getEnvResult == JNI_EDETACHED) {
                if (VM->AttachCurrentThread(&env, nullptr) != 0) ERROR_MESSAGE = O("Failed to attach thread to JVM");
            } else if (getEnvResult != JNI_OK) {
                ERROR_MESSAGE = O("Failed to get JNIEnv");
            } else {
                std::thread([](std::string androidId, std::string key) {
                    Login(androidId, key);
                }, getAndroidID(env), AutoLogin ? persistent_string[O("key")] : getClipboard(env)).detach();
            }

            first_time = false;
        }
        
        PopStyleVar();
        PopStyleColor(3);
        
        Dummy(ImVec2(0, 30));
        
        // Footer text
        ImVec2 helpSize = FluxFont::CalcTextBox(O("Key is read from clipboard"), FluxFont::Tiny);
        SetCursorPosX((cardW - helpSize.x) * 0.5f);
        TextColored(FluxPalette::TextMutedV(), O("Key is read from clipboard"));
    }

    End();
    PopStyleVar(3);
    PopStyleColor();
    
    End();
}


INLINE void SetupImgui() {
    PACKAGE_NAME = string(getcmdline());

    ImGui::CreateContext();

    auto& style = ImGui::GetStyle();
    auto& io = ImGui::GetIO();

    io.ConfigFlags |= ImGuiConfigFlags_IsTouchScreen;

    switch_theme(current_theme);

    load_persistence();
    load_imgui_style();

    static string INI_PATH = O("/data/user_de/0/") + PACKAGE_NAME + O("/no_backup/.ini");
    io.IniFilename = persistent_bool["bImguiAutoSave"] ? INI_PATH.c_str() : nullptr;
    io.ConfigWindowsMoveFromTitleBarOnly = persistent_bool["bMoveOnlyWithTitleBar"];

    ImFontConfig font_cfg;
    font_cfg.SizePixels = persistent_float["fFontScale"];
    io.Fonts->AddFontDefault(&font_cfg);

    ImGui_ImplAndroid_Init();
    ImGui_ImplOpenGL3_Init(O("#version 300 es"));

    bImguiSetup = true;
}

DEFINES(EGLBoolean, Draw, EGLDisplay dpy, EGLSurface surface) {
    eglQuerySurface(dpy, surface, EGL_WIDTH, &Width);
    eglQuerySurface(dpy, surface, EGL_HEIGHT, &Height);

    if (Width <= 0 || Height <= 0) return _Draw(dpy, surface);

    screenCenter = Vector2(Width / 2, Height / 2);

    if (!bImguiSetup) SetupImgui();

    ImGuiIO& io = ImGui::GetIO();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplAndroid_NewFrame(Width, Height);
    ImGui::NewFrame();

    if (!is_segv_handler_active()) setup_global_segv_handler();
    if (true) { // DEBUG: auth bypass
        DrawFloatingButton(io);
        DrawMenu(io);
    } else {
        DrawLogin(io);
    }
    StatusOverlay::Tick();
    StatusOverlay::Draw();
    ImGui::EndFrame();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    ImGui_ClearHoverEffect();

    return _Draw(dpy, surface);
}

void __IMGUI__() {
    create_directory_recursive(CONC(O("/data/user_de/0/"), PACKAGE_NAME.c_str(), O("/no_backup")));
}
