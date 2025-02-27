#include "./include/parser.hpp"
#include "./include/engine.hpp"
#include "./include/helpers.hpp"
#include "./include/exceptions.hpp"

int main(const int argc, const char* argv[]) {
    Parser parser;
    Command command;
    try {
        command = parser.parse(argc, argv);
    } catch(const InvalidParameter& e) {
        print(e.what(), OutputType::ERROR);
        return 1;
    }

    Engine engine;
    engine.run(command);

    return 0;
}