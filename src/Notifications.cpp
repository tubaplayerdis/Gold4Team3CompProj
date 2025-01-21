#include "Notifications.h"
#include "vector"
#include "string"
#include "Bot.h"
#include "UISystem.h"
#include "api/vexui.h"

std::vector<std::string> Notifications::NotificationList = std::vector<std::string>();
int Notifications::notifIndex = 0;
bool Notifications::isNotifying = false;

void Notifications::addNotification(std::string notif) {
    //Only new notifs not the same crap
    for(std::string obj : NotificationList) {
        if(obj == notif) return;
    }
    isNotifying = true;
    NotificationList.push_back(notif);
    notifIndex = NotificationList.size()-1;
    UISystem::diagDropdown.addString(notif);
    isNotifying = false;
}

//Dont use
int Notifications::notificationLoop() {
    while(true) {

        vex::this_thread::sleep_for(50);
    }
    return 0;
}

void Notifications::notifBackward() {
    int newval = notifIndex - 1;
    if(newval < 0) {
        notifIndex = NotificationList.size()-1;
    } else if(newval > NotificationList.size()-1) {
        notifIndex = 0;
    } else {
        notifIndex = newval;
    }
}

void Notifications::notifForward() {
    int newval = notifIndex + 1;
    if(newval < 0) {
        notifIndex = NotificationList.size()-1;
    } else if(newval > NotificationList.size()-1) {
        notifIndex = 0;
    } else {
        notifIndex = newval;
    }
}