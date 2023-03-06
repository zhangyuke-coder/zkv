#include <memory>
#include <stdio.h>
using namespace std;
struct trangle {
    trangle(int c, int r) 
        :col(c), row(r) {

    }
    int col;
    int row;
};

class F {
public:
    F(int col, int row) :
        col_(col), row_(row) {

    }
    void init() {
        trang_ = std::make_shared<trangle>(col_, row_);
    }
    void test() {
        printf("%d, %d \n", trang_->col, trang_->row);
    }
private:
    std::shared_ptr<trangle> trang_;
    int col_;
    int row_;
};
int main() {
    F f(2,3);
    f.init();

    f.test();
}