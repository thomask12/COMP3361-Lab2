/*
 * Trace records contain multiple hexadecimal numeric fields, separated by white space (blanks and tabs). 
 * Each line consists of a hexadecimal command code, followed by optional hexadecimal integer arguments to the command. Some arguments may be addresses, some may be other numeric values, depending on the command as described below. 
 * Addresses and counts are all 32 bit unsigned values (type uint32_t).
 * 
 * The trace file will contain the following record types. All numerical values (including counts) are hexadecimal (base 16), without a leading "0x". 
 * Any output should also use hexadecimal format for numeric data except where otherwise specified.
 */

/* 
 * File:   Trace.cpp
 * Author: kt & om
 * 
 * Created on April 11, 2019, 10:30 AM
 */

#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include "Trace.h"

using namespace std;

Trace::Trace(string file_name) {
    inFile.open(file_name);

    if (!inFile.is_open()) {
        throw runtime_error("Could not open file: " + file_name);
        exit(1);
    }
}

Trace::~Trace() {
}

/**
 * RunTrace - parses commands and runs specific commands depending of hex values given
 */
void Trace::RunTrace() {
    string line;
    int commandLine = 1;
    vector<string> commands;

    while (getline(inFile, line)) {
        //Sets all characters in a string to lower case for easier parsing
        transform(line.begin(), line.end(), line.begin(), ::tolower);
        commands.push_back(line);
    }
    for (int i = 0; i < commands.size(); i++) {
        //Searches for which commands to execute
        if (commands[i].find("*") != string::npos) {
            cout << dec << commandLine << ":" << commands[i] << "\n";
        } else if (commands[i].find("f00 ") != string::npos) {
            cout << dec << commandLine << ":" << commands[i] << "\n";
            allocateMemory(commands[i]);
        } else if (commands[i].find("cb1 ") != string::npos) {
            cout << dec << commandLine << ":" << commands[i] << "\n";
            compareValues(commands[i]);
        } else if (commands[i].find("cba ") != string::npos) {
            cout << dec << commandLine << ":" << commands[i] << "\n";
            compareSingleValue(commands[i]);
        } else if (commands[i].find("301 ") != string::npos) {
            cout << dec << commandLine << ":" << commands[i] << "\n";
            setBytes(commands[i]);
        } else if (commands[i].find("30a ") != string::npos) {
            cout << dec << commandLine << ":" << commands[i] << "\n";
            setMultiBytes(commands[i]);
        } else if (commands[i].find("31d ") != string::npos) {
            cout << dec << commandLine << ":" << commands[i] << "\n";
            repRange(commands[i]);

        } else if (commands[i].find("4f0 ") != string::npos) {
            cout << dec << commandLine << ":" << commands[i] << "\n";
            outputBytes(commands[i]);
        }            //Handles empty line case
        else {
            cout << dec << commandLine << ":\n";
        }
        commandLine++;
    }
}

/**
 * allocateMemory - Set amount of memory to count blocks of memory, where each block is 1024 (0x400) bytes. 
 * Subsequent F00 commands change the amount of allocated memory. 
 * Contents of newly-allocated memory should be initialized to 0.
 * 
 * @param line - string containing values needed to be parsed for given function
 */
void Trace::allocateMemory(string &line) {
    uint32_t memSize;
    string word;
    stringstream stream(line);
    stringstream ss;
    while (stream >> word) {
    }
    ss << hex << word;
    ss >> memSize;
    //Multiply per byte to get kB
    memSize *= 146432;
    memory.resize(memSize, 0);
}

/**
 * compareValues - Compare 8-bit values to memory locations starting at addr. 
 * If the values of any of the bytes starting at addr are different from the corresponding values, 
 * write an error message to standard output for each difference with the address and the differing values  (all in hexadecimal).
 * 
 * @param line - string containing values needed to be parsed for given function
 */
void Trace::compareValues(string &line) {
    stringstream stream(line);
    stringstream ss;
    vector<string> input;
    string word;
    uint32_t address;

    while (stream >> word) {
        input.push_back(word);
    }

    ss << hex << input[1];
    ss >> address;

    for (int i = 2; i < input.size(); i++) {
        stringstream valStream;
        uint32_t value;
        valStream << hex << input[i];
        valStream >> value;
        if (memory[address + (i - 2)] != value) {
            cout << "compare error at address " << hex << address + (i - 2) << " expected " << hex << value << ", actual is " << hex << setfill('0') << setw(2) << static_cast<uint32_t> (memory[address + (i - 2)]) << "\n";
        }
    }
}

/**
 * compareSingleValue  -Compare count bytes in memory starting at addr to the specified 8-bit value. 
 * If the value of any of the countbytes starting at addr are different from the specified value, 
 * write an error message to standard output for each difference with the address and the differing values  (all in hexadecimal).
 * 
 * @param line - string containing values needed to be parsed for given function
 */
void Trace::compareSingleValue(string &line) {
    stringstream stream(line);
    stringstream ss;
    stringstream addr;
    stringstream val;
    string word;
    uint32_t count;
    uint32_t address;
    uint32_t value;
    vector<string> input;

    while (stream >> word) {
        input.push_back(word);
    }
    ss << hex << input[1];
    ss >> count;
    addr << hex << input[2];
    addr >> address;
    val << hex << input[3];
    val >> value;

    for (int i = 0; i < count; i++) {
        if (memory[address + i] != value) {
            cout << "compare error at address " << hex << address + i << " expected " << hex << setfill('0') << setw(2) << value << ", actual is " << hex << setfill('0') << setw(2) << static_cast<uint32_t> (memory[address + i]) << "\n";
        }
    }
}

/**
 * setBytes - Store values starting at addr; values is a list of byte values, separated by white space.
 * 
 * @param line - string containing values needed to be parsed for given function
 */
void Trace::setBytes(string &line) {
    stringstream stream(line);
    stringstream ss;
    vector<string> input;
    string word;
    uint32_t address;

    while (stream >> word) {
        input.push_back(word);
    }

    ss << hex << input[1];
    ss >> address;

    for (int i = 2; i < input.size(); i++) {
        stringstream valStream;
        uint32_t value;
        valStream << hex << input[i];
        valStream >> value;
        memory[address + (i - 2)] = value;
    }
}

/**
 * setMultiBytes - Store count copies of value starting at addr.
 * 
 * @param line - string containing values needed to be parsed for given function
 */
void Trace::setMultiBytes(string &line) {
    stringstream stream(line);
    stringstream ss;
    stringstream addr;
    stringstream val;
    string word;
    vector<string> input;
    uint32_t count;
    uint32_t address;
    uint32_t value;

    while (stream >> word) {
        input.push_back(word);
    }

    ss << hex << input[1];
    ss >> count;
    addr << hex << input[2];
    addr >> address;
    val << hex << input[3];
    val >> value;

    for (int i = 0; i < count; i++) {
        memory[address + i] = value;
    }
}

/**
 * repRange - Copy count bytes starting at src_addr into count bytes starting at dest_addr. 
 * The source and destination ranges will not overlap.
 * 
 * @param line - string containing values needed to be parsed for given function
 */
void Trace::repRange(string &line) {
    stringstream stream(line);
    stringstream ss;
    stringstream sAddr;
    stringstream eAddr;
    string word;
    vector<string> input;
    uint32_t addrStart;
    uint32_t addrEnd;
    uint32_t count;

    while (stream >> word) {
        input.push_back(word);
    }
    ss << hex << input[1];
    ss >> count;
    eAddr << hex << input[2];
    eAddr >> addrEnd;
    sAddr << hex << input[3];
    sAddr >> addrStart;

    for (int i = 0; i < count; i++) {
        memory[addrEnd + i] = memory[addrStart + i];
    }
}

/**
 * outputBytes - Print count bytes to standard output, starting at address addr. 
 * Write 16 bytes per line, with commas between values. 
 * Each line should start with the address of the first byte on that line, 8 hexadecimal digits with leading zeroes, followed by a colon and a space. 
 * Print each byte as exactly 2 digits with a leading 0 for values less than 10 (hex). Separate the bytes with a comma.
 * 
 * @param line - string containing values needed to be parsed for given function
 */
void Trace::outputBytes(string &line) {
    stringstream stream(line);
    stringstream ss;
    stringstream addr;
    string word;
    vector<string> input;
    uint32_t count;
    uint32_t address;

    while (stream >> word) {
        input.push_back(word);
    }

    ss << hex << input[1];
    ss >> count;
    addr << hex << input[2];
    addr >> address;

    int j = 0;
    //Outputs data in proper format
    for (int i = 0; i < count; i++) {
        if (i % 16 == 0) {
            if (i > 1) {
                cout << "\n";
            }
            cout << hex << setfill('0') << setw(8) << address + i << ": ";
            j = 0;
        }
        cout << hex << setfill('0') << setw(2) << static_cast<uint32_t> (memory[address + i]);
        if (j < 15) {
            cout << ",";
            j++;
        }
    }
    cout << "\n";
}