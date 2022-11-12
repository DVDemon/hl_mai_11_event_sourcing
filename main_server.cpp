#include <iostream>
#include <httplib.h>
#include <string>

#include <Poco/Data/MySQL/Connector.h>
#include <Poco/Data/MySQL/MySQLException.h>
#include <Poco/Data/SessionFactory.h>
#include <Poco/Data/SessionPool.h>
#include <Poco/Data/Transaction.h>
#include <Poco/Data/RecordSet.h>

#include <Poco/JSON/Object.h>
#include <Poco/JSON/Array.h>
#include <Poco/JSON/Parser.h>
#include <Poco/Dynamic/Var.h>

auto main() -> int
{
    httplib::Server srv;
    Poco::Data::MySQL::Connector::registerConnector();
    std::string connection_str;
    connection_str = "host=127.0.0.1;user=stud;db=stud;password=stud";

    Poco::Data::SessionPool pool("MySQL", connection_str);

    srv.Post("/entity", [&pool](const httplib::Request &req, httplib::Response &res)
             {
                if(!req.has_param("entity_id")||!req.has_param("value")) return 404;


                std::string entity_id = req.get_param_value("entity_id");
                std::string value = req.get_param_value("value");
                try
                {
                    Poco::Data::Session session(pool.get());
                    Poco::Data::Transaction tr(session);

                    Poco::Data::Statement insert(session);
                    insert << "INSERT INTO Event (entity_id, value) VALUES (?,?)",
                        Poco::Data::Keywords::use(entity_id),
                        Poco::Data::Keywords::use(value);
                    insert.execute();
                    tr.commit();
                }
                catch (std::exception ex)
                {
                    std::cout << ex.what() << std::endl;
                    return 404;
                }
                res.set_content("{ \'result\' : \'ok\'", "text/json; charset=utf-8");
        return 200; });

    srv.Get(R"(/entity/(\d+))", [&pool](const httplib::Request &req, httplib::Response &res)
            {
                std::string entity_id = req.matches[1];     
                try
                {
                    Poco::Data::Session session(pool.get());
                    Poco::Data::Statement select(session);
                    int id;
                    std::string value;
                    
                    select << "SELECT id, value FROM Event WHERE entity_id = ? ORDER BY id DESC",
                        Poco::Data::Keywords::into(id),
                        Poco::Data::Keywords::into(value),
                        Poco::Data::Keywords::use(entity_id),
                        Poco::Data::Keywords::range(0,1);

                    select.execute();
                    Poco::Data::RecordSet rs(select);
                    if (!rs.moveFirst()) throw std::logic_error("not found");

                    Poco::JSON::Object obj;
                    obj.set("id",id);
                    obj.set("value",value);
                    std::stringstream ss;
                    Poco::JSON::Stringifier::stringify(obj,ss);
                    res.set_content(ss.str(), "text/json; charset=utf-8");
                    
                    return 200;
                        
                }catch(std::exception ex)
                {
                    std::cout << ex.what() << std::endl;
                    return 404;
                }

        return 404; });

    std::cout << "Event Sourcing Server started at 8080" << std::endl;
    srv.listen("0.0.0.0", 8080);

    return 0;
}