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
		None
	};

	template<typename T>
	concept Label = std::is_base_of_v<LambdaCalculusBase, T>&& T::type == BasicType::Label;

	struct None : public LambdaCalculusBase {
		static constexpr BasicType type = BasicType::None;
		static constexpr int required = 0;

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
		'x', 'y', 'z', 'a', 'b', 'c', 'd', 'm', 'p', 'q', 'r', 't', 'u', 'v', 'w', 'f', 'g', 'h', 'o', 'n'
	};

	template<char label>
	struct L : public LambdaCalculusBase {
		static constexpr BasicType type = BasicType::Label;
		static constexpr int required = 0;
		using Labels = TypeTuple<L>;

		template<char NewLabel = label>
		using Prototype = L<NewLabel>;

		using Call = Prototype<>;
		template<Label Other, Valid T>
		using Replace = __If<std::is_same_v<Prototype<>, Other>, T, Prototype<>>::Type;
		template<Label old, Label nov>
		using Change = Replace<old, nov>;
		template<Valid Arg>
		using Apply = __If<std::is_same_v<Arg, None>, Prototype<>, Arg>::Type;
		template<Valid...Args>
		using Calculate = LambdaCalculus::template Calculate<Prototype<>, Args...>;

		static char print() {
			return label;
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
			template<typename _Left, typename _Result = TypeTuple<void>, bool = _Result::template find<_Left::Front>()>
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
			return ((std::string("��") + labels::print()) + ...);
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

	template<Valid...Args>
	struct Compose : public LambdaCalculusBase {
		static constexpr BasicType type = BasicType::Conjunction;
		using Labels = LabelInfoBuilder<Args...>::Type;
		static constexpr int required = Labels::size;
		using Params = TypeTuple<Args...>;

		template<typename Params>
		struct Simplify {
			template<typename Tuple, int index = 1, typename Result = TypeTuple<void>, int = Tuple::size>
			struct RemoveNone {
				using Current = Tuple::template Type<index>;
				static constexpr bool isNone = std::is_same_v<Current, None>;

				using Type = RemoveNone<Tuple, index + 1, typename __If<isNone, Result, typename Merge<Result, Current>::Type>::Type>::Type;
			};
			template<typename Tuple, int index, typename Result>
			struct RemoveNone<Tuple, index, Result, index> {
				using Current = Tuple::template Type<Tuple::size>;
				static constexpr bool isNone = std::is_same_v<Current, None>;
				using Type = __If<isNone, Result, typename Merge<Result, Current>::Type>::Type;
			};
			template<int index>
			struct RemoveNone<TypeTuple<void>, index, TypeTuple<void>> {
				using Type = TypeTuple<void>;
			};

			template<typename Tuple, bool = std::is_same_v<Tuple, TypeTuple<void>>>
			struct RemoveFirstCompose {
				template<typename Front = typename Tuple::Front, typename Next = typename Tuple::Next, bool = Front::type == BasicType::Conjunction>
				struct _RemoveFirstCompose {
					using Type = Tuple;
				};
				template<typename Front, typename Next>
				struct _RemoveFirstCompose<Front, Next, true> {
					using Type = Merge<typename Front::template Prototype<>::Params, Next>::Type;
				};
				using Type = _RemoveFirstCompose<>::Type;
			};
			template<typename Tuple>
			struct RemoveFirstCompose<Tuple, true> {
				using Type = Tuple;
			};

			template<typename Tuple, bool = (Tuple::size > 1)>
			struct TryEvaluation {
				template<typename Front = typename Tuple::Front, typename Arg = typename Tuple::Next::Front, typename Left = typename Tuple::Next::Next, bool = Front::type == BasicType::Label>
				struct Process {
					using Type = Merge<typename Front::template Apply<Arg>, Left>::Type;
				};
				template<typename Front, typename Arg, typename Left>
				struct Process<Front, Arg, Left, true> {
					using Type = Tuple;
				};
				using AfterProcess = Process<>::Type;

				template<typename Last = void, typename Result = AfterProcess>
				struct Rec {
					using Type = Rec<Result, typename TryEvaluation<Result>::AfterProcess>::Type;
				};
				template<typename Result>
				struct Rec<Result, Result> {
					using Type = Result;
				};

				using Type = Rec<>::Type;
			};
			template<typename Tuple>
			struct TryEvaluation<Tuple, false> {
				using AfterProcess = Tuple;
				using Type = Tuple;
			};
			using Type = TryEvaluation<typename RemoveFirstCompose<typename RemoveNone<Params>::Type>::Type>::Type;
		};

		template<typename _Args, bool = std::is_same_v<Params, _Args>>
		struct Rebind {
			using Type = Compose;
		};
		template<Valid...Params>
		struct Rebind<TypeTuple<Params...>, false> {
			using Type = Compose<Params...>;
		};
		template<typename Altered = typename Simplify<Params>::Type>
		using Prototype = Rebind<Altered>::Type;

		template<Label label, Valid Arg, typename Result = Params, int index = 1>
		struct _Replace {
			using Current = Result::template Type<index>;
			using AfterReplace = Current::template Replace<label, Arg>;
			using Type = _Replace<label, Arg, typename Alter<index, AfterReplace, Result>::Type, index + 1>::Type;
		};

		template<Label label, Valid Arg, typename Result>
		struct _Replace<label, Arg, Result, Params::size + 1> {
			using Type = Result;
		};

		using Call = Prototype<>;
		template<Label label, Valid Arg>
		using Replace = Prototype<typename _Replace<label, Arg>::Type>;
		template<Label old, Label nov>
		using Change = Replace<old, nov>;
		template<Valid Arg>
		using Apply = __If<std::is_same_v<Arg, None>, Compose, Prototype<typename Merge<Params, Arg>::Type>>::Type;
		template<Valid..._Args>
		using Calculate = LambdaCalculus::template Calculate<Prototype<>, _Args...>;

		static std::string print() {
			return ("(" + ... + (Args::print() + std::string(" "))) + std::string(")");
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

		template<typename OuterOrder, Valid T>
		struct Simplify {
			template<typename Simplified = typename T::template Prototype<>, bool = Simplified::type == BasicType::Conjunction>
			struct RemoveConjunction {
				using Type = Simplified;
			};
			template<Valid Simplified>
			struct RemoveConjunction<Simplified, true> {
				template<typename Tuple = typename Simplified::Params, int Qty = Tuple::size>
				struct GetInside {
					using Type = Simplified;
				};
				template<typename Tuple>
				struct GetInside<Tuple, 1> {
					using Type = Tuple::Front;
				};
				template<typename Tuple>
				struct GetInside<Tuple, 0> {
					using Type = None;
				};
				using Type = GetInside<>::Type;
			};

			template<typename InnerExprssion>
			struct GetLabels {
				using Type = Merge<OuterOrder, typename InnerExprssion::Labels>::Type;
			};

			template<Valid Simplified, BasicType = Simplified::type>
			struct MergeExpression {
				using Type = Simplified;
				using Labels = OuterOrder;
			};
			template<Valid Simplified>
			struct MergeExpression<Simplified, BasicType::Expression> {
				using InnerOrder = Simplified::Labels;
				template<typename Inner = InnerOrder, typename Result = Simplified>
				struct LabelChangeInspector {
					using Front = Inner::Front;
					static constexpr bool conflict = OuterOrder::template find<Front>();
					template<typename = Result, bool = conflict>
					struct Process {
						using Type = LabelChangeInspector<typename Inner::Next, Result>::Type;
					};
					template<typename Source>
					struct Process<Source, true> {
						using NewLabel = CreateNewLabel<OuterOrder, typename Source::Labels, typename Simplified::Uncovered>::Type;
						using AfterProcess = Source::template SelfChange<Front, NewLabel>;
						using Type = LabelChangeInspector<typename AfterProcess::Labels, AfterProcess>::Type;
					};
					using Type = Process<>::Type;
				};
				template<typename Result>
				struct LabelChangeInspector<TypeTuple<void>, Result> {
					using Type = Result;
				};
				using Result = LabelChangeInspector<>::Type;
				using Type = Result::InnerExpression;
				using Labels = GetLabels<Result>::Type;
			};

			using Result = MergeExpression<typename RemoveConjunction<>::Type>;
			using Type = Result::Type;
			using Labels = Result::Labels;
		};

		template<typename Altered, typename Labels>
		struct _Prototype {
			using Type = Expression<typename ToOrder<Labels>::Type, Altered>;
		};
		template<typename _>
		struct _Prototype<void, _> {
			using Result = Simplify<Labels, expression>;
			using SimplifiedExpression = Result::Type;
			using SimplifiedLabels = Result::Labels;
			using Type = Expression<typename ToOrder<SimplifiedLabels>::Type, SimplifiedExpression>;
		};
		template<Valid Altered>
		struct _Prototype<Altered, void> {
			using Type = Expression<typename ToOrder<Labels>::Type, Altered>;
		};
		template<typename Altered = void, typename Labels = void>
		using Prototype = _Prototype<Altered, Labels>::Type;

		template<Valid Arg, bool = Order<labels...>::empty>
		struct _Apply {
			template<Valid = Arg, BasicType type = Arg::type>
			struct LabelDealer {
				using Current = Labels::Front;
				using Type = Prototype<typename expression::template Replace<Current, Arg>, typename Labels::Next>;
			};
			template<Label _>
			struct LabelDealer<_, BasicType::Label> {
				template<typename T = Prototype<>, bool = Labels::template find<Arg>()>
				struct Process {
					using NewLabel = CreateNewLabel<TypeTuple<Arg>, Labels, typename T::Uncovered>::Type;
					using Type = typename T::template SelfChange<Arg, NewLabel>;
				};
				template<typename T>
				struct Process<T, false> {
					using Type = T;
				};
				using Changed = Process<>::Type;
				using Current = Changed::Labels::Front;
				using Type = Prototype<typename Changed::InnerExpression::template Replace<Current, Arg>, typename Changed::Labels::Next>;
			};
			//To Do
			//struct LabelDealer<_, BasicType::Conjunction>
			//struct LabelDealer<_, BasicType::Expression>
			using Type = LabelDealer<>::Type;
		};
		template<Valid Arg>
		struct _Apply<Arg, true> {
			using Type = expression::template Apply<Arg>;
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
			using Type = Prototype<typename expression::template Change<old, nov>>;
		};
		template<Label old, Valid nov>
		struct AssociatedChange<old, nov, true, true> {
			using NewLabel = CreateNewLabel<TypeTuple<nov>, Labels, Uncovered>::Type;
			using LabelAfterProcess = Alter<Find<nov, Labels>::loc, NewLabel, Labels>::Type;
			using AfterProcess = Prototype<typename expression::template Change<nov, NewLabel>, LabelAfterProcess>;
			using Type = Prototype<typename expression::template Change<old, nov>>;
		};

		using Call = Prototype<>;
		template<Label label, Valid Arg>
		using Replace = __If<Uncovered::template find<label>(), Prototype<typename expression::template Replace<label, Arg>>, Expression>::Type;
		template<Label old, Label nov, typename OrderAfterReplace = typename Alter<Find<old, Labels>::loc, nov, Labels>::Type>
		using SelfChange = Prototype<typename expression::template Change<old, nov>, OrderAfterReplace>;
		template<Label old, Valid nov>
		using Change = AssociatedChange<old, nov>::Type;
		template<Valid Arg>
		using Apply = _Apply<Arg>::Type;
		template<Valid...Args>
		using Calculate = LambdaCalculus::template Calculate<Prototype<>, Args...>;

		static std::string print() {
			return "(" + Order<labels...>::print() + "." + expression::print() + ")";
		}
	};

	using True = Expression<Order<L<'a'>, L<'b'>>, L<'a'>>;
	using False = Expression<Order<L<'a'>, L<'b'>>, L<'b'>>;
	using If = Expression<Order<L<'p'>, L<'a'>, L<'b'>>, Compose<L<'p'>, L<'a'>, L<'b'>>>;
	using Identity = Expression<Order<L<'x'>>, L<'x'>>;
}

#endif
