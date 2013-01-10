#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <string>
#include <functional>
#include <sstream>
#include <cctype>
#include <boost/random/random_device.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <windows.h>

std::size_t length = 127;
const std::string
    class_alpha = "abcdefghijklmnopqrstuvwxyz",
    class_lalpha = "ABCDEFGHIJKLMNOPQRSTUVWXYZ",
    class_num = "0123456789",
    class_symbols = "!@#$%^&*()`~-_=+[{]{\\|;:'\",<.>/?";

std::set<const std::string*> ref_classes;
struct ref_classes_init_type{
    ref_classes_init_type(){
        ref_classes.insert(&class_alpha);
        ref_classes.insert(&class_lalpha);
        ref_classes.insert(&class_num);
    }
} ref_classes_init;

std::string passwd;
boost::random::random_device rng;

void help(){
    std::cout << "-------- srndgen --------\n";
    std::cout << "-h    : output this message.\n";
    std::cout << "-gen  : generate.\n";
    std::cout << "-cout : output to standerd c out.\n";
    std::cout << "-copy : output to clipboard.\n";
    std::cout << "-num  : set length. default = 127.\n";
    std::cout << "-a    : set classes, [a-z].\n";
    std::cout << "-A    :              [A-Z].\n";
    std::cout << "-0    :              [0-9].\n";
    std::cout << "-!    :              (SYMBOLS).\n";
    std::cout << "void  : '-gen -cout'." << std::endl;
}

void gen(){
    std::string chars;
    for(auto iter = ref_classes.cbegin(), end = ref_classes.cend(); iter != end; ++iter){
        const auto &char_class(**iter);
        chars += char_class;
    }
    boost::random::uniform_int_distribution<> index_dist(0, chars.size() - 1);
    passwd.clear();
    for(std::size_t i = 0; i < length; ++i){
        passwd += chars[index_dist(rng)];
    }
}

void cout(){
    std::cout << passwd << std::endl;
}

void copy(){
    HGLOBAL hmem = GlobalAlloc(GHND, passwd.size() + 1);
    LPTSTR pmem;
    pmem = (LPTSTR)GlobalLock(hmem);
    lstrcpy((LPTSTR)pmem, passwd.c_str());
    GlobalUnlock(hmem);
    OpenClipboard(nullptr);
    EmptyClipboard();
    SetClipboardData(CF_TEXT, hmem);
    CloseClipboard();
}

enum e{
    e_num,
    e_classes,
    e_other,
    e_non
};

e lex(std::string s){
    e r = e_non;
    const std::string classes = "aA0!";
    for(auto iter = s.cbegin(), end = s.cend(); iter != end; ++iter){
        char c = *iter;
        if(r == e_non){
            if(classes.find(c) != classes.npos){
                r = e_classes;
            }else if(std::isdigit(c)){
                r = e_num;
            }else{
                return e_other;
            }
        }else{
            if(r == e_classes && (classes.find(c) != classes.npos)){
                r = e_classes;
            }else if(r == e_num && std::isdigit(c)){
                r = e_num;
            }else{
                return e_other;
            }
        }
    }
    return r == e_non ? e_other : r;
}

void scan_commandline_arg(const std::vector<std::string> &arg_vec){
    std::map<std::string, std::function<void()>> fn_map;
    fn_map["-h"] = fn_map["--help"] = help;
    fn_map["-gen"] = gen;
    fn_map["-cout"] = cout;
    fn_map["-copy"] = copy;
    for(auto iter = arg_vec.cbegin(), end = arg_vec.cend(); iter != end; ++iter){
        const auto &arg(*iter);
        auto map_iter = fn_map.find(arg);
        if(map_iter != fn_map.end()){
            map_iter->second();
        }else{
            std::string str(arg.begin() + 1, arg.end());
            switch(lex(str)){
            case e_num:
                {
                    std::istringstream a(str);
                    a >> length;
                }
                break;

            case e_classes:
                ref_classes.clear();
                for(auto c_iter = str.cbegin(), c_end = str.cend(); c_iter != c_end; ++c_iter){
                    char c = *c_iter;
                    switch(c){
                    case 'a':
                        ref_classes.insert(&class_alpha);
                        break;

                    case 'A':
                        ref_classes.insert(&class_lalpha);
                        break;

                    case '0':
                        ref_classes.insert(&class_num);
                        break;

                    case '!':
                        ref_classes.insert(&class_symbols);
                        break;

                    default: break;
                    }
                }
                break;

            default: break;
            }
        }
    }
}

int main(int argc, char *argv[]){
    std::vector<std::string> arg_vec;
    if(argc == 1){
        arg_vec.push_back("-gen");
        arg_vec.push_back("-cout");
    }else{
        for(int i = 1; i < argc; ++i){
            arg_vec.push_back(argv[i]);
        }
    }
    scan_commandline_arg(arg_vec);
    return 0;
}
