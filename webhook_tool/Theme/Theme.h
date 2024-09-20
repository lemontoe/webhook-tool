#pragma once

class Theme {
public:
    ImFont *font, *font1, *font2, *font3, *font4, *fontsp, *arial;
    void Register(ImGuiIO& io) {

        ImFontConfig icons_config; icons_config.MergeMode = true; icons_config.PixelSnapH = true;
        icons_config.FontDataOwnedByAtlas = false;

        ImFontConfig CustomFont;
        CustomFont.FontDataOwnedByAtlas = false;

        ImFontConfig font_cfg;
        font_cfg.FontDataOwnedByAtlas = false;
    }
    
};