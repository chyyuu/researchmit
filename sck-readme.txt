02/28/2013
==================================
read book "C++ Concurrency in Action" and download src code
g++ -std=c++11 -g listing_1.1.cpp -lpthread

work with nickolai and find a bug!

make HW=mtrace mtrace.out
fix a bug exec.cc (should set pe to be writable)

then in qemu
 $ftest -t 1-10 
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


----------------------------------------
syscall

user interface
o.qemu/lib/sysstubs.S  produced by tools/syscalls.py

kernel interface
sysentry :: kernel/trapasm.S
     sysentry_c :: trap.cc
        syscall :: syscall.cc
          sys_write
              getfile
                ...

proc
   filetable *ftable
   sref<inode> cwd;             // Current directory
   sref<mnode> cwd_m;           // Current directory

class mdir;
class mfile;
class mdev;
class msock;
mfile mnode
----------------------------------------
FS

ref.hh
sref class : a more RCU-friendly referenced base class


nstbl?
