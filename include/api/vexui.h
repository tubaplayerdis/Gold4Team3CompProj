/*---------------------------------------------*/
/*                                             */
/*    Module:     vexui.h                      */
/*    Author:     Aaron Wilk                   */
/*    Created:    13 Nov 2024                  */
/*    Description: Header for vexui lib        */
/*                                             */
/*---------------------------------------------*/


#pragma once

#include "vex.h"
#include <string>
#include <sstream>
#include <vector>
#include <functional>
#include <algorithm>
#include <cmath>

#define DEG_TO_RAD(deg) ((deg) * M_PI / 180.0)

/*The Vex Brain Sceen is 480 x 240*/

namespace vexui
{   
    enum gameElements {
        mobileGoal,
        redRing,
        blueRing,
    };

    inline void resetColor() {
        uint32_t white = (255 << 16) | (255 << 8) | 255;
        uint32_t black = (0 << 16) | (0 << 8) | 0;
        vexDisplayForegroundColor(white);
        vexDisplayBackgroundColor(black);
    }

    inline uint32_t rgbtocol(int r, int g, int b) {
        uint32_t color = (r << 16) | (g << 8) | b;
        return color;
    }

    inline std::string to_string_float(float x){
        std::stringstream s;
        s << x;
        return s.str();
    }
    class Color {
        public:
            int R, G, B;
            bool Foreground;
            Color(int r, int g, int b, bool foreground_only);

            void set(const std::string &color);

            void mset(int r, int g, int b);

            void gset();

            static inline Color getColor(const std::string &color, bool f) {
                if (color == "red") return vexui::Color(178,31,53, f);
                else if (color == "redorange") return vexui::Color(216, 39, 53, f);
                else if (color == "orange") return vexui::Color(255, 116, 53, f);
                else if (color == "orangeyellow") return vexui::Color(255, 161, 53, f);
                else if (color == "yellow") return vexui::Color(255, 240, 53, f);
                else if (color == "darkgreen") return vexui::Color(0, 117, 58, f);
                else if (color == "green") return vexui::Color(0, 158, 71, f);
                else if (color == "lightgreen") return vexui::Color(22, 221, 53, f);
                else if (color == "darkblue") return vexui::Color(0, 82, 165, f);
                else if (color == "blue") return vexui::Color(0, 121, 252, f);
                else if (color == "lightblue") return vexui::Color(0, 169, 252, f);
                else if (color == "cyan") return vexui::Color(0, 255, 255, f);
                else if (color == "violet") return vexui::Color(104, 30, 126, f);
                else if (color == "purple") return vexui::Color(125, 60, 181, f);
                else if (color == "lightpurple") return vexui::Color(189, 122, 246, f);
                else if (color == "underwearcrust") return vexui::Color(169, 104, 64, f);
                else if (color == "brown") return vexui::Color(183, 97, 39, f);
                else if (color == "lightbrown") return vexui::Color(210, 138, 90, f);
                else if (color == "darkbrown") return vexui::Color(95, 44, 10, f);
                else if (color == "black") return vexui::Color(0, 0, 0, f);
                else if (color == "white") return vexui::Color(255, 255, 255, f);
                else if (color == "gold") return vexui::Color(160, 160, 60, f);
                else if (color == "gray") return vexui::Color(160, 160, 160, f);
                else if (color == "lightgray") return vexui::Color(211, 211, 211, f);
                else if (color == "darkgray") return vexui::Color(105, 105, 105, f);
                else return vexui::Color(0,0,0, f);
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
            short lastPressCount;
        public:
            int x, y, width, height;
            bool dorender;
            vexui::Event pressEvent;

            UIElement();

            virtual ~UIElement() = default;

            bool isPress();

            int getLastPressX();

            int getLastPressY();

            void cpoi();

            virtual std::pair<int, int> getSize();

            virtual void setSize(int w, int h);

            virtual void render() = 0;

            
    };

    
    class Rectangle : public UIElement {
        public:
            std::string text = " ";
            bool showText = false;
            vexui::Color color{255,255,255, false}, txcolor{0,0,0,true};

            Rectangle(int x, int y, int x2, int y2);

            void render() override;
    };
    

    enum LabelSize {
        SMALL = 0,
        MEDIUM = 1,
        LARGE = 2
    };

    class Label : public UIElement {
        public:
            std::string text;
            vexui::Color color{255, 255, 255, true}, bgcolor{0,0,0, false};
            LabelSize txsize = LabelSize::MEDIUM;

            Label(int x, int y, const std::string &text);

            ~Label() = default;

            void setText(const std::string &nt);
            std::string getText() const;

            void render() override;
    };

    class Button : public UIElement {
        public:
            std::string text;
            vexui::Color bdcolor{0, 0, 0, true}, txcolor{255, 255, 255, true}, bgcolor{140, 140, 140, false};
            bool renderBackground;
            bool renderBorder;

            Button(int x, int y, int width, int height, const std::string &text);

            ~Button() = default;

            std::string getText() const;
            void setSize(int w, int h) override;
            void render() override;
    };

    class Dropdown : public UIElement
    {
        private:
            std::vector<UIElement*> items;
            const int buffer = 5;

        public:
            std::string text;
            vexui::Color bdcolor{200, 200, 200, true}, txcolor{255, 255, 255, true}, bgcolor{160, 160, 160, false}, btcolor{50,50,50, true};
            bool renderBorder;
            bool isCollapsable;
            bool collapsed;

            Dropdown(int x, int y, int width, int height, const std::string &text, bool iscollapsable);

            void toggle();

            void addElement(UIElement* element);

            void removeElement(int index);

            void removeAll();

            void render() override;
            
    };

    class TButton : public UIElement {
        public:
            std::string text;
            vexui::Color bdcolor{0, 0, 0, true}, txcolor{255, 255, 255, true}, bgcolor{140, 140, 140, false}, oncolor{160,160,160, false}, ofcolor{20,20,20, false};
            bool renderBackground;
            bool renderBorder;
            bool toggle;

            TButton(int x, int y, int width, int height, const std::string &text);

            void toggleToggle();

            std::string getText();

            void render() override;
    };
    
    class Panel : public UIElement {
        private:
            std::vector<UIElement*> items;

        public:
            bool renderBackground;
            bool renderBorder;
            vexui::Color color{55,55,55, false}, bdcolor{20,20,20, false};

            Panel(int x, int y, int width, int height);

            void addElement(UIElement* element);

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
            vexui::Color bgcolor{160,160,160, false}, bdcolor{0,0,0, false}, slcolor{105, 105, 105, false}, selcolor{0,0,0,false}, lncolor{0,0,0, false}, maxtxcolor{255,255,255, true}, mintxcolor{255,255,255, true}, valbxcolor{150,150,150, true}, valtxcolor{255,255,255, true};
           
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

    typedef struct StartingPosition {
        OdometryPoint pos;
        float heading;
        std::string name;
    };

    inline int fti(float value) {
        return (int)(value >= 0.0f ? value + 0.5f : value - 0.5f);
    }

    typedef struct OdometryGameElement {
        int x;
        int y;
        gameElements elementtype;
        bool render;
    };

    class OdometryMap : public UIElement {
        private:

            int mapheight = 150;
            int mapwidth = 150;
            float botLinelen = 20;

            
        public:
            double* xref;
            double* yref;
            double* headingref;


            vexui::Color mpgcolor{192,192,192, false}, lncolor{81,81,81, false}, blcolor{25, 173, 207, false}, botcolor{150, 61, 39, false}, botheadingcolor{150, 132, 39, false}, txcolor{255,255,255, true}, txbgcolor{55,55,55, false};
            OdometryUnits unit;
            std::vector<OdometryGameElement> elements = std::vector<OdometryGameElement>();

            OdometryMap(int x, int y, double* xref, double* yref, double* headingref, OdometryUnits uints);

            OdometryPoint* translateCoords();

            void setNewX(double x, OdometryUnits units);

            void setNewY(double y, OdometryUnits units);

            void setNewH(double h);

            void render() override;
    };

}