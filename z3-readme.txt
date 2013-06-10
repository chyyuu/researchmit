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

