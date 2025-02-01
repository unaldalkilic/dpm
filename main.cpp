#include "./include/parser.h"
#include "./include/engine.h"

int main(const int argc, const char* argv[]) {
    Parser parser;
    Command command = parser.parse(argc, argv);

    Engine engine;
    engine.run(command);

    return 0;
}