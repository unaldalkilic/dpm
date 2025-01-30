#include "include/parser/parser.h"

int main(const int argc, const char* argv[]) {
    Parser parser;
    Command command = parser.parse(argc, argv);

    return 0;
}