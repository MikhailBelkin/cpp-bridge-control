
#include "netlink.h"


namespace netlink_class{

NetLink::NetLink(){
    // create  NetLink socket
        sockaddr_nl src_addr;
        sockfd = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
        if (sockfd < 0) {
            std::cerr<<"Error creating socket" <<std::endl;
            throw std::runtime_error("Error creating socket");
        }

        src_addr = {};
        
        src_addr.nl_family = AF_NETLINK;
        src_addr.nl_pid = getpid(); // current process PID

        int retval = bind(sockfd, (struct sockaddr *)&src_addr, sizeof(src_addr));
        if (retval < 0) {
            std::cerr<<"Error binding to the socket"<<std::endl;
            close(sockfd);
            throw std::runtime_error("Error binding to the socket");
        }
        

    }



    int NetLink::DelBridge(int bridge_index, const std::string& bridge_name){
        struct NetLinkReq{
            nlmsghdr header;
            ifinfomsg msg;
            rtattr  rta;
            char data[4096];
        };

    
        int err = 0;
        sockaddr_nl sa = {0};
        iovec iov = {0};
        msghdr msg = {0};
        nlmsghdr *nh;
        NetLinkReq req = {0};


        // Fill NetLink headers for sending request 

        req.header.nlmsg_len = NLMSG_LENGTH(sizeof(ifinfomsg)+sizeof(rtattr)+bridge_name.size()+1);

        req.header.nlmsg_flags = NLM_F_REQUEST| NLM_F_ROOT;
        req.header.nlmsg_type = RTM_DELLINK;
        
        req.header.nlmsg_seq = 1;
        req.msg.ifi_family = AF_UNSPEC;
        
        

        req.msg.ifi_index = bridge_index;
        
        
        req.msg.ifi_change = 0xFFFFFFFF;

        sa.nl_family = AF_NETLINK;
        iov.iov_base = &req;
        iov.iov_len = req.header.nlmsg_len;
        msg.msg_name = &sa;
        msg.msg_namelen = sizeof sa;
        msg.msg_iov = &iov;
        msg.msg_iovlen = 1;

        req.rta.rta_type = IFLA_IFNAME;
        req.rta.rta_len = RTA_LENGTH(bridge_name.size()+1);

        
        strcpy(req.data, bridge_name.c_str()); 

        std::cout<<"Deleting bridge "<< bridge_name<<" index is "<<bridge_index<<std::endl;


        if (sendmsg(sockfd, &msg, 0) < 0){
            std::cerr<<"Can't send message to NetLink"<<std::endl;
            return -1;
        }


        


        return 0;



    }



    int NetLink::DelBridgeIf(int bridge_index, const std::string& bridge_name, uint32_t master_index){
  

        
        struct NetLinkReq{
            nlmsghdr header;
            ifinfomsg msg;
            rtattr  rta;
            uint32_t index;
            
        };
        

    
        
        sockaddr_nl sa = {0};
        iovec iov = {0};
        msghdr msg = {0};
        
        NetLinkReq req = {0};


        // Fill NetLink headers for sending request 


        req.header.nlmsg_len = NLMSG_LENGTH(sizeof(struct ifinfomsg)); 
        
        req.header.nlmsg_flags = NLM_F_REQUEST | NLM_F_ACK | NLM_F_CREATE| NLM_F_REPLACE;
        req.header.nlmsg_type = RTM_SETLINK;
        req.header.nlmsg_seq = 1;
        req.header.nlmsg_pid = 0;
        req.msg.ifi_family = AF_UNSPEC;
        req.msg.ifi_index = master_index;
        

        sa.nl_family = AF_NETLINK;
        sa.nl_groups = 	RTMGRP_LINK | RTMGRP_IPV4_IFADDR | RTMGRP_IPV4_ROUTE;	
	    

        
        sa.nl_pid = getpid();
        iov.iov_base = &req;
        iov.iov_len = req.header.nlmsg_len;
        msg.msg_name = &sa;
        msg.msg_namelen = sizeof sa;
        msg.msg_iov = &iov;
        msg.msg_iovlen = 1;

        req.rta.rta_type = IFLA_MASTER;
        req.rta.rta_len = RTA_LENGTH(sizeof(uint32_t));
        req.index = 0;
        
        req.header.nlmsg_len += NLMSG_LENGTH(sizeof(rtattr)+sizeof(uint32_t));


       if (send(sockfd, &req, req.header.nlmsg_len, MSG_CONFIRM)<0){
            std::cerr<<"Can't send message to NetLink"<<std::endl;
            return -1;
        }
        
        char buf[BUF_SIZE];
        if (recv(sockfd, buf, BUF_SIZE, 0) < 0) {
            perror("Failed to receive netlink acknowledgment");
            return -1;
        }

        struct nlmsghdr *msg_hdr = (struct nlmsghdr *)buf;
        if (msg_hdr->nlmsg_type == NLMSG_ERROR) {
            struct nlmsgerr *err = (struct nlmsgerr *)NLMSG_DATA(msg_hdr);
            if (msg_hdr->nlmsg_len < NLMSG_LENGTH(sizeof(struct nlmsgerr))) {
                std::cerr<<"Error message received"<<std::endl;
                return -1;
            }
            if (err->error == 0) {
                std::cout << "Operation succefully done" << std::endl;
            } else {
                std::cerr << "Operation error: " << strerror(-err->error) << std::endl;
                return -1;
            }
        } else if (msg_hdr->nlmsg_type == NLMSG_DONE) {
            std::cout << "Operation succefully done" << std::endl;
        } else {
            std::cerr << "Error message received" << std::endl;
            return -1;
        }

  

        return 0;


    }




    int NetLink::AddBridgeIf(const std::string& bridge_name, uint16_t bridge_index, uint32_t master_index){


        
        struct NetLinkReq{
            nlmsghdr header;
            ifinfomsg msg;
            rtattr  rta;
            uint32_t index;
            
        };
        

    
        
        sockaddr_nl sa = {0};
        iovec iov = {0};
        msghdr msg = {0};
        
        NetLinkReq req = {0};


        // Fill NetLink headers for sending request 


        req.header.nlmsg_len = NLMSG_LENGTH(sizeof(struct ifinfomsg)); 
        
        req.header.nlmsg_flags = NLM_F_REQUEST | NLM_F_ACK | NLM_F_CREATE| NLM_F_REPLACE;
        req.header.nlmsg_type = RTM_SETLINK;
        req.header.nlmsg_seq = 1;
        req.header.nlmsg_pid = 0;
        req.msg.ifi_family = AF_UNSPEC;
        req.msg.ifi_index = master_index;
        

        sa.nl_family = AF_NETLINK;
        sa.nl_groups = 	RTMGRP_LINK | RTMGRP_IPV4_IFADDR | RTMGRP_IPV4_ROUTE;	
	    

        
        sa.nl_pid = getpid();
        iov.iov_base = &req;
        iov.iov_len = req.header.nlmsg_len;
        msg.msg_name = &sa;
        msg.msg_namelen = sizeof sa;
        msg.msg_iov = &iov;
        msg.msg_iovlen = 1;



        req.rta.rta_type = IFLA_MASTER;
        req.rta.rta_len = RTA_LENGTH(sizeof(uint32_t));
        req.index = bridge_index;
        
        req.header.nlmsg_len += NLMSG_LENGTH(sizeof(rtattr)+sizeof(uint32_t));

//        std::cout<<"Adding bridge "<<bridge_name<< " data = '"<< bridge_index<<"'"<< master_index <<std::endl;

       if (send(sockfd, &req, req.header.nlmsg_len, MSG_CONFIRM)<0){
            std::cerr<<"Can't send message to NetLink"<<std::endl;
            return -1;
        }
        
        char buf[BUF_SIZE];
        if (recv(sockfd, buf, BUF_SIZE, 0) < 0) {
            perror("Failed to receive netlink acknowledgment");
            return -1;
        }

        struct nlmsghdr *msg_hdr = (struct nlmsghdr *)buf;
          if (msg_hdr->nlmsg_type == NLMSG_ERROR) {
            struct nlmsgerr *err = (struct nlmsgerr *)NLMSG_DATA(msg_hdr);
            if (msg_hdr->nlmsg_len < NLMSG_LENGTH(sizeof(struct nlmsgerr))) {
                std::cerr<<"Error message received"<<std::endl;
                return -1;
            }
            if (err->error == 0) {
                std::cout << "Operation succefully done" << std::endl;
            } else {
                std::cerr << "Operation error: " << strerror(-err->error) << std::endl;
                return -1;
            }
        } else if (msg_hdr->nlmsg_type == NLMSG_DONE) {
            std::cout << "Operation succefully done" << std::endl;
        } else {
            std::cerr << "Error message received" << std::endl;
            return -1;
        }

        return 0;

    }


    int NetLink::AddBridge(const std::string& bridge_name){


        
        struct NetLinkReq{
            nlmsghdr header;
            ifinfomsg msg;
            rtattr  rta;
            char data[16];
            
        };
        

    
        
        sockaddr_nl sa = {0};
        iovec iov = {0};
        msghdr msg = {0};
        
        NetLinkReq req = {0};


        // Fill NetLink headers for sending request 


        req.header.nlmsg_len = NLMSG_LENGTH(sizeof(struct ifinfomsg)); 
        
        req.header.nlmsg_flags = NLM_F_REQUEST | NLM_F_ACK | NLM_F_CREATE| NLM_F_REPLACE | NLM_F_APPEND;
        req.header.nlmsg_type = RTM_NEWLINK;
        req.header.nlmsg_seq = 1;
        req.header.nlmsg_pid = 0;
        req.msg.ifi_family = AF_UNSPEC;
        req.msg.ifi_index = 0;
        req.msg.ifi_type = ARPHRD_ETHER;


        sa.nl_family = AF_NETLINK;
        sa.nl_groups = 	RTMGRP_LINK | RTMGRP_IPV4_IFADDR | RTMGRP_IPV4_ROUTE;	
	    

        
        sa.nl_pid = getpid();
        iov.iov_base = &req;
        iov.iov_len = req.header.nlmsg_len;
        msg.msg_name = &sa;
        msg.msg_namelen = sizeof sa;
        msg.msg_iov = &iov;
        msg.msg_iovlen = 1;

        

        req.rta.rta_type = IFLA_IFNAME;
        req.rta.rta_len = RTA_LENGTH(bridge_name.size()+1);
        

        strcpy(req.data, bridge_name.c_str());
        
        req.header.nlmsg_len = NLMSG_LENGTH(sizeof(struct ifinfomsg)+sizeof(rtattr)+req.rta.rta_len); 


        //std::cout<<"Adding bridge "<<bridge_name<< " data = '"<< req.data<<"'"<<std::endl;
       if (send(sockfd, &req, req.header.nlmsg_len, MSG_CONFIRM)<0){
            std::cerr<<"Can't send message to NetLink"<<std::endl;
            return -1;
        }
        
        char buf[BUF_SIZE];
        if (recv(sockfd, buf, BUF_SIZE, 0) < 0) {
            perror("Failed to receive netlink acknowledgment");
            return -1;
        }

        struct nlmsghdr *msg_hdr = (struct nlmsghdr *)buf;
          if (msg_hdr->nlmsg_type == NLMSG_ERROR) {
            struct nlmsgerr *err = (struct nlmsgerr *)NLMSG_DATA(msg_hdr);
            if (msg_hdr->nlmsg_len < NLMSG_LENGTH(sizeof(struct nlmsgerr))) {
                std::cerr<<"Error message received"<<std::endl;
                return -1;
            }
            if (err->error == 0) {
                std::cout << "Operation succefully done" << std::endl;
            } else {
                std::cerr << "Operation error: " << strerror(-err->error) << std::endl;
                return -1;
            }
        } else if (msg_hdr->nlmsg_type == NLMSG_DONE) {
            std::cout << "Operation succefully done" << std::endl;
        } else {
            std::cerr << "Error message received" << std::endl;
            return -1;
        }

        return 0;

    }




    const std::vector<Bridge> & NetLink::ShowBridges(){

        struct NetLinkReq{
            nlmsghdr header;
            ifinfomsg msg;
        };

    
        char buf[BUF_SIZE] = {0};
        int err = 0;
        uint32_t master_idx = 0;
        sockaddr_nl sa = {0};
        iovec iov = {0};
        msghdr msg = {0};
        nlmsghdr *nh;
        NetLinkReq req = {0};


        // Fill NetLink headers for sending request 

        req.header.nlmsg_len = NLMSG_LENGTH(sizeof(struct ifinfomsg));
        req.header.nlmsg_flags = NLM_F_REQUEST | NLM_F_ROOT;
        req.header.nlmsg_type = RTM_GETLINK;
        req.header.nlmsg_seq = 1;
        req.msg.ifi_family = AF_UNSPEC;

        sa.nl_family = AF_NETLINK;
        iov.iov_base = &req;
        iov.iov_len = req.header.nlmsg_len;
        msg.msg_name = &sa;
        msg.msg_namelen = sizeof sa;
        msg.msg_iov = &iov;
        msg.msg_iovlen = 1;

        if (sendmsg(sockfd, &msg, 0) < 0){
            perror("Failed to sendmsg to netlink socket");
            return bridges;
        }
        
        //Fill NetLink headers for receiving info

        iov.iov_base = buf;
        iov.iov_len = sizeof buf;



        while (true){ //reading info from NetLink

            ssize_t len = recvmsg(sockfd, &msg, MSG_DONTWAIT);
            if (len < 0){ //try again if network error
                if (errno == EINTR || errno == EAGAIN){
                    
                    usleep(250000);
                    continue;
                }

                continue;
            }

            for (nh = (struct nlmsghdr *)buf; NLMSG_OK(nh, (__u32)len); nh = NLMSG_NEXT(nh, len)){
                
                size_t len;
                struct ifinfomsg *msg;
                struct rtattr *rta;

                if(nh->nlmsg_type == NLMSG_DONE){
                    

                    // all info readed, preparing result

                    for(auto &bridge: bridges){
                        if (bridge.GetBridgedTo()){
                            continue;
                        }
                        std::string priority = get_bridge_priority(bridge.GetName().c_str());
                        bridge.SetPriority(priority);
                        std::string stp = get_stp_state(bridge.GetName().c_str());
                        bridge.SetStp(stp);
                        
                        if (bridges_tree.count(bridge.GetIndex())!=0){
                            bridge.SetInterface(bridges_tree.at(bridge.GetIndex()));
                            
                        }

                    }

                    return bridges;

                }

                if(nh->nlmsg_type != RTM_BASE){
                    // igore if that is base message
                    continue;
                }

                msg = (struct ifinfomsg *)NLMSG_DATA(nh); // message payload

                if (msg->ifi_type != ARPHRD_ETHER){
                    // ignore if this is not from ethernet connection
                    continue;

                }


                master_idx = 0; //reset master index

                rta = IFLA_RTA(msg); // message attributes
                len = nh->nlmsg_len - NLMSG_LENGTH(sizeof *msg);

                for (; RTA_OK(rta, len); rta = RTA_NEXT(rta, len))
                {

                    struct ifinfomsg *iface;
                    struct nlattr * nla;
                    char *iface_name;
                    
                    std::string str_iface_name;
                    char mac_addr[32];
                    std::string str_mac;
                    iface = (struct ifinfomsg *)NLMSG_DATA(nh);
                    nla = (struct nlattr *)((char *)iface + NLMSG_ALIGN(sizeof(struct ifinfomsg)));
                    unsigned char *mac;
                    switch (rta->rta_type)
                    {
                        case IFLA_MASTER:
                            master_idx = *(__u16 *)((char *) rta + NLA_HDRLEN);
                            if (!bridges.empty()){
                                bridges.back().SetBridgedTo(master_idx);
                                bridges_tree[master_idx]=bridges.back().GetName();
                            }
                            
                            break;
                        case IFLA_IFNAME:
                            bridges.push_back(Bridge());
                            bridges.back().SetIndex(iface->ifi_index);
                            
                
                            str_iface_name = (char *)(((struct rtattr *)nla) + 1);
                            bridges.back().SetName(str_iface_name);
                            break;
                        case IFLA_ADDRESS:
                            mac = (unsigned char *)rta + NLA_HDRLEN;
                            sprintf(mac_addr, "%02x:%02x:%02x:%02x:%02x:%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
                            str_mac = mac_addr;

                            bridges.back().SetMac(str_mac);

                            sprintf(mac_addr, "%02x%02x%02x%02x%02x%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
                            str_mac = mac_addr;
                            bridges.back().SetId(str_mac);

                            break;

                        default:
                        break;
                    }

                }
            }
        }

    return bridges;

    }    




std::string NetLink::get_stp_state(const char* bridge_name){
    char stp_state_path[BUF_SIZE];
    char stp_state[BUF_SIZE];

    snprintf(stp_state_path, sizeof(stp_state_path), "/sys/class/net/%s/bridge/stp_state", bridge_name);

    FILE *fp = fopen(stp_state_path, "r");
    if (fp == NULL) {
        return {};
    }

    if (fgets(stp_state, sizeof(stp_state), fp) == NULL){
        fclose(fp);
        return {};
    }

    fclose(fp);
    
    std::string result = (stp_state[0]=='0')?"No":"Yes";
    return result;
}


std::string NetLink::get_bridge_priority(const char* bridge_name){
    
    char bridge_priority_path[BUF_SIZE];
    char bridge_priority[BUF_SIZE];

    snprintf(bridge_priority_path, sizeof(bridge_priority_path), "/sys/class/net/%s/bridge/priority", bridge_name);

    FILE *fp = fopen(bridge_priority_path, "r");
    if (fp == NULL) {
        return {};
    }

    if (fgets(bridge_priority, sizeof(bridge_priority), fp) == NULL) {
        fclose(fp);
        return {};
    }
    
    fclose(fp);
    bridge_priority[strlen(bridge_priority)-1]=0;
    
    
    std::string result = bridge_priority;
    if (!result.empty()){
        unsigned short prior = std::stoi(bridge_priority, 0, 10);
        sprintf(bridge_priority, "%04x", prior);
        result = bridge_priority;
    
    }
    else{
        return {};
    }

    return result;

    }   


}//namspace