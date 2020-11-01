#include "console_connector.h"
#include "socket_connector.h"
#include "Solver.h"

#include <iostream>
#include <memory>
#include <sstream>
#include <vector>
#include <stdexcept>
#include <chrono>
#include <cstring>
#include <ctime>

using namespace std;

class client {
    std::chrono::duration<double> process_timeout_s;
    std::unique_ptr<connector> _connector;
    std::string remained_buffer;
    Solver your_solver;
public:
    client(std::unique_ptr<connector> conn, int process_timeout_ms, const char token[], int seed)
            : _connector(std::move(conn)), process_timeout_s(process_timeout_ms / 1000.) {
        if (!_connector->is_valid()) {
            std::cerr << "[main] " << "Not a valid connector" << std::endl;
            return;
        }

        std::vector<std::string> login_messages;
        std::stringstream ss;
        string command;
        ss << "START " << token << " " << seed;
        getline(ss, command);
        login_messages.push_back(command);
        ss.clear();

        login_messages.emplace_back(".");

        send_messages(login_messages);
    }

    void send_messages(const std::vector<std::string> &messages) {
        std::string message;

        for (const auto &i : messages) {
            message += i + '\n';
            cout << i << endl;
        }

        int sent_bytes = _connector->send(message.c_str(), message.size());

        if (sent_bytes != (int) message.size()) {
            std::cerr << "[main] " << "Warning: Cannot sent message properly: " << message << std::endl;
            std::cerr << "[main] " << sent_bytes << " byte sent from " <<
                      message.size() << ". Closing connection." << std::endl;
            _connector->invalidate();
        }
    }

    std::vector<std::string> receive_message() {
        std::vector<std::string> result;

        std::string curr_buffer;
        std::swap(curr_buffer, remained_buffer);
        while (true) {
            std::string line;
            std::stringstream consumer(curr_buffer);
            while (std::getline(consumer, line)) {
                if (line == ".") {
                    if (!consumer.eof()) { // eof = end of file
                        remained_buffer = consumer.str().substr(consumer.tellg());
                    }
                    result.emplace_back(".");
                    return result;
                } else if (!line.empty()) {
                    result.push_back(line);
                }
            }

            char array_buffer[512];

            int received_bytes = _connector->recv(array_buffer, 511);

            switch (received_bytes) {
                case -1:
                    std::cerr << "[main] " << "Error: recv failed!" << std::endl;
                case 0:
                    std::cerr << "[main] " << "Connection closed." << std::endl;
                    _connector->invalidate();
                    if (!result.empty()) {
                        std::cerr << "[main] " << "Latest message processing ..." << std::endl;
                    }
                    return result;
            }
            array_buffer[received_bytes] = '\0';
            if (!curr_buffer.empty() && curr_buffer.back() != '\n') {
                curr_buffer = result.back();
                result.pop_back();
            } else {
                curr_buffer.clear();
            }
            curr_buffer += array_buffer;
        }
    }

public:
    void run() {
        while (_connector->is_valid()) {
            auto measure_start = std::chrono::steady_clock::now();

            std::vector<std::string> tmp = receive_message();

            for (const auto &response : tmp) {
                cout << response << endl;
            }

            std::chrono::duration<double> read_seconds = std::chrono::steady_clock::now() - measure_start;
            if (read_seconds > process_timeout_s * 2) {
                std::cerr << "[main] " << "Read took: " << read_seconds.count() << " seconds (>"
                          << (process_timeout_s * 2).count() << "s)" << std::endl;
            }

            if (tmp.empty()) {
                continue;
            }

            std::clock_t measure_clock_start = std::clock();
            measure_start = std::chrono::steady_clock::now();

            tmp = your_solver.process(tmp);

            if (tmp.size() == 1 && tmp[0] == "Game over") {
                break;
            } else {
                if (!tmp.empty()) {

                    std::chrono::duration<double> process_seconds = std::chrono::steady_clock::now() - measure_start;
                    if (process_seconds > process_timeout_s) {
                        std::cerr << "[main] " << "Process took: " << process_seconds.count() << " seconds (>"
                                  << process_timeout_s.count() << "s)" << std::endl;
                        std::cerr << "[main] " << "CPU time used: "
                                  << 1.0 * (std::clock() - measure_clock_start) / CLOCKS_PER_SEC
                                  << std::endl;
                    }

                    if (!_connector->is_valid() || tmp.empty()) {
                        continue;
                    }

                    send_messages(tmp);

                    std::chrono::duration<double> process_with_send_seconds =
                            std::chrono::steady_clock::now() - measure_start;
                    if (process_seconds > process_timeout_s) {
                        std::cerr << "[main] " << "Process with send took: " << process_with_send_seconds.count()
                                  << " seconds (>"
                                  << process_timeout_s.count() << "s)" << std::endl;
                        std::cerr << "[main] " << "CPU time used: "
                                  << 1.0 * (std::clock() - measure_clock_start) / CLOCKS_PER_SEC
                                  << " sec" << std::endl;
                    }
                }
            }
        }
        std::cerr << "[main] " << "Game over" << std::endl;
    }
};

int main(int argc, char **argv) { // argc = argument count, argv = argument vector, char** = dinamikus string tömb
    const bool from_console = argc > 2 && 0 == std::strcmp("console", argv[2]) ||
                              (argc > 1 && 0 == std::strcmp("console",
                                                            argv[1])); // ha console-ról akar kommunikálni a client, akkor be kell írnia

    /* config area */
    const char host_name[] = "ericsson.dyndns.org";//"127.0.0.1";//;"nagyd.ddns.net";
    const unsigned short port = 11223; //; 1234;
    const char token[] = "Y6oosTdXL";
    int seed = 2;

    try {
        client(from_console ? static_cast<std::unique_ptr<connector>>(std::make_unique<console_connector>()) :
               std::make_unique<socket_connector>(host_name, port), from_console ? 200 : 2000, token, seed).run();
    } catch (std::exception &e) {
        std::cerr << "[main] " << "Exception throwed. what(): " << e.what() << std::endl;
    }
}
