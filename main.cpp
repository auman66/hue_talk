#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <algorithm>
#include <nlohmann/json.hpp>
#include <cpr/cpr.h>
#include "input_check.h"
#include "include/HueLink.h"
#include <chrono>


using json = nlohmann::json;
using namespace std;



void print_help() {
    cout
            << "Usage:  hue_talk -h | --help  : Print usage\n"
               "        hue_talk <IP address> : IP address is required for connection.\n"
               "                                IP address must be first argument\n"
               "        Options:\n"
               "        -p <port number>      : Designate a port number for the connection\n"
               "                              : Default port number is 80\n"
               "        -u <username>         : Designate a username for the connection\n"
               "                              : If no username is provided, the Link-Button will be needed\n"
               "\n"
               "Example usage:    hue_talk 192.168.2.1 -p 80\n"
               "\n"
               "       After connection the initial state of lights is displayed and then changes are noted every second.\n"
            << endl;
}



int main(int argc, char *argv[]) {
    // create variables
    string ip_address;
    int port = 80; //default value
    string username;

    //acceptable number of arguments
    if (argc == 2 || argc == 4 || argc == 6) {
        if(string(argv[1]) == "-h" || string(argv[1]) == "--help"){
            print_help();
            return EXIT_SUCCESS;
        }
        //check IP
        if (validateIP(argv[1])) {
            ip_address = argv[1];
        } else {
            cout << argv[1] << " is not a valid IP address.\n"
                               "Try 'hue_talk --help' for more info" << endl;

            return EXIT_FAILURE;
        }
        //check for port and username inputs
        if (argc >= 4){
            if (string(argv[2]) == "-p"){
                if(validatePort(argv[3])){
                    port = stoi(argv[3]);
                }else{
                    cout << argv[3] << " is not a valid port number.\n"
                                       "Try 'hue_talk --help' for more info" << endl;
                    return EXIT_FAILURE;
                }
            }else if(string(argv[2]) == "-u"){
                if(validateUn(argv[3])){
                    username = string(argv[3]);
                }else{
                    cout << argv[3] << " is not a valid username - username must not have spaces.\n"
                                       "Try 'hue_talk --help' for more info" << endl;
                    return EXIT_FAILURE;
                }
            }else{
                cout <<"Invalid input.\n"
                       "Try 'hue_talk --help' for more info" << endl;
                return EXIT_FAILURE;
            }
            if(argc == 6){
                if(string(argv[2]) == "-u" && string(argv[4]) == "-p"){
                    if(validatePort(argv[5])){
                        port = stoi(argv[5]);
                    }else{
                        cout << argv[5] << " is not a valid port number.\n"
                                           "Try 'hue_talk --help' for more info" << endl;
                        return EXIT_FAILURE;
                    }
                }else if(string(argv[2]) == "-p" && string(argv[4]) == "-u"){
                    if(validateUn(argv[5])){
                        username = string(argv[5]);
                    }else{
                        cout << argv[5] << " is not a valid username - username must not have spaces.\n"
                                           "Try 'hue_talk --help' for more info" << endl;
                        return EXIT_FAILURE;
                    }
                }else{
                    cout <<"Invalid input.\n"
                           "Try 'hue_talk --help' for more info" << endl;
                    return EXIT_FAILURE;
                }
            }
        }


    }
    //if inputs are invalid, note and exit
    else {
        cout <<"Invalid input.\n"
               "Try 'hue_talk --help' for more info" << endl;
        return EXIT_FAILURE;
    }

    //create the link object for making API requests
    HueLink link(ip_address, port, username);
    cout << "Looking for Hue Lights..." << endl;

    //get and display an initial set of all the lights
    json lights;
    while(lights.empty()){
        int result = link.getAllLights_wrapper(lights);
        if (result != 0) return EXIT_FAILURE;
    }
    cout << lights.dump(2) << endl;

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
    //loop and check every second to see if there were any updates
    while(true){
        this_thread::sleep_for(chrono::milliseconds(1000) );
        json changes;
        int result =  link.lightsUpdate(lights, changes);
        if (result != 0) return EXIT_FAILURE;
        if (!changes.is_null()) cout << changes.dump(2) << endl;
    }
#pragma clang diagnostic pop



    return 0;
}

