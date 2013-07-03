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


13/6/5
===========
1. Simple test suite for symbolic types
    写了一个简单的测试程序symtest.py 分析各种符号结构的实现情况 

2. Pass concrete values through MetaZ3Wrapper._wrap  simsym.py
    Sometimes we want to use a specific wrapper type to wrap a value that
    might actually be a concrete Python value of the corresponding Python
    type.  Previously we only supported this for type-generic wrap and
    would fail if we tried a type-specific _wrap.  Augment the classes
    that have corresponding Python types so that type-specific _wrap will
    pass through appropriate concrete values accordingly.
    
    It might not be possible to trigger this right now, but it will be as
    soon as we start managing our own compound values for structs and
    maps, since a Python value may pass through these completely untouched
    by Z3.




3. Switch from ADTs to internal compounds for structs  simsym.py   
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
    
13/6/7
===================
1.  Introduce synonym types  simsym.py
   
    These aren't useful yet, but the next patch will introduce a way to
    map a Z3 constant back to its Symbolic type.  Synonym types will let
    us declare new types for things that need to behave like basic types.
    We can then associate information with these types.
    
2. Function to map from a Z3 constant to a Symbolic type simsym.py

13/6/11
====================
1. spec: Replace pseudo_sorts with type-based isomorphisms  spec.py
    
    The old psuedo-sorts mechanism was broken because it depended on
    datatype projections to detect special sort handling, but our new
    struct implementation performs projections purely in Python.
    
    This introduces a new, cleaner mechanism that allows the specification
    to override the way isomorphism is handled for specific
    simsym.Symbolic types.  Combined with synonym types, this lets us
    specify to either ignore instances of specific synonym types or treat
    them like uninterpreted sorts.

2. symtypes: Allow overriding of list length type  symtypes.py
    
    With synonym types, it makes sense to allow the caller to specify an
    SInt-like type for the length of lists and indexes into them.
    
3. fs: Use synonym types and new type-based isomorphism fs.py
    
    This wraps all primitive types used by the FS spec in synonym types
    and replaces the old pseudo-sort specifications with type-based
    isomorphism handling.  This also means we no longer need to specially
    register FD and offset variables; their types tell us everything we
    need to know.    
    
13/6/21
================
1. Disable Fs.open and Fs.pipe tests
    
    open/pipe crashes Z3 and pipe/pread produces "array-ext" applications
    in the model that model_unwrap doesn't know what to do with.

2. All Fs.pipe tests work with latest Z3 unstable
    
    It turns out we weren't supposed to be seeing "array-ext" in the
    model.  This was a bug in Z3:
      http://stackoverflow.com/questions/17215640/getting-concrete-values-from-a-model-containing-array-ext
    It has been fixed as of Z3 commit 185f125 on June 20, 2013.

3. All Fs.open combinations work now, too
    
    Maybe it was a related problem?
    
    Also, we no longer ever restart the solver.
    
    Now the only remaining problem is that the test case generator still
    encodes the old struct conventions.

4. Command-line option to produce a file of Z3 models

5. Print test case count as we generate
    
    Also shows the number of code paths handled and the total number, to
    give a sense of progress.

6. Turn symbolic_apply into a generator
    
    And make it yield SymbolicApplyResult objects, which we can easily
    extend in the future (e.g., with more information about the code
    path).  For now, spec.py just turns the results back into condition
    lists, but this new interface is much more flexible.
    
13/6/23
======================
1. Generate test case models as we execute paths
    
    Now that symbolic_apply is a generator, we can create test case models
    as we explore code paths.  This also moves the test case model
    generator into a class so that it's easy to integrate into the path
    enumeration loop, and improves the progress display.

13/6/24
======================
1.  Never use existing solver in simsym.check
    
    Previously, simsym.check would use the existing solver if there was
    one and otherwise create a new solver.  This made for an inconsistent
    interface that would account for the current path condition if there
    was one, but check the expression in isolation if used outside
    symbolic execution.  Since we never intentionally use this function
    during symbolic execution, made it check satisfiability in isolation.
    
    This fixes a bug in mmap/mmap that caused it to infinitely enumerate
    models after the switch to the generator symbolic_apply caused the
    model enumerator to use the existing solver instead of a fresh one. 

13/6/25
======================
1. fs.open: Fix time assumptions for creat path
    
    We need to make sure time moves forward in case the creat path of open
    reuses a deleted inode, but previously we made these assumptions
    *after* installing the new SInode object, which would have overwritten
    the existing inode's times with completely unconstrained times.  Move
    these assumptions to before we install the new SInode so they apply to
    the existing deleted inode's time values.

2. symtypes: Method to initialize a key in a map
    
    Currently, we often create a dummy symbolic struct just so we have
    something to assign to a map entry to create that entry.  While this
    is a natural Python approach, creating the dummy struct introduces
    extra symbolic variables that we have to deal with later.  Instead, in
    most of these cases, it makes sense to simply mark the map key as
    valid and then assign to the struct at that key.  The new 'create'
    method facilitates this.

3. fs: Eliminate dummy symbolic variables
    
    We used to have restrictions on the assignment of deeply nested struct
    fields and, as a result, there were several places in the fs model
    that would first create a dummy struct, set it up, and then assign the
    whole struct into a symbolic map or symbolic dictionary.  This is no
    longer a problem, so replace all of these dummy variables with direct
    initialization of symbolic structs in maps and dictionaries.
    Interestingly, not having these dummy variables and related logic
    hanging around eliminates a handful of code paths from open and mmap
    and slightly reduces the number of test cases generated.
    
13/6/26
======================
1. symtypes: Document SDictBase.create
    
    Besides good practice, there are some important subtleties to using
    this method correctly

2. simsym: Abstraction for working with models
    
    This provides an abstraction for working with models in a natural way.
    SymbolicApplyResults can provide a simsym.Model object for their code
    path, which in turn can be indexed using the same variables names
    provided by the user to create Symbolic variables (this is tricky
    because these may not be the same as the underlying Z3 constant
    names).  The returned values re-use the same Symbolic type hierarchy
    that was used during symbolic execution, which enables map indexing
    and struct field lookups like expected, but everything is interpreted
    under the appropriate Z3 model, so these values are rooted in concrete
    Python values.
    
    This also paves the way for test generation-driven isomorphism
    construction, since all concrete value interpretation is done through
    simsym.Model._eval.

3.  Fix variable naming typo in simsym: Abstraction for working with models (见上一个log)
    
    Not sure how I missed this.
    
4. simsym: Rename Symbolic.any to Symbolic.var
    
    'any' was always somewhat misleading because it could return an value
    backed by an already constrained symbolic constant if a name was
    reused.  It became especially problematic with the addition of the
    'model' argument because, if a model was provided, the returned value
    was, in fact, concrete in that model, and could not have any value.
    Hence, rename the method to 'var', which better captures that it
    returns a variable (or what Z3 confusingly calls a constant).
    
5. simsym: Rename SStructBase.constVal to var
    
    Since the interface strictly extends Symbolic.var, it might as well be
    called the same thing.

13/6/27
======================
1.  Fix counter.py example
    
    Though model enumeration doesn't work because there's an integer
    involved and in this case it doesn't make sense to make that integer
    uninterpreted.

2. Fix upipe.py example
    
    Though this has the same model enumeration problem as the counter.

3. simsym: Complain about _assumptions API

4. spec: Use a simsym.tstruct as base class for model states
    
    Previously, we used a specialized model.Struct type, which did
    symbolic equality on a set of declared fields, but nothing else.  Now
    that we have full-fledged mutable symbolic structures, we can use them
    as the base classes of our models.  As a result, we no longer have to
    create each individual model state field; spec.py simply creates a
    symbolic instance of the model state struct.  This also means we don't
    have to give string names to all of the fields; only the top-level
    struct instance created by spec.py has a name now, which will make
    accessing it in the final concrete assignment model much more
    pleasant.

5.  simsym: Fix add_internal of compound values
    
    This broke when we switched to the new Python-side compound value
    representation.  But we never declared any internal variables of
    compound type, so we didn't notice.

6. model: Bundle model method arguments in a struct
    
    Previously, we declared each model method argument as a separate
    symbolic variable named after the argument.  Instead, bundle them all
    in one struct so we only have to name the struct instance.  This will
    make accessing the concrete assignment model cleaner.

7.   appears unnecessary with latest z3 unstable

8. Rename fs-test.py to be import-able

13/6/28
======================
1. symtypes: Prevent 'in' operator on SDict
    
    We can't return a symbolic value for this (Python will force it to a
    bool), nor can we make Python disallow 'in', so throw an exception.
    
2. simsym: Comment
    
    Maybe we should wrap uninterpreted model values?

3. spec: Invoke test generator directly from model enumeration
    
    This eliminates the JSON test case file in preference of invoking test
    case generation directly from the model enumeration code.  This gives
    test case generation direct access to the full Z3 model, which will
    help with much of the complexity and limitations of working from our
    previous half-baked JSON test case file.
    
4. fs: Switch to new test case generation approach
    
    This modifies the FS test case generator to work directly from the
    simsym.Models generated by model enumeration.  This has several
    advantages:
    
    0) Test case generation works again.  Since we're working from the
       simsym Model, the test case generator no longer has to hard-code
       simsym's internal representation of structs and maps.  This had
       gotten out of sync when we changed the representation, breaking
       test generation.
    
    1) We can reuse all of the map and struct mechanism of simsym, so we
       no longer have to do manual array walks and messy struct indexing.
       We even get to use derived types like SDict and SList.
    
    2) We no longer depend on a lossy encoding of the model in JSON.
       Queries of the model are now interpreted directly by Z3.
    
    3) With Z3's model completion support, we no longer need to think
       about default values for variables that aren't in the model.  If we
       ask for a variable that exists but wasn't assigned in the model, Z3
       will supply a default interpretation for it.
    
    Also, we're now in a good place to do test generation-driven
    isomorphism construction.  We can see exactly what assignments the
    test case depended on and use those to drive isomorphism construction,
    rather than guessing based on what Z3 happened to put in the model and
    depending on hand-wavy arguments about what parts of infinite maps
    might matter.
    
5. fs_testgen: Merge SetAllocator and UninterpretedMap into DynamicDict
    
    Taking the best of both.
    
6. fs_testgen: Replace per-process dicts with one DynamicDict
    
    This is a little cleaner and will be important if we want to support
    more than two processes.

7. fs_testgen: Simplify fds and vas DynamicDicts

8. fs_testgen: Remove get_fd and get_va
    
    These are now simple enough that there's no reason to wrap these
    operations.

9. fs_testgen: Simplify fdmap and vamap
    
    Rather than pulling apart the SFd and SVMA structs just to put their
    fields into a dictionary, use the structs directly.
    
    
    
============================================================
对symbolic实现的分析    

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

# Map from Z3 constant names to (outer Symbolic type, compound path)
constTypes = {}

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
        def mkValue(path, sort):
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

Symbolic
------------------------------------------------------------------------------------------------------
   |                |                  |            |           |          |           |             |
SymbolicConst      SSynonymBase    SMapBase      SStructBase  SListBase   SDictBase  SSetBase      SBagBase
   

Symbolic+MetaZ3Wrapper
     |
    SExpr  //__ref_type__ = z3.ExprRef   __wrap__ = ["__eq__", "__ne__"]
     |
   ---------------------------------------------------------------
     |       |                              |                  +SymbolicConst
     |     SEnumBase/STupleBase      SConstMapBase             ----------------------------------------
     |                                z3.ArrayRef              |     |      |         |       |        |  
     | __ref_type__=z3.DatatypeRef                           SFn   SInum   SDataByte  SVa  SPipeId  SBool
     |                                                                                               |   
     |                                                                                               V
     |                                                                    // __ref_type__ = z3.BoolRef __pass_type__ = bool __z3_sort__ = z3.BoolSort()
     |
     |
     SArith  //__ref_type__ = z3.ArithRef
     +
     SymbolicConst
       |
       SInt //__z3_sort__ = z3.IntSort()  __pass_type__ = int


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
m = __import__(args.module)  //args.module=counter，即代表import counter.py module
   ==> class Counter(simsym.tstruct(counter=simsym.SInt)):
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
                            s = base.var(base.__name__)  //初始化状态 base是Counter
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
                                        if fields:
                                            raise AttributeError("Unknown struct field %r" % fields.keys()[0])
                                        return cls._new_lvalue(fvals, __model)
        
        
                                            
                                    
                            r = {}
                            seqname = ''.join(map(lambda i: chr(i + ord('a')), callseq))
                            for idx in callseq:
                                r[idx] = calls[idx](s, chr(idx + ord('a')), seqname)
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
