#ifndef _COMMON_H_
#define _COMMON_H_

#define UDP_SERVER_IP "127.0.0.1"
#define UDP_SERVER_PORT (5555)

#define ROUTE_CMD_JOIN (0x01)   //request join
#define ROUTE_CMD_UPDATE (0x02)
#define MEM_LEN_1024 (1024)
#define INF (0xff)
#define NODES (6)

#define WARN(fmt, ...) printf("[%s %d]"fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)
typedef unsigned char u8;
typedef unsigned short u16;

typedef struct{
	u8 cmd;
	u8 uid;
}client_join_req_t;

typedef struct{
	u8 cmd;
	u8 uid;
	u8 costs[NODES*NODES];
}client_join_rep_t;

typedef struct{
	u8 cmd;  
    u8 sourceid; 
    u8 destid; 
    u8 mincost[NODES]; 
}client_update_req_t;
#endif