#pragma once
#include <bits/stdc++.h>
using namespace std;

struct Node {
    Node *leftChild = nullptr;
    Node *rightChild = nullptr;
    Node *parent = nullptr;
    string name;
    int depth;
    int id;
    bool terminal;

    Node(string name) : name(name) {}
    Node(string name, int depth) : name(name), depth(depth) {}
    Node() : name("") {}
    ~Node() = default;

    string printExpr();
    int countNodes();
};