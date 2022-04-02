#include <iostream>
#include <httplib.h>
#include <string>
auto main() -> int
{

    httplib::Server srv;

    srv.Get(R"(/request/(\d+))", [](const httplib::Request &req, httplib::Response &res) {
        auto numbers = req.matches[1];
        std::string result;
        result = "{'id': '";
        result += numbers;
        result +="'}";
        res.set_content(result, "text/json; charset=utf-8");
    });

    srv.Get("/events", [](const httplib::Request &req, httplib::Response &res) {
        std::string id_from = req.get_param_value("from");
        std::string id_to = req.get_param_value("to");

        res.set_content("result", "text/json; charset=utf-8");
    });

    std::cout << "Event Sourcing Server started at 8080" << std::endl;
    srv.listen("0.0.0.0", 8080);

    return 0;
}