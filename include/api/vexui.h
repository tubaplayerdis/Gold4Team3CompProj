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

/*The Vex Brain Sceen is 480 x 240*/

#pragma region Helper_Functions 

std::string to_string_float(float x){
  std::stringstream s;
  s << x;
  return s.str();
}

void resetColor() {
    uint32_t color_code = (255 << 16) | (255 << 8) | 255;
    vexDisplayForegroundColor(color_code);
    vexDisplayBackgroundColor(color_code);
}

#pragma endregion

namespace vexui
{   
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
                uint32_t color_code = (R << 16) | (G << 8) | B;
                //Colors
                vexDisplayForegroundColor(color_code);
                vexDisplayBackgroundColor(color_code);
            }

            private:
                static Color getColor(const std::string &color) {
                    if (color == "red") return Color(178,31,53);
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
                }
    };

    inline int getStringWidth(const std::string &text) {
        return Bot::Brain.Screen.getStringWidth(text.c_str());
    }

    inline int getStringHeight(const std::string &text) {
        return Bot::Brain.Screen.getStringHeight(text.c_str());
    }

    class Event {
        public:
            std::vector<void (*)(void)> listeners;

            Event() {
                this->listeners = std::vector<void (*)(void)>();
            }
            void addListener(void (*listener)(void)) { listeners.push_back(listener); }
            void removeListener(int index) { listeners.erase(listeners.begin() + index); }
            void removeAll() { listeners.clear(); }
            void InvokeListeners() {
                for (auto &listener : listeners) listener();
            }
    };

    class UIElement {
        private:
            int lastPressX = -1;
            int lastPressY = -1;
        public:
            int x = 0, y = 0, width = 100, height = 20;
            bool dorender = true;
            Event pressEvent = Event();

            UIElement() {
                
            }

            bool isPress() {
                V5_TouchStatus status;
                vexTouchDataGet(&status);

                if(status.pressCount > 0 && ((status.lastXpos <= x && status.lastXpos >= x+width) && (status.lastYpos <= y && status.lastYpos >= y+height))) {
                    pressEvent.InvokeListeners();
                    lastPressX = status.lastXpos;
                    lastPressY = status.lastYpos;
                    return true;
                }
                return false;
              
            }

            int getLastPressX() {
                return lastPressX;
            }

            int getLastPressY() {
                return lastPressY;
            }

            virtual void render();
    };

    class Label : public UIElement {
        public:
            std::string text;
            Color color{0, 0, 0};

            Label(int x, int y, const std::string &text) : UIElement() { 
                this->x = x;
                this->y = y;
                this->text = text;
                width = getStringWidth(text);
            }
            void setText(const std::string &nt) { text = nt; width = getStringWidth(text); }
            std::string getText() const { return text; }

            void render() { 
                if(!dorender) return;
                color.gset();
                vexDisplayStringAt(x,y, text.c_str());
                resetColor();
            }
    };

    class Button : public UIElement {
        public:
            std::string text;
            Color bdcolor{0, 0, 0}, txcolor{0, 0, 0}, bgcolor{140, 140, 140};
            bool renderBackground;
            bool renderBorder;

            Button(int x, int y, int width, int height, const std::string &text) : UIElement() {
                this->x = x;
                this->y = y;
                this->width = width;
                this->height = height;
                this->renderBackground = true;
                this->renderBorder = true;
                this->text = text;
            }

            std::string getText() const { return text; }
            std::pair<int, int> getSize() const { return {width, height}; }
            void setSize(int w, int h) { width = w; height = h; }
            void render() {
                if(!dorender) return;
                bgcolor.gset();
                if(renderBackground) vexDisplayRectFill(x, y, x+width, y+height);
                bdcolor.gset();
                if(renderBorder) vexDisplayRectDraw(x, y, x+width, y+ height);
                txcolor.gset();
                vexDisplayStringAt(x+3,y+(height/10),text.c_str());
                resetColor();
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

            Dropdown(int x, int y, int width, int height, const std::string &text, bool iscollapsable) : UIElement() {
                this->x = x;
                this->y = y;
                this->width = width;
                this->height = height;
                this->renderBorder = true;
                this->isCollapsable = iscollapsable;
                items = std::vector<UIElement>();
            }

            void toggle() {
                collapsed = !collapsed;
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
                if(!dorender) return;
                if(isPress()) toggle();

                bgcolor.gset();
                vexDisplayRectFill(x, y, x+width, y+width);
                bdcolor.gset();
                if(renderBorder) vexDisplayRectDraw(x, y, x+width, y+ height);
                txcolor.gset();
                vexDisplayStringAt(x+3,y+(height/10),text.c_str());

                if(!collapsed) {
                    if(items.size() > 0) {
                        btcolor.gset();
                        vexDisplayLineDraw(x+5,y+(height/1.5),x+10,y+(height/3));
                        vexDisplayLineDraw(x+15,(y+height/1.5),x+10,y+(height/3));
                        int space = 0;
                        int enumeration = 0;
                        for(UIElement item : items) {
                            item.x = this->x+5;
                            int addto = 5;
                            for(int i = 0; i < enumeration; i++) {
                                addto += items[i].height;
                                addto += buffer;
                            }
                            space += (item.height+buffer);
                            item.y = y-(height/2+addto+buffer);
                            enumeration=enumeration+1;
                        }
                        bgcolor.gset();
                        vexDisplayRectFill(x+1, y-space, x+width-1, space+1);
                        for(UIElement item : items) {
                            item.render();
                        }
                    }
                } else {
                    btcolor.gset();
                    vexDisplayLineDraw(x+5,y+(height/3),x+10,y+(height/1.5));
                    vexDisplayLineDraw(x+15,y+(height/3),x+10,y+(height/1.5));
                }
                resetColor();
            }
            
    };

    class TButton : public UIElement {
        public:
            std::string text;
            Color bdcolor{0, 0, 0}, txcolor{0, 0, 0}, bgcolor{140, 140, 140}, oncolor{160,160,160}, ofcolor{0,0,0};
            bool renderBackground;
            bool renderBorder;
            bool toggle;

            TButton(int x, int y, int width, int height, const std::string &text) : UIElement() {
                this->x = x;
                this->y = y;
                this->width = width;
                this->height = height;
                this->renderBackground = true;
                this->renderBorder = true;
                this->toggle = false;
                this->text = text;
            }

            void toggleToggle() { toggle = !toggle; }


            std::string getText() const { return text; }

            std::pair<int, int> getSize() const { return {width, height}; }

            void setSize(int w, int h) { width = w; height = h; }

            void render() {
                if(!dorender) return;
                bgcolor.gset();
                if(renderBackground) vexDisplayRectFill(x, y, x+width, y+height);
                bdcolor.gset();
                if(renderBorder) vexDisplayRectDraw(x, y, x+width, y+height);

                if(toggle) oncolor.gset(); else ofcolor.gset();
                vexDisplayCircleFill(x+(height/2), y+(height/2), height/3);

                txcolor.gset();
                vexDisplayStringAt(x+height,y+(height/10),text.c_str());
                resetColor();
            }
    };
    
    class Panel : public UIElement {
        private:
            std::vector<UIElement> items;

        public:
            bool renderBackground;
            bool renderBorder;
            Color color{55,55,55}, bdcolor{0,0,0};

            Panel(int x, int y, int width, int height) : UIElement() {
                this->x = x;
                this->y = y;
                this->width = width;
                this->height = height;
                this->renderBackground = true;
                this->renderBorder = true;
                items = std::vector<UIElement>();
            }

            void addElement(UIElement element) {
                element.x = x+element.x;
                element.y = y+element.y;
                items.push_back(element);
            }

            void removeElement(int index) {
                items.erase(items.begin() + index);
            }

            void removeAll() {
                items.clear();
            }

            void render() {
                if(!dorender) return;
                color.gset();
                if(renderBackground) vexDisplayRectFill(x, y, x+width, y+width);
                bdcolor.gset();
                if(renderBorder) vexDisplayRectDraw(x, y, x+width, y+ height);
                for(UIElement item : items) {
                    item.render();
                }
                resetColor();
            }

    };

    class Slider : public UIElement {
        private:
            float prvalue;

        public:
            int x, y, width, height;
            float rangemax, rangemin, value;
            bool isint, renderBackground, renderBorder;
            Color bgcolor{160,160,160}, bdcolor{0,0,0}, slcolor{105, 105, 105}, selcolor{0,0,0}, lncolor{0,0,0}, maxtxcolor{0,0,0}, mintxcolor{0,0,0}, valbxcolor{150,150,150}, valtxcolor{0,0,0};
           
            Event onValueChange = Event();
            // Constructor
            Slider(int x, int y, int width, int height, float min, float max, float value, bool isInt) : UIElement()
            {
                this->x = x;
                this->y = y;
                this->width = width;
                this->height = height;
                this->rangemin = min;
                this->rangemax = max;
                this->isint = isint;
                this->renderBackground = true;
                this->renderBorder = true;
                prvalue = x + (width / 2);
            }

            void evalValue() {
                float medval = (prvalue - x - 5) / (width - 10);
                float frange = abs(rangemax) + abs(rangemin);
                if (isint) {
                    value = round((medval * frange) - abs(rangemin));
                } else {
                    value = (medval * frange) - abs(rangemin);
                }
            }

            void render() {
                if(!dorender) return;
                bgcolor.gset();
                if(renderBackground) vexDisplayRectFill(x, y, x+width, y+width);
                bdcolor.gset();
                if(renderBorder) vexDisplayRectDraw(x, y, x+width, y+ height);
                lncolor.gset();
                vexDisplayLineDraw(x+5,y+(height/1.5),(x+width)-5,y+(height/1.5));

                if(isPress()) {
                    int tempval = getLastPressX();
                    prvalue = tempval;
                    if(prvalue < x+5) prvalue = x+5;
                    if (prvalue > x+width-5) prvalue = x+width-5;
                    evalValue();

                    valbxcolor.gset();
                    vexDisplayRectFill(prvalue-10,y+height+3,35,15);
                    valtxcolor.gset();
                    vexDisplayStringAt(prvalue-8, y+height+4, to_string_float(value).c_str());

                    onValueChange.InvokeListeners();
                }

                slcolor.gset();
                vexDisplayCircleFill(prvalue,y+(height/1.5),5);
                mintxcolor.gset();
                vexDisplayStringAt(x+2,y+(height/3)-8, to_string_float(rangemin).c_str());
                maxtxcolor.gset();
                vexDisplayStringAt(x+width-getStringWidth(to_string_float(rangemax).c_str()),y+(height/3)-8, to_string_float(rangemax).c_str());
                resetColor();
            }
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

    int fti(float value) {
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
            Color mpgcolor{192,192,192}, lncolor{81,81,81}, blcolor{25, 173, 207}, botcolor{150, 61, 39}, botheadingcolor{150, 132, 39};
            OdometryUnits unit = OdometryUnits::INCHES;

            OdometryMap(int x, int y, float* xref, float* yref, float* headingref, OdometryUnits uints) : UIElement() {
                this->x = x;
                this->y = y;
                this->width = 200;
                this->height = 250;
                this->xref = xref;
                this->yref = yref;
                this->headingref = headingref;
                this->unit = uints;
            }

            #define DEG_TO_RAD(deg) ((deg) * M_PI / 180.0)

            OdometryPoint* translateCoords() {
                OdometryPoint* corners = (OdometryPoint*)malloc(6 * sizeof(OdometryPoint));

                const float scaleX = 200.0f / 12.0f;
                const float scaleY = 200.0f / 12.0f;

                float feetx = 0.0f, feety = 0.0f;
                switch (unit)
                {
                    case INCHES:
                        feetx = *xref/12.0f;
                        feety = *yref/12.0f;
                        break;

                    case FEET:
                        feetx = *xref;
                        feety = *yref;
                        break;

                    case MILIMETERS:
                        feetx = *xref/304.8f;
                        feety = *yref/304.8f;
                        break;

                    case METERS:
                        feetx = *xref*3.281f;
                        feety = *yref*3.281f;
                        break;
                }

                float centerX = feetx * scaleX;
                float centerY = feety * scaleY;


                float angleRad = DEG_TO_RAD(*headingref);

                
                float halfWidth = 10 / 2.0f;
                float halfHeight = 10 / 2.0f;

                
                OdometryPoint localCorners[4] = {
                    { -halfWidth, -halfHeight }, // Top-left
                    {  halfWidth, -halfHeight }, // Top-right
                    {  halfWidth,  halfHeight }, // Bottom-right
                    { -halfWidth,  halfHeight }  // Bottom-left
                };

                // Rotate and translate each corner
                for (int i = 0; i < 4; i++) {
                    // Apply rotation
                    float rotatedX = localCorners[i].x * cos(angleRad) - localCorners[i].y * sin(angleRad);
                    float rotatedY = localCorners[i].x * sin(angleRad) + localCorners[i].y * cos(angleRad);

                    // Translate to the center position
                    corners[i].x = centerX + rotatedX;
                    corners[i].y = centerY + rotatedY;
                }

                corners[4].x = centerX + botLinelen * cos(angleRad);
                corners[4].y = centerY + botLinelen * sin(angleRad);

                corners[5].x = centerX;
                corners[5].y = centerY;

                return corners;
            } 

            void render() {
                //Draw Map 200x200
                mpgcolor.gset();
                vexDisplayRectDraw(x,y,x+mapwidth,y+mapheight);
                lncolor.gset();
                for(int i = 0; i < 5; i++) {
                    vexDisplayLineDraw(x+(i*40+40),y, x+(i*40+40), y+mapheight);
                }
                for(int i = 0; i < 5; i++) {
                    vexDisplayLineDraw(x,y+(i*40+40), x+mapwidth, y+(i*40+40));
                }
                blcolor.gset();
                vexDisplayLineDraw(x+100, y, x+100, y+mapheight);
                vexDisplayLineDraw(x, y+100, x+mapwidth, y+100);

                //Get Unit as String
                std::string unitstring = "";
                switch (unit)
                {
                    case INCHES:
                        unitstring = "in";

                    case FEET:
                        unitstring = "ft";

                    case MILIMETERS:
                        unitstring = "mm";

                    case METERS:
                        unitstring = "m";
                
                    default:
                        unitstring = "in";
                }


                //Draw Position and Heading on Info
                std::stringstream ss;
                ss << "X: " << *xref << " " << unitstring << ", Y: " << *yref << " " << unitstring << ", H: " << *headingref << " deg";
                vexDisplayStringAt(x+20, y+220, ss.str().c_str());

                //Draw Bot Character
                OdometryPoint* cc = translateCoords();
                //20x20 Bot, x and y are middle pos
                botcolor.gset();
                vexDisplayLineDraw(fti(cc[0].x), fti(cc[0].y),fti(cc[1].x), fti(cc[1].y));
                vexDisplayLineDraw(fti(cc[1].x), fti(cc[1].y),fti(cc[2].x), fti(cc[2].y));
                vexDisplayLineDraw(fti(cc[2].x), fti(cc[2].y),fti(cc[3].x), fti(cc[3].y));
                vexDisplayLineDraw(fti(cc[3].x), fti(cc[3].y),fti(cc[0].x), fti(cc[0].y));

                botheadingcolor.gset();
                vexDisplayLineDraw(fti(cc[4].x), fti(cc[4].y), fti(cc[5].x), fti(cc[5].y));

                //Free the mem
                free(cc);

                //Reset Color
                resetColor();
            }
    };

}



