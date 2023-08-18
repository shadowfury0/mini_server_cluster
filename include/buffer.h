#ifndef __L_BUFFER__
#define __L_BUFFER__

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <limits.h>

//灵感来源，单字符存储环形缓冲区
//注意多线程问题
typedef struct {
    uint8_t* buf;//刚好存一个字符
    uint32_t size;
    uint32_t read_pos;
    uint32_t write_pos;
} ringbuffer_t;

//是否为二的几次方
inline int is_power_of_two(uint32_t sz){
    if ( sz < 2) return 0;
    return sz & (sz-1) == 0;
}
//按照2的多少次方向上去整
inline uint32_t roundup_power_of_two(uint32_t sz){
    if (sz < 2) sz = 2;
    int i ;
    for (i = 0; sz != 0; i++)
        sz >>= 1;
    return 1u << i;//无符号左移
}

void rin_buf_init(ringbuffer_t* r,uint32_t sz){
    r->buf = (uint8_t*)malloc(sz * sizeof(uint8_t));
    if (!is_power_of_two(sz)) sz = roundup_power_of_two(sz);
    r->size = sz;
    //因为int占4位
    // r->read_pos = r->write_pos = UINT32_MAX - 2;//起始位置
    r->read_pos = r->write_pos = 0;//起始位置
}

bool rin_buf_isempty(ringbuffer_t * r){
    return r->read_pos == r->write_pos;
}

bool rin_buf_isfull(ringbuffer_t* r){
    return r->write_pos - r->read_pos == r->size;
}

void rin_buf_free(ringbuffer_t* r){
    if (r->buf != 0){
        free(r->buf);
        r->buf = 0;
    }
    r->read_pos = r->write_pos = r->size = 0;
}

int rin_buf_len(ringbuffer_t* r){
    return r->write_pos - r->read_pos;
}
//剩余空间
uint32_t rin_buf_remain(ringbuffer_t* r){
    return r->size - rin_buf_len(r);
}

inline uint32_t min(uint32_t l,uint32_t r) {
    return l > r ? r : l ;
}

uint32_t rin_buf_write(ringbuffer_t * r,void* buf,uint32_t sz){
    if ( sz > rin_buf_remain(r)){
        return 0;
    }

    uint32_t i = min(sz,r->size - (r->write_pos & (r->size - 1)) );
    memcpy(r->buf + (r->write_pos & (r->size - 1)),buf,i);
    memcpy(r->buf,buf + i,sz + i);

    r->write_pos += sz;
    return sz;
}

uint32_t rin_buf_read(ringbuffer_t* r,void* buf,uint32_t sz){
    if (rin_buf_isempty(r)){
        return 0;
    }
    sz = min(sz,rin_buf_len(r));
    uint32_t i = min(sz,r->size - (r->read_pos & (r->size - 1)));
    
    memcpy(buf,r->buf + (r->read_pos & (r->size - 1)),i);
    memcpy(buf + i,r->buf,sz - i);
    r->read_pos += sz;
    return sz;
}


#endif