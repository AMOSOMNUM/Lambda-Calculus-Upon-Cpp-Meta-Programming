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
        static constexpr int size = sizeof...(Types) + 1;
        using Next = TypeTuple<Types...>;
        using Front = Current;

        template<int index, bool = index == 1>
        struct AUX;
        template<int index> 
        struct AUX<index, true>{
            using Type = Current;
        };
        template<int index>
        struct AUX<index, false> {
            using Type = Next::template Type<index - 1>;
        };
        template<int index, typename = std::enable_if_t<(index <= size) && (index > 0)>>
        using Type = AUX<index>::Type;

        template<typename T>
        inline static constexpr bool find() {
            return std::is_same_v<Current, T> ? true : Next::template find<T>();
        }
    };

    template<typename Current>
    struct TypeTuple<Current> {
        using Next = TypeTuple<void>;
        static constexpr int size = 1;
        using Front = Current;

        template<int index, typename = std::enable_if_t<index == 1>>
        using Type = Current;

        template<typename T>
        inline static constexpr bool find() {
            return std::is_same_v<Current, T>;
        }
    };

    template<>
    struct TypeTuple<void> {
        static constexpr int size = 0;

        template<typename T>
        inline static constexpr bool find() {
            return false;
        }
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

    template<NotVoid T, typename...Args>
    struct Front<0, TypeTuple<T, Args...>> {
        using Type = TypeTuple<void>;
        using Left = TypeTuple<T, Args...>;
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

    template<int n, typename Value, typename Source, typename = std::enable_if_t<(n <= Source::size && n > 0)>>
    struct Alter;

    template<int n, typename Value, typename...Args>
    struct Alter<n, Value, TypeTuple<Args...>> {
        using Type = Merge<typename Merge<typename Front<n - 1, TypeTuple<Args...>>::Type, Value>::Type, typename Front<n, TypeTuple<Args...>>::Left>::Type;
    };

    template<typename Current = void, typename...Args>
    struct Reverse {
        using Type = Merge<typename Reverse<Args...>::Type, Current>::Type;
    };

    template<>
    struct Reverse<void> {
        using Type = TypeTuple<void>;
    };

    template<typename T, typename Tuple, typename = std::enable_if_t<!std::is_same_v<T, void>>>
    struct Find {
        template<typename = Tuple, bool found = Tuple::template find<T>()>
        struct _Find {
            static constexpr bool value = found;
            static constexpr int loc = 0;
        };
        template<typename _Tuple>
        struct _Find<_Tuple, true> {
            template<typename Left = Tuple, bool = std::is_same_v<T, typename Left::Front>>
            struct __Find {
                static constexpr int loc = 1;
            };
            template<typename Front, typename...Left>
            struct __Find<TypeTuple<Front, Left...>, false> {
                static constexpr int loc = __Find<TypeTuple<Left...>>::loc + 1;
            };
            static constexpr bool value = true;
            static constexpr int loc = __Find<>::loc;
        };
        static constexpr bool value = Tuple::template find<T>();
        static constexpr int loc = _Find<>::loc;
    };
};