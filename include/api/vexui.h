/*---------------------------------------------*/
/*                                             */
/*    Module:     vexui.h                      */
/*    Author:     Aaron Wilk                   */
/*    Created:    13 Nov 2024                  */
/*                                             */
/*---------------------------------------------*/


#pragma once

#include "vex.h"
#include <string>
#include <vector>
#include <functional>
#include <algorithm>
#include <cmath>

#define DEG_TO_RAD(deg) ((deg) * M_PI / 180.0)

/*The Vex Brain Sceen is 480 x 240*/

namespace vexui
{   
    inline void resetColor() {
        uint32_t color_code = (255 << 16) | (255 << 8) | 255;
        vexDisplayForegroundColor(color_code);
        vexDisplayBackgroundColor(color_code);
    }

    inline std::string to_string_float(float x){
        std::stringstream s;
        s << x;
        return s.str();
    }
    class Color {
        public:
            int R, G, B;
            Color(int r, int g, int b);

            void set(const std::string &color);

            void mset(int r, int g, int b);

            void gset();

            static inline Color getColor(const std::string &color) {
                if (color == "red") return vexui::Color(178,31,53);
                else if (color == "redorange") return vexui::Color(216, 39, 53);
                else if (color == "orange") return vexui::Color(255, 116, 53);
                else if (color == "orangeyellow") return vexui::Color(255, 161, 53);
                else if (color == "yellow") return vexui::Color(255, 240, 53);
                else if (color == "darkgreen") return vexui::Color(0, 117, 58);
                else if (color == "green") return vexui::Color(0, 158, 71);
                else if (color == "lightgreen") return vexui::Color(22, 221, 53);
                else if (color == "darkblue") return vexui::Color(0, 82, 165);
                else if (color == "blue") return vexui::Color(0, 121, 252);
                else if (color == "lightblue") return vexui::Color(0, 169, 252);
                else if (color == "cyan") return vexui::Color(0, 255, 255);
                else if (color == "violet") return vexui::Color(104, 30, 126);
                else if (color == "purple") return vexui::Color(125, 60, 181);
                else if (color == "lightpurple") return vexui::Color(189, 122, 246);
                else if (color == "underwearcrust") return vexui::Color(169, 104, 64);
                else if (color == "brown") return vexui::Color(183, 97, 39);
                else if (color == "lightbrown") return vexui::Color(210, 138, 90);
                else if (color == "darkbrown") return vexui::Color(95, 44, 10);
                else if (color == "black") return vexui::Color(0, 0, 0);
                else if (color == "white") return vexui::Color(255, 255, 255);
                else if (color == "gold") return vexui::Color(160, 160, 60);
                else if (color == "gray") return vexui::Color(160, 160, 160);
                else if (color == "lightgray") return vexui::Color(211, 211, 211);
                else if (color == "darkgray") return vexui::Color(105, 105, 105);
                else return vexui::Color(0,0,0);
            }
    };

    inline int getStringWidth(const std::string &text) {
        return vexDisplayStringWidthGet(text.c_str());
    }

    inline int getStringHeight(const std::string &text) {
        return vexDisplayStringHeightGet(text.c_str());
    }

    class Event {
        public:
            std::vector<void (*)(void)> listeners;

            Event();
            void addListener(void (*listener)(void));
            void removeListener(int index);
            void removeAll();
            void InvokeListeners();
    };

    class UIElement {
        protected:
            int lastPressX;
            int lastPressY;
        public:
            int x, y, width, height;
            bool dorender;
            vexui::Event pressEvent;

            UIElement();

            virtual ~UIElement() = default;

            bool isPress();

            int getLastPressX();

            int getLastPressY();

            virtual void render() = 0;

            
    };

    class Label : public UIElement {
        public:
            std::string text;
            vexui::Color color{0, 0, 0};

            inline Label(int x, int y, const std::string &text);

            ~Label() = default;

            void setText(const std::string &nt);
            std::string getText() const;

            void render() override;
    };

    class Button : public UIElement {
        public:
            std::string text;
            vexui::Color bdcolor{0, 0, 0}, txcolor{0, 0, 0}, bgcolor{140, 140, 140};
            bool renderBackground;
            bool renderBorder;

            Button(int x, int y, int width, int height, const std::string &text);

            ~Button() = default;

            std::string getText() const;
            void setSize(int w, int h);
            void render() override;
    };

    class Dropdown : public UIElement
    {
        private:
            std::vector<UIElement> items;
            const int buffer = 5;

        public:
            std::string text;
            vexui::Color bdcolor{0, 0, 0}, txcolor{0, 0, 0}, bgcolor{160, 160, 160}, btcolor{0,0,0};
            bool renderBorder;
            bool isCollapsable;
            bool collapsed;

            Dropdown(int x, int y, int width, int height, const std::string &text, bool iscollapsable);

            void toggle();

            void addElement(UIElement &element);

            void removeElement(int index);

            void removeAll();

            void render() override;
            
    };

    class TButton : public UIElement {
        public:
            std::string text;
            vexui::Color bdcolor{0, 0, 0}, txcolor{0, 0, 0}, bgcolor{140, 140, 140}, oncolor{160,160,160}, ofcolor{0,0,0};
            bool renderBackground;
            bool renderBorder;
            bool toggle;

            TButton(int x, int y, int width, int height, const std::string &text);

            void toggleToggle();

            std::string getText();

            void setSize(int w, int h);

            void render() override;
    };
    
    class Panel : public UIElement {
        private:
            std::vector<UIElement> items;

        public:
            bool renderBackground;
            bool renderBorder;
            vexui::Color color{55,55,55}, bdcolor{0,0,0};

            Panel(int x, int y, int width, int height);

            void addElement(UIElement &element);

            void removeElement(int index);

            void removeAll();

            void render() override;

    };

    class Slider : public UIElement {
        private:
            float prvalue;

        public:
            int x, y, width, height;
            float rangemax, rangemin, value;
            bool isint, renderBackground, renderBorder;
            vexui::Color bgcolor{160,160,160}, bdcolor{0,0,0}, slcolor{105, 105, 105}, selcolor{0,0,0}, lncolor{0,0,0}, maxtxcolor{0,0,0}, mintxcolor{0,0,0}, valbxcolor{150,150,150}, valtxcolor{0,0,0};
           
            vexui::Event onValueChange;
            // Constructor
            Slider(int x, int y, int width, int height, float min, float max, float value, bool isInt);

            void evalValue();

            void render() override;
    };

    enum OdometryUnits {
        INCHES = 0,
        FEET = 1,
        MILIMETERS = 2,
        METERS = 3
    };

    typedef struct {
        float x;
        float y;
    } OdometryPoint;

    inline int fti(float value) {
        return (int)(value >= 0.0f ? value + 0.5f : value - 0.5f);
    }

    class OdometryMap : public UIElement {
        private:
            float* xref;
            float* yref;
            float* headingref;

            int mapheight = 200;
            int mapwidth = 200;
            float botLinelen = 20;

        public:
            vexui::Color mpgcolor{192,192,192}, lncolor{81,81,81}, blcolor{25, 173, 207}, botcolor{150, 61, 39}, botheadingcolor{150, 132, 39};
            OdometryUnits unit = OdometryUnits::INCHES;

            OdometryMap(int x, int y, float* xref, float* yref, float* headingref, OdometryUnits uints);

            OdometryPoint* translateCoords();

            void render() override;
    };

}



