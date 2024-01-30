brcontrol is clone of Linux CLT brctl utility, but use Netlink for the work

Target operation system: Linux


Build:
Make biuld directory
cd ./build
cmake ..
cmake --build .

or just use command line 
g++ -std=c++17 main.cpp netlink.cpp -o bridgecontrol

Usage

Usage: ./bridgecontrol [commands]
commands:
addbr           <bridge>                add bridge
delbr           <bridge>                delete bridge
addif           <bridge> <device>       add interface to bridge
delif           <bridge> <device>       delete interface from bridge
show            [ <bridge> ]            show a list of bridges



