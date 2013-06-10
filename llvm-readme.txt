开始下学习llvm

读 LLVM Language Reference Manual文档

然后接着读 LLVM Programmer's Manual，此文提示需要了解STL，不得不再抽空看看"C++标准程序库"一书

Type Up-references???

一条指令的注释中yields {i32} 是啥意思？
<result> = urem i32 4, %var          ; yields {i32}:result = 4 % %var 

Embedded Metadata
Embedded metadata provides a way to attach arbitrary data to the instruction stream without affecting the behaviour of the program. There are two metadata primitives, strings and nodes. All metadata has the metadata type and is identified in syntax by a preceding exclamation point ('!').


'shufflevector' Instruction ???


'getelementptr' Instruction
Syntax:

  <result> = getelementptr <pty>* <ptrval>{, <ty> <idx>}*
  <result> = getelementptr inbounds <pty>* <ptrval>{, <ty> <idx>}*
Overview:
The 'getelementptr' instruction is used to get the address of a subelement of an aggregate data structure. It performs address calculation only and does not access memory.

struct RT {
  char A;
  int B[10][20];
  char C;
};
struct ST {
  int X;
  double Y;
  struct RT Z;
};

int *foo(struct ST *s) {
  return &s[1].Z.B[5][13];
}
----------------------------
%RT = type { i8 , [10 x [20 x i32]], i8  }
%ST = type { i32, double, %RT }

define i32* @foo(%ST* %s) {
entry:
  %reg = getelementptr %ST* %s, i32 1, i32 2, i32 1, i32 5, i32 13
  ret i32* %reg
}

含义
In the example above, the first index is indexing into the '%ST*' type, which is a pointer, yielding a '%ST' = '{ i32, double, %RT }' type, a structure. The second index indexes into the third element of the structure, yielding a '%RT' = '{ i8 , [10 x [20 x i32]], i8 }' type, another structure. The third index indexes into the second element of the structure, yielding a '[10 x [20 x i32]]' type, an array. The two dimensions of the array are subscripted into, yielding an 'i32' type. The 'getelementptr' instruction returns a pointer to this element, thus computing a value of 'i32*' type.

Note that it is perfectly legal to index partially through a structure, returning a pointer to an inner element. Because of this, the LLVM code for the given testcase is equivalent to:

  define i32* @foo(%ST* %s) {
    %t1 = getelementptr %ST* %s, i32 1                        ; yields %ST*:%t1
    %t2 = getelementptr %ST* %t1, i32 0, i32 2                ; yields %RT*:%t2
    %t3 = getelementptr %RT* %t2, i32 0, i32 1                ; yields [10 x [20 x i32]]*:%t3
    %t4 = getelementptr [10 x [20 x i32]]* %t3, i32 0, i32 5  ; yields [20 x i32]*:%t4
    %t5 = getelementptr [20 x i32]* %t4, i32 0, i32 13        ; yields i32*:%t5
    ret i32* %t5
  }


'phi' Instruction???
Syntax:

  <result> = phi <ty> [ <val0>, <label0>], ...
Overview:

The 'phi' instruction is used to implement the φ node in the SSA graph representing the function.

-------------------------
-------------------------
LLVM Programmer's Manual

Sentinels ???


Basic Recursive Type Construction这一节讲了如何构造递归结构
The waymarking algorithm ???

Advanced Topics的其它部分没有太明白???
