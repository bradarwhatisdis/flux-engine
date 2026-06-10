#pragma once

#include "../src/imgui.h"

// ═══════════════════════════════════════════════════════════════
// FLUXFONT — Typography system for Inferno UI
// ═══════════════════════════════════════════════════════════════
// Size constants + direct ImDrawList text helpers.
// Uses ImGui's default font — no PushFont/PopFont needed.
// ═══════════════════════════════════════════════════════════════

struct FluxFont {
    // ── Size tiers ───────────────────────────────────────
    static constexpr float H1       = 48.0f;
    static constexpr float H2       = 40.0f;
    static constexpr float Body     = 34.0f;
    static constexpr float Small    = 28.0f;
    static constexpr float Tiny     = 22.0f;

    // ── Draw helpers (ImDrawList::AddText with explicit size) ─
    static void DrawH1(ImDrawList* dl, const char* text, ImVec2 pos, ImU32 color) {
        dl->AddText(ImGui::GetFont(), H1, pos, color, text);
    }
    static void DrawH2(ImDrawList* dl, const char* text, ImVec2 pos, ImU32 color) {
        dl->AddText(ImGui::GetFont(), H2, pos, color, text);
    }
    static void DrawBody(ImDrawList* dl, const char* text, ImVec2 pos, ImU32 color) {
        dl->AddText(ImGui::GetFont(), Body, pos, color, text);
    }
    static void DrawSmall(ImDrawList* dl, const char* text, ImVec2 pos, ImU32 color) {
        dl->AddText(ImGui::GetFont(), Small, pos, color, text);
    }
    static void DrawTiny(ImDrawList* dl, const char* text, ImVec2 pos, ImU32 color) {
        dl->AddText(ImGui::GetFont(), Tiny, pos, color, text);
    }

    static void RenderText(ImDrawList* dl, const char* text, ImVec2 pos,
                           ImU32 color, float size, ImU32 shadow = 0) {
        if (shadow) {
            dl->AddText(ImGui::GetFont(), size * 1.02f,
                        ImVec2(pos.x + 1, pos.y + 1), shadow, text);
        }
        dl->AddText(ImGui::GetFont(), size, pos, color, text);
    }

    // ── Text measurement ─────────────────────────────────
    static ImVec2 CalcTextBox(const char* text, float size) {
        return ImGui::GetFont()->CalcTextSizeA(size, FLT_MAX, 0.0f, text);
    }
};
