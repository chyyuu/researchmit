
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


ssh://chyyuu:iloveqf@amsterdam.csail.mit.edu/home/am0/6.828/xv6.git

used 
https://www.acpica.org/
The ACPI Component Architecture Project

The ACPI Component Architecture (ACPICA) project provides an operating system (OS)-independent reference implementation of the Advanced Configuration and Power Interface Specification (ACPI). It can be easily adapted to execute under any host OS. The ACPICA code is meant to be directly integrated into the host OS as a kernel-resident subsystem. Hosting the ACPICA subsystem requires no changes to the core ACPICA code. Instead, a small OS-specific interface layer is written specifically for each host OS in order to interface the ACPICA code to the native OS services.

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




