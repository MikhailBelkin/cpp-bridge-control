#pragma once

#include <string>


namespace bridge_class{

    class Bridge{
    public:
        Bridge(){}


        void SetName(const std::string& name){
            name_=name;
        }


        void SetStp(const std::string& stp){
            stp_=stp;
        }

        void SetInterface(const std::string& interface){
            interface_ = interface;
        }

        const std::string& GetName() const{
            return name_;
        }

        std::string GetFullId() const{
            return priority_+std::string(".")+id_;
        }

        const std::string& GetStp() const{
            return stp_;
        }

        const std::string& GetInterface() const{
            return interface_;
        }

        void SetIndex(int index){
            index_=index;
        }

        int GetIndex() const{
            return index_;
        }

        void SetBridgedTo(int bridged_to){
            bridged_to_=bridged_to;
        }
        int GetBridgedTo() const{
            return bridged_to_;
        }

        void SetPriority(const std::string& priority){
            priority_=priority;
        }

        const std::string& GetPriority() const{
            return priority_;
        }

        void SetMac(const std::string& mac){
            mac_=mac;
        }

        void SetId(const std::string& id){
            id_=id;
        }

        const std::string& GetId() const {
            return id_;
        }

    private:
        std::string name_={};
        std::string id_={};
        int bridged_to_=0;
        std::string stp_={};
        std::string interface_={};
        int index_ = 0;
        std::string mac_={};
        std::string priority_={};
        



    };

}