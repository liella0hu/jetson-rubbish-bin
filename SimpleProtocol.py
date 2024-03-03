from ConvertCStruct import SimTypes


#缓冲区大小
SIMPRO_BUF_SIZE=256
SIMPRO_HEAD=0xab
SIMPRO_TAIL=0xcd
SIMPRO_PAD=0xea #填充的flab，避免在数据包内出现head或tail重复,遇到ea则会跳过下一个字符的head和tail检查

SIMPROTOL_SIZE=5 #协议包大小，不包含数据，即最小发送的数据长度,协议固定参数,请勿修改

def SimPro_CRC8(crc,buf):
    if isinstance(crc,bytes):
        crc = int.from_bytes(crc,'little')
    crc %= 0x100
    for b in buf:
        if isinstance(b,bytes):
            b = int.from_bytes(b,'little')
        crc=table[crc ^ b]
        crc %= 0x100
    return crc

class SimpleProtocol:
    def __init__(self) -> None:
        self.__recv_buf=b''
        self.__head_tail_buf=[]
        self.__pack_id=0
        self.recv_id=0
        self.__send_bytes=b''
        self.__last_recv_pad=False
        self.__has_head=False
    def __SimPro_EnSendChar(self,cb):
        if not isinstance(cb,bytes):
            cb=cb.to_bytes(1,'little')
        self.__send_bytes+=cb
    def __SimPro_CheckPad(self,c):
        return  c == SIMPRO_HEAD or c == SIMPRO_TAIL or c == SIMPRO_PAD
        return  c == SIMPRO_HEAD[0] or c == SIMPRO_TAIL[0] or c == SIMPRO_PAD[0]
    def SimPro_Packet(self,pack):
        self.__send_bytes = b''
        val_sum = 0
        assert len(pack)<250," The maximum length of a pack is 250！"
            
        self.__SimPro_EnSendChar(0)
        self.__SimPro_EnSendChar(SIMPRO_HEAD)
        self.__SimPro_EnSendChar(SIMPRO_PAD)
        self.__SimPro_EnSendChar(self.__pack_id)
        self.__SimPro_EnSendChar(SIMPRO_PAD)
        self.__SimPro_EnSendChar(len(pack)%0x100)
        self.__SimPro_EnSendChar(SIMPRO_PAD)
        self.__SimPro_EnSendChar(SimPro_CRC8(self.__pack_id,pack))
        
        for c in pack:
            if self.__SimPro_CheckPad(c):
                self.__SimPro_EnSendChar(SIMPRO_PAD)
                
            self.__SimPro_EnSendChar(c)
        self.__SimPro_EnSendChar(SIMPRO_TAIL)
        self.__pack_id=(self.__pack_id+1)%0x100
        return self.__send_bytes
    def SimPro_EnChar(self,byets):
        for bt in byets:
            if self.__last_recv_pad:
                if self.__has_head:
                    self.__recv_buf+=bt.to_bytes(1,'little')
            elif bt==SIMPRO_PAD:
                pass
            elif bt == SIMPRO_HEAD:
                self.__recv_buf=bt.to_bytes(1,'little')#清空之前的
                self.__has_head=True
            elif bt == SIMPRO_TAIL:
                self.__recv_buf+=bt.to_bytes(1,'little')
                self.__head_tail_buf.append(self.__recv_buf)
                self.__recv_buf=b''
                self.__has_head=False
            elif self.__has_head:
                if len(self.__recv_buf)>=SIMPRO_BUF_SIZE:#超出数据长度
                    self.__recv_buf=b''
                    self.__has_head=False
                else:
                    self.__recv_buf+=bt.to_bytes(1,'little')
            self.__last_recv_pad= bt==SIMPRO_PAD
    def SimPro_UnPacking(self):
        if len(self.__head_tail_buf)==0:
            return b""
        while(len(self.__head_tail_buf)>0):
            recv_buf_data = self.__head_tail_buf.pop(0)
            if len(recv_buf_data) < SIMPROTOL_SIZE:
                continue
            if recv_buf_data[0]!=SIMPRO_HEAD and recv_buf_data[-1]!=SIMPRO_TAIL:
                continue
            pack_id=recv_buf_data[1]
            pack_size=recv_buf_data[2]
            val_sum=recv_buf_data[3] #拿到校验和
            pack_ptr=recv_buf_data[4:-1] #拿到数据段
            if len(recv_buf_data) != pack_size+SIMPROTOL_SIZE: #长度不一致
                continue
            if val_sum != SimPro_CRC8(pack_id,pack_ptr): #检查校验
                continue
            self.recv_id=pack_id
            return pack_ptr
        return b''
table = [
        0x00, 0x07, 0x0e, 0x09, 0x1c, 0x1b, 0x12, 0x15,
        0x38, 0x3f, 0x36, 0x31, 0x24, 0x23, 0x2a, 0x2d,
        0x70, 0x77, 0x7e, 0x79, 0x6c, 0x6b, 0x62, 0x65,
        0x48, 0x4f, 0x46, 0x41, 0x54, 0x53, 0x5a, 0x5d,
        0xe0, 0xe7, 0xee, 0xe9, 0xfc, 0xfb, 0xf2, 0xf5,
        0xd8, 0xdf, 0xd6, 0xd1, 0xc4, 0xc3, 0xca, 0xcd,
        0x90, 0x97, 0x9e, 0x99, 0x8c, 0x8b, 0x82, 0x85,
        0xa8, 0xaf, 0xa6, 0xa1, 0xb4, 0xb3, 0xba, 0xbd,
        0xc7, 0xc0, 0xc9, 0xce, 0xdb, 0xdc, 0xd5, 0xd2,
        0xff, 0xf8, 0xf1, 0xf6, 0xe3, 0xe4, 0xed, 0xea,
        0xb7, 0xb0, 0xb9, 0xbe, 0xab, 0xac, 0xa5, 0xa2,
        0x8f, 0x88, 0x81, 0x86, 0x93, 0x94, 0x9d, 0x9a,
        0x27, 0x20, 0x29, 0x2e, 0x3b, 0x3c, 0x35, 0x32,
        0x1f, 0x18, 0x11, 0x16, 0x03, 0x04, 0x0d, 0x0a,
        0x57, 0x50, 0x59, 0x5e, 0x4b, 0x4c, 0x45, 0x42,
        0x6f, 0x68, 0x61, 0x66, 0x73, 0x74, 0x7d, 0x7a,
        0x89, 0x8e, 0x87, 0x80, 0x95, 0x92, 0x9b, 0x9c,
        0xb1, 0xb6, 0xbf, 0xb8, 0xad, 0xaa, 0xa3, 0xa4,
        0xf9, 0xfe, 0xf7, 0xf0, 0xe5, 0xe2, 0xeb, 0xec,
        0xc1, 0xc6, 0xcf, 0xc8, 0xdd, 0xda, 0xd3, 0xd4,
        0x69, 0x6e, 0x67, 0x60, 0x75, 0x72, 0x7b, 0x7c,
        0x51, 0x56, 0x5f, 0x58, 0x4d, 0x4a, 0x43, 0x44,
        0x19, 0x1e, 0x17, 0x10, 0x05, 0x02, 0x0b, 0x0c,
        0x21, 0x26, 0x2f, 0x28, 0x3d, 0x3a, 0x33, 0x34,
        0x4e, 0x49, 0x40, 0x47, 0x52, 0x55, 0x5c, 0x5b,
        0x76, 0x71, 0x78, 0x7f, 0x6a, 0x6d, 0x64, 0x63,
        0x3e, 0x39, 0x30, 0x37, 0x22, 0x25, 0x2c, 0x2b,
        0x06, 0x01, 0x08, 0x0f, 0x1a, 0x1d, 0x14, 0x13,
        0xae, 0xa9, 0xa0, 0xa7, 0xb2, 0xb5, 0xbc, 0xbb,
        0x96, 0x91, 0x98, 0x9f, 0x8a, 0x8d, 0x84, 0x83,
        0xde, 0xd9, 0xd0, 0xd7, 0xc2, 0xc5, 0xcc, 0xcb,
        0xe6, 0xe1, 0xe8, 0xef, 0xfa, 0xfd, 0xf4, 0xf3,
    ]
if __name__ == "__main__":
    a=b"0x11"
    # print(a+(0x55.to_bytes(1,'little')))
    # print(SimPro_CRC8(0,[0x11,0x22]))
    SIMPRO1=SimpleProtocol()
    p=SimTypes([
        [SimTypes.c_int8,'aa',1],
        [SimTypes.c_float,'a',4],
        (SimTypes.c_int8,'b',4),
        [SimTypes.c_uint32,'c'],
    ])
    p.aa=0xff
    p.a=[11111,11111,11111,11111]
    p.b=[0xff,0xab,0xab,0xee]
    p.c=0xabaacddd
    
    bytes_sed = SIMPRO1.SimPro_Packet(p.toBytes())
    SIMPRO1.SimPro_EnChar(bytes_sed)
    recv=SIMPRO1.SimPro_UnPacking()
    
    new_pr=SimTypes([
        [SimTypes.c_int8,'aa',1],
        [SimTypes.c_float,'a',4],
        (SimTypes.c_int8,'b',4),
        [SimTypes.c_uint32,'c'],
    ])
    print(recv)
    if len(recv)==len(new_pr):
        new_pr.fromBytes(recv)
        print("p pr 1  ",p.aa,new_pr.aa)
        print("p pr 2  ",p.a,new_pr.a)
        print("p pr 3  ",p.b,new_pr.b)
        print("p pr 4  ",p.c,new_pr.c)