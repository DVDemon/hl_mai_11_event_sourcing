#include <iostream>
#include <httplib.h>
#include <string>

#include <Poco/Data/MySQL/Connector.h>
#include <Poco/Data/MySQL/MySQLException.h>
#include <Poco/Data/SessionFactory.h>
#include <Poco/Data/SessionPool.h>
#include <Poco/Data/Transaction.h>

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

    srv.Get(R"(/request/(\d+))", [&pool](const httplib::Request &req, httplib::Response &res) {
        auto numbers = req.matches[1];
        std::string result;
        result = "{'id': '";
        result += numbers;
        result += "'}";
        std::cout << result << std::endl;

        Poco::Data::Session session(pool.get());
        Poco::Data::Transaction tr(session);

        int id = atoi(std::string(numbers).c_str());
        std::string new_value = req.get_param_value("value");
        std::string old_value;
        ;
        int version{0};

        try
        {
            Poco::Data::Statement select(session);
            select << "SELECT value, version FROM Entity WHERE id = ?",
                Poco::Data::Keywords::into(old_value),
                Poco::Data::Keywords::into(version),
                Poco::Data::Keywords::use(id);

            if (!select.done())
                if (select.execute())
                {
                    version++;
                    Poco::Data::Statement update(session);
                    update << "UPDATE Entity SET version = ?, value = ? WHERE id = ?",
                        Poco::Data::Keywords::use(version),
                        Poco::Data::Keywords::use(new_value),
                        Poco::Data::Keywords::use(id);
                    update.execute();
                }
                else
                {
                    Poco::Data::Statement insert(session);
                    insert << "INSERT INTO Entity (id,value,version) VALUES(?,?, ?)",
                        Poco::Data::Keywords::use(id),
                        Poco::Data::Keywords::use(new_value),
                        Poco::Data::Keywords::use(version);
                    insert.execute();
                }

            result = "{'id': '";
            result += std::to_string(id);
            result += "', 'version': '";
            result += std::to_string(version);
            result += "', 'old_value': '";
            result += old_value;
            result += "', 'new_value': '";
            result += new_value;
            result += "'}";

            Poco::Data::Statement insert_event(session);
            insert_event << "INSERT INTO Event (entity_id,event_value) VALUES(?,?)",
                Poco::Data::Keywords::use(id),
                Poco::Data::Keywords::use(result);
            insert_event.execute();

            //throw std::logic_error("some exception");

            tr.commit();
        }
        catch (std::exception ex)
        {
            result = "exception";
            std::cout << ex.what() << std::endl;
        }


        res.set_content(result, "text/json; charset=utf-8");
    });

    srv.Get("/events", [&pool](const httplib::Request &req, httplib::Response &res) {
        int id_from = atoi(req.get_param_value("from").c_str());
        std::string result("{'events': [");       
        try
        {
            Poco::Data::Session session(pool.get());
            Poco::Data::Statement select(session);
            int id;
            int entity_id;
            std::string value;
            
            select << "SELECT id, entity_id, event_value FROM Event WHERE id >=?",
                Poco::Data::Keywords::into(id),
                Poco::Data::Keywords::into(entity_id),
                Poco::Data::Keywords::into(value),
                Poco::Data::Keywords::use(id_from),
                Poco::Data::Keywords::range(0,1);

            std::cout << "select events from " << id_from << std::endl;

            while(!select.done())
                if (select.execute())
                {
                    result += "{'id': '";
                    result += std::to_string(id);
                    result += "', 'event': ";
                    result += value;
                    result += "} ,";
                }
        }catch(std::exception ex)
        {
            std::cout << ex.what() << std::endl;
        }
        result += "]}";

        res.set_content(result, "text/json; charset=utf-8");
    });

    std::cout << "Event Sourcing Server started at 8080" << std::endl;
    srv.listen("0.0.0.0", 8080);

    return 0;
}