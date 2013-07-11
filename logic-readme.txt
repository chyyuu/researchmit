4月-6月的一些总结
从4月1日起，通过与frans和nickolai交流，了解到commutativity的一些思想。

1 元编程
------------------
发现自己对python还不够了解，借着分析commuter,学习了python，但发现其实现主要用到了元编程，
这个又不懂了，于是查了不少资料，对于元编程，觉得有用的资料包括：
1 “learning python” ver 4th的 part VI,中的31章“Advanced Class Topics ”, VIII的37“Managed Attributes ”,38“Decorators ”,39章“Metaclasses”专门讲解了metaclass和面向对象的一些深入内容，对python元编程讲得比较深入。
2 进一步看到ruby对元编程更加偏爱，其中一本书“Ruby元编程”写得不错，对元编程的原理、思路和实例讲解等有比较深入地分析。
3 “Python源码剖析”比较有意思，源码级讲解了python的实现，对了解python如何实现对象和进行解释执行等有较大帮助。

2 logic
-------------------
看完后，发现对"model checking"和"symbolic execution"还不够清楚，只得又看相关书籍，看不太懂，原因是忘了以前学习的数理逻辑，
找了一些书籍，觉得比较好的资料有：
0 一阶逻辑 http://en.wikipedia.org/wiki/First-order_logic  讲解了一阶逻辑的起源，历史，基本概念，挺好的
First-order logic is a formal system used in mathematics, philosophy, linguistics, and computer science. It is also known as first-order predicate calculus, the lower predicate calculus, quantification theory, and predicate logic. First-order logic is distinguished from propositional logic by its use of quantified variables.

1 "面向计算机科学的数理逻辑" ver2 有英文版"logic in computer science"，可以对照不少中英文翻译，且有例子，有基础部分”命题逻辑“，”谓词逻辑“，也讲解了一部分model checking的原理和实现技术，程序验证，模态逻辑，符号模型检测等。需要再读。

2 "软件工程形式化方法与语言"，国内写的，还没读，有基础的：”命题逻辑“，”谓词逻辑“，“集合”，“对象”，“关系”，“函数”; 还看到了比较高级的：“refinement theory”求精理论，“type theory”类型理论，“时序逻辑”

3 发现现有逻辑不太好表示堆，共享变量等，不得不进一步了解separation logic, 比较简单论文有：“An Overview of Separation Logic”-2008, “Separation Logic: A Logic for Shared Mutable Data Structures”-2001, “A Primer on Separation Logic”-2012； 书籍有“An Introduction to Separation Logic”-2008 204 pages。还需进一步了解。王老师给了三篇文章，讲seplog的语义问题，还没时间看。

4 “暗时间”一书中的Ｐ178, "康托尔、歌德尔、图灵--永恒的金色对角线"对停机问题，命令式语言（起源图灵机）与函数式语言（起源 church等的lamabda calculus）的起源差异和殊途同归，讲了Functional language的基础lamabda calculus的递归定义等，把好几个分离的东西（康托尔、歌德尔、图灵，丘齐）给串起来了，很有意思。



其它一些还没看的书或文章


[数理逻辑].(美)Herbert.B.Enderton.清晰版.pdf 讲了数理逻辑相关的基本概念，需要看！

A Logical Introduction to Proof, Daniel W. Cunningham, Springer, 2012

A Short Introduction to Hoare Logic 2008

Background reading on Hoare Logic  p133

A First Course in Logic: An introduction to model theory, proof theory, computability, and complexity (好像看过，比较基础)


3 static analysis
-------------------
由于是源码分析

1. Principles of Program Analysis, Nielson,  Nielson, Hankin, 1999, 目录见最后，电子版只有1-4章，只好到mit lib借纸版，了解了许多英文的基本概念和大致含义：
 a. "data flow analysis-- the equational approach, the constraint based approach", 
  a.1 Intraprocedural Analysis
     Live Variables Analysis
     Monotone Frameworks  ???
     Equation Solving

  a.2 Interprocedural Analysis
     Flow-Sensitivity versus Flow-Insensitivity
  
  a.3 Shape Analysis //类似指针分析 pointer-to analysis  
      Shape Graphs 

 b. Constraint Based Analysis //类似控制流分析
    Abstract 0-CFA Analysis
        Coinduction versus Induction 
    Syntax Directed 0-CFA Analysis
    Constraint Based 0-CFA Analysis  
    Adding Data Flow Analysis
  
 c. Abstract Interpretation
    A Mundane Approach to Correctness ???
    Approximation of Fixed Points ???
    Galois Connections ???
    Systematic Design of Galois Connections ???
    Induced Operations   ???
    
 d. Type and Effect Systems
    5.1 Control Flow Analysis  //这里才算控制流分析???
    5.2 Inference Algorithms ???
    5.3 Effects ???
 ...
    但觉得还有很多不懂，以后需要进一步细读。 http://books.google.com/books?id=RLjt0xSj8DcC&printsec=frontcover&hl=zh-CN#v=onepage&q&f=false 可以看到一部分内容

2. a practical theory of programming 有中文版“一种实用的程序设计理论”，提到了“函数理论”，“集合论”，对典型程序控制结构和数据结构，并发，交互（共享变量）的形式化描述，有特点的是refinement calculus的提出。

3. 程序设计语言理论 ver2 陈意云 2010. 也很深奥，讲了lamabda calculus , Coinduction(同余代数...), rewriting system(重写系统)，“类型推断”. 有时间还要看看 

也许还要看的书
Data Flow Analysis Theory and Practice， CRC, 2009

    
3 model check, SAT, SMT
--------------------

1 "Decision Procedures：An Algorithmic Point of View" 2008,  讲解如何具体实现SAT,SMT的各种算法。涉及"Decision Procedures for Propositional Logic" "Decision Procedures for Equality Logic and Uninterpreted Functions", linear arithmetic, bit vectors, array, pointer logic， quantified formulas, deciding a combination of theoris, 有一个C++的简单例子，提到SMT-LIB等. 值得再看！


2 Handbook.of.Practical.Logic.and.Automated.Reasoning,.Harrison,.CUP,.2009-use-ocaml 讲了如何具体实现自动reason，有一些细节不错，对理解SAT, SMT，COQ等是如何实现的有帮助，且用ocaml，值得深入看看。

p427,or 449 对 Craig’s interpolation theorem的讲解
If |= φ1 ⇒ φ2 then there is an ‘interpolant’ ψ, whose free variables and function and
predicate symbols occur in both φ1 and φ2, such that |= φ1 ⇒ ψ and |= ψ ⇒ φ2 .

We will find it more convenient to prove the following equivalent, which
treats the two starting formulas symmetrically and fits more smoothly into
our refutational approach.

If |= φ1 ∧ φ2 ⇒ ⊥ then there is an ‘interpolant’ ψ whose only variables and function
and predicate symbols occur in both φ1 and φ2 , such that |= φ1 ⇒ ψ and |= φ2 ⇒ ¬ψ.



3 HANDBOOK OF SATISFIABILITY， P981容量很大，涉及SAT方方面面， 14章讲了“Bounded Model Checking”， 26章讲了“SMT”


4 Concepts, Techniques, and Models of Computer Programming ，讲了如何建模 MIT教授 Peter Van Roy涉及了一个工具alloy来进行建模，有一篇文章提到用alloy 描述communativity(一个医疗系统中)

其它需要读的
[itpub.net]Introduction to Mathematics of Satisfiability-(has SAT)

The Calculus of Computation Decision Procedures with Applications to Verification 2007 有SMT方面的内容， 12章 Invariant Generation 

"Baader-Nipkow-Term rewriting and all that" 可能是讲重写系统的
附录
------------------------------------
BN: 3540654100
TITLE: Principles of Program Analysis
AUTHOR: Nielson, Flemming; Nielson, Hanne R.; Hankin, Chris
TOC:

1 Introduction 1
1.1 The Nature of Program Analysis 1
1.2 Setting the Scene 3
1.3 Data Flow Analysis 5
1.3.1 The Equational Approach 5
1.3.2 The Constraint Based Approach 8
1.4 Constraint Based Analysis 10
1.5 Abstract Interpretation 13
1.6 Type and Effect Systems 17
1.6.1 Annotated Type Systems 18
1.6.2 Effect Systems 22
1.7 Algorithms 25
1.8 Transformations 26
Concluding Remarks 29
Mini Projects 29
Exercises 31
2 Data Flow Analysis 33
2.1 Intraprocedural Analysis 33
2.1.1 Available Expressions Analysis 37
2.1.2 Reaching Definitions Analysis 41
2.1.3 Very Busy Expressions Analysis 44
2.1.4 Live Variables Analysis 47
2.1.5 Derived Data Flow Information 50
2.2 Theoretical Properties 52
2.2.1 Structural Operational Semantics 52
2.2.2 Correctness of Live Variables Analysis 57
2.3 Monotone Frameworks 63
2.3.1 Basic Definitions 65
2.3.2 The Examples Revisited 68
2.3.3 A Non-distributive Example 70
2.4 Equation Solving 72
2.4.1 The MFP Solution 72
2.4.2 The MOP Solution 76
2.5 Interprocedural Analysis 80
2.5.1 Structural Operational Semantics 83
2.5.2 Intraprocedural versus Interprocedural Analysis 86
2.5.3 Making Context Explicit 88
2.5.4 Call Strings as Context 93
2.5.5 Assumption Sets as Context 97
2.5.6 Flow-Sensitivity versus Flow-Insensitivity 99
2.6 Shape Analysis 102
2.6.1 Structural Operational Semantics 103
2.6.2 Shape Graphs 107
2.6.3 The Analysis 113
Concluding Remarks 126
Mini Projects 130
Exercises 133
3 Constraint Based Analysis 139
3.1 Abstract 0-CFA Analysis 139
3.1.1 The Analysis 141
3.1.2 Well-definedness of the Analysis 148
3.2 Theoretical Properties 151
3.2.1 Structural Operational Semantics 151
3.2.2 Semantic Correctness 156
3.2.3 Existence of Solutions 160
3.2.4 Coinduction versus Induction 163
3.3 Syntax Directed 0-CFA Analysis 166
3.3.1 Syntax Directed Specification 167
3.3.2 Preservation of Solutions 169
3.4 Constraint Based 0-CFA Analysis 171
3.4.1 Preservation of Solutions 173
3.4.2 Solving the Constraints 174
3.5 Adding Data Flow Analysis 180
3.5.1 Abstract Values as Powersets 180
3.5.2 Abstract Values as Complete Lattices 183
3.6 Adding Context Information 187
3.6.1 Uniform k-CFA Analysis 189
3.6.2 The Cartesian Product Algorithm 194
Concluding Remarks 196
Mini Projects 200
Exercises 203
4 Abstract Interpretation 209
4.1 A Mundane Approach to Correctness 209
4.1.1 Correctness Relations 212
4.1.2 Representation Functions 214
4.1.3 A Modest Generalisation 217
4.2 Approximation of Fixed Points 219
4.2.1 Widening Operators 222
4.2.2 Narrowing Operators 228
4.3 Galois Connections 231
4.3.1 Properties of Galois Connections 237
4.3.2 Galois Insertions 240
4.4 Systematic Design of Galois Connections 244
4.4.1 Component-wise Combinations 247
4.4.2 Other Combinations 251
4.5 Induced Operations 256
4.5.1 Inducing along the Abstraction Function 256
4.5.2 Application to Data Flow Analysis 260
4.5.3 Inducing along the Concretisation Function 265
Concluding Remarks 268
Mini Projects 272
Exercises 274
5 Type and Effect Systems 281
5.1 Control Flow Analysis 281
5.1.1 The Underlying Type System 282
5.1.2 The Analysis 285
5.2 Theoretical Properties 289
5.2.1 Natural Semantics 290
5.2.2 Semantic Correctness 292
5.2.3 Existence of Solutions 295
5.3 Inference Algorithms 298
5.3.1 An Algorithm for the Underlying Type System 298
5.3.2 An Algorithm for Control Flow Analysis 304
5.3.3 Syntactic Soundness and Completeness 310
5.3.4 Existence of Solutions 315
5.4 Effects 317
5.4.1 Side Effect Analysis 317
5.4.2 Exception Analysis 323
5.4.3 Region Inference 328
5.5 Behaviours 337
5.5.1 Communication Analysis 337
Concluding Remarks 347
Mini Projects 351
Exercises 357
6 Algorithms 363
6.1 Worklist Algorithms 363
6.1.1 The Structure of Worklist Algorithms 366
6.1.2 Iterating in LIFO and FIFO 370
6.2 Iterating in Reverse Postorder 372
6.2.1 The Round Robin Algorithm 376
6.3 Iterating Through Strong Components 379
Concluding Remarks 382
Mini Projects 385
Exercises 387
A Partially Ordered Sets 391
A.1 Basic Definitions 391
A.2 Construction of Complete Lattices 395
A.3 Chains 396
A.4 Fixed Points 400
Concluding Remarks 402
B Induction and Coinduction 403
B.1 Proof by Induction 403
B.2 Introducing Coinduction 405
B.3 Proof by Coinduction 409
Concluding Remarks 413
C Graphs and Regular Expressions 415
C.1 Graphs and Forests 415
C.2 Reverse Postorder 419
C.3 Regular Expressions 424
Concluding Remarks 425
Index of Notation 427
Index 431
Bibliography 437
END


5 计算机编程的原理
book
concepts, Techniques, and Models of Computer Programming, Peter Van Roy, Seif Haridi, The MIT Press, 2004

Stateless and stateful programming are often called declarative and imperative
programming,

A state is a sequence of values in time that contains the interme-
diate results of a desired computation.

The sequence need only exist in the mind of the programmer. It does not need any
support at all from the computation model. This kind of state is called implicit
state or declarative state. 

Explicit state cannot be expressed in the declarative model. To have it, we extend
the model with a kind of container that we call a cell. A cell has a name, an indefinite
lifetime, and a content that can be changed. 
An explicit state in a procedure is a state whose lifetime extends
over more than one procedure call without being present in the
procedure’s arguments.


对variable的理解：
============
Variables are just shortcuts for values. also like mathematical variables.
They cannot be assigned more than once. But you can declare another
variable with the same name as a previous one. The previous variable then becomes
inaccessible. Previous calculations that used it are not changed

because there are two concepts hiding behind the word “variable”:
1. The identifier. This is what you type in. Variables start with a capital letter
and can be followed by any number of letters or digits. For example, the character
sequence Var1 can be a variable identifier.
2. The store variable. This is what the system uses to calculate with. It is part of
the system’s memory, which we call its store

The declare statement creates a new store variable and makes the variable
identifier refer to it. Previous calculations using the same identifier are not changed
because the identifier refers to another store variable.

Declarative variables
=====================
Variables in the single-assignment store are called declarative variables
Once bound, a declarative variable stays bound throughout the computation
and is indistinguishable from its value. 
Once bound, a declarative variable stays bound throughout the computation
and is indistinguishable from its value. What this means is that it can be used in
calculations as if it were the value. Doing the operation x + y is the same as doing
11 + 22, if the store is {x = 11, y = 22}.

Value store
===========
A store where all variables are bound to values is called a value store.
Another way to say this is that a value store is a persistent mapping from variables to values.

A value is a mathematical constant.

Value creation
=============
The basic operation on a store is binding a variable to a newly created value. We
will write this as xi =value. Here xi refers directly to a variable in the store (it is
not the variable’s textual name in a program!) and value refers to a value, e.g.,
314 or [1 2 3]. For example, figure 2.7 shows the store of figure 2.6 after the two
bindings:
x1 = 314
x2 = [1 2 3]

The single-assignment operation xi =value constructs value in the store and then
binds the variable xi to this value. If the variable is already bound, the operation
will test whether the two values are compatible. If they are not compatible, an error
is signaled (using the exception-handling mechanism; see section 2.7).

Variable identifiers
===================
It would be nice if we could refer to a
store entity from outside the store. This is the role of variable identifiers. A variable
identifier is a textual name that refers to a store entity from outside the store. The
mapping from variable identifiers to store entities is called an environment.

The variable names in program source code are in fact variable identifiers.


Partial values
==============
A partial value is a data structure that may contain unbound variables.
A declarative variable can be bound to several partial values,
as long as they are compatible with each other. 

 We say a set of partial values is
compatible if the unbound variables in them can be bound in such a way as to make
them all equal.  For example, person(age:25) and person(age:x) are compatible
(because x can be bound to 25), but person(age:25) and person(age:26) are
not.


Variable-variable binding
=========================
Variables can be bound to variables. For example, consider two unbound variables
x1 and x2 referred to by the identifiers X and Y. After doing the bind X=Y, we get the
situation in figure 2.14. The two variables x1 and x2 are equal to each other. 
The figure shows this by letting each variable refer to the other. We say that {x1 , x2}
form an equivalence set （From a formal viewpoint, the two variables form an equivalence class with respect to
equality.）
邏輯上等價（Logical Equivalence）

Values and types
===============
A type or data type is a set of values together with a set of operations on those
values. A value is “of a type” if it is in the type’s set.

The basic types of the declarative model are numbers (integers and floats), records
(including atoms, booleans, tuples, lists, and strings), and procedures. 
In addition to basic types, programs can define their own types. These are called
abstract data types, or ADTs. 

Dynamic/static typing
=======================
In static typing, all variable types are known at compile time.
In dynamic typing, the variable type is known only when the variable is bound.

Basic types
===========
1. Numbers. Numbers are either integers or floating point numbers. 
2. Atoms. An atom is a kind of symbolic constant that can be used as a single
element in calculations. 
3. Booleans. A boolean is either the symbol true or the symbol false
4. Records. A record is a compound data structure. It consists of a label followed by
a set of pairs of features and variable identifiers. Features can be atoms, integers, or
booleans.
5. Tuples. A tuple is a record whose features are consecutive integers starting from 1. 
6. Lists. A list is either the atom nil or the tuple  ́| ́(H T) (label is vertical bar),
where T is either unbound or bound to a list. This tuple is called a list pair or a
cons. 
7. Strings. A string is a list of character codes. 
8. Procedures. A procedure is a value of the procedure type. 
注意，如果是function, 则需要有确定的参数和返回值。但procedure不需要

Basic operations
================
X=A*B is syntactic sugar for {Number.‘×’ ́A B X}
where Number. ́* ́ is a procedure associated with the type Number.

1. Arithmetic. Floating point numbers have the four basic operations, +, -, *, and /,
with the usual meanings. Integers have the basic operations +, -, *, div, and mod,

2. Record operations. Three basic operations on records are Arity, Label, and “. ”
(dot, which means field selection). 
For example, given
X=person(name:"George" age:25)
then {Arity X}=[age name], {Label X}=person, and X.age=25. 

The call to Arity returns a list that contains first the integer features in ascending order and
then the atom features in ascending lexicographic order

3. Comparisons. The boolean comparison functions include == and \=, which can
compare any two values for equality, as well as the numeric comparisons =<, <,
>=, and >, which can compare two integers, two floats, or two atoms. Atoms are
compared according to the lexicographic order of their print representations. 

4. Procedure operations. There are three basic operations on procedures: defining
them (with the proc statement), calling them (with the curly brace notation), and
testing whether a value is a procedure with the IsProcedure function. The call
{IsProcedure P} returns true if P is a procedure and false otherwise.


A simple execution
==================
During normal execution, statements are executed one by one in textual order. Let
us look at a simple execution:
  local A B C D in
    A=11
    B=2
    C=A+B
    D=C*C
  end
This seems simple enough; it will bind D to 169. Let us see exactly what it does.
The local statement creates four new variables in the store, and makes the four
identifiers A, B, C, D refer to them. (For convenience, this extends slightly the
local statement of table 2.1.) This is followed by two bindings, A=11 and B=2.
The addition C=A+B adds the values of A and B and binds C to the result 13. The
multiplication D multiples the value of C by itself and binds D to the result 169.
This is quite simple.

Variable identifiers and static scoping
=======================================
We saw that the local statement does two things: it creates a new variable and
it sets up an identifier to refer to the variable. The identifier only refers to the
variable inside the local statement, i.e., between the local and the end. The
program region in which an identifier refers to a particular variable is called the
scope of the identifier. Outside of the scope, the identifier does not mean the same
thing. 


Procedures
===========
Procedures are one of the most important basic building blocks of any language.
We give a simple example that shows how to define and call a procedure. Here is a
procedure that binds Z to the maximum of X and Y:
    proc {Max X Y ?Z}
        if X>=Y then Z=X else Z=Y end
    end
To make the definition easier to read, we mark the output argument with a
question mark “?”. This has absolutely no effect on execution; it is just a comment.
Calling {Max 3 5 C} binds C to 5. How does the procedure work, exactly? When
Max is called, the identifiers X, Y, and Z are bound to 3, 5, and the unbound
variable referenced by C. When Max binds Z, then it binds this variable. Since C
also references this variable, this also binds C. This way of passing parameters is
called call by reference. Procedures output results by being passed references to
unbound variables, which are bound inside the procedure. This book mostly uses
call by reference, both for dataflow variables and for mutable variables. Section 6.4.4
explains some other parameter-passing mechanisms.


Static/dynamic scope
Static scope. The variable corresponding to an identifier occurrence is the one
defined in the textually innermost declaration surrounding the occurrence in the
source program.
Dynamic scope. The variable corresponding to an identifier occurrence is the one
in the most-recent declaration seen during the execution leading up to the current
statement.



function:
=========
We do this by defining a function:
declare
    fun {Fact N}
        if N==0 then 1 else N*{Fact N-1} end
    end
The declare statement creates the new variable Fact. The fun statement defines
a function. The variable Fact is bound to the function. 

list:
==========

A list is actually a chain of links, where each
link contains two things: one list element and a reference to the rest of the chain.
Lists are always created one element at a time, starting with nil and adding links
one by one. A new link is written H|T, where H is the new element and T is the
old part of the chain. Let us build a list. We start with Z=nil . We add a first link
Y=7|Z and then a second link X=6|Y. Now X references a list with two links, a list
that can also be written as [6 7]
The link H|T is often called a cons, a term that comes from Lisp.2 We also call
it a list pair. Creating a new link is called consing. If T is a list, then consing H and
T together makes a new list H|T:

Correctness:
===========
A program is correct if it does what we would like it to do. How can we tell whether
a program is correct?

To prove correctness in general, we have
to reason about the program. This means three things:

1. We need a mathematical model of the operations of the programming language,
defining what they should do. This model is called the language’s semantics.

2. We need to define what we would like the program to do. Usually, this is a
mathematical definition of the inputs that the program needs and the output that
it calculates. This is called the program’s specification.

3. We use mathematical techniques to reason about the program, using the seman-
tics. We would like to demonstrate that the program satisfies the specification.

Eager/Lazy evaluation

The functions we have written so far will do their calculation as soon as they are
called. This is called eager evaluation. 

In lazy evaluation, a calculation is done only when the
result is needed. 
This is one of the advantages of lazy evaluation: we
can calculate with potentially infinite data structures without any loop boundary
conditions.

Explicit state
==============

How can we let a function learn from its past? That is, we would like the function
to have some kind of internal memory, which helps it do its job. Memory is needed
for functions that can change their behavior and learn from their past. This kind of
memory is called explicit state. 

A memory cell

There are lots of ways to define explicit state. The simplest way is to define a
single memory cell. This is a kind of box in which you can put any content. Many
programming languages call this a “variable.”

There are three functions on cells: NewCell creates a new
cell, := (assignment) puts a new value in a cell, and @ (access) gets the current
value stored in the cell. Access and assignment are also called read and write. For
example:
    declare
    C={NewCell 0}
    C:=@C+1
    {Browse @C}
This creates a cell C with initial content 0, adds one to the content, and displays it.

这个也叫 cell store, 相对前面讲的一次性赋值（or bind）的变量，其对应的称为 value store.
value store 用于 lisp类的语言
cell store 用于C类的语言




object:
=========
A function with internal memory is usually called an object. 


class:
=========
What if we need more than one counter?
It would be nice to have a “factory” that can make as many counters as we need.
Such a factory is called a class.

Nondeterminism and time
======================
This is because the order in which threads access the state can
change from one execution to the next. This variability is called nondeterminism.

The difficulties occur if the nondeterminism shows up in the program, i.e., if it is
observable. An observable nondeterminism is sometimes called a race condition.

Interleaving
============
That is, threads take turns each executing a little

Atomicity
===========
An operation is atomic if no intermediate states can be observed. It seems to jump directly from the initial state
to the result state. 
