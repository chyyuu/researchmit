1 driver related thoughts
1.1 commutativity of driver (compare to syscall level)
1.2 synthesis of driver based on the spec of hardware (or auto produce the driver)
1.3 the interface between OS and driver (the spec of interface of driver/OS )
1.4 security of driver (based on commutativity?)
1.5 IO optimization of driver?

上述工作的原则：最小功能集合即可，无需更多功能集合，更多的集合意味着资源的占用和潜在的不安全因素。
这与安全设计里面的“最小权限原则类似”
