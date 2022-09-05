#pragma once

#include <map>
#include <mutex>
#include <memory>

class LinkedList {
public:
    int key;
    int val;
    std::shared_ptr<LinkedList> prev;
    std::shared_ptr<LinkedList> next;

    LinkedList(int k, int v);
};

class LRUCache {
private:
    int capacity;
    int size;
    
    std::shared_ptr<LinkedList> dummyHead;
    std::shared_ptr<LinkedList>dummyTail;
    
    // we store a pointer to the linked list node so we can control it's lifecycle when they are deleted and evicted otherwise memory leaky
    std::map<int, std::shared_ptr<LinkedList> > valToNode;

    std::mutex mu;

public:
    LRUCache(int cap);
    
    int get(int key);
    
    void put(int key, int value);
};
