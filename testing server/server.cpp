#include <iostream>
#include "httplib.h"

int main() {
    httplib::Server server;

    server.Get("/", [](const httplib::Request& req,httplib::Response& res){res.set_content("Hello from C++ server!", "text/plain");});

    std::cout << "Server running on http://localhost:6767\n";

    server.listen("localhost", 6767);

    return 0;
}
