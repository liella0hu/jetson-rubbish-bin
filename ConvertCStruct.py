"""
兼容python与micropython的一款c结构体类型数据与python类对象的转换
支持常用的c整形和浮点型，支持一维数组，支持c结构体字节对齐
提供了数据序列化以及反序列化的方法
常用于通信转码
# 但注意数据出来的可能并不一致，这是因为结构体是需要固定的类型，而python的类型是动态的，如果超出类型范围的话可能会变为0
支持直接使用.成员名 进行读写，对于数组，支持传list或者按照索引修改
标准cpython下支持与结构体数据同步修改，修改时会检查数据错误以及溢出

micropython的平台下仅支持部分功能，无法同步修改，因此只有在调用toBytes和fromBytes时才会更新类实例与cstruct的数据，
其他过程用户修改变量，中途程序无法检查修改的变量是否符合要求，若用户自己未进行数据溢出检查则可能和实际转换成功cstruct不符合，可能直到用户调用toBytes和fromBytes才会触发某些错误
"""
import sys

MAX_STRUCT_SIZE=1024*2 #结构体最大长度

#c结构体基类，其他平台可继承这个类进行一些实现
class SimBaseTypes:
    _py_name=sys.implementation.name
    # _py_name="micropython"#测试
    c_char=None

    c_int8=None
    c_int16=None
    c_int32=None
    c_int64=None

    c_uint8=None
    c_uint16=None
    c_uint32=None
    c_uint64=None
    c_float=None
    c_double=None
    class ByteArrayType(bytearray):
        def __init__(self,parent,name,max_len,*args):
            super().__init__(*args)
            self.parent=parent
            self.name=name
            self.max_len=max_len
        def __setitem__(self, index, value):
            if not isinstance(index,int):
                str_msg="invalid index: %s , '%d' : only supported single index "%(index,self.name)
                raise ValueError(str_msg)
            if index >= self.max_len or abs(index) > self.max_len:
                str_msg="invalid index: %d , '%s' max length %d"%(index,self.name,self.max_len)
                raise IndexError(str_msg)
            n_value = self.parent._setattrIndex_(self.name,index,value)
            super().__setitem__(index, n_value)
    class ListArrayType(list):
        def __init__(self,parent,name,max_len,*args):
            super().__init__(*args)
            self.parent=parent
            self.name=name
            self.max_len=max_len
        def __setitem__(self, index, value):
            if not isinstance(index,int):
                str_msg="invalid index: %s , '%s' : only supported single index "%(index,self.name)
                raise ValueError(str_msg)
            if index >= self.max_len or abs(index) > self.max_len:
                str_msg="invalid index: %d , '%s' max length %d"%(index,self.name,self.max_len)
                raise IndexError(str_msg)
            n_value = self.parent._setattrIndex_(self.name,index,value)
            super().__setitem__(index, n_value)
    def __init__(self,struct_list) -> None:
        self.struct_data=None
        self._struct_list=struct_list # [[type1,name1],[type2,name2,size],...]
        self._name_set = dict()
        for item in struct_list:
            if len(item) < 2:
                str_msg = "%s should have at least two parameters[name,type] or [name,type,size]"%(item[1])
                raise TypeError(str_msg)
            if item[0] is None:
                str_msg = "var name: '%s' is Unsupported type"%(name)
                raise NameError(str_msg)
            name = item[1]
            if name in self._name_set:
                str_msg = "'%s' is repeatedly defined"%(name)
                raise NameError(str_msg)
            self._name_set.update({name:item})
            if len(item) > 2:
                if not isinstance(item[2],int):
                    str_msg = "%s of '"%(item[1])+str(item[2])+"' should be 'int' but is %s"%(type(item[2]))
                    raise ValueError(str_msg)
                if item[2]>1:
                    if item[0] == self.c_char:
                        self._set_member_value(self,name,SimBaseTypes.ByteArrayType(self,name,item[2],b'\0'*item[2]))
                    else:
                        self._set_member_value(self,name,SimBaseTypes.ListArrayType(self,name,item[2],[0]*item[2]))
                else:
                    self._set_member_value(self,name,0)
            else:
                self._set_member_value(self,name,0)

    def _set_member_value(self,target,member_str,value,index=None):
        global_var={"target":target,'member_str':member_str,'value':value}
        if index is None:
            run_code="target.%s=value"%(member_str)
        else:
            run_code="target.%s[%d]=value"%(member_str,index)
        exec(run_code,global_var)

    def __str__(self):
        return str(dict(self))
    def __iter__(self):#迭代器支持
        self.index = 0
        return self
    def __next__(self):#迭代器支持
        if self.index < len(self._struct_list):
            name = self._struct_list[self.index][1]
            value = getattr(self,name)
            self.index += 1
            return name,value
        else:
            raise StopIteration()
    def _setattrIndex_(self, name,idx, value): #索引修改拦截,检查数据是否成功修改
        if hasattr(self,"_name_set") and name in self._name_set:
            if not self.struct_data is None and hasattr(self.struct_data,name):
                item=self._name_set[name]
                v=self._type_value_convert(item[0],value)
                # print("value old:",value,' new:',v)
                return v
        return value
    def __setattr__(self, name, value): #修改拦截
        if hasattr(self,"_name_set") and name in self._name_set:
            item = self._name_set[name]
            if not self.struct_data is None and hasattr(self.struct_data,name):
                if len(item)>2 and int(item[2])>1:
                    for i,v in enumerate(value):
                        nv=self._type_value_convert(item[0],v)
                        self._set_member_value(self,name,nv,i)
                    return
                value=self._type_value_convert(item[0],value)
        return super().__setattr__(name,value)
    def _data_assign_var(self,target,src,name):
        if hasattr(self,"_name_set") and name in self._name_set:
            item = self._name_set[name]
            size=1
            if len(item)>2:
                size=int(item[2])
            global_var={"target":target,'src':src}
            if size==1: #非数组，可直接赋值
                run_code='target.%s=src.%s'%(name,name)
                exec(run_code,global_var)
            else: #数组
                self_name = None
                if src==self:
                    self_name='src'
                elif target==self:
                    self_name='target'
                if not self_name is None: #当从当前类拷贝到ctype struct时，考虑c_char未非比较修改项，只修改前n项
                    global_var["size"]=size
                    run_code='size=len(%s.%s)'%(self_name,name)
                    exec(run_code,global_var)
                    less_size=size-global_var["size"]
                    if less_size>0:
                        if item[0]==self.c_char:
                            run_code='%s.%s+=bytes(%d)'%(self_name,name,less_size)
                        else:
                            run_code='%s.%s+=[0]*%d'%(self_name,name,less_size)
                        exec(run_code,global_var)
                if target==self and item[0]==self.c_char: #bytes转bytearray
                    global_var["is_bytes"]=False

                    run_code='is_bytes=isinstance(target.%s,bytes)'%(name)
                    exec(run_code,global_var)
                    if global_var["is_bytes"]:
                        global_var["self"]=self
                        global_var["name"]=item[1]
                        global_var["size"]=size
                        global_var["ByteArrayType"]=self.ByteArrayType
                        run_code='target.%s=ByteArrayType(self,name,size,target.%s)'%(name,name)
                        exec(run_code,global_var)
                for i in range(size):
                    run_code='target.%s[%d]=src.%s[%d]'%(name,i,name,i)
                    exec(run_code,global_var)
    def _data_assign(self,target,src):
        for item in self._struct_list:
            name = item[1]
            self._data_assign_var(target,src,name)
    def toBytes(self) -> bytes:
        raise TypeError("Cannot use base class methods 'toBytes'")
    def fromBytes(self,bytes_data : bytes):
        raise TypeError("Cannot use base class methods 'toBytes'")
    def _type_value_convert(self,type_item,value): #检查数据是否符合类型范围，不符合的话将会计算求余的值
        raise TypeError("Cannot use base class methods '_type_value_convert'")
        return None
#适配标准python带有ctypes的平台
class PySimTypes(SimBaseTypes):
    if SimBaseTypes._py_name != "micropython":
    # if True:
        import ctypes
        _ctypes = ctypes
        # class sim_c_char(ctypes.c_uint8):
        #     pass
        c_char=ctypes.c_uint8
        c_int8=ctypes.c_int8
        c_int16=ctypes.c_int16
        c_int32=ctypes.c_int32
        c_int64=ctypes.c_int64

        c_uint8=ctypes.c_uint8
        c_uint16=ctypes.c_uint16
        c_uint32=ctypes.c_uint32
        c_uint64=ctypes.c_uint64

        c_float= ctypes.c_float
        c_double=ctypes.c_double
    def __init__(self, struct_list):
        super().__init__(struct_list)
        n_struct_list=[]
        for item in self._struct_list:
            c_type=item[0]
            name=item[1]
            size = 1
            if len(item)>2:
                size=item[2]
            if size>1:
                c_type = c_type * size
            n_struct_list.append((name,c_type))
        class Structure(PySimTypes._ctypes.Structure):
            _fields_ = n_struct_list
        self.Structure=Structure

        self.struct_data=Structure()
        struct_size=len(self)
        str_msg = "structure size: %d out of limit size: %d "%(struct_size,MAX_STRUCT_SIZE)
        assert struct_size <= MAX_STRUCT_SIZE,str_msg
    def __len__(self):
        return PySimTypes._ctypes.sizeof(self.struct_data)
    def toBytes(self) -> bytes:
        self._data_assign(self.struct_data,self) #经过处理后再返回结果
        packed_data = PySimTypes._ctypes.string_at(PySimTypes._ctypes.addressof(self.struct_data), PySimTypes._ctypes.sizeof(self.struct_data))
        return packed_data
    def fromBytes(self,bytes_data:bytes):
        self.struct_data=self.Structure.from_buffer_copy(bytes_data)
        self._data_assign(self,self.struct_data)
    def _type_value_convert(self,type_item,value): #检查数据是否符合类型范围，不符合的话将会计算求余的值
        #raise TypeError("Cannot use base class methods '_type_value_convert'")
        return type_item(value).value
#适配micropython平台（如k210）
class MicroSimTypes(SimBaseTypes):
    if SimBaseTypes._py_name == "micropython":
    # if True:
        import uctypes
        _uctypes = uctypes
        c_char=uctypes.UINT8
        c_int8=uctypes.INT8
        c_int16=uctypes.INT16
        c_int32=uctypes.INT32
        # c_int64=uctypes.INT64

        c_uint8=uctypes.UINT8
        c_uint16=uctypes.UINT16
        c_uint32=uctypes.UINT32
        # c_uint64=uctypes.UINT64

        c_float= uctypes.FLOAT32
        # c_double=uctypes.FLOAT64
        class ByteArrayType(bytearray):
            def __init__(self,parent,name,max_len,*args):
                super().__init__(*args)
        class ListArrayType(list):
            def __init__(self,parent,name,max_len,*args):
                super().__init__(*args)
        SimBaseTypes.ByteArrayType=ByteArrayType
        SimBaseTypes.ListArrayType=ListArrayType
        type_size_map={
            c_char:1,
            c_int8:1,
            c_int16:2,
            c_int32:4,
            # c_int64:8,
            c_uint8:1,
            c_uint16:2,
            c_uint32:4,
            # c_uint64:8,
            c_float:4,
            # c_double:8,
        }
    def __init__(self, struct_list):
        super().__init__(struct_list)
        self.VarAddrStart,self.struct_size=self.alignment_cstruct()
        keys=[]
        values=[]
        for addr,item in zip(self.VarAddrStart,self._struct_list):
            name = item[1]
            value = item[0]
            size=1
            if len(item)>2 and item[2]>1:
                size=item[2]
                value = (addr | MicroSimTypes._uctypes.ARRAY, size | value)
            else:
                value = addr |value
            keys.append(name)
            values.append(value)
        self.Structure=dict(zip(keys,values))
        struct_size=len(self)
        str_msg = "structure size: %d out of limit size: %d "%(struct_size,MAX_STRUCT_SIZE)
        assert struct_size <= MAX_STRUCT_SIZE,str_msg
        self.Structure_buf = bytearray(len(self))
        self.struct_data = MicroSimTypes._uctypes.struct(MicroSimTypes._uctypes.addressof(self.Structure_buf), self.Structure)
    def alignment_cstruct(self):
        #适用于字节对齐（不适用位域结构体的位对齐，不适用于嵌套结构体，只支持基础类型）
        max_byte_size=1
        offest_addrs=[0]*len(self._struct_list)
        #找到最长度变量
        for i,item in enumerate(self._struct_list):
            type_name = item[0]
            var_name = item[1]
            if not type_name in MicroSimTypes.type_size_map:
                str_msg = "invalid type! var name:'%s'"%(var_name)
                raise TypeError(str_msg)
            type_size=MicroSimTypes.type_size_map[type_name]
            if type_size > max_byte_size:
                max_byte_size=type_size
        offest=0
        offest_addrs=[]
        #计算每个变量
        for i in range(len(self._struct_list)):
            item=self._struct_list[i]
            type_name,name=item[:2]
            var_num=1
            type_size = MicroSimTypes.type_size_map[type_name]
            less_size=max_byte_size - offest%max_byte_size #检查需要填充大小
            if less_size<type_size: #剩余空间至少能填充一个当前变量的类型，因此可以可以追加
                offest+=less_size #为了对齐需要跳过这部分数据
            if len(item)>2:
                var_num=int(item[2])
            offest_addrs.append(offest) #输入当前类型的偏移量(开始地址)
            var_size = type_size*var_num  #变量实际利用空间
            offest+=var_size
        if offest%max_byte_size!=0:
            offest+=max_byte_size - offest%max_byte_size
        return offest_addrs,offest
    def __len__(self):
        # return MicroSimTypes._uctypes.sizeof(self.Structure,MicroSimTypes._uctypes.LITTLE_ENDIAN)
        return self.struct_size
    def toBytes(self) -> bytes:
        self._data_assign(self.struct_data,self)
        self._data_assign(self,self.struct_data)
        return self.Structure_buf
    def fromBytes(self,bytes_data:bytes):
        self.Structure_buf=bytes_data
        self.struct_data=MicroSimTypes._uctypes.struct(MicroSimTypes._uctypes.addressof(self.Structure_buf), self.Structure)
        self._data_assign(self,self.struct_data)
    def _type_value_convert(self,type_item,value): 
        return value

if SimBaseTypes._py_name != "micropython":
    SimTypes=PySimTypes
else:
    SimTypes=MicroSimTypes

if __name__ == "__main__":
    UserStruct=[
        [SimTypes.c_int8,'aa',1],
        [SimTypes.c_float,'a',4],
        (SimTypes.c_int8,'b',2),
        [SimTypes.c_uint32,'c'],
    ]
    p=SimTypes(UserStruct)
    # p=PySimTypes(UserStruct)

    p.aa=1
    p.a=[1,2,3,4]
    p.b=[1,266]
    p.b[1]=99
    p.a[1]=1
    p_bytes = p.toBytes() #序列化数据
    print("初始结构体：",p)
    print("序列化数据:",p_bytes)
    var=SimTypes(UserStruct)
    var.fromBytes(p_bytes)
    print("反序列化构体：",var)

