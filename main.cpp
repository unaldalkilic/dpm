#include "./include/parser.hpp"
#include "./include/engine.hpp"

int main(const int argc, const char* argv[]) {
    Parser parser;
    Command command = parser.parse(argc, argv);

    Engine engine;
    engine.run(command);

    return 0;
}