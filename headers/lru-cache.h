#pragma once

#include <map>

using namespace std;

class LinkedList {
public:
    int key;
    int val;
    LinkedList* prev;
    LinkedList* next;

    LinkedList(int k, int v);
};

class LRUCache {
private:
    int capacity;
    int size;
    
    LinkedList* dummyHead;
    LinkedList* dummyTail;
    
    // we store a pointer to the linked list node so we can control it's lifecycle when they are deleted and evicted otherwise memory leaky
    map<int, LinkedList*> valToNode;

public:
    LRUCache(int cap);

    ~LRUCache();
    
    int get(int key);
    
    void put(int key, int value);
};
