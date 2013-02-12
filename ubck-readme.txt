

Xi Wang tell me a powerful grep replacement tool: ack-grep

​﻿read
http://clang.llvm.org/get_started.html
to install llvm clang  (should use svn trunk code)
according ubck's INSTALL doc, we use the following parameters to LLVM's configure:

        --enable-optimized --enable-targets=host --enable-bindings=none --enable-shared --enable-debug-symbols



read
http://askubuntu.com/questions/76885/where-can-i-find-a-g-4-7-package
to install gcc/g++-4.7.x  (for C++11 standard)

git clone git://g.csail.mit.edu/kint

according ubck's INSTALL doc,
----------------------------------------------
If building from git, first do:
       $ cd kint
        $ autoreconf -fvi

If building from a source tarball, skip the above step.

Then configure and make.

        $ mkdir build
        $ cd build
        $ ../configure
        $ make

Finally, either add `<KINT_ROOT>/build/bin` to PATH, or do:

        $ sudo make install
---------------------------------------------------

Testing
according ubck's INSTALL doc,

$ cd /path/to/your/project
$ kint-build make
$ find . -name "*.ll" > bitcode.lst
$ intglobal @bitcode.lst
$ pintck


You can find bug reports in `pintck.txt`.



===============================
prepare 

. How to write a llvm pass
http://llvm.org/docs/WritingAnLLVMPass.html
http://kitoslab.blogspot.com/2012/10/llvm-pass.html
http://funningboy.blogspot.com/2011/03/sampe-pass-llvm.html


. LLVM’s Analysis and Transform Passes
http://llvm.org/docs/Passes.html


===============================
important files
---------------
build a libsat.a using src/*.cc
libtool: link: ar cru .libs/libsat.a .libs/libsat_la-ValueGen.o .libs/libsat_la-PathGen.o .libs/libsat_la-Diagnostic.o .libs/libsat_la-SMTSolver.o .libs/libsat_la-PHIRange.o .libs/libsat_la-LoopPrepare.o .libs/libsat_la-ElimAssert.o .libs/libsat_la-BugOn.o .libs/libsat_la-BugOnInt.o .libs/libsat_la-BugOnNull.o .libs/libsat_la-BugOnGep.o .libs/libsat_la-BugOnAlias.o .libs/libsat_la-BugOnFree.o .libs/libsat_la-BugOnBounds.o .libs/libsat_la-BugOnUndef.o .libs/libsat_la-BugOnLoop.o .libs/libsat_la-BugOnAssert.o .libs/libsat_la-BugOnLibc.o .libs/libsat_la-BugOnLinux.o .libs/libsat_la-SMTBoolector.o 

build a libintck.so using IntRewrite.cc IntLibcalls.cc IntSat.cc OverflowIdiom.cc OverflowSimplify.cc LoadRewrite.cc and libsat.a
/bin/bash ../libtool --tag=CXX --mode=link g++ `llvm-config --cxxflags` -Werror -Wall -std=c++11 -fno-strict-aliasing -g -O2   -o libintck.la -rpath /usr/local/lib -module IntRewrite.lo IntLibcalls.lo IntSat.lo OverflowIdiom.lo OverflowSimplify.lo LoadRewrite.lo libsat.la 

build a libcmpck.so using  CmpTautology.cc CmpOverflow.cc CmpSat.cc and libsat.a
/bin/bash ../libtool --tag=CXX --mode=link g++ `llvm-config --cxxflags` -Werror -Wall -std=c++11 -fno-strict-aliasing -g -O2   -o libcmpck.la -rpath /usr/local/lib -module CmpTautology.lo CmpOverflow.lo CmpSat.lo libsat.la

build a libantiopt.so using  AntiFunctionPass.cc  AntiDCE.cc AntiSimplify.cc SimplifyDelete.cc  IgnoreLoopInitial.cc LoadElim.cc and libsat.a
/bin/bash ../libtool --tag=CXX --mode=link g++ `llvm-config --cxxflags` -Werror -Wall -std=c++11 -fno-strict-aliasing -g -O2   -o libantiopt.la -rpath /usr/local/lib -module AntiFunctionPass.lo AntiDCE.lo AntiAlgebra.lo AntiSimplify.lo InlineOnly.lo SimplifyDelete.lo IgnoreLoopInitial.lo LoadElim.lo libsat.la 

build tool intglobal from IntGlobal.o Annotation.o CallGraph.o Taint.o Range.o
libtool: link: g++ -I/usr/local/include -D_DEBUG -D_GNU_SOURCE -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS -O3 -fomit-frame-pointer -g -fvisibility-inlines-hidden -fno-exceptions -fno-rtti -fPIC -Woverloaded-virtual -Wcast-qual -Werror -Wall -std=c++11 -fno-strict-aliasing -g -O2 -o intglobal IntGlobal.o Annotation.o CallGraph.o Taint.o Range.o  -L/usr/local/lib -lpthread -lrt -ldl -lm -lLLVM-3.3svn

build lib libintfe.so using IntAction.cc
/bin/bash ../libtool --tag=CXX --mode=link g++ `llvm-config --cxxflags` -Werror -Wall -std=c++11 -fno-strict-aliasing -g -O2   -o libintfe.la -rpath /usr/local/lib -module IntAction.lo

---------------------------------------------------
shell tools

./kint/test/kint-build 
#!/usr/bin/env bash

DIR=$(dirname "${BASH_SOURCE[0]}")
ABS_DIR=$(cd "${DIR}/kint"; pwd)
export PATH="${ABS_DIR}:${PATH}"
export CC="${ABS_DIR}/cc"
export CXX="${ABS_DIR}/c++"
"$@"

--------------------------------
./kint/test/cc1   //is a wrapper of gcc g++ clang.... using python


antiopt 
#!/bin/bash

DIR=$(dirname "${BASH_SOURCE[0]}")
OPT="`llvm-config --bindir`/opt"
exec ${OPT} -load=${DIR}/../lib/libantiopt.so "$@"
--------------

cmpck 
#!/usr/bin/env bash

DIR=$(dirname "${BASH_SOURCE[0]}")
OPT="`llvm-config --bindir`/opt"
exec ${OPT} -disable-output -load=${DIR}/../lib/libcmpck.so \
	-cmp-tautology $@ 2>&1
--------------
intck 
#!/usr/bin/env bash

DIR=$(dirname "${BASH_SOURCE[0]}")
OPT="`llvm-config --bindir`/opt"
exec ${OPT} -disable-output -load=${DIR}/../lib/libintck.so \
	-targetlibinfo -tbaa -basicaa -globalopt -ipsccp -deadargelim \
	-simplifycfg -basiccg -prune-eh -inline -functionattrs -argpromotion \
	-scalarrepl-ssa -early-cse -simplify-libcalls -lazy-value-info \
	-jump-threading -correlated-propagation -simplifycfg \
	-strip-dead-prototypes -globaldce -constmerge \
	-overflow-idiom -adce -simplifycfg \
	-int-rewrite -overflow-simplify -int-libcalls \
	-std-compile-opts \
	-overflow-simplify -adce -simplifycfg \
	-load-rewrite \
	-O2 \
	-int-sat $@ 2>&1

--------------
ncpu 
#!/usr/bin/env python

import multiprocessing

print(multiprocessing.cpu_count())


--------------
optck 
#!/usr/bin/env bash

DIR=$(dirname "${BASH_SOURCE[0]}")
OPT="`llvm-config --bindir`/opt"
exec ${OPT} --disable-output -load=${DIR}/../lib/libantiopt.so \
	-targetlibinfo -tbaa -basicaa -globalopt -sccp -deadargelim \
	-basiccg -prune-eh -simplify-delete -load-elim \
	-inline-only -functionattrs -argpromotion \
	-strip-dead-prototypes \
	-simplify-libcalls -adce \
	-elim-assert \
	-bugon-null \
	-bugon-gep \
	-bugon-bounds \
	-bugon-free \
	-bugon-alias \
	-bugon-int \
	-bugon-libc -bugon-linux \
	-anti-simplify \
	-anti-algebra \
	-anti-dce \
	-show-bugon-true \
	"$@" 2>&1


--------------
pcmpck 
#!/bin/bash

DIR=$(dirname "${BASH_SOURCE[0]}")
NCPU=`${DIR}/ncpu`
OUT='pcmpck.txt'
TIMEOUT=500
find . -name '*.ll' -type f -print0 | xargs -0 -P ${NCPU} -I{} -t bash -c "${DIR}/cmpck -smt-timeout=${TIMEOUT} '{}' > '{}.out'"
rm -f ${OUT}
find . -name '*.ll.out' -type f -print0 | xargs -0 -I{} bash -c "cat '{}' >> ${OUT}"

---------------
pintck 
#!/bin/bash

DIR=$(dirname "${BASH_SOURCE[0]}")
NCPU=`${DIR}/ncpu`
OUT='pintck.txt'
TIMEOUT=500
find . -name '*.ll' -type f -print0 | xargs -0 -P ${NCPU} -I{} -t bash -c "${DIR}/intck -smt-timeout=${TIMEOUT} '{}' > '{}.out'"
rm -f ${OUT}
find . -name '*.ll.out' -type f -print0 | xargs -0 -I{} bash -c "cat '{}' >> ${OUT}"

--------------
poptck 
#!/bin/bash

DIR=$(dirname "${BASH_SOURCE[0]}")
NCPU=`${DIR}/ncpu`
OUT='poptck.txt'
TIMEOUT=5000
find . -name '*.ll' -type f -print0 | xargs -0 -P ${NCPU} -I{} -t bash -c "${DIR}/optck -smt-timeout=${TIMEOUT} '{}' > '{}.out'"
rm -f ${OUT}
find . -name '*.ll.out' -type f -print0 | xargs -0 -I{} bash -c "cat '{}' >> ${OUT}"
