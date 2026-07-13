#include <iostream>
#include <mutex>
using namespace std;

class Singleton {
private:
    Singleton() {
        count++;
    }
public:
    static int count;
    static Singleton* instance;
    static Singleton *getInstance() {
        if (instance!=nullptr) {
            return instance;
        }
        instance = new Singleton();
        return instance;
    }
     
    int getCount() {
        return count;
    }

};

int Singleton::count = 0;
Singleton* Singleton::instance = nullptr;

int main() {
    Singleton *sol = Singleton::getInstance();
    Singleton *sol2 = Singleton::getInstance();
    cout << sol2->getCount() <<"\n";
    return 0;
}