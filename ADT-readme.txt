Specification of Abstract data type...

在google book上找到两本书

http://books.google.com/books/about/Specification_of_abstract_data_types.html?id=L7NQAAAAMAAJ
Specification of abstract data types， Jacques Loeckx, Hans-Dieter Ehrich, Markus Wolf Wiley, 1996年12月11日
Specification of Abstract Data Types provides an authoritative introduction to the mathematical foundations of algebraic program specification. Unlike most other publications on the subject, this book does not draw on category theory, but instead tries to demystify the topic and promote its use in practical applications. It clearly distinguishes between the study of algebras, logic, specification methods and specification languages and it avoids focusing on a particular logic or a particular specification method. After an informal discussion on the design of reliable software, the book presents the main notions and properties of algebras. Next it investigates logic, introducing a general notion of logic, encompassing those commonly used. On the basis of these fundamentals it describes in some detail three specification methods and the principles of specification languages. It concludes with a case study illustrating the use of abstract data type specification in software design. While treating the subject with mathematical precision, the book contains numerous examples, exercises and comments to provide a deeper understanding of concepts discussed. It was conceived as a student textbook but will also be a useful source of reference for researchers and developers using formal specification methods for software design.
在mit lib中也有

1 state-oriented specifications
State-oriented specifications allows one to spedify pieces of programs wirtten in an imperative programming language. 
{precondition} P {postcondition}

2 axiomatic specifications
An axiomatic speicification consists of a set of  formulas of, for instance, predicate logic. Sets of formulas are classically used in logci and in mathematics to define “theories”, such as group theory or Peano arithmetic. The formulas are then called axioms of the theory.

讲了对ADT的介绍，在对软件进行形式化规范（formal specification）方面有两e种方法：
1 state-oriented specifications
细化：
A method based on modularization on the basis of control flow and on the use of state-oriented specifications.
介绍了有 VDM(Jon90), Z(Spi89), RAISE[Gro92], VDM-SL[MID-93]， 类似hoar logic, 
A mehtod based on modularization on the basis of imperative data types and on the use of state-oriented specifications

：
A many-sorted algebra consists of sets and functions. In fixing the names of these sets and these functions, a signature may be viewed as the syntax of an algebra.
Informally, a sort denotes (i.e. is a name of ) a type and an operation denotes a function.
讲了signature的概念：一些函数的参数类型定义加上对函数操作的语法描述

A many-sorted algebra assigns a meaning to a signature by associating a set of data to each sort and a function to each operation.

Homomorphisms: constitue mapping between algebras or, more precisely, mappings between their carrier sets that “respect” their functions.
对homomorphism 和 isomorphism有定义

Isomorphism is bijective homomorphism

Logic
讲解了algebra logic，并具体有predicate logic, equational logic, conditional equational logic.
定义了model, theory...

后面讲了如何构建小的规范，和用specification languages 建立大的规范。
还提到了rapid prototype.  9.10  adding an environment ，即 env specification

第十章 modularization and parameterization 

另外一本
http://books.google.com/books?id=hJ6IOaiHVYUC&printsec=frontcover&hl=zh-CN#v=onepage&q&f=false
Abstract Data Types: Specifications, Implementations, and Applications
 作者：Nell B. Dale,Henry Mackay Walker
This text expands the traditional course focus to examine not only the structure of a data object, but also its type. This broader focus requires a new paradigm for classifying data types. Within each classification, the different ADTs are presented using axiomatic specifications. Various implementation alternatives are discussed for each ADT and algorithms are written in a pseudo-code based on the Pascal-Modula- 2-Ada model. Next, the Big-O complexity of each implementation is discussed and each ADT is used in an application. Classic algorithms provide applications for some of the ADTs; implementation of a previously defined ADT is the application for others. The result is a clear, logical presentation that gives students a solid, practical foundation in current software engineering principles. Applications are included to demonstrate how the ADTs are used in problem-solving. Proven pedagogical features such as detailed examples, highlighted definitions, numerous illustrations, and exercises teach problem-solving skills.


提到了一个概念 “范畴论”，有一本电子书，有空可看看
范畴论, 贺伟, 科学出版社, 2006.djvu

Procedures differ from functions in that they may have side-effects and need not have a value..
