/*
 * firmware_ringbuffer.c
 * Firmware for SeeedStudio Mesh Bee(Zigbee) module
 *
 * Copyright (c) NXP B.V. 2012.
 * Spread by SeeedStudio
 * Author     : Jack Shao
 * Create Time: 2013/10
 * Change Log :
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "firmware_ringbuffer.h"

int init_ringbuffer(struct ringbuffer *r, void *buff, uint32 size)
{
    struct ringbuffer *ringBuf = r;
    if(ringBuf){
    r->buf = buff;
    r->size = size;
    clear_ringbuffer(r);
    return 0;
    }
    else{
         DBG_vPrintf(TRACE_SLEEP, "Failed: init_ringbuffer\r\n");
    r->buf = NULL;
    r->size = 0;
    clear_ringbuffer(r);
     return 0;
}
}
void free_ringbuffer(struct ringbuffer *r)
{
    //free(r->buf);
}

void clear_ringbuffer(struct ringbuffer *r)
{
    struct ringbuffer *ringBuf = r;
    if(ringBuf){
    r->begin = 0;
    r->end = 0;
    }
    else{
        DBG_vPrintf(TRACE_SLEEP, "Failed: clear_ringbuffer\r\n");
    }
return;
}
uint32 ringbuffer_free_space(struct ringbuffer *r)
{
    struct ringbuffer *ringBuf = r;
    if(ringBuf){
    if (r->begin == 0 && r->end == 0) 
    return r->size;
    if (r->begin <= r->end) 
    return r->size - (r->end - r->begin) - 1;
    else return r->begin - r->end - 1;
}
DBG_vPrintf(TRACE_SLEEP, "Failed: ringbuffer_free_space\r\n");
return 0;
}
uint32 ringbuffer_data_size(struct ringbuffer *r)
{
    struct ringbuffer *ringBuf = r;
    if(ringBuf){
    if (r->begin == 0 && r->end == 0) return 0;

    if (r->begin <= r->end) return r->end - r->begin + 1;
    else return r->size - (r->begin - r->end) + 1;
}
DBG_vPrintf(TRACE_SLEEP, "Failed: ringbuffer_data_size\r\n");
return 0;
}

//push data into buffer : in stack
void ringbuffer_push(struct ringbuffer *r, const void *data, uint32 size)
{
    struct ringbuffer *ringBuf = r;
    void *BufData = data;
    uint32 s = r->buf + r->size - r->end;
    if((ringBuf) && (BufData)){
    if (ringbuffer_free_space(r) < size) return;

    if (r->begin == 0 && r->end == 0)
    {
        memcpy(r->buf, data, size);
        r->begin = r->buf;
        r->end = r->buf + size - 1;
        return;
    }

    r->end++;
    if (r->begin < r->end)
    {
        if ((uint32)(r->buf + r->size - r->end/*begin*/) >= size)
        {
            /* we can fit without cut */
            memcpy(r->end, data, size);
            r->end += size - 1;
        } else
        {
            /* make a cut */
            
            memcpy(r->end, data, s);
            size -= s;
            memcpy(r->buf, (char *)data + s, size);
            r->end = r->buf + size - 1;
        }
    } else
    {
        memcpy(r->end, data, size);
        r->end += size - 1;
    }
}
  DBG_vPrintf(TRACE_SLEEP, "Failed: ringbuffer_push\r\n"); 
  return;
}
//get buffer data : out stack
void ringbuffer_pop(struct ringbuffer *r, void *data, uint32 size)
{
    int need_clear = 0;
    uint32 s = r->buf + r->size - r->begin;
    struct ringbuffer *ringBuf = r;
    void *BufData = data;
    if((ringBuf) && (BufData)){
    
    if (ringbuffer_data_size(r) < size) return;
    if (ringbuffer_data_size(r) == size) need_clear = 1;

    if (r->begin < r->end)
    {
        if (data) memcpy(data, r->begin, size);
        r->begin += size;
    } else
    {
        if ((uint32)(r->buf + r->size - r->begin) >= size)
        {
            if (data) memcpy(data, r->begin, size);
            r->begin += size;
        } else
        {
            if (data) memcpy(data, r->begin, s);
            size -= s;
            if (data) memcpy((char *)data + s, r->buf, size);
            r->begin = r->buf + size;
        }
    }

    if (need_clear) clear_ringbuffer(r);
}
DBG_vPrintf(TRACE_SLEEP, "Failed: ringbuffer_pop\r\n"); 
return;
}
/* read without pop out */
void ringbuffer_read(struct ringbuffer *r, void *data, uint32 size)
{
    struct ringbuffer *ringBuf = r;
    void *BufData = data;
    uint32 s = r->buf + r->size - r->begin;
    if((ringBuf) && (BufData)){
        
    if (ringbuffer_data_size(r) < size) return;

    if (r->begin < r->end) memcpy(data, r->begin, size);
    else
    {
        if ((uint32)(r->buf + r->size - r->begin) >= size) memcpy(data, r->begin, size);
        else
        {
            
            memcpy(data, r->begin, s);
            size -= s;
            memcpy((char *)data + s, r->buf, size);
        }
    }
}
DBG_vPrintf(TRACE_SLEEP, "Failed: ringbuffer_read\r\n"); 
return;
}
