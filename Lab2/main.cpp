/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.cpp
 * Author: kt & om
 *
 * Created on April 11, 2019, 10:25 AM
 */

#include <cstdlib>

#include "Trace.h"

using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {

    Trace t(argv[1]);
    t.RunTrace();
    return 0;
}

