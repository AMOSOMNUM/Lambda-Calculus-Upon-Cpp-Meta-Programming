#ifndef __H_LAMBDA_CALCULUS__
#define __H_LAMBDA_CALCULUS__

#include "TypeTuple.hpp"
#include <string>

namespace Lambda {
	class LambdaCalculusBase {};

	template<typename T>
	concept Valid = std::is_base_of_v<LambdaCalculusBase, T>;

	enum class BasicType {
		Label,
		Expression,
		Conjunction,
		None,
		Y
	};

	template<typename T>
	concept Label = std::is_base_of_v<LambdaCalculusBase, T>&& T::type == BasicType::Label;

	struct None : public LambdaCalculusBase {
		static constexpr BasicType type = BasicType::None;

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
		template<Label label, Valid T>
		using Replace = None;
		template<Label old, Label nov>
		using Change = None;
		template<Valid...Others>
		using Calculate = AUX<Others...>::Type;

		static std::string print() {
			return "";
		}
	};

	struct LambdaCalculus {
		template<Valid T, Valid Other>
		struct _Apply {
			using Type = T::template Apply<Other>;
		};
		template<Valid T>
		struct _Apply<T, None> {
			using Type = T;
		};
		template<Valid T, Valid Other>
		using Apply = _Apply<T, Other>::Type;

		template<Valid T, Valid Current, Valid...Others>
		struct __Calculate {
			using Type = Apply<T, Current>::template Calculate<Others...>;
		};
		template<Valid T, Valid Current>
		struct __Calculate<T, Current> {
			using Type = Apply<T, Current>::Call;
		};
		template<Valid T, Valid...Others>
		struct _Calculate {
			using Type = __Calculate<T, Others...>::Type;
		};
		template<Valid T>
		struct _Calculate<T> {
			using Type = T::Call;
		};
		template<Valid T, Valid...Others>
		using Calculate = _Calculate<T, Others...>::Type;
	};
	/*
		template<typename Type, Type v>
		struct Value : public LambdaCalculusBase {
			static constexpr Type value = v;
		};

		template<typename Expression, Valid Result = None>
		struct Reduce {
			using Type = Reduce<typename Expression::Next, typename Result::template Apply<typename Expression::Front>>::Type;
		};
		template<Valid Result>
		struct Reduce<TypeTuple<void>, Result> {
			using Type = Result;
		};
	*/
	constexpr char LABEL_DEFAULT_CHARSET[] = {
		't', 'x', 'y', 'z', 'a', 'b', 'c', 'd', 'm', 'p', 'q', 'r', 'k', 'u', 'v', 'w', 'f', 'g', 'h', 'o', 'n'
	};

	template<char label>
	struct L : public LambdaCalculusBase {
		static constexpr BasicType type = BasicType::Label;
		using Labels = TypeTuple<L>;

		template<char NewLabel = label>
		using Prototype = L<NewLabel>;

		using Call = L;
		template<Label Other, Valid T>
		using Replace = __If<std::is_same_v<L, Other>, T, L>::Type;
		template<Label old, Label nov>
		using Change = Replace<old, nov>;
		template<Valid Arg>
		using Apply = __If<std::is_same_v<Arg, None>, L, Arg>::Type;
		template<Valid...Args>
		using Calculate = LambdaCalculus::template Calculate<L, Args...>;

		static std::string print() {
			return std::string() + label;
		}
	};

	template<Label label, typename Outer, typename Inner, typename Uncovered>
	constexpr bool exists = Outer::template find<label>() | Inner::template find<label>() | Uncovered::template find<label>();

	template<typename Outer, typename Inner, typename Uncovered, int index = 0, bool isExist = exists<L<LABEL_DEFAULT_CHARSET[index]>, Outer, Inner, Uncovered>>
	struct CreateNewLabel {
		using Type = L<LABEL_DEFAULT_CHARSET[index]>;
	};
	template<typename Outer, typename Inner, typename Uncovered, int index>
	struct CreateNewLabel<Outer, Inner, Uncovered, index, true> {
		using Type = CreateNewLabel<Outer, Inner, Uncovered, index + 1>::Type;
	};
	template<typename Outer, typename Inner, typename Uncovered>
	struct CreateNewLabel<Outer, Inner, Uncovered, sizeof(LABEL_DEFAULT_CHARSET) / sizeof(char) - 1, true> {
		using Type = L<'?'>;
	};

	template<Valid...Args>
	struct LabelInfoBuilder {
		using Params = TypeTuple<Args...>;
		template<typename Left = Params, typename Result = TypeTuple<void>>
		struct Rec {
			template<Valid T, BasicType = T::type>
			struct GetLabels {
				using Type = TypeTuple<void>;
			};
			template<Valid T>
			struct GetLabels<T, BasicType::Label> {
				using Type = TypeTuple<T>;
			};
			template<Valid T>
			struct GetLabels<T, BasicType::Conjunction> {
				using Type = T::Labels;
			};
			template<Valid T>
			struct GetLabels<T, BasicType::Expression> {
				using Type = T::Uncovered;
			};
			template<typename _Left, typename _Result = TypeTuple<void>, bool = _Result::template find<typename _Left::Front>()>
			struct Combine;
			template<typename _Left, typename _Result>
			struct Combine<_Left, _Result, true> {
				using Type = Combine<typename _Left::Next, _Result>::Type;
			};
			template<typename _Left, typename _Result>
			struct Combine<_Left, _Result, false> {
				using Type = Combine<typename _Left::Next, typename Merge<_Result, typename _Left::Front>::Type>::Type;
			};
			template<typename Last, typename _Result>
			struct Combine<TypeTuple<Last>, _Result, false> {
				using Type = Merge<_Result, Last>::Type;
			};
			template<typename Last, typename _Result>
			struct Combine<TypeTuple<Last>, _Result, true> {
				using Type = _Result;
			};
			template<typename Some, typename _Result = TypeTuple<void>, bool = std::is_same_v<Some, TypeTuple<void>>>
			struct CombineSome {
				using Type = Combine<Some, _Result>::Type;
			};
			template<typename Some, typename _Result>
			struct CombineSome<Some, _Result, true> {
				using Type = _Result;
			};
			using Type = Rec<typename Left::Next, typename CombineSome<typename GetLabels<typename Left::Front>::Type, Result>::Type>::Type;
		};
		template<typename Result>
		struct Rec<TypeTuple<void>, Result> {
			using Type = Result;
		};
		using Type = Rec<>::Type;
	};

	template<Label...labels>
	struct Order {
		static constexpr bool empty = false;
		using Labels = TypeTuple<labels...>;
		using Front = Labels::Front;
		template<typename Next = typename Labels::Next, bool = std::is_same_v<Next, TypeTuple<void>>>
		struct _Next;
		template<Label...pack>
		struct _Next<TypeTuple<pack...>, false>
		{
			using Type = Order<pack...>;
		};
		template<typename Next>
		struct _Next<Next, true>
		{
			using Type = Order<>;
		};
		using Next = _Next<>::Type;

		static std::string print() {
			return ((std::string("\u03bb") + labels::print()) + ...);
		}
	};
	template<>
	struct Order<> {
		static constexpr bool empty = true;
		using Labels = TypeTuple<void>;

		static std::string print() {
			return "";
		}
	};

	template<typename Tuple>
	struct ToOrder;
	template<Label...pack>
	struct ToOrder<TypeTuple<pack...>> {
		using Type = Order<pack...>;
	};
	template<>
	struct ToOrder<TypeTuple<void>> {
		using Type = Order<>;
	};

	template<typename RawTuple, bool NonExpand = false>
	struct ConjunctionSimplifiedTuple {
		template<typename Tuple = RawTuple, int index = 1, typename Result = TypeTuple<void>>
		struct RemoveNone {
			using Current = Tuple::template Type<index>::Call;
			static constexpr bool isNone = std::is_same_v<Current, None>;
			using Type = RemoveNone<Tuple, index + 1, typename __If<isNone, Result, typename Merge<Result, Current>::Type>::Type>::Type;
		};
		template<typename Tuple, typename Result>
		struct RemoveNone<Tuple, RawTuple::size + 1, Result> {
			using Type = Result;
		};

		template<typename Tuple, bool Empty = std::is_same_v<Tuple, TypeTuple<void>>>
		struct RemoveFirstCompose {
			template<typename Front = typename Tuple::Front, typename Next = typename Tuple::Next, bool = Front::type == BasicType::Conjunction>
			struct _RemoveFirstCompose {
				using Type = Tuple;
			};
			template<typename Front, typename Next>
			struct _RemoveFirstCompose<Front, Next, true> {
				using Type = Merge<typename __If<Front::unexpand, Front, typename Front::Params>::Type, Next>::Type;
			};
			using Type = _RemoveFirstCompose<>::Type;
		};
		template<typename Tuple>
		struct RemoveFirstCompose<Tuple, true> {
			using Type = Tuple;
		};

		template<typename Tuple, bool = (Tuple::size > 1) && !NonExpand>
		struct TryEvaluation {
			template<typename Last, bool = Tuple::Front::type == BasicType::Expression>
			struct TryOnce {
				using Front = typename Last::Front;
				using Arg = typename Last::Next::Front;
				using Left = typename Last::Next::Next;
				using Type = Merge<typename Front::template Apply<Arg>::Call, Left>::Type;
			};
			template<typename Result>
			struct TryOnce<Result, false> {
				using Left = TypeTuple<void>;
				using Type = Result;
			};

			template<typename Last = Tuple, typename Result = typename TryOnce<Tuple>::Type, typename Left = typename TryOnce<Tuple>::Left>
			struct Rec {
				using Type = Rec<Result, typename TryOnce<Result>::Type, typename TryOnce<Result>::Left>::Type;
			};
			template<typename Last, typename Result>
			struct Rec<Last, Result, TypeTuple<void>> {
				using Type = Result;
			};

			using Type = Rec<>::Type;
		};
		template<typename Tuple>
		struct TryEvaluation<Tuple, false> {
			using Type = Tuple;
		};

		using Type = TryEvaluation<typename RemoveFirstCompose<typename RemoveNone<>::Type>::Type>::Type;
	};

	template<Label label, Valid Arg, typename Result, int index = 1, int max = Result::size + 1>
	struct ConjunctionReplacedTuple {
		using Current = Result::template Type<index>;
		using AfterReplace = Current::template Replace<label, Arg>;
		using Type = ConjunctionReplacedTuple<label, Arg, typename Alter<index, AfterReplace, Result>::Type, index + 1, max>::Type;
	};

	template<Label label, Valid Arg, typename Result, int max>
	struct ConjunctionReplacedTuple<label, Arg, Result, max, max> {
		using Type = Result;
	};

	template<Valid...Args>
	struct Compose : public LambdaCalculusBase {
		static constexpr BasicType type = BasicType::Conjunction;
		static constexpr bool unexpand = false;
		using Labels = LabelInfoBuilder<Args...>::Type;
		using Params = TypeTuple<Args...>;
		using Simplified = ConjunctionSimplifiedTuple<Params>::Type;

		struct RemoveBracket {
			template<typename Tuple = Params, int size = Params::size>
			struct GetInside {
				using Type = Compose;
			};
			template<typename T>
			struct GetInside<T, 1> {
				using Type = T::Front;
			};
			template<typename T>
			struct GetInside<T, 0> {
				using Type = None;
			};

			using Type = GetInside<>::Type;
		};

		template<typename _Args, bool = std::is_same_v<Params, _Args>>
		struct Rebind {
			using Type = Compose;
		};
		template<Valid...Params>
		struct Rebind<TypeTuple<Params...>, false> {
			using Type = Compose<Params...>;
		};
		template<typename Altered = Simplified>
		using Prototype = Rebind<Altered>::Type;

		using Call = Prototype<>::RemoveBracket::Type;
		template<Label label, Valid Arg>
		using Replace = Prototype<typename ConjunctionReplacedTuple<label, Arg, Params>::Type>;
		template<Label old, Label nov>
		using Change = Replace<old, nov>;
		template<Valid Arg>
		using Apply = __If<std::is_same_v<Arg, None>, Compose, Prototype<typename Merge<Params, Arg>::Type>>::Type;
		template<Valid..._Args>
		using Calculate = LambdaCalculus::template Calculate<Call, _Args...>;

		static std::string print() {
			const auto str = (std::string("(") + ... + (Args::print() + ' '));
			return str.substr(0, str.length() == 1 ? 1 : str.length() - 1) + ')';
		}
	};

	template<typename paramlist, Valid expression>
	struct LabelInspector;
	template<Label...labels, Valid expression>
	struct LabelInspector<Order<labels...>, expression> {
		using Params = TypeTuple<labels...>;
		using Used = LabelInfoBuilder<expression>::Type;
		template<typename Left = Used, typename Uncovered = TypeTuple<void>>
		struct CurryInspector {
			using Type = CurryInspector<typename Left::Next, typename __If<Params::template find<typename Left::Front>(), Uncovered, typename Merge<Uncovered, typename Left::Front>::Type>::Type>::Type;
		};
		template<typename Result>
		struct CurryInspector<TypeTuple<void>, Result> {
			using Type = Result;
		};
		using Uncovered = CurryInspector<>::Type;
		static constexpr bool curried = std::is_same_v<Uncovered, TypeTuple<void>>;
	};

	template<typename paramlist, Valid expression>
	struct Expression;
	template<Label...labels, Valid expression>
	struct Expression<Order<labels...>, expression> : public LambdaCalculusBase {
		static constexpr BasicType type = BasicType::Expression;
		using Labels = Order<labels...>::Labels;
		using LabelInfo = LabelInspector<Order<labels...>, expression>;
		using Uncovered = LabelInfo::Uncovered;
		static constexpr bool curried = !std::is_same_v<typename LabelInfo::Uncovered, TypeTuple<void>>;
		using InnerExpression = expression;

		struct RemoveBracket {
			using Type = __If<Labels::size == 0, InnerExpression, Expression>::Type;
		};

		struct Simplify {
			template<typename Simplified = typename InnerExpression::Call, BasicType = InnerExpression::Call::type>
			struct MergeExpression {
				using Type = Simplified;
				using NewLabels = Labels;
			};
			template<Valid Simplified>
			struct MergeExpression<Simplified, BasicType::Expression> {
				template<typename CurrentLabels, typename Old, bool = Simplified::Labels::template find<Old>()>
				struct NewLabel {
					using Type = CreateNewLabel<CurrentLabels, typename Simplified::Labels, typename Simplified::Uncovered>::Type;
				};
				template<typename CurrentLabels, typename Old>
				struct NewLabel<CurrentLabels, Old, false> {
					using Type = Old;
				};
				template<typename Last = Labels, int index = 1, int max = Labels::size + 1>
				struct LabelChangeInspector {
					using Result = Alter<index, typename NewLabel<Last, typename Last::template Type<index>>::Type, Last>::Type;
					using Type = LabelChangeInspector<Result, index + 1>::Type;
				};
				template<typename Result, int max>
				struct LabelChangeInspector<Result, max, max> {
					using Type = Merge<Result, typename Simplified::Labels>::Type;
				};
				using Type = Simplified::InnerExpression;
				using NewLabels = LabelChangeInspector<>::Type;
			};

			using Result = MergeExpression<>;
			using Type = Result::Type;
			using Labels = Result::NewLabels;
		};

		template<typename Altered, typename Labels>
		struct _Prototype {
			using Type = Expression<typename ToOrder<Labels>::Type, Altered>;
		};
		template<typename _>
		struct _Prototype<void, _> {
			using SimplifiedExpression = Simplify::Type;
			using SimplifiedLabels = Simplify::Labels;
			using Type = Expression<typename ToOrder<SimplifiedLabels>::Type, SimplifiedExpression>;
		};
		template<Valid Altered>
		struct _Prototype<Altered, void> {
			using Type = Expression<typename ToOrder<Labels>::Type, Altered>;
		};
		template<typename Altered = void, typename Labels = void>
		using Prototype = _Prototype<Altered, Labels>::Type;

		template<Label old, Label nov, typename OrderAfterReplace = typename Alter<Find<old, Labels>::loc, nov, Labels>::Type>
		using SelfChange = Prototype<typename InnerExpression::template Change<old, nov>, OrderAfterReplace>;

		template<Valid Arg, bool = Order<labels...>::empty>
		struct _Apply {
			template<typename Left, typename Labels, typename Last = TypeTuple<void>>
			struct IsConflict {
				static constexpr bool found = Labels::template find<typename Left::Front>();
				using Result = __If<found, typename Merge<Last, typename Left::Front>::Type, Last>::Type;
				using Type = IsConflict<typename Left::Next, Labels, Result>::Type;
			};
			template<typename Labels, typename Result>
			struct IsConflict<TypeTuple<void>, Labels, Result> {
				using Type = Result;
			};

			template<Valid = Arg, BasicType type = Arg::type>
			struct LabelDealer {
				using Current = Labels::Front;
				using Type = Prototype<typename InnerExpression::template Replace<Current, Arg>, typename Labels::Next>;
			};
			template<Label _>
			struct LabelDealer<_, BasicType::Label> {
				template<typename = Expression, bool = Labels::Next::template find<Arg>()>
				struct Process {
					using NewLabel = CreateNewLabel<TypeTuple<Arg>, Labels, Uncovered>::Type;
					using Type = SelfChange<Arg, NewLabel>;
				};
				template<typename T>
				struct Process<T, false> {
					using Type = T;
				};
				using Changed = Process<>::Type;
				using Current = Changed::Labels::Front;
				using Type = Prototype<typename Changed::InnerExpression::template Replace<Current, Arg>, typename Changed::Labels::Next>;
			};
			template<Valid _>
			struct LabelDealer<_, BasicType::Expression> {
				template<typename = Expression, bool = Arg::curried && !std::is_same_v<typename IsConflict<typename Labels::Next, typename Arg::Uncovered>::Type, TypeTuple<void>>>
				struct Process {
					using Conflict = IsConflict<Labels, typename Arg::Uncovered>::Type;
					template<typename Left = Conflict, typename Last = Expression>
					struct RecChange {
						using NewLabel = CreateNewLabel<typename Arg::Uncovered, typename Last::Labels, typename Last::Uncovered>::Type;
						using Result = typename Last::template SelfChange<typename Left::Front, NewLabel>;
						using Type = RecChange<typename Left::Next, Result>::Type;
					};
					template<typename Result>
					struct RecChange<TypeTuple<void>, Result> {
						using Type = Result;
					};
					using Type = RecChange<>::Type;
				};
				template<typename T>
				struct Process<T, false> {
					using Type = T;
				};

				using Changed = Process<>::Type;
				using Current = Changed::Labels::Front;
				using Type = Prototype<typename Changed::InnerExpression::template Replace<Current, Arg>, typename Changed::Labels::Next>;
			};
			template<Valid _>
			struct LabelDealer<_, BasicType::Conjunction> {				
				template<typename = Expression, bool = !std::is_same_v<typename IsConflict<Labels, typename Arg::Labels>::Type, TypeTuple<void>>>
				struct Process {
					using Conflict = IsConflict<Labels, typename Arg::Labels>::Type;
					template<typename Left = Conflict, typename Last = Expression>
					struct RecChange {
						using NewLabel = CreateNewLabel<typename Arg::Labels, typename Last::Labels, typename Last::Uncovered>::Type;
						using Result = typename Last::template SelfChange<typename Left::Front, NewLabel>;
						using Type = RecChange<typename Left::Next, Result>::Type;
					};
					template<typename Result>
					struct RecChange<TypeTuple<void>, Result> {
						using Type = Result;
					};
					using Type = RecChange<>::Type;
				};
				template<typename T>
				struct Process<T, false> {
					using Type = T;
				};

				using Changed = Process<>::Type;
				using Current = Changed::Labels::Front;
				using Type = Prototype<typename Changed::InnerExpression::template Replace<Current, Arg>, typename Changed::Labels::Next>;
			};
			using Type = LabelDealer<>::Type;
		};
		template<Valid Arg>
		struct _Apply<Arg, true> {
			using Type = InnerExpression::template Apply<Arg>;
		};
		template<bool Boolean>
		struct _Apply<None, Boolean> {
			using Type = Expression;
		};

		template<Label old, Valid nov, bool = Uncovered::template find<old>(), bool = Labels::template find<nov>()>
		struct AssociatedChange {
			using Type = Expression;
		};
		template<Label old, Valid nov>
		struct AssociatedChange<old, nov, true, false> {
			using Type = Prototype<typename InnerExpression::template Change<old, nov>>;
		};
		template<Label old, Valid nov>
		struct AssociatedChange<old, nov, true, true> {
			using NewLabel = CreateNewLabel<TypeTuple<nov>, Labels, Uncovered>::Type;
			using LabelAfterProcess = Alter<Find<nov, Labels>::loc, NewLabel, Labels>::Type;
			using AfterProcess = Prototype<typename InnerExpression::template Change<nov, NewLabel>, LabelAfterProcess>;
			using Type = Prototype<typename InnerExpression::template Change<old, nov>>;
		};

		template<Label label, Valid Arg, bool = Uncovered::template find<label>()>
		struct _Replace {
			using Type = Expression;
		};
		template<Label label, Valid Arg>
		struct _Replace<label, Arg, true> {
			using Type = Prototype<typename InnerExpression::template Replace<label, Arg>>;
		};

		using Call = typename Prototype<>::RemoveBracket::Type;
		template<Label label, Valid Arg>
		using Replace = _Replace<label, Arg>::Type;
		template<Label old, Valid nov>
		using Change = AssociatedChange<old, nov>::Type;
		template<Valid Arg>
		using Apply = _Apply<Arg>::Type;
		template<Valid...Args>
		using Calculate = LambdaCalculus::template Calculate<Call, Args...>;

		static std::string print() {
			auto inner = InnerExpression::print().substr();
			if constexpr (InnerExpression::type == BasicType::Conjunction)
				inner = inner.substr(1, inner.length() - 2);
			return "(" + Order<labels...>::print() + "." + inner + ")";
		}
	};

	template<unsigned n, Valid F, Valid X>
	struct __N {
		template<int index = 0, bool = index == n>
		struct Rec {
			using Type = Compose<F, typename Rec<index + 1>::Type>;
		};
		template<int index>
		struct Rec<index, true> {
			using Type = X;
		};
		using Type = Rec<>::Type;
	};

	template<unsigned n, Valid F, Valid X>
	using _N = __N<n, F, X>::Type;

	using True = Expression<Order<L<'a'>, L<'b'>>, L<'a'>>;
	using False = Expression<Order<L<'a'>, L<'b'>>, L<'b'>>;
	using If = Expression<Order<L<'p'>, L<'a'>, L<'b'>>, Compose<L<'p'>, L<'a'>, L<'b'>>>;
	using Identity = Expression<Order<L<'x'>>, L<'x'>>;
	using S = Expression<Order<L<'f'>, L<'g'>, L<'x'>>, Compose<L<'f'>, L<'x'>, Compose<L<'g'>, L<'x'>>>>;
	using K = Expression<Order<L<'x'>, L<'y'>>, L<'x'>>;
	using I = Identity;
	using Iota = Expression<Order<L<'f'>>, Compose<L<'f'>, S, K>>;
	using B = Expression<Order<L<'g'>, L<'h'>, L<'x'>>, Compose<L<'g'>, Compose<L<'h'>, L<'x'>>>>;
	using C = Expression<Order<L<'x'>, L<'y'>, L<'z'>>, Compose<L<'x'>, L<'z'>, L<'y'>>>;
	using W = Expression<Order<L<'x'>, L<'y'>>, Compose<L<'x'>, L<'y'>, L<'y'>>>;
	using U = Expression<Order<L<'x'>, L<'y'>>, Compose<L<'x'>, L<'x'>, L<'y'>>>;
	template<unsigned n>
	using N = Expression<Order<L<'f'>, L<'x'>>, _N<n, L<'f'>, L<'x'>>>::Call;
	using Succ = Expression<Order<L<'n'>, L<'f'>, L<'x'>>, Compose<L<'f'>, Compose<L<'n'>, L<'f'>, L<'x'>>>>;
	using Plus = Expression<Order<L<'a'>, L<'b'>, L<'f'>, L<'x'>>, Compose<L<'a'>, L<'f'>, Compose<L<'b'>, L<'f'>, L<'x'>>>>;
	using Mult = Expression<Order<L<'a'>, L<'b'>, L<'f'>>, Compose<L<'a'>, Compose<L<'b'>, L<'f'>>>>;
	using Pred = Expression<Order<L<'n'>, L<'f'>, L<'x'>>, Compose<L<'n'>, Expression<Order<L<'g'>, L<'h'>>, Compose<L<'h'>, Compose<L<'g'>, L<'f'>>>>, Expression<Order<L<'u'>>, L<'x'>>, Identity>>;
	using Sub = Expression<Order<L<'a'>, L<'b'>>, Compose<L<'b'>, Pred, L<'a'>>>;
	using IsZero = Expression<Order<L<'n'>>, Compose<L<'n'>, Expression<Order<L<'x'>>, False>, True>>;
	using Cons = Expression<Order<L<'a'>, L<'b'>, L<'p'>>, Compose<L<'p'>, L<'a'>, L<'b'>>>;
	using Car = Expression<Order<L<'x'>>, Compose<L<'x'>, True>>;
	using Cdr = Expression<Order<L<'x'>>, Compose<L<'x'>, False>>;
	/**Alternative Pred
	* using Pred_ZeroCons = Compose<Cons, N<0>, N<0>>;
	* using Pred_SuccCons = Expression<Order<L<'x'>>, Compose<Cons, Compose<Succ, Compose<Car, L<'x'>>>, Compose<Car, L<'x'>>>>;
	* using Pred = Expression<Order<L<'n'>>, Compose<Cdr, Compose<L<'n'>, Pred_SuccCons, Pred_ZeroCons>>>;
	*/
	using Fib_ZeroCons = Compose<Cons, N<0>, N<1>>;
	using Fib_SuccCons = Expression<Order<L<'x'>>, Compose<Cons, Compose<Cdr, L<'x'>>, Compose<Plus, Compose<Car, L<'x'>>, Compose<Cdr, L<'x'>>>>>;
	using Fib = Expression<Order<L<'n'>>, Compose<Cdr, Compose<L<'n'>, Fib_SuccCons, Fib_ZeroCons>>>;

	template<Valid...Args>
	struct NonExpandCompose : public LambdaCalculusBase {
		static constexpr BasicType type = BasicType::Conjunction;
		static constexpr bool unexpand = true; 
		using Labels = LabelInfoBuilder<Args...>::Type;
		using Params = TypeTuple<Args...>;
		using Simplified = ConjunctionSimplifiedTuple<Params, unexpand>::Type;

		template<typename _Args, bool = std::is_same_v<Params, _Args>>
		struct Rebind {
			using Type = NonExpandCompose;
		};
		template<Valid...Params>
		struct Rebind<TypeTuple<Params...>, false> {
			using Type = NonExpandCompose<Params...>;
		};
		template<typename Altered = Simplified>
		using Prototype = Rebind<Altered>::Type;

		using Call = Prototype<>;
		template<Label label, Valid Arg>
		using Replace = Prototype<typename ConjunctionReplacedTuple<label, Arg, Params>::Type>;
		template<Label old, Label nov>
		using Change = Replace<old, nov>;
		template<Valid Arg>
		using Apply = Compose<NonExpandCompose, Arg>::Call;
		template<Valid..._Args>
		using Calculate = LambdaCalculus::template Calculate<Call, _Args...>;

		static std::string print() {
			const auto& str = ('[' + ... + (Args::print() + ' '));
			return str.substr(0, str.length() == 1 ? 1 : str.length() - 1) + ']';
		}
	};

	//Y Combinator
	constexpr unsigned RecMax = 64;
	template<Valid Fun>
	struct _Y :public LambdaCalculusBase {
		//f(x x) f(x x)
		using InnerFunction = NonExpandCompose<Expression<Order<L<'x'>>, Compose<Fun, Compose<L<'x'>, L<'x'>>>>, Expression<Order<L<'x'>>, Compose<Fun, Compose<L<'x'>, L<'x'>>>>>;
		//n f (f(x x) f(x x))
		template<unsigned n>
		using ExpressionN = _N<n, Fun, InnerFunction>;

		static constexpr BasicType type = BasicType::Y;
		using Labels = TypeTuple<void>;

		template<Valid X>
		struct Try {
			template<Valid Last = None, unsigned n = 1>
			struct Rec {
				static_assert(n <= RecMax, "Too Much Recursion!");
				using Result = ExpressionN<n>::template Apply<X>::Call;

				template<Valid Result, bool = std::is_same_v<Result, Last>>
				struct Compare {
					using Type = Rec<Result, n + 1>::Type;
				};
				template<Valid Result>
				struct Compare<Result, true> {
					using Type = Result;
				};
				using Type = Compare<Result>::Type;
			};

			using Call = Rec<>::Type;
		};

		using Call = _Y;
		template<Valid X>
		using Apply = __If<std::is_same_v<X, None>, _Y, Try<typename X::Call>>::Type::Call;
		template<Label label, Valid T>
		using Replace = Call;
		template<Label old, Label nov>
		using Change = Call;
		template<Valid...Others>
		using Calculate = LambdaCalculus::template Calculate<Call, Others...>;

		static std::string print() {
			return InnerFunction::print();
		}
	};

	struct Y : public LambdaCalculusBase {
		//f(x x) f(x x)
		using InnerFunction = Expression<Order<L<'f'>>, NonExpandCompose<Expression<Order<L<'x'>>, Compose<L<'f'>, Compose<L<'x'>, L<'x'>>>>, Expression<Order<L<'x'>>, Compose<L<'f'>, Compose<L<'x'>, L<'x'>>>>>>;

		static constexpr BasicType type = BasicType::Y;
		using Labels = TypeTuple<void>;
		
		using Call = Y;
		template<Valid Fun>
		using Apply = __If<std::is_same_v<Fun, None>, Y, _Y<typename Fun::Call>>::Type;
		template<Label label, Valid T>
		using Replace = Call;
		template<Label old, Label nov>
		using Change = Call;
		template<Valid...Others>
		using Calculate = LambdaCalculus::template Calculate<Call, Others...>;

		static std::string print() {
			return InnerFunction::print();
		}
	};
}

#endif
