#ifndef _CIPHER_TOKEN_H
#define _CIPHER_TOKEN_H
#ifdef  __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stdint.h>

typedef enum {
    ONENET_METHOD_MD5 = 0,
    ONENET_METHOD_SHA1,
    ONENET_METHOD_SHA256,
}method_t;

typedef struct onenet_msg
{
    char produt_id[32];
    char device_name[32];
    char key[128];
}onenet_msg_t;

int onenet_creat_token_init(onenet_msg_t* msg, long long time,method_t token_method,char *token,int maxlen);


#ifdef  __cplusplus
}
#endif /* __cplusplus */
#endif /* _CIPHER_SHA256_H */
