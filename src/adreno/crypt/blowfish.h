#ifndef BLOWFISH_H
#define BLOWFISH_H

#define ECB 0
#define CBC 1
#define CFB 2

typedef struct
{
	unsigned int Left;
	unsigned int Right;
} AdrenoBlowfishBlock;

typedef struct
{
	AdrenoBlowfishBlock Chain;
	AdrenoBlowfishBlock Chain0;
	unsigned int P[18];
	unsigned int S[4][256];
} AdrenoBlowfish;

#ifdef __cplusplus
extern "C"
{
#endif
	
	extern void AdrenoBlowfish_Initialize(AdrenoBlowfish *bf);
	extern void AdrenoBlowfish_SetKey(AdrenoBlowfish *bf, unsigned char *key, unsigned int size, AdrenoBlowfishBlock chain);
	extern void AdrenoBlowfish_Reset(AdrenoBlowfish *bf);
	extern void AdrenoBlowfish_Encrypt(AdrenoBlowfish *bf, unsigned char *data, unsigned char *out, unsigned int size, int mode);
	extern void AdrenoBlowfish_Decrypt(AdrenoBlowfish *bf, unsigned char *data, unsigned char *out, unsigned int size, int mode);

#ifdef __cplusplus
}
#endif

#endif
