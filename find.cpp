#include <iostream>
#include <vector>
#include <string>

using std::vector;
using std::string;

enum size_intent {no, lower, equal, greater};

struct intent {
    bool wants_inum = false;      size_t inum;
    bool wants_name = false;      string name;
    size_intent wants_size = no;  size_t size;
    bool wants_nlinks = false;    size_t nlinks;
    bool wants_exec = false;      string exec_path;
    bool good = false;            string path;
};

intent parse_args(int argc, char *argv[]) {
    intent res;
    if (argc <= 1 || (argc - 2) % 2 != 0) {
        return res;
    }
    res.path = argv[1];
    for (int i = 2; i < argc; i += 2) {
        string flag(argv[i]);
        string value(argv[i + 1]);
        if (flag == "-inum") {
            res.wants_inum = true;
        } else if (flag == "-name") {
            res.wants_name = true;
        } else if (flag == "-size") {
            //res.wants_size = true;
        } else if (flag == "-nlinks") {
            res.wants_nlinks = true;
        } else if (flag == "-exec") {
            res.wants_exec = true;
        } else {
            return res;
        }
    }
    res.good = true;
    return res;
}

int main(int argc, char *argv[]) {
    intent user = parse_args(argc, argv);
}
