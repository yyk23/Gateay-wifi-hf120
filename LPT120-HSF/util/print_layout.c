#include <stdio.h>
#include <string.h>

#include <ssv_config.h>


#if 0
typedef unsigned char       	U8;
typedef unsigned short      	U16;
typedef unsigned int        	U32;
typedef unsigned long long  U64;

typedef char            		S8;
typedef short           		S16;
typedef int             		S32;
typedef long long       		S64;
typedef unsigned int		BOOL;
#endif

#define LINE_LENGTH (40)
#define KB (1024)
char line_buf[LINE_LENGTH];



void reset_buf(){
    memset((void*)&line_buf[0],' ',LINE_LENGTH);
}

void print_line()
{
    int loop_i = 0;
    for(loop_i=0;loop_i<LINE_LENGTH;loop_i++)
    {
        printf("-");
    }
    printf("\n");
}

void print_boundary()
{
    int loop_i = 0;
    for(loop_i=0;loop_i<LINE_LENGTH;loop_i++)
    {
        if( (loop_i==0) || (loop_i==(LINE_LENGTH-1)) )
        {
            printf("|");
        } 
        else 
        {
            printf(" ");
        }
       
    }
    printf("\n");
}


void print_msg_addr(char *msg,int addr)
{
    int msg_pos = 8;
    char astr_size[32] = "";
    int size_slen = 0;
    int msg_slen = 0;

    reset_buf();
    sprintf(&astr_size[0]," = 0x%x",addr);

    size_slen = strlen(astr_size);
    msg_slen = strlen(msg);
    
    memcpy(&line_buf[msg_pos], msg, msg_slen);
    memcpy(&line_buf[msg_pos + msg_slen], astr_size, size_slen);
    
    line_buf[0] = '|';
    line_buf[LINE_LENGTH-1] = '|';
    
    printf("%s \n",&line_buf[0]);
}

void print_msg_size(char *msg,int flash_size)
{
    int msg_pos = 8;
    char astr_size[32] = "";
    int size_slen = 0;
    int msg_slen = 0;

    reset_buf();

    if( (flash_size%(KB) != 0) && (flash_size <= (4*KB))){
        sprintf(&astr_size[0]," = %d(Bytes)",flash_size);
    } else {
        sprintf(&astr_size[0]," = %d(KB)",flash_size/KB);
    }


    size_slen = strlen(astr_size);
    msg_slen = strlen(msg);
    
    memcpy(&line_buf[msg_pos], msg, msg_slen);
    memcpy(&line_buf[msg_pos + msg_slen], astr_size, size_slen);
    
    line_buf[0] = '|';
    line_buf[LINE_LENGTH-1] = '|';
    
    printf("%s \n",&line_buf[0]);
}


void print_map(char *msg,int flash_size)
{
    print_line(); 
    print_boundary();
    print_msg_size( msg, flash_size);
}

void print_map_two_msg(char *msg0,unsigned int size0,char *msg1,unsigned int size1)
{
    print_line(); 
    print_boundary();
    print_msg_addr( msg0, size0);
    print_msg_size( msg1, size1);
}

void print_map_top_msg(char *msg)
{
  printf("\n<%s>\n", msg);
}

void print_map_sram()
{
    //size
    #define TOTAL_SRAM_SIZE (192*KB)
    #define  NORMAL_STACK_SIZE (0x00001000)
    #define  IRQ_STACK_SIZE (0X00000400)
    #define  FIQ_STACK_SIZE (0x00000400)
    #define  ABT_STACK_SIZE (0x00000040)
    #define  UND_STACK_SIZE (0x00000040)
    #define TOTAL_STACK_SIZE (NORMAL_STACK_SIZE + IRQ_STACK_SIZE + FIQ_STACK_SIZE + ABT_STACK_SIZE + UND_STACK_SIZE)

    #define BOOT_SECTOR_SRAM_SIZE (4*KB)
    #define OS_SRAM_SIZE (TOTAL_SRAM_SIZE - BOOT_SECTOR_SRAM_SIZE - TOTAL_STACK_SIZE)

    //start address
    #define SRAM_START_ADDR (0x2000000)
    #define BOOT_SECTOR_SRAM_START_ADDR (SRAM_START_ADDR)
    #define OS_SRAM_START_ADDR (BOOT_SECTOR_SRAM_START_ADDR + BOOT_SECTOR_SRAM_SIZE)
    
    #define NORMAL_STACK_START_ADDR (SRAM_START_ADDR + TOTAL_SRAM_SIZE - TOTAL_STACK_SIZE)
    #define IRQ_STACK_START_ADDR ( NORMAL_STACK_START_ADDR + NORMAL_STACK_SIZE)
    #define FIQ_STACK_START_ADDR ( IRQ_STACK_START_ADDR + IRQ_STACK_SIZE)
    #define ABT_STACK_START_ADDR ( FIQ_STACK_START_ADDR + FIQ_STACK_SIZE)    
    #define UND_STACK_START_ADDR ( ABT_STACK_START_ADDR + ABT_STACK_SIZE)    

    print_map_top_msg("SRAM layout");
    printf("TOTAL_SRAM_SIZE = %d KB\n",TOTAL_SRAM_SIZE/KB);

    //BOOT SECTOR
    print_map_two_msg("BOOT SECTOR ADDR", BOOT_SECTOR_SRAM_START_ADDR,"SIZE", BOOT_SECTOR_SRAM_SIZE);

    //OS
    print_map_two_msg("OS ADDR", OS_SRAM_START_ADDR,"SIZE", OS_SRAM_SIZE);

    //NORMAL
    print_map_two_msg("NORMAL STACK ADDR", NORMAL_STACK_START_ADDR,"SIZE", NORMAL_STACK_SIZE);

    //IRQ
    print_map_two_msg("IRQ STACK ADDR", IRQ_STACK_START_ADDR,"SIZE", IRQ_STACK_SIZE);

    print_line(); 

}

void print_map_flash()
{
    print_map_top_msg("FLASH layout");

	printf("CFG_OTA_UPGRADE_BOOT = %d\n",CFG_OTA_UPGRADE_BOOT);
	 
    printf("CFG_OTA_ENABLE = %d\n",CFG_OTA_ENABLE);
    printf("TOTAL FLASH SIZE = %d KB\n",CFG_FLASH_SIZE/KB);
    //boot-sector
    print_map_two_msg("BOOT_SECTOR ADDR", CFG_FLASH_START_ADDR,"SIZE", CFG_BOOT_SECTOR_SIZE);

    //mp
    print_map_two_msg("MP ADDR", CFG_MP_START_ADDR,"SIZE", CFG_MP_SIZE);

    //config
    print_map_two_msg("CONFIG ADDR", CFG_CONFIG_START_ADDR,"SIZE", CFG_CONFIG_SIZE);
    
    
    //image 0
    print_map_two_msg("IMAGE0 ADDR", CFG_IMAGE0_START_ADDR,"SIZE", CFG_IMAGE0_SIZE);

#if (CFG_OTA_ENABLE==1)
    //image 1
    print_map_two_msg("IMAGE1 ADDR", CFG_IMAGE1_START_ADDR,"SIZE", CFG_IMAGE1_SIZE);
#endif

    print_map_two_msg("CUSTOMER ADDR", CFG_CUSTOMER_START_ADDR,"SIZE", CFG_CUSTOMER_SIZE);

    print_map_two_msg("FILE_SYSTEM ADDR", CFG_FILE_SYSTEM_ADDR,"SIZE", CFG_FILE_SYSTEM_SIZE);
    print_line(); 
}


unsigned int check_sum32(unsigned char *data,int length)
{
	unsigned int sum=0;
	int i=0;
	
	for(i=0;i<length;i++)
	{
		sum += data[i];
	}

	return sum;
}

int gen_auto_upgrade_image(const char *filename)
{
	unsigned char hdr[128];
	FILE *p_img_file,*p_sw_file, *p_mcu_file;
	char fn[256];
	char buf[1024];
	unsigned int length=0;
	unsigned int sum=0;
	unsigned int upgrade_boot=CFG_OTA_UPGRADE_BOOT;
	unsigned int mcu_ota_flag=1, mcu_addr=0, mcu_len=0, mcu_sum=0;
	int rd;
	
	sprintf(fn,"%s.bin",filename);
	if((p_sw_file = fopen(fn,"rb"))==NULL)
	{
		printf("can't find the software file %s \n",fn);
		return -1;
	}
	
	sprintf(fn,"%s_upgrade.bin",filename);
	p_img_file = fopen(fn,"wb");
	p_mcu_file = fopen("mcu_ota.bin", "rb");
	
	memset(hdr,0,sizeof(hdr));
	strcpy(hdr,"HF-MC3000 Image");
	fseek(p_img_file,128,SEEK_SET);
	while((rd=fread(buf,1,1024,p_sw_file))>0)
	{
		sum+= check_sum32(buf,rd);
		length+=rd;
		fwrite(buf,1,rd,p_img_file);
	}

	if(p_mcu_file != NULL)
	{
		fseek(p_mcu_file,0,SEEK_SET);
		fseek(p_mcu_file,0,SEEK_END);
		mcu_len=ftell(p_mcu_file);
		if(mcu_len <= 96*1024)
		{
			fseek(p_mcu_file, 0, SEEK_SET);

			mcu_addr=128+length;
			mcu_len=0;
			while((rd=fread(buf,1,1024,p_mcu_file))>0)
			{
				mcu_sum+= check_sum32(buf,rd);
				mcu_len+=rd;
				fwrite(buf,1,rd,p_img_file);
			}
		}
		else
			printf("| MCU ota file is too big, must <96KB |\n");
	}
	
	fseek(p_img_file,0,SEEK_SET);
	memcpy(hdr+16,&length,4);
	memcpy(hdr+20,&sum,4);
	memcpy(hdr+24,&upgrade_boot,4);
	if(p_mcu_file != NULL)
	{
		memcpy(hdr+64,&mcu_ota_flag,4);
		memcpy(hdr+68,&mcu_addr,4);
		memcpy(hdr+72,&mcu_len,4);
		memcpy(hdr+76,&mcu_sum,4);
	}
	
	fwrite(hdr,1,128,p_img_file);

	fclose(p_sw_file);
	fclose(p_img_file);
	if(p_mcu_file != NULL)
		fclose(p_mcu_file);
	
	if(p_mcu_file == NULL)
		printf("| gen image file checksum=%08x,size=%08x |\n",sum,length);
	else
		printf("| gen image file checksum=%08x,size=%08x , MCU ota checksum=%08x,size=%08x |\n",sum,length,mcu_sum,mcu_len);
	return;
}


int main(int argc,char **argv)
{
	if(argc==2)
	{
	  printf("**************************************************\n");
	  gen_auto_upgrade_image(argv[1]);
	  printf("**************************************************\n");
	}
    print_map_flash();
    
    print_map_sram();
    
    return 0;
}

