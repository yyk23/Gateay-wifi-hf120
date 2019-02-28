#include <stdio.h>
#include <string.h>
#include<sys/stat.h>

#define FOUR_BYTE 4
#define MAX_FILE_NAME_LEN 256
#define OTA_MAGIC_NUM (0x4D4D4F49)

long filesize( FILE *fp )
{
    long int save_pos;
    long size_of_file;

    /* Save the current position. */
    save_pos = ftell( fp );

    /* Jump to the end of the file. */
    fseek( fp, 0L, SEEK_END );

    /* Get the end position. */
    size_of_file = ftell( fp );

    /* Jump back to the original position. */
    fseek( fp, save_pos, SEEK_SET );

    return( size_of_file );
}

int main(int argc,char **argv){
    char bin_old_file[MAX_FILE_NAME_LEN], bin_new_file[MAX_FILE_NAME_LEN], *tmp_name = "ota-";
    unsigned int readbuf, tmpbuf;
    char len = 0;
    unsigned char module_id;
    unsigned int checksum = 0, magic = 0;
	int cnt = 0, write_len;
	long file_size = 0;
    FILE *F_O_BIN=NULL, *F_N_BIN=NULL;
    
    //printf ("argc=%d, module_id=%s file_name: %s\n", argc, argv[1], argv[2]);
    if(argc != 3) {
        printf("##########input parameters fail, please check##########\n");
        return;
    }
    F_O_BIN = fopen(argv[2],"r");
    if(!F_O_BIN) {
        printf("########## %s is not exist, please check##########\n", argv[1]);
        return;
    }
    
    module_id = atoi(argv[1]);
    strcpy(bin_old_file, argv[2]);
    strcpy(bin_new_file, tmp_name);
    strcpy(bin_new_file + strlen(tmp_name), bin_old_file);
    //printf ("bin_old_file: %s, bin_new_file: %s, module_id: %d\n", bin_old_file, bin_new_file, module_id);


    F_N_BIN = fopen(bin_new_file,"w");
	file_size = filesize(F_O_BIN);
	//printf("st.st_size: %ld\n" ,file_size);
    if(F_N_BIN) {
        fclose(F_N_BIN);
        remove(bin_new_file);
        F_N_BIN = fopen(bin_new_file,"w");
    }


    while(file_size > 0) {
		memset(&readbuf, 0, FOUR_BYTE);
		if(file_size >= FOUR_BYTE)
			write_len = FOUR_BYTE;
		else
			write_len = file_size;
        fread(&readbuf, write_len, 1, F_O_BIN);
		checksum = checksum ^ readbuf;
		//printf("readbuf:%08x checksum: %08x file_size: %ld\n", readbuf, checksum, file_size);
		fwrite(&readbuf, write_len, 1, F_N_BIN);
		file_size -= write_len;
		//printf("[%d] %x %x\n", cnt, checksum, readbuf);
		cnt++;
    }
    
    if(F_O_BIN != NULL) {
        fclose(F_O_BIN);
    }
    
    if(F_N_BIN != NULL) {
		magic = OTA_MAGIC_NUM;
        fwrite(&magic, sizeof(magic), 1, F_N_BIN);
        fwrite(&module_id, 1, 1, F_N_BIN);
        for(cnt = 0; cnt < sizeof(checksum); cnt++) {
            tmpbuf = (checksum >> (cnt * 8)) & 0xff;
            fwrite(&tmpbuf, 1, 1, F_N_BIN);
        }
        fclose(F_N_BIN);
    }
	
	remove(bin_old_file);
	rename(bin_new_file, bin_old_file);
    return 0;
}
