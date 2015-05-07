#ifndef LSB_HPP
#define LSB_HPP


#include<vector>


namespace LSB
{


inline void embedInChar(unsigned char &src, int data, int len)
{
    int mask = (1 << len) - 1;
    src &= ~mask;
    data &= mask;
    src |= data;
}


inline unsigned char unEmbedFromChar(unsigned char src, int len)
{
    return src & ( (1<<len) -1);
}


inline unsigned char getNBit(unsigned char src,
                             int i, int n)
{
    int geter = (1 << n) - 1;
    return (src & (geter<<i) ) >> i;
}


//返回，为了隐写len字节的字符，消耗了img多少字节
int embed(const unsigned char *data, int len,
          char *img, int lsb);//lsb指明用多少位隐写


int unEmbed(const char* img, int lsb,//lsb指明用多少位隐写
            std::vector<unsigned char> &vec);



//vec得到隐写的数据。返回值是为了隐写数据，消耗了img多少字节
int unEmbedFromVec(const char* img, int lsb,//lsb指明用多少位隐写
                   std::vector<unsigned char> &vec);

//嵌入len字节时，需要多少载体字节.lsb参数指明是每个像素嵌入多少比特
//目前只支持嵌入1, 2, 3比特
int needBytes(int lsb, int len);




void layerEmbed(const unsigned char *data,int len,
                char *img, int img_size);

inline void layerEmbedInVec(const std::vector<unsigned char> &data,
                            char *img, int img_size)
{
    layerEmbed(&data[0], data.size(), img, img_size);
}

void layerUnEmbed(const char *img, int img_size,
                  std::vector<unsigned char> &vec);


}

#endif // LSB_HPP

