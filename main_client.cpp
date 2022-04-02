#include <iostream>

//#define CPPHTTPLIB_OPENSSL_SUPPORT
#include <httplib.h>
#include <string>

auto main() -> int
{
    httplib::Client cli("localhost", 8080);

    auto res = cli.Get("/events?from=1");

    if (res)
    {
        if(res->status == 200)
            std::cout << res->body << std::endl;
    }
    else
    {
        std::cout << "error: " << httplib::to_string(res.error()) << std::endl;
    }

    return 0;
}