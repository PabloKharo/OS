#include <iostream>
#include <string>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include "zmq.hpp"

#include "zeromq.hpp"

using namespace std;

void* context;
void* sub;
void* parent_pub;
void* left_pub;
void* right_pub;

char parent_sub_end[MAX_LEN];
char left_sub_end[MAX_LEN];
char right_sub_end[MAX_LEN];

char parent_pub_end[MAX_LEN];
char left_pub_end[MAX_LEN];
char right_pub_end[MAX_LEN];

int parent_id;
int client_pid;
int client_id;

bool has_left;
bool has_right;


void Init() {
	SetEndpoint(left_pub_end, client_id, cl_left_pub);
    SetEndpoint(right_pub_end, client_id, cl_right_pub);
	SetEndpoint(parent_pub_end, client_id, cl_parent_pub);

    context = CreateContext();

    sub = CreateSocket(context, ZMQ_SUB);
    BindSocket(sub, parent_sub_end);

    parent_pub = CreateSocket(context, ZMQ_PUB);
    ConnectSocket(parent_pub, parent_pub_end);

    right_pub = CreateSocket(context, ZMQ_PUB);
    ConnectSocket(right_pub, right_pub_end);

    left_pub = CreateSocket(context, ZMQ_PUB);
    ConnectSocket(left_pub, left_pub_end);
}

void Deinit() {
	UnbindSocket(sub, parent_sub_end);
	if (has_left) {
		UnbindSocket(sub, left_sub_end);
	}
	if (has_right) {
		UnbindSocket(sub, right_sub_end);
	}
	CloseSocket(sub);

	ConnectSocket(parent_pub, parent_pub_end);
	ConnectSocket(left_pub, left_pub_end);
	ConnectSocket(right_pub, right_pub_end);
	
	CloseSocket(parent_pub);
	CloseSocket(left_pub);
	CloseSocket(right_pub);

	DestroyContext(context);
}

void sig_handler(int signal) {
	Deinit();
	exit(0);
}

void CreateClient(int id) {
	int fork_pid = fork();
	if (fork_pid < 0) {
		printf("[%d] Error: Unable to fork a child\n", client_pid);
		exit(-1);
	} else if (fork_pid == 0) {
		if (id < client_id) {
			ExeclClient(id, client_id, left_pub_end);
		} else {
			ExeclClient(id, client_id, right_pub_end);
		}
	} else {
		if (id < client_id) {
			SetEndpoint(left_sub_end, id, cl_parent_pub);
			BindSocket(sub, left_sub_end);
			has_left = true;
		} else {
			SetEndpoint(right_sub_end, id, cl_parent_pub);
			BindSocket(sub, right_sub_end);
			has_right = true;
		}
	}
}

void SearchPattern(message& msg){
	char text[msg.textsz] = "";
	char pattern[msg.textsz] = "";

	GetSearchMsg(sub, msg.textsz, msg.textsz, text, pattern);
	if(msg.id != client_id){
		if (msg.id < client_id) {
			SendSearchMsg(left_pub, msg.id, text, pattern);
		} else {
			SendSearchMsg(right_pub, msg.id, text, pattern);
		}
		return;
	}

	string ans;
	for (int i = 0; i <= msg.textsz - msg.patternsz; i++)
    {
		int j = 0;
        for (j = 0; j < msg.patternsz - 1; j++){
            if (text[i + j] != pattern[j])
                break;
		}

        if (j == msg.patternsz - 1)
            ans += to_string(i) + "; ";
    }

	char ansc[ans.length()+1] = "";
	strcpy(ansc, ans.c_str());
	SendAnsExecMsg(parent_pub, ansc);
}

void RemoveSub(int id) {
	if (id < client_id) {
		has_left = false;
		UnbindSocket(sub, left_sub_end);
	} else {
		has_right = false;
		UnbindSocket(sub, right_sub_end);
	}
}

int main(int argc, char *argv[])  {
	if (signal(SIGTERM, sig_handler) == SIG_ERR) {
		printf("[%d] ", getpid());
        perror("Error signal ");
		return -1;
	}

	client_pid = getpid();
	client_id = atoi(argv[1]);
	parent_id = atoi(argv[2]);
	strcpy(parent_sub_end, argv[3]);
	
	Init();
	
	while(true) {
		message msg;
		GetMsg(sub, msg);
		if (msg.id == 0) {
			if(msg.cmd == exec){
				char ans[msg.textsz];
				GetAnsExecMsg(sub, msg.textsz, ans);
				SendAnsExecMsg(parent_pub, ans);
			}else{
				SendMsg(parent_pub, msg);
			}
		} else if (msg.id != client_id && msg.cmd != create_cl 
					&& msg.cmd != terminate_cl && msg.cmd != exec) {
			if (has_left && msg.id < client_id) {
				SendMsg(left_pub, msg);
			} else if(has_right && msg.id > client_id) {
				SendMsg(right_pub, msg);
			}
		} else if (msg.cmd == create_cl) {
			if (has_left && msg.id < client_id) {
				SendMsg(left_pub, msg);
			} else if (has_right && msg.id > client_id) {
				SendMsg(right_pub, msg);
			} else {
				CreateClient(msg.id);
			}
		} else if (msg.cmd == remove_cl) {
			if (parent_id != 0) {
				msg.cmd = remove_sub;
				msg.id = parent_id;
				msg.pid = client_id;
				SendMsg(parent_pub, msg);
			}
			msg.cmd = terminate_cl;
			SendMsg(left_pub, msg);
			SendMsg(right_pub, msg);
			raise(SIGTERM);
		} else if (msg.cmd == terminate_cl) {
			SendMsg(left_pub, msg);
			SendMsg(right_pub, msg);
			raise(SIGTERM);
		} else if (msg.cmd == ping_cl) {		
			msg.id = 0;
			msg.pid = client_pid;
			SendMsg(parent_pub, msg);
		} else if (msg.cmd == remove_sub) {
            RemoveSub(msg.pid);
		} else if(msg.cmd == exec){
			SearchPattern(msg);
		} 
	}

	Deinit();
	return 0;
}