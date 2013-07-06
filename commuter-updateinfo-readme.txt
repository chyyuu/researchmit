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



注意：这个需要仔细看看!!!
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
    
    
这个函数是干啥的???    
+        def mkValue(name, sort):  //是在symbolic.var函数中调用的symbolic.new_lvalue的调用参数  
                                   //cls._new_lvalue(mkValue((), cls._z3_sort()), model)
+            if isinstance(sort, dict):
+                return {k: mkValue(name + "." + k, v)
+                        for k, v in sort.iteritems()}   我理解是根据sort中的内容创建符号常量
+            return z3.Const(name, sort)
+        return cls._new_lvalue(mkValue(name, cls._z3_sort()))


证据了compound
+# Compounds
+#
+
+# A "compound X" is either an X or a dictionary from component names
+# (e.g., struct fields) to compound X's.
+
+def compound_map(func, *compounds):

在此设置断点，则对于
SPipeMap = symtypes.tmap(SPipeId, SPipe)
 tmap 创建新的type 为
   name ='SMap_SPipeId_SStruct_nread_data'
   base = (SMapBase,),  
   dict=   {"_indexType" : indexType, "_valueType" : valueType, "__z3_sort__" : sort})
     其中
      indexType= <class 'fs.SPipeId'>
      valueType= <class 'simsym.SStruct_nread_data'>
      sort={'nread': Array(PipeId, Int), 'data': {'_len': Array(PipeId, Int), '_vals': Array(PipeId, Array(Int, DataByte))}}


SFdMap = symtypes.tdict(SFdNum, SFd)

name='SMap_SFdNum_SStruct_ispipe_pipewriter_off_pipeid_inum'
base=(SMapBase,), 
dict={"_indexType" : indexType, "_valueType" : valueType,"__z3_sort__" : sort}
    indexType=<class 'simsym.SFdNum'>
    valueType=<class 'simsym.SStruct_ispipe_pipewriter_off_pipeid_inum'>
    sort={'ispipe': Array(Int, Bool), 'pipewriter': Array(Int, Bool), 'off': Array(Int, Int), 'pipeid': Array(Int, PipeId), 'inum': Array(Int, Inum)}

tdict 比 tmap多了一个_valid, 即key可能无效
    base = tstruct(_map = tmap(keyType, valueType),
                   _valid = tmap(keyType, SBool))



                   
13/6/7
===================
1.  Introduce synonym types  simsym.py
   
    These aren't useful yet, but the next patch will introduce a way to
    map a Z3 constant back to its Symbolic type.  Synonym types will let
    us declare new types for things that need to behave like basic types.
    We can then associate information with these types.
    
    比如 SOffset = simsym.tsynonym("SOffset", simsym.SInt)  在处理isomorphisms时可以不用理睬它
    
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


13/7/1
4. commit 30741e53ac7366de04437b255f0a153a5094a3d9
    spec: Note bug in isomorphism for "equal" pseudo-sorts
    
    This hasn't affected so far because we only have one such pseudo-sort
    and it's not worth fixing because this code is about to be replaced
    with test generation-driven isomorphism construction.    
    
13/7/2
1. 
commit 27335f1dd1797ba18efb803309d620c7a44fdcc2
    spec: Better progress reporting
    
    The previous progress reports moved the cursor up a line to overwrite
    the previous progress report, but if we had printed something else
    since the last progress report, this would overwrite the last line of
    that.  Instead, put the cursor in the wrap-around column after
    printing the progress report so that we can still back it up to
    overwrite the current progress report, but anything else written to
    the terminal will cause it to wrap and start on the next line.
    
2.
commit eb109d35db52df484c58b795144546db309775c2

    simsym: Use subclassing for synonym types
    
    Previously, synonym types existed in the type hierarchy, but there was
    never an *instance* of a synonym type because it was always resolved
    to its base type before the value of that type was created.
    Unfortunately, this meant if you had a Symbolic instance, you couldn't
    map it back to a synonym type.  This was going to be a problem when we
    start tracking assignments used by testgen because we won't know the
    synonym types of those expressions, so we wouldn't be able to map them
    back to isomorphism types.
    
    This fixes that by directly subclassing synonym types from their base
    types.

13/7/3
1.
commit 8810ea2c630c2cd63a4122ee108349aa46dcfda9

    simsym: Avoid querying the model for unaccessed struct fields
    
    This would otherwise cause problems when we start tracking the
    expressions evaluated in the model.

2.
commit f3faa696ac500e8f3d8c3512c7af7ac408a92664

    simsym: Track model expression assignments
    
    This will let us track which expression values are actually used
    during test case generation and use these to drive model enumeration.
       
13/7/4
1.
commit 3cff8e28b7387863a59d04e73739ed4d582cd9ab
simsym: Update comments about wrapper classes
    
    I finally really understand what I arrived at organically in the
    simsym class hierarchy.  We're not paralleling Z3's *Ref class
    hierarchy (though that is how it started); rather we're paralleling
    its *sorts* and happen to use a class hierarchy so we can easily share
    common operators between interpreted sorts.

2. 
commit 2d39e99bf8dc1fb4beb1e5e72ba46db7e8101b7f
    spec: Fix interleaving of progress reports and verbose testgen output

3.
commit 2768e91630a74e1ed60ed5286dcb7d14418a469f
    spec: Comment about potential optimization

4.
commit d3df617bfd423998867af9b5175d148831ec240e
    Some generic Z3 utilities
    
    An AST wrapper the makes ASTs hashable and ==-able like regular Python
    objects, a set of ASTs, and some debugging stuff.
5.
commit 9cf592d58f64f5fb249e7a52ecdb31b1bb162720
    spec: Test generation-driven isomorphism construction
    
    This switches from doing purely model-driven isomorphism construction
    to enumerate models to using the expressions evaluated during test
    case generation to drive model enumeration.  The model-driven
    isomorphism construction had to do a lot of approximation to handle
    unbounded sorts, which both failed to capture some potentially
    important models while also producing many models that varied only in
    ways that didn't matter to the generated test cases.  By driving model
    enumeration directly from what's actually used by test generation, we
    can mostly avoid both problems.
    
    This uncovers a few new problems because of the new sensitivity to the
    test case generator, which the next few patches will fix.
6.
commit c3694f2b5501e98134c4b3b55942620833604193
    fs: Ignore SDatabyte
    
    This fixes problems with model enumeration for functions that touch
    file contents.  Without this, we try to enumerate all distinct
    equivalence classes of file contents.  The new model enumeration is
    sensitive to this because the test case generator queries the value of
    each byte in files it creates.

7.
commit 1e9ca0f68aab3feae6d33821ad1f62a87daceb14

    fs: Stricter isomorphism types
    
    With the improved model enumeration, we can eliminate the special
    isomorphism types for SNLink and STime because they are not accessed
    by the test case generator and we can change SOffset from "ignore" to
    "equal" because we no longer have problems with link/link.

8.
commit baab129193cd301bea820c334e49e2273710aff6

    fs_testgen: Don't access 'va' if we don't need it
    
    This was producing unnecessary models.

9.
commit 98df89976af310295e163cba1b13fad2e997c94d

    simsym: Return simsym uninterpreted instance from Model._eval
    
    Previously we leaked a raw Z3 instance if we needed to return an
    uninterpreted value from _eval.

10.
commit 38d96b59b6c18b3c31380ecc3bf52f6f0961f0e5

    simsym: Support uninterpreted sorts directly
    
    Previously simsym had no wrapper for uninterpreted sorts, so fs was
    doing its own wrapping.


13/7/5
--------------
1.
commit cb31867d5cb77b9a73d58868d85198b371d56a74

    fs_testgen: "Freeze" DynamicDict when it is iterated over
    
    fs_testgen has some serious bugs where it consumes a DynamicDict and
    the later adds more items to it.  This catches those bugs.

2.
commit 05aaffe560e8f1fe802556a82cbe7b27d10073f6

    fs_testgen: Simplify DynamicDict using HashableAst

3.
commit d38f449ac2359e5627dca04d36304cde123e1a83

    fs_testgen: Fix consume-before-extend bugs
    
    This makes fs_testgen call setup_proc (which extends inodefiles)
    before calling setup_inodes (which consumes inodefiles).  Previously,
    these were calling in the opposite order, so setup_inodes would fail
    to create inodes files for inodes initialized by setup_proc.

4.
commit f534d0c2849df74bf676d2644751f43e8bbb0670

    simsym: New type assumptions system
    
    The old system was bottom-up, requiring each type to construct a
    single assumption expression that combined its own assumptions,
    superclass assumptions, and any components of compound types.  Besides
    being clunky, it not fully implemented (notably, structs didn't
    support assumptions on their field types), and tended to assume
    expressions that were full of literal 'True's.
    
    This new approach is top-down, with assumptions being declared
    directly at the leaves.  If there are no assumptions to be declared,
    nothing happens.  If there are many assumptions, that's also fine.
    Since no combining is necessary, its easy to extend a type's
    assumptions.
    
    This also fixes a bug where lists in structs (such as inode data in
    the fs model) could have negative lengths because their assumptions
    were not initialized.

5.
commit 4856974d71536898640d08ff198d89c2e249f0bc
Author: Austin Clements <amdragon@mit.edu>
Date:   Fri Jul 5 12:53:24 2013 -0400

    fs: Force SData length to be <= 16 DataBytes
    
    Previously, we capped the length of files in testgen, but this caused
    problems with related values such as offsets, which Z3 had constrained
    relative to what it thought the file length was (which was sometimes
    very large).
    
    Now that we have a better and working assumptions system, move this
    cap into an assumption on the SData type in the fs model.

6.
commit 3ee1d95d67edd4e52fa7e3b8b91786fe48b4b3d0

    fs_testgen: Code generator for pipe
    
    Finally.

7.
commit e942f4de7ed8915c62193a74d46c2eba2475c9ec

    simsym: Fix equality for structs with only concrete-valued fields
    
    Without this, we pass only Python values to z3.And, which makes it
    throw an exception.  Use our symand wrapper instead, which handles this.
    
    This suggests that maybe we should never store concrete Python values
    in symbolic structs.  I thought that would be simpler, but its
    consequences keep spreading.



