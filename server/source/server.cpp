#include <iostream>
#include <fstream>
#include <drogon/drogon.h>

using namespace drogon;

typedef std::function<void(const HttpResponsePtr&)> Callback;

void root_handler(const HttpRequestPtr& request, Callback&& callback) {
    std::cout << request->getBody() << std::endl;
    std::cout << "=======================================\n";
    std::cout << request->body() << std::endl;
    std::cout << "=======================================\n";
    std::cout << request->bodyData() << std::endl;
    std::ofstream outf("req.png", std::ios::out | std::ios::binary | std::ios::app);
    auto data = request->body();
    outf.write(data.data(), sizeof data.data());
    outf.close();

    Json::Value jsonBody;
    jsonBody["message"] = "Hello, world";

    auto response = HttpResponse::newHttpJsonResponse(jsonBody);
    callback(response);
}

int main()
{
    app()
        .registerHandler("/", &root_handler, { Get })
        .addListener("127.0.0.1", 8000)
        .setThreadNum(1)
        .enableServerHeader(false)
        .run();
    return EXIT_SUCCESS;
}