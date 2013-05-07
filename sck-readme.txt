04/30/2013 some info
From: Marc Shapiro -- at work <marc.shapiro@acm.org>
Subject: your SOSP submission

A couple of quick questions or comments on your submission:

 - p 2 "the presence or absence of synchronization instructions
   is orthogonal to scalability,"  Amdahl's Law states exactly
   the opposite.  I think I understand what you mean, thanks to the
   explanation that follows, but its non-obvious to the unprepared reader.
   (Self-contradition: on p. 10 you say the opposite: "Another source of
   non-scalability is coarse grained locking.")

 - Section 5.1 it would have been useful to show an example specification.

 - The Linux implementation of tmpfs doesn't scale so you set out to
   implement your own, except not in Linux.  Thus any comparison between the
   two is unconvincing.  You say: "...because the file system implementation
   is not modular and the ScaleFS design would have required making
   changes throughout the entire Linux kernel."  That's easy to believe.
   However, did you consider the opposite experience: porting tmpfs to xv6,
   confirming the Linux measurements (i.e. the non-scalable parts are in
   tmpfs, not in the rest of the kernel), and *then* comparing to yours?

 - The reference to xv6 violates the anonymity requirements.

                                        Marc
---------------------------------------------------------

03/25/2013
还没完全理解scale-xv6，又开始理解ucore plus了。
在amd64-smp分支下
trap.c中
	SETGATE(idt[T_IPI], 0, GD_KTEXT, __vectors[T_IPI], DPL_USER);
	SETGATE(idt[T_IPI_DOS], 0, GD_KTEXT, __vectors[T_IPI_DOS], DPL_USER);
是啥意思？


03/21/2013
===================
QEMU
./param.h:64:#define PERFSIZE      (16<<20ull)
BEN
./param.h:82:#define PERFSIZE      (128<<20ull)
KVM
./param.h:87:#define PERFSIZE      (128<<20ull)

PERFSIZE 只用在
./kernel/kalloc.cc:1070:  slabmem[slab_perf].order = ceil_log2(PERFSIZE);
./kernel/sampler.cc:27:#define LOG_SEGMENTS (PERFSIZE / LOG_SEGMENT_MAX)

可以看出是设置记录性能采集参数的bufer大小

扩展sysbench.cc 
在做mmap/munmap的test中发现如果迭代次数过多，会out of memory
请教austin, 一起分析，发现可能是mmap(0,...)则每次获得起始地址增长，导致用于管理这个地址的metadata 过量，从而out of memory

过程如下：
sysbench.cc::thr_mmap_munmap中

char *p = (char*) mmap(0, 256 * 1024,

表明要map 64个page
----------------------
qemu-kvm -serial mon:stdio -nographic -numa node -numa node  -m 4g -smp 4 -net user -net nic,model=e1000 -redir tcp:2323::23 -redir tcp:8080::80  -kernel o.kvm/kernel.elf

$ sysbench 4 4 10 0   //表示每个线程要执行400000次  tr_mmap_munmap
kalloc: out of memory
kernel trap 14 err 0x0 cpu 1 cs 16 ds 24 ss 24
  rip ffffffffc014bc9e rsp ffffff000781dcd8 rbp ffffff000781de58
  cr2 0000000000000008 cr3 00000000ab9e1000 cr4 00000000000006b0
  rdi 0000000000000010 rsi 0000000000000000 rdx ffffff000781ddd8
  rcx 00000000000003d5 r8  ffffff00000b8e60 r9  ffffff011fe79000
  rax 0000000000000000 rbx ffffff011ffca002 r10 0000000000000000
  r11 0000000000000213 r12 0000000003dce904 r13 0000000000000000
  r14 ffffff00437bb440 r15 ffffff011ffca0c0 rflags 0000000000010246
  proc: name sysbench pid 109 kstack 0xffffff000781c000
  page fault: non-present page reading 0000000000000008 from kernel mode
  ffffffffc0144933
  ffffffffc01394d3
  ffffffffc015120c
  ffffffffc0152a5b
  0000000000403b61
  0000000000409c12
  0000000000401213
  00000000004001cb
  00000000004018d2
  00000000004039ca



chy@chyhome-PC:/home/devel/bitbucket/xv6/o.kvm$ addr2line -Csfipe kernel.elf
  ffffffffc0144933
  ffffffffc01394d3
  ffffffffc015120c
  ffffffffc0152a5b
sys_munmap(userptr<void>, unsigned long) at sysproc.cc:153
syscall(unsigned long, unsigned long, unsigned long, unsigned long, unsigned long, unsigned long, unsigned long) at syscall.cc:84
sysentry_c at trap.cc:47
sysentry at trapasm.S:86
ffffffffc014bc9e
page_holder::add(sref<page_info, void>&&) at vm.cc:100
 (inlined by) vmap::remove(unsigned long, unsigned long) at vm.cc:258
chy@chyhome-PC:/home/devel/bitbucket/xv6/o.kvm$ python
Python 2.7.3 (default, Aug  1 2012, 05:14:39) 
[GCC 4.6.3] on linux2
Type "help", "copyright", "credits" or "license" for more information.
>>> 64 * 400000 * 4   //64个page, 400000次迭代，4个thread, 这个乘积就是要分配的元数据，根据论文，每个页都有一个元数据管理，我理解一个元数据占64byte，这样一页可放 4096/64个。
102400000
>>> 64 * 400000 * 4 / (4096/64)   //我理解一个元数据占64byte，这样一页可放 4096/64个。这样需要 1600000 页，即6G，即会out of memory
1600000
>>> 1600000 * 4096
6553600000
>>> (1600000 * 4096) / 1024 / 1024
6250
>>> (1600000 * 4096) / 1024 / 1024 / 1024
6

大约要6GB
----------------------
添加 fork, forkexec bench
>40000次会out of memory
现在限制在每个线程2000次
待查


03/20/2013
===================
进一步阅读radixVM论文，了解其实现细节
radixtree, refcache, tlb shutdown, 定位到code！！！

在trap.cc::initmsr函数中
cpu0-(unknown): panic: Cannot control hardware prefetcher for this CPU model
可以把panic 改为cprintf
与austin讨论，这是由于E7 cpu一般enable prefetcher
这样取出的是128B，而不是通常的64B，这样有可能造成false sharing, 在linkbenc，曾经测出5 times 性能差距

austin 找到了qemu multiboot的一个bug, 这样我就可以在qemu or kvm中得到正确的memory了
方法：按照邮件内容修改multiboot.S 编译，安装，但是最后还是要手动地把这个文件拷到对应的目录下！！！
kvm
/home2/chy/opt/share/qemu

qemu
/home2/chy/local/share/qemu
===================================

http://lists.nongnu.org/archive/html/qemu-devel/2013-03/msg02500.html
-----------------------
Previously, the multiboot option ROM set the mmap_length field of the
multiboot info structure to the length of the mmap array *excluding*
the final element of the array, rather than the total length of the
array.  The multiboot specification indicates that this is incorrect,
and it's incompatible with GRUB's [1] and SYSLINUX's [2] multiboot
loaders, which both set mmap_length to the length of the entire mmap
array.

This bug is easy to miss: if the VM is configured with 3584 MB of RAM
or less, the last E820 entry is simply a reserved region that does not
overlap with any other region, so there's no harm in omitting it.
However, if it's started with more than 3584 MB of RAM, the memory
above the high memory hole appears as the last entry in the E820 map
and will be omitted from the multiboot mmap array.

This patch rewrites the loop that constructs the mmap array from the
E820 map to simplify it and fix the final mmap_length value.

[1] grub-core/loader/i386/multiboot_mbi.c:grub_multiboot_make_mbi

[2] com32/mboot/mem.c:mboot_make_memmap

Signed-off-by: Austin Clements <address@hidden>
---
 pc-bios/multiboot.bin         |  Bin 1024 -> 1024 bytes
 pc-bios/optionrom/multiboot.S |   25 +++++++++----------------
 2 files changed, 9 insertions(+), 16 deletions(-)

diff --git a/pc-bios/multiboot.bin b/pc-bios/multiboot.bin
index 
7b3c1745a430ea5e0e15b9aa817d1cbbaa40db14..24690fcc7d49d4751835c58878b9618425427dec
 100644
GIT binary patch
delta 119
address@hidden;KdryK*rZVrajPB|8aZaEt!AcKt|ty|7+V*6}8
z1|Ue=DFP(bI(eIqv!v}%cmWjI72xO_d{(sixJVJ3;_VWq*SAwT?Q|F>hcoVBOq(pq
TWXLG8*_~+z<KzM+4n`#aoCG2B

delta 123
address@hidden;KdryK*rZVrajPB|8aZaEhwAcKt|ty|7*V*6}U
z76t}}w4EYAQY}5L)BjJ}4uuy$kzE0fzQJcji$oP~mk92VVEF#H^LR?9oeo3uahBKT
address@hidden>t9srM{CD#A|

diff --git a/pc-bios/optionrom/multiboot.S b/pc-bios/optionrom/multiboot.S
index 003bcfb..f8d374e 100644
--- a/pc-bios/optionrom/multiboot.S
+++ b/pc-bios/optionrom/multiboot.S
@@ -89,41 +89,34 @@ run_multiboot:
 
        /* Initialize multiboot mmap structs using int 0x15(e820) */
        xor             %ebx, %ebx
-       /* mmap start after first size */
-       movl            $4, %edi
+       /* edi = position in mmap struct list */
+       movl            $0, %edi
 
 mmap_loop:
        /* entry size (mmap struct) & max buffer size (int15) */
        movl            $20, %ecx
        /* store entry size */
-       /* old as(1) doesn't like this insn so emit the bytes instead:
-       movl            %ecx, %es:-4(%edi)
-       */
-       .dc.b           0x26,0x67,0x66,0x89,0x4f,0xfc
+       mov             %cx, %es:(%di)
+       add             $4, %di
        /* e820 */
        movl            $0x0000e820, %eax
        /* 'SMAP' magic */
        movl            $0x534d4150, %edx
        int             $0x15
 
-mmap_check_entry:
+       /* move entry pointer forward */
+       add             $20, %di
        /* last entry? then we're done */
        jb              mmap_done
        and             %bx, %bx
        jz              mmap_done
        /* valid entry, so let's loop on */
-
-mmap_store_entry:
-       /* %ax = entry_number * 24 */
-       mov             $24, %ax
-       mul             %bx
-       mov             %ax, %di
-       movw            %di, %fs:0x2c
-       /* %di = 4 + (entry_number * 24) */
-       add             $4, %di
        jmp             mmap_loop
 
 mmap_done:
+       /* update mmap_length in bootinfo */
+       movw            %di, %fs:0x2c
+
 real_to_prot:
        /* Load the GDT before going into protected mode */
 lgdt:
-- 
----------------------------------------------------------------------

03/14/2013
===================
这两天，进一步阅读代码（主要是初始化部分，比较复杂），fix了另外一个bug（makeifle中的）。
找到了out of memory的问题，但没有fix,因为需要比较深入地了解fs的实现才行。
用c++写os，让我理解得很费劲。 :(
争取抓紧深入理解xv6，测试syscall情况。带学生做好大实验。

03//11/2013
====================
find&fix a little bug in kalloc.ccc with the help of austin
find another bug related with mem leak. try to find the root cause.

try to find the scalability problem of syscall implement.

begin the port work of ucore amd64-smp




03/07/2013
=================================
CPU Core-i73610QM, L3 6MB, 12-way, Cache Line 64B
num of set = 6M/(12*64)=2^13
64=2^6
x-6+1=13
x=18
size of page=4K   y=12
x-y+1=7
number of page color =2^7=128


E7-8870 10 cores L3 30MB cache 

读FOS的master thesis paper "Message Passing in a Factored OS" http://dspace.mit.edu/bitstream/handle/1721.1/66407/755082145.pdf
take care of the 
design of user-level message passing  (barrelfish)
AND
design of kernel-level message passing  (L4)

读 Towards Practical Page Coloring-based Multi-core Cache Management （eurosys 2009）

This paper proposes a hot-page coloring approach in
which cache mapping colors are only enforced on a small
set of frequently accessed (or hot) pages for each process

读ULCC: A User-Level Facility for Optimizing
Shared Cache Performance on Multicores

读Operating system techniques for reducing processor state pollution ph.d thesis


One source of inefficiency that affects the performance of modern processor caches is cache
pollution. Cache pollution can be defined as the displacement of a cache element by a less useful
one.


User Messaging Optimizations
--------------------------
Barrelfish URPC implemention is particularly efficient because it uses a scheme that inval-
idates only a single cache line for sufficiently small messages. This is achieved by
sending data in cache-line sized chunks and reserving the last few bytes for a genera-
tion number. 
 
Memory Remapping
----------------
for large buffer transfer


Hardware Message Passing
-----------------------
Tile64 and intel's SCC


The first is competition and favors running the OS and the
application on separate cores. The second is cooperation and favors running the OS
and application on the same core.

 
Specifically, because each core is dedicated
to a specific task (belonging to either the application or the OS,) the working set size
on any given core is reduced to better fit within the capacity of the local cache as
well as other architectural resources 
======================================== 
------------------------------------------ 
bin目录下有许多小bench. 有80个core上标志的是mapbench 和 mailbench
bench：象是调用其他bench执行的一个总bench
benchhdr: 打印基本内核和配置信息的一个文件，不算bench
fstest:像是用来分析commutativity的，执行了大量的各种open/read/write/close等文件操作

countbench: Benchmark physical page reference counting in the VM system by
            repeatedly duplicating and unmapping a physical page in several
            threads.
crwpbench:  Concurrent reading and writing of a pipe.  Forks n processes with one shared
            pipe.  Even process write a character to the pipe, odd ones read the character. 
allocbench: 许多线程不停地mmap
dirbench: 许多线程不停地open/close 不同的file, 但是一个dir
filebench：许多进程不停的open/read/write同一文件，但r/w的是不同的文件地方的内容
forkexecbench: 创建多个进程执行，即多次执行 fork + execv
gcbench：多进程测试gc,还不够了解，用到了 /dev/sampler ，好像还有/dev/kstat，pmc等都是用于xv6性能统计的

linkbench: Benchmark concurrent stats and links/unlinks.  Ideally, this will
           move a single cache line between stat and link: the cache line for
           the link count.  Our hypothesis is that this is sufficient to limit
           scalability, while tweaking stat to not return the link count will
           lead to perfect scalability of stat.
mailbench: usage: %s n-server-threads n-client-procs nmsg filter deliver", 测试网络的scalability
           多次 recvfrom/sendto server threads<-->client processes.
           
mapbench: 在80个core下做local/batch/global的map/unmap看性能情况，需要再问问austin

schedbench: 简单的基于futex（Fast Userspace muTexes）的多线程访问，查看sched的性能
vmimbalbench: 内存alloc和free操作 （比较新）
        "usage: %s [npages] [consumer,[producers...]]...\n%s
        // $ vmimbalbench 1000000 0,1
        // CPU 0 allocates 4GB, which CPU 1 frees.
        // $ vmimbalbench 1000000 0,8,9,10 16,24,25,26
        // CPU 0 allocates 4GB of pages, which are freed at CPUs 8-10. Likewise with
        // 16 and 24-26.
        // $ vmimbalbench 1000000 0,7 7,0
        // CPU 0 allocates 4GB of pages which are freed at CPU 7. Simultaneously, CPU
        // 7 allocates 4GB of pages which are freed at CPU 0.


           
03/06/2013
=================================
xv6 analysis

在kvm下也可以运行for qemu的kernel.elf,这是好消息，可以测试测试。
按道理这就可以在40 core机器上测试了。

修改dirbench，让它可以在linux和mtrace下运行。

分析了一下现有的cache的模拟实现，思考一下如何在mtrace下实现一个简单的多核cache模拟，
希望可以分析出
用户态<-->内核态来回切换，导致cache污染或冲突的问题。


下面还需尝试直接在硬件上运行？ ipxe方式？

计划，实现一个syscall的方面，


把xv6 mtrace自己编译和使用，进一步熟悉
在kernel目录下有一个文件incbin.S
----------------------------------
#define __str_1(x...)     #x
#define __str(x...)       __str_1(x)

#define include_bin(file, name) \
  .globl _##name##_start; \
  _##name##_start:; \
  .incbin __str(MAKE_OUT/file); \
  _##name##_end:; \
  .globl _##name##_size; \
  _##name##_size:; \
    .quad _##name##_end - _##name##_start

.section .init_rodata, "a", @progbits
include_bin(kernel/initcode,initcode)
include_bin(kernel/bootother,bootother)
include_bin(fs.img,fs_img)
../kernel/incbin.S (END)
------------------------------------
直接把fs.img放进去了，导致kernel.elf巨大
其实也就是把kernel.elf中嵌入了文件，使得整个文件系统都在内存中


系统调用列表是通过tools/syscalls.py实现的
python tools/syscalls.py --kvectors kernel/*.cc > o.qemu/kernel/sysvectors.cc.tmp



03/05/2013
=================================
xv6 analysis

kmtrace.hh
mtstart被调用的地方
idle.cc::mtstart(idleloop,myproc())         //98
proc.cc::mtstart(forkret,myproc())          //136
proc.cc::mtstart(trap,myproc())             //242
proc.cc::mtstart(fn,myproc())               //569
syscall.cc::mtstart(syscalls[num],myproc()) //80
trap.cc::mtstart(trap,myproc())             //40, 49, 161

mtstop被调用的地方
proc.cc::mtstop(myproc())           //138, 238
sched.cc::mtstop(prev)              //300
syscall.cc::mtstop(myproc())        //86
trap.cc::mtstart(myproc())          //67, 81, 316

mtpause被调用的地方
sched.cc::mtpause(prev)             //302
trap.cc::mtpause(myproc())          //160


mtresume被调用的地方
sched.cc::mtresume(next)             //313
trap.cc::mtresume(myproc())          //69, 318

上诉代码将调用 mtrace_entry_register ()
static inline void mtrace_entry_register(volatile struct mtrace_entry_header *h,
					 unsigned long type,
					 unsigned long len)
{
    mtrace_magic(MTRACE_ENTRY_REGISTER, (unsigned long)h,
		 type, len, 0, 0);
}

最终调用到mtrace-magic.h::mtrace_magic
/*
 * Magic instruction for calling into mtrace in QEMU.
 */
static inline void mtrace_magic(unsigned long ax, unsigned long bx, 
				unsigned long cx, unsigned long dx,
				unsigned long si, unsigned long di)
{
    __asm __volatile("xchg %%bx, %%bx" 
		     : 
		     : "a" (ax), "b" (bx), 
		       "c" (cx), "d" (dx), 
		       "S" (si), "D" (di));
}

然后，在qemu中的target-i386/translate.c::disas_insn::L5300中
        do_xchg_reg:
	    /* the instruction for calling into memory trace code? */
	    if (reg == R_EBX && rm == R_EBX)
		gen_helper_mtrace_inst_exec();
            gen_op_mov_TN_reg(ot, 0, reg);
            gen_op_mov_TN_reg(ot, 1, rm);
            gen_op_mov_reg_T0(ot, rm);
            gen_op_mov_reg_T1(ot, reg);
可以看到调用了gen_helper_mtrace_inst_exec函数

位于mtrace/target-i386/op_helper.c::helper_mtrace_inst_exec(void)
    void helper_mtrace_inst_exec(void)
    {
        mtrace_inst_exec(EAX, EBX, ECX, EDX, ESI, EDI);
    }
可以看出 
EAX=MTRACE_ENTRY_REGISTER
EBX=struct mtrace_entry_header *h
ECX=type
EDX=len
ESI=EDI= 0

并进一步调用  mtrace_call[a0](a1, a2, a3, a4, a5);

查看：在mtrace/mtrace.c中
static void (*mtrace_call[])(target_ulong, target_ulong, target_ulong,
			     target_ulong, target_ulong) = 
{
    [MTRACE_ENTRY_REGISTER]	= mtrace_entry_register,
};

所以页就是调用了
mtrace_entry_register（h, type, len, 0, 0）
这个函数是一个总控函数
把gust soft的指令指出的entry拷贝到host中来，然后开始分析。


03/04/2013
==================================
new idea:
The basic question I hope to understand is which is the better
syscall interface and implement for multicore OS?  I hope I can do below things:
1 analyze the  cache behavior in syscall subsystem of mtraced xv6 and linux in qemu.
2 analyze the  cache behavior in syscall subsystem of mtraced xv6 and linux in real machine.
3 follow the flexsc, design new  syscall subsystem for multicore OS,
first step in xv6  and ucore, second in Linux (If we have enough
time).

This week, I will try to analyze the  cache behavior in syscall
subsystem of mtraced xv6 and linux with the qemu.


analyze mtrace.out and mtrace.[ch]

see sck-mtrace-analysis.txt

03/03/2013
==================================
analyze mtrace
see sck-mtrace-analysis.txt

03/01/2013
==================================
read book  "C++ Concurrency in Action" and other web info 
to unstand the concurrent memory model in C++11  (C++多线程内存模型)
在atomic类型提供的三种顺序属性
1. 顺序一致性模型（Sequential Consistency）
2 acquire release ordering : 对顺序性的约束程度介于sequential consistency（顺序一致性）和relaxed ordering之间，
3 relaxed ordering
需进一步看《C++0x漫谈》系列之：多线程内存模型 和参考文献

另外 可进一步了解go的csp模型
http://swtch.com/~rsc/thread/  
Bell Labs and CSP Threads

02/28/2013
==================================
read book "C++ Concurrency in Action" and download src code
g++ -std=c++11 -g listing_1.1.cpp -lpthread

work with nickolai and find a bug!

make HW=mtrace mtrace.out
fix a bug exec.cc (should set pe to be writable)

then in qemu
 $fstest -t 1-10 
 $halt

got mtrace.out, now analyze  
../mtrace/mtrace-tools/mscan --check-testcases

Done! read the results

notice: xv6/libutil/testgen.c

02/27/2013
==================================
build xv6 with mtrace

0 install gcc/g++-4.7.2+ (if gcc-4.6 )

1 get newest xv6
git ssh://chyyuu@amsterdam.csail.mit.edu/home/am0/6.828/xv6.git

2 checkout branch scale-amd64
git checkout -b scale-amd64 origin/scale-amd64

3 test make qemu
make qemu

4 test make mtrace, read README

git ssh://chyyuu@amsterdam.csail.mit.edu/home/am6/mpdev/qemu.git mtrace
cd mtrace
git checkout -b mtrace origin/mtrace
//read README.mtrace

 ./configure --prefix=/home2/chy/local \
            --target-list="x86_64-softmmu" \
            --disable-kvm  \
            --audio-card-list="" \
            --disable-vnc-jpeg \
            --disable-vnc-png \
            --disable-strip
make
make install

set PATH

5 build mtrace in xv6
cd ../xv6
make mtrace.out  // get mtrace.out


6 build mscan 
cd ../mtrace/mtrace-tools
//read README
git clone ssh://chyyuu@am.lcs.mit.edu/home/am6/mpdev/libelfin.git
cd libelfin
cd elf ; make        // build libelf++ lib
cd ../dwarf ; make   //build libdwarf++ lib
cd ../..
make                // now get mscan tool

cd ../xv6
make mscan.out      //done, but don't know how to use mscan tool
  

----------------------
how os support support new lang, such as lang?
----------------------
question:
golang said it support multicore better with goroutine and channel mechanism.
The fact is right?
on xv6 for better scalability
golang beleive: Don't communicate by sharing memory; share memory by communicating.
The idea is true? Can channel mechanism be scalable in current multi core arch? 

-------------------------------------------------------
How OS support channel mechanism (CSP mechanism)?
CSP mechanism can be support in kernel?
OS kernel can use goroutine and channel?

Should vm/sched/fs subsystem support goroutine and channel? 


------------------
memory
------------------
vmap: An address space. This manages the mapping from virtual addresses to virtual memory descriptors. LIKE mm in linux
 in vmap, there is a filed:
  // Virtual page frames
  typedef radix_array<vmdesc, USERTOP / PGSIZE, PGSIZE,
                      kalloc_allocator<vmdesc> > vpf_array;

vmdesc:  is  a virtual memory descriptor that maintains metadata for pages in an address space., LIKE vma in linux
age_tracker: page tracker maintains the per-page metadata necessary to compute TLB shootdowns.  
pgmap :  One level in an x86-64 page table, typically the top level
class class page_map_cache : shootdown::cache_tracker  //A page_map_cache controls the hardware cache of virtual-to-physical page mappings.

Every core has a page table,  so if a process run a several core, then there are several pagetable for process, support lazy create page table .

support 2~3 memory mechanism: see param.h

namespace mmu_shared_page_table {
namespace mmu_per_core_page_table {

// The MMU scheme.  One of:
//  mmu_shared_page_table
//  mmu_per_core_page_table
#define MMU_SCHEME    mmu_per_core_page_table
// The TLB shootdown scheme, for shared page tables.  One of:
//  batched_shootdown
//  core_tracking_shootdown   //calss in hwvm.hh 
#define TLB_SCHEME    core_tracking_shootdown
// Physical page reference counting scheme.  One of:
//  :: for shared reference counters
//  refcache:: for refcache counters
//  locked_snzi:: for SNZI counters
#define PAGE_REFCOUNT refcache::

in cpu.hh
namespace MMU_SCHEME {
  class page_map_cache;
};


---------------
pagefault process
do_pagefault () in trap.cc
  --> pagefault(myproc()->vmap, addr, tf->err) in vm.cc
     -->  vmap->pagefault(va, err);


----------
radix tree V.S. concurrent skip list?? in paper 5.4  ref 14
----------------
gc.cc
---------------
means a rcu implementation for object garbage collection //not used in vm


---------------
sched : implement by Nickolai Zeldovich 泽利多维奇

----------------
thread is  forkt(base + stack_size, (void*) start, arg, FORK_SHARE_VMAP | FORK_SHARE_FD)


kalloc.cc have the Physical page allocator and Slab allocator


** mpboot
void
mpboot(void)
{
  initseg(&cpus[bcpuid]);
  inittls(&cpus[bcpuid]);       // Requires initseg
  initpg();
-----------------------

sck

RadixVM: Scalable address spaces for multithreaded applications

Austin Clements, M. Frans Kaashoek, and Nickolai Zeldovich

chy@chyhome-PC:~/mit/sck/xv6$ git remote -v

origin	ssh://chyyuu@amsterdam.csail.mit.edu/home/am0/6.828/xv6.git (fetch)
origin	ssh://chyyuu@amsterdam.csail.mit.edu/home/am0/6.828/xv6.git (push)
chy@chyhome-PC:~/mit/sck/xv6$ git branch -a
  master
* scale-amd64
  xv6-homework
  remotes/origin/HEAD -> origin/master
  remotes/origin/font
  remotes/origin/master
  remotes/origin/mtrace
  remotes/origin/origin
  remotes/origin/rsc-amd64
  remotes/origin/scale
  remotes/origin/scale-amd64
  remotes/origin/symlinks
  remotes/origin/xv6-homework


------------------------
some important data struct
------------
vmap in vm.hh 
// An address space. This manages the mapping from virtual addresses
// to virtual memory descriptors.

vmdesc in vm.hh
// A virtual memory descriptor that maintains metadata for pages in an
// address space.  This plays a similar role to the more traditional
// "virtual memory area," but this does not know its size (it could
// represent a single page or the entire address space).
struct vmdesc : public mmu::tracker

page_holder ???



some important comments in git
-----
commit 0c8970a4f8e11526e95a6422c1ca56434b2f1a46
Author: Austin Clements <amdragon@mit.edu>
Date:   Mon Aug 20 12:59:17 2012 -0400

    vm: Switch from VMAs to VM descriptors
    
    A VM descriptor records the metadata for a mapping, but not where that
    mapping is.  It is also small, so we can store it directly for each
    mapped page (rather than pointing to a shared reference-counted VMA)
    and designed to be radix node compressable for large mappings.
    
    This switch brings with it several other changes:
    * It eliminates the vmnode's page array, instead embedding the
      canonical page table in the radix array storing the VM descriptors.
      Hence, there are no more limitations on the size of a mapping.
    * Reference counts are tracked per page instead of per mapping, and
      use the refcache.  As a result, pages no longer leak when mappings
      are partially remapped, and we will no longer hit a 64 core limit
      because of the size of the reference counting metadata.
    * COW is performed per page instead of per mapping.
    * File mappings are lazily faulted.
    * Files can only be mapped in whole page increments.  As a result,
      exec must specially handle zero-padded ELF segments by eagerly
      loading the partial page on the boundary between the file-backed
      part of the segment and the zeroed part of the segment.
    * sbrk now unmaps for negative n and doesn't need a buffer page
      between the break and the next mapping.
    
    This currently uses page-level locking for read operations.  This
    needs to be switched to seqlocks for consistent reads of vmdescs plus
    RCU for freeing radix nodes and external elements.
    
    This also doesn't touch the TLB code.  You still have the choice
    between broadcasts and incoherent per-thread page tables.  It does set
    us up to track page/core mappings.
    
    usertests pass.  There is one known bug: since file mappings are now
    lazily faulted, it's possible for us to attempt to sleep in
    vmap::ensure_page -> readi while holding a spinlock (e.g., within a
    page fault).
---------------------------


reference cache scheme : in refcache.hh

// Scalable cached reference counts
//
// This implements space-efficient, scalable reference counting using
// per-core reference delta caches.  Increment and decrement
// operations are expected to be core-local, especially for workloads
// with good locality.  In contrast with most scalable reference
// counting mechanisms, refcache requires space proportional to the
// sum of the number of reference counted objects and the number of
// cores, rather than the product, and the per-core overhead can be
// adjusted to trade off space and scalability by controlling the
// reference cache eviction rate.  Finally, this mechanism guarantees
// that objects will be garbage collected within an adjustable time
// bound of when their reference count drops to zero.
//
// In refcache, each reference counted object has a global reference
// count (much like a regular reference count) and each core also
// maintains a local, fixed-size cache of deltas to object's reference
// counts.  Incrementing or decrementing an object's reference count
// modifies only the local, cached delta and this delta is
// periodically flushed to the object's global reference count.  The
// true reference count of an object is thus the sum of its global
// count and any local deltas for that object found in the per-core
// caches.  The value of the true count is generally unknown, but we
// assume that once it drops to zero, it will remain zero.  We depend
// on this stability to detect a zero true count after some delay.
//
// To detect a zero true reference count, refcache divides time into
// periodic *epochs* during which each core flushes all of the
// reference count deltas in its cache, applying these updates to the
// global reference count of each object.  The last core in an epoch
// to finish flushing its cache ends the epoch and after some delay
// (our implementation uses 10ms) all of the cores repeat this
// process.  Since these flushes occur in no particular order and the
// caches batch reference count changes, updates to the reference
// count can be reordered.  As a result, a zero global reference count
// does not imply a zero true reference count.  However, once the true
// count *is* zero, there will be no more updates, so if the global
// reference count of an object drops to zero and *remains* zero for
// an entire epoch, then the refcache can guarantee that the true
// count is zero and free the object.
//
// This lag between the true reference count and the global reference
// count of an object is the main complication for refcache.  For
// example, consider the following sequence of increments, decrements,
// and flushes for a single object:
//
//         t ->
// core 0     -   *   |       *     * +   |   * +     |
//      1   +         *     * |     *     |       - * |
//      2           * |     * |           *         * |
//      3       *     |     * |         - *           *
// global  1 1 1 1 0 0 1 1 1 1 1 1 1 1 1 1 0 0 1 1 1 0 0
// true    1 2 1 1 1 1 1 1 1 1 1 1 1 1 2 1 1 1 1 2 1 1 1
// epoch   ^----1-----^---2---^-----3-----^-----4-----^
//
// Because of flush order, the two updates in epoch 1 are applied to
// the global reference count in the opposite order of how they
// actually occurred.  As a result, core 0 observes the global count
// temporarily drop to zero when it flushes in epoch 1, even though
// the true count is non-zero.  This is remedied as soon as core 1
// flushes its increment delta, and when core 0 reexamines the object
// at the end of epoch 2, after all cores have again flushed their
// reference caches, it can see that the global count is non-zero and
// hence the zero count it observed was not a true zero and the object
// should not be freed.
//
// It is not enough for the global reference count to be zero when an
// object is reexamined; rather, it must have been zero for the entire
// epoch.  For example, core 0 will observe a zero global reference
// count at the end of epoch 3, and again when it reexamines the
// object at the end of epoch 4.  However, the true count is not zero,
// and the global reference count was temporarily non-zero during the
// epoch.  We call this a *dirty* zero and in this situation the
// refcache will queue the object to be examined again after another
// epoch.
//
// We can extend this approach to support "weak references", which
// provide a controlled way to access an object without preventing its
// reference count from reaching zero.  This is useful in situations
// like caches, where the cache needs to reference objects while still
// allowing them to be garbage collected.  A caller can convert a weak
// reference into a regular reference; this will simply fail if the
// referenced object has already been garbage collected.  A weak
// reference is simply a regular pointer plus a "dying" bit.  When an
// object's global reference count initially reaches zero, refcache
// marks the weak reference dying.  After this, the weak reference can
// either be "revived" when a caller converts it to a regular
// reference by clearing the dying bit, or it can be garbage collected
// after the review process clears both its dying bit and its pointer.
// In a race, which succeeds is determined by which clears the dying
// bit first.
//
// The pseudocode for refcache is given below.  Each core maintains a
// hash table storing its reference delta cache and a "review" queue
// that tracks objects whose global reference counts reached zero.  A
// core reviews an object once it can guarantee that all cores have
// flushed their reference caches after it put the object in its
// review queue.
//
//   flush():
//     evict all cache entries
//     update the current epoch
//
//   evict(object, delta):
//     object.refcnt <- object.refcnt + delta
//     if object.refcnt = 0:
//       if object is not on any review queue:
//         object.dirty <- false
//         add (object, epoch) to the local review queue
//         if object.weakref
//           object.weakref.dying <- true
//       else:
//         object.dirty <- true
//
//   review():
//     for each (object, oepoch) in local review queue:
//       if oepoch <= epoch + 2:
//         remove object from the review queue
//         if object.refcnt = 0:
//           if object.dirty:
//             evict(object, 0)
//           else if object.weakref and not object.weakref.dying:
//             evict(object, 0)
//           else:
//             if object.weakref:
//               object.weakref.pointer <- null
//               object.weakref.dying <- false
//             free object
//         else:
//           if object.weakref:
//             object.weakref.dying <- false
//
//   get_weakref(weakref):
//     weakref.dying <- false
//     if weakref.pointer:
//       inc(weakref.pointer)
//     return weakref.pointer
//
// For epoch management, our current implementation uses a simple
// barrier scheme that tracks a global epoch counter, per-core epochs,
// and a count of how many per-core epochs have reached the current
// global epoch.  This scheme suffices for our benchmarks, but more
// scalable schemes are possible, such as the tree-based quiescent
// state detection scheme used by Linux's hierarchical RCU
// implementation [http://lwn.net/Articles/305782/].








----------------------------------------------------
in radix_array.hh Austin
/**
 * A sparse array with range-oriented modification, run compression,
 * range locking, lock-free lookup, and concurrent independent
 * modifications.
 *
 * For maximum space and time-efficiency, a radix_array requires its
 * value type to store two bits of state information: a "set" bit and
 * a "locked" bit.  It must also support copy construction and copy
 * assignment, which are used for decompression and filling,
 * respectively.  The copy assignment operator must not temporarily
 * clear the lock bit if it is set and, if the caller can read during
 * a concurrent write, concurrent copy assignment and reading must be
 * safe.  Ideally, the value type should also be trivially
 * constructable, but this is not required.  Specifically, the value
 * type must satisfy the following interface
 *
 * <code>
 * struct T
 * {
 *   T();  // ideally = default
 *   T(T& o);
 *   T &operator=(T& o);
 *   bit_spinlock get_lock();
 *   bool is_set() const;
 * };
 * </code>
 *
 * A radix_array often needs large, zeroed blocks of memory.  Because
 * of this, it takes a ZAllocator instead of a regular Allocator.  A
 * ZAllocator<U> supports the standard Allocator<U> interface, plus
 * <code>U* default_allocate()</code>, which allocates a single @c U
 * and default-initializes it.  Allocators that perform pre-zeroing
 * can optimize this operation if @c U has a trivial default
 * constructor.  #zallocator_adaptor can adapt a regular Allocator to
 * this interface.  The internal classes of radix_array are designed to
 * be trivially constructable in the common case, particularly if the
 * value type is trivially constructable.
 *
 * radix_array supports node compression.  If the same value spans
 * large ranges of the array (specifically, the index space covered by
 * an entire radix node), it will be stored just once.
 *
 * Internally, the radix_array breaks the array into pages of size @c
 * NodeBytes.  Pointers to these pages are maintained in a another
 * array, which is also divided into pages of size @c, and so on until
 * there is a single page of pointers (the "root node").  If an entire
 * page would store the same value (or all unset values), then that
 * value is stored in the parent node in lieu of a pointer to the
 * page.  For unset values, this is simply a null pointer; for set
 * values, the value is heap-allocated and the parent node stores this
 * heap pointer (which introduces overhead, but much less overhead
 * than the fully expanded page would).  This compression process
 * continues up to the root.
 *
 * @tparam T Type of values to store in the array.
 * @tparam N Number of elements in the array.
 * @tparam NodeBytes The size of a node in the radix tree, in bytes.
 * @tparam ZAllocator A zero-optimized allocator.  Defaults to the
 * standard allocator.
 */
template<typename T, std::size_t N, std::size_t NodeBytes = 4096,
         typename ZAllocator = zallocator_adaptor<std::allocator<T> > >
class radix_array


----------------------------------------------------
gc.cc
// A simple RCU implementation, but general:
// - processes can call sleep in an epoch
// - processes can migrate during an epoch
//
// The GC scheme is adopted from Fraser's epoch-based scheme:
// a machine has a global_epoch
// a process add to its core global_epoch's delayed freelist on delayed_free()
// a core maintains an epoch, min_epoch (>= global_epoch)
//   min_epoch is the lowest epoch # a process on that core is in
//   a process always ends an epoch on the core it started (even if the
//   process is migrated)
// one gc thread and state (e.g., NEPOCH delaylist) per core
// a gcc thread performs two jobs:
// 1. in parallel gc threads free the elements on the delayed-free lists
//   (costs linear in the number of elements to be freed, but a local operation)
// 2a (global scheme). one gcc thread perform step 1: updates global_epoch
//   (costs linear in the number of cores, but a global operation)
// 2b (local scheme). each core reads the global_min and cur_epoch from its
//   neighbor. it updates its global_min by computing the min(global_min read from
//   neighbor and its min_epoch). it sets its cur_epoch to the 
//   neighbor's cur_epoch.  core 0 is the ring leader: it increases epoch, 
//   if global_min from last core >= cur_epoch-2.  If local communication scales,
//   this gc scheme will scale, but the delay to increase cur_epoch is linear
//   in the number of cores.
//
// To limit the number of delayed free lists per core, each core also has a
// variable nexttofree_epoch, which <= min_epoch. cur_epoch isn't increased
// unless nexttofree_epoch >= cur_epoch-2, implicitly also ensuring the
// constraint that cur_epoch is only increases when min_epoch >= cur_epoch-2.

=====================================================
ssh://chyyuu:iloveqf@amsterdam.csail.mit.edu/home/am0/6.828/xv6.git

used 
https://www.acpica.org/
The ACPI Component Architecture Project

For don't modify Makefile
add x86_64-jos-elf-* in /usr/local/bin

chy@chyhome-PC:~/mit/sck/xv6$ ls -l /usr/local/bin/x86_64-jos-elf-*
lrwxrwxrwx 1 root root 12  2月 12 15:47 /usr/local/bin/x86_64-jos-elf-g++ -> /usr/bin/g++
lrwxrwxrwx 1 root root 12  2月 12 15:47 /usr/local/bin/x86_64-jos-elf-gcc -> /usr/bin/gcc
lrwxrwxrwx 1 root root 11  2月 12 15:49 /usr/local/bin/x86_64-jos-elf-ld -> /usr/bin/ld
lrwxrwxrwx 1 root root 16  2月 12 15:50 /usr/local/bin/x86_64-jos-elf-objcopy -> /usr/bin/objcopy
lrwxrwxrwx 1 root root 14  2月 12 15:50 /usr/local/bin/x86_64-jos-elf-strip -> /usr/bin/strip



$ make
$ make qemu

Then you can see the running status




Component 		Line count		Files
-----------------------------------------------------
Radix tree 		1,377			radix_array.hh
Refcache 		573			refcache.[cc,hh]
MMU abstraction 	813			hwvm.[cc,hh]
Syscall interface 	648			syscall.cc syscalls.py



mmu_shared_page_table in hwvm.*
mmu_per_core_page_table in hwvm.*


Other important files

vm.[cc,hh]


--------------
amd64 vm addr related info
http://www.njyangqs.com/hardware/amd64.htm
http://blog.chinaunix.net/uid-7295895-id-3011309.html
http://hi.baidu.com/chinfs/item/20055efa06cdd94a922af214
http://zh.wikipedia.org/wiki/X86-64

-------------
radix tree related info
http://baike.baidu.com/view/9592206.htm
http://www.cnblogs.com/Bozh/archive/2012/04/15/Radix.html
http://blog.donews.com/cunono/archive/2005/12/06/648663.aspx
* http://blog.sina.com.cn/s/blog_6ce9c5b70100zxnc.html


---------------
C++0x atomic, etc. related info

C++0x 内存模型和原子操作 （std:::atomic memory order等相关资料）
http://hi.baidu.com/widebright/item/ee9d66d2be106dba32db904e

C++11 Features in Visual C++ 11
http://blogs.msdn.com/b/vcblog/archive/2011/09/12/10209291.aspx



-------------------
ACPI related info

https://www.acpica.org/
The ACPI Component Architecture Project

The ACPI Component Architecture (ACPICA) project provides an operating system (OS)-independent reference implementation of the Advanced Configuration and Power Interface Specification (ACPI). It can be easily adapted to execute under any host OS. The ACPICA code is meant to be directly integrated into the host OS as a kernel-resident subsystem. Hosting the ACPICA subsystem requires no changes to the core ACPICA code. Instead, a small OS-specific interface layer is written specifically for each host OS in order to interface the ACPICA code to the native OS services.

---------------------
SMP related info

PIC 、APIC(IOAPIC LAPIC)
http://blog.csdn.net/hgf1011/article/details/5925661



-------------------
mfence instructions around atomic loads
useful ref: http://www.cl.cam.ac.uk/~pes20/weakmemory/cacm.pdf

Page fault description http://en.wikipedia.org/wiki/Page_fault

Minor
If the page is loaded in memory at the time the fault is generated, but is not marked in the memory management unit as being loaded in memory, then it is called a minor or soft page fault. The page fault handler in the operating system merely needs to make the entry for that page in the memory management unit point to the page in memory and indicate that the page is loaded in memory; it does not need to read the page into memory. This could happen if the memory is shared by different programs and the page is already brought into memory for other programs. The page could also have been removed from a process's Working Set, but not yet written to disk or erased, such as in operating systems that use Secondary Page Caching. For example, HP OpenVMS may remove a page that does not need to be written to disk (if it has remained unchanged since it was last read from disk, for example) and place it on a Free Page List if the working set is deemed too large. However, the page contents are not overwritten until the page is assigned elsewhere, meaning it is still available if it is referenced by the original process before being allocated. Since these faults do not involve disk latency, they are faster and less expensive than major page faults.
[edit]Major
If the page is not loaded in memory at the time the fault is generated, then it is called a major or hard page fault. The page fault handler in the operating system needs to find a free page in memory, or choose another non-free page in memory to be used for this page's data, which might be used by another process. In this latter case, the OS first needs to write out the data in that page if it hasn't already been written out since it was last modified, and mark that page as not being loaded into memory in its process page table. Once the page has thus been made available, the OS can read the data for the new page into the physical page, and then make the entry for that page in the memory management unit point to the page in memory and indicate that the page is loaded in memory.[clarification needed] Major faults are more expensive than minor page faults and add disk latency to the interrupted program's execution. This is the mechanism used by an operating system to increase the amount of program memory available on demand. The operating system delays loading parts of the program from disk until the program attempts to use it and the page fault is generated.
[edit]Invalid
If a page fault occurs for a reference to an address that's not part of the virtual address space, so that there can't be a page in memory corresponding to it, then it is called an invalid page fault. The page fault handler in the operating system then needs to terminate the code that made the reference, or deliver an indication to that code that the reference was invalid. A null pointer is usually represented as a pointer to address 0 in the address space; many operating systems set up the memory management unit to indicate that the page that contains that address is not in memory, and do not include that page in the virtual address space, so that attempts to read or write the memory referenced by a null pointer get an invalid page fault.

----------------------------------------
RCU related
[译文]What is RCU, Fundamentally?
* http://fosbin.blog.163.com/blog/static/182746007201133041019851/  ==  * http://blog.csdn.net/wangyunqian6/article/details/8425713

rcu锁机制  * http://blog.csdn.net/wangyunqian6/article/details/8432054

Userspace RCU
http://lttng.org/urcu


---------------
C++11
http://zh.wikipedia.org/wiki/C%2B%2B11	

C++11中值得关注的几大变化
http://blog.csdn.net/lanphaday/article/6564162/details

CODEX codex.hh
----------------
Transactional Synchronization in Haswell
http://software.intel.com/en-us/blogs/2012/02/07/transactional-synchronization-in-haswell/



Problem:
1 meaning of class steal_order in kalloc.cc
2 meaning of gc  in gc.cc
3 content of lb.hh ( balance_pool  used in sched.cc) rnd.hh  AND sched using steal mechanism or some balance struct?



