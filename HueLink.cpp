//
// Created by alex on 5/22/20.
//

#include "include/HueLink.h"
#include <nlohmann/json.hpp>
#include <utility>
#include <cpr/cpr.h>
#include <iostream>

using namespace std;
using json = nlohmann::json;


HueLink::HueLink (string  ip, int port, string  username)
        : ip(ip),
          port(port),
          username(username)
{
    base_url = "http://" + ip + ":" + to_string(port) + "/api";
}

json HueLink::getAllLights() {
    auto r_all= cpr::Get(cpr::Url{base_url+"/"+username+"/lights"});
    json res_all = json::parse(r_all.text);
    json lights;
    for (auto& l : res_all.items()){
        auto r = cpr::Get(cpr::Url{base_url+"/"+username+"/lights/"+l.key()});
        if (r.status_code == 200) {
            json res = json::parse(r.text);
            //cout << res.dump(2) << endl;
            json lj;
            lj["name"] = res["name"];
            lj["id"] = l.key();
            lj["on"] = res["state"]["on"];
            lj["brightness"] = res["state"]["bri"];
            lights.push_back(lj);
        }
    }

    return lights;
}

int HueLink::getAllLights_wrapper(json &lights){
    if (username.empty()){
        auto newUn = newUsername();
        if (newUn.empty()){
            cout << "Unable to connect. Please rerun program and try again" << endl;
            return EXIT_FAILURE;
        }else{
            cout << "Connected!!!" << endl;
        }
    }
    try {
        std::mutex m;
        std::condition_variable cv;

        std::thread t([&cv, &lights, this]()
                      {
                          lights = getAllLights();
                          cv.notify_one();
                      });

        t.detach();

        {
            std::unique_lock<std::mutex> l(m);
            if(cv.wait_for(l, 10s) == std::cv_status::timeout)
                throw std::runtime_error("Timeout");
        }

    }
    catch(nlohmann::detail::parse_error &e){
        cout << "No Target at the give IP address or port. Please check and try again." << endl;
        return EXIT_FAILURE;
    }
    catch(std::runtime_error& e){
        cout << "Unable to connect. Trying to link to Hue Bridge." << endl;
        auto newUn = newUsername();
        if (newUn.empty()){
            cout << "Unable to connect. Please rerun program and try again" << endl;
            return EXIT_FAILURE;
        }else{
            cout << "Connected!!!" << endl;
        }
    }
    return 0;
}

json HueLink::newUsername() {
    cout << "Please push the link button in the next 60 seconds" << endl;
    json body;
    body["devicetype"] = "hue_talk#app";
    if (!username.empty()) body["username"] = username;

    chrono::steady_clock::time_point start = chrono::steady_clock::now();
    while(chrono::steady_clock::now() - start < chrono::seconds(60)) {
        this_thread::sleep_for(chrono::seconds(1) );
        auto r = cpr::Post(cpr::Url{base_url},
                           cpr::Body{body.dump()});
        json res = json::parse(r.text);

        if (res[0].contains("success")){
            username = res[0]["success"]["username"];
            cout << "The username is " << username << ". Save this for faster connection in the future" << endl;
            return res[0];
        }
    }
    return json();
}

int HueLink::lightsUpdate(json &lights, json &changes) {
    vector<string> updated_ids;
    json new_lights;
    int result = getAllLights_wrapper(new_lights);
    for (json &l: lights) {
        auto new_l_in = find_if(new_lights.begin(), new_lights.end(), [&l](json &nl) {
            return nl["id"] == l["id"];
        });
        if (new_l_in != new_lights.end()) {
            json new_l = *new_l_in;
            updated_ids.push_back(new_l["id"]);
            if (new_l["on"] != l["on"]) {
                changes.push_back({{"id", new_l["id"]},
                                   {"on", new_l["on"]}});
            }
            if (new_l["brightness"] != l["brightness"]) {
                changes.push_back({{"id",         new_l["id"]},
                                   {"brightness", new_l["brightness"]}});
            }
            if (new_l["name"] != l["name"]) {
                changes.push_back({{"id",   new_l["id"]},
                                   {"name", new_l["name"]}});
            }
        }
    }
    //unable to test these since no way to add or delete lights with simulator
    //note if lights were removed
    if (updated_ids.size() < lights.size()) {
        for (json &l: lights) {
            auto new_l_in = find(updated_ids.begin(), updated_ids.end(), l["id"]);
            if (new_l_in != updated_ids.end()) {
                changes.push_back({"removed", l});
            }
        }
    }

    //note if lights were added
    if (updated_ids.size() < new_lights.size()) {
        for (json &l: new_lights) {
            auto new_l_in = find(updated_ids.begin(), updated_ids.end(), l["id"]);
            if (new_l_in != updated_ids.end()) {
                changes.push_back({"new light", l});
            }
        }

    }
    if(result == 0) lights = new_lights;
    return result;
}