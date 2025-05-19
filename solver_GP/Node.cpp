#include "Node.h"
#include <bits/stdc++.h>
using namespace std;

string Node::printExpr(){
    if(!leftChild && !rightChild){
        return name;
    }
    string s = "(" + leftChild->printExpr();
    s += " " + name + " ";
    s += rightChild->printExpr() + ")";
    return s;
}

int Node::countNodes(){
    int l = 0, r = 0;
    if(leftChild)
        l = leftChild->countNodes();
    if (rightChild)
        r = rightChild->countNodes();
    return 1 + l + r;
}