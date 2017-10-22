#pragma once

#include "config.h"


//The main work is done in this class
//It process the commands from the client
//and controls the execution of the music
class Manager {
public:
    //Create/destroy pipes for the client-server comunication
    //and checks if there are other daemon already running
    Manager(int argc, char* argv[]);
    ~Manager();

    //Start the main loop
    void StartServer();
private:
    //template <typename T>
    //void ProcessCommand(T& proto, CommandControler& cmd);

    Config conf;

    //FIXME: It can not be a data member because when
    //daemonizing OpenAL library crashes.
    //Music music;
};
