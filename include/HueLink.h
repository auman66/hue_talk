//
// Created by alex on 5/22/20.
//

#ifndef HUE_TALK_HUELINK_H
#define HUE_TALK_HUELINK_H

#include <nlohmann/json.hpp>
#include <cpr/cpr.h>

using namespace std;
using json = nlohmann::json;

class HueLink {
    public:
        HueLink(string  ip, int port, string  username);
        json newUsername();
        int getAllLights_wrapper(json &lights);
        int lightsUpdate(json &lights, json &new_lights);


    private:
        string ip;
        int port;
        string username;
        string base_url;
        json getAllLights();


};


#endif //HUE_TALK_HUELINK_H
