#include "SimpleProtocol.h"

#define SIMPRO_HEAD 0xab
#define SIMPRO_TAIL 0xcd
#define SIMPRO_PAD 0xea //����flab�����������ݰ��ڳ���head��tail�ظ�,����ea���������һ���ַ���head��tail���
#define SIMPROTOL_SIZE 5 //Э�����С�����������ݣ�����С���͵����ݳ���,Э��̶�����,�����޸�

SimpleProtocol __SIMPROS[MAX_STATIC_SIMPRO];
static uint8_t __simpro_is_init=0;//���ڱ���Ƿ��ʼ�����

void SimPro_Init(SimpleProtocol *SIMPRO);
uint8_t SimPro_CheckPad(uint8_t c);
int SimPro_Packet(SimpleProtocol *SIMPRO,void *buffer,int buf_size);
int SimPro_GetBufIdx(int idx,int max_size);
void SimPro_EnChar(SimpleProtocol *SIMPRO,uint8_t Data);
int SimPro_UnPacking(SimpleProtocol *SIMPRO);
uint8_t SimPro_CRC8(uint8_t crc,void *buf,int buf_size); // 8λcrcУ��

//����ѭ�������±����ʱ���±����,���뷶Χֻ����Ϊ0~2*max_size-1
int SimPro_GetBufIdx(int idx,int max_size){
    if(idx>=max_size)return idx-max_size;
    return idx;
}


void SimPro_InitAll(){
    int i=0;
    for(i=0;i<MAX_STATIC_SIMPRO;++i){
        SimPro_Init(&(__SIMPROS[i]));
    }
    __simpro_is_init=1;
}

void SimPro_Init(SimpleProtocol *SIMPRO){

    SimpleProtocolContext *priv_data=NULL;
    if(SIMPRO==NULL)return;
    priv_data = &SIMPRO->priv_data;

    SIMPRO->pack_idx=0;
    priv_data->last_char=0;
    priv_data->start_unpacking=0;
    priv_data->count_reco_size=0;
    priv_data->head_idx=-1;
    priv_data->tail_idx=-1;
    priv_data->recv_start=0;
    priv_data->recv_end=0;
    
    SIMPRO->send_size=0;
    SIMPRO->data_size=0;
}
/*
���ط�����ȣ�0Ϊ�������
*/
uint8_t SimPro_CheckPad(uint8_t c){
    return  c == SIMPRO_HEAD ||
            c == SIMPRO_TAIL ||
            c == SIMPRO_PAD;
}
int SimPro_Packet(SimpleProtocol *SIMPRO,void *buffer,int buf_size){
    uint8_t *buf_ptr;
    uint8_t *send_buf_ptr;
    uint16_t packet_size=0;
    int i=0;
    uint8_t c;
    if(__simpro_is_init==0){
        SimPro_InitAll();
    }
    if(SIMPRO==NULL){
        return 0;
    }
    SIMPRO->send_size=0;
    if(buffer==NULL){
        return 0;
    }

    buf_ptr=(uint8_t *)buffer;
    send_buf_ptr=SIMPRO->send_buf;
    send_buf_ptr[packet_size++]=0;//ͷ����ӷ�SIMPRO_PAD�ַ��������֡�ܵ���һ֡�Ĵ���Ӱ��
    send_buf_ptr[packet_size++]=SIMPRO_HEAD;
    send_buf_ptr[packet_size++]=SIMPRO_PAD;
    send_buf_ptr[packet_size++]=SIMPRO->pack_idx;
    send_buf_ptr[packet_size++]=SIMPRO_PAD;
    send_buf_ptr[packet_size++]=buf_size;//�û����ݳ���
    send_buf_ptr[packet_size++]=SIMPRO_PAD;
    send_buf_ptr[packet_size++]=SimPro_CRC8(SIMPRO->pack_idx,buffer,buf_size);//�û����ݼ���������
    for(i=0;i<buf_size && packet_size<SIMPRO_BUF_SIZE-1 ;++i){//����û�����,������󳤶ȣ����������޷�����д
        c=buf_ptr[i];
        if(SimPro_CheckPad(c)){//����Ƿ���Ҫpad
            send_buf_ptr[packet_size++]=SIMPRO_PAD;
            if(packet_size>=SIMPRO_BUF_SIZE-1){
                break;
            }
        }
        send_buf_ptr[packet_size++]=c;
    }
    
    send_buf_ptr[packet_size++]=SIMPRO_TAIL;
    SIMPRO->send_size=packet_size;
    SIMPRO->pack_idx+=1;
    return packet_size;
} 

void SimPro_EnChar(SimpleProtocol *SIMPRO,uint8_t byte){
    SimpleProtocolContext *priv_data=NULL;
    if(SIMPRO==NULL)return;
    priv_data = &SIMPRO->priv_data;
    if(SimPro_GetBufIdx(priv_data->recv_end+1,SIMPRO_BUF_SIZE) == priv_data->recv_start){ //������
        if(priv_data->start_unpacking){//���ڽ����У���ֹ�����޸�����
            return;
        }else{
            //ɾ����ɵ�һ������
            priv_data->recv_start=SimPro_GetBufIdx(priv_data->recv_start+1,SIMPRO_BUF_SIZE);
        }
    }
    priv_data->recv_buf[priv_data->recv_end]=byte;
    priv_data->recv_end=SimPro_GetBufIdx(priv_data->recv_end+1,SIMPRO_BUF_SIZE);
}   
int SimPro_GetPack(SimpleProtocol *SIMPRO){//����ʶ��һ������֡
    //��һ������
    SimpleProtocolContext *priv_data=NULL;
    uint8_t last_char=0;
    uint8_t byte=0;
    int i;
    uint8_t *reco_buf_ptr;
    int reco_index=0;
    if(SIMPRO==NULL)return 0;
    priv_data = &SIMPRO->priv_data;
    priv_data->start_unpacking=1;
    reco_index=priv_data->count_reco_size;
    reco_buf_ptr=priv_data->reco_buf;
    last_char=priv_data->last_char;
    for(i=priv_data->recv_start;i!=priv_data->recv_end;i=SimPro_GetBufIdx(i+1,SIMPRO_BUF_SIZE)){
        byte=priv_data->recv_buf[i];
        if(last_char==SIMPRO_PAD && priv_data->head_idx!=-1){
            reco_buf_ptr[reco_index++]=byte;
            last_char=byte;
            if(byte==SIMPRO_PAD){//��һ��Ҳ��PAD�����PADΪ��Ч��
                last_char=0;//����Ϊһ����PAD�ַ�����Ӱ����һ���ַ�
            }
        }else{
            if(byte!=SIMPRO_PAD){//�����յ��Ĳ�Ϊpad�ַ�
                if(byte==SIMPRO_HEAD){
                    // if(head_idx!=-1){//˵�������ظ���֡ͷ
                    priv_data->head_idx=i;
                    priv_data->recv_start=priv_data->head_idx; //�ƶ�ͷ
                    reco_index=0; //ʶ��ͷ�����
                    reco_buf_ptr[reco_index++]=byte;
                }else if(byte==SIMPRO_TAIL && priv_data->head_idx!=-1){
                    priv_data->tail_idx=i;
                    reco_buf_ptr[reco_index++]=byte;
                    break;
                }else if(priv_data->head_idx!=-1){
                    reco_buf_ptr[reco_index++]=byte;
                }
            }
            last_char=byte;
        }
        if(priv_data->head_idx==-1){//δʶ��ͷ��ֱ������������
            priv_data->recv_start=i;
        }
        if(reco_index>=SIMPRO_BUF_SIZE){
            if(priv_data->head_idx!=-1){
                priv_data->tail_idx=i;//ǿ���޸�β������ǰ��������
            }
            break;
        }
    }
    priv_data->last_char=last_char;
    priv_data->count_reco_size=reco_index;
    if(priv_data->head_idx==-1){//δʶ��ͷ��ֱ����������
        priv_data->recv_start=i;
    }else if(priv_data->tail_idx==-1){
        priv_data->recv_start=priv_data->head_idx;//ʶ��ͷ���ƶ����ݵ�ͷ�±�
    }else{ 
        priv_data->recv_start=SimPro_GetBufIdx(priv_data->tail_idx+1,SIMPRO_BUF_SIZE);//ʶ��ͷβ���ƶ����ݵ�β�±�+1
        //���ͷβ����
        priv_data->head_idx=-1;
        priv_data->tail_idx=-1;
        priv_data->count_reco_size=0;
        priv_data->start_unpacking=0;
        return reco_index;
    }
    priv_data->tail_idx=-1;
    priv_data->start_unpacking=0;
    return 0; //�������ݳ���
}
int SimPro_UnPacking(SimpleProtocol *SIMPRO){//���Խ��,ֻ����һ֡����
    uint8_t *reco_buf_ptr;
    uint8_t *pack_ptr;
    uint8_t val_sum=0;
    int pack_size=0;
    int pack_id;
    int reco_size=0;
    SimpleProtocolContext *priv_data=NULL;
    if(SIMPRO==NULL)return 0;
    priv_data=&SIMPRO->priv_data;
    if(__simpro_is_init==0){
        SimPro_InitAll();
    }
    SIMPRO->data_size=0;
    while (priv_data->recv_start!=priv_data->recv_end){
        // reco_size=SimPro_GetPack(SIMPRO);
        pack_size=0;
        reco_size=SimPro_GetPack(SIMPRO);
        // reco_size=priv_data->reco_size;
        if(reco_size<=0)break;
        reco_buf_ptr=priv_data->reco_buf;
        
        if(reco_size<=SIMPROTOL_SIZE){//����8���ֽ�,����������Ч
            continue;
        }
        if(reco_buf_ptr[0]!=SIMPRO_HEAD && reco_buf_ptr[reco_size-1]!=SIMPRO_TAIL){
            continue;
        }
        pack_id=reco_buf_ptr[1];
        pack_size=reco_buf_ptr[2];
        val_sum=reco_buf_ptr[3];//�õ�У���
        pack_ptr=&reco_buf_ptr[4];//�õ�����ָ��
        if(reco_size != pack_size+SIMPROTOL_SIZE){//���Ȳ�һ��
            continue;
        }
        if(val_sum != SimPro_CRC8(pack_id,pack_ptr,pack_size)){//���У��
            continue;
        }
        memcpy(SIMPRO->data_buf,pack_ptr,pack_size);//��������������������,�����û�ʹ��
        SIMPRO->data_pack_idx=pack_id;
        SIMPRO->data_size=pack_size;
        break;
    }
    return pack_size;
}

uint8_t SimPro_CRC8(uint8_t crc,void *buf,int buf_size){
    static const uint8_t table[] = {
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
    };
    const uint8_t *p = (const uint8_t*)buf;
    int i;
    for (i = 0; i < buf_size; ++i) {
        crc = table[crc ^ p[i]];
    }
    return crc;
}
