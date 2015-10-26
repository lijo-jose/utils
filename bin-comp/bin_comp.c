
#include <stdio.h>
#include <stdlib.h>

#define ENABLE_COLORING

#ifdef ENABLE_COLORING
#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#else
#define KNRM  ""
#define KRED  ""
#endif

int main(int argc, char * argv[])
{
	int i, j, k, l; 
	int block_size = 1024;
	int block_mode = 0;
	int bytes_per_line = 8; 
	int offset1 = 0; 
	int offset2 = 0; 
	int change_color_flag = 0; 
	


	if(3 > argc)
	{
		printf("Usage: %s <infile1> <infile2>\n", argv[0]); 
	}
	
	for(i = 1; i < (argc-2); i++)
	{
		if(0 == strcmp(argv[i], "bs"))
		{
			block_size = atoi(argv[i+1]);
			printf("block size specified %d\n", block_size);
			i++; 
		}
		if(0 == strcmp(argv[i], "m"))
		{
			block_mode = atoi(argv[i+1]);
			printf("block mode specified %d\n", block_mode); 
			i++; 
		}
		/* bytes per line */
		if(0 == strcmp(argv[i], "bpl"))
		{
			bytes_per_line = atoi(argv[i+1]); 
			printf("Bytes per line %d\n", bytes_per_line); 
			i++;
		}
		
		if(0 == strcmp(argv[i], "of1"))
		{
			offset1 = atoi(argv[i+1]);
			printf("Change in offset 1 %d\n", offset1); 
			i++;
		}
		if(0 == strcmp(argv[i], "of2"))
		{
			offset2 = atoi(argv[i+1]);
			printf("Change in offset 2 %d\n", offset2); 
			i++;
		}
	}
	
	
	
	{
		FILE *infp1 = fopen(argv[argc -2 ], "rb"); 
		if(NULL == infp1)
		{
			printf("Failed to open the file %s\n", argv[1]); 
			return 0;
		}
		if(0 != offset1)
			fseek(infp1, offset1, SEEK_SET); 
		
		FILE * infp2 = fopen(argv[argc - 1], "rb"); 
		if(NULL == infp2)
		{
			printf("Failed to open the file %s\n", argv[2]); 
			fclose(infp1); 
			return 0;
		}
		if(0 != offset2)
			fseek(infp2, offset2, SEEK_SET); 
		
		int read_size1, read_size2; 
		unsigned char buf1[1024], buf2[1024];
		int tot_size = 0; 
		
		do
		{
			
			read_size1 = fread(buf1, 1, block_size, infp1);
			read_size2 = fread(buf2, 1, block_size, infp2); 
			
			i = (read_size1 > read_size2)?read_size2:read_size1;
			
			for(j = 0; j < i; j++)
			{
				if(buf1[j] != buf2[j])
				{
					if(0 == block_mode)
						printf("diff at 0x%x\t%02x\t%02x\n", (tot_size + j), buf1[j], buf2[j]); 
					else if(1 == block_mode)
					{
#ifdef ENABLE_COLORING
						if(1 == change_color_flag)
							printf(KNRM);
						change_color_flag = 0;
#endif
						printf("Block starts at 0x%x\n", (tot_size)); 
						for(k = 0; k < i; k+=bytes_per_line)
						{
#ifdef ENABLE_COLORING
							if(1 == change_color_flag)
								printf(KNRM);
							change_color_flag = 0;
#endif

							printf("  "); 
							for(l = 0; l < bytes_per_line; l++)
							{
								if((l+k)< i)
								{
									if(buf1[l+k] != buf2[l+k])
									{
#ifdef ENABLE_COLORING
										if(0 == change_color_flag)
											printf(KRED);
										change_color_flag = 1;
#endif
										printf("%02x" , buf1[l+k]); 
										printf(" "); 
									}
									else
									{
#ifdef ENABLE_COLORING
										if(1 == change_color_flag)
											printf(KNRM);
										change_color_flag = 0;
#endif
										printf("%02x", buf1[l+k]); 
										printf(" "); 
									}
									
								}
								else
									printf("   "); 
							}
							printf("  "); 
							for(l = 0; l < bytes_per_line; l++)
							{
								if((l+k)< i)
								{
									if(buf1[l+k] != buf2[l+k])
									{
#ifdef ENABLE_COLORING
										if(0 == change_color_flag)
											printf(KRED);
										change_color_flag = 1;
#endif
										printf("%02x", buf2[l+k]); 
										printf(" "); 
									}
									else
									{
#ifdef ENABLE_COLORING
										if(1 == change_color_flag)
											printf(KNRM);
										change_color_flag = 0;
#endif
										printf("%02x", buf2[l+k]); 
										printf(" "); 
									}
								}
								else
									printf("   "); 
							}
							printf("\n"); 
						}
						break;
					}
					else if(2 == block_mode)
					{
						printf("0x%x\n", (tot_size)); 
						break;
					}
				}
				
			}
			
			if((read_size1 > read_size2) || (feof(infp2) && !feof(infp1)))
			{
				printf("%s is longer than %s\n", argv[argc -2], argv[argc-1]); 
				break;
			}
			if((read_size2 > read_size1) || (feof(infp1) && !feof(infp2)))
			{
				printf("%s is longer than %s\n", argv[argc - 1], argv[argc - 2]); 
				break;
			}
			
			if(feof(infp1) || feof(infp2))
				break;
				
			tot_size += i; 
		}
		while(1);
		
		fclose(infp1); 
		fclose(infp2); 
	}
	
	return 0; 
}
