#pragma once
#include <string>
#include <vector>

namespace commands {

class ListGroup {
public:
    inline static const std::string flag = "lg,--list-group";
    static const inline std::vector<std::string> flags = {"list-group", "lg"};

    static void execute();
    void run();
};

} // namespace commands
