-------------------------------------------
体会：
规范和逻辑是自己定的，在比较复杂的情况下，可以通过SMT等工具来帮你推出一些结论信息。
最近我一直在看一下与commuter paper相关的文章，书籍等。
以前关注的是os的implementation，对抽象，规范，形式化描述等理解不深。
我对形式逻辑，符号执行等相关基础知识要不忘了，要不理解不深入。
为此补充学习了一些一阶逻辑，静态分析的基础知识之后，再回头看我两个月前看过的文章

1 Commutativity-based concurrency control for abstract data structure-1988
2 "Semantics Based Commutativity Analysis of Object Methods" John Eberhard and Anand Tripathi (05-004.pdf)
3 Verification of semantic commutativy conditions-pldi11.pdf
5 communitivity-submit.pdf 2013

4 Exploiting the commutativity lattice. (PLDI), 2011.
提到了 
Speculative parallelization is a hot topic
Recent efforts have argued that we should use semantic conflict detection
   [Carlstrom et al. PPoPP07] [Ni et al. PPoPP07] [Kulkarni et al. PLDI07] [Herlihy & Koskinen
PPoPP08, POPL2010]

Key idea: exploit commutativity of method invocations

问题：
a) How can we implement a commutativity checker?
b) How do we know our implementation is correct?
c) How do we choose an implementation?
d) What if our implementation has too much overhead? Does not allow enough parallelism?
e) What we need is a systematic way to implement commutativity and reason about these questions

用lattice表述
a) the lower you go in the lattice, the stronger the commutativity condition
b) The less likely the condition is to prove that two methods commute
c) The more likely a transaction is to conflict with other transactions
d) If all commutativity conditions are “false” then no methods commute and all transactions are
serialized
e) Exact effect on parallelism is application dependent

结论
a) Commutativity lattice provides a framework to reason about commutativity specifications
b) Construction algorithms provide systematic way to build conflict checkers given a specification
c) Can combine lattice and algorithms to provide disciplined approach to building and selecting conflict checkers for an application



6 A comprehensive study of Convergent and Commutative Replicated Data Types 2011
 Eventual consistency aims to ensure that replicas of some mutable shared
object converge without foreground synchronisation. Previous approaches to eventual con-
sistency are ad-hoc and error-prone. We study a principled approach: to base the design of
shared data types on some simple formal conditions that are sufficient to guarantee even-
tual consistency. We call these types Convergent or Commutative Replicated Data Types
(CRDTs). This paper formalises asynchronous object replication, either state based or op-
eration based, and provides a sufficient condition appropriate for each case. It describes
several useful CRDTs, including container data types supporting both add and remove op-
erations with clean semantics, and more complex types such as graphs, montonic DAGs,
and sequences. It discusses some properties needed to implement non-trivial CRDTs.
 这篇文章虽然是针对分布式系统数据结构操作的抽象和形式化，但让我想起了oplog的设计实现思路。oplog其实也需要形式化一些东西。
 以后有时间需要深入看看。

Limits of commutativity on abstract data types ， CISMOD'92 1992
没有细看
We present some formal properties of (symmetrical) commutativity, the
major criterion used in transactional systems, which allow us to fully
understand its advantages and disadvantages. The main result is that
commutativity is subject to the same limitation as compatibility for
arbitrary objects. However, commutativity has also a number of
attracting properties, one of which is related to recovery and, to our
knowledge, has not been exploited in the literature. Advantages and
disadvantages are illustrated on abstract data types of interest. We also
show how limits of commutativity have been circumvented, which gives
guidelines for doing so (or not!).



 Automating Commutativity Analysis at the Design Level， 2004
虽然用的是alloy系统分析的一个传统的医院处理事务（也是commutativity），但其实有也说明了commutativity的应用广泛性。也许可以放到普适计算领域。

Enforcing Commutativity Using Operational Transformation  VERICO 2011
这里的shared document 是 Collaborative object, 实际讨论的是合作文档撰写问题。又是一个commutativity的应用广泛性的说明。



，体会很不一样。一个体会是：
1 抽象规范与设计实现有很大的区别和直接的联系。
2 对一个问题的理解，最好从源头读起，看看这个问题（或一系列问题）是如何一步一步解决的。

更深刻理解这个体会花了2个月. :(

----------------------------------------------------

 
commutative的规范
"Semantics Based Commutativity Analysis of Object Methods" John Eberhard and Anand Tripathi (05-004.pdf)  2005 P8  rinard-pldi11.pdf是ppt，比较直观
-----------------------------------------------------------------------------------------------------------
In his work, Weihl [16], [17], [18] identified commutativity relationships between pairs of operations. An operation
was defined to include the parameters passed to the operation as well as the output of the operation. Weihl classified
commutativity as two forms: forward and right backward commutativity. Informally, if two operations forward
commute and are defined on a given state, then the effect of executing the operations in either order is the same.
If operation β right backward commutes with operation α and it is possible to execute the operations in the order αβ,
then executing the operations in the reverse order would have had the same effect. 

使用the Method Specification Table (MST) which describes the semantics of an object’s methods
来表示Method Commutativity Specification (MCS) 

Method Specification Table (MST)包括：
   1. e precondition (P) ： is a boolean predicate of the form f (T, p 1, ..., pn ), where T represents the state of the object and p1, ..., pn represent the parameters of the method.
   2. method name (M) ：  Because a method may have different behavior under different conditions, a method may be described by more than one row.
   3. state postcondition (S)： describes the effect of the method upon the state of the object. It has the form T ′ = g(T, p1, ..., pn ), meaning the new state,
T’, of the object is determined by the function g which uses the current state of the object and the parameters of the method. 
   4. output postcondition (R)： describes the value returned by the method. This predicate has the form Vi = h(T, p1, ..., pn), where Vi is the output value and the subscript i identifies the executed method. 
例子:MST for a bank account object
P           M                 S           R
TRUE    withdraw(y)     bal’ = bal-y    V3 = OK
...

Hoare Logic expressions representing rows i and j of an MST.
    Pi{Mi}Si ∧ Ri and Pj {Mj }Sj ∧ Rj

In summary, the following expression represents the precondition and postcondition of executing the two methods
as a sequence, Mi, Mj, where Pij , Sij , and Rij are derived as explained above.
    Pi ∧ Pij {Mi; Mj }Sij ∧ Ri ∧ Rij
 



"Exploiting the Commutativity Lattice"  Milind Kulkarni, 2011 PLDI  (Verification of semantic commutativy conditions-pldi11.pdf)
-----------------------------------------------------------------------------------------------------------
A commutativity specification is a set of logical formulae that rep-
resent commutativity conditions for each pair of methods in a data
structure.
 The logic, L1, of these formulae is given in Figure 1. The
vocabulary of the logic includes the arguments and return values of
method invocations.




commutative 满足的条件：
"Semantics Based Commutativity Analysis of Object Methods" John Eberhard and Anand Tripathi   2005 P8
-----------------------------------------------------------------------------------------------------------
Consider a method group with a group precondition, PG , which defines sets of methods, where each set, M, consists
of methods m1 , m2, ..., mn . This method group is a commutative method group if for every state of the object and
every set M permitted by PG, and for all possible orderings of m 1, m2 , ..., mn, the following conditions are true.:
• CC1 (Commutativity Condition 1) :
The precondition for each method in set M is always true before the method’s execution, regardless of the order
in which it is executed.
• CC2 (Commutativity Condition 2):
The final state of the object is the same for all possible orderings of m1 , m2, ..., mn .
• CC3 (Commutativity Condition 3):
Each method’s return value remains the same, regardless of the order in which it is executed.

2) Using Hoare Logic to Analyze Commutativity: Given these three commutativity conditions, we can use the
Hoare Logic representation of rows of the MST to determine whether those rows commute. We first consider the
sequential execution of two methods. After determining the pair-wise commutativity of methods, the analysis is
extended to larger groups of methods.
For two methods Mi and Mj, the preconditions and postconditions for the two possible method sequences are the
following:
 Pi ∧ Pij{Mi ; Mj}Sij ∧ Ri ∧ Rij
 Pj ∧ Pji{Mj ; Mi}Sji ∧ Rj ∧ Rji
The two methods commute if they satisfy the three commutativity conditions. First, because of the definition of P ij
and Pji , CC1 will alway be satisfied when Pi ∧ Pij ∧ Pj ∧ Pji is true. Let PR represent this required precondition
(Pi ∧ Pij ∧ Pj ∧ P ji ). When choosing a group precondition, P G, it must be chosen such that P G ⊃ PR . In many cases
PG can be chosen to be PR .
Second, to satisfy CC2, the final state of the object for the two sequences must be the same when the precondition,
PG, is true. If Sij is of the form, T ′ = gj (gi (T, ...), ...), and Sji is of the form, T ′ = gi(gj (T, ...)
, ...), then the states are the same when gj (gi(T, ...), ...) = g i (gj (T, ...), ...). This is logically equivalent to stating
that S ij ∧ Sji is true.
Third, to satisfy CC3, the return values must be equal. If Ri is of the form, Vi = hi (T, ...), and R ji is of the form,
Vi = hi(gj (T, ...)), then the return values are equal when hi(T, ...) = hi(gj (T, ...)). In other terms, Ri ∧ Rji is true
when the precondition is true. Similarly, Rj ∧ Rij must also be true.

这样可以得出如下逻辑表示是否可以commute的判断：

1. Conjecture 1: PR ⊃ N OT (Sij ∧ Sji ∧ (Ri ∧ Rji) ∧ (Rj ∧ Rij ))  ：determine if the two methods do not commute. Two methods do not commute if the precon-
dition is always false. Otherwise, if the precondition can be true, then the methods do not commute since one of the
postconditions is false.

2. Conjecture 2: PR ⊃ (Sij ∧ Sji ∧ (Ri ∧ Rji ) ∧ (Rj ∧ Rij )) ：determine if the methods commute。 This conjecture requires that when the precondition is true, then the postconditions are true

3. Conjecture 3: PG ⊃ (Sij ∧ Sji ∧ (Ri ∧ Rji ) ∧ (Rj ∧ Rij ))：  show that CC2 and CC3 are satisfied when the group condition is true.

4. Conjecture 4: PG ⊃ (Pi ∧ Pij ∧ Pj ∧ Pji) ： verify that the group condition is stricter than the precondition required by CC1.

5. Conjecture 5: PR ∧ ( PG ) ⊃  (Sij ∧ Sji ∧ (Ri ∧ Rji ) ∧ (Rj ∧ Rij )) ： can be used to verify that the group precondition is complete.


可进一步弱化某些逻辑表示  （Weakening of Consistency），进一步提高潜在并行性：
 CC3 (Commutativity Condition 3) may be weakened.

 We can weaken CC3 by permitting the return value to differ by some degree of imprecision. Instead of requiring the return values of a method for different sequences to be equivalent, we can require that they satisfy a certain function. 

 For example, suppose that our imprecision function is weakReturn, then we can rewrite
Conjecture 3 as the following Conjecture .
Conjecture 6: PG ⊃ (Sij ∧ Sji ∧ weakReturn(Ri , Rji ) ∧ weakReturn(Rj , Rij ))



"Exploiting the Commutativity Lattice"  Milind Kulkarni, 2011  PLDI
--------------------------------------------------------------------------
DEFINITION 3. A commutativity condition, φ, is a predicate on
two method invocations m1(v1) σ1 and m2(v2 ) σ2 in an exe-
cution history H that is true only if, for all histories H ≡C H
that contain a sub-history h = m1 (v1), m2(v2) σ , m1(v1) and
m2(v2) commute with respect to σ .
In other words, a commutativity condition is a predicate φ that,
when true in one history, means that in any C -EQUIVALENT history
where the two method invocations happen back to back, the invo-
cations commute. We say that a predicate is a valid commutativity
condition if it satisfies the requirements of Definition


------------------------------
我的理解（基于CC1~CC3）：
a,b 代表两个transtraction, method, or syscall.
对于a,b 是syscall而言，a,b 的precondition 总是真

state
s'abn：表示执行完a,b后的obj内部状态（第n种）
r'an：表示执行王a后的返回值（第ｎ种）

则执行a,b 序列后return value
 a  r1 b r2
 b r2 b r1 
 
 x (r1, r2)
 x (r2, r1)

对于a的内部实现，即objs为完成a的要求而要执行的动作的返回值和状态有n,m中  
a return value:(ra1,...,ran), stats(sa1,..., sam)，
b return value:(rb1,...,rbn), stats(sb1,....,sbm)

a,b 执行完后的状态 Pij{Mi ; Mj}Sij

a 执行完后的状态和返回值： {s11, r11}, {s1n, r1n}
b 执行完后的状态和返回值： {s21, r21}, {s2n, r2n}

a,b 执行完后的状态和返回值: {s'ab1, r'a1, r'b1}, {s'abn, r'an,r'bn}
b,a 执行完后的状态和返回值: {s“ba1, r"b1, r"a1}, {s"ban, r"bn,r"an}

如果二者可以commut，则
需满足的性质：(r'a1=r"a1 AND r'b1=r"b1 AND s'ab1=s“ba1)  ...OR (r'an=r"an AND r'bn=r"bn AND s'abn=s“ban)







------------------------------


04/25/2013
目前 python spec.py fs
real	8m46.775s
user	8m45.548s
sys	0m0.620s

python spec.py fs -t t.out
下面执行太久，无法通过
mprotect mprotect
  can commute: maybe
  cannot commute, something can diverge: maybe


simsym.py
    def __getattr__(self, name):
        if name not in self._fields:
            raise AttributeError(name)
        fgetter = getattr(self.__z3_sort__, name)
        return self._fields[name]._wrap_lvalue(
            lambda: fgetter(self._getter()),
            lambda val: self.__setattr__(name, val))
是啥意思            


def z3_sort_hash(self):
    return hash(str(self))
z3.SortRef.__hash__ = z3_sort_hash
del z3_sort_hash
定义后又删除是为了啥？


========================
symtypes.py
========================
形成了通用数据结构的符号定义

构造一个 SMap类
------------------------------
def tmap(indexType, valueType):
    """Return a subclass of SMapBase that maps from 'indexType' to
    'valueType', where both must be subclasses of Symbolic."""
    # XXX We could accept a size and check indexes if indexType is an
    # ordered sort
    name = "SMap_%s_%s" % (indexType.__name__, valueType.__name__)
    sort = z3.ArraySort(indexType._z3_sort(), valueType._z3_sort())
    return type(name, (SMapBase,),
                {"_indexType" : indexType, "_valueType" : valueType,
                 "__z3_sort__" : sort})

构造一个 SStruct类
------------------------------
def tstruct(**fields):
    """Return a subclass of SStructBase for a struct type with the
    given fields.  'fields' must be a dictionary mapping from names to
    symbolic types."""

    name = "SStruct_" + "_".join(fields.keys())
    z3name = anon_name(name)
    sort = z3.Datatype(z3name)
    fieldList = fields.items()
    sort.declare(z3name, *[(fname, typ._z3_sort()) for fname, typ in fieldList])
    sort = sort.create()

    type_fields = {"__slots__": [], "_fields": fields, "_fieldList": fieldList,
                   "_z3name": z3name, "__z3_sort__": sort,
                   "_ctor": getattr(sort, z3name)}
    return type(name, (SStructBase,), type_fields)

构造SList类
--------------------------
def tlist(valueType):
    name = "SList_" + valueType.__name__
    base = tstruct(_vals = tmap(SInt, valueType), _len = SInt)
    return type(name, (base, SListBase), {})

构造SDict类
---------------------------
def tdict(keyType, valueType):
    name = "SDict_" + keyType.__name__ + "_" + valueType.__name__
    base = tstruct(_map = tmap(keyType, valueType),
                   _valid = tmap(keyType, SBool))
    return type(name, (base, SDictBase), {})

构造 SSet类
----------------------------
def tset(valueType):
    """Return a set type with the given value type."""
    name = "SSet_" + valueType.__name__
    mapType = tmap(valueType, SBool)
    base = tstruct(_bmap = mapType)
    return type(name, (base, SSetBase), {"_mapType": mapType})

构造 SBag类
-----------------------------
def tbag(valueType):
    name = "SBag_" + valueType.__name__
    mapType = tmap(valueType, SInt)
    base = tstruct(_imap = mapType)
    return type(name, (base, SBagBase), {"_valueType": valueType})
    
    

========================
fs.py
========================
pn:pathname

// File name
class SFn(simsym.SExpr, simsym.SymbolicConst):
    __z3_sort__ = z3.DeclareSort('Fn')
// Inode number
class SInum(simsym.SExpr, simsym.SymbolicConst):
    __z3_sort__ = z3.DeclareSort('Inum')
//data content in Inode
class SDataByte(simsym.SExpr, simsym.SymbolicConst):
    __z3_sort__ = z3.DeclareSort('DataByte')
// Virtual addr
class SVa(simsym.SExpr, simsym.SymbolicConst):
    __z3_sort__ = z3.DeclareSort('VA')
// Pipe Id
class SPipeId(simsym.SExpr, simsym.SymbolicConst):
    __z3_sort__ = z3.DeclareSort('PipeId')
    

dictionary SDict (keyType, valueType) {
   SMap (keyType, valueType)  _map
   SMap (keyType, SBool)      _valid
}

SDict has a _valid attr which SMap hasn't


// for proc define
struct SProc  {
   SFdMap fd_map 
   SVaMap va_map
}

// for va --> vma map in proc
dictionary SVaMap::SDict{
   SMap [Sva]=SVMA  _map    //  va --> vma
   SMap [Sva]=Sbool _valid  //  va is valid
}

// for fd-->inode map in proc
dictionary SFdMap ::SDict{
   SMap [SInt]=SFd   _map   // fd_num -->inode num
   SMap [SInt]=Sbool _valid // fd_num is valid
}

// the inode num and R/W offset of a opened file in proc
struct SFd {
   SInum  inum             // inode num (uninterpreted)
   SInt   off              // offset
}

// the mem area's vma in proc
//no mmap length of a anon mem area
struct SVMA {
 SBool anon                 // is a anonymous memory?
 SBool writable             //writable ?
 SInum inum                 // inode number if mapped a file
 SInt  off                  // read/write offset of file ???
 SDataByte anondata         // the anon memory area
}

// for file inode table, os level
map  SIMap [SInum] = SInode

// for root dir (filename-->inode number), os level
dictionary SDirMap :: SDict {
       SMap [SFn]= SInum   _map     // filename --> inode number (the index of dir entry)
       SMap [SFn]= Sbool   _valid   // filename is valid?
}

// for pipe定义，data是pipe中的tlist (可以理解为一个数据数组) ， nread？？？      
SPipe = simsym.tstruct(data = SData,
                       nread = simsym.SInt)
                       

FS (or 将来进化到OS)的model全局变量
=========================================
// for dir   
 self.i_map = SIMap.any('Fs.imap')        // SIMap = symtypes.tmap(SInum, SInode)
//root dir's inode map
self.root_dir = SDirMap.any('Fs.rootdir') // SDirMap = symtypes.tdict(SFn, SInum)
// for proc0 and proc1
 self.proc0 = SProc.any('Fs.proc0')       // SProc = symtypes.tstruct(fd_map = SFdMap,                         
 self.proc1 = SProc.any('Fs.proc1')       //                          va_map = SVaMap)
// for pipe 
 self.pipes = SPipeMap.any('Fs.pipes')    // SPipeMap = symtypes.tmap(SPipeId, SPipe)
 
 
 在代码中的assume中的表达式会被加入到 assume_list中
 也会加入到solver中的asserts中，表明一个要满足的属性
 比如
     def add_selfpid(self, pid):
        ## XXX hack due to our simplified PID model
        ## without loss of generality, assume syscall "a" happens in proc0
        if str(pid).startswith('a.'):
            simsym.assume(pid == False)
 中的 simsym.assume(pid == False)
 
 
 model.py
 =================
 
import simsym
//在fs.py中大量用到了，比如
----
    @model.methodwrap(va=SVa, pid=SPid)
    def munmap(self, va, pid):
        self.add_selfpid(pid)
        del self.getproc(pid).va_map[va]
        return ('ok',)
----
这其实是表示在执行munmap之前，需要先执行methodwrap。这是就是python decorator机制
目的是把参数符号化，记录内部变量到
internal_vars = {None: SInt.any('__dummy')} //Helpers for tracking "internal" variables

以上面函数为例
symbolic_apply --> rv = fn(*args) ==> test (*args) --> calls[0|1]-->munmap
即
r[idx] = calls[idx](s, chr(idx + ord('a')), seqname)  //s是类fs.Fs的一个object实例
 -->
    munmap（self, whichcall=(a|b), whichseq=(ab|ba))

1） 在执行 b, a调用顺序， 
 s=base()      //是类fs.Fs的一个object实例
 r={}
所以先后调用  
  r[0]=munmap(fs.Fs object, a, ab) 
  r[1]=munmap(fs.Fs object, b, ab) 
  
  all_s.append(s)
  all_r_append(r)
  
  此时记录了，a执行munmp, 然后b执行mumap 后的结果r,和状态s
  call[0](s,a,ab)
  call[1](s,b,ab)
  all_s=  [<fs.Fs object at 0x32d61b0>]
  all_r= [{0: ('ok',), 1: ('ok',)}] 这说明a/b执行munmap的某个分支执行完毕，a/b返回ok

  
2）接下来需要在执行 b, a调用顺序，
 即
   s=base()  
   r={}  
   call[1](s,b,ba)
   call[0](s,a,ba)
  
   r[1]=munmap(fs.Fs object, b, ba) 
   r[0]=munmap(fs.Fs object, a, ba) 
   all_s= [<fs.Fs object at 0x32d61b0>, <fs.Fs object at 0x31c8890>] 
   all_r= [{0: ('ok',), 1: ('ok',)}, {0: ('ok',), 1: ('ok',)}]
     
3) 执行ab,ba执行结果和状态的比较（注意，这只是二者的一个munmap执行分支的情况）
L25 : 
      if simsym.symor([all_r[0] != r for r in all_r[1:]]):
         diverge += ('results',)
      if simsym.symor([all_s[0] != s for s in all_s[1:]]):
         diverge += ('state',)
对二者做比较，看他们是否不同。如果有不同，则把diverge添加一个结果
s 比较复杂，记录了执行路径中的逻辑表达式
[Not(And(Fs.imap == Fs.imap,
        SStruct_va_map_fd_map7(SStruct__valid__map6(Store(_valid(va_map(SStruct_va_map_fd_map7(SStruct__valid__map6(Store(_valid(va_map(Fs.proc0)),
                                        a.munmap.va,
                                        False),
                                        _map(va_map(Fs.proc0))),
                                        fd_map(Fs.proc0)))),
                                        b.munmap.va,
                                        False),
                                        _map(va_map(SStruct_va_map_fd_map7(SStruct__valid__map6(Store(_valid(va_map(Fs.proc0)),
                                        a.munmap.va,
                                        False),
                                        _map(va_map(Fs.proc0))),
                                        fd_map(Fs.proc0))))),
                               fd_map(SStruct_va_map_fd_map7(SStruct__valid__map6(Store(_valid(va_map(Fs.proc0)),
                                        a.munmap.va,
                                        False),
                                        _map(va_map(Fs.proc0))),
                                        fd_map(Fs.proc0)))) ==
        SStruct_va_map_fd_map7(SStruct__valid__map6(Store(_valid(va_map(SStruct_va_map_fd_map7(SStruct__valid__map6(Store(_valid(va_map(Fs.proc0)),
                                        b.munmap.va,
                                        False),
                                        _map(va_map(Fs.proc0))),
                                        fd_map(Fs.proc0)))),
                                        a.munmap.va,
                                        False),
                                        _map(va_map(SStruct_va_map_fd_map7(SStruct__valid__map6(Store(_valid(va_map(Fs.proc0)),
                                        b.munmap.va,
                                        False),
                                        _map(va_map(Fs.proc0))),
                                        fd_map(Fs.proc0))))),
                               fd_map(SStruct_va_map_fd_map7(SStruct__valid__map6(Store(_valid(va_map(Fs.proc0)),
                                        b.munmap.va,
                                        False),
                                        _map(va_map(Fs.proc0))),
                                        fd_map(Fs.proc0)))),
        Fs.proc1 == Fs.proc1,
        Fs.pipes == Fs.pipes,
        Fs.rootdir == Fs.rootdir))]
        
        


    
这时，先调用了munmap对应的wrapped(self, whichcall, whichseq)
    self= fs.Fs object at 0x21890f0>
    whichcall='a'
    whichseq='ab'
    kwargs={'va': <class 'fs.SVa'>, 'pid': <class 'simsym.SBool'>}
    m = <function munmap at 0x218aa28>
    arg='va'
    args={}

当执行到
return m(self, **args)
args={'va': a.munmap.va, 'pid': a.munmap.pid}
这时
a.munmap.va的type是{SVa}
a.munmap.pid的type是{SBool}
即变成了调用
munmap(self, {SVa}a.munmap.va, {SBool}a.munmap.pid)
这样在执行munmap时，就已经是在符号执行了

如果参数是'internal_'开头的，则会把它放入internal_vars中internal_vars［'参数'］=参数
我理解internal表示内部变量，即执行后的state情况，对外不可见，所以从调用参数上无法体现。
其实在IsomorphicMatch中，这类变量不会处理生成same逻辑
        
执行完m(self, **args)后，由于munmap返回('ok',)，所以  wrapped也返回('ok',)     

这时回到了r[idx] = calls[idx](s, chr(idx + ord('a')), seqname)
所以r[idx]=('ok',) ,即dict r=(0:'ok',)，
由于idx取值为(0,1)
所以还要执行一次r[1] = calls[1](s, chr(1 + ord('a')), seqname)
这里意味着进程b执行了一次munmap
dict r={(0:'ok',)，(1:'ok',)}
s中包含了各Fs个符号变量的信息，包括：
i_map, proc0, proc1, root_dir, root_inum
注意，这里面除了Bool，其它都不是啥逻辑表达式。

接下来把s和r分别append保存到all_s, all_r，表示进程a执行完munmap后的状态和返回值。
这里表示进程a,b按顺序执行了一次munmap,munmap的状态和返回值
all_r={list}[{0: ('ok',), 1: ('ok',)}]
all_s={list}[<fs.Fs object at 0x21890f0>]

然后test函数再执行一次ba,即表示进程b,a按顺序执行了一次munmap,munmap的状态和返回值
创新创建了一个s       
all_r=[{0: ('ok',), 1: ('ok',)}, {0: ('ok',), 1: ('ok',)}]
all_s=[<fs.Fs object at 0x21890f0>, <fs.Fs object at 0x2192dc0>]

test函数测试完毕ab和ba，得到两次的result和state,接下来就是进行比较了。
看看这两次的r和s是否相同
结果两次比较都是相等的，所以diverage=()，所以test返回为()

这时回到了symbolic_apply中，由于schedq中新加入了一个节点（注意前面asssume和getproc函数中的if）
结果cursched中有7个节点，在symbolic_apply再次执行test函数前:
    cursched是取出的一个schedq中节点
    curschedidx设置为1
再次调用函数test时，
    all_s all_r设置为空
    s已经是新创建的了
    
在执行过程中，如果碰到了if(也就意味着有分叉)，那么需要重点看class SBool的__nonzero__函数
这其实在符号执行的每个if语句时，都会调用到这里。且if中的逻辑值会转换成z3的一个逻辑表达式
对__nonzero__函数的分析
前提：
1 有一个schedq保存了不同的执行路径（每个路径没有分叉，如果要分叉，就形成了新的执行路径）。一个执行路径是一个 list,里面每一项可以成是一个节点。其中的每个节点包含了一个逻辑值（None, True or False）和一个graphnode (用于画图，显示这个节点对应的pythn文件中的行数)。

2 在上次的某次if执行中，由于len(cursched) == curschedidx,意味着执行到了当前路径的顶点了，所以需要产生一个分叉 （solve当前的所有记录的表达式加上现在这个逻辑表达式对象，如果即可以为true，也可以为false，就要产生新执行路径）。即在schedq中push了一个newsched，但执行还是源着cursched在走。

3 考虑2中其实有了一个新的newsched,所以在symbolic_apply中，判断len(schedq)>0，于是就取出一个来，这是在 2中产生的那个list.且由于把curschedidx设置为1了，所以也就意味这要再完全按照list路径走一遍。

开始正题：
但到达某个(假设第一个)if时，会获取solver，这个solver会包含以前积累的逻辑表达式。
如果cursched（即当前执行路径）> curschedidx，这意味着不是执行路径最后的执行点，所以其if的结果直接从cursched[curschdidx]中取，并把当前的z3逻辑表达式取出，加入到z3 solver中保存（即放在solver.assertions()中）,但不用求解了。
curschedidx++，返回
“”“这样代码继续符号执行”“”，这主要在执行到assume（assume只是增加true的逻辑表达式，不会增加执行路径）和if中会再次跳回来，直到执行到执行路径的最后一个节点。最后一个节点是上次执行路径的“反”方向。这时已经是新的路径了。
“”“
如果又碰到if了，则len(cursched) == curschedidx:
 这时，也许还会产生分支，所以分别测试true和false两种可能性。
 
 “”“
    solver.add(self._v)
    canTrue = solver.check()
    
    solver.add(z3.Not(self._v))
    canFalse = solver.check()
”“”
self是SBool类的实例对象，self._v就是z3 expr
如果都有，则产生新的执行路径，并放到schedq中！

--------------------------
具体实例说明符号执行的处理过程，代码如下：

class Counter(model.Struct):
    __slots__ = ["counter"]

    def __init__(self):
        # XXX This name matters since it connects the initial counter
        # value of different Counter objects.  Will this scale to more
        # complex state?
        #self.counter = simsym.SInt.any('Counter.counter')
        self.counter = simsym.SInt.any('counter')

        # simsym.assume(self.counter == 0)
        add_pseudo_sort_decl(simsym.unwrap(self.counter).decl(), 'counter')

    @model.methodwrap()
    def sys_inc(self):
        self.counter = self.counter + 1
        return "ok"  

    @model.methodwrap()
    def sys_dec(self):
        if self.counter > 0 :
            self.counter = self.counter - 1
            return "ok"
        else:
            return "err"

分析一下执行 inc和dec的情况。进程a, 进程b
inc只有一条路径，dec有两条路径
第一次探测
1 a.inc->ok, b.dec->ok,  result=(a.ok, b.ok), final_state（表示内部的执行状态，这里用的是counter的最后的表达式，和counter的关系来表示）=   counter+1-1, counter>0
2 b.dec->ok, a.inc->ok   result=(a.ok, b.ok), final_state counter-1+1, counter>0
如果要能够commtativity，则 1.result==2.result 这里是等的; 1.final_state==2.final_state 这样需要有 counter+1-1 == counter-1+1, 且 counter>0，这里就成了 counter为1时就可以了。

第二次探测
1 a.inc->ok,  b.dec=err, ....
2 b.dec->ok,  a.inc->ok, ....
结果不一致

第三次探测
1 a.inc->ok,   b.dec=err, ....
2 b.dec->err,  a.inc->ok, ....
有可以commtativity的条件，即comter=-1

第四次探测
1 a.inc->ok,   b.dec=ok, ....
2 b.dec->err,  a.inc->ok, ....
结果不一致

这样就遍历了4次
---------------------------
def methodwrap(**kwargs):
    def decorator(m):
        def wrapped(self, whichcall, whichseq):
            args = {}
            for arg in kwargs:
                name = '%s.%s.%s' % (whichcall, m.__name__, arg)
                if arg.startswith('internal_'):
                    name += '.%s' % whichseq
                args[arg] = kwargs[arg].any(name)
                if arg.startswith('internal_'):
                    simsym.add_internal(args[arg])
            return m(self, **args)
        wrapped.__name__ = m.__name__
        return wrapped
    return decorator

==============================
在fs.py中
class Fs(model.Struct):
表明Fs继承了modle.Struct类，这样在做相等或不等操作时会调用__eq__或__ne__
__ne__仅仅是not(__eq__）
进行对field的全面符号比较。

class Struct(object):
    __slots__ = []

    def __eq__(self, o):
        if self.__class__ != o.__class__:
            return NotImplemented
        fieldeqs = [getattr(self, field) == getattr(o, field)
                    for field in self.__slots__]
        return simsym.symand(fieldeqs)

    def __ne__(self, o):
        r = (self == o)
        if r is NotImplemented:
            return NotImplemented
        return simsym.symnot(r)
        
        
        
 ============================
 MetaZ3Wrapper是一个type class，
 而SExpr设置了属性__metaclass__，结果SInt, SArith,....都需要在创建类时执行MetaZ3Wrapper的__new__函数。
 
 class SExpr(Symbolic):
    __metaclass__ = MetaZ3Wrapper
    
 -------------------   
class MetaZ3Wrapper(type):    
    def __new__(cls, classname, bases, classdict):
        if "__wrap__" in classdict:
            ref_type = classdict["__ref_type__"]
            for method in classdict.pop("__wrap__"):
                base_method = getattr(ref_type, method)
                nargs = base_method.__func__.__code__.co_argcount
                args = ["o%d" % i for i in range(nargs - 1)]
                code = "def %s(%s):\n" % (method, ",".join(["self"] + args))
                for o in args:
                    code += " if isinstance(%s, Symbolic): %s=%s._v\n" % \
                        (o, o, o)
                code += " return wrap(self._v.%s(%s))" % (method, ",".join(args))
                locals_dict = {}
                exec code in globals(), locals_dict
                classdict[method] = locals_dict[method]

        return type.__new__(cls, classname, bases, classdict)
 
 上述代码通过exec动态执行来产生了需要包裹的新类的函数。比如：
 class SExpr(Symbolic):
    __metaclass__ = MetaZ3Wrapper
    __ref_type__ = z3.ExprRef
    __wrap__ = ["__eq__", "__ne__"]
    
 所以在创建一个新类时（其实在import simsym时，就要执行这些新类的创建过程）
 执行
 def __new__(cls, classname, bases, classdict):
 cls=<class 'simsym.MetaZ3Wrapper'>
 classname='SExpr'
 classdict={'__wrap__': ['__eq__', '__ne__'], '__module__': 'simsym', '__metaclass__': <class 'simsym.MetaZ3Wrapper'>, '__str__': <function __str__ at 0x1c1f6e0>, '__ref_type__': <class z3.ExprRef at 0x1beba10>, '__repr__': <function __repr__ at 0x1c1faa0>, '_z3_value': <function _z3_value at 0x1c1fb18>}
 bases=(<class 'simsym.Symbolic'>,)
 
 当执行到exec时
 第一次
 code='def __eq__(self,o0):
 if isinstance(o0, Symbolic): o0=o0._v
 return wrap(self._v.__eq__(o0))'
 
 当执行王exec code和
 classdict[method] = locals_dict[method]
 后，
 以后SExpr类的对象执行到__eq__时，将执行这个刚定义好的__eq__
 
 第二次
 code='def __ne__(self,o0):
 if isinstance(o0, Symbolic): o0=o0._v
 return wrap(self._v.__ne__(o0))'
 解释类似
 
 这样确保都是在进行符号执行。
 
 
 -----------------------
 分析spec.py
 执行  这里m是fs.py
 L375: pseudo_sort_decls = getattr(m, 'pseudo_sort_decls', [])
 
 [(nlink, 'file-nlink'), (_len, 'file-length'), (atime, 'time'), (mtime, 'time'), (ctime, 'time'), (off, 'file-length')]
 
 L376: pseudo_sort_ignore = getattr(m, 'pseudo_sort_ignore', {})
 
 [(nlink, 'file-nlink'), (_len, 'file-length'), (atime, 'time'), (mtime, 'time'), (ctime, 'time'), (off, 'file-length')]
 
 L385     calls = m.model_functions
 [<unbound method Fs.open>, <unbound method Fs.pipe>, <unbound method Fs.pread>, <unbound method Fs.pwrite>, <unbound method Fs.read>, <unbound method Fs.write>, <unbound method Fs.unlink>, <unbound method Fs.link>, <unbound method Fs.rename>, <unbound method Fs.stat>, <unbound method Fs.fstat>, <unbound method Fs.close>, <unbound method Fs.mmap>, <unbound method Fs.munmap>, <unbound method Fs.mprotect>, <unbound method Fs.mem_read>, <unbound method Fs.mem_write>]
 
L389: for callset in itertools.combinations_with_replacement(calls, args.ncomb):
 first time: callset= (<unbound method Fs.open>, <unbound method Fs.open>)
 
simsym.py::L 810  : rv = fn(*args)


注意  目的是在每条路径上把所有的元素之间的等或不等的表达式找出来并求解。
class IsomorphicMatch(object):


for callset in itertools.combinations_with_replacement(calls, args.ncomb):          // choose one syscall set, e.g. (open, close)
...
    for result, condlist in simsym.symbolic_apply(test, base, *callset).items():    // check (open,close) and (close, open)'s all exec path
    ...    //symbolic_apply ==> rv = fn(*args) ==> test (*args) --> calls[0|1]-->munmap
    for diverge, condlist in sorted(conds.items()):                                 // print some thing...
    ...
    for e in conds[()]:               
        e = simsym.simplify(e)                                                      // e = the conditions(asserts) in a specific exec path && sat the commuter spec
        while ncond < args.max_testcases:                                           // try to get all possible different model from e  
            check, model = simsym.check(e)                                          // get a check and model, if check=sat, then this model 
            if check == z3.unsat: break
            ...
            
            vars = { model_unwrap(k, model): model_unwrap(model[k], model)          //下面这段代码生成一个testcase，并加入到testcases list中
                     for k in model
                     if '!' not in model_unwrap(k, model) }
            if args.verbose_testgen:
                print 'New assignment', ncond, ':', vars
            testcases.append({
                'calls': [c.__name__ for c in callset],
                'vars':  vars,
            })
            ncond += 1
                        
            ...
            
            same = IsomorphicMatch(model)                                           // in this model, checkout the params in syscall , 
            notsame = same.notsame_cond()                                           // 
            ...
            e = simsym.symand([e, notsame])
 
 
 
分析 IsomorphicMatch(object):
__init__(self, model):
    ...
    self.groups_changed = True
    while self.groups_changed:   //add_assignment_uninterp会改变groups_changed
       self.process_model(model)
    
    self.process_uninterp()://下面是此函数具体实现   ？？？???有些不清楚
        for sort in self.uninterps:
            groups = self.uninterp_groups(sort)
            for _, exprs in groups:
                for otherexpr in exprs[1:]:
                    self.conds.append(exprs[0] == otherexpr)
            representatives = [exprs[0] for _, exprs in groups]
            if len(representatives) > 1:
                self.conds.append(z3.Distinct(representatives))    
    
    -->process_model(model)
       for decl in model:  //注意，不处理 '!' in str(decl) or 'internal_' in str(decl) or 'dummy_' in str(decl)
          self.process_decl_assignment(decl, model[decl], model)
           
          -->process_decl_assignment(self, decl, val, model):
                dconst = decl()
                self.process_const_assignment(dconst, val, model)
               
               -->process_const_assignment(self, dconst, val, model):
                    dsort = dconst.sort()
                    分四种情况处理
                    if dsort.kind() in [z3.Z3_INT_SORT, z3.Z3_BOOL_SORT，z3.Z3_UNINTERPRETED_SORT]: 
                       self.add_assignment(dconst, val)
                    
                    if dsort.kind() == z3.Z3_DATATYPE_SORT:
                       for i in range(0, dsort.constructor(nc).arity()):
                            ...
                            self.process_const_assignment(dconst_field, childval, model)    //递归处理下层数据
                    
                    if dsort.kind() == z3.Z3_ARRAY_SORT:
                        ...
                        for fidx, fval in flist[:-1]:
                            ...
                            self.process_const_assignment(dconst[fidxrep], fval, model)     //递归处理数组元素
                            
                   if dconst.domain().kind() == z3.Z3_UNINTERPRETED_SORT:
                        univ = model.get_universe(dconst.domain())
                        ...
                        for idx in univ:
                            ...
                            self.process_const_assignment(dconst[idxrep], flist[-1], model)  //递归处理函数调用参数
                            
                            
                    -->add_assignment(self, expr, val):         
                       分四种情况讨论
                       if val.sort().kind() == z3.Z3_UNINTERPRETED_SORT:     
                            self.add_assignment_uninterp(expr, val, sort)
                       
                       for d, sortname in pseudo_sort_decls:         pseudo_sort_decls在fs.py中赋值
                        .... 如果是pseudo_sort_ignore[sortname]，直接返回不处理  pseudo_sort_ignore 在fs.py中赋值
                             如果是 not expr.decl().eq(d) continue 
                             self.add_assignment_uninterp(expr, val, sortname)
                                                          
                       如果不是上述情况，就应该是布尔表达式了，即 expr.sort().kind() == z3.Z3_BOOL_SORT:   
                       如果cond是新的，把它加入到self.conds中
                       cond = (expr == val)
                       if not any([c.eq(cond) for c in self.conds]):
                            self.conds.append(cond)  

                          --> add_assignment_uninterp(self, expr, val, sort):
                                new_group = True
                                for uexpr, uval in self.uninterps[sort]:
                                    if uval.eq(val):
                                        new_group = False
                                        if uexpr.eq(expr): return
                                if new_group:
                                    self.groups_changed = True            //如果有变化，则groups_changed为true
                                self.uninterps[sort].append((expr, val))
                                

IsomorphicMatch根据算出的model，计算处一个新的conds，比如对于mprotect,mprotect，z3 check出一个model
ModelRef: [a.mprotect.writable = False,
 b.mprotect.pid = False,
 Fs.proc0 = SStruct_va_map_fd_map7(SStruct__valid__map6(as-array,
                                        as-array),
                                   SStruct__valid__map4(as-array,
                                        as-array)),
 a.mprotect.va = VA!val!0,
 b.mprotect.va = VA!val!1,
 b.mprotect.writable = True,
 a.mprotect.pid = False,
 k!151 = [else -> False],
 k!152 = [VA!val!0 ->
          SStruct_writable_anon_off_anondata_inum5(False,
                                        False,
                                        1,
                                        DataByte!val!1,
                                        Inum!val!1),
          VA!val!1 ->
          SStruct_writable_anon_off_anondata_inum5(False,
                                        False,
                                        0,
                                        DataByte!val!0,
                                        Inum!val!0),
          else ->
          SStruct_writable_anon_off_anondata_inum5(False,
                                        False,
                                        1,
                                        DataByte!val!1,
                                        Inum!val!1)],
 k!153 = [VA!val!0 -> True, VA!val!1 -> True, else -> True],
 k!150 = [else ->
          SStruct_ispipe_pipewriter_off_pipeid_inum3(False,
                                        False,
                                        0,
                                        PipeId!val!0,
                                        Inum!val!1)]]
                                                                        
注意IsomorphicMatch中的重要成员变量
conds=[True, a.mprotect.writable == False, b.mprotect.pid == False]
uninterps=defaultdict(<type 'list'>, {VA: [(a.mprotect.va, VA!val!0)]})

group_changed=True
在成员函数add_assignment_uninterp(self, expr, val, sort):中有对group_changed=True的赋值，表明有新的uninterp的sort和变量了。

执行完
       while self.groups_changed:
            self.groups_changed = False
            self.process_model(model)
后
conds=[True, a.mprotect.writable == False, b.mprotect.pid == False, b.mprotect.writable == True, a.mprotect.pid == False, _valid(va_map(Fs.proc0))[a.mprotect.va] == True, _valid(va_map(Fs.proc0))[b.mprotect.va] == True, writable(_map(va_map(Fs.proc0))[a.mprotect.va]) == False, anon(_map(va_map(Fs.proc0))[a.mprotect.va]) == False, writable(_map(va_map(Fs.proc0))[b.mprotect.va]) == False, anon(_map(va_map(Fs.proc0))[b.mprotect.va]) == False]

uninterps=defaultdict(<type 'list'>, {VA: [(a.mprotect.va, VA!val!0), (b.mprotect.va, VA!val!1)], DataByte: [(anondata(_map(va_map(Fs.proc0))[a.mprotect.va]), DataByte!val!1), (anondata(_map(va_map(Fs.proc0))[b.mprotect.va]), DataByte!val!0)], Inum: [(inum(_map(va_map(Fs.proc0))[a.mprotect.va]), Inum!val!1), (inum(_map(va_map(Fs.proc0))[b.mprotect.va]), Inum!val!0)]})

接下来执行process_uninterp()函数，
有如下处理
  self.conds.append(z3.Distinct(representatives))
即把uninterp的变量增加一个表达式 ！=
把这些包含uninterps中的变量的!=表达式也加入到conds中，最终形成
conds=[True, a.mprotect.writable == False, b.mprotect.pid == False, b.mprotect.writable == True, a.mprotect.pid == False, _valid(va_map(Fs.proc0))[a.mprotect.va] == True, _valid(va_map(Fs.proc0))[b.mprotect.va] == True, writable(_map(va_map(Fs.proc0))[a.mprotect.va]) == False, anon(_map(va_map(Fs.proc0))[a.mprotect.va]) == False, writable(_map(va_map(Fs.proc0))[b.mprotect.va]) == False, anon(_map(va_map(Fs.proc0))[b.mprotect.va]) == False, a.mprotect.va != b.mprotect.va, anondata(_map(va_map(Fs.proc0))[a.mprotect.va]) !=
anondata(_map(va_map(Fs.proc0))[b.mprotect.va]), inum(_map(va_map(Fs.proc0))[a.mprotect.va]) !=
inum(_map(va_map(Fs.proc0))[b.mprotect.va])]


这样执行完 same = IsomorphicMatch(model) （spec.py, L448）后，获得了一个IsomorphicMatch类的对象same，即根据model生成了一个对应的表达式，其特征是
有解释的，比如int, bool类型的变量，根据model的具体值，生成一个==逻辑表达式，对于没解释的变量（这些可能是int啥的，但其实我们不关注，所以设置没解释类型），生成这些变量的!=逻辑表达式。
把 same 取反，即执行  notsame = same.notsame_cond()
这个notsame是表示下次不用这个生成的model了，我们需要生成一个新的model,所以
e = simsym.symand([e, notsame])

这样符合commuter的e也更新了，确保可以不会生成相同的model了。

  
注意fs.py中定义的fs的内部变量. m即fs.py 算是一个module
pseudo_sort_decls = getattr(m, 'pseudo_sort_decls', [])
[(nlink, 'file-nlink'), (_len, 'file-length'), (atime, 'time'), (mtime, 'time'), (ctime, 'time'), (off, 'file-length')]

pseudo_sort_ignore = getattr(m, 'pseudo_sort_ignore', {})
{'file-length': True, 'file-nlink': True, 'fd-num': False, 'time': True}

对 pseudo_sort_decls 和 pseudo_sort_ignore的分析
以counter3.py为例
pseudo_sort_decls = [
# below line can replace add_pseudo_sort_decl(simsym.unwrap(self.counter).decl(), 'counter')  IN Counter.__init__
    (simsym.unwrap(simsym.SInt.any('counter')).decl(),'counter'),
]
与 class Counter的__init__函数执行 add_pseudo_sort_decl(simsym.unwrap(self.counter).decl(), 'counter')的效果是一样的。
一个decl() 是啥意思? 在z3.py中,decal是一个z3 application的函数申明，如果是符号变量，这函数申明是符号变量本身，可看下面的例子
-----------------------------------------
    def decl(self):
        """Return the Z3 function declaration associated with a Z3 application.
        
        >>> f = Function('f', IntSort(), IntSort())
        >>> a = Int('a')
        >>> t = f(a)
        >>> eq(t.decl(), f)
        True
        >>> (a + 1).decl()
        >>> t.decl()
        Out[15]: f
        >>> a.decl()
        Out[16]: a
        >>> (a+1).decl()
        Out[17]: +
        
        >>> (a+1-1).decl()
        Out[18]: -

        >>> (a+1*1).decl()
        Out[19]: +

        >>> (a/1*1).decl()
        Out[20]: *
------------------------------------------
在这里，用来表示了struct的各个field不用不要做nosame操作。

比如 在fs.py中，有
pseudo_sort_decls = [
    (SInode.__z3_sort__.nlink, 'file-nlink'),
    (SData.__z3_sort__._len, 'file-length'),
    (SInode.__z3_sort__.atime, 'time'),
    (SInode.__z3_sort__.mtime, 'time'),
    (SInode.__z3_sort__.ctime, 'time'),
    (SVMA.__z3_sort__.off, 'file-length'),
]

这里的__z3_sort__是干啥的？
在Symbolic class的定义中有描述
    A subclass of Symbolic must have a __z3_sort__ class field giving
    the z3.SortRef for the value's type.  Subclasses must also
    implement the _z3_value and _wrap_lvalue methods."""
在fs.py中，有如下内容：
class SFn(simsym.SExpr, simsym.SymbolicConst):
    __z3_sort__ = z3.DeclareSort('Fn')   //z3.DeclareSort：：Create a new uninterpred sort named `name`.

## Ignore some pseudo sort names altogether when enumerating models.

pseudo_sort_ignore = {
    'file-nlink': True,     ## unused for test generation
    'file-length': True,    ## too many cases in link*link
    'time': True,           ## irrelevant for test generation for now
    'fd-num': False,
}

而对于struct而言，看看下面的函数
def tstruct(**fields):
    """Return a subclass of SStructBase for a struct type with the
    given fields.  'fields' must be a dictionary mapping from names to
    symbolic types."""

    name = "SStruct_" + "_".join(fields.keys())
    z3name = anon_name(name)
    sort = z3.Datatype(z3name)
    fieldList = fields.items()
    sort.declare(z3name, *[(fname, typ._z3_sort()) for fname, typ in fieldList])
    sort = sort.create()

    type_fields = {"__slots__": [], "_fields": fields, "_fieldList": fieldList,
                   "_z3name": z3name, "__z3_sort__": sort,
                   "_ctor": getattr(sort, z3name)}
    return type(name, (SStructBase,), type_fields)

有 "__z3_sort__": sort在SStructBase中，而sort是一个z3的DataType，
在 sort.declare(z3name, *[(fname, typ._z3_sort()) for fname, typ in fieldList])语句执行后
此结构下的所有的fields(包括field name和 filed type，已经申明在sort中了)

这样SInode.__z3_sort__.ctime，其实就代表了SInode struct中的一个field 即ctime 



  
IsomorphicMatch中的函数有对 pseudo_sort_decls和pseudo_sort_ignore 的判断  
  
    def add_assignment(self, expr, val):  
        ...
        for d, sortname in pseudo_sort_decls:
            if not expr.decl().eq(d): continue
            if pseudo_sort_ignore[sortname]: return
            self.add_assignment_uninterp(expr, val, sortname)
            return
        .....
        
        
 
