#include "../include/httplib.h"
#include "../include/json.hpp"

#include "../include/picosha2.h"
#include "../include/json.hpp"
#include <ctime>
#include <fstream>
#include "../include/httplib.h"
using namespace std;
using json = nlohmann::json;

int main() {
    httplib::Server svr;

    // ---------- یک مسیر GET ساده ----------
    svr.Get("/health", [](const httplib::Request& req, httplib::Response& res) {
        json j = {{"ok", true}, {"message", "server is alive"}};
        res.set_content(j.dump(), "application/json");
        res.status = 200;
    });
 
    svr.listen("127.0.0.1", 47002);  // بلاک می‌شود تا سرور بسته شود
}