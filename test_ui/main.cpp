// Inferno UI — Standalone Desktop Test
// Compile: mkdir build && cd build && cmake .. && make
// Run: ./inferno_ui_test

#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <map>
#include <string>
#include <vector>

#include <GLFW/glfw3.h>

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

// ═══════════════════════════════════════════════════════════════
// STUBS — replace game-specific dependencies
// ═══════════════════════════════════════════════════════════════

// O() — string obfuscation stub (just returns the string)
#define O(x) x

// Game globals stubs
static int Width = 1280, Height = 720;
static bool bImguiSetup = false;

// Persistence stubs
static std::map<std::string, bool> persistent_bool;
static std::map<std::string, float> persistent_float = { {"fFontScale", 38.0f} };
static std::map<std::string, int> persistent_int;
static int current_theme = 0;

static void save_persistence() {}
static void load_persistence() {}
static void load_imgui_style() {}
static void switch_theme(int idx) {}

// Game stubs
struct Vector2 { float x, y; };
static Vector2 screenCenter;

// ═══════════════════════════════════════════════════════════════
// FLUX PALETTE — copied from custom_theme.h
// ═══════════════════════════════════════════════════════════════

struct FluxPalette {
    static ImU32 SurfaceBg(float a = 1.0f)     { return IM_COL32(0, 0, 0, (int)(a * 255)); }
    static ImU32 SurfaceCard(float a = 1.0f)   { return IM_COL32(10, 10, 10, (int)(a * 255)); }
    static ImU32 SurfaceElevated(float a = 1.0f) { return IM_COL32(20, 20, 22, (int)(a * 255)); }
    static ImU32 Primary(float a = 1.0f)       { return IM_COL32(255, 107, 0, (int)(a * 255)); }
    static ImU32 PrimaryGlow(float a = 1.0f)   { return IM_COL32(255, 136, 0, (int)(a * 255)); }
    static ImU32 PrimaryDark(float a = 1.0f)   { return IM_COL32(204, 85, 0, (int)(a * 255)); }
    static ImU32 Amber(float a = 1.0f)         { return IM_COL32(255, 153, 0, (int)(a * 255)); }
    static ImU32 Red(float a = 1.0f)           { return IM_COL32(255, 34, 34, (int)(a * 255)); }
    static ImU32 RedGlow(float a = 1.0f)       { return IM_COL32(255, 68, 68, (int)(a * 255)); }
    static ImU32 TextPrimary(float a = 1.0f)   { return IM_COL32(255, 255, 255, (int)(a * 255)); }
    static ImU32 TextSecondary(float a = 1.0f) { return IM_COL32(204, 204, 204, (int)(a * 255)); }
    static ImU32 TextMuted(float a = 1.0f)     { return IM_COL32(136, 136, 136, (int)(a * 255)); }
    static ImVec4 PrimaryV(float a = 1.0f)     { return ImVec4(1.00f, 0.42f, 0.00f, a); }
    static ImVec4 PrimaryGlowV(float a = 1.0f) { return ImVec4(1.00f, 0.53f, 0.00f, a); }
    static ImVec4 PrimaryDarkV(float a = 1.0f) { return ImVec4(0.80f, 0.33f, 0.00f, a); }
    static ImVec4 SurfaceCardV(float a = 1.0f) { return ImVec4(0.04f, 0.04f, 0.04f, a); }
    static ImVec4 SurfaceBgV(float a = 1.0f)   { return ImVec4(0.00f, 0.00f, 0.00f, a); }
    static ImVec4 SurfaceElevatedV(float a = 1.0f) { return ImVec4(0.08f, 0.08f, 0.09f, a); }
    static ImVec4 RedV(float a = 1.0f)         { return ImVec4(1.00f, 0.13f, 0.13f, a); }
    static ImVec4 TextPrimaryV(float a = 1.0f) { return ImVec4(1.00f, 1.00f, 1.00f, a); }
    static ImVec4 TextSecondaryV(float a = 1.0f){ return ImVec4(0.80f, 0.80f, 0.80f, a); }
    static ImVec4 TextMutedV(float a = 1.0f)   { return ImVec4(0.53f, 0.53f, 0.53f, a); }
};

// ═══════════════════════════════════════════════════════════════
// FLUX FONT — copied from typography.h
// ═══════════════════════════════════════════════════════════════

struct FluxFont {
    static constexpr float H1    = 48.0f;
    static constexpr float H2    = 40.0f;
    static constexpr float Body  = 34.0f;
    static constexpr float Small = 28.0f;
    static constexpr float Tiny  = 22.0f;

    static void RenderText(ImDrawList* dl, const char* text, ImVec2 pos,
                           ImU32 color, float size, ImU32 shadow = 0) {
        if (shadow)
            dl->AddText(ImGui::GetFont(), size * 1.02f,
                        ImVec2(pos.x + 1, pos.y + 1), shadow, text);
        dl->AddText(ImGui::GetFont(), size, pos, color, text);
    }
    static ImVec2 CalcTextBox(const char* text, float size) {
        return ImGui::GetFont()->CalcTextSizeA(size, FLT_MAX, 0.0f, text);
    }
};

// ═══════════════════════════════════════════════════════════════
// DRAW HELPERS — copied from draw_helpers.h (simplified)
// ═══════════════════════════════════════════════════════════════

static void DrawGlassPanel(ImDrawList* dl, ImVec2 p1, ImVec2 p2,
                           ImU32 bg, float rounding, float glareAlpha) {
    dl->AddRectFilled(p1, p2, bg, rounding);
    if (glareAlpha > 0.01f) {
        float h = p2.y - p1.y;
        dl->AddRectFilledMultiColor(p1, ImVec2(p2.x, p1.y + h * 0.4f),
            IM_COL32(255, 255, 255, (int)(12 * glareAlpha)),
            IM_COL32(255, 255, 255, (int)(6 * glareAlpha)),
            IM_COL32(255, 255, 255, (int)(3 * glareAlpha)),
            IM_COL32(255, 255, 255, 0));
    }
}

// ═══════════════════════════════════════════════════════════════
// MENU STATE
// ═══════════════════════════════════════════════════════════════

struct MenuState {
    bool isOpen = false;
    int currentTab = 0;
    int prevTab = 0;
    float tabTransition = 0.0f;
    float tabBarHeight = 80.0f;
    float animProgress = 0.0f;
    float menuAlpha = 0.0f;
    float menuScale = 0.9f;
    ImVec4 accentColor = FluxPalette::PrimaryV();
};
static MenuState g_menu;

// ═══════════════════════════════════════════════════════════════
// EASING FUNCTIONS
// ═══════════════════════════════════════════════════════════════

static float EaseOutBack(float x) {
    const float c1 = 1.70158f, c3 = c1 + 1.0f;
    return 1.0f + c3 * powf(x - 1.0f, 3.0f) + c1 * powf(x - 1.0f, 2.0f);
}

static float EaseOutQuart(float x) {
    return 1.0f - powf(1.0f - x, 4.0f);
}

static void DrawGradientRect(ImDrawList* dl, ImVec2 p1, ImVec2 p2,
                             ImU32 col1, ImU32 col2, bool horizontal = true) {
    if (horizontal)
        dl->AddRectFilledMultiColor(p1, p2, col1, col2, col2, col1);
    else
        dl->AddRectFilledMultiColor(p1, p2, col1, col1, col2, col2);
}

// ═══════════════════════════════════════════════════════════════
// TAB BUTTON
// ═══════════════════════════════════════════════════════════════

static bool TabButton(const char* label, const char* icon, bool selected,
                      float width, float height) {
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems) return false;
    ImGuiContext& g = *GImGui;
    const ImGuiID id = window->GetID(label);
    ImVec2 pos = window->DC.CursorPos;
    ImVec2 size = ImVec2(width, height);
    const ImRect bb(pos, pos + size);
    ImGui::ItemSize(size, 0.0f);
    if (!ImGui::ItemAdd(bb, id)) return false;

    bool hovered, held;
    bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held);

    static std::map<ImGuiID, float> hoverAnim;
    float& animT = hoverAnim[id];
    float targetT = (selected || hovered) ? 1.0f : 0.0f;
    animT += (targetT - animT) * g.IO.DeltaTime * 12.0f;

    ImDrawList* dl = window->DrawList;
    if (selected) {
        dl->AddRectFilled(bb.Min, bb.Max, FluxPalette::Primary(220), 12.0f);
    } else if (animT > 0.01f) {
        dl->AddRectFilled(bb.Min, bb.Max, FluxPalette::Primary((int)(40 * animT)), 12.0f);
    }

    float iconY = bb.Min.y + (height - FluxFont::Small) * 0.5f;
    ImVec2 textSize = ImGui::CalcTextSize(label);
    float totalW = 14.0f + 8.0f + textSize.x;
    float startX = bb.Min.x + (width - totalW) * 0.5f;

    FluxFont::RenderText(dl, icon, ImVec2(startX, iconY),
        selected ? FluxPalette::TextPrimary() : FluxPalette::TextMuted((int)(180 + 75 * animT)),
        FluxFont::Small);
    FluxFont::RenderText(dl, label, ImVec2(startX + 14.0f + 8.0f, iconY),
        selected ? FluxPalette::TextPrimary() : FluxPalette::TextSecondary((int)(200 + 55 * animT)),
        FluxFont::Small);

    return pressed;
}

// ═══════════════════════════════════════════════════════════════
// DRAW TAB BAR
// ═══════════════════════════════════════════════════════════════

static void DrawTabBar(float winW) {
    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImVec2 winPos = ImGui::GetWindowPos();
    float barH = g_menu.tabBarHeight;

    dl->AddRectFilled(winPos, ImVec2(winPos.x + winW, winPos.y + barH), FluxPalette::SurfaceCard());
    dl->AddRectFilled(ImVec2(winPos.x, winPos.y + barH - 1), ImVec2(winPos.x + winW, winPos.y + barH),
                      FluxPalette::Primary(80), 1.0f);

    const int tabCount = 4;
    const char* tabLabels[] = { "ESP", "Auto Play", "Auto Queue", "Stats" };
    const char* tabIcons[] = { "\xE2\x96\xA0", "\xE2\x97\x86", "\xE2\x97\x8B", "\xE2\x96\xB2" };

    float tabHeight = 56.0f;
    float tabY = winPos.y + (barH - tabHeight) * 0.5f;
    float tabWidth = winW / tabCount;

    for (int i = 0; i < tabCount; i++) {
        ImGui::SetCursorPos(ImVec2(i * tabWidth, tabY - winPos.y));
        if (TabButton(tabLabels[i], tabIcons[i], g_menu.currentTab == i, tabWidth, tabHeight)
            && g_menu.currentTab != i) {
            g_menu.prevTab = g_menu.currentTab;
            g_menu.currentTab = i;
            g_menu.tabTransition = 0.001f;
        }
    }
}

// ═══════════════════════════════════════════════════════════════
// TOGGLE SWITCH
// ═══════════════════════════════════════════════════════════════

static bool ToggleSwitch(const char* label, bool* v) {
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems) return false;
    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);

    float height = 52.0f;
    float width = 76.0f;
    float radius = height * 0.5f;

    ImVec2 textSize = ImGui::CalcTextSize(label);
    ImVec2 pos = window->DC.CursorPos;
    ImVec2 size = ImVec2(ImGui::GetContentRegionAvail().x,
                         ImMax(height, textSize.y) + style.FramePadding.y * 2 + 10.0f);
    const ImRect bb(pos, pos + size);
    ImGui::ItemSize(size, style.FramePadding.y);
    if (!ImGui::ItemAdd(bb, id)) return false;

    bool hovered, held;
    bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held);
    if (pressed) *v = !*v;

    static std::map<ImGuiID, float> switchAnim;
    float& animT = switchAnim[id];
    float targetT = *v ? 1.0f : 0.0f;
    animT += (targetT - animT) * g.IO.DeltaTime * 14.0f;

    ImDrawList* dl = window->DrawList;
    if (hovered)
        dl->AddRectFilled(bb.Min, bb.Max, FluxPalette::SurfaceElevated(80), 10.0f);

    ImVec2 togglePos = ImVec2(bb.Max.x - width - 15.0f, bb.Min.y + (size.y - height) * 0.5f);
    ImVec2 toggleEnd = ImVec2(togglePos.x + width, togglePos.y + height);

    ImVec4 off4 = FluxPalette::SurfaceElevatedV();
    ImVec4 on4 = FluxPalette::PrimaryV();
    ImVec4 track4;
    track4.x = off4.x + (on4.x - off4.x) * animT;
    track4.y = off4.y + (on4.y - off4.y) * animT;
    track4.z = off4.z + (on4.z - off4.z) * animT;
    track4.w = off4.w + (on4.w - off4.w) * animT;
    ImU32 trackCol = ImGui::ColorConvertFloat4ToU32(track4);
    dl->AddRectFilled(togglePos, toggleEnd, trackCol, radius);

    if (animT > 0.01f) {
        float glowX = togglePos.x + radius + (width - height) * animT;
        float glowY = togglePos.y + radius;
        dl->AddCircleFilled(ImVec2(glowX, glowY), radius - 2.0f,
                           FluxPalette::Primary((int)(60 * animT)), 32);
    }
    float knobX = togglePos.x + radius + (width - height) * animT;
    float knobY = togglePos.y + radius;
    float knobR = radius - 4.0f;
    dl->AddCircleFilled(ImVec2(knobX, knobY + 1), knobR + 1.0f, IM_COL32(0, 0, 0, 30), 32);
    dl->AddCircleFilled(ImVec2(knobX, knobY), knobR, FluxPalette::TextPrimary(), 32);
    dl->AddText(ImVec2(bb.Min.x + 15.0f, bb.Min.y + (size.y - textSize.y) * 0.5f),
                FluxPalette::TextPrimary(), label);
    return pressed;
}

// ═══════════════════════════════════════════════════════════════
// CONTENT AREA
// ═══════════════════════════════════════════════════════════════

static void DrawContentArea(float winW, float winH) {
    bool need_save = false;
    float barH = g_menu.tabBarHeight;
    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImVec2 winPos = ImGui::GetWindowPos();

    // Header strip below tab bar
    dl->AddRectFilled(ImVec2(winPos.x, winPos.y + barH),
                      ImVec2(winPos.x + winW, winPos.y + barH + 50), FluxPalette::SurfaceCard());
    dl->AddRectFilled(ImVec2(winPos.x, winPos.y + barH + 49),
                      ImVec2(winPos.x + winW, winPos.y + barH + 50), FluxPalette::Primary(100));

    const char* tabTitles[] = { "ESP Settings", "Auto Play", "Auto Queue", "Stats" };

    dl->AddRectFilled(ImVec2(winPos.x + 15, winPos.y + barH + 14),
                      ImVec2(winPos.x + 19, winPos.y + barH + 36), FluxPalette::Primary(), 2.0f);
    ImGui::SetCursorPos(ImVec2(28, barH + 12));
    ImGui::SetWindowFontScale(1.2f);
    ImGui::TextColored(FluxPalette::TextPrimaryV(), "%s", tabTitles[g_menu.currentTab]);
    ImGui::SetWindowFontScale(1.0f);
    ImGui::SetCursorPos(ImVec2(28, barH + 36));
    ImGui::TextColored(FluxPalette::TextMutedV(), "Configure your settings below");

    // Theme switcher
    ImGui::SetCursorPos(ImVec2(winW - 180, barH + 10));
    ImGui::SetWindowFontScale(0.85f);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 6));
    ImGui::SetNextItemWidth(150.0f);
    if (ImGui::Combo("##theme", &current_theme, "Inferno\0Dark\0Light\0Classic\0")) {}
    ImGui::PopStyleVar(2);
    ImGui::SetWindowFontScale(1.0f);

    // Content child area
    float contentOffset = 0.0f;
    if (g_menu.tabTransition < 1.0f && g_menu.tabTransition > 0.001f) {
        contentOffset = (1.0f - EaseOutQuart(g_menu.tabTransition)) * 40.0f;
    }

    ImGui::SetCursorPos(ImVec2(15, barH + 65));
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0, 0, 0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_ScrollbarSize, 4.0f);
    float cx = ImGui::GetCursorPosX() + contentOffset;
    ImGui::SetCursorPosX(cx);
    ImGui::BeginChild("##ContentArea", ImVec2(winW - 30, winH - barH - 80), false,
                      ImGuiWindowFlags_NoScrollbar);

    // Tab content (simplified — show toggle switches for demo)
    switch (g_menu.currentTab) {
        case 0: {
            ImGui::Dummy(ImVec2(0, 15));
            static bool b1 = true;
            ToggleSwitch("Draw Prediction Lines", &b1);
            ImGui::Dummy(ImVec2(0, 4));
            static bool b2 = false;
            ToggleSwitch("Draw Pockets", &b2);
            ImGui::Dummy(ImVec2(0, 4));
            static bool b3 = true;
            ToggleSwitch("Draw Shot State", &b3);
            break;
        }
        case 1: {
            ImGui::Dummy(ImVec2(0, 15));
            static bool b4 = false;
            ToggleSwitch("Enable Auto Play", &b4);
            ImGui::Dummy(ImVec2(0, 25));
            ImGui::TextColored(FluxPalette::TextMutedV(), "Auto play will automatically");
            ImGui::TextColored(FluxPalette::TextMutedV(), "aim and shoot for you");
            break;
        }
        case 2: {
            ImGui::Dummy(ImVec2(0, 15));
            static bool b5 = false;
            ToggleSwitch("Enable Auto Queue", &b5);
            ImGui::Dummy(ImVec2(0, 25));
            ImGui::TextColored(FluxPalette::TextSecondaryV(), "Mode");
            ImGui::Dummy(ImVec2(0, 8));
            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
            static int mode = 0;
            ImGui::Combo("##mode", &mode, "Last Selected\0Smart\0");
            ImGui::Dummy(ImVec2(0, 30));
            ImGui::TextColored(FluxPalette::TextMutedV(), "You will be auto queued to");
            ImGui::TextColored(FluxPalette::TextMutedV(), "the last game mode you played");
            break;
        }
        case 3: {
            ImGui::Dummy(ImVec2(0, 15));
            ImGui::TextColored(FluxPalette::TextSecondaryV(), "Game Statistics");
            ImGui::Dummy(ImVec2(0, 12));
            static int gamesPlayed = 42, wins = 28, losses = 14, winStreak = 5, bestWinStreak = 12;
            struct StatRow { const char* label; int* value; };
            StatRow stats[] = {
                {"Games Played", &gamesPlayed}, {"Wins", &wins}, {"Losses", &losses},
                {"Win Streak", &winStreak}, {"Best Streak", &bestWinStreak},
            };
            for (int i = 0; i < 5; i++) {
                ImVec2 rowMin = ImGui::GetCursorScreenPos();
                ImVec2 rowMax(rowMin.x + ImGui::GetContentRegionAvail().x, rowMin.y + 40);
                dl->AddRectFilled(rowMin, rowMax, FluxPalette::SurfaceCard((int)((0.03f * (i % 2 == 0 ? 1 : 0)) * 255)));
                ImGui::Dummy(ImVec2(0, 12));
                ImGui::SetCursorPosX(10);
                ImGui::TextColored(FluxPalette::TextMutedV(), "%s", stats[i].label);
                ImGui::SameLine(ImGui::GetContentRegionAvail().x - 50);
                ImGui::TextColored(FluxPalette::PrimaryV(), "%d", *stats[i].value);
                ImGui::Dummy(ImVec2(0, 4));
            }
            ImGui::Dummy(ImVec2(0, 20));
            ImGui::SetCursorPosX(10);
            if (ImGui::Button("Reset Stats", ImVec2(ImGui::GetContentRegionAvail().x - 20, 44))) {}
            break;
        }
    }

    ImGui::EndChild();
    ImGui::PopStyleVar();
    ImGui::PopStyleColor();
}

// ═══════════════════════════════════════════════════════════════
// PARTICLES
// ═══════════════════════════════════════════════════════════════

static const int MAX_PARTICLES = 30;
struct Particle { ImVec2 pos, vel; float size, alpha, speed; };
static Particle g_particles[MAX_PARTICLES];
static bool g_particles_init = false;

static void InitParticles() {
    for (int i = 0; i < MAX_PARTICLES; i++) {
        g_particles[i].pos = ImVec2((float)(rand() % 800), (float)(rand() % 580));
        g_particles[i].vel = ImVec2((float)(rand() % 20 - 10) * 0.5f,
                                    -(float)(rand() % 30 + 10) * 0.3f);
        g_particles[i].size = (float)(rand() % 6 + 2);
        g_particles[i].alpha = (float)(rand() % 60 + 20) / 255.0f;
        g_particles[i].speed = (float)(rand() % 20 + 10) * 0.1f;
    }
    g_particles_init = true;
}

static void DrawParticles(ImDrawList* dl, float winX, float winY,
                          float winW, float winH, float alpha) {
    if (!g_particles_init) InitParticles();
    for (int i = 0; i < MAX_PARTICLES; i++) {
        auto& p = g_particles[i];
        p.pos.x += p.vel.x * ImGui::GetIO().DeltaTime * p.speed;
        p.pos.y += p.vel.y * ImGui::GetIO().DeltaTime * p.speed;
        p.vel.x += (float)(sinf(ImGui::GetTime() * 0.5f + i) * 0.1f) * ImGui::GetIO().DeltaTime;
        if (p.pos.y < -20) { p.pos.y = winH + 20; p.pos.x = (float)(rand() % (int)winW); }
        if (p.pos.x < -20) p.pos.x = winW + 20;
        if (p.pos.x > winW + 20) p.pos.x = -20;
        float fade = p.alpha * alpha;
        if (fade <= 0.0f) continue;
        ImU32 col = FluxPalette::Primary((int)(fade * 255));
        dl->AddCircleFilled(ImVec2(winX + p.pos.x, winY + p.pos.y), p.size, col, 16);
    }
}

// ═══════════════════════════════════════════════════════════════
// FLOATING BUTTON
// ═══════════════════════════════════════════════════════════════

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

    ImGui::SetNextWindowPos(buttonPos, ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(totalWidth, totalHeight), ImGuiCond_Always);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

    if (ImGui::Begin("##FloatBtn", nullptr,
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBackground |
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings))
    {
        ImDrawList* dl = ImGui::GetWindowDrawList();
        ImVec2 center = ImVec2(buttonPos.x + buttonRadius + 2, buttonPos.y + buttonRadius + 2);

        ImGui::SetCursorPos(ImVec2(0, 0));
        ImGui::InvisibleButton("##FloatBtnHit", ImVec2(totalWidth, totalHeight));
        isHovered = ImGui::IsItemHovered();

        float targetHover = isHovered ? 1.0f : 0.0f;
        hoverAnim += (targetHover - hoverAnim) * io.DeltaTime * 10.0f;

        float currentRadius = buttonRadius + hoverAnim * 4.0f;
        float pulse = sinf(pulseTime * 2.0f) * 0.04f + 1.0f;

        int glowAlpha = (int)(80 + 60 * hoverAnim + 40 * (pulse - 1.0f));
        dl->AddCircle(center, currentRadius * 1.3f * pulse, FluxPalette::Primary(glowAlpha), 48, 4.0f);
        dl->AddCircleFilled(ImVec2(center.x + 2, center.y + 3), currentRadius,
                           IM_COL32(0, 0, 0, 60), 48);
        dl->AddCircleFilled(center, currentRadius, FluxPalette::PrimaryDark(), 48);
        dl->AddCircleFilled(center, currentRadius - 3, FluxPalette::Primary(), 48);
        dl->AddCircleFilled(center, currentRadius - 6, FluxPalette::PrimaryGlow(), 48);
        dl->AddCircle(center, currentRadius, FluxPalette::PrimaryGlow((int)(180 + 75 * hoverAnim)),
                      48, 2.5f);

        float iconSize = 14.0f;
        ImU32 iconColor = FluxPalette::TextPrimary();
        if (g_menu.isOpen) {
            dl->AddLine(ImVec2(center.x - iconSize, center.y - iconSize),
                       ImVec2(center.x + iconSize, center.y + iconSize), iconColor, 3.0f);
            dl->AddLine(ImVec2(center.x + iconSize, center.y - iconSize),
                       ImVec2(center.x - iconSize, center.y + iconSize), iconColor, 3.0f);
        } else {
            float barW = 16.0f, barH = 2.5f, gap = 5.5f, r = 2.0f;
            dl->AddRectFilled(ImVec2(center.x - barW, center.y - gap - barH),
                             ImVec2(center.x + barW, center.y - gap), iconColor, r);
            dl->AddRectFilled(ImVec2(center.x - barW, center.y - barH * 0.5f),
                             ImVec2(center.x + barW, center.y + barH * 0.5f), iconColor, r);
            dl->AddRectFilled(ImVec2(center.x - barW, center.y + gap),
                             ImVec2(center.x + barW, center.y + gap + barH), iconColor, r);
        }

        float textX = buttonPos.x + buttonSize + 12.0f;
        float textY = buttonPos.y + (totalHeight - 38.0f) * 0.5f;
        dl->AddText(ImVec2(textX, textY), FluxPalette::TextPrimary(), "FLUX");
        dl->AddText(ImVec2(textX, textY + 24.0f), FluxPalette::TextMuted(), "Engine");

        if (ImGui::IsItemActive() && ImGui::IsMouseDragging(0)) {
            isDragging = true;
            buttonPos.x += io.MouseDelta.x;
            buttonPos.y += io.MouseDelta.y;
            buttonPos.x = ImClamp(buttonPos.x, 0.0f, (float)Width - totalWidth);
            buttonPos.y = ImClamp(buttonPos.y, 0.0f, (float)Height - totalHeight);
        }

        if (ImGui::IsItemClicked(0) && !isDragging) {
            g_menu.isOpen = !g_menu.isOpen;
        }
        if (!ImGui::IsItemActive()) isDragging = false;
    }
    ImGui::End();
    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor();
}

// ═══════════════════════════════════════════════════════════════
// MENU
// ═══════════════════════════════════════════════════════════════

static void DrawMenu(ImGuiIO& io) {
    float targetAlpha = g_menu.isOpen ? 1.0f : 0.0f;
    if (g_menu.isOpen) {
        g_menu.menuAlpha += (1.0f - g_menu.menuAlpha) * io.DeltaTime * 12.0f;
    } else {
        g_menu.menuAlpha = 0.0f;
    }

    if (g_menu.tabTransition > 0.001f && g_menu.tabTransition < 1.0f) {
        g_menu.tabTransition += io.DeltaTime * 6.0f;
        if (g_menu.tabTransition >= 1.0f) g_menu.tabTransition = 1.0f;
    } else if (g_menu.tabTransition < 0.001f) {
        g_menu.tabTransition = 1.0f;
    }

    if (g_menu.menuAlpha > 0.01f) {
        float winW = 920.0f, winH = 660.0f;
        ImGui::SetNextWindowSize(ImVec2(winW, winH), ImGuiCond_Always);
        ImGui::SetNextWindowPos(ImVec2(Width / 2.0f, Height / 2.0f),
                                ImGuiCond_FirstUseEver, ImVec2(0.5f, 0.5f));

        ImGui::PushStyleColor(ImGuiCol_WindowBg, FluxPalette::SurfaceCardV(g_menu.menuAlpha * 0.92f));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 16.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, g_menu.menuAlpha);

        ImGuiWindowFlags winFlags = ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar |
            ImGuiWindowFlags_NoScrollWithMouse;

        if (ImGui::Begin("##MainMenu", &g_menu.isOpen, winFlags)) {
            ImDrawList* dl = ImGui::GetWindowDrawList();
            ImVec2 winPos = ImGui::GetWindowPos();

            DrawParticles(dl, winPos.x, winPos.y, winW, winH, g_menu.menuAlpha * 0.6f);
            DrawGlassPanel(dl, winPos, ImVec2(winPos.x + winW, winPos.y + winH),
                          FluxPalette::SurfaceBg(), 16.0f, 0.08f);
            dl->AddRect(winPos, ImVec2(winPos.x + winW, winPos.y + winH),
                       FluxPalette::Primary((int)(150 * g_menu.menuAlpha)), 16.0f, 0, 1.5f);

            DrawTabBar(winW);
            DrawContentArea(winW, winH);
        }
        ImGui::End();
        ImGui::PopStyleVar(4);
        ImGui::PopStyleColor();
    }
}

// ═══════════════════════════════════════════════════════════════
// MAIN
// ═══════════════════════════════════════════════════════════════

static void glfw_error_callback(int error, const char* description) {
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

int main() {
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit()) return 1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(Width, Height, "Inferno UI Test", nullptr, nullptr);
    if (!window) { glfwTerminate(); return 1; }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // vsync

    // Setup ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_IsTouchScreen;
    io.Fonts->AddFontDefault();

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        glfwGetWindowSize(window, &Width, &Height);

        // Draw the UI
        DrawFloatingButton(io);
        DrawMenu(io);

        // Status overlay (simple version)
        ImDrawList* bg_dl = ImGui::GetBackgroundDrawList();
        bg_dl->AddText(ImVec2(16, Height - 40), FluxPalette::TextMuted(),
                       "Click the floating button (top-left) to toggle menu");

        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.02f, 0.02f, 0.02f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
