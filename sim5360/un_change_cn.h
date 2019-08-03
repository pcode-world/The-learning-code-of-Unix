/********************************************************************************
 *      Copyright:  (C) 2019 SCUEC
 *                  All rights reserved.
 *
 *       Filename:  un_change_cn.h
 *    Description:  This head file head of un_change_cn.c
 *
 *        Version:  1.0.0(08/03/2019)
 *         Author:  Donald Shallwing <donald_shallwing@foxmail.com>
 *      ChangeLog:  1, Release initial version on "08/03/2019 07:40:57 PM"
 *                 
 ********************************************************************************/

int unicode_to_utf8 (char *inbuf, size_t *inlen, char *outbuf, size_t *outlen);

int utf8_to_unicode (char *inbuf, size_t *inlen, char *outbuf, size_t *outlen);


