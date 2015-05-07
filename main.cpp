#include <iostream>
#include<string.h>
#include<algorithm>
#include"lsb.hpp"

using namespace std;


int testLSB()
{
    char img[1024];

    char data[100];
    int lsb = 2;

    while( 1 )
    {
        cin>>data;
        cin>>lsb;

        lsb = lsb < 1 ? 1 : lsb;
        lsb = lsb > 4 ? 4 : lsb;

        cout<<"sizeof "<<strlen(data)<<endl;
        cout<<"need "<<LSB::needBytes(lsb, strlen(data))<<" bytes"<<endl;

        if(lsb < 4)
        {
            int use = LSB::embed((unsigned char*)data, strlen(data), img, lsb);
            cout<<"use "<<use<<" bytes to hide"<<endl;
        }
        else
            LSB::layerEmbed((unsigned char*)data, strlen(data), img, sizeof(img));


        vector<unsigned char> vec;
        if( lsb < 4)
            LSB::unEmbed(img, lsb, vec);
        else
            LSB::layerUnEmbed(img, sizeof(img), vec);

        for(size_t i = 0; i < vec.size(); ++i)
            cout<<vec[i];
        cout<<endl;


        if( std::equal(vec.begin(), vec.end(), data) )
            cout<<"equal"<<endl;
        else
            cout<<"!!!!!equal"<<endl;

    }

    return 0;
}



int main()
{
    testLSB();
    return 0;
}

