// Copyright 2009 The Go Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "runtime.h"
#include "arch.h"
#include "defs.h"
#include "malloc.h"
#include "os.h"

bool	runtime·iscgo;

static void unwindstack(G*, byte*);

typedef struct Sched Sched;

M	runtime·m0;
G	runtime·g0;	// idle goroutine for m0

static	int32	debug	= 0;

int32	runtime·gcwaiting;

// Go scheduler
//
// The go scheduler's job is to match ready-to-run goroutines (`g's)
// with waiting-for-work schedulers (`m's).  If there are ready gs
// and no waiting ms, ready() will start a new m running in a new
// OS thread, so that all ready gs can run simultaneously, up to a limit.
// For now, ms never go away.
//
// By default, Go keeps only one kernel thread (m) running user code
// at a single time; other threads may be blocked in the operating system.
// Setting the environment variable $GOMAXPROCS or calling
// runtime.GOMAXPROCS() will change the number of user threads
// allowed to execute simultaneously.  $GOMAXPROCS is thus an
// approximation of the maximum number of cores to use.
//
// Even a program that can run without deadlock in a single process
// might use more ms if given the chance.  For example, the prime
// sieve will use as many ms as there are primes (up to runtime·sched.mmax),
// allowing different stages of the pipeline to execute in parallel.
// We could revisit this choice, only kicking off new ms for blocking
// system calls, but that would limit the amount of parallel computation
// that go would try to do.
//
// In general, one could imagine all sorts of refinements to the
// scheduler, but the goal now is just to get something working on
// Linux and OS X.

struct Sched {
	Lock;

	G *gfree;	// available gs (status == Gdead)

	G *ghead;	// gs waiting to run
	G *gtail;
	int32 gwait;	// number of gs waiting to run
	int32 gcount;	// number of gs that are alive

	M *mhead;	// ms waiting for work
	int32 mwait;	// number of ms waiting for work
	int32 mcount;	// number of ms that have been created
	int32 mcpu;	// number of ms executing on cpu
	int32 mcpumax;	// max number of ms allowed on cpu
	int32 gomaxprocs;
	int32 msyscall;	// number of ms in system calls

	int32 predawn;	// running initialization, don't run new gs.

	Note	stopped;	// one g can wait here for ms to stop
	int32 waitstop;	// after setting this flag
};

Sched runtime·sched;

// Scheduling helpers.  Sched must be locked.
static void gput(G*);	// put/get on ghead/gtail
static G* gget(void);
static void mput(M*);	// put/get on mhead
static M* mget(G*);
static void gfput(G*);	// put/get on gfree
static G* gfget(void);
static void matchmg(void);	// match ms to gs
static void readylocked(G*);	// ready, but sched is locked
static void mnextg(M*, G*);

// Scheduler loop.
static void scheduler(void);

// The bootstrap sequence is:
//
//	call osinit
//	call schedinit
//	make & queue new G
//	call runtime·mstart
//
// The new G does:
//
//	call main·init_function
//	call initdone
//	call main·main
void
runtime·schedinit(void)
{
	int32 n;
	byte *p;

	runtime·allm = m;
	m->nomemprof++;

	runtime·mallocinit();
	runtime·goargs();
	runtime·goenvs();

	// For debugging:
	// Allocate internal symbol table representation now,
	// so that we don't need to call malloc when we crash.
	// findfunc(0);

	runtime·sched.gomaxprocs = 1;
	p = runtime·getenv("GOMAXPROCS");
	if(p != nil && (n = runtime·atoi(p)) != 0)
		runtime·sched.gomaxprocs = n;
	runtime·sched.mcpumax = runtime·sched.gomaxprocs;
	runtime·sched.mcount = 1;
	runtime·sched.predawn = 1;

	m->nomemprof--;
}

// Called after main·init_function; main·main will be called on return.
void
runtime·initdone(void)
{
	// Let's go.
	runtime·sched.predawn = 0;
	mstats.enablegc = 1;

	// If main·init_function started other goroutines,
	// kick off new ms to handle them, like ready
	// would have, had it not been pre-dawn.
	runtime·lock(&runtime·sched);
	matchmg();
	runtime·unlock(&runtime·sched);
}

void
runtime·goexit(void)
{
	g->status = Gmoribund;
	runtime·gosched();
}

void
runtime·tracebackothers(G *me)
{
	G *g;

	for(g = runtime·allg; g != nil; g = g->alllink) {
		if(g == me || g->status == Gdead)
			continue;
		runtime·printf("\ngoroutine %d [%d]:\n", g->goid, g->status);
		runtime·traceback(g->sched.pc, g->sched.sp, 0, g);
	}
}

// Put on `g' queue.  Sched must be locked.
static void
gput(G *g)
{
	M *m;

	// If g is wired, hand it off directly.
	if(runtime·sched.mcpu < runtime·sched.mcpumax && (m = g->lockedm) != nil) {
		mnextg(m, g);
		return;
	}

	g->schedlink = nil;
	if(runtime·sched.ghead == nil)
		runtime·sched.ghead = g;
	else
		runtime·sched.gtail->schedlink = g;
	runtime·sched.gtail = g;
	runtime·sched.gwait++;
}

// Get from `g' queue.  Sched must be locked.
static G*
gget(void)
{
	G *g;

	g = runtime·sched.ghead;
	if(g){
		runtime·sched.ghead = g->schedlink;
		if(runtime·sched.ghead == nil)
			runtime·sched.gtail = nil;
		runtime·sched.gwait--;
	}
	return g;
}

// Put on `m' list.  Sched must be locked.
static void
mput(M *m)
{
	m->schedlink = runtime·sched.mhead;
	runtime·sched.mhead = m;
	runtime·sched.mwait++;
}

// Get an `m' to run `g'.  Sched must be locked.
static M*
mget(G *g)
{
	M *m;

	// if g has its own m, use it.
	if((m = g->lockedm) != nil)
		return m;

	// otherwise use general m pool.
	if((m = runtime·sched.mhead) != nil){
		runtime·sched.mhead = m->schedlink;
		runtime·sched.mwait--;
	}
	return m;
}

// Mark g ready to run.
void
runtime·ready(G *g)
{
	runtime·lock(&runtime·sched);
	readylocked(g);
	runtime·unlock(&runtime·sched);
}

// Mark g ready to run.  Sched is already locked.
// G might be running already and about to stop.
// The sched lock protects g->status from changing underfoot.
static void
readylocked(G *g)
{
	if(g->m){
		// Running on another machine.
		// Ready it when it stops.
		g->readyonstop = 1;
		return;
	}

	// Mark runnable.
	if(g->status == Grunnable || g->status == Grunning || g->status == Grecovery)
		runtime·throw("bad g->status in ready");
	g->status = Grunnable;

	gput(g);
	if(!runtime·sched.predawn)
		matchmg();
}

static void
nop(void)
{
}

// Same as readylocked but a different symbol so that
// debuggers can set a breakpoint here and catch all
// new goroutines.
static void
newprocreadylocked(G *g)
{
	nop();	// avoid inlining in 6l
	readylocked(g);
}

// Pass g to m for running.
static void
mnextg(M *m, G *g)
{
	runtime·sched.mcpu++;
	m->nextg = g;
	if(m->waitnextg) {
		m->waitnextg = 0;
		runtime·notewakeup(&m->havenextg);
	}
}

// Get the next goroutine that m should run.
// Sched must be locked on entry, is unlocked on exit.
// Makes sure that at most $GOMAXPROCS gs are
// running on cpus (not in system calls) at any given time.
static G*
nextgandunlock(void)
{
	G *gp;

	if(runtime·sched.mcpu < 0)
		runtime·throw("negative runtime·sched.mcpu");

	// If there is a g waiting as m->nextg,
	// mnextg took care of the runtime·sched.mcpu++.
	if(m->nextg != nil) {
		gp = m->nextg;
		m->nextg = nil;
		runtime·unlock(&runtime·sched);
		return gp;
	}

	if(m->lockedg != nil) {
		// We can only run one g, and it's not available.
		// Make sure some other cpu is running to handle
		// the ordinary run queue.
		if(runtime·sched.gwait != 0)
			matchmg();
	} else {
		// Look for work on global queue.
		while(runtime·sched.mcpu < runtime·sched.mcpumax && (gp=gget()) != nil) {
			if(gp->lockedm) {
				mnextg(gp->lockedm, gp);
				continue;
			}
			runtime·sched.mcpu++;		// this m will run gp
			runtime·unlock(&runtime·sched);
			return gp;
		}
		// Otherwise, wait on global m queue.
		mput(m);
	}
	if(runtime·sched.mcpu == 0 && runtime·sched.msyscall == 0)
		runtime·throw("all goroutines are asleep - deadlock!");
	m->nextg = nil;
	m->waitnextg = 1;
	runtime·noteclear(&m->havenextg);
	if(runtime·sched.waitstop && runtime·sched.mcpu <= runtime·sched.mcpumax) {
		runtime·sched.waitstop = 0;
		runtime·notewakeup(&runtime·sched.stopped);
	}
	runtime·unlock(&runtime·sched);

	runtime·notesleep(&m->havenextg);
	if((gp = m->nextg) == nil)
		runtime·throw("bad m->nextg in nextgoroutine");
	m->nextg = nil;
	return gp;
}

// TODO(rsc): Remove. This is only temporary,
// for the mark and sweep collector.
void
runtime·stoptheworld(void)
{
	runtime·lock(&runtime·sched);
	runtime·gcwaiting = 1;
	runtime·sched.mcpumax = 1;
	while(runtime·sched.mcpu > 1) {
		// It would be unsafe for multiple threads to be using
		// the stopped note at once, but there is only
		// ever one thread doing garbage collection,
		// so this is okay.
		runtime·noteclear(&runtime·sched.stopped);
		runtime·sched.waitstop = 1;
		runtime·unlock(&runtime·sched);
		runtime·notesleep(&runtime·sched.stopped);
		runtime·lock(&runtime·sched);
	}
	runtime·unlock(&runtime·sched);
}

// TODO(rsc): Remove. This is only temporary,
// for the mark and sweep collector.
void
runtime·starttheworld(void)
{
	runtime·lock(&runtime·sched);
	runtime·gcwaiting = 0;
	runtime·sched.mcpumax = runtime·sched.gomaxprocs;
	matchmg();
	runtime·unlock(&runtime·sched);
}

// Called to start an M.
void
runtime·mstart(void)
{
	if(g != m->g0)
		runtime·throw("bad runtime·mstart");
	if(m->mcache == nil)
		m->mcache = runtime·allocmcache();
	runtime·minit();
	scheduler();
}

// When running with cgo, we call libcgo_thread_start
// to start threads for us so that we can play nicely with
// foreign code.
void (*libcgo_thread_start)(void*);

typedef struct CgoThreadStart CgoThreadStart;
struct CgoThreadStart
{
	M *m;
	G *g;
	void (*fn)(void);
};

// Kick off new ms as needed (up to mcpumax).
// There are already `other' other cpus that will
// start looking for goroutines shortly.
// Sched is locked.
static void
matchmg(void)
{
	G *g;

	if(m->mallocing || m->gcing)
		return;
	while(runtime·sched.mcpu < runtime·sched.mcpumax && (g = gget()) != nil){
		M *m;

		// Find the m that will run g.
		if((m = mget(g)) == nil){
			m = runtime·malloc(sizeof(M));
			// Add to runtime·allm so garbage collector doesn't free m
			// when it is just in a register (R14 on amd64).
			m->alllink = runtime·allm;
			runtime·allm = m;
			m->id = runtime·sched.mcount++;

			if(runtime·iscgo) {
				CgoThreadStart ts;

				if(libcgo_thread_start == nil)
					runtime·throw("libcgo_thread_start missing");
				// pthread_create will make us a stack.
				m->g0 = runtime·malg(-1);
				ts.m = m;
				ts.g = m->g0;
				ts.fn = runtime·mstart;
				runtime·runcgo(libcgo_thread_start, &ts);
			} else {
				if(Windows)
					// windows will layout sched stack on os stack
					m->g0 = runtime·malg(-1);
				else
					m->g0 = runtime·malg(8192);
				runtime·newosproc(m, m->g0, m->g0->stackbase, runtime·mstart);
			}
		}
		mnextg(m, g);
	}
}

// Scheduler loop: find g to run, run it, repeat.
static void
scheduler(void)
{
	G* gp;

	runtime·lock(&runtime·sched);
	if(runtime·gosave(&m->sched) != 0){
		gp = m->curg;
		if(gp->status == Grecovery) {
			// switched to scheduler to get stack unwound.
			// don't go through the full scheduling logic.
			Defer *d;

			d = gp->defer;
			gp->defer = d->link;
			
			// unwind to the stack frame with d's arguments in it.
			unwindstack(gp, d->argp);

			// make the deferproc for this d return again,
			// this time returning 1.  function will jump to
			// standard return epilogue.
			// the -2*sizeof(uintptr) makes up for the
			// two extra words that are on the stack at
			// each call to deferproc.
			// (the pc we're returning to does pop pop
			// before it tests the return value.)
			// on the arm there are 2 saved LRs mixed in too.
			if(thechar == '5')
				gp->sched.sp = (byte*)d->argp - 4*sizeof(uintptr);
			else
				gp->sched.sp = (byte*)d->argp - 2*sizeof(uintptr);
			gp->sched.pc = d->pc;
			gp->status = Grunning;
			runtime·free(d);
			runtime·gogo(&gp->sched, 1);
		}

		// Jumped here via runtime·gosave/gogo, so didn't
		// execute lock(&runtime·sched) above.
		runtime·lock(&runtime·sched);

		if(runtime·sched.predawn)
			runtime·throw("init sleeping");

		// Just finished running gp.
		gp->m = nil;
		runtime·sched.mcpu--;

		if(runtime·sched.mcpu < 0)
			runtime·throw("runtime·sched.mcpu < 0 in scheduler");
		switch(gp->status){
		case Grunnable:
		case Gdead:
			// Shouldn't have been running!
			runtime·throw("bad gp->status in sched");
		case Grunning:
			gp->status = Grunnable;
			gput(gp);
			break;
		case Gmoribund:
			gp->status = Gdead;
			if(gp->lockedm) {
				gp->lockedm = nil;
				m->lockedg = nil;
			}
			unwindstack(gp, nil);
			gfput(gp);
			if(--runtime·sched.gcount == 0)
				runtime·exit(0);
			break;
		}
		if(gp->readyonstop){
			gp->readyonstop = 0;
			readylocked(gp);
		}
	}

	// Find (or wait for) g to run.  Unlocks runtime·sched.
	gp = nextgandunlock();
	gp->readyonstop = 0;
	gp->status = Grunning;
	m->curg = gp;
	gp->m = m;
	if(gp->sched.pc == (byte*)runtime·goexit) {	// kickoff
		runtime·gogocall(&gp->sched, (void(*)(void))gp->entry);
	}
	runtime·gogo(&gp->sched, 1);
}

// Enter scheduler.  If g->status is Grunning,
// re-queues g and runs everyone else who is waiting
// before running g again.  If g->status is Gmoribund,
// kills off g.
void
runtime·gosched(void)
{
	if(m->locks != 0)
		runtime·throw("gosched holding locks");
	if(g == m->g0)
		runtime·throw("gosched of g0");
	if(runtime·gosave(&g->sched) == 0)
		runtime·gogo(&m->sched, 1);
}

// The goroutine g is about to enter a system call.
// Record that it's not using the cpu anymore.
// This is called only from the go syscall library and cgocall,
// not from the low-level system calls used by the runtime.
// Entersyscall cannot split the stack: the runtime·gosave must
// make g->sched refer to the caller's stack pointer.
#pragma textflag 7
void
runtime·entersyscall(void)
{
	runtime·lock(&runtime·sched);
	// Leave SP around for gc and traceback.
	// Do before notewakeup so that gc
	// never sees Gsyscall with wrong stack.
	runtime·gosave(&g->sched);
	if(runtime·sched.predawn) {
		runtime·unlock(&runtime·sched);
		return;
	}
	g->status = Gsyscall;
	runtime·sched.mcpu--;
	runtime·sched.msyscall++;
	if(runtime·sched.gwait != 0)
		matchmg();
	if(runtime·sched.waitstop && runtime·sched.mcpu <= runtime·sched.mcpumax) {
		runtime·sched.waitstop = 0;
		runtime·notewakeup(&runtime·sched.stopped);
	}
	runtime·unlock(&runtime·sched);
}

// The goroutine g exited its system call.
// Arrange for it to run on a cpu again.
// This is called only from the go syscall library, not
// from the low-level system calls used by the runtime.
void
runtime·exitsyscall(void)
{
	runtime·lock(&runtime·sched);
	if(runtime·sched.predawn) {
		runtime·unlock(&runtime·sched);
		return;
	}
	runtime·sched.msyscall--;
	runtime·sched.mcpu++;
	// Fast path - if there's room for this m, we're done.
	if(runtime·sched.mcpu <= runtime·sched.mcpumax) {
		g->status = Grunning;
		runtime·unlock(&runtime·sched);
		return;
	}
	// Tell scheduler to put g back on the run queue:
	// mostly equivalent to g->status = Grunning,
	// but keeps the garbage collector from thinking
	// that g is running right now, which it's not.
	g->readyonstop = 1;
	runtime·unlock(&runtime·sched);

	// Slow path - all the cpus are taken.
	// The scheduler will ready g and put this m to sleep.
	// When the scheduler takes g away from m,
	// it will undo the runtime·sched.mcpu++ above.
	runtime·gosched();
}

// Restore the position of m's scheduler stack if we unwind the stack
// through a cgo callback.
static void
runtime·unwindcgocallback(void **spaddr, void *sp)
{
	*spaddr = sp;
}

// Start scheduling g1 again for a cgo callback.
void
runtime·startcgocallback(G* g1)
{
	Defer *d;

	runtime·lock(&runtime·sched);
	g1->status = Grunning;
	runtime·sched.msyscall--;
	runtime·sched.mcpu++;
	runtime·unlock(&runtime·sched);

	// Add an entry to the defer stack which restores the old
	// position of m's scheduler stack.  This is so that if the
	// code we are calling panics, we won't lose the space on the
	// scheduler stack.  Note that we are locked to this m here.
	d = runtime·malloc(sizeof(*d) + 2*sizeof(void*) - sizeof(d->args));
	d->fn = (byte*)runtime·unwindcgocallback;
	d->siz = 2 * sizeof(uintptr);
	((void**)d->args)[0] = &m->sched.sp;
	((void**)d->args)[1] = m->sched.sp;
	d->link = g1->defer;
	g1->defer = d;
}

// Stop scheduling g1 after a cgo callback.
void
runtime·endcgocallback(G* g1)
{
	Defer *d;

	runtime·lock(&runtime·sched);
	g1->status = Gsyscall;
	runtime·sched.mcpu--;
	runtime·sched.msyscall++;
	runtime·unlock(&runtime·sched);

	// Remove the entry on the defer stack added by
	// startcgocallback.
	d = g1->defer;
	if (d == nil || d->fn != (byte*)runtime·unwindcgocallback)
		runtime·throw("bad defer entry in endcgocallback");
	g1->defer = d->link;
	runtime·free(d);
}

void
runtime·oldstack(void)
{
	Stktop *top, old;
	uint32 argsize;
	byte *sp;
	G *g1;
	static int32 goid;

//printf("oldstack m->cret=%p\n", m->cret);

	g1 = m->curg;
	top = (Stktop*)g1->stackbase;
	sp = (byte*)top;
	old = *top;
	argsize = old.argsize;
	if(argsize > 0) {
		sp -= argsize;
		runtime·mcpy(top->argp, sp, argsize);
	}
	goid = old.gobuf.g->goid;	// fault if g is bad, before gogo

	if(old.free != 0)
		runtime·stackfree(g1->stackguard - StackGuard, old.free);
	g1->stackbase = old.stackbase;
	g1->stackguard = old.stackguard;

	runtime·gogo(&old.gobuf, m->cret);
}

void
runtime·newstack(void)
{
	int32 framesize, argsize;
	Stktop *top;
	byte *stk, *sp;
	G *g1;
	Gobuf label;
	bool reflectcall;
	uintptr free;

	framesize = m->moreframesize;
	argsize = m->moreargsize;
	g1 = m->curg;

	if(m->morebuf.sp < g1->stackguard - StackGuard)
		runtime·throw("split stack overflow");

	reflectcall = framesize==1;
	if(reflectcall)
		framesize = 0;

	if(reflectcall && m->morebuf.sp - sizeof(Stktop) - argsize - 32 > g1->stackguard) {
		// special case: called from reflect.call (framesize==1)
		// to call code with an arbitrary argument size,
		// and we have enough space on the current stack.
		// the new Stktop* is necessary to unwind, but
		// we don't need to create a new segment.
		top = (Stktop*)(m->morebuf.sp - sizeof(*top));
		stk = g1->stackguard - StackGuard;
		free = 0;
	} else {
		// allocate new segment.
		framesize += argsize;
		if(framesize < StackBig)
			framesize = StackBig;
		framesize += StackExtra;	// room for more functions, Stktop.
		stk = runtime·stackalloc(framesize);
		top = (Stktop*)(stk+framesize-sizeof(*top));
		free = framesize;
	}

//printf("newstack frame=%d args=%d morepc=%p morefp=%p gobuf=%p, %p newstk=%p\n",
//frame, args, m->morepc, m->morefp, g->sched.pc, g->sched.sp, stk);

	top->stackbase = g1->stackbase;
	top->stackguard = g1->stackguard;
	top->gobuf = m->morebuf;
	top->argp = m->moreargp;
	top->argsize = argsize;
	top->free = free;
	
	// copy flag from panic
	top->panic = g1->ispanic;
	g1->ispanic = false;

	g1->stackbase = (byte*)top;
	g1->stackguard = stk + StackGuard;

	sp = (byte*)top;
	if(argsize > 0) {
		sp -= argsize;
		runtime·mcpy(sp, m->moreargp, argsize);
	}
	if(thechar == '5') {
		// caller would have saved its LR below args.
		sp -= sizeof(void*);
		*(void**)sp = nil;
	}

	// Continue as if lessstack had just called m->morepc
	// (the PC that decided to grow the stack).
	label.sp = sp;
	label.pc = (byte*)runtime·lessstack;
	label.g = m->curg;
	runtime·gogocall(&label, m->morepc);

	*(int32*)345 = 123;	// never return
}

G*
runtime·malg(int32 stacksize)
{
	G *g;
	byte *stk;

	g = runtime·malloc(sizeof(G));
	if(stacksize >= 0) {
		stk = runtime·stackalloc(stacksize + StackGuard);
		g->stack0 = stk;
		g->stackguard = stk + StackGuard;
		g->stackbase = stk + StackGuard + stacksize - sizeof(Stktop);
		runtime·memclr(g->stackbase, sizeof(Stktop));
	}
	return g;
}

/*
 * Newproc and deferproc need to be textflag 7
 * (no possible stack split when nearing overflow)
 * because they assume that the arguments to fn
 * are available sequentially beginning at &arg0.
 * If a stack split happened, only the one word
 * arg0 would be copied.  It's okay if any functions
 * they call split the stack below the newproc frame.
 */
#pragma textflag 7
void
runtime·newproc(int32 siz, byte* fn, ...)
{
	byte *argp;
	
	if(thechar == '5')
		argp = (byte*)(&fn+2);  // skip caller's saved LR
	else
		argp = (byte*)(&fn+1);
	runtime·newproc1(fn, argp, siz, 0);
}

G*
runtime·newproc1(byte *fn, byte *argp, int32 narg, int32 nret)
{
	byte *sp;
	G *newg;
	int32 siz;

//printf("newproc1 %p %p narg=%d nret=%d\n", fn, argp, narg, nret);
	siz = narg + nret;
	siz = (siz+7) & ~7;
	if(siz > 1024)
		runtime·throw("runtime.newproc: too many args");

	runtime·lock(&runtime·sched);

	if((newg = gfget()) != nil){
		newg->status = Gwaiting;
		if(newg->stackguard - StackGuard != newg->stack0)
			runtime·throw("invalid stack in newg");
	} else {
		newg = runtime·malg(StackBig);
		newg->status = Gwaiting;
		newg->alllink = runtime·allg;
		runtime·allg = newg;
	}

	sp = newg->stackbase;
	sp -= siz;
	runtime·mcpy(sp, argp, narg);
	if(thechar == '5') {
		// caller's LR
		sp -= sizeof(void*);
		*(void**)sp = nil;
	}

	newg->sched.sp = sp;
	newg->sched.pc = (byte*)runtime·goexit;
	newg->sched.g = newg;
	newg->entry = fn;

	runtime·sched.gcount++;
	runtime·goidgen++;
	newg->goid = runtime·goidgen;

	newprocreadylocked(newg);
	runtime·unlock(&runtime·sched);

	return newg;
//printf(" goid=%d\n", newg->goid);
}

#pragma textflag 7
uintptr
runtime·deferproc(int32 siz, byte* fn, ...)
{
	Defer *d;

	d = runtime·malloc(sizeof(*d) + siz - sizeof(d->args));
	d->fn = fn;
	d->siz = siz;
	d->pc = runtime·getcallerpc(&siz);
	if(thechar == '5')
		d->argp = (byte*)(&fn+2);  // skip caller's saved link register
	else
		d->argp = (byte*)(&fn+1);
	runtime·mcpy(d->args, d->argp, d->siz);

	d->link = g->defer;
	g->defer = d;
	
	// deferproc returns 0 normally.
	// a deferred func that stops a panic
	// makes the deferproc return 1.
	// the code the compiler generates always
	// checks the return value and jumps to the
	// end of the function if deferproc returns != 0.
	return 0;
}

#pragma textflag 7
void
runtime·deferreturn(uintptr arg0)
{
	Defer *d;
	byte *argp, *fn;

	d = g->defer;
	if(d == nil)
		return;
	argp = (byte*)&arg0;
	if(d->argp != argp)
		return;
	runtime·mcpy(argp, d->args, d->siz);
	g->defer = d->link;
	fn = d->fn;
	runtime·free(d);
	runtime·jmpdefer(fn, argp);
}

static void
rundefer(void)
{	
	Defer *d;
	
	while((d = g->defer) != nil) {
		g->defer = d->link;
		reflect·call(d->fn, d->args, d->siz);
		runtime·free(d);
	}
}

// Free stack frames until we hit the last one
// or until we find the one that contains the argp.
static void
unwindstack(G *gp, byte *sp)
{
	Stktop *top;
	byte *stk;
	
	// Must be called from a different goroutine, usually m->g0.
	if(g == gp)
		runtime·throw("unwindstack on self");

	while((top = (Stktop*)gp->stackbase) != nil && top->stackbase != nil) {
		stk = gp->stackguard - StackGuard;
		if(stk <= sp && sp < gp->stackbase)
			break;
		gp->stackbase = top->stackbase;
		gp->stackguard = top->stackguard;
		if(top->free != 0)
			runtime·stackfree(stk, top->free);
	}

	if(sp != nil && (sp < gp->stackguard - StackGuard || gp->stackbase < sp)) {
		runtime·printf("recover: %p not in [%p, %p]\n", sp, gp->stackguard - StackGuard, gp->stackbase);
		runtime·throw("bad unwindstack");
	}
}

static void
printpanics(Panic *p)
{
	if(p->link) {
		printpanics(p->link);
		runtime·printf("\t");
	}
	runtime·printf("panic: ");
	runtime·printany(p->arg);
	if(p->recovered)
		runtime·printf(" [recovered]");
	runtime·printf("\n");
}
	
void
runtime·panic(Eface e)
{
	Defer *d;
	Panic *p;

	p = runtime·mal(sizeof *p);
	p->arg = e;
	p->link = g->panic;
	p->stackbase = g->stackbase;
	g->panic = p;

	for(;;) {
		d = g->defer;
		if(d == nil)
			break;
		// take defer off list in case of recursive panic
		g->defer = d->link;
		g->ispanic = true;	// rock for newstack, where reflect.call ends up
		reflect·call(d->fn, d->args, d->siz);
		if(p->recovered) {
			g->panic = p->link;
			if(g->panic == nil)	// must be done with signal
				g->sig = 0;
			runtime·free(p);
			// put recovering defer back on list
			// for scheduler to find.
			d->link = g->defer;
			g->defer = d;
			g->status = Grecovery;
			runtime·gosched();
			runtime·throw("recovery failed"); // gosched should not return
		}
		runtime·free(d);
	}

	// ran out of deferred calls - old-school panic now
	printpanics(g->panic);
	runtime·dopanic(0);
}

#pragma textflag 7	/* no split, or else g->stackguard is not the stack for fp */
void
runtime·recover(byte *argp, Eface ret)
{
	Stktop *top, *oldtop;
	Panic *p;

	// Must be a panic going on.
	if((p = g->panic) == nil || p->recovered)
		goto nomatch;

	// Frame must be at the top of the stack segment,
	// because each deferred call starts a new stack
	// segment as a side effect of using reflect.call.
	// (There has to be some way to remember the
	// variable argument frame size, and the segment
	// code already takes care of that for us, so we
	// reuse it.)
	//
	// As usual closures complicate things: the fp that
	// the closure implementation function claims to have
	// is where the explicit arguments start, after the
	// implicit pointer arguments and PC slot.
	// If we're on the first new segment for a closure,
	// then fp == top - top->args is correct, but if
	// the closure has its own big argument frame and
	// allocated a second segment (see below),
	// the fp is slightly above top - top->args.
	// That condition can't happen normally though
	// (stack pointers go down, not up), so we can accept
	// any fp between top and top - top->args as
	// indicating the top of the segment.
	top = (Stktop*)g->stackbase;
	if(argp < (byte*)top - top->argsize || (byte*)top < argp)
		goto nomatch;

	// The deferred call makes a new segment big enough
	// for the argument frame but not necessarily big
	// enough for the function's local frame (size unknown
	// at the time of the call), so the function might have
	// made its own segment immediately.  If that's the
	// case, back top up to the older one, the one that
	// reflect.call would have made for the panic.
	//
	// The fp comparison here checks that the argument
	// frame that was copied during the split (the top->args
	// bytes above top->fp) abuts the old top of stack.
	// This is a correct test for both closure and non-closure code.
	oldtop = (Stktop*)top->stackbase;
	if(oldtop != nil && top->argp == (byte*)oldtop - top->argsize)
		top = oldtop;

	// Now we have the segment that was created to
	// run this call.  It must have been marked as a panic segment.
	if(!top->panic)
		goto nomatch;

	// Okay, this is the top frame of a deferred call
	// in response to a panic.  It can see the panic argument.
	p->recovered = 1;
	ret = p->arg;
	FLUSH(&ret);
	return;

nomatch:
	ret.type = nil;
	ret.data = nil;
	FLUSH(&ret);
}


// Put on gfree list.  Sched must be locked.
static void
gfput(G *g)
{
	if(g->stackguard - StackGuard != g->stack0)
		runtime·throw("invalid stack in gfput");
	g->schedlink = runtime·sched.gfree;
	runtime·sched.gfree = g;
}

// Get from gfree list.  Sched must be locked.
static G*
gfget(void)
{
	G *g;

	g = runtime·sched.gfree;
	if(g)
		runtime·sched.gfree = g->schedlink;
	return g;
}

void
runtime·Breakpoint(void)
{
	runtime·breakpoint();
}

void
runtime·Goexit(void)
{
	rundefer();
	runtime·goexit();
}

void
runtime·Gosched(void)
{
	runtime·gosched();
}

void
runtime·LockOSThread(void)
{
	if(runtime·sched.predawn)
		runtime·throw("cannot wire during init");
	m->lockedg = g;
	g->lockedm = m;
}

// delete when scheduler is stronger
int32
runtime·gomaxprocsfunc(int32 n)
{
	int32 ret;

	runtime·lock(&runtime·sched);
	ret = runtime·sched.gomaxprocs;
	if (n <= 0)
		n = ret;
	runtime·sched.gomaxprocs = n;
	runtime·sched.mcpumax = n;
	// handle fewer procs?
	if(runtime·sched.mcpu > runtime·sched.mcpumax) {
		runtime·unlock(&runtime·sched);
		// just give up the cpu.
		// we'll only get rescheduled once the
		// number has come down.
		runtime·gosched();
		return ret;
	}
	// handle more procs
	matchmg();
	runtime·unlock(&runtime·sched);
	return ret;
}

void
runtime·UnlockOSThread(void)
{
	m->lockedg = nil;
	g->lockedm = nil;
}

// for testing of wire, unwire
void
runtime·mid(uint32 ret)
{
	ret = m->id;
	FLUSH(&ret);
}

void
runtime·Goroutines(int32 ret)
{
	ret = runtime·sched.gcount;
	FLUSH(&ret);
}
