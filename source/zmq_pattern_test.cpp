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

		for (int i = 1; i < 5; ++i)
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
			}
		}
		zmq_close(socket);
		zmq_ctx_term(ctx);
	}
}
TEST(ZeroMQ, ZMQ_PATTERN)
{
	boost::thread thrd(&PushFun);
	boost::thread thrd1(&PullFun, 1);
	boost::thread thrd2(&PullFun, 2);
	boost::thread thrd3(&PullFun, 3);
	
	Sleep(1000);
	stop = true;
	thrd.join();
	thrd1.join();
	thrd2.join();
	thrd3.join();
}