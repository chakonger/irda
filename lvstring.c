#include <stdio.h>
#include <string.h>


/*
  to convert the irda fixed code and param data to eLevel data
  irdaservice@leanvision.cn
  steven.lian@gmail.com 2016/8/10
*/

/* 
How to complile in linux for python:
	gcc -c -fPIC lvstring.c     
	gcc -shared -fPIC -o lvstring.so lvstring.o
How to use in python:
	from ctypes import *
	import os
	cLib=cdll.LoadLibrary(os.getcwd()+'/lvstring.so')
	s2=create_string_buffer(CONST_MAXSTRBUFFLEN)
	cLib.param2eLevel(fixedCode,param,s2)
	str2=s2.value
*/


/* 
www.irdaservice.cn

获取固定码和变化码的命令 IKA0
请求：
例1:
   {"CMD":"IKA0","TOKEN":"4FF32895E665D89722C5","typeID":"100001", "inst":"100800"}
解释：
CMD：命令类型
typeID:红外类型代码
返回:
成功：IKB0
例1: {"CMD":"IKB0","fixParam":"260104b80bb80bb80b941102260226020226027c06022602204e00",
"param":[{"delay":"0","actionID":"6","param":"017000653100000205000000010000A0E90000"}]}

解释：
IKAO的接口主要适用于配合我们的软件接口使用，调用此接口可以直接输出固定码和相应的命令参数。 

*/

/* 
int param2eLevel(char *fixedCode,char *param, char *strOut);
把IKA0返回的参数转换成电平长度字符串,其中
下面的C参数和IKA0返回信息之间的关系：
C代码 = IKA0返回参数
fixedCode = fixParam
param = param

strOut=调用函数预先分配的内存
返回值：
*/

/* 
decodeElevel(char *strParam, int *pn);
把param2eLevel返回的strOut格式转换成整形数组
strParam = param2eLevel 的 strOut
pn = 调用函数预先分配的内存
返回参数：
>0 数组有效数据个数
<0 错误
*/

/* demo code 
调试打印函数 
int printParam();

char demoFixedCode[]="2601022a0d2a0d0249034903024903db0902490370940449034c95ac0dac0d00";
char demoParam[]="02160060FC1C0000";


char demoRightResult[]="0160002a0d2a0d49034903490349034903490349034903490349034903db094903db094903490349034903490349034903db094903db094903db094903db094903db094903db0949034903490349034903db094903db094903db0949034903490370942a0d2a0d49034903490349034903490349034903490349034903db094903db094903490349034903490349034903db094903db094903db094903db094903db094903db0949034903490349034903db094903db094903db094903490349037094";
char demoBuff[CONST_MAXSTRBUFFLEN+1];
int  demoELevelBuff[CONST_MAXELEVELLEN+1];

int main()
{
    int rtn;
    int nT1,nT2,i;
    printf("\n--------------DEMO PROGRAM FOR IRDA DECODING-----------\n");
    printf("\n-------------- by LEANVISION -----------------\n");
    printf("\n-------------- www.irdaservice.com -----------------\n");
    printf ("\n** DEMO BEGIN**\n");
    printf ("\nfixedCode:%s",demoFixedCode);
    printf ("\nparam:%s",demoParam);
    printf ("\nresult:\n%s",demoRightResult);

    rtn=param2eLevel(demoFixedCode,demoParam, demoBuff);
    nT1=strcmp(demoBuff,demoRightResult);
    if (nT1==0)
        printf("\nCMP RESULT OK\n"); 
    else
        printf("\nCMP RESULT ERROR\n"); 
    printf("\ndemoBuff:\n%s\n",demoBuff);
    nT2=decodeElevel(demoBuff,demoELevelBuff);
    printf("\neLevel %d\n",nT2); 
    for (i=0;i<nT2;i++)
    {
        printf("%6d",demoELevelBuff[i]);
    } 
    printParam();
        
}

*/

#define CONST_MAXELEVELLEN 1000
#define CONST_MAXSTRBUFFLEN 4000
#define CONST_MAXSYNCCOUNT 32
#define CONST_MAXPARAMLEN  100
#define CONST_MAXDATACOUNT 2
#define CONST_MAXSTOPLEN 100

typedef struct
{
    int nLen;
    char achParam[CONST_MAXPARAMLEN];
    char achSign[2];    
} ST_SUBFIXED;

typedef struct 
{
    ST_SUBFIXED sHead;
    int nSyncCount;
    ST_SUBFIXED asSync[CONST_MAXSYNCCOUNT];
    ST_SUBFIXED sStop;
    int nDataCount;
    ST_SUBFIXED asData[CONST_MAXDATACOUNT];
} ST_FIXED;

int nLoopCount=1;

//unsigned char gachSendBuff[CONST_MAXSTRBUFFLENi+1];
/* for testing purpose only, please remove it in future */
//int ganSendBuff[CONST_MAXELEVELLEN+1];

ST_FIXED gsFixed;

//head
int int2hex(long nValue,int nZeroFill, int nLen,char *pBuff);
char ch_int2hex(int n);
int hex2int(char *hex);
int l4_hex2int(char *lhex);
char chHighLowSwitch(char chSwitch);

int decodeFixedCode(char *fixedCode);
int decodeFixedSub(char *strIn,ST_SUBFIXED *psSub);
int decodeParam(char *strIn,char *strOut);
int decodeParamSub(char *strOut,int nPos,char *pchLastSign,ST_SUBFIXED *pS);
int decodeElevel(char *strParam, int *pn);

int printParam();

int param2eLevel(char *fixedCode,char *param, char *strOut)
{
    int rtn=0;
    int i,j=0,k=0,m,nLen;
    int nT1;
    char achTBuff[16];
    //printf("\n test Begin:\n");
    decodeFixedCode(fixedCode);
    //printParam();
    //decodeParam(param,gachSendBuff);
    decodeParam(param,strOut);
/*
    k = decodeElevel(strOut,ganSendBuff);
    printf("\n test int array result:\n %d \n",k);
    for (i=0;i<k;i++)
    {
        printf(" %d",ganSendBuff[i]);
    }  
    printf("\n test end:\n");
*/
    return rtn;
}


int decodeFixedSub(char *strIn,ST_SUBFIXED *psSub)
{
    int rtn=0;
    int i,m,k;
    char eLevelSign='H';
    char strTBuff[10];
    memcpy(strTBuff,strIn,2);
    strTBuff[2]='\0';    
    m=hex2int(strTBuff);
    if (m>128)
    { 
        eLevelSign='L';
        m-=128;
    }
    if (m>0)
    {
        psSub->nLen=m;
        k=0;
        psSub->achSign[0]=eLevelSign;
        for (i=0;i<m;i++)
        {
            psSub->achParam[k]=eLevelSign;
            //k++;
            eLevelSign=chHighLowSwitch(eLevelSign);
            memcpy(&psSub->achParam[k],&strIn[2+i*4],4);
            k+=4;
        }
        eLevelSign=chHighLowSwitch(eLevelSign);
        psSub->achSign[1]=eLevelSign;
        psSub->achParam[k+1]='\0';
        rtn=m*4+2;
    }
    else
    {
        psSub->nLen=0;
        psSub->achSign[0]=eLevelSign;
        psSub->achSign[1]=eLevelSign;
        psSub->achParam[0]='\0';
        rtn=2;

    }
    //printf("\n val:--\n %d %s %c %c \n",m,strIn, eLevelSign,eLevelSign);
    //printf("\n Sub:--\n %d %s %c %c \n",psSub->nLen, psSub->achParam, psSub->achSign[0],psSub->achSign[1]);
    return rtn;
}

int decodeFixedCode(char *fixedCode)
{
    int nStart=4;
    int i,m;
    char strTBuff[10];
    //printf("\nsize of %d",sizeof(ST_FIXED));
    memset(&gsFixed,0,sizeof(ST_FIXED));
    //lead code
    //bit0
    //nStart+=decodeFixedSub(&fixedCode[nStart],&strHEAD[0],&nHeadLen,achHEADSign);
    nStart+=decodeFixedSub(&fixedCode[nStart],&gsFixed.sHead);
    //bit0
    nStart+=decodeFixedSub(&fixedCode[nStart],&gsFixed.asData[0]);
    //bit1
    nStart+=decodeFixedSub(&fixedCode[nStart],&gsFixed.asData[1]);
    gsFixed.nDataCount=2;
    //stop
    //printf("\nbefore STOP: %d %d %s",nStart,gsFixed.sStop.nLen,gsFixed.sStop.achParam);
    nStart+=decodeFixedSub(&fixedCode[nStart],&gsFixed.sStop);
    //printf("\nafter STOP: %d %d %s",nStart,gsFixed.sStop.nLen,gsFixed.sStop.achParam);
    //printParam();
    //SYNC
    i=0;
    while(1)
    {
        memcpy(strTBuff,&fixedCode[nStart],2);
        strTBuff[2]='\0';    
        m=hex2int(strTBuff);
        if (m==0)
            break; 
        //printf("\nbefore SYNC:%d %d %d %s",i,nStart,gsFixed.sStop.nLen,gsFixed.sStop.achParam);
        //printParam();
        nStart+=decodeFixedSub(&fixedCode[nStart],&gsFixed.asSync[i]);
        //printf("\nafter SYNC:%d %d %d %s",i,nStart,gsFixed.sStop.nLen,gsFixed.sStop.achParam);
        i++;
    }
    gsFixed.nSyncCount=i;
    //printf("\nafter sync");
    //printParam();
}

int decodeParamSub(char *strOut,int nPos,char *pchLastSign,ST_SUBFIXED *pS)
{
    int nLast,nCurr;
    char achTBuff[10];
    char *currSign;
    //printf("\nparamSUB:%c %c %c \n",*pchLastSign,pS->achSign[0],pS->achSign[1]);
    if (pS->nLen>0)
    {
        if (*pchLastSign == pS->achSign[0])
        {
            //merge two elevel due to the sign is equal 
            memcpy(achTBuff,&strOut[nPos-4],4);
            achTBuff[4]='\0';
            nLast=l4_hex2int(achTBuff);      
            memcpy(achTBuff,pS->achParam,4);
            achTBuff[4]='\0';
            nCurr=l4_hex2int(achTBuff);      
            int2hex(nCurr+nLast,4,1,achTBuff);
            //printf ("\n %d %d %s",nLast,nCurr,achTBuff);
            nPos-=4;
            memcpy(&strOut[nPos],achTBuff,4);
            nPos+=4;
            nCurr=pS->nLen*4-4;
            memcpy(&strOut[nPos],&pS->achParam[4],nCurr);
            nPos+=nCurr;
            strOut[nPos]='\0';
        }
        else
        {
            nCurr=pS->nLen*4;
            //printf("\n-----%d nPos:%d, %s",nCurr,nPos,&strOut[nPos]);
            memcpy(&strOut[nPos],pS->achParam,nCurr);
            nPos+=nCurr;
            strOut[nPos]='\0';
        }
        *pchLastSign=pS->achSign[1];
    }
    return nPos;
}

int decodeParam(char *strParam, char *strOut)
{
    int nStart=0;
    int nPos=0;
    int i,k,m;
    int nSync=0;
    int nBitCount,nByteCount;
    char chLastSign='L';
    char strTBuff[10];

    //send count
    memcpy(strOut,"010000",6);
    nPos=6;
    strOut[nPos]='\0';
    //printf("\nSEND BUF: %s",strOut);
  
    memcpy(strTBuff,&strParam[nStart],2);
    strTBuff[2]='\0';    
    m=hex2int(strTBuff);
    nLoopCount=m;
    nStart+=2;

    //head
    if (gsFixed.sHead.nLen>0)
    { 
        k=gsFixed.sHead.nLen*4;
        memcpy(&strOut[nPos],gsFixed.sHead.achParam,k);
        nPos+=k;
        strOut[nPos]='\0';
        chLastSign=gsFixed.sHead.achSign[1];
        //printf("\nSEND BUF: %s",strOut);
    }
    
    //data & sync
    while(1)
    {
        memcpy(strTBuff,&strParam[nStart],4);
        nStart+=4;
        strTBuff[4]='\0';    
        m=l4_hex2int(strTBuff);
        if (m==0)
            break; 
        nBitCount= m & 0x7;
        nByteCount=m >> 3;
        //printf ("\n while: %d, %d, %d",m,nBitCount,nByteCount);
        //printf ("\n lastSign: %c\n",chLastSign);
        for (i=0;i<nByteCount;i++)
        {
            //memcpy(strTBuff,&strParam[nStart],2);
            strTBuff[0]=strParam[nStart];
            nStart++;
            strTBuff[1]=strParam[nStart];
            nStart++;
            strTBuff[2]='\0';    
            m=hex2int(strTBuff);
            //printf("%02x ",m);
            for (k=0;k<8;k++)
            {
                int nT1;
                nT1=(m>>k)&1;
                switch(nT1)
                {
                    case 0:
                        nPos=decodeParamSub(strOut,nPos,&chLastSign,&gsFixed.asData[0]);
                        break;
                    case 1:
                        nPos=decodeParamSub(strOut,nPos,&chLastSign,&gsFixed.asData[1]);
                        break;
                } 
            //printf("\nSend BUF: %s\n", strOut);
            } 
        } 
        if (nBitCount>0)
        {
            strTBuff[0]=strParam[nStart];
            nStart++;
            strTBuff[1]=strParam[nStart];
            nStart++;
            strTBuff[2]='\0';    
            m=hex2int(strTBuff);
            for (k=0;k<nBitCount;k++)
            {
                int nT1;
                nT1=(m>>k)&1;
                //printf("%d",nT1);
                switch(nT1)
                {
                    case 0:
                        nPos=decodeParamSub(strOut,nPos,&chLastSign,&gsFixed.asData[0]);
                        break;
                    case 1:
                        nPos=decodeParamSub(strOut,nPos,&chLastSign,&gsFixed.asData[1]);
                        break;
                } 
            //printf("\nSend BUF: %s\n", strOut);
            }
        }
        // double check if the sync code is wrong
        memcpy(strTBuff,&strParam[nStart],4);
        //nStart+=4;
        strTBuff[4]='\0';    
        m=l4_hex2int(strTBuff);
        if (m==0)
            break; 
        //sync data
        nPos=decodeParamSub(strOut,nPos,&chLastSign,&gsFixed.asSync[nSync]);
        nSync++;
 
    }
    //stop 
    if (gsFixed.sStop.nLen>0)
        nPos=decodeParamSub(strOut,nPos,&chLastSign,&gsFixed.sStop);
    //sendCount,or loop
    k=nPos-6;
    for (i=0;i<nLoopCount-1;i++)
    {
       memcpy(&strOut[nPos],&strOut[6],k);
       nPos+=k;
        
    }
    //count eLevel
    k=(nPos-6)>>2;
    int2hex(k,4,1,strTBuff);
    memcpy(&strOut[2],strTBuff,4);
    
    //printf("\nnPos: %d\n", nPos);
    //printf("\nSend BUF: %s\n", strOut);
};

int decodeElevel(char *strParam, int an[])
{
    int nStart=2;
    int i,k,m;
    char strTBuff[10];
    memcpy(strTBuff,&strParam[nStart],4);
    nStart+=4;
    strTBuff[4]='\0';    
    k=l4_hex2int(strTBuff);
    //printf("\n elevel len: %d\n",k);
    if (k < CONST_MAXELEVELLEN)
    {
        int nPos=0;
        for (i=0;i<k;i++)
        {
            memcpy(strTBuff,&strParam[nStart],4);
            nStart+=4;
            strTBuff[4]='\0';    
            m=l4_hex2int(strTBuff);
            an[nPos]=m;
            //printf(" %d",an[nPos]);
            nPos++;
            
        }
    }
    else
        k=-1;
    return k;
};


char chHighLowSwitch(char chSwitch)
{
    if (chSwitch=='H')
    {
        chSwitch='L';
    }
    else
    {
        chSwitch='H';
    }
    return chSwitch;
}

char ch_int2hex(int n)
{
    char rtn=' ';
    char hex[]="0123456789ABCDEF";
    if (n<16 && n>=0)
        rtn=hex[n];
    return rtn; 
}



int int2hex(long nValue,int nFillLen, int nBigEnding,char *pBuff)
{
    char strMode[16]="%";
    int nModePos;
    int i,nT1;
    // special optimize for 04X format
    if (nValue<0x10000 && nFillLen==4)
    {
        for (i=0;i<4;i++)
        {
            nT1=nValue&0xF;
            nValue>>=4;
            pBuff[3-i]=ch_int2hex(nT1);
        } 
        pBuff[4]='\0';
        //printf("\n test int2hex %s\n",pBuff);
    }
    else
    {
        //printf ("\nstrMode:%d %s\n",nModePos,strMode);
        nModePos=1;
        if (nFillLen>0)
        {
            strMode[1]=0x30;
            nModePos++;
            sprintf(&strMode[nModePos],"%d",nFillLen);
        }
        strcat(strMode,"X");
        //printf ("\nstrMode:%d %s\n",nModePos,strMode);
        sprintf(pBuff,strMode,nValue);
        //printf ("\nbuff:%d %s\n",nValue,pBuff);
    }
    if (nBigEnding>0 && nFillLen>=4)
    { 
        for (i=0;i<nFillLen;i+=4)
        {
            nT1=pBuff[i];
            pBuff[i]=pBuff[i+2];
            pBuff[i+2]=nT1;
            nT1=pBuff[i+1];
            pBuff[i+1]=pBuff[i+3];
            pBuff[i+3]=nT1;

        }
    }
    //printf ("\nswitch buff:%d %s\n",nValue,pBuff);
}

int hex2int(char *hex)
{
    int i,t;
    int sum=0;
    for (i=0;hex[i];i++)
    {
        if (hex[i]=='x' || hex[i]=='X')
            continue;
        if (hex[i]<='9' && hex[i]>='0') 
             t=hex[i]-'0';
        else if (hex[i]<='f' && hex[i]>='a')
             t=hex[i]-'a'+10;
        else if (hex[i]<='F' && hex[i]>='A')
             t=hex[i]-'A'+10;
        else
            break;
        sum=(sum<<4)+t;
 
    }  
    return sum;
}

int l4_hex2int(char *lhex)
{
    char chT;
    chT=lhex[0];
    lhex[0]=lhex[2];
    lhex[2]=chT;
    chT=lhex[1];
    lhex[1]=lhex[3];
    lhex[3]=chT;
    lhex[4]='\0';
    return hex2int(lhex);
}



int printParam()
{
    int i;
    printf("\n--Print Param--");
    //lead code
    //printf("\nhead: %d %c %c %s",nHeadLen,achHEADSign[0],achHEADSign[1],strHEAD);
    printf("\nhead: %d %c %c %s",gsFixed.sHead.nLen,gsFixed.sHead.achSign[0],gsFixed.sHead.achSign[1],gsFixed.sHead.achParam);
    //bit0
    //printf("\nbit0: %d %c %c %s",anDataLen[0], achDATASign[0][0],achDATASign[0][1],asDATA[0]);
    printf("\nbit0: %d %c %c %s",gsFixed.asData[0].nLen,gsFixed.asData[0].achSign[0],gsFixed.asData[0].achSign[1],gsFixed.asData[0].achParam);
    //bit1
    //printf("\nbit1: %d %c %c %s",anDataLen[1], achDATASign[1][0],achDATASign[1][1],asDATA[1]);
    printf("\nbit1: %d %c %c %s",gsFixed.asData[1].nLen,gsFixed.asData[1].achSign[0],gsFixed.asData[1].achSign[1],gsFixed.asData[1].achParam);
    //stop
    //printf("\nSTOP: %d %c %c %s",nStopLen, achSTOPSign[0],achSTOPSign[1], strSTOP);
    printf("\nstop: %d %c %c %s",gsFixed.sStop.nLen,gsFixed.sStop.achSign[0],gsFixed.sStop.achSign[1],gsFixed.sStop.achParam);
    //SYNC
    printf("\nSYNC Count %d",gsFixed.nSyncCount);
    for (i=0;i<gsFixed.nSyncCount;i++)
    {
        //printf("\nSYNC: %d %c %c %s",anSyncLen[i],achSYNCSign[i][0],achSYNCSign[i][1],asSYNC[i]);
        printf("\nsync: %d %c %c %s",gsFixed.asSync[i].nLen,gsFixed.asSync[i].achSign[0],gsFixed.asSync[i].achSign[1],gsFixed.asSync[i].achParam);
    }
    printf("\n--end--\n");
}



