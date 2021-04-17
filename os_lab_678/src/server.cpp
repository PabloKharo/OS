#include <iostream>
#include <string.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include "zmq.hpp"

#include "zeromq.hpp"

using namespace std;

void* context;
void* pub;
void* sub;

char pub_endpoint[MAX_LEN] = "ipc:///tmp/zeromqlab/serv_pub";
char sub_endpoint[MAX_LEN];

bool has_client;
int client_id;
int server_pid;

void Init()
{
    context = CreateContext();

    pub = CreateSocket(context, ZMQ_PUB);
    ConnectSocket(pub, pub_endpoint);

    sub = CreateSocket(context, ZMQ_SUB);
    int timewait = 1000;
    zmq_setsockopt(sub, ZMQ_RCVTIMEO, &timewait, sizeof(timewait));
}


void Deinit() {
    DisconnectSocket(pub, pub_endpoint);
    CloseSocket(pub);

    if (has_client) {
        UnbindSocket(sub, sub_endpoint);
    }
    CloseSocket(sub);

    DestroyContext(context);
}

void TermAll() {
    if (has_client) {
        message msg(terminate_cl, -1);
        SendMsg(pub, msg);
    }
}

void sig_handler(int signal) {
	printf("[%d] Terminating server...\n", server_pid); 
    TermAll();
	Deinit();
	exit(0);
}

int PingClient(int id) {
    message sendmsg(ping_cl, id);
    SendMsg(pub, sendmsg);
    message msg;
    if (!GetMsg(sub, msg)) {
        return 0;
    }
    if (msg.id == 0 && msg.cmd == ping_cl) {
        return msg.pid;
    } 
    return 0;
}

void CreateClient(int id) {
    if (!has_client) {
        client_id = id;
        
        int fork_pid = fork();
        if (fork_pid < 0) {
            printf("[%d] Error: Unable to fork a child\n", server_pid);
            exit(-1);
        } else if (fork_pid == 0) {
            ExeclClient(id, 0, pub_endpoint);
        } else {
            SetEndpoint(sub_endpoint, id, cl_parent_pub);
            BindSocket(sub, sub_endpoint);
            sleep(1);
            int pinged_pid = PingClient(id);
            if (!pinged_pid) {
                printf("[%d] Error: client wasn't created\n", server_pid);
                UnbindSocket(sub, sub_endpoint);
            } else {
                printf("[%d] OK: %d\n", server_pid, pinged_pid);
                has_client = true;
            }
        }
    } else {
        if (PingClient(id)) {
            printf("[%d] Error: client already exist\n", server_pid);
        } else {
            message sendmsg(create_cl, id);
            SendMsg(pub, sendmsg);
            sleep(1);
            int pinged_pid = PingClient(id);
            if (!pinged_pid) {
                printf("[%d] Error: client wasn't created\n", server_pid);
            } else {
                printf("[%d] OK: %d\n", server_pid, pinged_pid);
            }
        }
    }
}

void RemoveClient(int id) {
    if (!PingClient(id)) {
        printf("[%d] Error: client not found\n", server_pid);
        return;
    }

    message sendmsg(remove_cl, id);
    SendMsg(pub, sendmsg);
    if (client_id == id) {
        DisconnectSocket(sub, sub_endpoint);
        has_client = false;
    }

    if (!PingClient(id)) {
        printf("[%d] OK\n", server_pid);
    } else {
        printf("[%d] Error: client wasn't removed\n", server_pid);
    }
}

void SearchPattern(int id) {
    if (!PingClient(id)) {
        printf("[%d] Error: client client not found\n", server_pid);
        return;
    }
    
    string text;
    string pattern;
    if(cin.peek() == '\n'){
        cin.ignore(1, '\n');
    }
    getline(cin, text);
    if(cin.peek() == '\n'){
        cin.ignore(1, '\n');
    }
    getline(cin, pattern);

    char textc[text.length()+1] = "";
    char patternc[pattern.length()+1] = "";
    
    strcpy(textc, text.c_str());
    strcpy(patternc, pattern.c_str());
    
    SendSearchMsg(pub, id, textc, patternc);

    message msg;
    if(!GetMsg(sub, msg)){
        printf("[%d] Error: client haven't responed\n", server_pid);
        return;
    }
    
    char ans[msg.textsz];

    if(!GetAnsExecMsg(sub, msg.textsz, ans)){
        printf("[%d] Error: client haven't responed\n", server_pid);
        return;
    }

    if(msg.textsz != 1)
        printf("[%d] OK:%d: %s\n", server_pid, id, ans);
    else
        printf("[%d] OK:%d: -1\n", server_pid, id);
}


void ClearInput()
{
    cin.ignore(64, '\n');
}


int main (int argc, char *argv[])  {
    if (signal(SIGINT, sig_handler) == SIG_ERR) {
        perror("ERROR signal ");
		return -1;
	}

    has_client = false;
    server_pid = getpid();
    printf("[%d] Starting server...\n", server_pid);
    Init();

    string cmd;
    int id;
    while(cin >> cmd >> id) {
        if (id < 1) {
            printf("[%d] Error: Invalid id\n", server_pid);
            continue;
        }
        if (cmd == "create") {
            CreateClient(id);
        }else if (cmd == "remove") {
            RemoveClient(id);
        } else if (cmd == "exec") {
            SearchPattern(id);
        } else if (cmd == "ping") {
            if (PingClient(id) == 0) {
                printf("[%d] Error: client not found\n", server_pid);
            } else {
                printf("[%d] OK: 1\n", server_pid);
            }
        } else  {
            printf("[%d] Error: Invalid command\n", server_pid);
            ClearInput();
        }
    }

	printf("[%d] Shutting down server...\n", server_pid);
    TermAll();
	Deinit();
	return 0;
}



