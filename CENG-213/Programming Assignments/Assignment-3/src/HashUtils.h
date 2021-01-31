// This file will be overwritten while running and evaluating, do not modify!
#ifndef __HASHUTILS__
#define __HASHUTILS__

#include <cmath>
#include <limits>
#include <string>
#include <climits>

/*
 * Returns the hash value of the given key
 */
int Hash(const std::string& key);

/*
 * Returns the hash value of the given key
 */
int Hash(int key);

/*
 * Finds the next appropriate hash table size from a given number
 */
int NextCapacity(int prev);

#endif