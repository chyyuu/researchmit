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
