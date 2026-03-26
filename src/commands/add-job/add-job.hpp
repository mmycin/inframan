#pragma once
#include <string>
#include <vector>

using namespace std;

namespace commands {

class AddJob {
public:
    inline static const string flag = "aj,--add-job";
    static const inline vector<string> flags = {"add-job", "aj"};

    static void execute();
    void run();

private:
    string group_name;
    string job_name;
    string file_path;
    string command;

    void selectGroup();
    void promptFields();
    void validate();
};

} // namespace commands
