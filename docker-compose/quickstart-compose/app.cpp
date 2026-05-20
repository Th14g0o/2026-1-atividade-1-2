#include <crow.h>
#include <sw/redis++/redis++.h>
#include <cstdlib>
#include <string>

int main() {
    crow::SimpleApp app;

    // Lê variáveis de ambiente
    std::string redis_host = std::getenv("REDIS_HOST")
        ? std::getenv("REDIS_HOST")
        : "redis";

    int redis_port = std::getenv("REDIS_PORT")
        ? std::stoi(std::getenv("REDIS_PORT"))
        : 6379;

    // Conexão Redis
    sw::redis::ConnectionOptions opts;
    opts.host = redis_host;
    opts.port = redis_port;

    sw::redis::Redis redis(opts);

    CROW_ROUTE(app, "/")([&redis]() {
        long long count = redis.incr("hits");

        return "Hello from Docker! I have been seen " +
               std::to_string(count) +
               " time(s).\n";
    });

    app.port(5000).multithreaded().run();

    return 0;
}