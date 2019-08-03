/*********************************************************************************
 *      Copyright:  (C) 2019 SCUEC
 *                  All rights reserved.
 *
 *       Filename:  iconv_test.c
 *    Description:  This file test for function iconv 
 *                 
 *        Version:  1.0.0(08/03/2019)
 *         Author:  Donald Shallwing <donald_shallwing@foxmail.com>
 *      ChangeLog:  1, Release initial version on "08/03/2019 07:38:16 PM"
 *                 
 ********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <iconv.h>
#include "un_change_cn.h"

int unicode_to_utf8 (char *inbuf, size_t *inlen, char *outbuf, size_t *outlen)
{
  /* 目的编码, TRANSLIT：遇到无法转换的字符就找相近字符替换
   *           IGNORE ：遇到无法转换字符跳过*/
  char *encTo = "UTF-8//IGNORE";
/* 源编码 */
  char *encFrom = "UNICODE";

  /* 获得转换句柄
   *@param encTo 目标编码方式
   *@param encFrom 源编码方式
   *
   * */
  iconv_t cd = iconv_open (encTo, encFrom);

  if (cd == (iconv_t)-1)
  {
     perror ("iconv_open");
  }

  /* 需要转换的字符串 */
  printf("inbuf=%s\n", inbuf);

  /* 打印需要转换的字符串的长度 */
  printf("inlen=%lu\n", *inlen);


  /* 由于iconv()函数会修改指针，所以要保存源指针 */
  char *tmpin = inbuf;
  char *tmpout = outbuf;
  size_t insize = *inlen;
  size_t outsize = *outlen;

  /* 进行转换
   *@param cd iconv_open()产生的句柄
   *@param srcstart 需要转换的字符串
   *@param inlen 存放还有多少字符没有转换
   *@param tempoutbuf 存放转换后的字符串
   *@param outlen 存放转换后,tempoutbuf剩余的空间
   *
   * */
  size_t ret = iconv (cd, &tmpin, inlen, &tmpout, outlen);
  if (ret == -1)
  {
     perror ("iconv");
  }

  /* 存放转换后的字符串 */
  printf("outbuf=%s\n", outbuf);

  //存放转换后outbuf剩余的空间
  printf("outlen=%lu\n", *outlen);

  int i = 0;

  for (i=0; i<(outsize-(*outlen)); i++)
  {
     //printf("%2c", outbuf[i]);
     printf("%x\n", outbuf[i]);
  }

  /* 关闭句柄 */
  iconv_close (cd);

  return 0;
}

int utf8_to_unicode (char *inbuf, size_t *inlen, char *outbuf, size_t *outlen)
{

  /* 目的编码, TRANSLIT：遇到无法转换的字符就找相近字符替换
   *           IGNORE ：遇到无法转换字符跳过*/
  char *encTo = "UNICODE//IGNORE";
  /* 源编码 */
  char *encFrom = "UTF-8";

  /* 由于iconv()函数会修改指针，所以要保存源指针 */
  char *tmpin = inbuf;
  char *tmpout = outbuf;
  size_t insize = *inlen;
  size_t outsize = *outlen;
  int i = 0;
  int j = 0;

  /* 获得转换句柄
   *@param encTo 目标编码方式
   *@param encFrom 源编码方式
   *
   * */
  iconv_t cd = iconv_open (encTo, encFrom);

  if (cd == (iconv_t)-1)
  {
      perror ("iconv_open");
  }

  /* 需要转换的字符串 */
  printf("inbuf=%s\n", inbuf);

  /* 打印需要转换的字符串的长度 */
  printf("inlen=%lu\n", *inlen);

  for(j;j<*inlen;j++)
  {
      printf("%x ",inbuf[j]);
  }

  printf("\n");
  /* 进行转换
   *@param cd iconv_open()产生的句柄
   *@param srcstart 需要转换的字符串
   *@param inlen 存放还有多少字符没有转换
   *@param tempoutbuf 存放转换后的字符串
   *@param outlen 存放转换后,tempoutbuf剩余的空间
   *
   * */
  size_t ret = iconv (cd, &tmpin, inlen, &tmpout, outlen);
  if (ret == -1)
  {
     perror ("iconv");
  }

  /* 存放转换后的字符串 */
  printf("outbuf=%s\n", outbuf);

  //存放转换后outbuf剩余的空间
  printf("outlen=%lu\n", *outlen);


  for (i=0; i<(outsize- (*outlen)); i++)
  {
     printf("%x ", outbuf[i]);
  }

  printf("\n");

  /* 关闭句柄 */
  iconv_close (cd);

  return 0;
}

int main ()
{
  /* 需要转换的字符串 */
  char *text = "你好linux";    
    
  char inbuf[1024] = {0};
  strcpy (inbuf, text);
  size_t inlen = strlen (inbuf);

  /* 存放转换后的字符串 */
  char outbuf[1024] = {0};
  size_t outlen = 1024;

  utf8_to_unicode (inbuf, &inlen, outbuf, &outlen);
  printf ("print outbuf: %s\n", outbuf);

  return 0;
}

/* 总结：
 * 存储以utf-8存储,变长编码,兼容assci码
 * Unicode编码,两个字节表示一个符号*/
