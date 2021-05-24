#include "../include/ILogger.h"
#include<fstream>

namespace{
    class MyLogger: public ILogger{
    public:
        MyLogger();
        MyLogger(std::string str, bool rewrite_if_exist = true);
        RC log(RC code, Level level, char const* const& srcfilem, char const* const& function, int line);
        RC log(RC code, Level level);
    private:
        std::fstream fout;
    };
}

MyLogger::MyLogger(){
    fout.open("Test.txt");
}

MyLogger::MyLogger(std::string str, bool rewrite_if_exist){
    fout.open(str);
}

RC MyLogger::log(RC code, ILogger::Level level, char const* const& srcfilem, char const* const& function, int line){
    switch(code){
    case RC::ALLOCATION_ERROR:
        fout << "ALLOCATION ERROR\n";
    case RC::FILE_NOT_FOUND:
        fout << "FILE NOT FOUND\n";
    case RC::INDEX_OUT_OF_BOUND:
        fout << "INDEX OUT OF BOUND\n";
    case RC::INFINITY_OVERFLOW:
        fout << "INFINITY OVERFLOW\n";
    case RC::INVALID_ARGUMENT:
        fout << "INVALID ARGUMENT\n";
    case RC::MISMATCHING_DIMENSIONS:
        fout << "MISATCHING DIMENSION\n";
    case RC::NOT_NUMBER:
        fout << "NOT NUMBER\n";
    case RC::NULLPTR_ERROR:
        fout << "NULLPTR ERROR\n";
    case RC::UNKNOWN:
        fout << "UNKNOWN\n";
    case RC::VECTOR_NOT_FOUND:
        fout << "VECTOR NOT FOUND\n";
    default:
        return RC::SUCCESS;
    }
    return RC::SUCCESS;
}

RC MyLogger::log(RC code, ILogger::Level level){
    return RC::SUCCESS;
}

ILogger* ILogger::createLogger(){
    return new MyLogger();
}

ILogger* ILogger::createLogger(char const* const& filename, bool rewrite_if_exist){
    return new MyLogger(filename);
}

ILogger::~ILogger(){}
