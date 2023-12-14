#pragma once

#include "bridge.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <asm/types.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <linux/if_arp.h>
#include <iostream>
#include <vector>
#include <map>


namespace netlink_class{


    const int  BUF_SIZE = 16192;

using namespace bridge_class;

    class NetLink{
    public:
        NetLink();

        ~NetLink(){
            close(sockfd);
        }

        const std::vector<Bridge> & ShowBridges();

        int AddBridge(const std::string& bridge_name);
        int AddBridgeIf(const std::string& bridge_name, uint16_t bridge_index, uint32_t master_index);

        int DelBridge(int bridge_index, const std::string& bridge_name);
        int DelBridgeIf(int bridge_index, const std::string& bridge_name, uint32_t master_index);
    
    private:

        std::string get_stp_state(const char* bridge_name);
        std::string get_bridge_priority(const char* bridge_name);

        
        int sockfd; //keep soket for all operations and close in detructor

        std::vector<Bridge> bridges;
        std::map<int, std::string> bridges_tree;

    };

}