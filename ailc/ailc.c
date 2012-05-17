/*	This file is part of libadreno.

    libadreno is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    libadreno is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with libadreno.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <memory.h>
#include <sys/stat.h>

#include <adreno/vm/vm.h>
#include <adreno/ail/ailc.h>

char *LoadInputFile(char *FileName) 
{
	FILE *Fin;
	char *Buf1;
	struct stat statbuf;
	size_t BytesRead;

	/* Sanity check. */
	if ((FileName == NULL) || (*FileName == '\0')) return(NULL);

	/* Open the file. */
	Fin = fopen(FileName,"rb");
	if (Fin == NULL) 
	{
		fprintf(stdout,"Could not open input file: %s\n",FileName);
		return(NULL);
	}

	/* Get the size of the file. */
	if (fstat(_fileno(Fin),&statbuf) != 0) 
	{
		fprintf(stdout,"Could not stat() the input file: %s\n",FileName);
		fclose(Fin);
		return(NULL);
	}

	/* Allocate memory for the input. */
	Buf1 = (char *)AdrenoAlloc(statbuf.st_size + 1);
	if ((Buf1 == NULL)) 
	{
		fprintf(stdout,"Not enough memory to load the file: %s\n",FileName);
		fclose(Fin);
		if (Buf1 != NULL) free(Buf1);
		return(NULL);
	}

	/* Load the file into memory. */
	BytesRead = fread(Buf1,1,statbuf.st_size,Fin);
	Buf1[BytesRead] = '\0';

	/* Close the file. */
	fclose(Fin);

	/* Exit if there was an error while reading the file. */
	if (BytesRead != statbuf.st_size) 
	{
		fprintf(stdout,"Error while reading input file: %s\n",FileName);
		free(Buf1);
		return(NULL);
	}

	return Buf1;
}

void WriteData(char *data, unsigned int size, char *to)
{
	FILE *fp = fopen(to, "wb");

	if (!fp)
	{
		printf("Error opening output file!\n");
		return;
	}

	fwrite(data, size, 1, fp);
	fclose(fp);
}

void ShowUsage()
{
	printf("usage: ailc [options] [input file]\n");
	printf("If no input is specified stdin is used.\n");
	printf("Options:\n");
	printf("\t-o <output file>: Save the output to output file [default = ail.bin]\n");
	printf("\t-h: Show this text\n");
	printf("\n");
}

int main(int argc, char *argv[])
{
	char *inFile;
	char *outFile = "ail.bin";
	int i = 0;
	
#ifdef USE_MEMORY_MANAGER
	AdrenoMM_Initialize();
#endif

	if (argc < 2)
	{
		ShowUsage();
		return;
	}

	for (i = 1; i < argc; i++)
	{
		if (strcmp(argv[i], "-h") == 0)
		{
			ShowUsage();
			return;
		}
		else if (strcmp(argv[i], "-o") == 0)
		{
			i++;

			if (i >= argc)
			{
				ShowUsage();
				return;
			}

			outFile = argv[i];
		}
		else if (i == argc - 1)
		{
			inFile = argv[i];
		}
		else
		{
			ShowUsage();
			return;
		}
	}

	{
		AilCompiler c;
		AdrenoScript *s;
		unsigned int size;
		char *data;

		AdrenoVM_StaticInit();

		AilCompiler_Initialize(&c, LoadInputFile(inFile));
		s = AilCompiler_Compile(&c);
		AilCompiler_Free(&c);
		AdrenoFree(c.Data);

		data = AdrenoScript_Save(s, &size);

		WriteData(data, size, outFile);
		AdrenoScript_Free(s);
		AdrenoFree(data);

		AdrenoVM_StaticDestroy();
	}
	
#ifdef USE_MEMORY_MANAGER
	AdrenoMM_Final();
#endif

	return 0;
}
