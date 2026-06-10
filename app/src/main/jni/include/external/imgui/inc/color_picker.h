#pragma once

#include "../src/imgui.h"
#include "custom_theme.h"

// ── Color Swatch Picker ─────────────────────────────────────
// Touch-friendly color picker that shows a preview circle + popup swatch grid.
// Usage:
//     static ImColor myColor = ImColor(255, 107, 0);
//     ColorPicker("##mycolor", &myColor);

static const int SWATCH_ROWS = 3;
static const int SWATCH_COLS = 4;
static ImU32 g_swatchColors[SWATCH_ROWS * SWATCH_COLS] = {
    // Row 1: Reds & Oranges (Inferno accent family)
    IM_COL32(255, 34, 34, 255),    // red
    IM_COL32(255, 68, 0, 255),     // deep orange
    IM_COL32(255, 107, 0, 255),    // primary orange
    IM_COL32(255, 165, 0, 255),    // warm orange
    // Row 2: Neutrals
    IM_COL32(255, 255, 255, 255),  // white
    IM_COL32(200, 200, 200, 255),  // light grey
    IM_COL32(100, 100, 100, 255),  // mid grey
    IM_COL32(30, 30, 30, 255),     // dark grey
    // Row 3: Colors
    IM_COL32(0, 200, 0, 255),      // green
    IM_COL32(0, 150, 255, 255),    // blue
    IM_COL32(255, 200, 0, 255),    // yellow
    IM_COL32(200, 0, 200, 255),    // purple
};

static bool ColorPicker(const char* label, ImColor* colorOut, float previewSize = 32.0f) {
    bool changed = false;
    ImGui::PushID(label);

    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImVec2 cursor = ImGui::GetCursorScreenPos();
    ImU32 currentCol = IM_COL32(
        (int)(colorOut->Value.x * 255),
        (int)(colorOut->Value.y * 255),
        (int)(colorOut->Value.z * 255),
        255
    );

    // Preview circle
    float half = previewSize * 0.5f;
    dl->AddCircleFilled(ImVec2(cursor.x + half, cursor.y + half), half, currentCol, 20);
    dl->AddCircle(ImVec2(cursor.x + half, cursor.y + half), half,
                  IM_COL32(255, 255, 255, 40), 20, 1.5f);
    ImGui::Dummy(ImVec2(previewSize + 4, previewSize + 4));

    ImGui::SameLine();
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 4);
    ImGui::TextUnformatted(label);

    // Open popup on click
    if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0)) {
        ImGui::OpenPopup("##picker_popup");
    }

    if (ImGui::BeginPopup("##picker_popup")) {
        ImGui::Text("Pick Color");
        ImGui::Separator();
        ImGui::Dummy(ImVec2(0, 4));

        ImDrawList* pdl = ImGui::GetWindowDrawList();
        ImVec2 popupPos = ImGui::GetCursorScreenPos();
        float swatchSize = 36.0f;
        float spacing = 6.0f;
        float gridW = SWATCH_COLS * (swatchSize + spacing);

        for (int i = 0; i < SWATCH_ROWS * SWATCH_COLS; i++) {
            int row = i / SWATCH_COLS;
            int col = i % SWATCH_COLS;
            ImVec2 pos(popupPos.x + col * (swatchSize + spacing),
                       popupPos.y + row * (swatchSize + spacing));
            ImVec2 rectEnd(pos.x + swatchSize, pos.y + swatchSize);

            // Swatch rect
            pdl->AddRectFilled(pos, rectEnd, g_swatchColors[i], 6.0f);
            pdl->AddRect(pos, rectEnd, IM_COL32(255, 255, 255, 30), 6.0f, 0, 1.0f);

            // Check if tapped
            bool tapped = false;
            if (ImGui::IsMouseClicked(0)) {
                ImVec2 mouse = ImGui::GetMousePos();
                if (mouse.x >= pos.x && mouse.x <= rectEnd.x &&
                    mouse.y >= pos.y && mouse.y <= rectEnd.y) {
                    tapped = true;
                }
            }

            // Invisible button for hover/hit
            ImGui::SetCursorScreenPos(pos);
            ImGui::InvisibleButton("##swatch", ImVec2(swatchSize, swatchSize));

            if (tapped || (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0))) {
                int r = (g_swatchColors[i] >> IM_COL32_R_SHIFT) & 0xFF;
                int g = (g_swatchColors[i] >> IM_COL32_G_SHIFT) & 0xFF;
                int b = (g_swatchColors[i] >> IM_COL32_B_SHIFT) & 0xFF;
                *colorOut = ImColor(r, g, b);
                ImGui::CloseCurrentPopup();
                changed = true;
            }
        }

        // Advance cursor past grid
        int totalRows = (SWATCH_ROWS * SWATCH_COLS + SWATCH_COLS - 1) / SWATCH_COLS;
        ImGui::SetCursorScreenPos(ImVec2(popupPos.x, popupPos.y + totalRows * (swatchSize + spacing)));
        ImGui::EndPopup();
    }

    ImGui::PopID();
    return changed;
}
