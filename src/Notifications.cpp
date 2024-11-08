#include "Notifications.h"
#include "vector"
#include "string"
#include <mutex>

std::vector<std::string> Notifications::NotificationList = std::vector<std::string>();
int Notifications::notifIndex = 0;

//std::mutex mtx;

void Notifications::addNotification(std::string notif) {
    //Only new notifs not the same crap
    for(std::string obj : NotificationList) {
        if(obj == notif) return;
    }
    NotificationList.push_back(notif);
    notifIndex = NotificationList.size()-1;
}

int Notifications::notificationLoop() {
    while(true) {
        if(NotificationList.empty()) continue;
        if(notifIndex > NotificationList.size()-1 || notifIndex < 0) {
            notifIndex = 0;
            continue;
        }
        
        Bot::Controller.Screen.setCursor(2,1);
        Bot::Controller.Screen.clearLine(2);
        Bot::Controller.Screen.print("%02d:", notifIndex);
        Bot::Controller.Screen.setCursor(2,4);
        Bot::Controller.Screen.print(NotificationList.at(notifIndex).c_str());

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