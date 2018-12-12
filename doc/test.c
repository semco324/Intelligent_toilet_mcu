
#include <stdio.h>
#include <string.h>
/**
 * @brief ll_crc24_generate()
 *
 * @param[in] seed  
 * @param[in] pdata  
 * @param[in] len  
 *
 * @return 
 *
 * @example:
 * ADV: {0x00,0x0d,0x08,0xac,0xd9,0xc9,0x84,0xf0,0x02,0x01,0x02,0x03,0x09,0x68,0x79}
 * seed: 0x555555
 * crc24: 0x53f51b
 **/
unsigned int ll_crc24_generate(char *ppdata, char len)
{
    char i, j;
    unsigned int seed = 0x555555;
    for(i=0; i<len; ++i)
    {
        for(j=0; j<8; ++j)
        {
            char factor = ((ppdata[i] >> j) ^ (seed >> 23)) & 0x01;

            seed <<= 1;
            seed = (seed & ~(1<<10)) | ((seed ^ (factor<<10)) & (1<<10));
            seed = (seed & ~(1<<9 )) | ((seed ^ (factor<<9 )) & (1<<9 ));
            seed = (seed & ~(1<<6 )) | ((seed ^ (factor<<6 )) & (1<<6 ));
            seed = (seed & ~(1<<4 )) | ((seed ^ (factor<<4 )) & (1<<4 ));
            seed = (seed & ~(1<<3 )) | ((seed ^ (factor<<3 )) & (1<<3 ));
            seed = (seed & ~(1<<1 )) | ((seed ^ (factor<<1 )) & (1<<1 ));
            seed = (seed & ~(1<<0 )) | factor;
        }
    }
    return seed & 0x00FFFFFF;
}


typedef struct
{
	char WTH;
	char WTL;
	char HT;
	char SAGE;
	char MODE;
	char MOD;
}UserInfo_TypeDef;
UserInfo_TypeDef  UserInfo;

int main(void)
{
	char adv[]={0x00,0x0d,0x08,0xac,0xd9,0xc9,0x84,0xf0,0x02,0x01,0x02,0x03,0x09,0x68,0x79};
	printf("%#x\r\n",ll_crc24_generate(adv, 15));
	printf("%d\r\n",sizeof(UserInfo));
	return 0;
}

