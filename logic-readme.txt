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
