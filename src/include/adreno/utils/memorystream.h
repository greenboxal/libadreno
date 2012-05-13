#ifndef ADRENOMS_H
#define ADRENOMS_H

typedef struct
{
	unsigned char *buffer;
	unsigned int bufferPosition, bufferSize, bufferMaxSize;
	unsigned int expansionFactor;
} AdrenoMS;

#ifdef __cplusplus
extern "C"
{
#endif

	extern void AdrenoMS_Open(AdrenoMS *ms);
	extern int AdrenoMS_Write(AdrenoMS *ms, unsigned char *buffer, unsigned int offset, unsigned int size);
	extern int AdrenoMS_Read(AdrenoMS *ms, unsigned char *buffer, unsigned int offset, unsigned int size);
	extern int AdrenoMS_Seek(AdrenoMS *ms, unsigned int origin, unsigned int offset);
	extern unsigned char *AdrenoMS_Clone(AdrenoMS *ms);
	extern void AdrenoMS_Close(AdrenoMS *ms);

#ifdef __cplusplus
}
#endif

#endif
