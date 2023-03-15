#include <string>
#include <iostream>
#include "../wlru.h"
#include "../wslru.h"

using namespace std;
using namespace zkv;

int main() {
    using Node = WCacheNode<string, string>;
    Node* node1 = new Node();
    node1->key = "asdf";
    node1->value = new string("qwer");
    node1->hash = 0x1234;
    Node* node2 = new Node();
    node2->key = "asdfasd";
    node2->value = new string("qweqwer");
    node2->hash = 0x123412;
    Node* node3 = new Node();
    node3->key = "asghjhdf";
    node3->value = new string("qwegsdfgr");
    node3->hash = 0x9878;
    Wslru<string, string> cache(2, 2);
    Node* delNode;
    cout << cache.size() << " "  << cache.capacity() << std::endl;
    cache.put(node1, delNode);
    cache.put(node2, delNode);
    cache.put(node3, delNode);
    Node node;
    node.key = "asdf";
    node.value = new string("qwer");
    node.hash = 0x123412;
    Node curNode;
    node.stage_ = PROBATION;
    cache.get(node, &curNode);
    cache.victim(&curNode);
    cout << "get success" << endl;
    cout << *(curNode.value) << endl;

    

}