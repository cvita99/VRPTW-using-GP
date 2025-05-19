#pragma once
#include <bits/stdc++.h>
#include "Node.h"

class FitnessFunction {
    public:
        virtual double fitness(Node *expr) = 0;
    };