#pragma once

#include "../../ImGui/imgui_settings.h"

#include <chrono>
#include <cctype>
#include <cstdio>
#include <ctime>
#include <string>
#include <cmath>

namespace font {
    extern ImFont* inter_semibold;
}

extern std::chrono::steady_clock::time_point appStartTime;

namespace floating_info {

// --- MATCH TRACKING VARIABLES ---
inline int totalMatches = 0;
inline bool wasInMatch = false;

inline std::string FormatTimeDuration(std::chrono::steady_clock::duration duration) {
    const auto hours = std::chrono::duration_cast<std::chrono::hours>(duration).count();
    const auto minutes = std::chrono::duration_cast<std::chrono::minutes>(duration % std::chrono::hours(1)).count();
    const auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration % std::chrono::minutes(1)).count();
    char buffer[32] = {};
    std::snprintf(buffer, sizeof(buffer), "%02d:%02d:%02d", (int)hours, (int)minutes, (int)seconds);
    return std::string(buffer);
}

inline void Render(ImDrawList* draw, float screenWidth, float screenHeight) {
    IM_UNUSED(screenWidth);
    if (draw == nullptr || Config.ExtraMenu.ClearDisplay) {
        return;
    }

    if (!font::inter_semibold) {
        return;
    }

    // --- MATCH TRACKING LOGIC ---
    bool isInMatch = false;
    auto* get_LocalPawn = GamePlay::get_LocalPawn();
    if (Tools::IsPtrValid(get_LocalPawn)) {
        isInMatch = true; 
    }

    if (isInMatch && !wasInMatch) {
        totalMatches++;
        wasInMatch = true;
    } else if (!isInMatch && wasInMatch) {
        wasInMatch = false;
    }

    // --- TIME & DATE CALCULATION ---
    const auto now = std::chrono::system_clock::now();
    std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
    std::tm* localTime = std::localtime(&currentTime);

    char dateStr[64] = {};
    char timeStr[64] = {};
    std::strftime(dateStr, sizeof(dateStr), "%b %d, %Y", localTime); 
    std::strftime(timeStr, sizeof(timeStr), "%I:%M:%S %p", localTime); 

    for (char* p = dateStr; *p; ++p) *p = (char)std::toupper((unsigned char)*p);
    for (char* p = timeStr; *p; ++p) *p = (char)std::toupper((unsigned char)*p);

    auto currentDuration = std::chrono::steady_clock::now() - appStartTime;
    std::string playTime = FormatTimeDuration(currentDuration);

    char totalMatchesStr[32] = {};
    std::snprintf(totalMatchesStr, sizeof(totalMatchesStr), "%d", totalMatches);

    // --- MATHEMATICAL PULSE FOR LUXURY EFFECT ---
    float pulseTime = (float)std::clock() / CLOCKS_PER_SEC;
    int pulseAlpha = (int)(160.0f + std::sin(pulseTime * 3.8f) * 70.0f); // smoother luxury pulse

    // --- GOLD LUXURY PALETTE ---
    const ImColor luxuryGold(255, 255, 255, 255);        // Main Gold
    const ImColor deepGold(180, 180, 180, 255);        // Rich Deep Gold
    const ImColor lightGold(235, 235, 235, 255);      // Highlight Gold
    const ImColor mutedGold(150, 150, 150, 255);       // Muted Elegant Gold
    const ImColor pureWhite(255, 255, 255, 255);
    const ImColor panelBg(5, 5, 5, 245);            // Warm Dark Ebony
    const ImColor borderGlow(0, 0, 0, 35);        // Soft Gold Glow
    const ImColor premiumGold(255, 255, 255, pulseAlpha); // Pulsing Luxury Gold

    // --- PREMIUM BOX RENDER ---
    const float boxW = 245.0f * c::scale;
    const float boxH = 152.0f * c::scale; 
    const float padding = 24.0f * c::scale;
    
    const float boxX = padding;
    const float boxY = screenHeight - padding - boxH;
    
    const float lineSpacing = 19.0f * c::scale;
    const float textStartX = boxX + 18.0f * c::scale;
    const float textEndX = boxX + boxW - 18.0f * c::scale; 
    const float startY = boxY + 16.0f * c::scale;

    // Shadow / Ambient Glow
    draw->AddRectFilled(ImVec2(boxX - 3.0f, boxY - 3.0f), ImVec2(boxX + boxW + 3.0f, boxY + boxH + 3.0f), borderGlow, 12.0f * c::scale);
    
    // Main Background
    draw->AddRectFilled(ImVec2(boxX, boxY), ImVec2(boxX + boxW, boxY + boxH), panelBg, 10.0f * c::scale);
    
    // Elegant Gold Border
    draw->AddRect(ImVec2(boxX, boxY), ImVec2(boxX + boxW, boxY + boxH), ImColor(255, 255, 255, 65), 10.0f * c::scale, 0, 1.6f * c::scale);

    // --- TEXT RENDERING ---
    auto drawTextWithSpacing = [&](const char* text, ImVec2 pos, ImColor color, float customTracking = 1.1f, bool rightAlign = false) {
        float xOffset = 0.0f;
        const float charSpacing = customTracking * c::scale;
        float fontSize = 11.5f * c::scale;

        if (rightAlign) {
            float totalWidth = 0.0f;
            for (int i = 0; text[i] != '\0'; ++i) {
                char glyph[2] = { text[i], '\0' };
                totalWidth += font::inter_semibold->CalcTextSizeA(fontSize, FLT_MAX, 0.0f, glyph).x + charSpacing;
            }
            pos.x -= totalWidth;
        }

        for (int i = 0; text[i] != '\0'; ++i) {
            char glyph[2] = { text[i], '\0' };
            draw->AddText(font::inter_semibold, fontSize, ImVec2(pos.x + xOffset, pos.y), color, glyph);
            xOffset += font::inter_semibold->CalcTextSizeA(fontSize, FLT_MAX, 0.0f, glyph).x + charSpacing;
        }
    };

    // --- DRAWING THE INTERFACE ---
    
    // 1. Header
    drawTextWithSpacing("Supremacy", ImVec2(textStartX, startY), pureWhite, 1.4f);
    
    // Status Dot + DELUXE
    float dotRadius = 3.0f * c::scale;
    float deluxeTextWidth = font::inter_semibold->CalcTextSizeA(11.5f * c::scale, FLT_MAX, 0.0f, "Novara").x + (1.4f * 6);
    ImVec2 dotPos = ImVec2(textEndX - deluxeTextWidth - (8.0f * c::scale), startY + (6.0f * c::scale));
    
    draw->AddCircleFilled(dotPos, dotRadius, premiumGold);
    draw->AddCircle(dotPos, dotRadius + (1.5f * c::scale), ImColor(255, 255, 255, 50), 0, 1.0f);

    drawTextWithSpacing("Premium", ImVec2(textEndX, startY), luxuryGold, 1.4f, true);
    
    // Divider
    float dividerY = startY + 18.0f * c::scale;
    draw->AddLine(ImVec2(textStartX, dividerY), ImVec2(textEndX, dividerY), ImColor(255, 255, 255, 40), 1.0f);

    float currentY = dividerY + 10.0f * c::scale;

    // 2. Info Rows
    drawTextWithSpacing("Date", ImVec2(textStartX, currentY), mutedGold, 1.0f);
    drawTextWithSpacing(dateStr, ImVec2(textEndX, currentY), pureWhite, 1.1f, true);
    
    currentY += lineSpacing;
    drawTextWithSpacing("Time", ImVec2(textStartX, currentY), mutedGold, 1.0f);
    drawTextWithSpacing(timeStr, ImVec2(textEndX, currentY), pureWhite, 1.1f, true);
    
    currentY += lineSpacing;
    drawTextWithSpacing("Session", ImVec2(textStartX, currentY), mutedGold, 1.0f); 
    drawTextWithSpacing(playTime.c_str(), ImVec2(textEndX, currentY), lightGold, 1.1f, true);

    // Matches
    currentY += lineSpacing;
    drawTextWithSpacing("Matches Played", ImVec2(textStartX, currentY), mutedGold, 1.0f);
    drawTextWithSpacing(totalMatchesStr, ImVec2(textEndX, currentY), pureWhite, 1.1f, true);

    // 3. Footer
    currentY += lineSpacing + 8.0f * c::scale;
    
    draw->AddLine(ImVec2(textStartX, currentY - 3.0f * c::scale), ImVec2(textStartX + 20.0f * c::scale, currentY - 3.0f * c::scale), deepGold, 2.0f);
    drawTextWithSpacing("Powered By One And Only Novara", ImVec2(textStartX, currentY), deepGold, 2.0f);
}

} // namespace floating_info