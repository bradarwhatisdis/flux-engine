#pragma once

#include "../src/imgui.h"

// ═══════════════════════════════════════════════════════════════
// INFERNO PALETTE — Black + Bright Orange + Red
// ═══════════════════════════════════════════════════════════════

struct FluxPalette {
    // ── Surface (Background) ─────────────────────────────────
    static ImU32 SurfaceBg(float a = 1.0f)     { return IM_COL32(0, 0, 0, (int)(a * 255)); }
    static ImU32 SurfaceCard(float a = 1.0f)   { return IM_COL32(10, 10, 10, (int)(a * 255)); }
    static ImU32 SurfaceElevated(float a = 1.0f) { return IM_COL32(20, 20, 22, (int)(a * 255)); }

    // ── Accent (Orange) ──────────────────────────────────────
    static ImU32 Primary(float a = 1.0f)       { return IM_COL32(255, 107, 0, (int)(a * 255)); }     // #FF6B00
    static ImU32 PrimaryGlow(float a = 1.0f)   { return IM_COL32(255, 136, 0, (int)(a * 255)); }     // #FF8800
    static ImU32 PrimaryDark(float a = 1.0f)   { return IM_COL32(204, 85, 0, (int)(a * 255)); }      // #CC5500
    static ImU32 Amber(float a = 1.0f)         { return IM_COL32(255, 153, 0, (int)(a * 255)); }     // #FF9900

    // ── Semantic ─────────────────────────────────────────────
    static ImU32 Red(float a = 1.0f)           { return IM_COL32(255, 34, 34, (int)(a * 255)); }     // #FF2222
    static ImU32 RedGlow(float a = 1.0f)       { return IM_COL32(255, 68, 68, (int)(a * 255)); }     // #FF4444
    static ImU32 RedDark(float a = 1.0f)       { return IM_COL32(204, 17, 17, (int)(a * 255)); }     // #CC1111

    // ── Text ─────────────────────────────────────────────────
    static ImU32 TextPrimary(float a = 1.0f)   { return IM_COL32(255, 255, 255, (int)(a * 255)); }
    static ImU32 TextSecondary(float a = 1.0f) { return IM_COL32(204, 204, 204, (int)(a * 255)); }
    static ImU32 TextMuted(float a = 1.0f)     { return IM_COL32(136, 136, 136, (int)(a * 255)); }

    // ── ImVec4 variants (for ImGui style colors) ────────────
    static ImVec4 PrimaryV(float a = 1.0f)       { return ImVec4(1.00f, 0.42f, 0.00f, a); }
    static ImVec4 PrimaryGlowV(float a = 1.0f)   { return ImVec4(1.00f, 0.53f, 0.00f, a); }
    static ImVec4 PrimaryDarkV(float a = 1.0f)   { return ImVec4(0.80f, 0.33f, 0.00f, a); }
    static ImVec4 AmberV(float a = 1.0f)         { return ImVec4(1.00f, 0.60f, 0.00f, a); }
    static ImVec4 SurfaceCardV(float a = 1.0f)   { return ImVec4(0.04f, 0.04f, 0.04f, a); }
    static ImVec4 SurfaceElevatedV(float a = 1.0f) { return ImVec4(0.08f, 0.08f, 0.09f, a); }
    static ImVec4 RedV(float a = 1.0f)           { return ImVec4(1.00f, 0.13f, 0.13f, a); }
    static ImVec4 RedGlowV(float a = 1.0f)       { return ImVec4(1.00f, 0.27f, 0.27f, a); }
    static ImVec4 TextPrimaryV(float a = 1.0f)   { return ImVec4(1.00f, 1.00f, 1.00f, a); }
    static ImVec4 TextSecondaryV(float a = 1.0f) { return ImVec4(0.80f, 0.80f, 0.80f, a); }
    static ImVec4 TextMutedV(float a = 1.0f)     { return ImVec4(0.53f, 0.53f, 0.53f, a); }

    // ── Gradient Helpers ──────────────────────────────────
    static void GradientHPrimary(ImDrawList* dl, ImVec2 p1, ImVec2 p2) {
        dl->AddRectFilledMultiColor(p1, p2, Primary(), PrimaryGlow(), PrimaryGlow(), Primary());
    }
    static void GradientVSurface(ImDrawList* dl, ImVec2 p1, ImVec2 p2) {
        dl->AddRectFilledMultiColor(p1, p2, SurfaceBg(), SurfaceCard(), SurfaceCard(), SurfaceBg());
    }
};

// ═══════════════════════════════════════════════════════════════
// THEME — Inferno Custom Theme
// ═══════════════════════════════════════════════════════════════

static int current_theme = 0;
static const char* themes[] = { "Inferno", "Dark", "Light", "Classic" };

inline void StyleColorsCustom(ImGuiStyle* _style = nullptr) {
    ImGuiStyle& style = _style ? *_style : ImGui::GetStyle();

    // ── Core Colors ──────────────────────────────────────────
    style.Colors[ImGuiCol_WindowBg]          = FluxPalette::SurfaceCardV(0.96f);
    style.Colors[ImGuiCol_ChildBg]           = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_PopupBg]           = FluxPalette::SurfaceElevatedV(0.94f);
    style.Colors[ImGuiCol_Border]            = FluxPalette::PrimaryV(0.20f);
    style.Colors[ImGuiCol_FrameBg]           = FluxPalette::SurfaceElevatedV(0.90f);
    style.Colors[ImGuiCol_FrameBgHovered]    = FluxPalette::PrimaryV(0.15f);
    style.Colors[ImGuiCol_FrameBgActive]     = FluxPalette::PrimaryV(0.25f);
    style.Colors[ImGuiCol_TitleBg]           = FluxPalette::SurfaceBg();
    style.Colors[ImGuiCol_TitleBgActive]     = FluxPalette::PrimaryV(0.35f);
    style.Colors[ImGuiCol_MenuBarBg]         = FluxPalette::SurfaceCard();

    // ── Widget Colors ────────────────────────────────────────
    style.Colors[ImGuiCol_Button]            = FluxPalette::PrimaryV(0.85f);
    style.Colors[ImGuiCol_ButtonHovered]     = FluxPalette::PrimaryGlowV();
    style.Colors[ImGuiCol_ButtonActive]      = FluxPalette::PrimaryDarkV();
    style.Colors[ImGuiCol_Header]            = FluxPalette::PrimaryV(0.70f);
    style.Colors[ImGuiCol_HeaderHovered]     = FluxPalette::PrimaryV(0.85f);
    style.Colors[ImGuiCol_HeaderActive]      = FluxPalette::PrimaryGlowV();
    style.Colors[ImGuiCol_Separator]         = FluxPalette::TextMutedV(0.25f);
    style.Colors[ImGuiCol_SeparatorHovered]  = FluxPalette::PrimaryV(0.50f);
    style.Colors[ImGuiCol_SeparatorActive]   = FluxPalette::PrimaryGlowV();

    // ── Slider ───────────────────────────────────────────────
    style.Colors[ImGuiCol_SliderGrab]        = FluxPalette::PrimaryV(0.90f);
    style.Colors[ImGuiCol_SliderGrabActive]  = FluxPalette::PrimaryGlowV();

    // ── Scrollbar ────────────────────────────────────────────
    style.Colors[ImGuiCol_ScrollbarBg]       = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_ScrollbarGrab]     = FluxPalette::PrimaryV(0.50f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = FluxPalette::PrimaryV(0.70f);
    style.Colors[ImGuiCol_ScrollbarGrabActive]  = FluxPalette::PrimaryGlowV();

    // ── Checkbox / Combo ─────────────────────────────────────
    style.Colors[ImGuiCol_CheckMark]         = FluxPalette::Primary();

    // ── Text ─────────────────────────────────────────────────
    style.Colors[ImGuiCol_Text]              = FluxPalette::TextPrimaryV();
    style.Colors[ImGuiCol_TextDisabled]      = FluxPalette::TextMutedV();

    // ── Resize ───────────────────────────────────────────────
    style.Colors[ImGuiCol_ResizeGrip]        = FluxPalette::PrimaryV(0.40f);
    style.Colors[ImGuiCol_ResizeGripHovered] = FluxPalette::PrimaryV(0.60f);
    style.Colors[ImGuiCol_ResizeGripActive]  = FluxPalette::PrimaryGlowV();

    // ── Tab ──────────────────────────────────────────────────
    style.Colors[ImGuiCol_Tab]               = FluxPalette::SurfaceElevatedV(0.86f);
    style.Colors[ImGuiCol_TabHovered]        = FluxPalette::PrimaryV(0.50f);
    style.Colors[ImGuiCol_TabActive]         = FluxPalette::PrimaryV(0.80f);
    style.Colors[ImGuiCol_TabUnfocused]      = FluxPalette::SurfaceCardV(0.86f);
    style.Colors[ImGuiCol_TabUnfocusedActive]= FluxPalette::SurfaceElevatedV(0.90f);

    // ── Docking ──────────────────────────────────────────────
    style.Colors[ImGuiCol_DockingPreview]    = FluxPalette::PrimaryV(0.50f);
    style.Colors[ImGuiCol_DockingEmptyBg]    = FluxPalette::SurfaceBg();

    // ── Modal ────────────────────────────────────────────────
    style.Colors[ImGuiCol_ModalWindowDimBg]  = ImVec4(0.00f, 0.00f, 0.00f, 0.60f);
}

static void switch_theme(int theme_idx) {
    switch (theme_idx) {
        case 0: StyleColorsCustom(); break;
        case 1: ImGui::StyleColorsDark(); break;
        case 2: ImGui::StyleColorsLight(); break;
        case 3: ImGui::StyleColorsClassic(); break;
    }

    ImGuiStyle& style = ImGui::GetStyle();
    style.TouchExtraPadding = ImVec2(10.0f, 10.0f);
    style.ItemSpacing = ImVec2(12, 8);
    style.ItemInnerSpacing = ImVec2(10, 6);
    style.FramePadding = ImVec2(12, 6);
    style.ScrollbarSize = 6.0f;
    style.WindowRounding = 12.0f;
    style.ChildRounding = 8.0f;
    style.FrameRounding = 8.0f;
    style.PopupRounding = 10.0f;
    style.ScrollbarRounding = 4.0f;
    style.GrabRounding = 6.0f;
    style.TabRounding = 6.0f;
}
