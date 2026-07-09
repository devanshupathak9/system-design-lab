# Load Balancer

Load balancer is used to balance the load between multiple server instances. It distributes the traffic to so the server doesn't overload and can scale for users.

## L4 (Transport layer) vs L7 (Application layer)

### L4 (TCP/UDP): 
- It makes decision using only TCP/UDP information, Its doesn't peek into the data.

## Algorithms
### Static algorithms (decide without looking at server state)
1. **Round Robin**
- cycle through servers 1, 2, 3, 1, 2, 3
- Best to be used when all the servers are identical.

2. **Weighted Round Robin**
- same, but a server with weight 3 gets 3× the traffic.
- Used when your server has heterogeneous hardware.

### Dynamic algorithms (use live server state)
1. Least Connection Algorithm
-  send the request to the server with the fewest active connections.

2. Least response time
3. 


## Health Checks

When the server goes down, the load balancer ensures that the request doesn't go to that server.
To ensure that, Load balancer used health check features to monitor the system using health check request.