#include <Common.h>

/******************************************************

Test the bind and connect pattern of ZEROMQ socket.
REQ-REP is used in this test, so are PUSH-PULL and PUB-SUB.

******************************************************/
TEST(ZeroMQ, ZMQ_SOCKET)
{
	const std::string url1 = "tcp://127.0.0.1:5555";
	const std::string url2 = "tcp://127.0.0.1:5556";
	int rc;

	void* ctx = zmq_ctx_new();
	ASSERT_TRUE(ctx != NULL);
	
	void* server_socket1 = zmq_socket(ctx, ZMQ_REP);
	ASSERT_TRUE(server_socket1 != NULL);
	void* server_socket2 = zmq_socket(ctx, ZMQ_REP);
	ASSERT_TRUE(server_socket2 != NULL);
	void* client_socket1 = zmq_socket(ctx, ZMQ_REQ);
	ASSERT_TRUE(client_socket1 != NULL);
	void* client_socket2 = zmq_socket(ctx, ZMQ_REQ);
	ASSERT_TRUE(client_socket2 != NULL);

	//one socket can bind to different urls
	rc = zmq_bind(server_socket1, url1.c_str());
	ASSERT_EQ(rc, 0);
	rc = zmq_bind(server_socket1, url2.c_str());
	ASSERT_EQ(rc, 0);
	//the same url can't be bind more than once
	rc = zmq_bind(server_socket1, url1.c_str());
	ASSERT_EQ(rc, -1);
	rc = zmq_bind(server_socket2, url2.c_str());
	ASSERT_EQ(rc, -1);
	//one socket can connect to different urls
	rc = zmq_connect(client_socket1, url1.c_str());
	ASSERT_EQ(rc, 0);
	rc = zmq_connect(client_socket1, url2.c_str());
	ASSERT_EQ(rc, 0);
	//the same url can be connected more than once
	rc = zmq_connect(client_socket1, url1.c_str());
	ASSERT_EQ(rc, 0);
	rc = zmq_connect(client_socket2, url1.c_str());
	ASSERT_EQ(rc, 0);

	zmq_close(server_socket1);
	zmq_close(server_socket2);
	zmq_close(client_socket1);
	zmq_close(client_socket2);
	zmq_ctx_term(ctx);
}


namespace
{

	std::string port = "8888";
	const std::string zeromq_server = std::string("tcp://127.0.0.1:") + port;

	size_t msg_size[] = { 0, 1, 31, 32, 50, 100, 1000, 10000, 100000000 };

	void ServerFun()
	{
		int rc;
		void* ctx = zmq_ctx_new();
		ASSERT_TRUE(ctx != NULL);
		void* socket = zmq_socket(ctx, ZMQ_REP);
		ASSERT_TRUE(socket != NULL);
		rc = zmq_bind(socket, zeromq_server.c_str());
		ASSERT_TRUE(rc == 0);

		int i = 0;
		size_t msg_sz;
		int more = 0;
		size_t more_sz = sizeof(more);
		do
		{
			zmq_msg_t part;
			rc = zmq_msg_init(&part);
			ASSERT_TRUE(rc == 0);
			rc = zmq_msg_recv(&part, socket, 0);
			ASSERT_TRUE(rc != -1);

			msg_sz = zmq_msg_size(&part);
			ASSERT_TRUE(msg_size[i++] == msg_sz);

			rc = zmq_getsockopt(socket, ZMQ_RCVMORE, &more, &more_sz);
			ASSERT_TRUE(rc == 0);
			zmq_msg_close(&part);
		} while (more);

		zmq_close(socket);
		zmq_ctx_term(ctx);

	}
	void ClientFun()
	{
		int rc;
		void* ctx = zmq_ctx_new();
		ASSERT_TRUE(ctx != NULL);
		void* socket = zmq_socket(ctx, ZMQ_REQ);
		ASSERT_TRUE(socket != NULL);
		rc = zmq_connect(socket, zeromq_server.c_str());
		ASSERT_TRUE(rc == 0);

		size_t len = ARRAY_SIZE(msg_size);
		for (int i = 0; i < len; ++i)
		{
			zmq_msg_t msg;
			zmq_msg_init_size(&msg, msg_size[i]);
			memset(zmq_msg_data(&msg), 1, msg_size[i]);
			rc = zmq_msg_send(&msg, socket, (i < len - 1) ? ZMQ_SNDMORE : 0);
			ASSERT_TRUE(rc != -1);
			zmq_msg_close(&msg);
		}

		zmq_close(socket);
		zmq_ctx_term(ctx);
	}
}

/******************************************************

one zmq_msg_t sent, one zmq_msg_t received
the size of the received zmq_msg_t will be adjusted automatically

******************************************************/
TEST(ZeroMQ, ZMQ_MSG)
{
	boost::thread thrd1(&ServerFun);
	boost::thread thrd2(&ClientFun);
	thrd1.join();
	thrd2.join();
}