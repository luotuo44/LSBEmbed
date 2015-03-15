//Filename: embedLSB.cpp
//Date: 2015-3-15

//Author: luotuo44   http://blog.csdn.net/luotuo44

//Copyright 2015, luotuo44. All rights reserved.
//Use of this source code is governed by a BSD-style license


#include"embedLSB.hpp"

#include<assert.h>
#include<string.h>


static inline char getOneBit(unsigned char n, int i)//取数n的第i位。从低位开始算起的第i位。
{
    return (n & (1<<i) ) >> i;
}


static inline char getTwoBit(unsigned char n, int i)
{
    return (n & (3<<i)) >> i;
}


int do_embedLSB1(const unsigned char *data, int len, unsigned char *img)
{
    int i, j;

    unsigned char *p = img;
    for(i = 0; i < len; ++i)
    {
        for(j = 7; j >= 0; --j, ++p)
        {
            char c = getOneBit(data[i], j);//0 或者 1
            *p &= 0xFE; //1111 1110

            *p += c;
        }
    }

    return p - img;
}

//返回，为了隐写len字节的字符，消耗了img多少字节
int embedLSB1(const unsigned char *data, int len, unsigned char *img)
{
    assert(sizeof(int) == 4);

    unsigned char str_len[4];
    memcpy(str_len, &len, 4);

    int use = do_embedLSB1(str_len, 4, img);

    use += do_embedLSB1(data, len, img + use);

    return use;
}


//返回，为了隐写len字节的字符，消耗了img多少字节
int do_unEmbedLSB1(const unsigned char* img, int len,//隐写的字节
                   std::vector<unsigned char> &vec)
{
    vec.reserve(len);

    const unsigned char *p = img;
    int i, j;

    for(i = 0; i < len; ++i)
    {
        char c = 0;
        for(j = 0; j < 8; ++j)
        {
            c <<= 1;
            c += getOneBit(*p++, 0);
        }

        vec.push_back(c);
    }

    return p - img;
}


//vec得到隐写的数据。返回值是为了隐写数据，消耗了img多少字节
int unEmbedLSB1(const unsigned char* img, std::vector<unsigned char> &vec)
{
    std::vector<unsigned char> temp;
    int use = do_unEmbedLSB1(img, 4, temp);

    int len;
    memcpy(&len, &temp[0], 4);

    use += do_unEmbedLSB1(img + use, len, vec);

    return use;
}



//返回，为了隐写len字节的字符，消耗了img多少字节
int do_embedLSB2(const unsigned char *data, int len , unsigned char *img)
{
    int i, j;

    unsigned char *p = img;
    for(i = 0; i < len; ++i)
    {
        for(j = 6; j >= 0; j -= 2, ++p)
        {
            char c = getTwoBit(data[i], j);

            *p &= 0xFC; //1111 1100
            *p += c;
        }
    }

    return p - img;
}



int embedLSB2(const unsigned char *data, int len , unsigned char *img)
{
    assert(sizeof(int) == 4);

    unsigned char str_len[4];
    memcpy(str_len, &len, 4);

    int use = do_embedLSB2(str_len, 4, img);

    use += do_embedLSB2(data, len, img + use);

    return use;
}



int do_unEmbedLSB2(const unsigned char *img, int len,
                   std::vector<unsigned char> &vec)
{
    vec.reserve(len);

    const unsigned char *p = img;
    int i, j;

    for(i = 0; i < len; ++i)
    {
        char c = 0;
        for(j = 0; j < 4; ++j)
        {
            c <<= 2;
            c += getTwoBit(*p++, 0);
        }

        vec.push_back(c);
    }

    return p - img;
}


int unEmbedLSB2(const unsigned char* img, std::vector<unsigned char> &vec)
{
    std::vector<unsigned char> temp;
    int use = do_unEmbedLSB2(img, 4, temp);

    int len;
    memcpy(&len, &temp[0], 4);

    use += do_unEmbedLSB2(img + use, len, vec);

    return use;
}




char getThreeBit(int n, int i)
{
    return (n & (7<<i)) >> i;
}


//返回，为了隐写len字节的字符，消耗了img多少字节
int do_embedLSB3(const unsigned char *data, int len , unsigned char *img)
{
    int j;
    unsigned char *p = img;
    const unsigned char *q = data;
    const unsigned char *end = data + len;

    //每次隐写data的3个字节到img中。
    while( q < end )
    {
        int temp = 0;
        for(j = 0; j < 3; ++j)//3个字节一组
        {
            temp <<= 8; //左移一个字节
            temp += *q++;
        }

        //现在temp里面有3字节的待隐写数据
        for(j = 21; j >= 0; j -= 3, ++p)
        {
            char c = getThreeBit(temp, j);
            *p &= 0xF8;//1111 1000
            *p += c;
        }
    }

    return p - img;
}


//因为每次隐写3字节到img，所以当len不是3的倍数时，会进行补全
//这就需要额外的img空间存放补全信息
int embedLSB3(const unsigned char *data, int len , unsigned char *img)
{
    assert(sizeof(int) == 4);

    unsigned char str_len[6] = {0};
    memcpy(str_len, &len, 4);

    int use = do_embedLSB3(str_len, 6, img);

    //因为每次隐写data的3个字节到img中。所以len得是3的倍数
    int remain = len % 3;
    use += do_embedLSB3(data, len - remain, img + use);

    if(remain != 0)
    {
        unsigned char temp[3] = {0};
        memcpy(temp, data + len - remain, remain);
        use += do_embedLSB3(temp, 3, img + use);
    }

    return use;
}


char getEightBit(int n, int i)
{
    return (n & (0xff<<i)) >> i;
}

//从img中，提取出len字节
int do_unEmbedLSB3(const unsigned char *img, int len,
                   std::vector<unsigned char> &vec)
{
    vec.reserve(len);

    const unsigned char *p = img;
    int i, j;

    for(i = 0; i < len; i += 3)
    {
        int temp = 0;
        //img的每个字节嵌入3bit.所以需要向img中的
        //8个元素提取，才能提取出3字节
        for(j = 0; j < 8; ++j)//3个字节一组
        {
            temp <<= 3;
            temp += (*p++) & 7;//0111。后3比特
        }

        //现在整型变量temp的低24位，是隐写的3个字节
        for(j = 2; j >= 0; --j)
        {
            vec.push_back(getEightBit(temp, 8*j));
        }
    }

    //因为在提取的时候，每次提取3字节。即使要提取的数量
    //len并不是3的倍数。所以，当len不是3的倍数时，会多
    //提取一些数据到vec中，此时需要resize。
    vec.resize(len);

    return p - img;
}


int unEmbedLSB3(const unsigned char* img, std::vector<unsigned char> &vec)
{
    std::vector<unsigned char> temp;
    int use = do_unEmbedLSB3(img, 6, temp);

    int len;
    memcpy(&len, &temp[0], 4);

    use += do_unEmbedLSB3(img + use, len, vec);

    return use;
}



int embedLSB(const unsigned char *data, int len ,
             unsigned char *img, int lsb)//lsb指明用多少位隐写
{
    assert(1 <= lsb && lsb < 4);
    assert(sizeof(int) == 4);

    unsigned char str_len[4];
    memcpy(str_len, &len, 4);
    int use = 0;

    if( lsb == 1)
    {
        use = do_embedLSB1(str_len, 4, img);
        use += do_embedLSB1(data, len, img + use);
    }
    else if(lsb == 2)
    {
        use = do_embedLSB2(str_len, 4, img);
        use += do_embedLSB2(data, len, img + use);
    }
    else if( lsb == 3)
    {
        use = embedLSB3(data, len, img);
    }

    return use;
}


int unEmbedLSB(const unsigned char *img, int lsb, //lsb指明用多少位隐写
               std::vector<unsigned char> &vec)
{
    assert(1 <= lsb && lsb < 4);

    std::vector<unsigned char> temp;
    int use, len;

    if( lsb == 1)
    {
        use = do_unEmbedLSB1(img, 4, temp);
        memcpy(&len, &temp[0], 4);

        use += do_unEmbedLSB1(img + use, len, vec);
    }
    else if( lsb == 2)
    {
        use = do_unEmbedLSB2(img, 4, temp);
        memcpy(&len, &temp[0], 4);

        use += do_unEmbedLSB2(img + use, len, vec);
    }
    else if( lsb == 3)
    {
        use = unEmbedLSB3(img, vec);
    }

    return use;
}



//嵌入len字节时，需要多少载体字节
int needBytes(int lsb, int len)
{
    assert(1 <= lsb && lsb < 4);

    int use = -1;

    if( lsb == 1)
        use = (4 + len)*8;//4是长度信息。一个比特对应1个载体字节
    else if( lsb == 2)
        use = (4 + len)*4;
    else if( lsb == 3)
    {
        len = 3*( (len + 2)/3 );//使得len是3的倍数
        //6为长度信息，本来只需4字节的长度信息，6是为了补全为3
        //的倍数。lsb为3的时候，8个载体字节能提供24个比特位
        //刚好能隐写3个字节
        use = 8*( (6 + len)/3 );
    }

    return use;
}
