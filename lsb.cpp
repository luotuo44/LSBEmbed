#include"lsb.hpp"

#include<assert.h>
#include<string.h>


namespace  LSB
{

inline char getOneBit(char n, int i)//取数n的第i位。从低位开始算起的第i位。
{
    return (n & (1<<i) ) >> i;
}


inline char getTwoBit(char n, int i)
{
    return (n & (3<<i)) >> i;
}


int do_embedOneBit(const unsigned char *data, int len, char *img)
{
    int i, j;

    char *p = img;
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


int embedOneBitHeader(int len, char *img)
{
    assert(sizeof(int) == 4);

    unsigned char str_len[4];
    memcpy(str_len, &len, 4);

    return do_embedOneBit(str_len, 4, img);
}

int embedOneBitBody(const unsigned char *data,
                    int len, char *img, int use)
{
    return do_embedOneBit(data, len, img + use);
}

//返回，为了隐写len字节的字符，消耗了img多少字节
int embedOneBit(const unsigned char *data, int len, char *img)
{
    int use = embedOneBitHeader(len, img);
    use += embedOneBitBody(data, len, img, use);

    return use;
}


//返回，为了隐写len字节的字符，消耗了img多少字节
int do_unEmbedOneBit(const char* img, int len,//隐写的字节
                     std::vector<unsigned char> &vec)
{
    vec.reserve(len);

    const char *p = img;
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


//len is how many data embed in img
int unEmbedOneBitHeader(const char *img, int *len)
{
    std::vector<unsigned char> temp;
    int use = do_unEmbedOneBit(img, 4, temp);

    assert(len != NULL);
    memcpy(len, &temp[0], 4);

    return use;
}

int unEmbedOneBitBody(const char *img,
                      int use, int len,
                      std::vector<unsigned char> &vec)
{
    return do_unEmbedOneBit(img+use, len, vec);
}


//vec得到隐写的数据。返回值是为了隐写数据，消耗了img多少字节
int unEmbedOneBit(const char* img, std::vector<unsigned char> &vec)
{
    int len; //len is how many data embed in img
    int use = unEmbedOneBitHeader(img, &len);

    use += unEmbedOneBitBody(img, use, len, vec);

    return use;
}

//#######################################




//返回，为了隐写len字节的字符，消耗了img多少字节
int do_embedTwoBit(const unsigned char *data, int len , char *img)
{
    int i, j;

    char *p = img;
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


//len is how many data embed in img
int embedTwoBitHeader(int len, char *img)
{
    assert(sizeof(int) == 4);

    unsigned char str_len[4];
    memcpy(str_len, &len, 4);

    return do_embedTwoBit(str_len, 4, img);
}

int embedTwoBitBody(const unsigned char *data,
                    int len, char *img, int use)
{
    return do_embedTwoBit(data, len, img + use);
}

//返回，为了隐写len字节的字符，消耗了img多少字节
int embedTwoBit(const unsigned char *data, int len, char *img)
{
    int use = embedTwoBitHeader(len, img);
    use += embedTwoBitBody(data, len, img, use);

    return use;
}




int do_unEmbedTwoBit(const char *img, int len,
                   std::vector<unsigned char> &vec)
{
    vec.reserve(len);

    const char *p = img;
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



//len is how many data embed in img
int unEmbedTwoBitHeader(const char *img, int *len)
{
    std::vector<unsigned char> temp;
    int use = do_unEmbedTwoBit(img, 4, temp);

    assert(len != NULL);
    memcpy(len, &temp[0], 4);

    return use;
}

int unEmbedTwoBitBody(const char *img,
                      int use, int len,
                      std::vector<unsigned char> &vec)
{
    return do_unEmbedTwoBit(img+use, len, vec);
}


//vec得到隐写的数据。返回值是为了隐写数据，消耗了img多少字节
int unEmbedTwoBit(const char* img, std::vector<unsigned char> &vec)
{
    int len; //len is how many data embed in img
    int use = unEmbedTwoBitHeader(img, &len);

    use += unEmbedTwoBitBody(img, use, len, vec);

    return use;
}

//#####################################


inline char getThreeBit(int n, int i)
{
    return (n & (7<<i)) >> i;
}


//返回，为了隐写len字节的字符，消耗了img多少字节
int do_embedThreeBit(const unsigned char *data, int len , char *img)
{
    int j;
    char *p = img;
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


int embedThreeBitHeader(int len, char *img)
{
    assert(sizeof(int) == 4);

    unsigned char str_len[6] = {0};
    memcpy(str_len, &len, 4);

    return do_embedThreeBit(str_len, 6, img);
}


//因为每次隐写3字节到img，所以当len不是3的倍数时，会进行补全
//这就需要额外的img空间存放补全信息
int embedThreeBitBody(const unsigned char *data,
                      int len, int use, char *img)
{
    //因为每次隐写data的3个字节到img中。所以len得是3的倍数
    int remain = len % 3;
    use = do_embedThreeBit(data, len - remain, img + use);

    if(remain != 0)
    {
        unsigned char temp[3] = {0};
        memcpy(temp, data + len - remain, remain);
        use += do_embedThreeBit(temp, 3, img + use);
    }

    return use;
}



int embedThreeBit(const unsigned char *data, int len , char *img)
{
    int use = embedThreeBitHeader(len, img);

    use += embedThreeBitBody(data, len, use, img);

    return use;
}


inline char getEightBit(int n, int i)
{
    return (n & (0xff<<i)) >> i;
}

//从img中，提取出len字节
int do_unEmbedThreeBit(const char *img, int len,
                       std::vector<unsigned char> &vec)
{
    vec.reserve(len);

    const char *p = img;
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


int unEmbedThreeBitHeader(const char *img, int *len)
{
    std::vector<unsigned char> temp;
    int use = do_unEmbedThreeBit(img, 6, temp);

    memcpy(len, &temp[0], 4);

    return use;
}

int unEmbedThreeBitBody(const char *img, int len, int use,
                        std::vector<unsigned char> &vec)
{
    return do_unEmbedThreeBit(img+use, len, vec);
}


int unEmbedThreeBit(const char* img, std::vector<unsigned char> &vec)
{
    int len;
    int use = unEmbedThreeBitHeader(img, &len);

    use += unEmbedThreeBitBody(img, len, use, vec);

    return use;
}





//############################################

int embed(const unsigned char *data, int len,
          char *img, int lsb)//lsb指明用多少位隐写
{
    assert(data != NULL && img != NULL);

    int use = 0;
    switch(lsb)
    {
    case 1 :
        use = embedOneBit(data, len, img);
        break;

    case 2 :
        use = embedTwoBit(data, len, img);
        break;

    case 3 :
        use = embedThreeBit(data, len, img);
        break;

    default:
        use = -1;
        break;
    }

    return use;
}



int unEmbed(const char* img, int lsb,//lsb指明用多少位隐写
            std::vector<unsigned char> &vec)
{
    assert(img != NULL);

    int use = 0;
    switch(lsb)
    {
    case 1 :
        use = unEmbedOneBit(img, vec);
        break;

    case 2 :
        use = unEmbedTwoBit(img, vec);
        break;

    case 3 :
        use = unEmbedThreeBit(img, vec);
        break;

    default:
        use = -1;
        break;
    }

    return use;
}






//嵌入len字节时，需要多少载体字节
int needBytes(int lsb, int len)
{
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


//提取data的第i比特位,其他比特位设为0
inline char getBit(char data, int i)
{
    return data & (1<<i);
}



//将字符流转换成比特流
std::vector<bool>
    charToBitStream(const unsigned char *data,int len)
{

    std::vector<bool> bitstream;
    bitstream.reserve(8*len);
    for(int i = 0; i < len; ++i)
    {
        for(int j = 7; j >= 0; --j)
        {
            char c = getOneBit(data[i], j);//0 或者 1
            bitstream.push_back((bool)c);
        }
    }

    return bitstream;
}


void layerEmbedBitStream(const std::vector<bool> &bitstream,
                         char *img, char* curr, int img_size)
{
    int bit = 0;//从最低比特位开始嵌入
    char *s = curr;
    char *e = img + img_size;
    auto it = bitstream.begin();

    assert(s < e);
    while(it != bitstream.end())
    {
        for( ; s != e  && it != bitstream.end(); ++s)
        {
            //~(1<<bit)得到1111 1110或者1111 1101这样的数
            //(*s & ~(1<<bit))就是把*s对应的第bit位,置0
            *s = (*s & ~(1<<bit)) | (*it++<<bit);
        }

        //跳出内循环后，说明已经嵌完图像一层比特位
        s = img;
        ++bit;
    }
}


void do_layerEmbed(const unsigned char *data, int len,
                   char *img, char* curr, int img_size)
{
    //先将字符流转换成比特流
    std::vector<bool> bitstream = charToBitStream(data, len);

    //将比特流嵌入到图像中
    layerEmbedBitStream(bitstream, img, curr, img_size);
}



int layerEmbedHeader(int len, char *img, int img_size)
{
    assert(sizeof(int) == 4);

    unsigned char str_len[4];
    memcpy(str_len, &len, 4);

    do_layerEmbed(str_len, 4, img, img, img_size);

    //32是指嵌入长度信息str_len需要32个比特
    return 32;
}


void layerEmbedBody(const unsigned char *data, int len,
                    char *img, int img_size, int use)
{
    do_layerEmbed(data, len, img, img+use, img_size);
}


void layerEmbed(const unsigned char *data, int len,
                char *img, int img_size)
{
    assert(data != NULL && img != NULL);

    int use = layerEmbedHeader(len, img, img_size);

    layerEmbedBody(data, len, img, img_size, use);
}




void bitStreamToChar(const std::vector<bool> &bitstream,
                     std::vector<unsigned char> &vec)
{
    assert(bitstream.size()%8 == 0);

    vec.clear();
    vec.reserve(bitstream.size()/8);

    auto it = bitstream.begin();
    while( it != bitstream.end() )
    {
        unsigned char c = 0;
        for(int j = 7; j >= 0; --j)
        {
            c <<= 1;
            c += *it++;
        }

        vec.push_back(c);
    }
}


std::vector<bool>
    layerUnEmbedBitStream(const char *img, const char *curr,
                          int img_size, int len)//len是要提取的字节数
{
    int total = 8 * len;
    std::vector<bool> bitstream;
    bitstream.reserve(total);

    const char *s = curr;
    const char *e = img + img_size;
    int bit = 0;

    //先提取比特流，然后再将之转换成字符流
    int i = 0;
    while(i < total)
    {
        while( (s!=e) && (i++<total) )
        {
            bitstream.push_back((bool)getOneBit(*s++, bit));
        }

        s = img;
        ++bit;
    }

    return bitstream;
}


void do_layerUnEmbed(const char *img, const char *curr,
                     int img_size,
                     int len,//要提取的字节数
                     std::vector<unsigned char> &vec)
{
    //先从图像中提取出比特流
    std::vector<bool> bitstream =
            layerUnEmbedBitStream(img, curr, img_size, len);


    //将比特流转化成字节流
    bitStreamToChar(bitstream, vec);
}



int layerUnEmbedHeader(const char *img, int img_size,
                       int *len)
{
    assert(img != NULL  && len != NULL);

    std::vector<unsigned char> temp;
    do_layerUnEmbed(img, img, img_size, 4, temp);

    memcpy(len, &temp[0], 4);

    return 32;//4字节一共是32比特
}


void layerUnEmbedBody(const char *img, int use, int img_size,
                      int len, std::vector<unsigned char> &vec)
{
    do_layerUnEmbed(img, img+use, img_size, len, vec);
}


void layerUnEmbed(const char *img, int img_size,
                  std::vector<unsigned char> &vec)
{
    assert(img != NULL);

    int len;
    int use = layerUnEmbedHeader(img, img_size, &len);

    layerUnEmbedBody(img, use, img_size, len, vec);
}


char embedLSB(char src, int data, int len)
{
    assert(0<=len && len<8);

    int mask = (1 << len) - 1;
    src &= ~mask;
    data &= mask;
    src |= data;
    return src;
}


char getNBit(char src, int i, int n)
{
    int geter = (1 << n) - 1;
    return (src & (geter<<i) ) >> i;
}




}

