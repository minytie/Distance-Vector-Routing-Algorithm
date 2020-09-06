#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "common.h"

#define FILENAME "myroute.cnf"

typedef struct{
	u8 uid;
	char client_ip[20];
	u16 client_port;
}client_msg_t;

static client_msg_t client[NODES] = {0}; 
static int routecost[NODES][NODES] = {{0,   7,  3,  5, -1, -1},
									 {7,   0,  3, -1,  4, -1},
									 {3,   3,  0,  4,  7, -1},
									 {5,  -1,  4,  0,  7,  9},
									 {-1,  4,  8,  7,  0,  2},
									 {-1 ,-1, -1,  9,  2,  0}}; 

static int load_config(char* filename)
{
    int i = 0;
    char buff[128] = {0}; 

    FILE* fp = fopen(filename, "r");
    if (NULL == fp){
        printf("Err: failed to open filename\n");
        return 0;
    }
   
    while(!feof(fp)){
        if (!fgets(buff, sizeof(buff), fp)){
            break;
        }
        int a,b,c,d,e,f;
        sscanf(buff, "%d,%d,%d,%d,%d,%d", &routecost[i][0], &routecost[i][1], 
            &routecost[i][2], &routecost[i][3], 
            &routecost[i][4], &routecost[i][5]);        
        i++;
    }

    return 0;
}

void join(int sock, char* buff, int n, char* ip, u16 port)
{
	if (NULL == buff || n <= 0){
		WARN("Err: join param is err,please check! \n");
		return;
	}

	client_join_req_t* req = (client_join_req_t*)buff;
	if (req->uid >= NODES){
		WARN("Err: the uid %d is err, please check! \n", req->uid);
		return;
	}

	u8 uid = req->uid;
	client[uid].uid = uid;
	client[uid].client_port= port;
	strcpy(client[uid].client_ip, ip);
	WARN("client%d join...\n", uid);
	
	client_join_rep_t rep;
	rep.cmd = ROUTE_CMD_JOIN;
	rep.uid = uid;

	int i = 0;
	int j = 0;

	for(i = 0; i < NODES; i++){
		for(j = 0; j < NODES; j++){
			rep.costs[i*NODES + j] = routecost[i][j];
			printf("%3d ",  rep.costs[i*NODES + j]);
		}
		printf("\n");
	}

	struct sockaddr_in sa;
	memset(&sa, 0, sizeof(struct sockaddr_in));
	sa.sin_family = AF_INET;
    sa.sin_port = htons(port);
    sa.sin_addr.s_addr = inet_addr(ip);			
	
	sendto(sock, (char*)&rep, sizeof(client_join_rep_t), 0, (struct sockaddr*)&sa, sizeof(struct sockaddr));
	WARN("Info: send route infor to client%d; [%s:%d]\n", uid, ip, port);
	return;	
}

void update(int sock, char* buff, int n, char* ip, u16 port)
{
	client_update_req_t* rtpkt;
	if (NULL == buff || n <= 0){
		WARN("Err: update param is err,please check!\n");
		return;
	}

	rtpkt = (client_update_req_t*)buff;


	u8 sourceid = rtpkt->sourceid;
	u8 destid = rtpkt->destid;

	
	if (client[destid].uid!= destid){
		printf("Err:the dest id %d not register in client array.Do nothing!\n", destid);
		return;
	}

	struct sockaddr_in sa;
	memset(&sa, 0, sizeof(struct sockaddr_in));
	sa.sin_family = AF_INET;
    sa.sin_port = htons(client[destid].client_port);
    sa.sin_addr.s_addr = inet_addr(client[destid].client_ip);	
	
	sendto(sock, buff, n, 0, (struct sockaddr*)&sa, sizeof(struct sockaddr));	
	WARN("Info: update from client%d to client%d\n", sourceid, destid);
	return;
}
int main(int argc, char **argv)
{
	int n  = 0;
	int res = 0;
    int sock = 0;
	int len = 0;
    char buff[512] = { 0 };
	
	  
	struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(UDP_SERVER_PORT);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);


	res = load_config("rr");
	if (res < 0){		
		return 0;
	}

    sock = socket(AF_INET, SOCK_DGRAM, 0);
	
    if (sock < 0){
        perror("socket");
        exit(1);
    }
	
    if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0){
        perror("bind");
        exit(1);
    }

	
	   
    while (1){
		struct sockaddr_in sa; 
		len = sizeof(sa);   
		memset(&sa, 0, sizeof(struct sockaddr_in));
    	memset(buff, 0, sizeof(buff));
		
        n = recvfrom(sock, buff, sizeof(buff), 0, (struct sockaddr*)&sa, &len);
		if (n < 0){
			printf("Err: failed to call recvfrom, please check!\n");
			break;
		}      

       printf("server recv from:%s:%u, msg len:%d\n", inet_ntoa(sa.sin_addr), ntohs(sa.sin_port), n);
	  
	   u8 cmd = buff[0];
	   if (cmd == ROUTE_CMD_JOIN){
			join(sock, buff, n, inet_ntoa(sa.sin_addr), ntohs(sa.sin_port));
	   }else if (cmd == ROUTE_CMD_UPDATE){
		    update(sock, buff, n, inet_ntoa(sa.sin_addr), ntohs(sa.sin_port));
	   }else{
			;
	   }
	   
    }
    return 0;
}

