#ifndef __H_LAMBDA_CALCULUS__
#define __H_LAMBDA_CALCULUS__

#include "TypeTuple.hpp"

namespace Lambda {
    class LambdaCalculusBase {};

    template<typename T>
    concept Valid = std::is_base_of_v<LambdaCalculusBase, T>;

    template<Valid T, typename Params>
    struct Rebind;

    template<Valid T, Valid...Params>
    struct Rebind<T, TypeTuple<Params...>> {
        using Type = T::template Prototype<Params...>;
    };

    struct None : public LambdaCalculusBase {
        using Call = None;
        template<Valid Other>
        using Apply = Other;
        template<Valid...>
        struct AUX {
            using Type = None;
        };
        template<Valid T, Valid...Args>
        struct AUX<T, Args...> {
            using Type = T::template Calculate<Args...>;
        };
        template<Valid...Others>
        using Calculate = AUX<Others...>::Type;
    };

    template<int n, Valid...Args>
    struct LambdaCalculus {
        using Check = std::enable_if_t<sizeof...(Args) <= n>;
        static constexpr int required = n;
        using Params = TypeTuple<Args...>;
        template<Valid Other>
        static constexpr bool too_much = sizeof...(Args) + 1 > required;

        template<Valid T, Valid Other, bool = too_much<Other>>
        struct AUX_Apply;
        template<Valid T, Valid Other>
        struct AUX_Apply<T, Other, false> {
            using Type = T::template Prototype<Args..., Other>::Call;
        };
        template<Valid T, Valid Other>
        struct AUX_Apply<T, Other, true> {
            using Type = T::template Apply<Other>;
        };
        template<Valid T, bool Boolean>
        struct AUX_Apply<T, None, Boolean> {
            using Type = T;
        };
        template<Valid T, Valid Other>
        using Apply = AUX_Apply<T, Other>::Type;

        template<Valid T, Valid Current, Valid...Others>
        struct AUX_AUX_Calculate {
            using Type = Apply<T, Current>::Call::template Calculate<Others...>;
        };
        template<Valid T, Valid Current>
        struct AUX_AUX_Calculate<T, Current> {
            using Type = Apply<T, Current>::Call;
        };
        template<Valid T, Valid...Others>
        struct AUX_Calculate {
            using Type = AUX_AUX_Calculate<T, Others...>::Type;
        };
        template<Valid T>
        struct AUX_Calculate<T> {
            using Type = T::Call;
        };
        template<Valid T, Valid...Others>
        using Calculate = AUX_Calculate<T, Others...>::Type;
    };

    template<Valid...Args>
    struct If : public LambdaCalculusBase {
        using Base = LambdaCalculus<3, Args...>;
        static constexpr int required = Base::required;

        template<Valid...Params>
        using Prototype = If<Params...>;
        using Call = If;
        template<Valid Other>
        using Apply = Base::template Apply<If, Other>;
        template<Valid...Others>
        using Calculate = Base::template Calculate<If, Others...>;
    };

    template<Valid P, Valid T, Valid F>
    struct If<P, T, F> : public LambdaCalculusBase {
        using Base = LambdaCalculus<3, P, T, F>;
        static constexpr int required = Base::required;

        template<Valid...Params>
        using Prototype = If<Params...>;
        using Call = P::template Calculate<T, F>;
        template<Valid Other>
        using Apply = Base::template Apply<If, Other>;
        template<Valid...Others>
        using Calculate = Base::template Calculate<If, Others...>;
    };

    template<typename Type, Type v>
    struct Value : public LambdaCalculusBase {
        static constexpr Type value = v;
    };

    template<Valid...Args>
    struct True : public Value<bool, true> {
        using Base = LambdaCalculus<2, Args...>;
        static constexpr int required = Base::required;

        template<Valid...Params>
        using Prototype = True<Params...>;
        using Call = True;
        template<Valid Other>
        using Apply = Base::template Apply<True, Other>;
        template<Valid...Others>
        using Calculate = Base::template Calculate<True, Others...>;
    };

    template<Valid T, Valid F>
    struct True<T, F> : public Value<bool, true> {
        using Base = LambdaCalculus<2, T, F>;
        static constexpr int required = Base::required;

        template<Valid...Params>
        using Prototype = True<Params...>;
        using Call = T;
        template<Valid Other>
        using Apply = Base::template Apply<True, Other>;
        template<Valid...Others>
        using Calculate = Base::template Calculate<True, Others...>;
    };

    template<Valid...Args>
    struct False : public Value<bool, false> {
        using Base = LambdaCalculus<2, Args...>;
        static constexpr int required = Base::required;

        template<Valid...Params>
        using Prototype = False<Params...>;
        using Call = False;
        template<Valid Other>
        using Apply = Base::template Apply<False, Other>;
        template<Valid...Others>
        using Calculate = Base::template Calculate<False, Others...>;
    };

    template<Valid T, Valid F>
    struct False<T, F> : public Value<bool, false> {
        using Base = LambdaCalculus<2, T, F>;
        static constexpr int required = Base::required;

        template<Valid...Params>
        using Prototype = False<Params...>;
        using Call = F;
        template<Valid Other>
        using Apply = Base::template Apply<False, Other>;
        template<Valid...Others>
        using Calculate = Base::template Calculate<False, Others...>;
    };

    template<Valid...Args>
    struct Identity : public LambdaCalculusBase {
        using Base = LambdaCalculus<1, Args...>;
        static constexpr int required = Base::required;

        template<Valid...Params>
        using Prototype = Identity<Params...>;
        using Call = Identity;
        template<Valid Other>
        using Apply = Base::template Apply<Identity, Other>;
        template<Valid...Others>
        using Calculate = Base::template Calculate<Identity, Others...>;
    };

    template<Valid X>
    struct Identity<X> : public LambdaCalculusBase {
        using Base = LambdaCalculus<1, X>;
        static constexpr int required = Base::required;

        template<Valid...Params>
        using Prototype = Identity<Params...>;
        using Call = X;
        template<Valid Other>
        using Apply = Base::template Apply<Identity, Other>;
        template<Valid...Others>
        using Calculate = Base::template Calculate<Identity, Others...>;
    };

    template<Valid Expression, int n>
    struct _Let {
        template<typename Args = TypeTuple<void>, bool = Args::size == n>
        struct Type;
        template<Valid...Args>
        struct Type<TypeTuple<Args...>, false> : public LambdaCalculusBase {
            using Base = LambdaCalculus<n, Args...>;
            static constexpr int required = Base::required;
            template<Valid...Params>
            using Prototype = std::enable_if_t<(sizeof...(Params) <= n), Type<TypeTuple<Params...>>>;
            using Call = Type;
            template<Valid Other>
            using Apply = Base::template Apply<Type, Other>;
            template<Valid...Others>
            using Calculate = Base::template Calculate<Prototype<>, Others...>;
        };
        template<Valid...Args>
        struct Type<TypeTuple<Args...>, true> : public LambdaCalculusBase {
            using Base = LambdaCalculus<n, Args...>;
            static constexpr int required = Base::required;
            template<Valid...Params>
            using Prototype = std::enable_if_t<(sizeof...(Params) <= n), Type<TypeTuple<Params...>>>;
            using Call = Expression::template Calculate<Args...>;
            template<Valid Other>
            using Apply = Call::template Apply<Other>;
            template<Valid...Others>
            using Calculate = Call::template Calculate<Others...>;
        };
        template<bool Boolean>
        struct Type<TypeTuple<void>, Boolean> : public LambdaCalculusBase {
            using Base = LambdaCalculus<n>;
            static constexpr int required = Base::required;
            template<Valid...Params>
            using Prototype = std::enable_if_t<(sizeof...(Params) <= n), Type<TypeTuple<Params...>>>;
            using Call = Type;
            template<Valid Other>
            using Apply = Base::template Apply<Type, Other>;
            template<Valid...Others>
            using Calculate = Base::template Calculate<Prototype<>, Others...>;
        };
    };

    template<Valid Expression, int n = 0, Valid...Args>
    using Let = std::enable_if_t<(n >= sizeof...(Args)), typename _Let<Expression, n>::template Type<TypeTuple<Args...>>>;

    struct IndexBase {};

    template<int m, int n = 1, typename = std::enable_if_t<(m > 0 && n > 0)>>
    struct Index : public IndexBase {
        constexpr static int Qty = n;
        constexpr static int Loc[] = { m };
        constexpr static int size = 1;
    };

    template<int...index>
    struct Repeat : public IndexBase {
        constexpr static int Qty = 1;
        constexpr static int Loc[] = { index... };
        constexpr static int size = sizeof...(index);
    };

    template<>
    struct Repeat<> : public IndexBase {
        constexpr static int Qty = 1;
        constexpr static int Loc[1] = { -1 };
        constexpr static int size = 0;
    };

    template<typename T>
    concept ValidIndex = std::is_base_of_v<IndexBase, T>;

    template<typename Order, ValidIndex Current = Index<1, 1>>
    struct CurrentIndex {};

    template<ValidIndex...Order, ValidIndex Current>
    struct CurrentIndex<TypeTuple<Order...>, Current> {
        using Orders = TypeTuple<Order...>;
        using Type = Orders::template Type<Current::Loc[0]>;
        using Inc = CurrentIndex<Orders, typename __If<(Type::Qty > Current::Qty), Index<Current::Loc[0], Current::Qty + 1>, Index<Current::Loc[0] + 1>>::Type>;
        constexpr static int size = Type::size;
        template<int index = 0, typename = std::enable_if_t<(index < size)>>
        constexpr static int Loc = Type::Loc[index];
        constexpr static int sum = (Order::Qty + ...);
    };

    template<ValidIndex...Order>
    struct CurrentIndex<TypeTuple<Order...>, Index<sizeof...(Order), TypeTuple<Order...>::template Type<sizeof...(Order)>::Qty>> {
        using Inc = CurrentIndex<TypeTuple<Order...>, IndexBase>;
        using Type = TypeTuple<Order...>::template Type<sizeof...(Order)>;
        constexpr static int size = Type::size;
        template<int index = 0, typename = std::enable_if_t<(index < size)>>
        constexpr static int Loc = Type::Loc[index];
        constexpr static int sum = (Order::Qty + ...);
    };

    template <Valid Result, typename Left>
    struct Reduce;
    template <Valid Result, Valid Front, Valid...Left>
    struct Reduce<Result, TypeTuple<Front, Left...>> {
        using Type = Reduce<typename Result::template Apply<Front>, TypeTuple<Left...>>::Type;
    };
    template <Valid Result>
    struct Reduce<Result, TypeTuple<void>> {
        using Type = Result;
    };

    template<Valid Front, Valid...Args>
    struct Compose : public LambdaCalculusBase {
        using Base = LambdaCalculus<0>;
        static constexpr int required = 0;
        using Params = TypeTuple<Front, Args...>;

        template<Valid...Params>
        using Prototype = Compose<Front, Args..., Params...>;
        using Call = Reduce<Front, TypeTuple<Args...>>::Type;
        template<Valid Other>
        using Apply = Call::template Apply<Other>;
        template<Valid...Others>
        using Calculate = Base::template Calculate<Prototype<>, Others...>;

        template<typename Current, typename Args = TypeTuple<void>, int left = Current::sum - Args::size, typename Result = Params>
        struct Ordered;

        template<typename Current, Valid T, Valid...Args, int left, typename Result>
        struct Ordered<Current, TypeTuple<T, Args...>, left, Result> {
            template<int Loc, typename Result>
            using Changed = Alter<Loc, typename Result::template Type<Loc>::template Apply<T>, Result>::Type;

            template<int index = 0>
            struct RecChange {
                using Type = Changed<Current::template Loc<index>, typename RecChange<index + 1>::Type>;
            };
            template<>
            struct RecChange<Current::size> {
                using Type = Result;
            };
            using ParamsChanged = RecChange<>::Type;
            using Type = Ordered<typename Current::Inc, TypeTuple<Args...>, left, ParamsChanged>::Type;
        };

        template<typename Current, Valid...Others, Valid...Args>
        struct Ordered<Current, TypeTuple<Others...>, 0, TypeTuple<Args...>> {
            using Type = Compose<Args...>::template Calculate<Others...>;
        };

        template<typename Current, Valid...Args>
        struct Ordered<Current, TypeTuple<void>, 0, TypeTuple<Args...>> {
            using Type = Compose<Args...>::Call;
        };

        template<typename Current, int left, typename Result>
        struct Ordered<Current, TypeTuple<void>, left, Result> : public LambdaCalculusBase {
            using Type = Ordered;
            using Base = LambdaCalculus<left>;
            static constexpr int required = Base::required;

            template<Valid...Params>
            using Prototype = Ordered<Current, TypeTuple<Params...>, left - sizeof...(Params), Result>::Type;
            using Call = Ordered;
            template<Valid Other>
            using Apply = Base::template Apply<Ordered, Other>;
            template<Valid...Others>
            using Calculate = Base::template Calculate<Ordered, Others...>;
        };

        template<ValidIndex...Order>
        using SetOrder = std::enable_if_t<(sizeof...(Order) > 0), typename Ordered<CurrentIndex<TypeTuple<Order...>>>::Type>;
    };

    template<int n, typename = std::enable_if_t<(n > 0)>>
    struct ComposeN : public LambdaCalculusBase {
        using Base = LambdaCalculus<n>;
        static constexpr int required = Base::required;
        template<typename Args = TypeTuple<void>, bool = Args::size == required>
        struct Type;
        template<Valid...Args>
        struct Type<TypeTuple<Args...>, false> : public LambdaCalculusBase {
            static constexpr int required = n - sizeof...(Args);
            template<Valid...Params>
            using Prototype = Type<TypeTuple<Args..., Params...>>;
            using Call = Type;
            template<Valid Other>
            using Apply = Base::template Apply<Type, Other>;
            template<Valid...Others>
            using Calculate = Base::template Calculate<Prototype<>, Others...>;
        };
        template<Valid...Args>
        struct Type<TypeTuple<Args...>, true> : public LambdaCalculusBase {
            static constexpr int required = n - sizeof...(Args);
            template<Valid...Params>
            using Prototype = Type<TypeTuple<Args..., Params...>>;
            using Call = Compose<Args...>::Call;
            template<Valid Other>
            using Apply = Call::template Apply<Other>;
            template<Valid...Others>
            using Calculate = Base::template Calculate<Prototype<>, Others...>;
        };
        template<bool Boolean>
        struct Type<TypeTuple<void>, Boolean> : public LambdaCalculusBase {
            static constexpr int required = n;
            template<Valid...Params>
            using Prototype = Type<TypeTuple<Params...>>;
            using Call = Type;
            template<Valid Other>
            using Apply = Base::template Apply<Type, Other>;
            template<Valid...Others>
            using Calculate = Base::template Calculate<Prototype<>, Others...>;
        };

        template<Valid...Params>
        using Prototype = Type<TypeTuple<Params...>>;
        using Call = Prototype<>::Call;
        template<Valid Other>
        using Apply = Prototype<>::template Apply<Other>;
        template<Valid...Others>
        using Calculate = Prototype<>::template Calculate<Others...>;
    };

    template<Valid T, Valid...Args>
    struct Flow : public LambdaCalculusBase {
        using Base = LambdaCalculus<0>;
        static constexpr int required = 0;
        using Params = Reverse<T, Args...>::Type;

        template<typename Args>
        struct RecursiveApply;
        template<Valid...Args>
        struct RecursiveApply<TypeTuple<Args...>> {
            using Type = Compose<Args...>;
        };

        template<Valid...Params>
        using Prototype = Flow<T, Args..., Params...>;
        using Call = RecursiveApply<Params>::Type::Call;
        template<Valid Other>
        using Apply = RecursiveApply<Params>::Type::template Apply<Other>;
        template<Valid...Others>
        using Calculate = Base::template Calculate<Prototype<>, Others...>;
    };

    template<auto = 'x'>
    using L = ComposeN<1>;

    template<Valid...Args>
    using AND = Let<Compose<L<'a'>, L<'b'>, False<>>::SetOrder<Index<1>, Index<2>>, 2, Args...>;

    template<Valid...Args>
    using OR = Let<Compose<L<'a'>, True<>, L<'b'>>::SetOrder<Index<1>, Index<3>>, 2, Args...>;

    template<Valid...Args>
    using NOT = Let<Compose<L<'x'>, False<>, True<>>::SetOrder<Index<1>>, 1, Args...>;

    template<int n, int loc, int index = 1, int...pack>
    struct RepeatN {
        using Type = RepeatN<n, loc, index + 1, pack..., loc>::Type;
    };
    template<int n, int loc, int...pack>
    struct RepeatN<n, loc, n, pack...> {
        using Type = Repeat<pack..., loc>;
    };

    template<Valid...Args>
    using Inc = Let<Compose<L<'f'>, Compose<L<'n'>, L<'f'>, L<'x'>>::SetOrder<Index<1>, Index<2>, Index<3>>>::SetOrder<Index<2>, Repeat<1, 2>, Index<2>>, 1, Args...>;

    template<unsigned n>
    struct _N : public Value<unsigned, n> {
        using Type = Inc<_N<n - 1>>;
    };
    template<>
    struct _N<0> : public Value<unsigned, 0> {
        using Type = Compose<None, L<'x'>>::SetOrder<Repeat<>, Index<2>>;
    };

    template<unsigned n>
    using N = _N<n>::Type;
}

#endif
