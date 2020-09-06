1. The server starts and loads the configuration file myrout.cnf
[The conf-file format to be determined, the loading part written in the code]
root@u:~/dv_algorithm# ./server 
Info: success to load config file.
server recv from:127.0.0.1:58754, msg len:2
[join 70]client0 join...
  0   7   3   5 255 255 
  7   0   3 255   4 255 
  3   3   0   4   7 255 
  5 255   4   0   7   9 
255   4   8   7   0   2 
255 255 255   9   2   0 


2. Client start, send join request, the server received json, send table to client

[join 94]Info: send route infor to client0; [127.0.0.1:38292]

table:
  0   7   3   5 255 255 
  7   0   3 255   4 255 
  3   3   0   4   7 255 
  5 255   4   0   7   9 
255   4   8   7   0   2 
255 255 255   9   2   0 


3. Server send the route table about the myroute.cnf to client
4. Client updates local routing information, afetr the local routing information update, send the information to server, server send data to other cilent

node0 start
[join_reuqest 101]update: Node0 Sending  cost update to  Node1 :	  0   7   3   5 255 255
[join_reuqest 101]update: Node0 Sending  cost update to  Node2 :	  0   7   3   5 255 255
[join_reuqest 101]update: Node0 Sending  cost update to  Node3 :	  0   7   3   5 255 255
[join_reuqest 101]update: Node0 Sending  cost update to  Node4 :	  0   7   3   5 255 255
[join_reuqest 101]update: Node0 Sending  cost update to  Node5 :	  0   7   3   5 255 255

node1 start
[join_reuqest 102]update: Node1 Sending  cost update to  Node0 :	  7   0   3 255   4 255
[join_reuqest 102]update: Node1 Sending  cost update to  Node2 :	  7   0   3 255   4 255
[join_reuqest 102]update: Node1 Sending  cost update to  Node3 :	  7   0   3 255   4 255
[join_reuqest 102]update: Node1 Sending  cost update to  Node4 :	  7   0   3 255   4 255
[join_reuqest 102]update: Node1 Sending  cost update to  Node5 :	  7   0   3 255   4 255

node2 start
[join_reuqest 102]update: Node2 Sending  cost update to  Node0 :	  3   3   0   4   7 255
[join_reuqest 102]update: Node2 Sending  cost update to  Node1 :	  3   3   0   4   7 255
[join_reuqest 102]update: Node2 Sending  cost update to  Node3 :	  3   3   0   4   7 255
[join_reuqest 102]update: Node2 Sending  cost update to  Node4 :	  3   3   0   4   7 255
[join_reuqest 102]update: Node2 Sending  cost update to  Node5 :	  3   3   0   4   7 255


node3 start
[join_reuqest 102]update: Node3 Sending  cost update to  Node0 :	  5 255   4   0   7 009
[join_reuqest 102]update: Node3 Sending  cost update to  Node1 :	  5 255   4   0   7 009
[join_reuqest 102]update: Node3 Sending  cost update to  Node2 :	  5 255   4   0   7 009
[join_reuqest 102]update: Node3 Sending  cost update to  Node4 :	  5 255   4   0   7 009
[join_reuqest 102]update: Node3 Sending  cost update to  Node5 :	  5 255   4   0   7 009

node4 start
[join_reuqest 102]update: Node4 Sending  cost update to  Node0 :	255   4   8   7   0 002
[join_reuqest 102]update: Node4 Sending  cost update to  Node1 :	255   4   8   7   0 002
[join_reuqest 102]update: Node4 Sending  cost update to  Node2 :	255   4   8   7   0 002
[join_reuqest 102]update: Node4 Sending  cost update to  Node3 :	255   4   8   7   0 002
[join_reuqest 102]update: Node4 Sending  cost update to  Node5 :	255   4   8   7   0 002

node5 start
[join_reuqest 102]update: Node5 Sending  cost update to  Node0 :	255 255 255   9   2 000
[join_reuqest 102]update: Node5 Sending  cost update to  Node1 :	255 255 255   9   2 000
[join_reuqest 102]update: Node5 Sending  cost update to  Node2 :	255 255 255   9   2 000
[join_reuqest 102]update: Node5 Sending  cost update to  Node3 :	255 255 255   9   2 000
[join_reuqest 102]update: Node5 Sending  cost update to  Node4 :	255 255 255   9   2 000


5. After receiving the information, the server will forward it to the online router. If the target server is not online, it will be discarded directly.
[join 94]Info: send route infor to client0; [127.0.0.1:55047]
server recv from:127.0.0.1:55047, msg len:9
Err:the dest id 1 not register in client array.Do nothing!
server recv from:127.0.0.1:55047, msg len:9
Err:the dest id 2 not register in client array.Do nothing!
server recv from:127.0.0.1:55047, msg len:9
Err:the dest id 3 not register in client array.Do nothing!
server recv from:127.0.0.1:55047, msg len:9
Err:the dest id 4 not register in client array.Do nothing!
server recv from:127.0.0.1:55047, msg len:9
Err:the dest id 5 not register in client array.Do nothing!



6. The following figure is the interactive information of node 0 and node 1

node 1 start first, then node 0 start

[join_reuqest 102]update: Node0 Sending  cost update to  Node1 :	  0   7   3   5 255 255  --Here routing information is sent to node 1
[join_reuqest 102]update: Node0 Sending  cost update to  Node2 :	  0   7   3   5 255 255
[join_reuqest 102]update: Node0 Sending  cost update to  Node3 :	  0   7   3   5 255 255
[join_reuqest 102]update: Node0 Sending  cost update to  Node4 :	  0   7   3   5 255 255
[join_reuqest 102]update: Node0 Sending  cost update to  Node5 :	  0   7   3   5 255 255


NODE 1:  Received cost update from Node 0:   0   7   3   5 255 255   -- Here received the update exchange information from node 0
NODE 1:  Sending  cost update to   Node 0:   0   0   0   0   0 000
NODE 1:  Sending  cost update to   Node 2:   0   0   0   0   0 000
NODE 1:  Sending  cost update to   Node 3:   0   0   0   0   0 000
NODE 1:  Sending  cost update to   Node 4:   0   0   0   0   0 000
NODE 1:  Sending  cost update to   Node 5:   0   0   0   0   0 000

