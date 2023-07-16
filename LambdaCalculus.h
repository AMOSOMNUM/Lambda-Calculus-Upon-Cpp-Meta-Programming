#ifndef __H_LAMBDA_CALCULUS__
#define __H_LAMBDA_CALCULUS__

#include "TypeTuple.hpp"

#include <string>

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

    template<template<Valid...> typename T, Valid...Args>
    struct Rebind<T<Args...>, TypeTuple<Args...>> {
        struct Fallback {
            template<typename...>
            using Calculate = T<Args...>;
        };
        using Type = Fallback;
    };

    //Impossible Fallback
    template<int n, typename tuple, int progress = 0, typename Result = TypeTuple<>>
    struct Arrange {
        using Type = tuple;
        using Left = TypeTuple<>;
    };

    template<int n, Valid...Args>
    struct Arrange<n, TypeTuple<Args...>> {
        using Base = Front<n, TypeTuple<Args...>>;
        using Params = Base::Type;
        using Left = Base::Left;
        using Type = __If<Left::size == 0, Params, typename Arrange<n, Params, 1, Left>::Type>::Type;
    };

    template<int n, typename ParamList, int progress, Valid...Args>
    struct Arrange<n, ParamList, progress, TypeTuple<Args...>> {
        using Current = ParamList::template Type<progress>;
        using CurrentParams = Merge<typename Current::Params, TypeTuple<Args...>>::Type;
        using Base = Arrange<Current::required, CurrentParams>;
        using Left = Base::Left;
        using Result = Alter<progress, typename Rebind<Current, typename Base::Type>::Type, ParamList>::Type;
        using Type = __If<progress == n, Result, typename Arrange<n, Left, progress + 1, Result>::Type>::Type;
    };

    template<int n, Valid...Args>
    struct LambdaCalculus {
        using Check = std::enable_if_t<sizeof...(Args) <= n>;
        static constexpr int required = n;
        using Params = TypeTuple<Args...>;
        template<Valid...Others>
        using Normalised = Arrange<required, TypeTuple<Args..., Others...>>::Type;
        template<Valid...Others>
        static constexpr bool too_few = sizeof...(Args) + sizeof...(Others) < required;

        template<Valid T, Valid...Others>
        using Return = Rebind<T, Normalised<Others...>>::Type::Call;
        template<Valid T, Valid Other>
        using Apply = __If<too_few<Other>, typename T::template Prototype<Args..., Other>, Return<T, Other>>::Type;

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

    template<Valid Expression, int n = 0>
    struct Let : public LambdaCalculusBase {
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
            using Call = Expression::template Calculate<Args...>;
            template<Valid Other>
            using Apply = Base::template Apply<Type, Other>;
            template<Valid...Others>
            using Calculate = Base::template Calculate<Prototype<>, Others...>;
        };
        template<bool Boolean>
        struct Type<TypeTuple<void>, Boolean> : public LambdaCalculusBase {
            static constexpr int required = n;
            template<Valid...Params>
            using Prototype = Type<TypeTuple<Params...>>;
            using Call = Expression::Call;
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
        using Calculate = Prototype<>::template Calculate<Others...>;;
    };

    struct IndexBase {};

    template<int m, int n, typename = std::enable_if_t<(m > 0 && n > 0)>>
    struct Index : public IndexBase {
        constexpr static int Loc = m;
        constexpr static int Qty = n;
    };

    template<typename T>
    concept ValidIndex = std::is_base_of_v<IndexBase, T>;

    template<typename Order, ValidIndex Current = Index<1, 1>>
    struct CurrentIndex;

    template<ValidIndex...Order, ValidIndex Current>
    struct CurrentIndex<TypeTuple<Order...>, Current> {
        using Orders = TypeTuple<Order...>;
        using Inc = CurrentIndex<Orders, typename __If<((Orders::template Type<Current::Loc>::Qty) > (Current::Qty)), Index<Current::Loc, Current::Qty + 1>, Index<Current::Loc + 1, 1>>::Type>;
        constexpr static int Loc = Orders::template Type<Current::Loc>::Loc;
        constexpr static int sum = (Order::Qty + ...);
    };
    
    template<ValidIndex...Order>
    struct CurrentIndex<TypeTuple<Order...>, Index<sizeof...(Order), TypeTuple<Order...>::template Type<sizeof...(Order)>::Qty>> {
        using Inc = CurrentIndex<TypeTuple<Order...>, IndexBase>;
        constexpr static int Loc = TypeTuple<Order...>::template Type<sizeof...(Order)>::Loc;
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
            using ParamsChanged = Alter<Current::Loc, typename Result::template Type<Current::Loc>::template Apply<T>, Result>::Type;
            using Type = Ordered<typename Current::Inc, TypeTuple<Args...>, left, ParamsChanged>::Type;
        };
        
        template<typename Current, Valid...Others, Valid...Args>
        struct Ordered<Current, TypeTuple<Others...>, 0, TypeTuple<Args...>> {
            using Type = Compose<Args...>::template Calculate<Others...>;
        };

        template<typename Current, Valid...Args>
        struct Ordered<Current, TypeTuple<void>, 0, TypeTuple<Args...>> {
            using Type = Compose<Args...>::template Call;
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
            using Apply = Ordered<Current, TypeTuple<Other>, left - 1, Result>::Type;
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

    template<Valid...Args>
    using AND = Compose<typename ComposeN<2>::template Calculate<Args...>, False<>>;

    template<Valid...Args>
    using OR = Compose<ComposeN<1>, True<>, ComposeN<1>>::SetOrder<Index<1, 1>, Index<3, 1>>::Calculate<Args...>;
}

#endif
