# Hue Talk - seeing what's up with our smart light bulb friends

This program integrates with the Philips Hue platform. The id, name, on status, and brightness for all lights are displayed. The program then runs continuously and displays any changes to the light’s status. 

## Usage:

Basic usage:
```sh
./hue_talk <IP address>
```

Full usage and options:

```sh
./hue_talk -h | --help  : Print usage
hue_talk <IP address>   : IP address is required for connection.
                        : IP address must be first argument
Options:
p <port number>         : Designate a port number for the connection
                        : Default port number is 80
-u <username>           : Designate a username for the connection
                        : If no username is provided, the Link-Button will be needed
 ```                             

Example usage:

```sh
$ ./hue_talk 192.168.0.2 -p 80 -u my_hue_username
```

## Steps to Install:
```sh
git clone ---recursive https://github.com/auman66/hue_talk
cd hue_talk && mkdir build && cd build
cmake ..
make
./hue_talk <IP address>
```

## Thank you to the coders who came before me

This program relies on two external packages:
- JSON for Modern C++: https://github.com/nlohmann/json
- C++ Requests: Curl for People, a spiritual port of Python Requests: https://github.com/whoshuu/cpr
