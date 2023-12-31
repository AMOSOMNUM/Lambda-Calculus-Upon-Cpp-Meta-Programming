#ifndef __H_LAMBDA_CALCULUS__
#define __H_LAMBDA_CALCULUS__

#include "TypeTuple.hpp"
#include <string>

inline namespace LambdaCalculus {
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

	constexpr char LABEL_DEFAULT_CHARSET[] = {
		't', 'x', 'y', 'z', 'a', 'b', 'c', 'd', 'm', 'p', 'q', 'r', 'k', 'u', 'v', 'w', 'f', 'g', 'h', 'o', 'n'
	};

	template<char label>
	struct Symbol : public LambdaCalculusBase {
		static constexpr BasicType type = BasicType::Label;
		using Labels = TypeTuple<Symbol>;

		template<char NewLabel = label>
		using Prototype = Symbol<NewLabel>;

		using Call = Symbol;
		template<Label Other, Valid T>
		using Replace = __If<std::is_same_v<Symbol, Other>, T, Symbol>::Type;
		template<Label old, Label nov>
		using Change = Replace<old, nov>;
		template<Valid Arg>
		using Apply = __If<std::is_same_v<Arg, None>, Symbol, Arg>::Type;
		template<Valid...Args>
		using Calculate = LambdaCalculus::template Calculate<Symbol, Args...>;

		static std::string print() {
			return std::string() + label;
		}
	};

	template<Label label, typename Outer, typename Inner, typename Uncovered>
	constexpr bool exists = Outer::template find<label>() | Inner::template find<label>() | Uncovered::template find<label>();

	template<typename Outer, typename Inner, typename Uncovered, int index = 0, bool isExist = exists<Symbol<LABEL_DEFAULT_CHARSET[index]>, Outer, Inner, Uncovered>>
	struct CreateNewLabel {
		using Type = Symbol<LABEL_DEFAULT_CHARSET[index]>;
	};
	template<typename Outer, typename Inner, typename Uncovered, int index>
	struct CreateNewLabel<Outer, Inner, Uncovered, index, true> {
		using Type = CreateNewLabel<Outer, Inner, Uncovered, index + 1>::Type;
	};
	template<typename Outer, typename Inner, typename Uncovered>
	struct CreateNewLabel<Outer, Inner, Uncovered, sizeof(LABEL_DEFAULT_CHARSET) / sizeof(char) - 1, true> {
		using Type = Symbol<'?'>;
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
	struct Param {
		static constexpr bool empty = false;
		using Labels = TypeTuple<labels...>;
		using Front = Labels::Front;
		template<typename Next = typename Labels::Next, bool = std::is_same_v<Next, TypeTuple<void>>>
		struct _Next;
		template<Label...pack>
		struct _Next<TypeTuple<pack...>, false>
		{
			using Type = Param<pack...>;
		};
		template<typename Next>
		struct _Next<Next, true>
		{
			using Type = Param<>;
		};
		using Next = _Next<>::Type;

		static std::string print() {
			return ((std::string("\u03bb") + labels::print()) + ...);
		}
	};

	template<>
	struct Param<> {
		static constexpr bool empty = true;
		using Labels = TypeTuple<void>;

		static std::string print() {
			return "";
		}
	};

	template<typename Tuple>
	struct ToParam;

	template<Label...pack>
	struct ToParam<TypeTuple<pack...>> {
		using Type = Param<pack...>;
	};
	template<>

	struct ToParam<TypeTuple<void>> {
		using Type = Param<>;
	};

	template<char...symbols>
	using L = Param<Symbol<symbols>...>;

	template<typename Tuple, int index = 1, typename Result = TypeTuple<void>, int max = Tuple::size + 1>
	struct RemoveNone {
		using Current = Tuple::template Type<index>::Call;
		static constexpr bool isNone = std::is_same_v<Current, None>;
		using Type = RemoveNone<Tuple, index + 1, typename __If<isNone, Result, typename Merge<Result, Current>::Type>::Type>::Type;
	};

	template<typename Tuple, int index, typename Result>
	struct RemoveNone<Tuple, index, Result, index> {
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

	template<typename Tuple, bool = (Tuple::size > 1)>
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

	template<typename Tuple, bool NonExpand = false>
	struct ConjunctionSimplifiedTuple {
		using Type = TryEvaluation<typename RemoveFirstCompose<typename RemoveNone<Tuple>::Type>::Type>::Type;
	};

	template<typename Tuple>
	struct ConjunctionSimplifiedTuple<Tuple, true> {
		using Type = RemoveFirstCompose<typename RemoveNone<Tuple>::Type>::Type;
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
	struct LabelInspector<Param<labels...>, expression> {
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
	struct Expression<Param<labels...>, expression> : public LambdaCalculusBase {
		static constexpr BasicType type = BasicType::Expression;
		using Labels = Param<labels...>::Labels;
		using LabelInfo = LabelInspector<Param<labels...>, expression>;
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
			using Type = Expression<typename ToParam<Labels>::Type, Altered>;
		};
		template<typename _>
		struct _Prototype<void, _> {
			using SimplifiedExpression = Simplify::Type;
			using SimplifiedLabels = Simplify::Labels;
			using Type = Expression<typename ToParam<SimplifiedLabels>::Type, SimplifiedExpression>;
		};
		template<Valid Altered>
		struct _Prototype<Altered, void> {
			using Type = Expression<typename ToParam<Labels>::Type, Altered>;
		};
		template<typename Altered = void, typename Labels = void>
		using Prototype = _Prototype<Altered, Labels>::Type;

		template<Label old, Label nov, typename ParamAfterReplace = typename Alter<Find<old, Labels>::loc, nov, Labels>::Type>
		using SelfChange = Prototype<typename InnerExpression::template Change<old, nov>, ParamAfterReplace>;

		template<Valid Arg, bool = Param<labels...>::empty>
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
			return "(" + Param<labels...>::print() + "." + inner + ")";
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

	using True = Expression<L<'a', 'b'>, Symbol<'a'>>;
	using False = Expression<L<'a', 'b'>, Symbol<'b'>>;
	using If = Expression<L<'p', 'a', 'b'>, Compose<Symbol<'p'>, Symbol<'a'>, Symbol<'b'>>>;
	using Identity = Expression<L<'x'>, Symbol<'x'>>;
	using S = Expression<L<'f', 'g', 'x'>, Compose<Symbol<'f'>, Symbol<'x'>, Compose<Symbol<'g'>, Symbol<'x'>>>>;
	using K = Expression<L<'x', 'y'>, Symbol<'x'>>;
	using I = Identity;
	using Iota = Expression<L<'f'>, Compose<Symbol<'f'>, S, K>>;
	using B = Expression<L<'g', 'h', 'x'>, Compose<Symbol<'g'>, Compose<Symbol<'h'>, Symbol<'x'>>>>;
	using C = Expression<L<'x', 'y', 'z'>, Compose<Symbol<'x'>, Symbol<'z'>, Symbol<'y'>>>;
	using W = Expression<L<'x', 'y'>, Compose<Symbol<'x'>, Symbol<'y'>, Symbol<'y'>>>;
	using U = Expression<L<'x', 'y'>, Compose<Symbol<'x'>, Symbol<'x'>, Symbol<'y'>>>;
	template<unsigned n>
	using N = Expression<L<'f', 'x'>, _N<n, Symbol<'f'>, Symbol<'x'>>>::Call;
	using Succ = Expression<L<'n', 'f', 'x'>, Compose<Symbol<'f'>, Compose<Symbol<'n'>, Symbol<'f'>, Symbol<'x'>>>>;
	using Plus = Expression<L<'a', 'b', 'f', 'x'>, Compose<Symbol<'a'>, Symbol<'f'>, Compose<Symbol<'b'>, Symbol<'f'>, Symbol<'x'>>>>;
	using Mult = Expression<L<'a', 'b', 'f'>, Compose<Symbol<'a'>, Compose<Symbol<'b'>, Symbol<'f'>>>>;
	using Pred = Expression<L<'n', 'f', 'x'>, Compose<Symbol<'n'>, Expression<L<'g', 'h'>, Compose<Symbol<'h'>, Compose<Symbol<'g'>, Symbol<'f'>>>>, Expression<L<'u'>, Symbol<'x'>>, Identity>>;
	using Sub = Expression<L<'a', 'b'>, Compose<Symbol<'b'>, Pred, Symbol<'a'>>>;
	using IsZero = Expression<L<'n'>, Compose<Symbol<'n'>, Expression<L<'x'>, False>, True>>;
	using Cons = Expression<L<'a', 'b', 'p'>, Compose<Symbol<'p'>, Symbol<'a'>, Symbol<'b'>>>;
	using Car = Expression<L<'x'>, Compose<Symbol<'x'>, True>>;
	using Cdr = Expression<L<'x'>, Compose<Symbol<'x'>, False>>;
	/**Alternative Pred
	* using Pred_ZeroCons = Compose<Cons, N<0>, N<0>>;
	* using Pred_SuccCons = Expression<L<'x'>, Compose<Cons, Compose<Succ, Compose<Car, Symbol<'x'>>>, Compose<Car, Symbol<'x'>>>>;
	* using Pred = Expression<L<'n'>, Compose<Cdr, Compose<Symbol<'n'>, Pred_SuccCons, Pred_ZeroCons>>>;
	*/
	using Fib_ZeroCons = Compose<Cons, N<0>, N<1>>;
	using Fib_SuccCons = Expression<L<'x'>, Compose<Cons, Compose<Cdr, Symbol<'x'>>, Compose<Plus, Compose<Car, Symbol<'x'>>, Compose<Cdr, Symbol<'x'>>>>>;
	using Fib = Expression<L<'n'>, Compose<Cdr, Compose<Symbol<'n'>, Fib_SuccCons, Fib_ZeroCons>>>;

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
		using InnerFunction = NonExpandCompose<Expression<Param<Symbol<'x'>>, Compose<Fun, Compose<Symbol<'x'>, Symbol<'x'>>>>, Expression<Param<Symbol<'x'>>, Compose<Fun, Compose<Symbol<'x'>, Symbol<'x'>>>>>;
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
		using InnerFunction = Expression<Param<Symbol<'f'>>, NonExpandCompose<Expression<Param<Symbol<'x'>>, Compose<Symbol<'f'>, Compose<Symbol<'x'>, Symbol<'x'>>>>, Expression<Param<Symbol<'x'>>, Compose<Symbol<'f'>, Compose<Symbol<'x'>, Symbol<'x'>>>>>>;

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
