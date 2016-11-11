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

## ZMQ_OBSERVER_TEST
Use the **ROUTER-DEALER** pattern to implement a simple observer pattern.

The **ROUTER ZeroMQ socket** plays the role of publisher, and the **DEALER ZeroMQ socket** plays the role of subscriber. The **DEALER** socket first send a message to subscribe. And the **ROUTER** socket stores the **id** of every subscriber, to which the messages will be published to. 
Most common observer patterns may be implemented using the **PUB-SUB ZeroMQ socket**. **But different publishers share one url(socket) to publish different topics. The subscribers will receive all topics and must filter the topics and the sub-topics by themselves, which may be chaotic when the requirements are completed. In this pattern, different publishers are corresponding to different paths in the url. The ROUTER socket could classify different subscribers by their ids to the specific path(publisher). The the subscribers will only receive the topic they subscribed.**




