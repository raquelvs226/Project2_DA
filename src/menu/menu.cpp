#include "menu.h"

#include "parser/parser.h"

#include <iostream>
#include <string>

using namespace std;

void runMenu() {

    vector<Variable> variables;
    Registers registers;
    bool filesLoaded = false;

    int option;

    do {

        cout << "\n========== REGISTER ALLOCATION TOOL ==========\n";
        cout << "1. Load input files\n";
        cout << "2. Run allocation and save results to file\n";
        cout << "0. Exit\n";
        cout << "Option: ";

        cin >> option;

        switch (option) {

            case 1: {

                string rangesFile;
                string registersFile;

                cout << "\nRanges file: ";
                cin >> rangesFile;

                cout << "Registers file: ";
                cin >> registersFile;

                try {

                    variables = parseRangesFile(rangesFile);
                    registers = parseRegistersFile(registersFile);

                    filesLoaded = true;

                    cout << "\nFiles loaded successfully.\n";

                    // Optional debug info
                    cout << "Variables loaded: " << variables.size() << endl;
                    cout << "Registers: " << registers.registers << endl;
                    cout << "Algorithm: " << registers.algorithm << endl;
                }
                catch (const exception& e) {
                    cerr << "\nError: " << e.what() << endl;
                }

                break;
            }

            case 2: {

                if (!filesLoaded) {
                    cout << "\nPlease load input files first.\n";
                    break;
                }

                string outputFile;

                cout << "\nOutput file: ";
                cin >> outputFile;

                // TODO:
                // call allocation algorithm
                // save output file

                cout << "\nAllocation not implemented yet.\n";

                break;
            }

            case 0:
                cout << "\nExiting...\n";
                break;

            default:
                cout << "\nInvalid option.\n";
        }

    } while (option != 0);
}