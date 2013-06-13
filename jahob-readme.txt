 transitive closure 传递闭包
 
1 Verification of Semantic Commutativity Conditions and Inverse Operations on Linked Data Structures PLDI11

An Integrated Proof Language for Imperative Programs PLDI09
 

 Full Functional Verification of Linked Data Structures  PLDI08 
 abstract: 
 We present the first verification of full functional correctness for
a range of linked data structure implementations, including muta-
ble lists, trees, graphs, and hash tables. Specifically, we present the
use of the Jahob verification system to verify formal specifications,
written in classical higher-order logic, that completely capture the
desired behavior of the Java data structure implementations (with
the exception of properties involving execution time and/or mem-
ory consumption). Given that the desired correctness properties in-
clude intractable constructs such as quantifiers, transitive closure,
and lambda abstraction, it is a challenge to successfully prove the
generated verification conditions.
Our Jahob verification system uses integrated reasoning to split
each verification condition into a conjunction of simpler subformu-
las, then apply a diverse collection of specialized decision proce-
dures, first-order theorem provers, and, in the worst case, interac-
tive theorem provers to prove each subformula. Techniques such as
replacing complex subformulas with stronger but simpler alterna-
tives, exploiting structure inherently present in the verification con-
ditions, and, when necessary, inserting verified lemmas and proof
hints into the imperative source code make it possible to seam-
lessly integrate all of the specialized decision procedures and theo-
rem provers into a single powerful integrated reasoning system. By
appropriately applying multiple proof techniques to discharge dif-
ferent subformulas, this reasoning system can effectively prove the
complex and challenging verification conditions that arise in this
context.

Our specifications use abstract sets and relations to characterize the
abstract state of the data structure.

A verified abstraction function
establishes the correspondence between the concrete values that the
implementation manipulates when it executes and the abstract sets
and relations in the specification. 

Method preconditions and post-
conditions written in classical higher-order logic use these abstract
sets and relations to express externally observable properties of the
data structures.

高阶逻辑（HOL）的好处： 感觉 Set 和 relation是关键
1 quantifiers for invariants in programs that manipulate an un-
bounded number of objects,
2 a notation for sets and relations, which we use to concisely
specify data structure interfaces,
3 transitive closure, which is essential for specifying important
properties of recursive data structures,
4 the cardinality operator, which is suitable for specifying numer-
ical properties of data structures, and
5 lambda abstraction, which can represent definitions of per-
object specification fields and is useful for parameterized short-
hands.
 
jahob的限制 Limitations
1。 we assume that each data structure operation executes atomically.
For this assumption to hold in concurrent settings, some form of
synchronization would be required. 
2。 Our current system also does not support dynamic class loading, exceptions, or dynamic dispatch.  ???
3。 We currently model numbers as algebraic quantities with
unbounded precision and assume that object allocation always suc-
cessfully produces a new object. 
4。  we make no attempt to verify any property related to the running time or the memory consumption of the data structure implementation.In particular, we do not attempt to verify the absence of infinite loops or memory leaks.

4.1  Representation of Program Memory 对内存空间的表示

It converts field and
array assignments into assignments of global variables whose right-
hand side contains function update expressions. 
状态： The state of a Jahob program is given by a finite number of con-
crete and specification variables.

type (类型)：
 Static reference variables become
variables of type obj (obj is the type of all object identifiers)

field (即我理解的C语言中的struct中的filed)
An instance variable f in a class declaration class C {D f} becomes a
function f :: obj ⇒ obj mapping object identifiers to object iden-
tifiers. 

 The Java expression x.f becomes f x, that is, the function f
applied to x. Jahob represents Java class information using a set
of objects for each class. For example, Jahob generates the axiom
∀x.x ∈ C → f x ∈ D for the above field f . 

array的表示：
 Jahob represents an object-valued array
as a function of type obj ⇒ int ⇒ obj, which accepts an array
and an index and returns the value of the array at the index. Jahob
also introduces a function of type obj ⇒ int that indicates the ar-
ray size, and uses it to generate array bounds check assertions.

在实现上：
Jahob’s transformation of Java into guarded commands 
 It converts field and array assignments into assignments of global variables whose righthand side contains function update expressions.
 Having taken the side effects into account, it transforms Java expressions into mathematical expressions in higher-order logic.

 
