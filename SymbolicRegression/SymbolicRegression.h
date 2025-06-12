#include <bits/stdc++.h>
#include "../solver_GP/FitnessFunction.h"
using namespace std;

class SymbolicRegression : public FitnessFunction{
    public:
        SymbolicRegression();

        Node* result_expr;
        const int num_of_dots = 23; //[0, 3];
        vector<double> result_dots;

        double fitness(Node *expr) override;

        double eval(double x, Node *expr);
        void resultExpr();
};