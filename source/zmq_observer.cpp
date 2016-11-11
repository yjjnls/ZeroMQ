#include <Common.h>


namespace
{

	std::string port = "8888";
	const std::string zeromq_server = std::string("tcp://127.0.0.1:") + port;
	bool stop = false;
	void PubFun()
	{
		int rc;
		void *ctx = zmq_ctx_new();
		ASSERT_TRUE(ctx != NULL);
		void *socket = zmq_socket(ctx, ZMQ_ROUTER);
		ASSERT_TRUE(socket != NULL);
		rc = zmq_bind(socket, zeromq_server.c_str());
		ASSERT_TRUE(rc == 0);

		zmq_msg_t id;
		zmq_msg_init(&id);
		zmq_msg_recv(&id, socket, 0);
		std::string id_copy;
		id_copy.append((char *)zmq_msg_data(&id), zmq_msg_size(&id));
		zmq_msg_close(&id);

		zmq_msg_t d;
		zmq_msg_init(&d);
		zmq_msg_recv(&d, socket, 0);
		zmq_msg_close(&d);

		zmq_msg_t tmp;
		zmq_msg_init(&tmp);
		zmq_msg_recv(&tmp, socket, 0);
		zmq_msg_close(&tmp);

		for (int i = 0; i < 10; ++i)
		{
			zmq_msg_t id;
			zmq_msg_init_size(&id, id_copy.size());
			memcpy(zmq_msg_data(&id), id_copy.c_str(), id_copy.size());
			zmq_msg_send(&id, socket, ZMQ_SNDMORE);
			zmq_msg_close(&id);

			zmq_msg_t d;
			zmq_msg_init(&d);
			zmq_msg_send(&d, socket, ZMQ_SNDMORE);
			zmq_msg_close(&d);

			zmq_msg_t data;
			zmq_msg_init_size(&data, 5);
			memcpy(zmq_msg_data(&data), "ABCDE", 5);
			zmq_msg_send(&data, socket, 0);
			zmq_msg_close(&data);
		}


		zmq_close(socket);
		zmq_ctx_term(ctx);

	}
	void SubFun()
	{
		int rc;
		void *ctx = zmq_ctx_new();
		ASSERT_TRUE(ctx != NULL);
		void *socket = zmq_socket(ctx, ZMQ_DEALER);
		ASSERT_TRUE(socket != NULL);
		rc = zmq_connect(socket, zeromq_server.c_str());
		ASSERT_TRUE(rc == 0);

		zmq_msg_t d;
		zmq_msg_init(&d);
		zmq_msg_send(&d, socket, 0);
		zmq_msg_close(&d);

		zmq_msg_t start;
		zmq_msg_init(&start);
		zmq_msg_send(&start, socket, 0);
		zmq_msg_close(&start);

		zmq_pollitem_t item[] = { socket, 0, ZMQ_POLLIN, 0 };
		while (!stop)
		{
			int len = zmq_poll(item, 1, 100);
			if (len==0)
			{
				continue;
			}
			zmq_msg_t delimiter;
			zmq_msg_init(&delimiter);
			zmq_msg_recv(&delimiter, socket, 0);
			zmq_msg_close(&delimiter);

			zmq_msg_t part;
			zmq_msg_init(&part);
			zmq_msg_recv(&part, socket, 0);
			std::string data;
			data.append((char *)zmq_msg_data(&part), zmq_msg_size(&part));
			zmq_msg_close(&part);

			std::cout << data << std::endl;
		}
		


		zmq_close(socket);
		zmq_ctx_term(ctx);
	}
}

/******************************************************

Use the ROUTER-DEALER pattern to implement a simple observer pattern.

The ROUTER ZeroMQ socket plays the role of publisher, and the DEALER 
ZeroMQ socket plays the role of subscriber. The DEALER socket first 
send a message to subscribe. And the ROUTER socket stores the id of 
every subscriber, to which the messages will be published to.
Most common observer patterns may be implemented using the PUB-SUB ZeroMQ socket. 
But different publishers share one url(socket) to publish different topics. 
The subscribers will receive all topics and must filter the topics and the 
sub-topics by themselves, which may be chaotic when the requirements are completed. 
In this pattern, different publishers are corresponding to different paths in the url. 
The ROUTER socket could classify different subscribers by their ids to the specific 
path(publisher). The the subscribers will only receive the topic they subscribed.

******************************************************/
TEST(ZeroMQ, ZMQ_BROKER)
{
	boost::thread thrd1(&PubFun);
	boost::thread thrd2(&SubFun);
	SLEEP(1000);
	stop = true;
	thrd1.join();
	thrd2.join();
}