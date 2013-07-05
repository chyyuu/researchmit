from  http://www.cnblogs.com/RicCC/archive/2007/09/16/893606.html
=======================================
set, bag, list, map的语义
集合最重要的一点是集合的语义。Java JPA中对Set, List, Collection, Map四种集合进行了定义（Java的Collection允许bag语义），NHibernate从Hibernate移植时照搬了这些概念。但是.Net社区中这些概念比较弱，很多人对set, bag, map的说法很陌生，这也对NHibernate集合映射的使用造成一定障碍。

set
集合中的对象是唯一的，无序的，不能通过索引、key值访问，只能使用enumerator列举集合对象。
.Net没有原生的set类，所以NHibernate使用Iesi.Collections的set。
不同的set实现可能存在一些差异，因此导致set表现出来的特性不大一样，但在设计、使用上的主要原则是将set看作唯一、无序的。
Iesi.Collections中，基于System.Collections.SortedList实现的ListSet在列举集合对象时顺序跟添加到set的顺序一致，但基于System.Collections.HashTable实现的HashSet就不一致了。HashSet根据对象的GetHashCode()返回值判断对象是否相等，而ListSet则使用对象的Equals()方法进行判断，所以如果没有注意重载GetHashCode()和Equals()方法，在保证唯一性上就有问题。HybridSet是Iesi.Collections中的一个混合类型，基于System.Collections.Specialized.HybridDictionary实现，主要是出于性能的考虑，内部实现会根据集合中对象的数量，自动在ListSet和HashSet两种类型间转换。
另外Iesi.Collections中的SortedSet允许提供一个IComparer接口，这样在列举集合对象时将按照IComparer提供的方法排序。

bag:
跟set基本一样，唯一不同之处在于bag中允许重复对象。
.Net没有原生的bag类，PowerCollections中有bag实现。

list:
有序集合，可以重复，使用从0开始的整数作为索引。
.Net中的List、ArrayList、LinkedList等，ArrayList用数组实现，LinkedList用双向链表实现。

map:
无序集合，key值不可以重复，可以使用任意类型的对象作为索引。
.Net中的map类有Dictionary、SortedDictionary、HashTable、SortedList等，SortedDictionary提供了排序支持。
Java中的map有HashMap和SortedMap。

set, bag, list, map语义与System.Collections的对应关系
在System.Collections下面，IList跟list语义一致，IDictionary跟map语义一致。对于具体实现类，根据它们实现的接口来确定跟set, bag, list, map语义的对应关系。
例如上面提到SortedList现了IDictionary接口，因此是一个map实现，这使得它的名字跟语义不符。
ICollection接口对语义没有明确声明，完全由具体类的实现决定属于哪种语义。抽象类CollectionBase实现了IList、ICollection接口，因此是list语义。PowerCollections中的Bag<T>是基于ICollection实现bag语义。

集合映射中实体（Entity）跟值对象（Value Object）的区别
实体拥有独立的生命周期，拥有自己的实体标识（Entity Identifier）；而值对象的生命周期完全依附于它所属的实体，没有自己的标识（Identifier）。
最基本的值类型是.Net原生的那些value types，业务中简单的值对象常见的如Money、欧美风格的姓名等。复杂的业务中实体跟值对象有时候很难区分，并且同一个概念对象在不同系统环境中也可能不一样，例如在专业的地理信息处理系统中，地址可能是实体，而对于其它系统例如ERP、人力资源管理等，地址可能是值对象。
一些简单的原则可以用于区分实体跟值对象：
1. 生命周期。值对象不能脱离它所属的实体而存在，它在实体创建时或之后被创建，跟随所属的实体一起被销毁。这个生命周期是针对业务面而言，如果用内存中对象的生命周期去理解就会有疑惑，因此结合数据库中的记录（即持久化状态的对象）来理解这个生命周期概念更直观。
2. 共享引用。值对象不能被共享引用。以地址为例，如果user A和user B都有一个地址并且是一样的，设计为值对象时地址表中会有两条记录，一条属于user A一条属于user B，设计为实体时地址表中只有一条记录，user A跟user B都通过地址对象的标识引用。应该采用哪种方案看具体的业务需要。
粗糙的设计将值对象和实体一视同仁，稍微细致一点的设计就会出现不少值对象。
NHibernate中，<set>, <bag>, <list>, <map>等与<element>、<composite-element>结合，完成值对象的集合映射；one-to-one, many-to-one, one-to-many, many-to-many的出现则意味着实体间的关联关系。
值对象的集合映射不需要cascade，因为它的生命周期原则在定义中就已经确定了，cascade只用于实体间的生命周期关联控制。
集合映射中，值对象跟实体保存在不同的表中，但值对象不需要独立的配置文件，在实体映射文件中通过<set>, <bag>, <list>, <map>已经完全描述清楚了。而实体间的one-to-one, many-to-one, one-to-many, many-to-many关联，每个实体都拥有自己的配置文件。
===========================================

z3-sampler.pdf 简要介绍了z3基于的术语，技术基础和相关theory, 
术语 （第4页的 2小节 Preliminaries）
A propositional formula ! can be a propositional variable p or a negation
?0, a conjunction !0 ! !1, a disjunction !0 " !1, an implication !0 # !1,
or a bi-implication !0 $ !1 of smaller formulas !0,!1.

A truth assignment M for a formula ! maps the propositional variables in ! to {true, false}.

satisfies
satisfiable
equisatisfiable
conjunctive normal form (CNF)

sorts： which can be seen as abstract data types,

Many-sorted (first-order) logic： is a commonly used formalism and framework
for formulating SMT problems

A many-sorted signature： is composed of a set of sorts, a set of function symbols, and a set of predicate symbols.

Each function symbol f has associated with it an arity of the form
"1 ×. . .×"n & ", where "1, . . . ,"n," are sorts.
constant symbol： If n = 0, we say f is a constant symbol.

each predicate symbol p has associated with it an arity of the form "1 ×. . .×"n. 

propositional symbol：If n = 0, we say p is a propositional symbol.
variable：We assume a set of variables X, where each variable is associated with a sort.

predicate symbols： those function symbols that have codomain Bool. 

A term t with sort " has the form x or f(t1, . . . , tn), where x is a variable with sort ", and f is a function symbol with arity "1×. . .×"n & ", where for each i ) {1, . . . , n}, ti has sort "i.

An atom is of the form p(t1, . . . , tn) where p is a predicate symbol with arity "1 ×. . .×"n, and for each i ) {1, . . . , n}, ti is a term with sort "i.

A formula ! is an atom, or has the form ?0, !0 ! !1, !0 " !1, !0 # !1, !0 $ !1, (*x: ". !0), or (+x: ". !0), where !0,!1 are smaller formulas.
We say a variable x is free in formula ! if it is not bound by
any quantifier +, *.
A sentence is a formula without free variables.
A quantifier-free formula is a formula not containing ! or ".

A structure M for a signature ! and variables X consists of non-empty
domains |M|! for each sort in !, for each x # X with sort ", M(x) # |M|!,
for each function symbol f with arity "1 × . . . × "n % ", M(f) is a total
map from |M|!1 × . . . × |M|!n to |M|!, and for each predicate symbol p
with arity "1 × . . . × "n, M(p) is a subset of |M|!1 × . . . × |M|!n.

The interpretation of a term t is given by M[[x]] = M(x) and M[[f(t1, . . . , tn)]] = M(f)(M[[t1]], . . .,M[[tn]]).

the equality =! is a builtin predicate symbol with arity " × " that does not occur in any
signature and for every structure M, M(=!) is the identity relation over
|M|!×|M|!.

A theory is essentially a set of sentences. More formally, a #-theory is a
collection of sentences over a signature #.

Given a theory T, we say ! is satisfiable modulo T if T $ {!} is satisfiable. We use M |=T ! to denote M |= {!} $T.

We say the satisfiability problem for theory T is decidable if there is a
procedure S that checks whether any quantifier-free formula is satisfiable or
not. In this case, we say S is a decision procedure for T.

技术包括
DPLL ：used in SAT
Combining Procedures： Strongly Disjoint Theories, Nelson-Oppen Combination
Nelson-Oppen combination:
Meta-Procedures:
Instantiation Based Meta-Procedures:
Rewriting Based Meta-Procedures:


theory包括
	Linear Arithmetic:
	Difference arithmetic: a fragment of linear arithmetic
	Non-linear arithmetic:
	Free functions: It is also known as the theory of uninterpreted functions.a congruence closure can be used for representing the smallest set of implied equalities This representation can be used to check if a mixture of equalities and disequalities are satisfiable.
	Bit-vectors:
	Arrays:
	-------
	theory of pairs
	basic theory of acyclic finite lists
	theory of strings: is closely related to the theory of lists
	The theory of acyclic finite recursive data-types


mcmaster07.pdf 是一个介绍z3实现的ppt
分两部分： 
1 background

也介绍了上述基本概念，还有
model

Satisfiability and Validity

各种theory
Pure Theory of Equality (EUF)
Linear Integer Arithmetic
Linear Real Arithmetic
Difference Logic
heory of Arrays
Bit-vectors
Partial orders
Tuples & Records
Algebraic data types
...

Combination of Theories

Purification:is satisfiability preserving and terminating.

Stably-Infinite Theories: A theory is stably infinite if every satisfiable QFF is satisfiable in an infinite model.

Convexity: theory T is convex iff for all finite sets Γ of literals and for all non-empty disjunctions
 i∈I  xi = yi of variables,

Nelson-Oppen Combination


2 Implementing SMT solvers
architecture:

Preprocessor/Simplifier.
SAT solver.
Blackboard: “bus” used to connect the theories.
Theories:
    Arithmetic,
    Bit-vectors,
    Arrays,
    etc.
Heuristic quantifier instantiation.


SMT-BookChapter.pdf
"Handbook of Satisfiability" 的第12章专门讲解了SMT,介绍了实现SMT的大部份技术。 有一个单独的文件SMT-BookChapter.pdf


SMT-intro-smt11.pdf
Satisfiability Modulo Theories: Introduction and Applications
简单讲解了SMT的大致原理和各种应用


SMT-manuscript.pdf
有很好的例子具体描述各种概念： signature， sorts，formula，theory. function symbols， predicate symbols 

简称
Automated Theorem Proving (ATP),
Boolean Satisfiability (SAT)
Satisfiability Modulo Theory （SMT) :studies practical methods to solve logical formulæ that have to
be interpreted modulo a certain background theory. For instance, we are interested in solving formulæ
like
(x + y ≤ 0) ∧ (x = 0) ∧ (¬a ∨ (x = 1) ∨ (y ≥ 0)) (1.1)
 
 where x, y are variables ranging over Z, a is a Boolean variable, and ∧, ∨, ¬, +, =, ≤, ≥ have the
expected logical and arithmetical interpretation. By “solving a formula” we mean finding values to the
variables in such a way that the formula is satisfied. If these values cannot be found, then we will say
that the formula is unsatisfiable. For instance, the formula (1.1) above is satisfied by the assignment
x = 0, y = −10, and a = ⊥. The following formula
(x + y ≤ 0) ∧ (x = 0) ∧ ((x = 1) ∨ (y ≥ 0)) ∧ ¬(y = 0) (1.2)
is instead unsatisfiable.



语法
---------------------
we said, the goal of SMT is to decide the satisfiability of a formula in a theory. In order to write a
formula we need to specify the syntax to be used: in particular we need some basic sorts, which can
be seen as abstract data types, and a signature Σ, a collection of function symbols.


Example 1 Consider two sorts Real and Bool and Σ = {0, 1, +, −, ≤, =} as signature. Σ contains
two 0-arity function symbols (0, 1) of sort Real, two binary function symbols (+, −) of sort Real ×
Real → Real and a binary function symbol (≤) of sort Real × Real → Bool.

From now on we shall call predicate symbols those function symbols that have codomain Bool. The
signature Σ, together with a set V = {a, b, c, . . . , x, y, z} of variables, can be used to construct terms
and formulæ. In particular, as shown in Figure 1.1, a term is either a variable (x), a 0-arity functional
symbol (c) or, recursively, the application of a function symbol or arity n to n terms. A formula is
either an atom, which could be a Boolean variable (a) or the application of a predicate symbol of arity
n to n terms or, recursively, a Boolean combination of formulæ using the classical Boolean connectives
{∧, ∨, . . .}.


Example 2 Continuing Example 1, examples of terms are
0, 1 + x, (x + y) − 0, ...

whereas examples of formulæ are
¬(0 = 1), x + y = y, x ≤ y ∧ ¬(x = y),
...


Syntactic rules to build terms and formulæ.

term := x
| c
| f( term_1, ..., term_n )

formula := a
| q( term_1, ..., term_n )
| formula_1 ∧ formula_2
| formula_1 ∨ formula_2
| formula_1 → formula_2
| ¬ formula


语义
---------------
So far we have focused on how terms and formulæ can be written; this is the syntactic level. However
in order speak about interesting concepts such as truth, satisfiability, . . . we need to assign a meaning
to terms and formulæ: this is the semantic level. The semantic is constructed as follows:
• each sort symbol s is associated to a set μ(s); the sort Bool is always associated to {Ｔ, ⊥};
• each 0-arity functional symbol c of sort s is associated to an element μ(c) ∈ μ(s);
• each n-arity functional symbol f of sort s1 × . . . × sn → s is associated to a function μ(f ) of type
μ(s1 ) × . . . × μ(sn ) → μ(s).
• each n-arity predicate symbol q of sort s1 × . . . × sn → Bool is associated to a relation μ(q) of
type μ(s1) × . . . × μ(sn).

However in SMT we don’t have to bother much about constructing this correspondence everytime,
as we always assume to work in some predefined theories where functional symbols are interpreted.
These theories are formally defined in the SMT-LIB 2.0 standard [2] and the semantic for predefined
operators and predicates is “fixed”. When an arithmetic theory is defined, the notion of +, ≤, . . . is
exactly the one we learn at elementary schools, i.e., the sort Int is matched with the set of integer
numbers Z, and the semantic for + is the following function μ(+) of type Z × Z → Z:
(0, 0) → 0
(0, 1) → 1
(1, 0) → 1
(1, 1) → 2
...


The only remaining degree of freedom for the semantic of terms and formulæ concerns uninterpreted
symbols, e.g., variables. Consider the following formula φ in a theory of integer numbers
x = y ∧ (x + 1 ≤ y ∨ x − y = 0) (1.3)
in which we have two variables x and y: as we said, all the other symbols have already a known
meaning. Therefore in order to assign a semantic to (1.3) we need to define and assignment, i.e., a
value μ(x) for x and μ(y) for y in the domain Z. A possibile assignment is μ(x) = 0 and μ(y) = 1: this
assignment can be used to evaluate φ (μ(φ)) into
 or ⊥. The recursive evaluation process is shown in Figure 1.2.
figure 1.2 推理过程
μ( x = y ∧ (x + 1 ≤ y ∨ x − y = 0) )
μ(x = y) ∧ ( μ(x + 1 ≤ y) ∨ μ(x − y = 0) )
μ(=)(μ(x), μ(y)) ∧ ( μ(≤)(μ(x + 1), μ(y)) ∨ μ(=)(μ(x − y), μ(0)) )
μ(=)(0, 1) ∧ ( μ(≤)(μ(+)(μ(x), μ(1))), 1) ∨ μ(=)(μ(−)(μ(x), μ(y)), 0) )
⊥ ∧ ( μ(≤)(μ(+)(0, 1), 1) ∨ μ(=)(μ(−)(0, 1), 0) )
⊥ ∧ ( μ(≤)(1, 1) ∨ μ(=)(−1, 0) )
⊥ ∧ (
 ∨ ⊥ )
⊥ ∧
⊥



1.3 SMT-Solving via Reduction to SAT 没有看得太明白


相关类型说明：
枚举类型： 应该可以用于ne, eq比较

Color = Datatype('Color')
Color.declare('red')
Color.declare('green')
Color.declare('blue')
Color = Color.create()

print is_expr(Color.green)
print Color.green == Color.blue
print simplify(Color.green == Color.blue)

# Let c be a constant of sort Color
c = Const('c', Color)
# Then, c must be red, green or blue
prove(Or(c == Color.green, 
         c == Color.blue,
         c == Color.red))
         
快捷方式         
Color, (red, green, blue) = EnumSort('Color', ('red', 'green', 'blue'))


set, bag类型模拟：

The following example defines several abbreviations for sort expressions
(define-sort Set (T) (Array T Bool))
(define-sort IList () (List Int))
(define-sort List-Set (T) (Array (List T) Bool))
(define-sort I () Int)

(declare-const s1 (Set I))
(declare-const s2 (List-Set Int))
(declare-const a I)
(declare-const l IList)

(assert (= (select s1 a) true))
(assert (= (select s2 l) false))
(check-sat)
(get-model)


Bags as Arrays

We can use the parametrized map function to encode finite sets and finite bags. Finite bags can be modeled similarly to sets. A bag is here an array that maps elements to their multiplicity. Main bag operations include union, obtained by adding multiplicity, intersection, by taking the minimum multiplicity, and a dual join operation that takes the maximum multiplicity. In the following example, we define the bag-union using map. Notice that we need to specify the full signature of + since it is an overloaded operator.

(define-sort A () (Array Int Int Int))
(define-fun bag-union ((x A) (y A)) A
  ((_ map (+ (Int Int) Int)) x y))
(declare-const s1 A)
(declare-const s2 A)
(declare-const s3 A)
(assert (= s3 (bag-union s1 s2)))
(assert (= (select s1 0 0) 5))
(assert (= (select s2 0 0) 3))
(assert (= (select s2 1 2) 4))
(check-sat)
(get-model)


---------------
数组元素比较
I want to describe the following problem using Z3
int []array1=new int[100];
int []array2=new int[100];
array1[0~99]={i0, i1, ..., i99}; (i0...i99 > 0)
array2[0~99]={j0, j1, ..., j99}; (j0...j99 < 0)
int i, j; (0<=i<=99, 0<=j<=99)
does array1[i]==array2[j]?

For solving this problem, Z3 will use a Brute-force approach, it will essentially try all possible combinations. It will not manage to find the "smart" proof that we (as humans) immediately see. On my machine, it takes approximately 17 secs for solving for arrays of size 100, 2.5 secs for arrays of size 50, and 0.1 secs for arrays of size 10.

However, if we encode the problem using quantifiers, it can instantaneously prove for any array size, we don't even need to specify a fixed array size. In this encoding, we say that for all i in [0, N), a1[i] > 0 and a2[i] < 0. Then, we say we want to find j1 and j2 in [0, N) s.t. a1[j1] = a2[j2]. Z3 will immediately return unsat. Here is the problem encoded using the Z3 Python API.
In the encoding above, we are using the quantifiers to summarize the information that Z3 could not figure out by itself in your encoding. The fact that for indices in [0, N) one array has only positive values, and the other one only negative values.

a1 = Array('a1', IntSort(), IntSort())
a2 = Array('a2', IntSort(), IntSort())
N  = Int('N')
i  = Int('i')
j1  = Int('j1')
j2  = Int('j2')
s = Solver()
s.add(ForAll(i, Implies(And(0 <= i, i < N), a1[i] > 0)))
s.add(ForAll(i, Implies(And(0 <= i, i < N), a2[i] < 0)))
s.add(0 <= j1, j1 < N)
s.add(0 <= j2, j2 < N)
s.add(a1[j1] == a2[j2])
print s
print s.check()



------------------------
注意，类似整数常量 1, 2, 也可以是z3的Int, 比如 IntNumRef(42) 就把python的Integer常数 42，转变成z3的Integer 42
可以在ipython中从看到
x=Int('a')

x+1
Out[26]: a + 1

(x+1).children()
Out[27]: [a, 1]

(x+1).children()[1]
Out[28]: 1

(x+1).children()[1].__class__
Out[29]: z3.IntNumRef

可以看出 1 已经是z3的interger了。



