#ifndef S0M4B0DY_TYPE_TRAITS_HPP
#define S0M4B0DY_TYPE_TRAITS_HPP

#include <type_traits>

namespace _helpers
{
    template <class Iterator_t>
    concept AddableIterator = requires(Iterator_t it) {
        { *it + *it };
    } or requires(Iterator_t it) {
        { std::declval<Iterator_t::value_type>() + std::declval<Iterator_t::value_type>() };
    };

    template <typename T>
    struct is_dereferenceable
    {
    private:
        // A type that can be anything (used for SFINAE)
        typedef char yes[1];
        typedef char no[2];

        // Test function to check if the dereference operator (*) is valid
        template <typename U>
        static yes &test(decltype(*std::declval<U>()) *);

        // Fallback test function if the dereference operator (*) is not valid
        template <typename U>
        static no &test(...);

    public:
        // If the size of the test function with the valid dereference operator (*) is sizeof(yes),
        // then U is dereferenceable; otherwise, it is not.
        static constexpr bool value = sizeof(test<T>(nullptr)) == sizeof(yes);
    };

    template <typename T>
    struct has_value_type
    {
    private:
        typedef char yes[1];
        typedef char no[2];

        template <typename U>
        static yes &test(typename U::value_type *);

        template <typename U>
        static no &test(...);

    public:
        static constexpr bool value = sizeof(test<T>(nullptr)) == sizeof(yes);
    };

    template <typename T>
    inline constexpr bool is_dereferenceable_v = is_dereferenceable<T>::value;

    template <typename T>
    inline constexpr bool has_value_type_v = has_value_type<T>::value;

    template <class Iterator_t, class Enable = void>
    struct IteratorValueType;

    template <class Iterator_t>
    struct IteratorValueType<Iterator_t, std::enable_if_t<has_value_type_v<Iterator_t>>>
    {
        using value_type = typename Iterator_t::value_type;
    };

    template <class Iterator_t>
    struct IteratorValueType<Iterator_t, std::enable_if_t<is_dereferenceable_v<Iterator_t>>>
    {
        using value_type = decltype(*std::declval<Iterator_t>());
    };
}

#endif
