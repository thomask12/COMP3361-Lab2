/*
 * Trace records contain multiple hexadecimal numeric fields, separated by white space (blanks and tabs). 
 * Each line consists of a hexadecimal command code, followed by optional hexadecimal integer arguments to the command. Some arguments may be addresses, some may be other numeric values, depending on the command as described below. 
 * Addresses and counts are all 32 bit unsigned values (type uint32_t).
 * 
 * The trace file will contain the following record types. All numerical values (including counts) are hexadecimal (base 16), without a leading "0x". 
 * Any output should also use hexadecimal format for numeric data except where otherwise specified.
 */

/* 
 * File:   Trace.h
 * Author: kt & om
 *
 * Created on April 11, 2019, 10:30 AM
 */

#ifndef TRACE_H
#define TRACE_H

#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <vector>

using namespace std;

class Trace {
public:
    Trace(string file_name);
    virtual ~Trace();
     
    //Delete copy and move constructors
    Trace(const Trace & other) = delete;
    Trace(Trace && other) = delete;
    Trace &operator=(Trace && other) = delete;
    Trace &operator=(const Trace & other) = delete;
    
    void RunTrace();
    
private:
    ifstream inFile;
    vector<uint8_t> memory;
    //OxF00
    void allocateMemory(string &line);
    //OxCB1
    void compareValues(string &line);
    //OxCBA
    void compareSingleValue(string &line);
    //Ox301
    void setBytes(string &line);
    //Ox30A
    void setMultiBytes(string &line);
    //Ox31D
    void repRange(string &line);
    //Ox4F0
    void outputBytes(string &line);
};

#endif /* TRACE_H */

