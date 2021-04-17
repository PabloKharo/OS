#include "zeromq.hpp"

using namespace std;

void SendMsg(void* socket, message& msg) {
    if (!zmq_send(socket, &msg, sizeof(message), 0)) {
        printf("[%d] ", getpid());
        perror("Error: SendMsg \n");
        exit(-1);
    }
}

bool GetMsg(void* socket, message& msg) {
    if (zmq_recv(socket, &msg, sizeof(message), 0) == -1) {
        return false;
    }
    return true;
}

void SetEndpoint(char* endp, int id, endpoint type) {
    string tmp;
    if (type == cl_left_pub) {
        tmp = "ipc:///tmp/zeromqlab/left_pub";
    } else if (type == cl_right_pub) {
        tmp = "ipc:///tmp/zeromqlab/right_pub";
    } else if (type == cl_parent_pub) {
        tmp = "ipc:///tmp/zeromqlab/par_pub";
    }
    strcpy(endp, (tmp+to_string(id)).c_str());
}

void ExeclClient(int id, int parent_id, char* sub_endpoint){
    char client[MAX_LEN] = "./client";
    execl(client, client, to_string(id).c_str(), to_string(parent_id).c_str(), sub_endpoint, NULL);
}


void SendSearchMsg(void* socket, int id, char* text, char* pattern){
    message msg(exec, id);
    msg.textsz = strlen(text)+1;
    msg.patternsz = strlen(pattern)+1;

    if (!zmq_send(socket, &msg, sizeof(message), ZMQ_SNDMORE)) {
        printf("[%d] ", getpid());
        perror("Error: SendSearchMsg \n");
        exit(-1);
    }

    if(!zmq_send(socket, text, msg.textsz * sizeof(char), ZMQ_SNDMORE)){
        printf("[%d] ", getpid());
        perror("Error: SendSearchMsg \n");
        exit(-1);
    }

    if(!zmq_send(socket, pattern, msg.patternsz * sizeof(char), 0)){
        printf("[%d] ", getpid());
        perror("Error: SendSearchMsg \n");
        exit(-1);
    }

}

bool GetSearchMsg(void* socket, int textsz, int patternsz, char* textstr, char* patternstr){
    if(zmq_recv(socket, textstr, textsz * sizeof(char), 0) == -1){
        return false;
    }
    if(zmq_recv(socket, patternstr, patternsz * sizeof(char), 0) == -1){
        return false;
    }

    return true;
}

void SendAnsExecMsg(void* socket, char* text){
    message msg(exec, 0);
    msg.textsz = strlen(text)+1;
    if (!zmq_send(socket, &msg, sizeof(message), ZMQ_SNDMORE)) {
        printf("[%d] ", getpid());
        perror("Error: SendAnsMsg \n");
        exit(-1);
    }
    if (!zmq_send(socket, text, msg.textsz, 0)) {
        printf("[%d] ", getpid());
        perror("Error: SendAnsMsg \n");
        exit(-1);
    }
}

bool GetAnsExecMsg(void* socket, int textsz, char* text){
    if(zmq_recv(socket, text, textsz * sizeof(char), 0) == -1){
        return false;
    }
    return true;
}


void* CreateContext() {
    void* context = zmq_ctx_new();
    if (context == NULL) {
        printf("[%d] ", getpid());
        perror("Error: CreateContext ");
        exit(-1);
    }
    return context;
}

void DisconnectSocket(void* socket, char* endpoint) {
    if (zmq_disconnect(socket, endpoint) != 0) {
        printf("[%d] ", getpid());
        perror("Error: DisconnectSocket ");
        exit(-1);
    }
}

void ConnectSocket(void* socket, char* endpoint) {
    if (zmq_connect(socket, endpoint) != 0) {
        printf("[%d] ", getpid());
        perror("Error: ConnectSocket ");
        exit(-1);
    }
}

void BindSocket(void* socket, char* endpoint) {
    if (zmq_bind(socket, endpoint) != 0) {
        printf("[%d] ", getpid());
        perror("Error: BindSocket ");
        exit(-1);
    }
}

void UnbindSocket(void* socket, char* endpoint) {
    if (zmq_unbind(socket, endpoint) != 0) {
        printf("[%d] ", getpid());
        perror("Error: UnbindSocket ");
        exit(-1);
    }
}

void* CreateSocket(void* context, int type) {
    void* socket = zmq_socket(context, type);
    if (socket == NULL) {
        printf("[%d] ", getpid());
        perror("Error: CreateSocket ");
        exit(-1);
    }
    if (type == ZMQ_SUB) {
        zmq_setsockopt(socket, ZMQ_SUBSCRIBE, 0, 0);
    }
    return socket;
}

void CloseSocket(void* socket) {
    if (zmq_close(socket) != 0) {
        printf("[%d] ", getpid());
        perror("Error: CloseSocket ");
        exit(-1);
    }
}

void DestroyContext(void* context) {
    if (zmq_ctx_destroy(context) != 0) {
        printf("[%d] ", getpid());
        perror("Error: DestroyContext ");
        exit(-1);        
    }
}



