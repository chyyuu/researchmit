

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
 而SExpr设置了属性__metaclass__，结果SInt, SArith,....都需要在创建类是执行MetaZ3Wrapper的__new__函数。
 
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
    ...
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
  
IsomorphicMatch中的函数有对 pseudo_sort_decls和pseudo_sort_ignore 的判断  
  
    def add_assignment(self, expr, val):  
        ...
        for d, sortname in pseudo_sort_decls:
            if not expr.decl().eq(d): continue
            if pseudo_sort_ignore[sortname]: return
            self.add_assignment_uninterp(expr, val, sortname)
            return
        .....
