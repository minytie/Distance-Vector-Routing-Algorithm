#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>
#include <netinet/in.h>
#include <stdio.h>   
#include <stdio.h>   
#include <stdlib.h>   
#include <string.h>   
#include <ctype.h>   
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "common.h"

static u8 role = 0;  

int mincost[NODES][NODES] = {{0}}; 
int routecost[NODES][NODES] = {{0}}; 
int costs[NODES][NODES] = {{0}};  


void show_link_cost()
{
	int i, j;

	printf("table:\n");
	for(i = 0; i < NODES;i++){
		for(j = 0; j < NODES; j++){
			printf("%3d ", routecost[i][j]);
		}
		printf("\n");
	}
	printf("\n");
}

int send_raw(int sock, char *msg, int len)
{
    struct sockaddr_in sa;

    sa.sin_family = AF_INET;
    sa.sin_port = htons(UDP_SERVER_PORT);
    sa.sin_addr.s_addr = inet_addr(UDP_SERVER_IP);	
	return sendto(sock, msg, len, 0, (struct sockaddr *)&sa, sizeof(sa));
}

void join_reuqest(int sock, char* buff, int n)    
{   
    int i;
	int j;   

	//extra data from buff, and fill all message to  linkcost	
	if (n != sizeof(client_join_rep_t)){
		printf("Err: the len is error\n");
		return;
	}
	
	client_join_rep_t* rep = (client_join_rep_t*)buff;
	for(i = 0;i < NODES;i++){
		for(j = 0; j < NODES; j++){
			routecost[i][j] = rep->costs[i*NODES + j];
		}
	}
	
	show_link_cost();	
    for (j = 0; j < NODES; j++)   
    {   
        if (0 == j) 
			continue;
        for (i = 0; i<NODES; i++){   
            if (0 == i) 
				continue;   			
            if (i == j)   
                costs[i][j] = routecost[role][j];   
            else   
                costs[i][j] = INF;      
        }      
    }

    
    client_update_req_t senpkt;   
    senpkt.sourceid = role;   
    for (j = 0; j < NODES; j++)   
        senpkt.mincost[j] = mincost[role][j] = routecost[role][j];   


    for(j = 0; j < NODES; j ++){
		if (j == role)
			continue;
		senpkt.cmd = ROUTE_CMD_UPDATE;
		senpkt.destid = j; 		
		send_raw(sock, (char*)&senpkt, sizeof(client_update_req_t)); 
		WARN("update: Node%d Sending  cost update to  Node%d :\t", role, j);
		printf("%3d %3d %3d %3d %3d %03d\n", mincost[role][0], mincost[role][1], mincost[role][2], mincost[role][3], mincost[role][4], mincost[role][5]);		
	}   
}   

void update_request (int sock, char* buff, int n)   
{   
	int i = 0;
    int temp;   
    int update = 0;  

	client_update_req_t *updatepkt= (client_update_req_t*)buff;
    int sourceid = updatepkt->sourceid;   
   
    printf("NODE %d:  Received cost update from Node %d: %3d %3d %3d %3d %3d %3d\n",   
        role, sourceid, updatepkt->mincost[0], updatepkt->mincost[1],   
        updatepkt->mincost[2], updatepkt->mincost[3], updatepkt->mincost[4], updatepkt->mincost[5]);   

	/*exchange and start the distance vector compute */
	for(i = 0; i < NODES; i++){
		if (i == role)
			continue;
		
	    if (updatepkt->mincost[i] < INF){ 
	        costs[i][sourceid] = temp = routecost[role][sourceid] + updatepkt->mincost[i];   
	        if (mincost[role][i]>temp){  	   
	            update++;   
	            mincost[role][i] = temp;   	   
	        }      
	    }
	}

   	/*if the vector update, and send vector to server*/
    if (update > 0){    
        client_update_req_t senpkt;   
        senpkt.sourceid = role;   
        for (int i = 0; i < NODES; i++)   
            senpkt.mincost[i] = mincost[role][i];   

   		for(i = 0;i < NODES; i++){
			if (i == role)
				continue;
			senpkt.destid = i;
			send_raw(sock, (char*)&senpkt, sizeof(client_update_req_t));
			printf("NODE %d:  Sending  cost update to   Node %d: %3d %3d %3d %3d %3d %03d\n",   
            role, i, mincost[0][0], mincost[0][1], mincost[0][2], mincost[0][3], mincost[0][4],mincost[0][5]);   
			
		}       
    }   
    else{
		printf("NODE %d   No change in minimum costs.\n", role);   
	}   
}   
   
int main(int argc, char **argv)
{
	int sock = 0;
    char buff[MEM_LEN_1024] = { 0 };

	if(argc != 2){
		printf("Usage; client num_of_role[0-255].\n");
		return 0;
	}

	role =  atoi(argv[1]);
	if (role <0 ||role >255){
		printf("Usage; client num_of_role[0-255].\n");
		return 0;
	}
	
	sock = socket(AF_INET, SOCK_DGRAM, 0); 
	if (sock <= 0){
		printf("Err: failed to call socket.\n");
		return 0;
	}

	client_join_req_t req;
	req.cmd = ROUTE_CMD_JOIN;
	req.uid = role;	
	
	int n = send_raw(sock, (char*)&req, sizeof(req));
	if (n <= 0){
		WARN("Err: failed to send client request to client.\n");
		return 0;
	}else{
		WARN("Info: client%d send join to server.\n", role);
	}

	while(1){		
		struct sockaddr_in sa;		
	    char buff[MEM_LEN_1024] = {0};
	
		memset(buff, 0, sizeof(buff));
		memset((char*)&sa, 0, sizeof(struct sockaddr_in));
		socklen_t len = sizeof(struct sockaddr);

		sa.sin_family = AF_INET;
        sa.sin_port = htons(UDP_SERVER_PORT);
        sa.sin_addr.s_addr = inet_addr(UDP_SERVER_IP);			
		
		n = recvfrom(sock, buff, sizeof(buff), 0, (struct sockaddr*)&sa, &len);
		if (n <= 0){
			WARN("Err:recv from is err,please check! n=%d\n", n);
			return 0;
		}

		u8 cmd = buff[0];
		switch(cmd){
		case ROUTE_CMD_JOIN:
			join_reuqest(sock, buff, n);
			break;
		case ROUTE_CMD_UPDATE:						
			update_request(sock, buff, n);
			break;
		default:
			break;			
		}
	}
	printf("%d node exit.\n", role);    
    return 0;
}

