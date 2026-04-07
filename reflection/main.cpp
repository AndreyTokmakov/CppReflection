/**============================================================================
Name        : main.cpp
Created on  : 05.04.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Coroutines
============================================================================**/

#include <iostream>
#include <syncstream>
#include <iomanip>
#include <format>
#include <print>

#include <bitset>

#include <fstream>
#include <thread>
#include <functional>
#include <memory>
#include <concepts>
#include <utility>
#include <string_view>
#include <string>
#include <cassert>
#include <random>
#include <utility>
#include <span>

#include <algorithm>
#include <ranges>
#include <set>
#include <map>
#include <vector>
#include <unordered_map>

#include <meta>

// https://godbolt.org/z/6Prs3fT1b

#if 0
// ==============================================================================================================================
// https://medium.com/@massimiliano.bastia92/c-static-reflection-an-overview-of-the-metaprogramming-paradigm-shift-4cc2ca49a2c6
// ==============================================================================================================================

// The [: :] Splicer
// Once an entity has been reflected into a std::meta::info value, it can be reintroduced into the program’s code using the splicer syntax [: :].
// The splicer effectively injects a reflection back into the source code at compile time,
//    allowing the program to transform meta-level representations into real code.
// The compiler then processes the resulting code as if it had been written manually from the beginning,
//    ensuring smooth integration between the domain of reflection and the ordinary structure of the code.
// In this sense, the splicer completes the transformation initiated by reification, allowing meta-level representations produced through
//    reflection to materialize back into ordinary C++ code, ready to be compiled and executed like any other part of the program.


// The operand inside the splicer must be a constant expression that either is, or can be implicitly converted to a std::meta::info value.
// Depending on the context, splicing behaves differently:
//      [: r :]          -  produces an expression evaluating to the entity represented by r;
//      typename [: r :] -  produces a simple type specifier if r reflects a type;
//      template [: r :] -  produces a template name if r reflects a template.
//      [: r :]::        -  produces a nested-name-specifier corresponding to a namespace, enumeration type, or class.


void retrieve_Data_Members_Names()
{
    // Reflect the type User
    constexpr std::meta::info user_reflection = ^^User;

    // Retrieve the name of the reflected type
    std::cout << "Type: " << std::meta::identifier_of(user_reflection) << std::endl;

    // Reflect and list the non-static data members
    [: expand(std::meta::nonstatic_data_members_of(user_reflection)) :] >>
    [&]<std::meta::info member>() {
        std::cout << " Member: " << std::meta::identifier_of(member) << std::endl;
    };

    // Type: User
    //  Member: name
    //  Member: age
}

namespace demo1
{
    // #include <experimental/reflect>
    // using namespace std::experimental::reflect;

    enum Color {
        Red,
        Green,
        Blue
    };

    using MetaT = reflexpr(Color);

    void test()
    {
        constexpr std::string_view name = get_name_v<get_element_t<0, get_enumerators_t<MetaT>>>;
        std::cout << "The name of the first value is \"" << name << "\"" << std::endl;
    }

}

int main()
{
    retrieve_Data_Members_Names();
    demo1::test();
}

#endif



#if 0

// compiler : clang x86-64 Reflectin-TS
// flags  : -std=c++2b -O2 -pedantic-errors

namespace demo2
{

#include <experimental/reflect>
#include <string_view>

    enum class weekdays {
        monday,
        tuesday,
        wednesday,
        thursday,
        friday,
        saturday,
        sunday
    };

    namespace meta = std::experimental::reflect;

    template <typename... MEC>
    struct enum_to_string_helper
    {
        template <typename E>
        static std::string_view find(E e) {
            return _do_find(e, MEC{}...);
        }

        template <typename E, typename ME1, typename... MEs>
        static std::string_view _do_find(E e, ME1, MEs... mes)
        {
            if (meta::get_constant_v<ME1> == e) {
                return {meta::get_name_v<ME1>};
            }
            return _do_find(e, mes...);
        }

        template <typename E>
        static std::string_view _do_find(E) {
            return {};
        }
    };

    template <typename E>
    std::string_view enum_to_string(E e) {
        return meta::unpack_sequence_t<enum_to_string_helper, meta::get_enumerators_t<reflexpr(E)>>::find(e);
    }

    void test()
    {
        std::cout << enum_to_string(weekdays::monday) << std::endl;
        std::cout << enum_to_string(weekdays::tuesday) << std::endl;
        std::cout << enum_to_string(weekdays::wednesday) << std::endl;
        std::cout << enum_to_string(weekdays::thursday) << std::endl;
        std::cout << enum_to_string(weekdays::friday) << std::endl;
        std::cout << enum_to_string(weekdays::saturday) << std::endl;
        std::cout << enum_to_string(weekdays::sunday) << std::endl;
    }

    // monday
    // tuesday
    // wednesday
    // thursday
    // friday
    // saturday
    // sunday
}
#endif

//  Still not supported bt GCC
#if 0

namespace demo3
{
    #include <experimental/meta>
    #include <experimental/compiler>
    using namespace std::experimental;

    namespace enum_util
    {
        template<typename T>
        requires std::is_enum_v<T>
        constexpr std::string_view to_string(T value)
        {
            template for (constexpr meta::info e : meta::members_of(^T)) {
                if ([:e:] == value) {
                    return meta::name_of(e);
                }
            }
            throw std::runtime_error("Unknown enum value");
        }

        template<typename T>
        requires std::is_enum_v<T>
        constexpr std::optional<T> from_string(std::string_view value)
        {
            template for (constexpr meta::info e : meta::members_of(^T)) {
                if (meta::name_of(e) == value) {
                    return [:e:];
                }
            }
            return {};
        }

    } // namespace enum_util

    enum LightColor
    {
        Red,Green, Blue
    };

    void print_name(const LightColor color)
    {
        const std::string_view str = enum_util::to_string(color);
        std::cout << "The color name is " << str << std::endl;
    }

    void test()
    {
        print_name(Red);
        print_name(Green);
        print_name(Blue);
        try {
            print_name(static_cast<LightColor>(1337));
        } catch (const std::runtime_error& e) {
            std::cout << "exception is: " << e.what() << std::endl;
        }

        static_assert(enum_util::from_string<LightColor>("Red").value() == Red);
        static_assert(enum_util::from_string<LightColor>("Green").value() == Green);
        static_assert(enum_util::from_string<LightColor>("Blue").value() == Blue);
        static_assert(not enum_util::from_string<LightColor>("Magenta").has_value());

    }
}
#endif




namespace reflection
{
    struct User
    {
        std::string name;
        int age { 0 };
    };

    enum class RGB {
        Red,
        Blue,
        Green
    };

    enum class Color {
        Red,
        Green,
        Blue,
        Yellow,
        Purple
    };

    enum class Animal {
        Cat,
        Dog,
        Horse,
        Rabbit,
        Snail
    };
}

namespace reflection::types
{
    constexpr int max_connections = 100;

    void say_hello() { }


    void basics()
    {
        constexpr std::meta::info r = ^^int;

        typename[:r:] i = 42;       // Same as: int x = 42;
        static_assert(std::is_same_v<decltype(i), int>);

        typename[:^^char:] c = '*';  // Same as: char c = '*';
        static_assert(std::is_same_v<decltype(c), char>);
    }

    void reflectTypes_Simple()
    {
        // Reflect the struct type 'User'
         [[maybe_unused]]
        constexpr std::meta::info user_reflection = ^^User;

        // Reflect the global variable 'max_connections'
         [[maybe_unused]]
        constexpr std::meta::info connections_reflection = ^^max_connections;

        // Reflect the free function 'say_hello'
         [[maybe_unused]]
        constexpr std::meta::info hello_reflection = ^^say_hello;

        // Reflect the data member 'age' inside struct 'User'
         [[maybe_unused]]
        constexpr std::meta::info user_age_reflection = ^^User::age;
    }

    void useReflectedData()
    {
        constexpr std::meta::info int_reflection = ^^int;
        constexpr std::meta::info double_reflection = ^^double;
        constexpr std::meta::info string_reflection = ^^std::string;

        typename [: int_reflection :] number = 100;
        typename [: double_reflection :] pi = 3.14;
        typename [: string_reflection :] message = "Hello Reflection";

        std::cout << "Number: " << number << " of type " << typeid(number).name() << std::endl;
        std::cout << "Pi: " << pi  << " of type " << typeid(pi).name() << std::endl;
        std::cout << "Message: " << message  << " of type " << typeid(message).name() << std::endl;

        // Number: 100
        // Pi: 3.14
        // Message: Hello Reflection
    }

    template <typename T>
    void deduce_Type_of_Vector()
    {
        constexpr auto vec = ^^std::vector<T>;
        typename [: vec :] x = { T(1), T(2), T(3)};
        for (const auto& elem : x) {
            std::cout << elem << " ";
        }
        std::cout << std::endl;

        // Output: 1 2 3
    }

    template <typename T>
    struct MyVec
    {
        std::array<T, 3> data;
    };

    template <typename Key, typename Value>
    struct MyMap
    {
        Key key;
        Value value;
    };

    void Create_New_Type()
    {
        // Reflect the individual types we'll need
        constexpr std::meta::info key_info     = ^^int;       // Get meta info for 'int'
        constexpr std::meta::info vec_template = ^^MyVec;     // Get meta info for the template 'MyVec'
        constexpr std::meta::info value_arg    = ^^double;    // Get meta info for 'double'

        // Use reflection to create the type MyVec<double>
        // This "fills in" the template MyVec<T> with T = double
        constexpr std::meta::info vec_info = std::meta::substitute(vec_template, {value_arg});

        // Use reflection to create the type MyMap<int, MyVec<double>>
        // This creates a reflected instantiation of MyMap with the previous types
        constexpr std::meta::info map_info = std::meta::substitute(^^MyMap, {key_info, vec_info});

        // Splice the reflected type into real C++ code using [: :]
        // This gives us a real usable type: MyMap<int, MyVec<double>>
        typename [:map_info:] obj = {
            7,                              // obj.key = 7
            { {1.1, 2.2, 3.3} }             // obj.value = MyVec<double> with 3 doubles
        };

        // Use the object like any normal type
        std::cout << "Key: " << obj.key << std::endl;
        std::cout << "Values:\n";
        for (double v : obj.value.data)  // Loop through the MyVec's data array
            std::cout << v << '\n';

        /*
        Key: 7
        Values:
        1.1
        2.2
        3.3
        */
    }
}


namespace reflection::get_data_member_0
{
    void retrieve_Data_Members_Names()
    {
        // Reflect the type User
        constexpr std::meta::info user_reflection = ^^User;

        // Retrieve the name of the reflected type
        std::cout << "Type: " << std::meta::identifier_of(user_reflection) << std::endl;

        /*
        // Reflect and list the non-static data members
        [: expand(std::meta::nonstatic_data_members_of(user_reflection)) :] >>
        [&]<std::meta::info member>() {
            std::cout << " Member: " << std::meta::identifier_of(member) << std::endl;
        };
        */
    }
}

namespace reflection::get_data_member_1
{
    consteval auto data_members_of(const std::meta::info& info)
    {
        constexpr auto ctx = std::meta::access_context::current();
        return std::define_static_array(std::meta::nonstatic_data_members_of(info, ctx));
    }

    void demo()
    {
        template for( auto constexpr m: data_members_of(^^User) ) {   //***
            println( "member named: {}", std::meta::display_string_of(m) );
        }

        /*
        member named: reflection::User::name
        member named: reflection::User::age
        */
    }
}

namespace reflection::enums
{
    void printValues_Simple()
    {
        template for(constexpr auto color : std::define_static_array(std::meta::enumerators_of(^^Color)))
        {
            constexpr std::string_view colorName = std::meta::identifier_of(color);
            std::println("{}", colorName);
        }

        /*
        Green
        Blue
        Yellow
        Purple
        */
    }
}

namespace reflection::enums
{
    template<typename E>
    requires std::is_enum_v<E>
    consteval auto enumerators_of()
    {
        return []<size_t ... I>(std::index_sequence<I...>)
        {
            return std::array{[:enumerators_of(^^E)[I]:] ...};
        }
        (std::make_index_sequence<enumerators_of(^^E).size()>{});
    }

    template<typename E>
    requires std::is_enum_v<E>
    consteval auto identifiers_of()
    {
        return []<size_t ... I>(std::index_sequence<I...>)
        {
            return std::array{std::string_view(identifier_of(enumerators_of(^^E)[I])) ...};
        }
        (std::make_index_sequence<enumerators_of(^^E).size()>{});
    }

    template<typename E>
        requires std::is_enum_v<E>
    const auto& to_string(E value)
    {
        static const auto toStringMap = [] static
        {
            static constexpr auto AllValues = enumerators_of<E>();
            static constexpr auto AllStrings = identifiers_of<E>();
            static std::unordered_map<E, const std::string> result;
            for(const auto& [enumValue, string] : std::ranges::zip_view(AllValues, AllStrings))
                result.emplace(enumValue, string);
            return result;
        }();
        return toStringMap.at(value);
    }

    template<typename E>
    requires std::is_enum_v<E>
    std::ostream& operator<<(std::ostream& s, E value)
    {
        s << to_string(value);
        return s;
    }

    void printValues_Complex()
    {
        for(const auto color : enumerators_of<Color>())
            std::cout << color << std::endl;

        std::cout << std::endl;

        for(const auto animal : enumerators_of<Animal>())
            std::cout << animal << std::endl;

        /*
        Red
        Green
        Blue
        Yellow
        Purple

        Cat
        Dog
        Horse
        Rabbit
        Snail
        */
    }
}

namespace reflection::enums
{
    enum class Color
    {
        Transparent,
        Red = 2,
        Green,
        Blue = 8,
        Yellow
    };

    // https://compiler-explorer.com/z/W8b9xrx5j
    // https://andreasfertig.com/blog/2025/08/cpp26-reflection-at-compile-time/?trk=feed_main-feed-card_feed-article-content

    template<typename E>
    requires std::is_enum_v<E>
    constexpr inline auto num_enumerators_of {
        std::meta::enumerators_of(^^E).size()
    };

    template<typename E>
    requires std::is_enum_v<E>
    consteval auto get_enum_values() -> decltype(auto)
    {
        std::array<E, num_enumerators_of<E>> res;
        template for(size_t i{}; constexpr auto& e : std::define_static_array(std::meta::enumerators_of(^^E)))
        {
            res[i++] = [:e:];
        }
        return res;
    }

    void enumerateValues()
    {
        for(const auto e : get_enum_values<Color>()) {
            std::println("{} ", std::to_underlying(e));
        }

        /**
        0
        2
        3
        8
        9
        **/
    }
}

namespace reflection::enums
{
    bool validate_enum()
    {
        /*
        constexpr std::span fields = std::define_static_array(std::meta::enumerators_of(^^Color));
        for (const auto& field : fields) {
            std::cout << field << std::endl;
        }*/

        // constexpr auto vals = std::define_static_array(std::meta::enumerators_of(^^Color));
        template for(constexpr auto color : std::define_static_array(std::meta::enumerators_of(^^Color)) )
        {
            constexpr std::string_view colorName = std::meta::identifier_of(color);
            std::println("{}", colorName);
        }

        /*
        Green
        Blue
        Yellow
        Purple
        */

        /*
        return std::ranges::equal(fields, enumerators, [](auto field, auto enumerator) {
            return identifier_of(field) == identifier_of(enumerator);
        });*/

        return  true;
    }

}


namespace reflection::checking_type
{
    class A;

    class B
    {
        int a;
    };

    void is_complete_type()
    {
        constexpr std::meta::info aInfo = ^^A;
        constexpr std::meta::info bInfo = ^^B;

        std::println("Is 'A' complete type: {}", std::meta::is_complete_type(aInfo));
        std::println("Is 'B' complete type: {}", std::meta::is_complete_type(bInfo));
    }
}


namespace reflection::splicers
{
    void type_Aliases()
    {
        constexpr auto r = ^^int;
        typename[:r:] x = 42;

        static_assert(std::is_same_v<decltype(x), int>);
    }
}

namespace reflection::serialization
{
    struct Point2D
    {
        double x { 0.0 };
        double y { 0.0 };
    };

    template <typename T>
    void format(T const& t)
    {
        std::cout << identifier_of(^^T) << "{ ";
        constexpr auto access_ctx = std::meta::access_context::unchecked();

        template for (constexpr auto mem : std::define_static_array(nonstatic_data_members_of(^^T, access_ctx))) {
            std::cout << std::format("{} = {}, ", identifier_of(mem), t.[:mem:]);
        }

        std::cout << "}\n";
    }

    void simpleType()
    {
        format(Point2D{.x = 3.0, .y = 4.0});
    }
}

namespace reflection::generating_code
{
    // INFO: Metafunctions to generate code:
    //  - define_aggregate: Takes the reflection of an incomplete class/struct/union type and a list of reflections of data member
    //                      descriptions and completes the given class type with datab members as described in the given order.
    //  - data_member_spec: Returns a reflection a data member description for the data-member of a given type.


    struct Person;

    consteval {
        std::meta::define_aggregate(^^Person, {
            std::meta::data_member_spec(^^std::string, { .name = "name" }),
            std::meta::data_member_spec(^^int, { .name = "age" }),
            std::meta::data_member_spec(^^double, { .name = "weight" })
        });
    }

    void demo()
    {
    }
}

namespace test_1
{
    // INFO: https://godbolt.org/

    template <typename T>
    consteval auto find_member(std::string_view name)
    {
        const std::meta::access_context ctx = std::meta::access_context::current();
        for (std::meta::info field : std::meta::nonstatic_data_members_of(^^T, ctx)) {
            if (std::meta::has_identifier(field) && std::meta::identifier_of(field) == name) {
                return field;
            }
        }
        throw "Variable name in format string not found in local state!";
    }

    consteval bool is_inside_braces(const std::string_view s, const int idx)
    {
        if (s[idx] == '}')
            return true;
        for (int j = idx - 1; j >= 0; --j) {
            if (s[j] == '{')
                return true;
            if (s[j] == '}')
                return false;
        }
        return false;
    }


    template <const char* S, typename V>
    constexpr void fun(V& local_state)
    {
        constexpr std::string_view s = S;
        template for (constexpr int i : std::views::iota(0, (int)s.size()))
        {
            if constexpr (s[i] == '{')
            {
                constexpr size_t end = s.find('}', i);
                static_assert(end != std::string_view::npos, "Missing closing '}' in format string!");
                constexpr std::string_view var_name = s.substr(i + 1, end - i - 1);
                std::cout << local_state.[:find_member<V>(var_name):];
            }
            else if constexpr (!is_inside_braces(s, i)) {
                std::cout << s[i];
            }
        }
    }

#define fprint(fmt_string) fun<std::define_static_string(fmt_string)>(state);

#if 0
    void demo()
    {
        struct {
            int health = 100;
            float shield_level = 50.5f;
            const char* player_name = "Kratos";
        } state; // something like std::meta::local_variables would save us

        fprint("Player {player_name} has {health} HP and {shield_level} Shields.\n");
        fprint("Just testing braces: {health}\n");
    }
#endif
}


namespace experiments
{
    template <typename T>
    constexpr std::meta::info info = ^^T;

    template <template <typename...> typename F>
    consteval std::meta::info fmap(std::same_as<std::meta::info> auto... as) {
        return std::meta::substitute(^^F, {as...});
    }

    // join :: meta::info{meta::info{T}} -> meta::info{T}
    consteval auto join(std::meta::info a) {
        return std::meta::extract<std::meta::info>(a);
    }

    template <std::meta::info F>
    consteval auto bind(std::meta::info a) {
        return join(std::meta::substitute(F, {a}));
    }

    void demo()
    {
        constexpr auto info_int = info<int>;
    }

    void arrayOfTypes()
    {
        constexpr std::array types1  { ^^int, ^^float, ^^double };
        // constexpr std::array types2 = std::to_array<std::meta::info>( {^^int, ^^float, ^^double });

        constexpr std::array<size_t, types1.size()> sizes = [&] {
            std::array<size_t, types1.size()> result;
            std::ranges::transform(types1, result.begin(), std::meta::size_of);
            return result;
        }();

        std::println("Sizes: {}", sizes);  // --> Sizes: [4, 4, 8]
    }
}


int main([[maybe_unused]] int argc,
         [[maybe_unused]] char** argv)
{
    const std::vector<std::string_view> args(argv + 1, argv + argc);

    using namespace reflection;

    // types::basics();
    // types::reflectTypes_Simple();
    // types::useReflectedData();
    // types::deduce_Type_of_Vector<int>();
    // types::deduce_Type_of_Vector<double>();
    // types::Create_New_Type();

    // get_data_member_0::retrieve_Data_Members_Names();
    // get_data_member_1::demo();

    // enums::printValues_Simple();
    // enums::printValues_Complex();
    // enums::enumerateValues();
    // enums::validate_enum();

    // checking_type::is_complete_type();

    // splicers::type_Aliases();

    // serialization::simpleType();

    // experiments::demo();
    experiments::arrayOfTypes();


    return EXIT_SUCCESS;
}

