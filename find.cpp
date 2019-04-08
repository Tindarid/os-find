#include <stdio.h>
#include <vector>
#include <string>
#include <stdexcept>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

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
        res.data = "Incorrect number of arguments";
        return res;
    }
    string incorrect = "Incorrect value: ";
    string for_flag = " for flag ";
    for (int i = 2; i < argc; i += 2) {
        string flag(argv[i]);
        string value(argv[i + 1]);
        if (flag == "-name") {
            res.wants_name = true;
            res.name = value;
            continue;
        }
        if (flag == "-exec") {
            res.wants_exec = true;
            res.exec_path = value;
            continue;
        }
        if (flag == "-inum") {
            res.wants_inum = true;
        } else if (flag == "-nlinks") {
            res.wants_nlinks = true;
        } else if (flag == "-size") {
            if (value.length() <= 1) {
                res.data = incorrect + value + for_flag + flag;
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
                    res.data = incorrect + value + for_flag + flag;
                    return res;
            }
            value = value.substr(1);
        } else {
            res.data = "Incorrect option: " + flag;
            return res;
        }
        auto temp = convert(value);
        if (temp.first == "good") {
            if (flag == "-inum") {
                res.inum = temp.second;
            } else if (flag == "-nlinks") {
                res.nlinks = temp.second;
            } else {
                res.size = temp.second;
            } 
        } else {
            res.data = incorrect + value + temp.first + for_flag + flag;
            return res;
        }
    }
    res.data = argv[1];
    res.good = true;
    return res;
}

void print_usage() {
    printf("Usage: find [path] [options]\n");
    printf("Description: finds list of files, which satisfy predicate\n");
    printf("Options list:\n");
    printf("-size   [[-=+]number] : size in bytes (lower, equal, greater)\n");
    printf("-nlinks [number]      : number of links allowed\n");
    printf("-inum   [number]      : number of inode\n");
    printf("-name   [string]      : name of file\n");
    printf("-exec   [path]        : path of executable, which will get output\n");
}

std::vector<string> resolve(const intent& user) {
    std::vector<string> res;
    DIR* dir = opendir(user.data.data());
    if (dir == NULL) {
        perror("");
        exit(EXIT_FAILURE);
    }
    while (true) {
        errno = 0;
        dirent* next = readdir(dir);
        if (next == NULL) {
            if (errno != 0) {
                perror("");
                exit(EXIT_FAILURE);
            }
            return res;
        }
        bool flag = true;
        string path(next->d_name);
        if (user.wants_inum) {
            flag &= next->d_ino == user.inum;
        }
        if (user.wants_name) {
            //flag &= path == user.name;
        }
        if (user.wants_nlinks) {
            //flag &= next->d_ino == user.inum;
        }
        switch (user.wants_size) {
            case lower:
                break;
            case equal:
                break;
            case greater:
                break;
            case no:
            default:
                break;
        }
        if (flag) {
            res.push_back(path);
        }
    }
}

int main(int argc, char *argv[]) {
    intent user = parse_args(argc, argv);
    if (!user.good) {
        printf("%s\n", user.data.data());
        print_usage();
        exit(EXIT_FAILURE);
    }
    auto list = resolve(user);
    if (user.wants_exec) {

    } else {
        for (auto&& entry : list) {
            printf("%s\n", entry.data());
        }
    }
    exit(EXIT_SUCCESS);
}
