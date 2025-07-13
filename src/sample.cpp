#include <iostream>
#include "boost/di.hpp"  // パスが正しければOK

namespace di = boost::di;

int main() {
    auto injector = di::make_injector();
    std::cout << "DI構成成功\n";
    return 0;
}
