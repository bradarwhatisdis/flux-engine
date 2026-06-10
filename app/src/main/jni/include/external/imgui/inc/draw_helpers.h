#pragma once

#include "../src/imgui.h"

#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif
#include "../src/imgui_internal.h"

// ═══════════════════════════════════════════════════════════════
// DRAW HELPERS — Reusable primitives for Inferno UI
// ═══════════════════════════════════════════════════════════════

// ─── Animated Float (tween) ────────────────────────────────────
struct AnimatedFloat {
    float current = 0.0f;
    float target  = 0.0f;
    float speed   = 8.0f;  // lerp speed (higher = faster)

    void Tick(float dt = ImGui::GetIO().DeltaTime) {
        if (fabsf(current - target) < 0.001f) { current = target; return; }
        current += (target - current) * ImMin(1.0f, speed * dt);
    }

    void Set(float t) { target = t; }
    void Instant(float v) { current = target = v; }
    float Value() const { return current; }
    bool IsDone() const { return fabsf(current - target) < 0.001f; }
};

// ─── Draw Rounded Rect with Inner Glow ────────────────────────
inline void DrawGlowRect(ImDrawList* dl, ImVec2 p_min, ImVec2 p_max,
                         ImU32 color, float rounding = 8.0f,
                         float glow_size = 4.0f, ImU32 glow_color = 0) {
    if (!glow_color) glow_color = color & 0x00FFFFFF | 0x30000000; // 19% alpha
    // Outer glow pass (larger, translucent)
    if (glow_size > 0.0f) {
        dl->AddRectFilled(ImVec2(p_min.x - glow_size, p_min.y - glow_size),
                          ImVec2(p_max.x + glow_size, p_max.y + glow_size),
                          glow_color, rounding + glow_size);
    }
    // Core fill
    dl->AddRectFilled(p_min, p_max, color, rounding);
}

// ─── Draw Glass Panel (frosted glass effect simulation) ──────
inline void DrawGlassPanel(ImDrawList* dl, ImVec2 p_min, ImVec2 p_max,
                           ImU32 base_color, float rounding = 12.0f,
                           float border_alpha = 0.15f) {
    // Translucent fill
    dl->AddRectFilled(p_min, p_max, base_color, rounding);
    // Inner highlight (top edge shine)
    ImU32 highlight = IM_COL32(255, 255, 255, 20);
    dl->AddRectFilled(ImVec2(p_min.x + 2, p_min.y + 2),
                      ImVec2(p_max.x - 2, p_min.y + 4),
                      highlight, rounding);
    // Border (subtle)
    ImU32 border_col = IM_COL32(255, 255, 255, (int)(border_alpha * 255));
    dl->AddRect(p_min, p_max, border_col, rounding, ImDrawFlags_RoundCornersAll, 1.0f);
}

// ─── Draw Drop Shadow ──────────────────────────────────────────
inline void DrawDropShadow(ImDrawList* dl, ImVec2 p_min, ImVec2 p_max,
                           float shadow_size = 8.0f, float rounding = 12.0f,
                           ImU32 shadow_color = IM_COL32(0, 0, 0, 100)) {
    dl->AddShadowRect(p_min, p_max, shadow_color, shadow_size, ImVec2(0, 0),
                      rounding, ImDrawFlags_RoundCornersAll);
}

// ─── Gradient Text (left-to-right color blend) ────────────────
inline void AddGradientText(ImDrawList* dl, ImFont* font, float font_size,
                            ImVec2 pos, ImU32 col_left, ImU32 col_right,
                            const char* text) {
    // Fallback: solid color text
    dl->AddText(font, font_size, pos, col_left, text);
    // Optional: overlay gradient rect for richer effect
    ImVec2 text_size = font->CalcTextSizeA(font_size, FLT_MAX, 0.0f, text);
    ImVec2 p1 = pos;
    ImVec2 p2 = ImVec2(pos.x + text_size.x, pos.y + text_size.y);
    dl->AddRectFilledMultiColor(p1, p2, col_left, col_right, col_right, col_left);
}

// ─── Draw Rounded Rect with Full Gradient Fill (4-color) ────
inline void DrawGradientRectEx(ImDrawList* dl, ImVec2 p_min, ImVec2 p_max,
                               ImU32 col_tl, ImU32 col_tr,
                               ImU32 col_br, ImU32 col_bl,
                               float rounding = 0.0f) {
    for (int i = 0; i <= 4; i++) {  // Draw it 4x to make gradient visible
        float t = i / 4.0f;
        // progressively smaller rect
        float shrink = t * 0.0f; // no shrink, just layer
    }
    // Single multi-color rect (will blend 4 corners)
    dl->AddRectFilledMultiColor(p_min, p_max, col_tl, col_tr, col_br, col_bl);
}
