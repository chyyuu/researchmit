book "the principles of model checking"
P915 "A.3 Propositional Logic"

Given is a finite set AP of atomic propositions, sometimes also called propositional symbols.
In the following, Latin letters like a, b, and c (with or without subscripts) are used to denote elements of AP. The set of propositional logic formulae over AP, formulae for short, is
inductively defined by the following four rules:

1. true is a formula.
2. Any atomic proposition a ∈ AP is a formula.
3. If Φ1, Φ2 and Φ are formulae, then so are (¬Φ) and (Φ1 ∧ Φ2).
4. Nothing else is a formula.


 Formally,
an evaluation for AP is a function μ : AP → { 0, 1 }. Eval(AP) denotes the set of all
evaluations for AP. The semantics of propositional logic is specified by a satisfaction
relation |= indicating the evaluations μ for which a formula Φ is true. Formally, |= is a set
of pairs (μ, Φ) where μ is an evaluation and Φ is a formula. It is written

μ |= true 
μ |= a          iff μ(a) = 1
μ |= ¬Φ         iff μ |= Φ
μ |= Φ ∧ Ψ      iff μ |= Φ and μ |= Ψ.

Figure A.2: The satisfaction relation |= of propositional logic.


A transition system TS is a tuple (S, Act, →, I, AP, L) where
• S is a set of states,
• Act is a set of actions,
• −→ ⊆ S × Act × S is a transition relation,
• I ⊆ S is a set of initial states,
• AP is a set of atomic propositions, and
• L : S → 2AP is a labeling function.


Example: Beverage Vending Machine
 S = { pay , select, soda , beer }.
 
Act = { insert coin, get soda, get beer , τ }.

Some example transitions are:

     insert_coin   
pay −−−−−−−−−−−→ select

AND
       get_beer
beer−−−−−−−−−−−−→ pay 

 I = { pay }. 

AP = { paid , drink }
The atomic propositions in the transition system depend on the properties under con-
sideration.A simple choice is to let the state names act as atomic propositions, i.e.,
L(s) = { s } for any state s. If, however, the only relevant properties do not refer to the
selected beverage, as in the property
“The vending machine only delivers a drink after providing a coin”,
it suffices to use the two-element set of propositions AP = { paid , drink } with labeling
function:

L(pay) = ∅, L(soda) = L(beer ) = { paid , drink }, L(select) = { paid }.

Here, the proposition paid characterizes exactly those states in which the user has already
paid but not yet obtained a beverage.

The set of propositions AP is always chosen depending on the characteristics of interest. 




Program Graph (PG)
A program graph PG over set Var of typed variables is a tuple (Loc, Act, Effect, →, Loc0 , g0 ) where

• Loc is a set of locations and Act is a set of actions,
• Effect : Act × Eval(Var) → Eval(Var) is the effect function,
• → ⊆ Loc × Cond(Var) × Act × Loc is the conditional transition relation,
• Loc0 ⊆ Loc is a set of initial locations,
• g0 ∈ Cond(Var) is the initial condition.


Example 2.14.
 Beverage Vending Machine

The graph described in Example 2.12 (page 29) is a program graph. The set of variables
is
Var = { nsoda , nbeer }

where both variables have the domain { 0, 1, . . . , max }. The set Loc of locations equals
{ start , select } with Loc0 = { start }, and

Act = { bget , sget, coin, ret coin, refill } .

The effect of the actions is defined by:
Effect(coin, η)         = η
Effect(ret coin, η)     = η
Effect(sget, η)         = η[nsoda := nsoda−1]
Effect(bget, η)         = η[nbeer := nbeer −1]
Effect(refill , η)      = [nsoda := max , nbeer := max ]   


2.2 Parallelism and Communication
TS = TS1||TS2||...||TSn
is a transition system that specifies the behavior of the parallel composition of transition
systems TS1 through TSn . 
"||" it is assumed that is a commutative and associative operator. 

A widely adopted paradigm for parallel systems is that of interleaving. 
The interleaving representation of concurrency is subject to the idea that there is a sched-
uler which interlocks the steps of concurrently executing processes according to an a priori
unknown strategy. 

Definition 2.18.  Interleaving of Transition Systems
Let TSi = (Si , Acti, →i, Ii, APi, Li ) i=1, 2, be two transition systems. The transition
system TS1 ||| TS2 is defined by:
TS1 ||| TS2 = (S1 × S2, Act1 ∪ Act2 , →, I1 × I2, AP 1 ∪ AP2 , L)

Example 2.19.

Consider the two independent traffic lights described in Example 2.17 (page 36). The
depicted transition system is actually the transition system
TS = TrLight1 ||| TrLight2
For program graphs PG1 (on Var1) and PG2 (on Var2 ) without shared variables (i.e.,
Var 1 ∩ Var2 = ∅), the interleaving operator, which is applied to the appropriate transition
systems, yields a transition system
TS(PG1 ) ||| TS(PG 2)
that describes the behavior of the simultaneous execution of PG1 and PG2 .


Remark 2.23. On Atomicity





