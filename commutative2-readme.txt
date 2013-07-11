Austin 进行了一系列的修改

调用参数
chy@chyhome-PC:~/mit/sck/spec6.1$ python spec.py
usage: spec.py [-h] [-c] [-p] [-m MODEL_FILE] [-t TEST_FILE] [-n NCOMB]
               [-f FUNCTIONS] [--simplify-more]
               [--max-testcases MAX_TESTCASES] [--verbose-testgen]
               MODULE

chy@chyhome-PC:~/mit/sck/spec6.1$ python spec.py -h
usage: spec.py [-h] [-c] [-p] [-m MODEL_FILE] [-t TEST_FILE] [-n NCOMB]
               [-f FUNCTIONS] [--simplify-more]
               [--max-testcases MAX_TESTCASES] [--verbose-testgen]
               MODULE

positional arguments:
  MODULE                Module to test (e.g., fs)

optional arguments:
  -h, --help            show this help message and exit
  -c, --check-conds     Check commutativity conditions for sat/unsat
  -p, --print-conds     Print commutativity conditions
  -m MODEL_FILE, --model-file MODEL_FILE
                        Z3 model output file
  -t TEST_FILE, --test-file TEST_FILE
                        Test generator output file
  -n NCOMB, --ncomb NCOMB
                        Number of system calls to combine per test
  -f FUNCTIONS, --functions FUNCTIONS
                        Methods to run (e.g., stat,fstat)
  --simplify-more       Use ctx-solver-simplify
  --max-testcases MAX_TESTCASES
                        Maximum # test cases to generate per combination
  --verbose-testgen     Print diagnostics during model enumeration


如果执行没有参数-t test.out，则全部可以运行完毕
time python spec.py fs

real	9m18.593s
user	9m16.128s
sys	0m1.772s
大约9分钟

如果执行有参数 -t test.out，则需要disable open and pipe, then 可以运行完毕
time python spec.py fs -t test.out

...
mprotect mem_write
  30 paths (20 commutative), 251 testcases
  can commute: maybe
  cannot commute, something can diverge: maybe
mem_read mem_read
  32 paths (32 commutative), 1250 testcases
  can commute: maybe
mem_read mem_write
  45 paths (40 commutative), 2539 testcases
  can commute: maybe
  cannot commute, something can diverge: maybe
mem_write mem_write
  57 paths (52 commutative), 8584 testcases
  can commute: maybe
  cannot commute, something can diverge: maybe

real	24m15.878s
user	24m11.968s
sys	0m2.236s

大约24分钟
===================
重要的变化
===================
6b9e9eb270de8a87fe9bf39c5a2032390e190a47
Author: Austin Clements <amdragon@mit.edu>  2013-06-05 12:40:08
Committer: Austin Clements <amdragon@mit.edu>  2013-06-11 14:40:14
Parent: c3aa48c7209fbc74b103dfed50c4bad7b10a46e3 (Pass concrete values through MetaZ3Wrapper._wrap)
Child:  d71000c15d655b07d41072edbbfb44118ecd49fe (Introduce synonym types)
Branches: master, remotes/bit/master, remotes/origin/master
Follows: 
Precedes: 

    Switch from ADTs to internal compounds for structs
    
    Previously, we represented structs as Z3 ADTs.  While this meant that
    every symbolic value was directly represented as a Z3 symbolic
    expression, it had several drawbacks: 1) during concrete test case
    enumeration, Z3 could not return a partial ADT value, so we would
    enumerate concrete values of all struct fields even if some fields
    were untouched by the test case and 2) Z3 seems to handle ADTs more
    poorly than you might expect, since expressions that are trivially
    equivalent to solvable non-ADT expressions seem to be unsolvable by Z3
    at present.
    
    This change eliminates Z3 ADTs in favor of our own representation of
    compound values.  Now, everywhere that used to allow a Z3 sort or Z3
    value accepts a "compound Z3 sort" or "compound Z3 value".  In the
    base case, a compound Z3 sort/value is simply a Z3 sort/value, but it
    can also be a dictionary mapping component names to compound Z3
    sorts/values.  Structs are thus represented directly as a level in
    this dictionary tree mapping field names to field types/values.
    
    Maps, the only other basic mutable compound type we support, become
    more interesting, since we can no longer simply use a Z3 map type if
    the value type of the map is a compound Z3 type.  However, map indexes
    and struct field references commute, so a map type is structurally
    identical to its value type, but with the leaves of the value type
    transformed into Z3 map types.  Likewise, when you index into a map,
    we simply broadcast that index operation across the leaves of the
    compound map value.
    
    Since this completely changes the way structure variables are
    constructed, it of course breaks test case generation.    


比如执行到一定阶段，这个dict为：
{'root_dir': {'_valid': Fs.root_dir._valid, '_map': Fs.root_dir._map}, 'pipes': {'nread': Fs.pipes.nread, 'data': {'_len': Fs.pipes.data._len, '_vals': Fs.pipes.data._vals}}, 'i_map': {'nlink': Fs.i_map.nlink, 'atime': Fs.i_map.atime, 'data': {'_len': Fs.i_map.data._len, '_vals': Fs.i_map.data._vals}, 'ctime': Fs.i_map.ctime, 'mtime': Fs.i_map.mtime}, 'proc1': {'va_map': {'_valid': Fs.proc1.va_map._valid, '_map': {'writable': Fs.proc1.va_map._map.writable, 'anon': Fs.proc1.va_map._map.anon, 'off': Fs.proc1.va_map._map.off, 'anondata': Fs.proc1.va_map._map.anondata, 'inum': Fs.proc1.va_map._map.inum}}, 'fd_map': {'_valid': Fs.proc1.fd_map._valid, '_map': {'ispipe': Fs.proc1.fd_map._map.ispipe, 'pipewriter': Fs.proc1.fd_map._map.pipewriter, 'off': Fs.proc1.fd_map._map.off, 'pipeid': Fs.proc1.fd_map._map.pipeid, 'inum': Fs.proc1.fd_map._map.inum}}}, 'proc0': {'va_map': {'_valid': Store(Fs.proc0.va_map._valid, a.mmap.va, True), '_map': {'writable': Store(Store(Fs.proc0.va_map._map.writable,
            a.mmap.va,
            Fs.proc0.va_map._map.writable[a.mmap.va]),
      a.mmap.va,
      a.mmap.writable), 'anon': Store(Store(Fs.proc0.va_map._map.anon,
            a.mmap.va,
            a.mmap.anon),
      a.mmap.va,
      Store(Fs.proc0.va_map._map.anon,
            a.mmap.va,
            a.mmap.anon)[a.mmap.va]), 'off': Store(Store(Fs.proc0.va_map._map.off,
            a.mmap.va,
            Fs.proc0.va_map._map.off[a.mmap.va]),
      a.mmap.va,
      Store(Fs.proc0.va_map._map.off,
            a.mmap.va,
            Fs.proc0.va_map._map.off[a.mmap.va])[a.mmap.va]), 'anondata': Store(Store(Fs.proc0.va_map._map.anondata,
            a.mmap.va,
            Fs.proc0.va_map._map.anondata[a.mmap.va]),
      a.mmap.va,
      Store(Fs.proc0.va_map._map.anondata,
            a.mmap.va,
            Fs.proc0.va_map._map.anondata[a.mmap.va])[a.mmap.va]), 'inum': Store(Store(Fs.proc0.va_map._map.inum,
            a.mmap.va,
            Fs.proc0.va_map._map.inum[a.mmap.va]),
      a.mmap.va,
      Store(Fs.proc0.va_map._map.inum,
            a.mmap.va,
            Fs.proc0.va_map._map.inum[a.mmap.va])[a.mmap.va])}}, 'fd_map': {'_valid': Fs.proc0.fd_map._valid, '_map': {'ispipe': Fs.proc0.fd_map._map.ispipe, 'pipewriter': Fs.proc0.fd_map._map.pipewriter, 'off': Fs.proc0.fd_map._map.off, 'pipeid': Fs.proc0.fd_map._map.pipeid, 'inum': Fs.proc0.fd_map._map.inum}}}}
                
============================================================
对symbolic实现的分析    
------------------
环境
environment:
 i_map: the inode array(map)    SIMap = symtypes.tmap(SInum, SInode)
 pipes: the pipe                SPipeMap = symtypes.tmap(SPipeId, SPipe)
 root_dir : root directory      SDirMap = symtypes.tdict(SFn, SInum)
 2 procs, proc0/proc1           SProc = symtypes.tstruct(fd_map = SFdMap, va_map = SVaMap)  //SFdMap=tdict(simsym.SInt, SFd)  SVaMap=tdict(SVa, SVMA)  
 
  each proc has a fd_map(SFdMap) and va_map(SVaMap) 
     |
     ---->fd_map( type is SFdMap) is a tdict of fd,  fd_dict[SFdNum]= SFd , and SFd=tstruct {inum=SInum, off, ispipe, pipeid}
     |     inode num(type is SInum expr sort in z3, not a Interger!),  off (file R/W offset), ... 
     |       |
     |       ----the inode num --> a index of inode map (type is SIMap), inode_map[SInum]=SInode
     |                              |   
     |                              ----> inode is a structure (SInode)
     |                                     |
     |                                     ---> data (SData， tlist(SDataByte)， abstract file data block), 
     |                                          nlink (link num), atime/mtime/ctime
     |
     --->va_map is a tdict of memory blocks va_dict[SVa]=SVMA type(va)=SVa, type(vma)=SVMA
           
           ---> type(va)=SVa, which is a abstract expr sort in z3, not a Interger! 
           |             class SVa(simsym.SExpr, simsym.SymbolicConst):
           |             __z3_sort__ = z3.DeclareSort('VA')
           |
           ---> type(vma)=SVMA,  is a tstruct:  anon(SBool), anondata(SDataByte) a data block, SData 是否更好？
                                 writable(SBool, inum(SInum, for mmap a file), off(SInt, off of a file),  
如果表示指针关系？
SDirMap{tdict}[SFn]=SInum
SIMap{tmap}[SInum]=SInode
proc0.fd_map{tdict}[SFdNum]{tstruct}.inum

这里面包含一些 == 关系
但这样的赋值会产生什么后果呢？
vma.inum = myproc.fd_map[fd].inum

至少

x=Int('a')

y=Int('b')

x
Out[43]: a

y
Out[44]: b

x=y

x
Out[46]: b

可以看到 当执行了x=y后，'a'已经消失了。这样是否不符合 vma.inum = myproc.fd_map[fd].inum的理解???


??? why lenType=SInt?? for isomophism???
SOffset = simsym.tsynonym("SOffset", simsym.SInt)
SData = symtypes.tlist(SDataByte, lenType=SOffset)      
问了，确实是用在isomophism，用于不用生成太多的无关测试用例。
比如在fs.py中有
isomorphism_types = {
    SNLink: "ignore",  # Unused for test generation
    SOffset: "ignore", # Too many cases in link*link (XXX maybe fixed?)
    STime: "ignore",   # Irrelevant for test generation for now
    SFdNum: "equal",
}
注意，这里的equal, 表示the type should be constrained only on equality (just like an uninterpreted sort).
这里指仅关注这类的相等和不等的属性，其他属性不用关注，这样也是为了避免生成太多的无关测试用力。
比如 第一次 a_fdnum==b_fdnum, 可生成测试用例  a_fdnum=1, b_fdnum=1, 
   那么下一个isomophism测试是 a_fdnum<>b_fdnum， 可生成测试用例  a_fdnum=1, b_fdnum=0,

这样就只生成了两个，而不是无穷多个。   
    
创建对象的地方：
                            
-------------------
 _wrap_lvalue(cls, getter, setter, model): 
 Symbolic：为空
 SymbolicConst: return cls._wrap(getter(), model) 直接返回值，而不是创建一个对象
 SmapBase: 有 obj = cls.__new__(cls)语句
 SstructBase: 有 obj = cls.__new__(cls)语句 
 
 而调用_wrap_lvalue的地方有三处：
 Symbolic的成员函数_new_lvalue，调用了obj = cls._wrap_lvalue(lambda: val[0], setter, model)
 这个其实是class.var调用的。
 
 SMapBase的成员函数__getitem__(self, idx): return self._valueType._wrap_lvalue(
                                            lambda: compound_map(
                                                lambda z3val: z3.Select(z3val, z3idx), self._getter()),
                                            lambda val: self.__setitem__(idx, val),
                                            self._model)
SstructBase的成员函数 __getattr__(self, name):
                                if name not in self._fields:
                                    raise AttributeError(name)
                                return self._fields[name]._wrap_lvalue(
                                    lambda: self._getter()[name],
                                    lambda val: self.__setattr__(name, val),
                                    self._model)
 
所有的类型都继承自class Symbolic(object)，其描述如下：
    """Base class of symbolic types.  Symbolic types come in two
    groups: constant and mutable.  Symbolic constants are deeply
    immutable.  Generally they are primitive types, such as integers
    and booleans, but more complex types can also be constants (e.g.,
    an immutable symbolic tuple of constants).  Mutable symbolic
    values are used for compound and container types like maps and
    structs.  Mutable values act like lvalues except when assigned,
    where they are copied at the point of assignment.

    A subclass of Symbolic must have a __z3_sort__ class field giving
    the compound z3.SortRef for the value's type.  Subclasses must
    also implement the _z3_value and _wrap_lvalue methods.
    """
根据其描述，可以看出对于struct和maps，是支持mutable的。why?  
注意：Mtable values act like lvalues except when assigned,
    where they are copied at the point of assignment.

而primitive types, such as integers，and booleans, but more complex types can also be constants
(e.g., an immutable symbolic tuple of constants).     



实现流程 (以counter为例)
=============================

几个重要的全局变量
# Map from user-provided Symbolic variable names to Symbolic instance
# constructors.  Each instance constructor must take two arguments:
# the user variable name and a simsym.Model object that binds the Z3
# environment.
var_constructors = {}
比如
{'Counter': <function <lambda> at 0x24c2668>, '__dummy': <bound method MetaZ3Wrapper.var of <class 'simsym.SInt'>>, 'Counter.counter': <bound method MetaZ3Wrapper.var of <class 'simsym.SInt'>>}

# Map from Z3 constant names to (outer Symbolic type, compound path)
constTypes = {}
比如 {'Counter.counter': (<class 'simsym.SInt'>, ())}

====
符号变量的基类
==================
class Symbolic(object):
    """Base class of symbolic types.  Symbolic types come in two
    groups: constant and mutable.  Symbolic constants are deeply
    immutable.  Generally they are primitive types, such as integers
    and booleans, but more complex types can also be constants (e.g.,
    an immutable symbolic tuple of constants).  Mutable symbolic
    values are used for compound and container types like maps and
    structs.  Mutable values act like lvalues except when assigned,
    where they are copied at the point of assignment.

    A subclass of Symbolic must have a __z3_sort__ class field giving
    the compound z3.SortRef for the value's type.  Subclasses must
    also implement the _z3_value and _wrap_lvalue methods.
    """
    注意 var()==>_new_lvalue()==>_wrap_lvalue()
    
    _z3_sort__返回此类型对应的z3的类型
    _z3_value返回对应的z3符号常量  Return the compound Z3 value wrapped by this object.
    _wrap_lvalue：Return a new instance of this class.
    _assumptions（）："Return the assumptions that should apply to a fresh created lvalue of 'obj'
    def __init__(self):
        raise RuntimeError("%s cannot be constructed directly" % strtype(self))

    @classmethod
    def _z3_sort(cls):
        """Return the compound Z3 sort represented by this class."""
        return cls.__z3_sort__

    def _z3_value(self):
        """Return the compound Z3 value wrapped by this object.

        For mutable objects, this should be its current value.
        """
        raise NotImplementedError("_z3_value is abstract")

    @classmethod
    def _new_lvalue(cls, init, model):
        """Return a new instance of Symbolic with the given initial value,
        which must be a compound Z3 value.  The returned instance's
        state will not be shared with any existing instances.  model,
        if not None, provides the simsym.Model in which to evaluate
        concrete values.
        """
        val = [init]
        def setter(nval):
            val[0] = nval
        obj = cls._wrap_lvalue(lambda: val[0], setter, model)
        if model is None:
            assume(cls._assumptions(obj))
        return obj

    @classmethod
    def _wrap_lvalue(cls, getter, setter, model):
        """Return a new instance of this class.

        Return an instance of this class wrapping the compound Z3
        value returned by getter.  If this object is immutable, then
        it should call getter immediately to fetch the object's
        current value.  If this object is mutable, it should use
        getter on demand and it should reflect changes to its value by
        calling setter with its updated compound Z3 value.

        model, if not None, provides the simsym.Model in which to
        evaluate concrete values.  Immutable, non-compound objects
        should use this to evaluate their concrete Python value and
        return this concrete value.  Mutable, compound objects should
        pass this model down to their components.
        """
        raise NotImplementedError("_wrap_lvalue is abstract")

    @classmethod
    def var(cls, name=None, model=None):
        """Return a symbolic variable of this type.

        Initially, the variable's value will be unconstrained.  It may
        become constrained or even concrete as the program progresses.

        If this method is called multiple times with the same name,
        the returned instances will represent the same underlying
        symbolic value (though the instances themselves will be
        distinct).

        If model is provided, it must be a simsym.Model.  This
        symbolic variable will be interpreted in this model to get a
        concrete Python value.
        """

        if name is None:
            name = anon_name()
        elif model is None:
            var_constructors[name] = cls.var
        def mkValue(path, sort):   ???
            if isinstance(sort, dict):
                return {k: mkValue(path + (k,), v)
                        for k, v in sort.iteritems()}
            strname = ".".join((name,) + path)
            # Record the simsym type of this constant
            constTypes[strname] = (cls, path)
            # Create the Z3 constant
            return z3.Const(strname, sort)
        return cls._new_lvalue(mkValue((), cls._z3_sort()), model)

    @classmethod
    def _assumptions(cls, obj):
        """Return the assumptions that should apply to a fresh created
        lvalue of 'obj'."""
        # XXX Do we still need this?
        # XXX This seems horribly roundabout.  Can this just be an
        # instance method that just assumes stuff it cares about and
        # does nothing if it doesn't need to do anything, rather than
        # building up some huge symbolic and that's mostly Trues?
        return obj.init_assumptions()

    def init_assumptions(self):
        return wrap(z3.BoolVal(True))

    def __ne__(self, o):
        r = self == o
        if r is NotImplemented:
            return NotImplemented
        return symnot(r)


=====================
在一开始执行时，所有metaclass为MetaZ3Wrapper的类，比如SExpr等会执行MetaZ3Wrapper的__new__函数
，以SExpr为例，动态生成code字符串为
'def __eq__(self,o0):
 if isinstance(o0, Symbolic): o0=o0._v
 return wrap(self._v.__eq__(o0))'
并执行此code
 exec code in globals(), locals_dict
这里 globals(), locals_dict是全局和局部名称空间
这样，SExpr中的__wrap__列表中有几个值，就会动态生成几个SExpr类的新成员函数，
并保存在locals_dict中，及locals_dict['__eq__']= <function __eq__ at 0x1d00a28>
并在SExpr中的__dict__中增加新成员函数：
   classdict[method] = locals_dict[method]
   这里method='__eq__', classdict[method]={'__eq__': <function __eq__ at 0x1d00a28>}
接着是添加'__ne__'和对应的新成员函数
'def __ne__(self,o0):
 if isinstance(o0, Symbolic): o0=o0._v
 return wrap(self._v.__ne__(o0))'
 
最后调用
  return type.__new__(cls, classname, bases, classdict)
返回类对象
  cls=<class 'simsym.MetaZ3Wrapper'>
  classname=str'SExpr'
  bases=(<class 'simsym.Symbolic'>,)
  classdict={'__ne__': <function __ne__ at 0x1d00aa0>, '__module__': 'simsym', '__metaclass__': <class 'simsym.MetaZ3Wrapper'>, '__str__': <function __str__ at 0x1cfff50>, '__ref_type__': <class z3.ExprRef at 0x1cde6d0>, '__repr__': <function __repr__ at 0x1d00050>, '__eq__': <function __eq__ at 0x1d00a28>, '_z3_value': <function _z3_value at 0x1d000c8>}


接着生成SArith
返回类对象
  cls=<class 'simsym.MetaZ3Wrapper'>
  classname=str'SArith'
  bases=(<class 'simsym.SExpr'>,)
  classdict=
{'__module__': 'simsym', '__rtruediv__': <function __rtruediv__ at 0x1d0e1b8>, '__radd__': <function __radd__ at 0x1d00e60>, '__truediv__': <function __truediv__ at 0x1d00de8>, '__rsub__': <function __rsub__ at 0x1d0e140>, '__rdiv__': <function __rdiv__ at 0x1d00ed8>, '__rmul__': <function __rmul__ at 0x1d0e050>, '__rmod__': <function __rmod__ at 0x1d00f50>, '__lt__': <function __lt__ at 0x1d0e398>, '__rpow__': <function __rpow__ at 0x1d0e0c8>, '__pos__': <function __pos__ at 0x1d0e488>, '__mul__': <function __mul__ at 0x1d00c80>, '__ref_type__': <class z3.ArithRef at 0x1cde940>, '__pow__': <function __pow__ at 0x1d00cf8>, '__add__': <function __add__ at 0x1d00b18>, '__gt__': <function __gt__ at 0x1d0e2a8>, '__mod__': <function __mod__ at 0x1d00c08>, '__div__': <function __div__ at 0x1d00b90>, '__le__': <function __le__ at 0x1d0e320>, '__neg__': <function __neg__ at 0x1d0e410>, '__sub__': <function __sub__ at 0x1d00d70>, '__ge__': <function __ge__ at 0x1d0e230>}

生成 class SInt(SArith, SymbolicConst):
  cls=<class 'simsym.MetaZ3Wrapper'>
  classname=str'SInt'
  bases=(<class 'simsym.SArith'>, <class 'simsym.SymbolicConst'>)
  classdict={'__module__': 'simsym', '__z3_sort__': Int, '__pass_type__': <type 'int'>}

生成class SBool(SExpr, SymbolicConst):
  cls=<class 'simsym.MetaZ3Wrapper'>
  classname=str'SBool'
  bases=(<class 'simsym.SExpr'>, <class 'simsym.SymbolicConst'>)
  classdict={'__module__': 'simsym', '__z3_sort__': Bool, '__nonzero__': <function __nonzero__ at 0x1d0e7d0>, '__ref_type__': <class z3.BoolRef at 0x1cde7a0>, '__pass_type__': <type 'bool'>}
  这里的__nonzero__很重要

其他 
SEnumBase(SExpr): class STupleBase(SExpr):
__ref_type__ = z3.DatatypeRef

class SConstMapBase(SExpr):
__ref_type__ = z3.ArrayRef




元类
#
# Z3 wrappers
#

# We construct a type hierarchy that parallels Z3's expression type
# hierarchy.  Each type wraps the equivalent Z3 type and defers to the
# Z3 methods for all symbolic operations (unwrapping the arguments and
# re-wrapping the results).  However, these types add methods specific
# to symbolic execution; most notably __nonzero__.  The leaves of this
# type hierarchy also provide Python types corresponding to Z3 sorts
# that we care about.

class MetaZ3Wrapper(type):
    """Metaclass to generate wrappers for Z3 ref object methods.

    The class must have a __ref_type__ field giving the Z3 ref type
    wrapped by the class.  The class may optionally have a
    __pass_type__ field, giving a Python type or tuple or types that
    should be passed through wrap untouched.

    The class must also have a __wrap__ field giving a list of method
    names to wrap.  For each method in __wrap__, the generated class
    will have a corresponding method with the same signature that
    unwraps all arguments, calls the Z3 method, and re-wraps the
    result.
    """

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

    def _wrap(cls, z3ref, model):
        """Construct an instance of 'cls' wrapping the given Z3 ref
        object."""

        if not isinstance(z3ref, cls.__ref_type__):
            if hasattr(cls, "__pass_type__") and \
               isinstance(z3ref, cls.__pass_type__):
                return z3ref
            raise TypeError("%s expected %s, got %s" %
                            (cls.__name__, cls.__ref_type__.__name__,
                             strtype(z3ref)))
        if model:
            # Interpret this symbolic value into a concrete value
            # using the model
            return model._eval(z3ref)
        obj = cls.__new__(cls)
        obj._v = z3ref
        return obj


子类：
==============
__pass_type__ field的作用是???
问了：
这里是，有可能这个field用的是python的integer, 这样没有关系，在进行isomorphicmatch时，
碰到这样的类型，返回z3的integer
在 class MetaZ3Wrapper(type)的函数_wrap中有对 __pass_type__的判断

    def _wrap(cls, z3ref, model):

        if not isinstance(z3ref, cls.__ref_type__): 说明这个 z3ref不是一个真的z3的ref, 可能是python的ref，
            if hasattr(cls, "__pass_type__") and \
               isinstance(z3ref, cls.__pass_type__):  //这里确定一下是否这个z3ref确实是cls中预先指定的python的ref，比如 int, bool等
                return z3ref                          //如果是，则直接返回即可。
        ...

Symbolic
------------------------------------------------------------------------------------------------------
   |                |                  |            |           |          |           |             |
SymbolicConst      SSynonymBase    SMapBase      SStructBase  SListBase   SDictBase  SSetBase      SBagBase
   

Symbolic+MetaZ3Wrapper
     |
    SExpr  //__ref_type__ = z3.ExprRef   __wrap__ = ["__eq__", "__ne__"]
     |
   ---------------------------------------------------------------
     |       |                              |                    +SymbolicConst
     |     SEnumBase/STupleBase      SConstMapBase              ----------------------------------------
     |     在ttuple用了@property     z3.ArrayRef                 |     |      |         |       |        |  
     | __ref_type__=z3.DatatypeRef  __wrap__ = ["__getitem__"] SFn   SInum   SDataByte  SVa  SPipeId  SBool
     |                             tconstmap:immutable map type
     |                             (a z3 "array")                                                       |   
     |                                                                                                  V
     |                                                                         // __ref_type__ = z3.BoolRef __pass_type__ = bool __z3_sort__ = z3.BoolSort()
     |
     |
     SArith  //__ref_type__ = z3.ArithRef
     +
     SymbolicConst
       |
       SInt //__z3_sort__ = z3.IntSort()  __pass_type__ = int

tmap(indexType, valueType):
    name = "SMap_%s_%s" % (indexType.__name__, valueType.__name__)
    indexSort = indexType._z3_sort()
    sort = compound_map(lambda z3sort: z3.ArraySort(indexSort, z3sort),
                        valueType._z3_sort())
    base=(SMapBase,)
注意 tmap用到了 z3.ArraySort
 sort = compound_map(lambda z3sort: z3.ArraySort(indexSort, z3sort),
                        valueType._z3_sort())
                        
SMapBase用到了 应该用到z3.Array
    lambda z3val: z3.Select(z3val, z3idx), self._getter()),
    和lambda z3map, z3val: z3.Store(z3map, z3idx, z3val),

在fs.py中，的mmap的实现中，有如下操作：
        vma = myproc.va_map.create(va)
表明了:
    Fs.proc0.va_map._map.writable[a.mmap.va]
    Fs.proc0.va_map._map.anon[a.mmap.va]
    Fs.proc0.va_map._map.anondata[a.mmap.va]  
    Fs.proc0.va_map._map.off[a.mmap.va]
    有这么一个符号变量了，没有绑定。
          
        vma.anon = anon
表明了：
       Fs.proc0.va_map._map.anon[a.mmap.va]=a.mmap.anon
       即 Z3.Store(Fs.proc0.va_map._map.anon, a.mmap.va,  a.mmap.anon)
       
       vma.writable = writable
      类似上面的操作，这样形成了一个新的符号array,如下所示：
      ArrayRef: Store(Store(Fs.proc0.va_map._map.writable,
            a.mmap.va,
            Fs.proc0.va_map._map.writable[a.mmap.va]),
      a.mmap.va,
      a.mmap.writable)

注意  vma其实是 SMapBase的一个element， value为 Fs.proc0.va_map._map[a.mmap.va]，
    Fs.proc0.va_map是SDictBase
    z3 value为 Fs.proc0.va_map._map是SMapBase
所以在用创建vma （用的是SDictBase中的create成员变量）时，确定了 vma其实是 SMapBase的一个element,
而SMapBase在调用 def __getitem__(self, idx)时，是这样设定的：
    def __getitem__(self, idx):
        """Return the value at index 'idx'."""
        z3idx = unwrap(idx)
        return self._valueType._wrap_lvalue(
            lambda: compound_map(
                lambda z3val: z3.Select(z3val, z3idx), self._getter()),
            lambda val: self.__setitem__(idx, val),
            self._model)
这说明了把 item的 _getter设置成为了
lambda: compound_map(
                lambda z3val: z3.Select(z3val, z3idx), self._getter()),

把item的_setter设置成为了
lambda val: self.__setitem__(idx, val)  即SMapBase的 
    def __setitem__(self, idx, val):
        """Change the value at index 'idx'."""
        z3idx = unwrap(idx)
        self._setter(compound_map(
            lambda z3map, z3val: z3.Store(z3map, z3idx, z3val),
            self._getter(), unwrap(val)))
            
这样，如果map(id, a_struct)
则每个a_struct.field其实是个数组，都要更新一下。即做一次Store.            
            
这样，如果某个变量属于struct or map, 则会一层一层第访问到最高层的_getter, or __getitem__ ???
具体代码可见 Sstruct中的
    def __getattr__(self, name):  //如果这个属性不存在，则会调用此函数，创建一个field对应类型的obj
        if name not in self._fields:
            raise AttributeError(name)
        return self._fields[name]._wrap_lvalue(
            lambda: self._getter()[name],
            lambda val: self.__setattr__(name, val),
            self._model)
注意： self._fields[name]可获得此field的class，然后就是调用_wrap_lvalue创建此对象了，且设置此对象的读操作和写操作。           
            

    def __setattr__(self, name, val):
        if name not in self._fields:
            raise AttributeError(name)
        cval = self._getter()
        cval[name] = unwrap(val)
        self._setter(cval)
        
注意cval的处理。这样的目的是为了确定别找错了？

例如：
这里的self是最高层的Fs类型的对象了
self._getter()['proc0']['va_map']['_map']['writable']=ArrayRef: Store(Store(Fs.proc0.va_map._map.writable,
            a.mmap.va,
            Fs.proc0.va_map._map.writable[a.mmap.va]),
      a.mmap.va,
      a.mmap.writable)
      
而其它层次都是记录了一个dict，比如
self._getter()['proc0']['va_map']['_map']={'writable': Store(Store(Fs.proc0.va_map._map.writable,
            a.mmap.va,
            Fs.proc0.va_map._map.writable[a.mmap.va]),
      a.mmap.va,
      a.mmap.writable), 'anon': Store(Store(Fs.proc0.va_map._map.anon,
            a.mmap.va,
            a.mmap.anon),
      a.mmap.va,
      Store(Fs.proc0.va_map._map.anon,
            a.mmap.va,
            a.mmap.anon)[a.mmap.va]), 'off': Store(Store(Fs.proc0.va_map._map.off,
            a.mmap.va,
            Fs.proc0.va_map._map.off[a.mmap.va]),
      a.mmap.va,
      Store(Fs.proc0.va_map._map.off,
            a.mmap.va,
            Fs.proc0.va_map._map.off[a.mmap.va])[a.mmap.va]), 'anondata': Store(Store(Fs.proc0.va_map._map.anondata,
            a.mmap.va,
            Fs.proc0.va_map._map.anondata[a.mmap.va]),
      a.mmap.va,
      Store(Fs.proc0.va_map._map.anondata,
            a.mmap.va,
            Fs.proc0.va_map._map.anondata[a.mmap.va])[a.mmap.va]), 'inum': Store(Store(Fs.proc0.va_map._map.inum,
            a.mmap.va,
            Fs.proc0.va_map._map.inum[a.mmap.va]),
      a.mmap.va,
      Store(Fs.proc0.va_map._map.inum,
            a.mmap.va,
            Fs.proc0.va_map._map.inum[a.mmap.va])[a.mmap.va])}      
            
            
            
--------------------------------------------------------------------------
    
tstruct(**fields):
    name = "SStruct_" + "_".join(fields.keys())
    sort = {fname: typ._z3_sort() for fname, typ in fields.items()}
    type_fields = {"__slots__": [], "_fields": fields, "__z3_sort__": sort}
    base=(SStructBase,)
    
tlist(valueType, lenType=SInt):  
    name = "SList_" + valueType.__name__
    base=(tstruct(_vals = tmap(lenType, valueType), _len = lenType), SListBase)

tdict(keyType, valueType):
     name = "SDict_" + keyType.__name__ + "_" + valueType.__name__
     base = (tstruct(_map = tmap(keyType, valueType), _valid = tmap(keyType, SBool)), SDictBase)
     
tset(valueType):
     name = "SSet_" + valueType.__name__
     mapType = tmap(valueType, SBool)
     base=(tstruct(_bmap = mapType), SSetBase)
     
tbag(valueType):
    name = "SBag_" + valueType.__name__     
    mapType = tmap(valueType, SInt)
    base = (tstruct(_imap = mapType),SBagBase)

tuninterpreted(name):
    """Return a new uninterpreted symbolic type.

    This type is inhabited by an unbounded number of distinct
    constants.
    """
    return type(name, (SUninterpretedBase, SymbolicConst),
                {"__z3_sort__": z3.DeclareSort(name)})

tenum(name, vals):
   """Return a symbolic constant enumeration type called 'name' with
    the given values.  'vals' must be a list of strings or a string of
    space-separated names.  The returned type will have a class field
    corresponding to each concrete value and inherit from SEnumBase
    and SymbolicConst."""
  sort, consts = z3.EnumSort(name, vals)
  base=type(name, (SEnumBase, SymbolicConst), fields) 
注意：SEnumBase和STupleBase用到了z3.Datatype，
class SEnumBase(SExpr):
    __ref_type__ = z3.DatatypeRef

且ttuple还直接调用了,我理解由于z3.Datatype用处不大或效率很低，所以这里基本没用
    sort = z3.Datatype(name)

                   
??? 用来包装某些不需要在isomophism model是进行具体化的操作。
tsynonym(name, baseType):
    type(name, (SSynonymBase,),
                {"_baseType" : baseType, "__z3_sort__" : baseType._z3_sort()})
???
class SSynonymBase(Symbolic):
    @classmethod
    def _wrap_lvalue(cls, getter, setter, model):
        return cls._baseType._wrap_lvalue(getter, setter, model)
                

下面的没有用     
tconstmap(indexType, valueType):
    sort = z3.ArraySort(indexType._z3_sort(), valueType._z3_sort())
    name = "SConstMap_%s_%s" % (indexType.__name__, valueType.__name__)
    return type(name, (SConstMapBase, SymbolicConst), {"__z3_sort__" : sort})
注意：tconstmap 直接用到了z3.ArraySort
而对应的SConstMapBase用到了
    __ref_type__ = z3.ArrayRef
    z3.K(cls._z3_sort().domain(), unwrap(value)), None)
    z3.Store(unwrap(self), unwrap(index), unwrap(value)),


ttuple(name, *types):
    """Return a symbolic constant named tuple type with the given
    fields.  Each 'type' argument must be a pair of name and type.
    The returned type will inherit from STupleBase and SymbolicConst
    and will have properties for retrieving each component of the
    tuple."""
    fields[fname] = locals_dict[fname]
    type(name, (STupleBase, SymbolicConst), fields)
==========
simsym.py L1155
# Helpers for tracking "internal" variables
internal_vars = {None: SInt.var('__dummy')}
//SInt.var其实是SymbolicConst.var,这个函数调用了
        return cls._wrap(z3.Const(name, cls._z3_sort()), model)
这里z3.Const(name, cls._z3_sort())是声明了一个符号常量__dummy, 类型是ArithSortRef: Int
    其结果是ArithRef: __dummy
注意，这里的cls是SInt，但SInt._wrap其实是MetaZ3Wrapper._wrap（ArithRef: __dummy， {}） 
        ...
        obj = cls.__new__(cls)
        obj._v = z3ref
        return obj
大致执行情况是创建了一个SInt对象实例obj={SInt}__dummy  obj._v=ArithRef: __dummy
这样这个对象实例就包含了一个对应的z3符号常量，在_v成员变量中。
我理解这就是所谓的wrap, 反之，取出_v(这是针对基本类，对于SStruct则不是这样)就算是unwrap

执行完上面一句后，就开始分析启动参数了。可以理解总体的初始化完成。

初始化Counter类
==============
spec.py: L475
m = __import__(args.module)  //args.module=counter，即代表import counter.py module  这里开始创建model相关的新类了这里是counter.Counter类（它的基类是SStruct_counter，也是在此过程中创建的）。
   ==> class Counter(simsym.tstruct(counter=simsym.SInt)):  //tstruct 将返回一个SStruct_counter的新类
      ==> tstruct(**fields):  //fields={'counter': <class 'simsym.SInt'>}
                name = "SStruct_" + "_".join(fields.keys())     //name='SStruct_counter'
                sort = {fname: typ._z3_sort() for fname, typ in fields.items()}  
                  //fname=counter, typ= <class 'simsym.SInt'>, 所以 typ._z3_sort() = class SInt的__z3_sort__,及 z3.IntSort()
                  //所以 sort={'counter': Int}                  
                type_fields = {"__slots__": [], "_fields": fields, "__z3_sort__": sort}
                //调用type,构造新的类 name='SStruct_counter', base class= SStructBase, 
                            dict= {'__slots__': [], '_fields': {'counter': <class 'simsym.SInt'>}, '__z3_sort__': {'counter': Int}}
                return type(name, (SStructBase,), type_fields)
        注意： __z3_sort__的表示， _fields'的表示！，特别是更加负责的结构情况下。          
                
   ==>调用Counter中每个有@model.methodwrap()的方法，其实我理解是把与这方法相关的参数都给符号化，这样会分别执行inc, dec, iszero三个函数的methodwrap  
        def methodwrap(**arg_types):
            """Transform a method into a testable model method.

            This returns a decorator for model class methods to make them
            testable.  The keys of arg_types must correspond to the wrapped
            method's arguments (except 'self'), and its values must give the
            symbolic types of those arguments.  The decorated method will take
            two strings: one identifying the call in the call set being
            testing (which must stay the same regardless of permutation), and
            one identifying the current permutation.  This method invokes the
            wrapped method with the appropriate symbolic values for its
            arguments.

            This supports two types of arguments: regular and "internal"
            arguments.  For regular arguments, each call in a call set gets a
            distinct symbolic variable, but that same variable is reused
            regardless of the order the call set is invoked in (hence, the
            value of the argument is fixed across permutations).  Internal
            arguments are allowed to differ between the same call in different
            permutations; they represent non-deterministic or "internal"
            choices the method can make.  The name of an internal argument
            must begin with "internal_".
            """
            //小bug, 如果arg_types为空，可以直接跳过下面大部分片段，直到def decorator(m):
            
            # Separate regular and "internal" arguments.
            regular, internal = {}, {}
            for name, typ in arg_types.items():
                if name.startswith("internal_"):
                    internal[name] = typ
                else:
                    regular[name] = typ

            # Create symbolic struct types for the regular arguments and for
            # the internal arguments
            regular_struct = simsym.tstruct(**regular)
            internal_struct = simsym.tstruct(**internal)

            def decorator(m):
                def wrapped(self, whichcall, whichseq):
                    # Create the regular arguments for this call.  Note that
                    # this will construct the same name for the n'th call in
                    # the call set, regardless of its current position in the
                    # permutation, so we'll get the same initial symbolic
                    # value.
                    name = "%s.%s" % (whichcall, m.__name__)
                    regular_args = regular_struct.var(name)
                    # Create the internal arguments for this call.  This is
                    # named after both the permutation and the call within the
                    # permutation.
                    internal_args = internal_struct.var(whichseq + "." + name)
                    simsym.add_internal(internal_args)

                    # Build Python arguments dictionary
                    args = {}
                    for arg in regular:
                        args[arg] = getattr(regular_args, arg)
                    for arg in internal:
                        args[arg] = getattr(internal_args, arg)

                    return m(self, **args)
                wrapped.__name__ = m.__name__
                return wrapped
            return decorator   
   

这样counter.Counter类的基类是SStruct_counter，而SStruct_counter的基类又是SStructBase，而SStructBase的基类是Symbolic
Counter-->SStruct_counter-->SStructBase-->Symbolic



=========================================
开始测试
=========================================
// calls=[<unbound method Counter.sys_inc>, <unbound method Counter.sys_dec>, <unbound method Counter.sys_iszero>]
for callset in itertools.combinations_with_replacement(calls, args.ncomb):  
// 第一次 callset=(<unbound method Counter.sys_inc>, <unbound method Counter.sys_inc>)
    print ' '.join([c.__name__ for c in callset])
    test_writer.begin_call_set(callset)   //开始生成测试用例

    condlists = collections.defaultdict(list) //一开始为空，
    for sar in simsym.symbolic_apply(test, base, *callset):  //开始调用test, base=<class 'counter.Counter'>
         ==>symbolic_apply(fn, *args): 需要分析   
              ==>for sar in __symbolic_apply_loop(fn, *args): //实际开始调用 __symbolic_apply_loop
                 // fn = <function test at 0x1a16500>
                 // args= (<class 'counter.Counter'>, <unbound method Counter.sys_inc>, <unbound method Counter.sys_inc>)
                 ==> while len(schedq) > 0:
                        ... 
                        rv = fn(*args)
                        ...
                     ==> test(base, *calls):   
                       // base = <class 'counter.Counter'>
                       // calls = (<unbound method Counter.sys_inc>, <unbound method Counter.sys_inc>)
                       这个函数的实现与以前大致一样
                       for callseq in itertools.permutations(range(0, len(calls))):
                            s = base.var(base.__name__)  //var是类方法，初始化状态 base是Counter ，其实是完成了一个新类SStruct_counter的实例化，包括对其中的field, 比如这里的{SInt}Counter.counter的实例化 !!!
                            这里的base.var其实就是给model(fs, coutner, upipe等)进行符号常量assign
                            注意，有三个class有var成员方法：
                               Symbolic.var() : Return a symbolic variable of this type.
                               SymboliConst.var() : 一个简化版的Symbolic.var 主要是SBool啥的符号变量 
                               SStructBase.var() ：Return a struct instance with specified field values.
                               
                               SStructBasevar中还会递归调用其field的type的var
                                ==> 调用的是 class SStructBase(Symbolic)的类方法@classmethod def var(cls, __name=None, __model=None, **fields):
                                    //var 将Return a struct instance with specified field values.
                                    //在初始化时，已经创建了一个新类SStruct_counter，而Counter-->SStruct_counter
                                    //class Counter(simsym.tstruct(counter=simsym.SInt)): 
                                    所以这里var应该是创建Counter的一个对象实例  Counter-->SStruct_counter-->SStructBase-->Symbolic
                                    cls.var==>cls._new_lvalue==>cls._wrap_lvalue，完成对象实例的创建，与z3对应对象的绑定等

                                    @classmethod
                                    def var(cls, __name=None, __model=None, **fields):
                                        """Return a struct instance with specified field values.

                                        Any fields omitted from 'fields' will be unspecified.  If any
                                        fields are omitted, the first positional argument must be
                                        supplied to name the symbolic constants for the omitted
                                        fields.  If no field values are provided, this is equivalent
                                        to Symbolic.var.
                                        """ 
                                        // //cls=<class 'counter.Counter'>, cls__name='Counter' __model=none,  fields={}
                                        if __name is not None and __model is None:  //条件满足，但干的事情不够清楚
                                            # Field values may be mutable Symbolic values, but we want
                                            # to save their current value, so snapshot them by
                                            # unwrapping their values.
                                            fieldsSnapshot = {k: unwrap(v) for k,v in fields.items()}
                                            var_constructors[__name] \
                                                = lambda _, model: cls.var(__name, model, **fieldsSnapshot)
                                            //var_constructors= {'Counter': <function <lambda> at 0x185b500>, '__dummy': <bound method MetaZ3Wrapper.var of <class 'simsym.SInt'>>}            
                                        fvals = {}
                                        for fname, typ in cls._fields.iteritems(): //fields有一个counter
                                            if fname in fields:
                                                fvals[fname] = unwrap(fields.pop(fname))
                                            else:
                                                if __name is None:
                                                    raise ValueError(
                                                        "Name required for partially symbolic struct")
                                                fvals[fname] = unwrap(typ.var(__name + "." + fname, __model))
                                             //此时 fvals={'counter': Counter.counter} 
                                        if fields:
                                            raise AttributeError("Unknown struct field %r" % fields.keys()[0])
                                        return cls._new_lvalue(fvals, __model)
                                            ==>symbolic._new_lvalue
                                            //实际调用的是symbolic._new_lvalue(cls, init, model):
                                                val = [init]  //init=fvals={'counter': Counter.counter} 
                                                def setter(nval):
                                                    val[0] = nval
                                                obj = cls._wrap_lvalue(lambda: val[0], setter, model)
                                                    ==>实际调用的是SStructBase._wrap_lvalue(cls, getter, setter, model)
                                                        obj = cls.__new__(cls)  //cls=<class 'counter.Counter'>
                                                                                //counter.Counter是SStructBase的子类
                                                        # Don't go through the overridden __setattr__.
                                                        object.__setattr__(obj, "_getter", getter)
                                                        object.__setattr__(obj, "_setter", setter)
                                                        object.__setattr__(obj, "_model", model)
                                                        //这里 getter, setter的定义都在symbolic._new_lvaule中声明了。
                                                        get 和 set都是取和写val[0]
                                                        即元素为{名字：类型}的字典，列表val=[{'counter': Counter.counter}]
                                                        return obj
                                                        生成了一个新对象（其类为Counter），并设置好相关内容后，返回
                                                
                                                下面的代码是symbolic._new_lvalue的倒数两句部分        
                                                if model is None:  //一般为空
                                                    assume(cls._assumptions(obj))
                                                    assume函数把 cls._assumptions(obj)返回的 {SBool} And(True, Counter.counter >= 0)
                                                                存入到全局列表assume_list中
                                                    调用Counter类的_assumptions(obj)函数
                                                      ==>    def _assumptions(cls, obj):
                                                                    return simsym.symand([super(Counter, cls)._assumptions(obj),
                                                                                         obj.counter >= 0])
                                                          super(Counter, cls)._assumptions(obj)实际调用的是symbolic._assumptions(obj)
                                                                  ==> symbolic._assumptions(cls, obj):  //obj=<counter.Counter object >
                                                                            return obj.init_assumptions()
                                                                          其实调用了symbolic.init_assumptions()
                                                                              ==>symbolic.init_assumptions(self):
                                                                                    return wrap(z3.BoolVal(True)) 
                                                                                 //简单返回一个包含z3.True符号值的SBool变量
                                                       cls._assumptions(obj)= z3的And(True, Counter.counter >=0)  
                                                       注意：访问obj.counter会触发SStructBase.__getattr__(self,'counter')
                                                         其实现为：
                                                                return self._fields[name]._wrap_lvalue(
                                                                lambda: self._getter()[name],
                                                                lambda val: self.__setattr__(name, val),
                                                                self._model)
                                                                //self=<counter.Counter object at 0x24c55a8>
                                                                //name='coutner'
                                                                //self._fields[name]=<class 'simsym.SInt'>
                                                                //simsym.SInt._wrap_lvalue(...)会创建一个SInt变量（包含了对应的z3.Int符号常量）
                                                                    其实调用了SymbolicConst._wrap_lvalue
                                                下面的代码是symbolic._new_lvalue的最后部分                                  
                                                return obj       
                                            
                                    
                            r = {}
                            seqname = ''.join(map(lambda i: chr(i + ord('a')), callseq))
                            for idx in callseq:
                                //开始执行a.fun, b.fun, or b.fun, a.fun 设a,b为两个进程
                                r[idx] = calls[idx](s, chr(idx + ord('a')), seqname)
                                 ==> 第一次是a.sys_inc()
                                    首先执行@model.methodwrap()，由于没有参数，所以执行了一堆没太大用的东西，否则应该创建符号常量
                                    然后具体执行 self.counter = self.counter + 1
                                    这里 self=<counter.Counter object at 0x24c55a8>
                                         self._fields={'counter': <class 'simsym.SInt'>}
                                         self.counter={SInt}Counter.counter
                                         self.counter._v= {instance}ArithRef: Counter.counter
                                         self.counter._v.ast=<Ast object at 0x2329ef0>
                                     
                                     在做加法过程中，在此过程中，体现了mutable变量的处理过程
                                        其实调用了z3.__add__(self, other):
                                          最终返回了一个新的 ArithRef: Counter.counter + 1    
                                        还调用了z3.AstRef(Z3PPObject): __del__(self): 这里的self是IntNumRef: 1 其实是减少引用计数
                                        再调用 wrap(ArithRef: Counter.counter + 1),对ArithRef: Counter.counter + 1 进行warp，形成
                                            一个新的SInt, wrap会调用SInt._wrap，其实是MetaZ3Wrapper._wrap, 这样生成了一个新的SInt obj,
                                            这个obj包含了新的(ArithRef: Counter.counter + 1)
                                        再调用了SStructBase.__setattr__(self, name, val):
                                          //self=<counter.Counter object at 0x24c55a8>
                                            name='counter'
                                            val={SInt}Counter.counter + 1  
                                            
                                                SStructBase.__setattr__的实现如下
                                                if name not in self._fields:
                                                    raise AttributeError(name)
                                                cval = self._getter()
                                                //这里的self._getter其实执行的是 Symbolic._new_lvalue中定义的一个lambda函数 (simsym.py L70)
                                                  返回val[0]，所以 cval={dict}{'counter': Counter.counter}
                                                  
                                                cval[name] = unwrap(val)
                                                // 这里的unwrap(val) = ArithRef: Counter.counter + 1
                                                所以 cval['counter']= ArithRef: Counter.counter + 1
                                                
                                                self._setter(cval)  
                                                //这里的_setter是在SStruct._new_lvalue中设置的symbolic._new_lvalue定义的setter函数
                                                  其实就是 val[0] = nval  //(simsym.py L69)
                                                  这样
                                                     self=<counter.Counter object at 0x24c55a8>  //没变
                                                     self._fields={'counter': <class 'simsym.SInt'>} //没变
                                                     self.counter={SInt}Counter.counter + 1            //变了
                                                     self.counter._v= {instance}ArithRef: Counter.counter + 1 //变了
                                                     self.counter._v.ast=<Ast object at 0x24cbdd0> //变了
                                            
                            all_s.append(s)
                            all_r.append(r)
                       ....  
                       return diverge //返回是否能够commutative的情况
                         
        condlists[sar.value].append(sar.path_condition)
        test_writer.on_result(sar)

    test_writer.end_call_set()

    conds = collections.defaultdict(lambda: [simsym.wrap(z3.BoolVal(False))])
    for result, condlist in condlists.items():
        conds[result] = condlist

    # Internal variables help deal with situations where, for the same
    # assignment of initial state + external inputs, two operations both
    # can commute and can diverge (depending on internal choice, like the
    # inode number for file creation).
    commute = simsym.symor(conds[()])
    cannot_commute = simsym.symnot(simsym.exists(simsym.internals(), commute))

    for diverge, condlist in sorted(conds.items()):
        if diverge == ():
            print_cond('can commute', simsym.symor(condlist))
        else:
            print_cond('cannot commute, %s can diverge' % ', '.join(diverge),
                       simsym.symand([simsym.symor(condlist), cannot_commute]))

test_writer.finish()
