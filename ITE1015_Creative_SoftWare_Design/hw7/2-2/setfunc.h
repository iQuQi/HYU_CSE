#pragma once
#include <set>
#include <iostream>
#include <string>
using namespace std;
std::set<int> parseSet(const std::string& str);
void printSet(const std::set<int>&);
std::set<int> getIntersection(const std::set<int>& set0, const std::set<int>& set1);
std::set<int> getUnion(const std::set<int>& set0, const std::set<int>& set1);
std::set<int> getDifference(const std::set<int>& set0, const std::set<int>& set1);