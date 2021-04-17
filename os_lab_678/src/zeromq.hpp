#pragma once

#include <iostream>
#include <string>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include "zmq.hpp"

#define MAX_LEN 64

enum command{
    create_cl,
    remove_cl,
    terminate_cl,
    exec,
    ping_cl,
    remove_sub
};


struct message
{
    command cmd;
    int id;
    int pid;
    int textsz;
    int patternsz;

    message(command cmd_, int id_){
        cmd = cmd_;
        id = id_;
    }
    message(){
        id = -1;
    }
};


enum endpoint{
    cl_left_pub,
    cl_right_pub,
    cl_parent_pub
};

void SendMsg(void* socket, message& msg);
bool GetMsg(void* socket, message& msg);
void SetEndpoint(char* endp, int id, endpoint type);
void ExeclClient(int id, int parent_id, char* sub_endpoint);

void SendSearchMsg(void* socket, int id, char* text, char* pattern);
bool GetSearchMsg(void* socket, int textsz, int patternsz, char* text, char* pattern);
void SendAnsExecMsg(void* socket, char* text);
bool GetAnsExecMsg(void* socket, int textsz, char* text);


void* CreateContext();
void DisconnectSocket(void* socket, char* endpoint);
void ConnectSocket(void* socket, char* endpoint);
void BindSocket(void* socket, char* endpoint);
void UnbindSocket(void* socket, char* endpoint);
void* CreateSocket(void* context, int type);
void CloseSocket(void* socket);
void DestroyContext(void* context);

