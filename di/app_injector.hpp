#pragma once

#include "di/logger_injector.hpp"


#include <tuple>
#include <boost/di.hpp>

namespace di = boost::di;

namespace device_reminder {

namespace detail {

// 任意個の tuple を展開して、全injectorをまとめるユーティリティ
template <typename... Tuples>
auto merge_injectors(Tuples&&... tuples) {
    auto combined = std::tuple_cat(std::forward<Tuples>(tuples)...);
    return std::apply([](auto&&... injectors) {
        return di::make_injector(std::forward<decltype(injectors)>(injectors)...);
    }, std::move(combined));
}

} // namespace detail


inline auto make_app_injector() {
    auto logger = make_logger_injector();

    return detail::merge_injectors(
        logger
    );
}

}
