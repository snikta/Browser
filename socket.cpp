#include <iostream>
#include <string>
#include "socket.io-client-cpp/src/sio_client.h"
#include "socket.io-client-cpp/src/sio_message.h"
#include "socket.io-client-cpp/src/sio_socket.h"

void OnMessage(sio::event& evt)
{
	std::cout << std::to_string(evt.get_message()->get_binary()->length()) << std::endl;
}

int main() {
	sio::client h;
	sio::message::list li("true");

	h.connect("http://192.168.0.17:8080");
	h.socket()->on("frame", &OnMessage);
	h.socket()->emit("frameRequest", li);
	
	return 0;
}