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
        struct AUX_Calculate {
            using Type = Apply<T, Current>::Call::template Calculate<Others...>;
        };
        template<Valid T, Valid Current>
        struct AUX_Calculate<T, Current> {
            using Type = Apply<T, Current>::Call;
        };
        template<Valid T, Valid Current, Valid...Others>
        using Calculate = AUX_Calculate<T, Current, Others...>::Type;
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
        template<Valid Current, Valid...Others>
        using Calculate = Base::template Calculate<If, Current, Others...>;
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
        template<Valid Current, Valid...Others>
        using Calculate = Base::template Calculate<If, Current, Others...>;
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
        template<Valid Current, Valid...Others>
        using Calculate = Base::template Calculate<True, Current, Others...>;
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
        template<Valid Current, Valid...Others>
        using Calculate = Base::template Calculate<True, Current, Others...>;
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
        template<Valid Current, Valid...Others>
        using Calculate = Base::template Calculate<False, Current, Others...>;
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
        template<Valid Current, Valid...Others>
        using Calculate = Base::template Calculate<False, Current, Others...>;
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
        template<Valid Current, Valid...Others>
        using Calculate = Base::template Calculate<Identity, Current, Others...>;
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
        template<Valid Current, Valid...Others>
        using Calculate = Base::template Calculate<Identity, Current, Others...>;
    };

    template<Valid Expression, int n = 0>
    struct Let : public LambdaCalculusBase {
        using Base = LambdaCalculus<n>;
        static constexpr int required = Base::required;
        template<typename Args = TypeTuple<void>, bool = Args::size == required>
        struct Type;
        template<Valid...Args>
        struct Type<TypeTuple<Args...>, false> : public LambdaCalculusBase {
            template<Valid...Params>
            using Prototype = Type<TypeTuple<Args..., Params...>>;
            using Call = Type;
            template<Valid Other>
            using Apply = Base::template Apply<Type, Other>;
        };
        template<Valid...Args>
        struct Type<TypeTuple<Args...>, true> : public LambdaCalculusBase {
            template<Valid...Params>
            using Prototype = Type<TypeTuple<Args..., Params...>>;
            using Call = Expression::template Calculate<Args...>;
            template<Valid Other>
            using Apply = Base::template Apply<Type, Other>;
        };
        template<bool Boolean>
        struct Type<TypeTuple<void>, Boolean> : public LambdaCalculusBase {
            template<Valid...Params>
            using Prototype = Type<TypeTuple<Params...>>;
            using Call = Expression::Call;
            template<Valid Other>
            using Apply = Base::template Apply<Type, Other>;
        };

        template<Valid...Params>
        using Prototype = Type<TypeTuple<Params...>>;
        using Call = Prototype<>::Call;
        template<Valid Other>
        using Apply = Prototype<>::template Apply<Other>;
        template<Valid Current, Valid...Others>
        using Calculate = Base::template Calculate<Prototype<>, Current, Others...>;
    };
}

#endif
