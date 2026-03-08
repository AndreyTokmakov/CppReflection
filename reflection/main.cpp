/**============================================================================
Name        : main.cpp
Created on  : 05.04.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Coroutines
============================================================================**/

#include <iostream>
#include <vector>
#include <string_view>

// #include <experimental/meta>


namespace Reflection
{

    enum class Color
    {
        Transparent,
        Red = 2,
        Green,
        Blue = 8,
        Yellow
    };

#if 0

    .. https://compiler-explorer.com/z/W8b9xrx5j
    // https://andreasfertig.com/blog/2025/08/cpp26-reflection-at-compile-time/?trk=feed_main-feed-card_feed-article-content

    template<typename E>
    requires std::is_enum_v<E>
    constexpr inline auto num_enumerators_of{
        std::meta::enumerators_of(^^E).size()};

    template<typename E>
    requires std::is_enum_v<E>
    consteval auto get_enum_values()
    {
        std::array<E, num_enumerators_of<E>> res;

        template for(size_t i{}; constexpr auto& e : std::define_static_array(std::meta::enumerators_of(^^E)))
        {
            res[i++] = [:e:];
        }
        return res;
    }

    void Test()
    {
        for(const auto e : get_enum_values<Color>()) {
            std::print("{} ", std::to_underlying(e));
        }
        std::println();
    }

#endif

};



int main([[maybe_unused]] int argc,
         [[maybe_unused]] char** argv)
{
    const std::vector<std::string_view> args(argv + 1, argv + argc);

    // std::experimental::meta::info.
   
    return EXIT_SUCCESS;
}

