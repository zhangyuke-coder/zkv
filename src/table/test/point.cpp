#include <iostream>
using namespace std;


class base {
public:
    base() {
        cout << "base" << endl;
    }
    virtual void print();
};

class son : public base {
public:
    son() {
        cout << "son" << endl;
    }
    virtual void print() override{
        cout << "asdf" << endl;
    }
};
base* test() {
    base* iter;
    iter = new son();
    iter->print();
    return iter;
}
int main() {
    auto it = test();
    // it->print();
}