/*---------------------------------------------*/
/*                                             */
/*    Module:     vexui.h                      */
/*    Author:     Aaron Wilk                   */
/*    Created:    13 Nov 2024                  */
/*                                             */
/*---------------------------------------------*/


#pragma once

#include "vex.h"
#include "Bot.h"
#include "Odometry.h"
#include "ColorDetection.h"
#include "Drivetrain.h"
#include "Notifications.h"
#include <string>
#include <vector>
#include <functional>
#include <algorithm>
#include <cmath>


#pragma region badFunc 
int rgbToHue(int r, int g, int b) {
    float fr = r / 255.0f;
    float fg = g / 255.0f;
    float fb = b / 255.0f;

    float max_val = std::max({fr, fg, fb});
    float min_val = std::min({fr, fg, fb});
    float delta = max_val - min_val;

    // Calculate Hue
    float hue;
    if (delta == 0) {
        hue = 0; // Gray (no color)
    } else if (max_val == fr) {
        hue = 60 * (std::fmod(((fg - fb) / delta), 6));
    } else if (max_val == fg) {
        hue = 60 * (((fb - fr) / delta) + 2);
    } else {
        hue = 60 * (((fr - fg) / delta) + 4);
    }

    // Make sure hue is in the range 0-360
    if (hue < 0) {
        hue += 360;
    }

    return static_cast<int>(std::round(hue));
}

#pragma endregion

namespace vexui
{
    class vexui
    {
        public:
           static inline vex::brain::lcd Screen = Bot::Brain.Screen;
    };
    
    class Color {
        public:
            int R, G, B;
            Color(int r, int g, int b) {
                R = r;
                G = g;
                B = b;
            }

            void set(const std::string &color) {
                Color nc = getColor(color);
                R = nc.R;
                G = nc.G;
                B = nc.B;
            }

            void mset(int r, int g, int b) {
                R = r;
                G = g;
                B = b;
            }

            void gset() {
                vexDisplayForegroundColor(rgbToHue(R,G,B));
                vexDisplayBackgroundColor(rgbToHue(R,G,B));
            }
    };

    Color getColor(const std::string &color) {
        if (color == "red") return Color(178, 31, 53);
        else if (color == "redorange") return Color(216, 39, 53);
        else if (color == "orange") return Color(255, 116, 53);
        else if (color == "orangeyellow") return Color(255, 161, 53);
        else if (color == "yellow") return Color(255, 240, 53);
        else if (color == "darkgreen") return Color(0, 117, 58);
        else if (color == "green") return Color(0, 158, 71);
        else if (color == "lightgreen") return Color(22, 221, 53);
        else if (color == "darkblue") return Color(0, 82, 165);
        else if (color == "blue") return Color(0, 121, 252);
        else if (color == "lightblue") return Color(0, 169, 252);
        else if (color == "cyan") return Color(0, 255, 255);
        else if (color == "violet") return Color(104, 30, 126);
        else if (color == "purple") return Color(125, 60, 181);
        else if (color == "lightpurple") return Color(189, 122, 246);
        else if (color == "underwearcrust") return Color(169, 104, 64);
        else if (color == "brown") return Color(183, 97, 39);
        else if (color == "lightbrown") return Color(210, 138, 90);
        else if (color == "darkbrown") return Color(95, 44, 10);
        else if (color == "black") return Color(0, 0, 0);
        else if (color == "white") return Color(255, 255, 255);
        else if (color == "gold") return Color(160, 160, 60);
        else if (color == "gray") return Color(160, 160, 160);
        else if (color == "lightgray") return Color(211, 211, 211);
        else if (color == "darkgray") return Color(105, 105, 105);

        //Default
        return Color(0, 0, 0);
    }

    inline int getStringWidth(const std::string &text) {
        return Bot::Brain.Screen.getStringWidth(text.c_str());
    }

    inline int getStringHeight(const std::string &text) {
        return Bot::Brain.Screen.getStringHeight(text.c_str());
    }

    class Event {
        public:
            std::vector<std::function<void(void*)>> listeners;

            void addListener(std::function<void(void*)> listener) { listeners.push_back(listener); }
            void removeListener(int index) { listeners.erase(listeners.begin() + index); }
            void removeAll() { listeners.clear(); }
            void InvokeListeners(void *ref) {
                for (auto &listener : listeners) listener(ref);
            }
    };

    class UIElement {
        public:
            int x = 0, y = 0, width = 100, height = 20;
            bool input = false, dorender = true;
            Event pressEvent();

            bool isPress() {
                return (vexui::Screen.pressing() && ((vexui::Screen.xPosition() <= x && vexui::Screen.xPosition() >= x+width) && (vexui::Screen.yPosition() <= y && vexui::Screen.yPosition() >= y+height)));
            }

            virtual void render();
    };

    class Label : public UIElement {
        public:
            std::string text;
            Color color{0, 0, 0};

            Label(int x, int y, const std::string &text) { 
                this->x = x;
                this->y = y;
                this->text = text;
                width = getStringWidth(text);
            }
            void setText(const std::string &nt) { text = nt; width = getStringWidth(text); }
            std::string getText() const { return text; }

            void render() {
                color.gset();
                vexui::Screen.printAt(x,y, text.c_str());
            }
    };

    class Button : public UIElement {
        public:
            std::string text;
            Color bdcolor{0, 0, 0}, txcolor{0, 0, 0}, bgcolor{140, 140, 140};
            bool renderBackgroud;
            bool renderBorder;

            Button(int x, int y, int width, int height, const std::string &text) {
                this->x = x;
                this->y = y;
                this->width = width;
                this->height = height;
                this->renderBackgroud = true;
                this->renderBorder = true;
                this->text = text;
            }

            std::string getText() const { return text; }
            std::pair<int, int> getSize() const { return {width, height}; }
            void setSize(int w, int h) { width = w; height = h; }
            void render() {
                bgcolor.gset();
                if(renderBackgroud) vexDisplayRectFill(x, y, x+width, y+width);
                bdcolor.gset();
                if(renderBorder) vexDisplayRectDraw(x, y, x+width, y+ height);
                txcolor.gset();
                vexDisplayStringAt(x+3,y+(height/10),text.c_str());
            }
    };

    class Dropdown : public UIElement
    {
        private:
            std::vector<UIElement> items;
            const int buffer = 5;

        public:
            std::string text;
            Color bdcolor{0, 0, 0}, txcolor{0, 0, 0}, bgcolor{160, 160, 160}, btcolor{0,0,0};
            bool renderBorder;
            bool isCollapsable;
            bool collapsed;

            Dropdown(int x, int y, int width, int height, const std::string &text, bool iscollapsable) {
                this->x = x;
                this->y = y;
                this->width = width;
                this->height = height;
                this->renderBorder = true;
                this->isCollapsable = iscollapsable;
            }

            void addElement(UIElement element) {
                items.push_back(element);
            }

            void removeElement(int index) {
                items.erase(items.begin() + index);
            }

            void removeAll() {
                items.clear();
            }

            void render() {
                //Tomorrow add IMPL
            }
            
    };
    


}



