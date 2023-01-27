#include "status.h"
namespace zkv {
    bool operator==(const DBStatus &x, const DBStatus &y) {
    return x.code == y.code;
    }
    bool operator!=(const DBStatus &x, const DBStatus &y) {
    return x.code != y.code;
    }
}