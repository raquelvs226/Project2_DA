#include "menu/menu.h"
#include "parser/parser.h"
#include "pipeline/pipeline.h"

#include <iostream>
#include <string>

using namespace std;

int main(int argc, char* argv[]) {
    if (argc == 1) {
        runMenu();
        return 0;
    }

    if (argc == 5 && string(argv[1]) == "-b") {
        try {
            auto variables = parseRangesFile(argv[2]);
            auto registers = parseRegistersFile(argv[3]);
            runPipeline(variables, registers, argv[4]);
        }
        catch (const exception& e) {
            cerr << "Error: " << e.what() << "\n";
            return 1;
        }
        return 0;
    }

    cerr << "Usage: " << argv[0] << " [-b ranges.txt registers.txt allocation.txt]\n";
    return 1;
}