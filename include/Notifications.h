#pragma once

#include "vex.h"
#include "vector"
#include "string"
#include "Bot.h"

class Notifications {
    public:
        static int notificationLoop();
        static void addNotification(std::string notif);
        static void notifBackward();
        static void notifForward();
        static int notifIndex;
        static std::vector<std::string> NotificationList;
        static bool isNotifying;
};