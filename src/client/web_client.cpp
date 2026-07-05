#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"
#include <iostream>

int main(int argc, char **argv)
{
    httplib::SSLClient cli(argv[1]);
    cli.set_follow_location(true);

    auto res = cli.Get("/");
    if (res)
    {
        std::cout << res->status << std::endl; // 200
        std::cout << res->body << std::endl;   // First 100 chars of the HTML can be limited by res->body.subr(start, end)
    }
    else
    {
        std::cout << "Error: " << httplib::to_string(res.error()) << std::endl;
    }
}