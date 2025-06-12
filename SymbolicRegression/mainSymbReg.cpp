#include "SymbolicRegression.h"
#include "../solver_GP/GeneticProgramming.h"
#include <bits/stdc++.h>
using namespace std;

int main(){
    SymbolicRegression *symbReg = new SymbolicRegression();
    GeneticProgramming *solver = new GeneticProgramming(4, 200, 30000, 0.5, symbReg);
    solver->functions = { "+", "*", "/", "-"};
    solver->terminals = {"x"};
    solver->run();
    return 0;
}