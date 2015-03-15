//Filename: embedLSB.hpp
//Date: 2015-3-15

//Author: luotuo44   http://blog.csdn.net/luotuo44

//Copyright 2015, luotuo44. All rights reserved.
//Use of this source code is governed by a BSD-style license

#ifndef EMBEDLSB_HPP
#define EMBEDLSB_HPP

#include<vector>


//返回，为了隐写len字节的字符，消耗了img多少字节
int embedLSB(const unsigned char *data, int len ,
             unsigned char *img, int lsb);//lsb指明用多少位隐写

inline int embedLSB(const std::vector<unsigned char> &data,
                    unsigned char *img, int lsb)
{
    return embedLSB(&data[0], data.size(), img, lsb);
}

//vec得到隐写的数据。返回值是为了隐写数据，消耗了img多少字节
int unEmbedLSB(const unsigned char* img, int lsb,//lsb指明用多少位隐写
               std::vector<unsigned char> &vec);

//嵌入len字节时，需要多少载体字节.lsb参数指明是每个像素嵌入多少比特
//目前只支持嵌入1, 2, 3比特
int needBytes(int lsb, int len);

#endif // EMBEDLSB_HPP
