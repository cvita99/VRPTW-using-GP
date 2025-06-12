#include <bits/stdc++.h>
#include "SymbolicRegression.h"
using namespace std;

SymbolicRegression::SymbolicRegression() {

        cout << "result expr" << endl;
        resultExpr();
    
        for(int i=0; i < num_of_dots; i++){
            double x = (10.0 / num_of_dots) * i;
            result_dots.push_back(eval(x, result_expr));
        }

        cout << "result: \n"<< result_expr->printExpr() << endl;

}

double SymbolicRegression::eval(double x, Node *expr){
    if(!expr->leftChild && !expr->rightChild){
        if(expr->name == "1")
            return 1.0;
        else if(expr->name == "x")
            return x;
        else 
            cout << "unknown terminal" << endl;
    } else {
        double left = eval(x, expr->leftChild);
        double right = eval(x, expr->rightChild);
        if(expr->name == "*")
            return left * right;
        else if(expr->name == "+")
            return left + right;
        else if(expr->name == "-")
            return left - right;
        else if(expr->name == "min")
            return min(left, right);
        else if(expr->name == "max")
            return max(left, right);
        else if(expr->name == "/")
            if(right < 1e-4)
                return 1.0;
            else
                return left / right;
        else 
            cout << "unknown function" << endl;
    }
    return 0.0;
}

//string result = "x*x*x - (x*x + x + x)";
void SymbolicRegression::resultExpr(){
    Node *expr1 = new Node("+");
    expr1->leftChild = new Node("x");
    expr1->rightChild = new Node("x");

    Node *expr2 = new Node("*");
    expr2->leftChild = new Node("x");
    expr2->rightChild = new Node("x");

    Node *expr3 = new Node("+");
    expr3->leftChild = expr1;
    expr3->rightChild = expr2;

    Node *expr5 = new Node("*");
    expr5->leftChild = new Node("x");
    expr5->rightChild = new Node("x");

    Node *expr6 = new Node("*");
    expr6->leftChild = expr5;
    expr6->rightChild = new Node("x");

    Node *expr4 = new Node("-");
    expr4->leftChild = expr6;
    expr4->rightChild = expr3;
    
    result_expr = expr4;
}

double SymbolicRegression::fitness(Node *expr){
    double diff = 0;
    for(int i=0; i < num_of_dots; i++){
        double x = (10.0 / num_of_dots) * i;
        double val = result_dots[i] - eval(x, expr);
        diff += ((val > 0) ? val : (-val));
    }
    return diff;
}
