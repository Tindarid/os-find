#include <iostream>
#include <vector>
#include <string>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

using std::vector;
using std::string;

enum size_intent {no, lower, equal, greater};

struct intent {
    bool wants_inum = false;      ino_t inum;
    bool wants_name = false;      string name;
    size_intent wants_size = no;  off_t size;
    bool wants_nlinks = false;    nlink_t nlinks;
    bool wants_exec = false;      string exec_path;
    bool good = false;            string data; 
};

std::pair<string, size_t> convert(const string& str) {
    try {
        return {"good", std::stoull(str)};
    } catch (std::invalid_argument &) {
        return {" is not a number", 0};
    } catch (std::out_of_range &) {
        return {" is too big", 0};
    }
}

intent parse_args(int argc, char *argv[]) {
    intent res;
    if (argc <= 1 || (argc - 2) % 2 != 0) {
        res.data = "Incorrect arguments";
        return res;
    }
    string incorrect = "Incorrect value: ";
    for (int i = 2; i < argc; i += 2) {
        string flag(argv[i]);
        string value(argv[i + 1]);
        if (flag == "-inum") {
            res.wants_inum = true;
            auto temp = convert(value);
            if (temp.first == "good") {
                res.inum = temp.second;
            } else {
                res.data = incorrect + value + temp.first;
                return res;
            }
        } else if (flag == "-name") {
            res.wants_name = true;
            res.name = value;
        } else if (flag == "-size") {
            if (value.length() <= 1) {
                res.data = incorrect + value;
                return res;
            }
            switch (value[0]) {
                case '-':
                    res.wants_size = lower;
                    break;
                case '=':
                    res.wants_size = equal;
                    break;
                case '+':
                    res.wants_size = greater;
                    break;
                default:
                    res.data = incorrect + value;
                    return res;
            }
            auto temp = convert(value.substr(1));
            if (temp.first == "good") {
                res.nlinks = temp.second;
            } else {
                res.data = incorrect + value + temp.first;
                return res;
            }
        } else if (flag == "-nlinks") {
            res.wants_nlinks = true;
            auto temp = convert(value);
            if (temp.first == "good") {
                res.nlinks = temp.second;
            } else {
                res.data = incorrect + value + temp.first;
                return res;
            }
        } else if (flag == "-exec") {
            res.wants_exec = true;
            res.exec_path = value;
        } else {
            res.data = "Incorrect option: " + flag;
            return res;
        }
    }
    res.data = argv[1];
    res.good = true;
    return res;
}

int main(int argc, char *argv[]) {
    intent user = parse_args(argc, argv);
    std::cout << user.data << std::endl;
}
