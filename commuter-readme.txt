def z3_sort_hash(self):
    return hash(str(self))
z3.SortRef.__hash__ = z3_sort_hash
del z3_sort_hash

定义后又删除是啥意思？
