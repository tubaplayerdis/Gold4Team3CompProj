/*---------------------------------------------*/
/*                                             */
/*    Module:     vexui.h                      */
/*    Author:     Aaron Wilk                   */
/*    Created:    16 Nov 2024                  */
/*    Description: Source for vexui lib        */
/*                                             */
/*---------------------------------------------*/



#include "api/vexui.h"
#include "vex.h"
#include "v5_api.h"
#include <string>
#include <vector>
#include <functional>
#include <algorithm>
#include <cmath>
#include "Bot.h"


vexui::Color::Color(int r, int g, int b) {
    R = r;
    G = g;
    B = b;
}

void vexui::Color::set(const std::string &color) {
    Color nc = getColor(color);
    R = nc.R;
    G = nc.G;
    B = nc.B;
}

void vexui::Color::mset(int r, int g, int b) {
    R = r;
    G = g;
    B = b;
}

void vexui::Color::gset() {
    uint32_t color_code = (R << 16) | (G << 8) | B;
    //Colors
    vexDisplayForegroundColor(color_code);
    vexDisplayBackgroundColor(color_code);
}

vexui::Event::Event() {
    listeners = std::vector<void (*)(void)>();
}

void vexui::Event::addListener(void (*listener)(void)) {
    listeners.push_back(listener);
}

void vexui::Event::removeListener(int index) {
    listeners.erase(listeners.begin() + index);
}

void vexui::Event::removeAll() {
    listeners.clear();
}

void vexui::Event::InvokeListeners() {
    for (auto &listener : listeners) listener();
}

vexui::UIElement::UIElement() {
    x = 0;
    y = 0;
    width = 100;
    height = 20;
    dorender = true;
    pressEvent = vexui::Event();
    lastPressX = -1;
    lastPressY = -1;
}

bool vexui::UIElement::isPress() {
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

int vexui::UIElement::getLastPressX() {
    return lastPressX;
}

int vexui::UIElement::getLastPressY() {
    return lastPressY;
}

std::pair<int, int> vexui::UIElement::getSize() {
    return std::pair<int, int>(width, height);
}

void vexui::UIElement::setSize(int w, int h) {
    width = w;
    height = h;
}

void vexui::UIElement::render() {
    vexDisplayStringAt(x,y, "Unimplemented!");
}

vexui::Label::Label(int x, int y, const std::string &text)  : vexui::UIElement() {
    this->x = x;
    this->y = y;
    this->text = text;
    width = getStringWidth(text);
}

void vexui::Label::setText(const std::string &nt) { text = nt; width = getStringWidth(text); }

std::string vexui::Label::getText() const { return text; }

void vexui::Label::render() { 
    Bot::Brain.Screen.clearScreen();
    if(!dorender) return;
    color.gset();
    vexDisplayStringAt(x,y, text.c_str());
    resetColor();
}

vexui::Button::Button(int x, int y, int width, int height, const std::string &text) : vexui::UIElement() {
    this->x = x;
    this->y = y;
    this->width = width;
    this->height = height;
    this->renderBackground = true;
    this->renderBorder = true;
    this->text = text;
}

std::string vexui::Button::getText() const { return text; }

void vexui::Button::setSize(int w, int h) { width = w; height = h; }

void vexui::Button::render() {
    if(!dorender) return;
    bgcolor.gset();
    if(renderBackground) vexDisplayRectFill(x, y, x+width, y+height);
    bdcolor.gset();
    if(renderBorder) vexDisplayRectDraw(x, y, x+width, y+ height);
    txcolor.gset();
    vexDisplayStringAt(x+3,y+(height/10),text.c_str());
    resetColor();
}

vexui::Dropdown::Dropdown(int x, int y, int width, int height, const std::string &text, bool iscollapsable)  : vexui::UIElement() {
    this->x = x;
    this->y = y;
    this->width = width;
    this->height = height;
    this->renderBorder = true;
    this->isCollapsable = iscollapsable;
    items = std::vector<UIElement>();
}

void vexui::Dropdown::toggle() {
    collapsed = !collapsed;
}

void vexui::Dropdown::addElement(vexui::UIElement element) {
    items.push_back(element);
}

void vexui::Dropdown::removeElement(int index) {
    items.erase(items.begin() + index);
}

void vexui::Dropdown::removeAll() {
    items.clear();
}

void vexui::Dropdown::render() {
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

vexui::TButton::TButton(int x, int y, int width, int height, const std::string &text)  : vexui::UIElement() {
    this->x = x;
    this->y = y;
    this->width = width;
    this->height = height;
    this->renderBackground = true;
    this->renderBorder = true;
    this->toggle = false;
    this->text = text;
}

void vexui::TButton::toggleToggle() { toggle = !toggle; }

std::string vexui::TButton::getText() { return text; }

void vexui::TButton::render() {
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

vexui::Panel::Panel(int x, int y, int width, int height)  : vexui::UIElement() {
    this->x = x;
    this->y = y;
    this->width = width;
    this->height = height;
    this->renderBackground = true;
    this->renderBorder = true;
    items = std::vector<UIElement>();
}

void vexui::Panel::addElement(UIElement element) {
    element.x = x+element.x;
    element.y = y+element.y;
    items.push_back(element);
}

void vexui::Panel::removeElement(int index) {
    items.erase(items.begin() + index);
}

void vexui::Panel::removeAll() {
    items.clear();
}

void vexui::Panel::render() {
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

vexui::Slider::Slider(int x, int y, int width, int height, float min, float max, float value, bool isInt)  : vexui::UIElement() 
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

void vexui::Slider::evalValue() {
    float medval = (prvalue - x - 5) / (width - 10);
    float frange = abs(rangemax) + abs(rangemin);
    if (isint) {
        value = round((medval * frange) - abs(rangemin));
    } else {
        value = (medval * frange) - abs(rangemin);
    }
}

void vexui::Slider::render() {
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

vexui::OdometryMap::OdometryMap(int x, int y, float* xref, float* yref, float* headingref, OdometryUnits uints) : UIElement() {
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

vexui::OdometryPoint* vexui::OdometryMap::translateCoords() {
    vexui::OdometryPoint* corners = (vexui::OdometryPoint*)malloc(6 * sizeof(vexui::OdometryPoint));

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

void vexui::OdometryMap::render() {
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

    