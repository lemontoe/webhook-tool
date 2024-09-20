#pragma once
#include <string>
#include <vector>
#include "imgui/imgui.h"
#include "Theme/Theme.h"
#include "imgui/imgui_internal.h"
//#include "main.cpp"

extern bool reset_size;
extern bool ncmenu;
extern int tabs;
extern std::vector<std::string> log_buffer;

// default colors 
ImVec4 buttonColor = ImVec4(69.0f / 255.0f, 38.0f / 255.0f, 73.0f / 255.0f, 1.0f); // (69, 38, 73)
ImVec4 checkboxColor = ImVec4(91.0f / 255.0f, 68.0f / 255.0f, 93.0f / 255.0f, 1.0f); // (91, 68, 93)
ImVec4 frameColor = ImVec4(48.0f / 255.0f, 27.0f / 255.0f, 51.0f / 255.0f, 1.0f); // (48, 27, 51)
ImVec4 menuColor = ImVec4(69.0f / 255.0f, 38.0f / 255.0f, 73.0f / 255.0f, 1.0f); // (69, 38, 73)
ImVec4 textColor = ImVec4(255.0f / 255.0f, 255.0f / 255.0f, 255.0f / 255.0f, 1.0f); // (255, 255, 255)

extern bool spammer_active;
extern int spam_interval;
extern int spam_count;
extern int messages_sent;
extern bool stop_requested;

// multiple definitions go brrrrrrrrrrrrr

char message[500] = "";
char webhook[500] = "";

bool show_log_window = false;
bool show_embed_builder = false;

char embed_title[256] = "";
char embed_description[1024] = "";
char embed_footer[256] = "";
char embed_footer_image_url[512] = "";
char embed_image_url[512] = "";
char embed_thumbnail_url[512] = "";
char embed_field_name[256] = "";
char embed_field_value[512] = "";
bool inline_field = false;
struct EmbedField {
    std::string name;
    std::string value;
    bool inline_field;
};
std::vector<EmbedField> embed_fields;
bool include_timestamp = false;
int embed_color_decimal = 16777215;

void SendText(std::string text, std::string webhook);
void SendEmbed(const std::string& embed_json, const std::string& webhook);
std::string GetISO8601Timestamp();
void AddLog(const std::string& message);
void ExportLogToFile(const std::string& filename);

ImVec4 AdjustColorBrightness(const ImVec4& color, float factor) {
    return ImVec4(
        ImClamp(color.x * factor, 0.0f, 1.0f),
        ImClamp(color.y * factor, 0.0f, 1.0f),
        ImClamp(color.z * factor, 0.0f, 1.0f),
        color.w
    );
}


void RenderMenu(Theme& theme)
{
    if (ncmenu) {
        if (reset_size) {
            ImGui::SetNextWindowSize(ImVec2(400, 350));
            reset_size = false;
        }
        ImGui::SetNextWindowBgAlpha(1.0f);
        ImGui::PushFont(theme.font);
        ImGui::Begin("Discrotch Webnook Tool", &ncmenu);

        if (ImGui::BeginTabBar("##Tabs", ImGuiTabBarFlags_None)) {
            if (ImGui::BeginTabItem("Webhook")) {

                ImGui::Dummy(ImVec2(0, 10));
                ImGui::InputText("Webhook URL", webhook, IM_ARRAYSIZE(webhook));
                ImGui::Dummy(ImVec2(0, 10));
                ImGui::Separator();
                ImGui::Dummy(ImVec2(0, 10));
                ImGui::InputText("Classic Message", message, IM_ARRAYSIZE(message));
                ImGui::Dummy(ImVec2(0, 10));
                ImGui::Checkbox("Message Spammer", &spammer_active);
                if (spammer_active) {
                    ImGui::InputInt("Interval (ms)", &spam_interval);
                    ImGui::InputInt("Count", &spam_count);
                    ImGui::Spacing();
                    ImGui::Separator();
                    ImGui::Spacing();
                    if (ImGui::Button("  Spam Message(s)  ")) {
                        messages_sent = 0;
                        stop_requested = false;
                        while (messages_sent < spam_count && !stop_requested) {
                            SendText(message, webhook);
                            messages_sent++;
                            std::this_thread::sleep_for(std::chrono::milliseconds(spam_interval)); // shitty
                        }
                    }
                    if (ImGui::IsItemHovered()) {
                        ImGui::SetTooltip("program will freeze until finished."); // fix that shit already
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("  Stop Message Spam  ")) {
                        stop_requested = true;
                    }
                }
                else {
                    ImGui::Spacing();
                    ImGui::Separator();
                    ImGui::Spacing();
                    if (ImGui::Button("  Send Message  ")) {
                        SendText(message, webhook);
                    }
                }

                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Embed Builder")) {

                ImGui::Dummy(ImVec2(0, 10));
                ImGui::Checkbox("Master Switch", &show_embed_builder);
                if (show_embed_builder) {
                    ImGui::Dummy(ImVec2(0, 10));
                    ImGui::Separator();
                    ImGui::Dummy(ImVec2(0, 10));

                    ImGui::Text("Main Embed");
                    ImGui::Spacing();
                    ImGui::InputText("Title", embed_title, IM_ARRAYSIZE(embed_title));
                    ImGui::InputTextMultiline("Description", embed_description, IM_ARRAYSIZE(embed_description));
                    ImGui::InputInt("Color (Decimal)", &embed_color_decimal);
                    ImGui::Dummy(ImVec2(0, 10));
                    ImGui::InputText("Big Image URL", embed_image_url, IM_ARRAYSIZE(embed_image_url));
                    ImGui::InputText("Thumbnail URL", embed_thumbnail_url, IM_ARRAYSIZE(embed_thumbnail_url));
                    ImGui::Dummy(ImVec2(0, 10));
                    ImGui::InputText("Footer", embed_footer, IM_ARRAYSIZE(embed_footer));
                    ImGui::InputText("Footer Image URL", embed_footer_image_url, IM_ARRAYSIZE(embed_footer_image_url));
                    ImGui::Checkbox("Include Timestamp", &include_timestamp);

                    ImGui::Dummy(ImVec2(0, 10));
                    ImGui::Text("Embed Fields");
                    ImGui::Spacing();
                    ImGui::InputText("Field Name", embed_field_name, IM_ARRAYSIZE(embed_field_name));
                    ImGui::InputText("Field Value", embed_field_value, IM_ARRAYSIZE(embed_field_value));
                    if (ImGui::Button("Add Field")) {
                        embed_fields.push_back({ embed_field_name, embed_field_value, inline_field }); // inline_field 
                        embed_field_name[0] = '\0';
                        embed_field_value[0] = '\0';
                    }
                    ImGui::SameLine();
                    ImGui::Checkbox("Inline Field", &inline_field);
                    ImGui::SameLine();
                    ImGui::Text(" (?)");
                    if (ImGui::IsItemHovered()) { 
                        ImGui::SetTooltip("code is fucked up so can't stack inline fields. (sorry)");
                    }

                    ImGui::Dummy(ImVec2(0, 10));

                    ImGui::Text("Fields :");
                    for (int i = 0; i < embed_fields.size(); ++i) {
                        const auto& field = embed_fields[i];
                        ImGui::PushID(i);
                        ImGui::BulletText("%s: %s (Inline: %s)", field.name.c_str(), field.value.c_str(), field.inline_field ? "Yes" : "No"); // inline_field check #1
                        if (ImGui::Button("  Remove Field  ")) {
                            embed_fields.erase(embed_fields.begin() + i);
                            --i;
                        }
                        ImGui::PopID();
                    }

                    ImGui::Spacing();
                    ImGui::Separator();
                    ImGui::Spacing();

                    if (ImGui::Button("  Send Embed  ")) {
                        std::string embed_json = "{";
                        embed_json += "\"title\":\"" + std::string(embed_title) + "\",";
                        embed_json += "\"description\":\"" + std::string(embed_description) + "\",";
                        embed_json += "\"color\":" + std::to_string(embed_color_decimal);

                        if (embed_footer[0] != '\0') {
                            embed_json += ",\"footer\":{";
                            embed_json += "\"text\":\"" + std::string(embed_footer) + "\"";
                            if (embed_footer_image_url[0] != '\0') {
                                embed_json += ",\"icon_url\":\"" + std::string(embed_footer_image_url) + "\"";
                            }
                            embed_json += "}";
                        }

                        if (embed_image_url[0] != '\0') {
                            embed_json += ",\"image\":{\"url\":\"" + std::string(embed_image_url) + "\"}";
                        }

                        if (embed_thumbnail_url[0] != '\0') {
                            embed_json += ",\"thumbnail\":{\"url\":\"" + std::string(embed_thumbnail_url) + "\"}";
                        }

                        if (include_timestamp) {
                            embed_json += ",\"timestamp\":\"" + GetISO8601Timestamp() + "\"";
                        }

                        if (!embed_fields.empty()) {
                            embed_json += ",\"fields\":[";
                            for (const auto& field : embed_fields) {
                                if (!field.name.empty() && !field.value.empty()) {
                                    embed_json += "{\"name\":\"" + field.name + "\", \"value\":\"" + field.value + "\", \"inline\":" + (inline_field ? "true" : "false") + "},"; // inline_field check #2
                                }
                            }
                            embed_json.pop_back(); // remove last comma
                            embed_json += "]";
                        }

                        embed_json += "}";

                        SendEmbed(embed_json, webhook);
                    }
                }

                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Miscellaneous")) {

                ImGui::Dummy(ImVec2(0, 10));
                ImGui::Checkbox("Show Logs", &show_log_window);
                if (show_log_window) {
                    ImGui::Dummy(ImVec2(0, 10));
                    ImGui::Separator();
                    ImGui::Dummy(ImVec2(0, 10));
                    ImGui::Text("Webhook Logs :");

                    std::string log_content;
                    for (const auto& log : log_buffer) {
                        log_content += log + "\n";
                    }

                    ImGui::BeginChild("LogWindow", ImVec2(0, 170), true, ImGuiWindowFlags_HorizontalScrollbar);
                    ImGui::InputTextMultiline("##log", &log_content[0], log_content.size() + 1, ImVec2(0, 150), ImGuiInputTextFlags_ReadOnly);
                    ImGui::EndChild();
                    ImGui::Spacing();
                    if (ImGui::Button("  Clear Log  ")) {
                        log_buffer.clear();
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("  Export Log  ")) {
                        ExportLogToFile("webhook_log.txt");  
                    }
                }

                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Settings")) {

                ImGui::Dummy(ImVec2(0, 10));
                ImGui::Text("Menu Color Customization");
                ImGui::Dummy(ImVec2(0, 10));
                if (ImGui::ColorEdit3("Button Color", (float*)&buttonColor)) {
                    ImGui::GetStyle().Colors[ImGuiCol_Button] = buttonColor;
                    ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered] = AdjustColorBrightness(buttonColor, 1.2f);
                    ImGui::GetStyle().Colors[ImGuiCol_ButtonActive] = AdjustColorBrightness(buttonColor, 1.4f);
                }

                if (ImGui::ColorEdit3("Checkbox Color", (float*)&checkboxColor)) {
                    ImGui::GetStyle().Colors[ImGuiCol_CheckMark] = checkboxColor;
                }
                
                if (ImGui::ColorEdit3("Frame Color", (float*)&frameColor)) {
                    ImGui::GetStyle().Colors[ImGuiCol_FrameBg] = frameColor;
                    ImGui::GetStyle().Colors[ImGuiCol_FrameBgHovered] = AdjustColorBrightness(frameColor, 1.2f);
                    ImGui::GetStyle().Colors[ImGuiCol_FrameBgActive] = AdjustColorBrightness(frameColor, 1.4f);
                }

                if (ImGui::ColorEdit3("Menu Color", (float*)&menuColor)) {
                    ImGui::GetStyle().Colors[ImGuiCol_Tab] = menuColor;
                    ImGui::GetStyle().Colors[ImGuiCol_TitleBgActive] = menuColor;
                    ImGui::GetStyle().Colors[ImGuiCol_TabHovered] = AdjustColorBrightness(menuColor, 1.2f);
                    ImGui::GetStyle().Colors[ImGuiCol_TabActive] = AdjustColorBrightness(menuColor, 1.4f);
                }
                
                if (ImGui::ColorEdit3("Text Color", (float*)&textColor)) {
                    ImGui::GetStyle().Colors[ImGuiCol_Text] = textColor;
                }

                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }

        ImGui::PopFont();
        ImGui::End();
    }
    else {
        exit(0);
    }
}

void ApplyDefaultColors() { // applys default menu colors
    ImGuiStyle& style = ImGui::GetStyle();
    
    style.Colors[ImGuiCol_Button] = buttonColor;
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(buttonColor.x * 0.8f, buttonColor.y * 0.8f, buttonColor.z * 0.8f, buttonColor.w);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(buttonColor.x * 0.6f, buttonColor.y * 0.6f, buttonColor.z * 0.6f, buttonColor.w);

    style.Colors[ImGuiCol_CheckMark] = checkboxColor;
    style.Colors[ImGuiCol_Text] = textColor;

    style.Colors[ImGuiCol_FrameBg] = frameColor;
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(frameColor.x * 0.8f, frameColor.y * 0.8f, frameColor.z * 0.8f, frameColor.w);
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(frameColor.x * 0.6f, frameColor.y * 0.6f, frameColor.z * 0.6f, frameColor.w);

    style.Colors[ImGuiCol_Tab] = menuColor;
    style.Colors[ImGuiCol_TitleBgActive] = menuColor;
    style.Colors[ImGuiCol_TabHovered] = ImVec4(menuColor.x * 0.8f, menuColor.y * 0.8f, menuColor.z * 0.8f, menuColor.w);
    style.Colors[ImGuiCol_TabActive] = ImVec4(menuColor.x * 0.6f, menuColor.y * 0.6f, menuColor.z * 0.6f, menuColor.w);
}
