#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>

using namespace std;

map<string, vector<string>> inbox;
vector<string> users;

string getParam(string req, string key) {
    size_t pos = req.find(key + "=");
    if (pos == string::npos) return "";
    pos += key.length() + 1;
    size_t end = req.find("&", pos);
    return req.substr(pos, end - pos);
}

string httpResponse(string body) {
    return "HTTP/1.1 200 OK\r\n"
           "Content-Type: text/plain\r\n"
           "Access-Control-Allow-Origin: *\r\n\r\n" + body;
}

int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;

    int port = atoi(getenv("PORT"));   // IMPORTANT for Render
    addr.sin_port = htons(port);

    bind(server_fd, (sockaddr*)&addr, sizeof(addr));
    listen(server_fd, 10);

    cout << "Chat server running on port " << port << endl;

    while (true) {
        int client = accept(server_fd, NULL, NULL);
        char buffer[4096] = {0};
        read(client, buffer, 4096);
        string req(buffer);

        string response;

        if (req.find("/join") != string::npos) {
            string name = getParam(req, "name");
            users.push_back(name);
            inbox[name];
            response = httpResponse("Joined: " + name);
        }
        else if (req.find("/send") != string::npos) {
            string name = getParam(req, "name");
            string msg = getParam(req, "msg");

            for (string u : users) {
                inbox[u].push_back(name + ": " + msg);
            }
            response = httpResponse("Message sent");
        }
        else if (req.find("/poll") != string::npos) {
            string name = getParam(req, "name");
            string out;
            for (string m : inbox[name]) out += m + "\n";
            inbox[name].clear();
            response = httpResponse(out.empty() ? "No new messages" : out);
        }
        else if (req.find("/users") != string::npos) {
            string out;
            for (string u : users) out += u + "\n";
            response = httpResponse(out);
        }
        else {
            response = httpResponse("Chat server online");
        }

        send(client, response.c_str(), response.size(), 0);
        close(client);
    }
}
