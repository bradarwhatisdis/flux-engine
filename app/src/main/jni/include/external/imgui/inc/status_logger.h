#pragma once

#include "../src/imgui.h"
#include "custom_theme.h"
#include "typography.h"

// ═══════════════════════════════════════════════════════════════
// STATUS OVERLAY — Real-time operation logger
// ═══════════════════════════════════════════════════════════════
// Bottom-left overlay showing 5 most recent status messages
// with auto-fade, color-coded levels, and FIFO eviction.
// ═══════════════════════════════════════════════════════════════

enum class StatusLevel : int {
    Log     = 0,
    Info    = 1,
    Success = 2,
    Warning = 3,
    Error   = 4
};

struct StatusMessage {
    char        text[128];
    StatusLevel level;
    float       opacity;      // 1.0 → 0.0
    float       lifetime;     // seconds remaining
    float       max_lifetime; // for fade calculation
};

class StatusOverlay {
public:
    static constexpr int MAX_MESSAGES = 5;
    static constexpr float DEFAULT_DURATION = 4.0f;
    static constexpr float ERROR_DURATION   = 8.0f;
    static constexpr float FADE_START       = 0.3f;  // fade begins at 30% remaining

    // ── Public API ─────────────────────────────────────────
    static void Log(const char* fmt, ...) {
        va_list args;
        va_start(args, fmt);
        PushFormatted(StatusLevel::Log, DEFAULT_DURATION, fmt, args);
        va_end(args);
    }

    static void Info(const char* fmt, ...) {
        va_list args;
        va_start(args, fmt);
        PushFormatted(StatusLevel::Info, DEFAULT_DURATION, fmt, args);
        va_end(args);
    }

    static void Success(const char* fmt, ...) {
        va_list args;
        va_start(args, fmt);
        PushFormatted(StatusLevel::Success, DEFAULT_DURATION, fmt, args);
        va_end(args);
    }

    static void Warning(const char* fmt, ...) {
        va_list args;
        va_start(args, fmt);
        PushFormatted(StatusLevel::Warning, DEFAULT_DURATION, fmt, args);
        va_end(args);
    }

    static void Error(const char* fmt, ...) {
        va_list args;
        va_start(args, fmt);
        PushFormatted(StatusLevel::Error, ERROR_DURATION, fmt, args);
        va_end(args);
    }

    // ── Update & Draw ─────────────────────────────────────
    static void Tick() {
        const float dt = ImGui::GetIO().DeltaTime;
        bool any_alive = false;

        for (int i = 0; i < MAX_MESSAGES; i++) {
            if (msgs[i].lifetime <= 0.0f) continue;
            any_alive = true;
            msgs[i].lifetime -= dt;

            // Opacity: hold at 1.0 until FADE_START fraction remains
            float fraction = msgs[i].lifetime / msgs[i].max_lifetime;
            if (fraction < FADE_START) {
                msgs[i].opacity = ImMax(0.0f, fraction / FADE_START);
            }

            if (msgs[i].lifetime <= 0.0f) {
                msgs[i].opacity = 0.0f;
                // Compact: shift remaining messages up
                for (int j = i; j < MAX_MESSAGES - 1; j++) {
                    msgs[j] = msgs[j + 1];
                }
                msgs[MAX_MESSAGES - 1].lifetime = 0.0f;
                msgs[MAX_MESSAGES - 1].opacity = 0.0f;
                i--; // re-check this index
            }
        }
        (void)any_alive;
    }

    static void Draw() {
        ImDrawList* dl = ImGui::GetBackgroundDrawList();
        ImVec2 screen = ImGui::GetIO().DisplaySize;

        const float margin   = 16.0f;
        const float padding  = 10.0f;
        const float line_h   = 18.0f;
        const float icon_sz  = 8.0f;

        float y = screen.y - margin;

        // Draw from bottom-up (newest at bottom)
        for (int i = MAX_MESSAGES - 1; i >= 0; i--) {
            if (msgs[i].lifetime <= 0.0f || msgs[i].opacity <= 0.0f) continue;

            float alpha = msgs[i].opacity;
            int a = (int)(alpha * 255);

            // Determine color
            ImU32 icon_color, text_color;
            const char* icon = "";
            switch (msgs[i].level) {
                case StatusLevel::Log:     icon = "";  icon_color = FluxPalette::TextMuted(a);   text_color = FluxPalette::TextSecondary(a); break;
                case StatusLevel::Info:    icon = "";  icon_color = FluxPalette::Primary(a);     text_color = FluxPalette::TextPrimary(a);   break;
                case StatusLevel::Success: icon = "";  icon_color = IM_COL32(0, 200, 80, a);     text_color = FluxPalette::TextPrimary(a);   break;
                case StatusLevel::Warning: icon = "";  icon_color = FluxPalette::Amber(a);       text_color = FluxPalette::TextPrimary(a);   break;
                case StatusLevel::Error:   icon = "";  icon_color = FluxPalette::Red(a);         text_color = FluxPalette::TextPrimary(a);   break;
            }

            // Measure text
            ImVec2 txt_sz = FluxFont::CalcTextBox(msgs[i].text, FluxFont::Small);

            // Draw background pill
            ImVec2 p_min(margin, y - line_h - padding);
            ImVec2 p_max(margin + txt_sz.x + padding * 2 + icon_sz + 6, y + 2);
            ImU32 bg_col = IM_COL32(10, 10, 10, (int)(alpha * 200));
            dl->AddRectFilled(p_min, p_max, bg_col, 6.0f);

            // Draw dot indicator (colored circle)
            ImVec2 dot_center(margin + padding + icon_sz / 2,
                              (p_min.y + p_max.y) * 0.5f);
            dl->AddCircleFilled(dot_center, icon_sz / 2, icon_color);

            // Draw text
            ImVec2 txt_pos(margin + padding + icon_sz + 8,
                           p_min.y + (p_max.y - p_min.y - FluxFont::Small) * 0.5f);
            FluxFont::RenderText(dl, msgs[i].text, txt_pos, text_color, FluxFont::Small);

            y = p_min.y - 4.0f; // gap between messages
        }
    }

private:
    static StatusMessage msgs[MAX_MESSAGES];
    static int msg_count;

    static void PushFormatted(StatusLevel level, float duration,
                              const char* fmt, va_list args) {
        // Shift existing messages up (FIFO evict oldest)
        if (msg_count >= MAX_MESSAGES) {
            for (int i = 0; i < MAX_MESSAGES - 1; i++) {
                msgs[i] = msgs[i + 1];
            }
            msg_count = MAX_MESSAGES - 1;
        }

        StatusMessage& m = msgs[msg_count];
        vsnprintf(m.text, sizeof(m.text), fmt, args);
        m.text[sizeof(m.text) - 1] = '\0';
        m.level = level;
        m.opacity = 1.0f;
        m.lifetime = duration;
        m.max_lifetime = duration;
        msg_count++;
    }
};

// ── Static storage ────────────────────────────────────────────
StatusMessage StatusOverlay::msgs[StatusOverlay::MAX_MESSAGES] = {};
int StatusOverlay::msg_count = 0;
