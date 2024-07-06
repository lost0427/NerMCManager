//  Subscribe to the 'status'
#include <string>
#include <iostream>
#include <vector>

#include <zmq.hpp>
#include <zmq_addon.hpp>

int main()
{
    // initialize the zmq context with a single IO thread
    zmq::context_t context{1};

    zmq::socket_t client{context, zmq::socket_type::req};
    client.connect("tcp://localhost:6223");
    
    client.send(zmq::str_buffer("status"), zmq::send_flags::none);

    client.close();

    return 0;
}
