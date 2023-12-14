#include "netlink.h"
#include <map>

using namespace std::literals::string_literals;
//using namespace netlink_class;

enum BrCtlCommands{
    SHOW,
    ADDBR,
    DELBR,
    DELIF,
    ADDIF
};

class BrCtl_command{
public:
    BrCtl_command(std::string& command, const std::string& param, const std::string& param1):
                command_(command), param_(param), param1_(param1){}
    void Proceed();
    void BrShow();
    void BrAdd(const std::string& arg);
    void BrAddIf(const std::string& arg, const std::string& arg1);
    void BrDelIf(const std::string& arg, const std::string& arg1);
    void BrDel(const std::string& arg);


private:
    std::string command_;
    std::string param_;
    std::string param1_;
    netlink_class::NetLink nl_;

};


void BrCtl_command::Proceed(){

        const std::map<std::string, BrCtlCommands> commands{ {"show", SHOW}, {"addbr", ADDBR}, {"delbr", DELBR}, {"delif", DELIF}, {"addif", ADDIF}  };

        if (commands.count(command_)==0){
            std::cerr<<"Unknown command " <<command_<< std::endl;
            return;
        }
        auto cmd = commands.at(command_);

        switch(cmd){

                case SHOW:
                    BrShow();
                    break;
                case ADDBR:
                    if (param_==""s){
                        std::cerr<<"Bridge name parameter missing"<<std::endl;
                    }
                    BrAdd(param_);
                    break;
                case DELBR:
                    BrDel(param_);
                    break;
                case DELIF:
                    BrDelIf(param_, param1_);
                    break;
                case ADDIF:
                    BrAddIf(param_, param1_);
                    break;


        }

        
}

void BrCtl_command::BrShow(){
        auto bridges = nl_.ShowBridges();
        std::cout<<"Bridge name\tbridge id\tSTP enabled\tinterfaces"<<std::endl;
        for(auto bridge: bridges){
            if (bridge.GetPriority()!=""s){ //if true bridge
                    std::cout<<bridge.GetName()<<"\t"
                    <<bridge.GetFullId()<<"\t"
                    <<bridge.GetStp()<<"\t"
                    <<bridge.GetInterface()<<//"\t"
                    //<<bridge.GetIndex()<<
                    std::endl;
            }
        }
    }

    void BrCtl_command::BrAdd(const std::string& arg){
        int res = nl_.AddBridge(arg);

        if (res){
            std::cerr<<"Can't add this bridge"<<std::endl;
            return;
        }


    }


    void BrCtl_command::BrAddIf(const std::string& arg, const std::string& arg1){
        

        auto bridges = nl_.ShowBridges();
        
        enum DelStatus{ UNKNOWN, ERROR, UPDATED };
        DelStatus stat = UNKNOWN;
        uint32_t master_index = 0;

        for (auto bridge: bridges){
               if (bridge.GetName()==arg1){ //interface found
                master_index=bridge.GetIndex();

               }
        }

        if(!master_index){
            std::cerr<<"Interface "<<arg1<<" not found" <<std::endl;
            return;
        }
        
        for (auto bridge: bridges){
            
            if (bridge.GetName()==arg){
                int bridge_index = bridge.GetIndex();
                if (bridge_index<=0){
                    stat = ERROR;
                }else
                if (nl_.AddBridgeIf(arg, bridge_index, master_index)){
                    
                    stat = ERROR;
                }
                else{
                    stat = UPDATED;
                }
            }

        }

        switch (stat){
        case UNKNOWN:
            std::cerr<<"Bridge "<<arg<< "  wasn't updates because wasn't found"<<std::endl;
            break;

        case ERROR:
            std::cerr<<"Bridge "<<arg<< "  wasn't updated because network error"<<std::endl;
            break;

        case UPDATED:
            std::cerr<<"Bridge "<<arg<< " was succesfully updates"<<std::endl;
            break;


        default:
            break;
        }

}




void BrCtl_command::BrDelIf(const std::string& arg, const std::string& arg1){
        
        auto bridges = nl_.ShowBridges();
        
        enum DelStatus{ UNKNOWN, ERROR, DELETED };
        DelStatus stat = UNKNOWN;
        uint32_t master_index = 0;

        for (auto bridge: bridges){
               if (bridge.GetName()==arg1){ //interface found
                master_index=bridge.GetIndex();

               }
        }

        if(!master_index){
            std::cerr<<"Interface "<<arg1<<" not found" <<std::endl;
            return;
        }
        
        for (auto bridge: bridges){
            
            if (bridge.GetName()==arg){
                int bridge_index = bridge.GetIndex();
                if (bridge_index<=0){
                    stat = ERROR;
                }else
                if (nl_.DelBridgeIf(bridge_index, arg, master_index)){
                    
                    stat = ERROR;
                }
                else{
                    stat = DELETED;
                }
            }

        }

        switch (stat){
        case UNKNOWN:
            std::cerr<<"Bridge "<<arg<< "  wasn't deleted because wasn't found"<<std::endl;
            break;

        case ERROR:
            std::cerr<<"Bridge "<<arg<< "  wasn't deleted because network error"<<std::endl;
            break;

        case DELETED:
            std::cerr<<"Bridge "<<arg<< " was succesfully deleted"<<std::endl;
            break;


        default:
            break;
        }

}

void BrCtl_command::BrDel(const std::string& arg){
        auto bridges = nl_.ShowBridges();
        enum DelStatus{ UNKNOWN, ERROR, DELETED };
        DelStatus stat = UNKNOWN;
        for (auto bridge: bridges){
            if (bridge.GetName()==arg){
                int bridge_index = bridge.GetIndex();
                if (bridge_index<=0){
                    stat = UNKNOWN;
                }else
                if (nl_.DelBridge(bridge_index, arg)){
                    stat = ERROR;
                }
                else{
                    stat = DELETED;
                }
            }

        }

        switch (stat){
        case UNKNOWN:
            std::cerr<<"Bridge "<<arg<< "  wasn't deleted because wasn't found"<<std::endl;
            break;

        case ERROR:
            std::cerr<<"Bridge "<<arg<< "  wasn't deleted because network error"<<std::endl;
            break;

        case DELETED:
            std::cerr<<"Bridge "<<arg<< " was succesfully deleted"<<std::endl;
            break;


        default:
            break;
        }
        
}



int main(int argc, char*argv[]){

    std::string param={}; 
    std::string param1={};
    std::string command={};
    if (argc < 2) {
        std::cerr << "Usage: "<< argv[0] << " [commands]" << std::endl;
        std::cerr<<"commands:"<<std::endl;
	    std::cerr<<"addbr     	<bridge>		add bridge"<<std::endl;
	    std::cerr<<"delbr     	<bridge>		delete bridge"<<std::endl;
    	std::cerr<<"addif     	<bridge> <device>	add interface to bridge"<<std::endl;
	    std::cerr<<"delif     	<bridge> <device>	delete interface from bridge"<<std::endl;
	    std::cerr<<"show      	[ <bridge> ]		show a list of bridges"<<std::endl;
        return 1;
    }
    command=argv[1];
    if (argc>=3){
        param=argv[2];
    }
    if (argc==4){
        param1=argv[3];
    }

    BrCtl_command brctl_exec(command, param, param1);
    brctl_exec.Proceed();

    return 0;
}