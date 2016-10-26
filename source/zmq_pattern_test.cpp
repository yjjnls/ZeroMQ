#include <Common.h>

namespace
{
	bool stop = false;
	const std::string url = "tcp://127.0.0.1:5555";
	boost::mutex print_lock;
	void PushFun()
	{
		int rc;

		void* ctx = zmq_ctx_new();
		ASSERT_TRUE(ctx != NULL);

		void* socket = zmq_socket(ctx, ZMQ_PUSH);
		ASSERT_TRUE(socket != NULL);
		rc = zmq_bind(socket, url.c_str());
		ASSERT_EQ(rc, 0);
		//it's important to wait for all connections established
		SLEEP(1000);

		for (int i = 1; i < 10; ++i)
		{
			zmq_msg_t msg;
			zmq_msg_init_size(&msg, sizeof(char));
			memset(zmq_msg_data(&msg), '0'+i, sizeof(char));
			zmq_msg_send(&msg, socket, 0);
			zmq_msg_close(&msg);
		}

		zmq_close(socket);
		zmq_ctx_term(ctx);
	}

	void PullFun(int num)
	{
		int rc;

		void* ctx = zmq_ctx_new();
		ASSERT_TRUE(ctx != NULL);

		void* socket = zmq_socket(ctx, ZMQ_PULL);
		ASSERT_TRUE(socket != NULL);
		rc = zmq_connect(socket, url.c_str());
		ASSERT_EQ(rc, 0);
		//zmq_setsockopt(socket, ZMQ_SUBSCRIBE, "", 0);

		zmq_pollitem_t items[] = { socket, 0, ZMQ_POLLIN, 0 };
		while (!stop)
		{
			int len = zmq_poll(items, 1, 100);
			if (len == 0)
			{
				continue;
			}
			if (items[0].revents & ZMQ_POLLIN)
			{
				std::string result;
				zmq_msg_t msg;
				zmq_msg_init(&msg);
				zmq_msg_recv(&msg, socket, 0);
				result.append(static_cast<char*>(zmq_msg_data(&msg)), zmq_msg_size(&msg));
				print_lock.lock();
				std::cout << "PULL - " << num << " : " << result << std::endl;
				print_lock.unlock();
				zmq_msg_close(&msg);
			}
		}
		zmq_close(socket);
		zmq_ctx_term(ctx);
	}
}

/******************************************************

Test the round-robin strategy of PUSH/REQ ZEROMQ socket.
The speed of ZEROMQ socket sending messages is much faster 
than it establishing connections. Hence, a synchronization 
mechanism must be used to ensure all the connections have 
been established before sending messages. Otherwise, int 
the PUSH-PULL/REQ-REP pattern, messages will be blocked in
the queue and then round-robined in part of the established 
sockets, sometimes only one.

While in the PUB-SUB pattern, messages will be discarded 
until the connections established. Then the messages will
be sending in the strategy of fan-out.

PUSH_PULL is taken for an instance, so is REQ-REP.
You could modify to test the REQ-REP, or even PUB-SUB.

******************************************************/
TEST(ZeroMQ, ZMQ_PATTERN)
{
	
	boost::thread thrd1(&PullFun, 1);
	boost::thread thrd2(&PullFun, 2);
	boost::thread thrd3(&PullFun, 3);
	boost::thread thrd(&PushFun);
	
	SLEEP(5000);
	stop = true;
	thrd.join();
	thrd1.join();
	thrd2.join();
	thrd3.join();
}