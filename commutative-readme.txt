def z3_sort_hash(self):
    return hash(str(self))
z3.SortRef.__hash__ = z3_sort_hash
del z3_sort_hash

定义后又删除是啥意思？

dictionary SDict (keyType, valueType) {
   SMap (keyType, valueType)
   SMap (keyType, SBool)
}

struct SProc  {
   SFdMap fd_map 
   SVaMap va_map
}

dictionary SVaMap::SDict{
   SMap [Sva]=SVMA  _map    //  va --> vma
   SMap [Sva]=Sbool _valid  //  va is valid
}

dictionary SFdMap ::SDict{
   SMap [SInt]=SFd   _map   // fd_num -->inode num
   SMap [SInt]=Sbool _valid // fd_num is valid
}

struct SFd {
   SInum  inum             // inode num (uninterpreted)
   SInt   off              // offset
}


//no mmap length of a anon mem area
struct SVMA {
 SBool anon                 // is a anonymous memory?
 SBool writable             //writable ?
 SInum inum                 // inode number if mapped a file
 SInt  off                  // read/write offset of file ???
 SDataByte anondata         // the anon memory area
}

// for dir
array/map SIMap [SInum] = SInode

array 

FS(or 将来进化到OS)的model全局变量
   
 self.i_map = SIMap.any('Fs.imap')    //root dir's inode map
 self.proc0 = SProc.any('Fs.proc0')   //proc0 struct
 self.proc1 = SProc.any('Fs.proc1')   //proc1 strcut

