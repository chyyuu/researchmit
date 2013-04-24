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

定义后又删除是啥意思？

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
       


FS (or 将来进化到OS)的model全局变量
// for dir   
 self.i_map = SIMap.any('Fs.imap')    
//root dir's inode map
self.root_dir = SDirMap.any('Fs.rootdir')
// for proc
 self.proc0 = SProc.any('Fs.proc0')   //proc0 struct
 self.proc1 = SProc.any('Fs.proc1')   //proc1 strcut
 
 
 
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
test --> calls[0|1]-->munmap
即
r[idx] = calls[idx](s, chr(idx + ord('a')), seqname)  //s=是类Fs的一个object实例
 -->
    munmap（self, whichcall=(a|b), whichseq=(ab|ba))
    
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
我理解internal表示内部变量，即执行后的state情况。
        
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
 
 
 
 
