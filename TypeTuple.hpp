#include <type_traits>

inline namespace _TypeTuple {
    template<bool P, typename T, typename F>
    struct __If;

    template<typename T, typename F>
    struct __If<true, T, F> {
        using Type = T;
    };

    template<typename T, typename F>
    struct __If<false, T, F> {
        using Type = F;
    };

    template<typename Current = void, typename...Types>
    struct TypeTuple {
        using Next = TypeTuple<Types...>;
        static constexpr unsigned size = sizeof...(Types) + 1;
        template<int index, typename = std::enable_if_t<(index <= size) && (index > 0)>>
            using Type = __If<index == 1, Current, typename Next::template Type<index - 1>>::Type;
        using Front = Current;
    };

    template<typename Current>
    struct TypeTuple<Current> {
        using Next = TypeTuple<void>;
        static constexpr unsigned size = 1;
        template<int index, typename = std::enable_if_t<index == 1>>
            using Type = Current;
        using Front = Current;
    };

    template<>
    struct TypeTuple<void> {
        static constexpr unsigned size = 0;
    };

    template<typename U, typename V>
    struct Merge;

    template<typename...U_Args, typename...V_Args>
    struct Merge<TypeTuple<U_Args...>, TypeTuple<V_Args...>> {
        using Type = TypeTuple<U_Args..., V_Args...>;
    };

    template<typename...U_Args>
    struct Merge<TypeTuple<U_Args...>, TypeTuple<void>> {
        using Type = TypeTuple<U_Args...>;
    };

    template<typename...V_Args>
    struct Merge<TypeTuple<void>, TypeTuple<V_Args...>> {
        using Type = TypeTuple<V_Args...>;
    };

    template<typename U, typename...V_Args>
    struct Merge<U, TypeTuple<V_Args...>> {
        using Type = TypeTuple<U, V_Args...>;
    };

    template<typename...U_Args, typename V>
    struct Merge<TypeTuple<U_Args...>, V> {
        using Type = TypeTuple<U_Args..., V>;
    };

    template<typename U>
    struct Merge<U, TypeTuple<void>> {
        using Type = TypeTuple<U>;
    };

    template<typename V>
    struct Merge<TypeTuple<void>, V> {
        using Type = TypeTuple<V>;
    };

    template<int n, typename Source, typename = std::enable_if_t<(n >= 0)>>
    struct Front;

    template<typename T>
    concept NotVoid = !std::is_same_v<T, void>;

    template<int n, NotVoid T, typename...Args>
    struct Front<n, TypeTuple<T, Args...>> {
        using Type = Merge<T, typename Front<n - 1, TypeTuple<Args...>>::Type>::Type;
        using Left = Front<n - 1, TypeTuple<Args...>>::Left;
    };

    template<typename...Args>
    struct Front<0, TypeTuple<Args...>> {
        using Type = TypeTuple<void>;
        using Left = TypeTuple<Args...>;
    };

    template<int n>
    struct Front<n, TypeTuple<void>> {
        using Type = TypeTuple<void>;
        using Left = TypeTuple<void>;
    };

    template<>
    struct Front<0, TypeTuple<void>> {
        using Type = TypeTuple<void>;
        using Left = TypeTuple<void>;
    };

    template<int n, typename Value, typename Source, typename = std::enable_if_t<(n <= Source::size)>>
    struct Alter;

    template<int n, typename Value, typename...Args>
    struct Alter<n, Value, TypeTuple<Args...>> {
        using Type = Merge<typename Front<n, TypeTuple<Args...>>::Left, typename Merge<typename Front<n - 1, TypeTuple<Args...>>::Type, Value>::Type>::Type;
    };
};