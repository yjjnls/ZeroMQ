# ZeroMQ
Test the properties of ZeroMQ and use it to do something.

## ZMQ_SOCKET_TEST
Test the bind and connect pattern of **ZeroMQ socket**.

One **ZeroMQ socket** can be bind or connect to different urls.  
One url can be connceted by multiple **ZeroMQ sockets**.  
But one url can only be bind with only one **ZeroMQ socket**.  


## ZMQ_MSG_TEST
Use struct **zmq_msg_t** to send and recvive messages in multiple parts.

The number of message parts the server received is corresponding to that the client sent. The function **zmq_msg_recv** will automatically adjust the size of **zmq_msg_t** to receive an intact message part.  

## ZMQ_PATTERN_TEST
Test the **round-robin** strategy of **PUSH/REQ ZeroMQ socket**.  

The speed of **ZeroMQ socket** sending messages is much faster than it establishing connections. Hence, a synchronization mechanism must be used to ensure all the connections have been established before sending messages. Otherwise, int the **PUSH-PULL/REQ-REP** pattern, messages will be blocked in the queue and then round-robined in part of the established sockets, sometimes only one. While in the **PUB-SUB** pattern, messages will be discarded until the connections established. Then the messages will be sending in the strategy of **fan-out**.
**PUSH_PULL** is taken for an instance, so is **REQ-REP**. You could modify to test the **REQ-REP**, or even **PUB-SUB**.



