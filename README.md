# ZeroMQ
Test the properties of ZeroMQ and use it to do something.


#ZMQ_SOCKET_TEST
Test the bind and connect pattern of ZeroMQ socket.

One ZeroMQ socket can be bind or connect to different urls.
One url can be connceted by multiple ZeroMQ sockets.
But one url can only be bind with only one ZeroMQ socket.


#ZMQ_MSG_TEST
Use struct zmq_msg_t to send and recvive messages in multiple parts.

The number of message parts the server received is corresponding to that the client sent. 
The function zmq_msg_recv will automatically adjust the size of zmq_msg_t to receive an intact message part.


