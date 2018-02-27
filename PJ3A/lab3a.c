#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "ext2_fs.h"

int csvfd;
struct ext2_super_block super;


void read_superblock(int filesystem);
void time_converter(__u32 seconds, char* buffer);


int main(int argc, char** argv) {

    if (argc != 2) {
        fprintf(stderr, "no file system\n");
        exit(1);
    }

    int file = open(argv[1], O_RDONLY);
    if (file == -1) {
        int errnum = errno;
        fprintf(stderr, "open file failed with error %s\n", strerror(errnum));
        exit(1);
    }
    read_superblock(file);
    printf("SUPERBLOCK,%d,%d,%d,%d,%d,%d,%d\n", super.s_blocks_count, super.s_inodes_count, 1024 << super.s_log_block_size, super.s_inode_size, super.s_blocks_per_group, super.s_inodes_per_group, super.s_first_ino);

    __u32 t_b = super.s_blocks_count;
    __u32 t_node = super.s_inodes_count;
    __u32 b_per_g = super.s_blocks_per_group;
    __u32 node_per_g = super.s_inodes_per_group;
    int blocksize = 1024 << super.s_log_block_size;
    __u16 nodesize = super.s_inode_size;
    int numberofGroups = (t_b + b_per_g -1) / b_per_g; //round up division
    struct ext2_group_desc group_desc[numberofGroups];

    pread(file, group_desc, (size_t)numberofGroups * 32, 2048);

    int b_thisgroup = b_per_g;
    int node_thisgroup = node_per_g;
    for (int i = 0; i < numberofGroups; i++) {
        if ((i + 1) * b_per_g > t_b) {
            b_thisgroup = t_b - i * b_per_g;
        }
        if ((i + 1) * node_per_g > t_node) {
            node_thisgroup = t_node - i * node_per_g;
        }
        printf("GROUP,%d,%d,%d,%d,%d,%d,%d,%d\n", i, b_thisgroup, node_thisgroup, group_desc[i].bg_free_blocks_count, group_desc[i].bg_free_inodes_count, group_desc[i].bg_block_bitmap, group_desc[i].bg_inode_bitmap, group_desc[i].bg_inode_table);

        int Boffset = (group_desc[i].bg_block_bitmap) * blocksize;
        int Ioffset = (group_desc[i].bg_inode_bitmap) * blocksize;
        void *blockBitmap = malloc(blocksize);
        void *InodeBitmap = malloc(blocksize);

        pread(file, blockBitmap, blocksize, Boffset);
        pread(file, InodeBitmap, blocksize, Ioffset);

        int FreeBlocknum = 1;
        int FreeInodenum = 1;
        for (int j = 0; j < blocksize; j++) {
            __u8 cur_bit = *((__u8*) blockBitmap + j);
            __u8 temp = 1;
            for (int k = 0; k < 8; k++) {
                if ((cur_bit & temp) == 0) {
                    printf("BFREE,%d\n", FreeBlocknum);
                }
                FreeBlocknum++;
                temp <<= 1;
            }
        }
        for (int a = 0; a < blocksize; a++) {
            __u8 cur_bit = *((__u8 *) InodeBitmap + a);
            __u8 temp = 1;
            for (int b = 0; b < 8; b++) {
                if((cur_bit & temp) == 0) {
                    if (FreeInodenum > node_thisgroup) {break;}
                    printf("IFREE,%d\n", FreeInodenum);
                }
                FreeInodenum++;
                temp <<= 1;
            }
        }

        for (int c = 0; c < node_thisgroup; c++) {
            int inodeTableOffset = group_desc[i].bg_inode_table * blocksize + c * nodesize;
            struct ext2_inode inode;
            void *inode_ptr = (void *)&inode;

            pread(file, inode_ptr, sizeof(inode), inodeTableOffset);

            if (inode.i_mode != 0 && inode.i_links_count != 0) {
                char type ='?';
                if (inode.i_mode & 0x8000) {
                    type = 'f';
                }else if (inode.i_mode & 0x4000) {
                    type = 'd';
                }else if (inode.i_mode & 0xA000){
                    type = 's';
                }
                __u32 c_time = inode.i_ctime;
                __u32 m_time = inode.i_mtime;
                __u32 a_time = inode.i_atime;
                char *ct = malloc(30);
                time_converter(c_time, ct);
                char *mt = malloc(30);
                time_converter(m_time, mt);
                char *at = malloc(30);
                time_converter(a_time, at);
                __u32 *i_block = inode.i_block;
                __u32 block_ptr[15];
                for (int l = 0; l < 15; l++) {
                    if ((i_block + l) != NULL) {
                        block_ptr[l] = *(i_block + l);
                    }else {
                        block_ptr[l] = 0;
                    }
                }
                printf("INODE,%d,%c,%o,%d,%d,%d,%s,%s,%s,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n", c + 1, type, inode.i_mode & 0xFFF, inode.i_uid, inode.i_gid, inode.i_links_count, ct, mt, at, inode.i_size, inode.i_blocks, block_ptr[0], block_ptr[1], block_ptr[2], block_ptr[3], block_ptr[4], block_ptr[5], block_ptr[6], block_ptr[7], block_ptr[8], block_ptr[9], block_ptr[10], block_ptr[11], block_ptr[12], block_ptr[13], block_ptr[14]);

                //directory analysis
                if (type == 'd') {
                    for (int m = 0; m < 12; m++) {
                        if (block_ptr[m] == 0) {
                            continue;
                        } else {
                            int currOffset = 0;
                            while (currOffset < blocksize) {
                                struct ext2_dir_entry dir;
                                void *dir_ptr = (void *) &dir;

                                pread(file, dir_ptr, sizeof(dir), block_ptr[m] * blocksize + currOffset);

                                if (dir.inode != 0) {
                                    printf("DIRENT,%d,%d,%d,%d,%d,\'%s\'\n", c + 1, currOffset, dir.inode, dir.rec_len,
                                           dir.name_len, dir.name);
                                }
                                currOffset += dir.rec_len;
                            }
                        }
                    }
                }

                __u32 *F_ptr = malloc(blocksize);
                __u32 *S_ptr = malloc(blocksize);
                __u32 *T_ptr = malloc(blocksize);

                if (type == 'd' || type == 'f') {
                    //FIRST INDIRECT
                    if (block_ptr[12] != 0) {
                        pread(file, F_ptr, blocksize, block_ptr[12] * blocksize);
                        for (int k = 0; k < blocksize / 4; k++) {
                            if (F_ptr[k] == 0) {
                                continue;
                            }else {
                                printf("INDIRECT,%d,%d,%d,%d,%d\n", c + 1, 1, 12 + k, block_ptr[12], F_ptr[k]);
                            }
                        }
                    }
                    memset(F_ptr, 0, blocksize);

                    //Second INDIRECT
                    if (block_ptr[13] != 0) {
                        pread(file, S_ptr, blocksize, block_ptr[13] * blocksize);
                        for (int n = 0; n < blocksize / 4; n++) {
                            if (S_ptr[n] == 0) {
                                continue;
                            }else {
                                printf("INDIRECT,%d,%d,%d,%d,%d\n", c + 1, 2, 256 + 12 + n, block_ptr[13], S_ptr[n]);
                                pread(file, F_ptr, blocksize, S_ptr[n] * blocksize);
                                for (int o = 0; o < blocksize / 4; o++) {
                                    if (F_ptr[o] == 0) {
                                        continue;
                                    }else {
                                        printf("INDIRECT,%d,%d,%d,%d,%d\n", c + 1, 1, 256 + 12 + o, S_ptr[n], F_ptr[o]);
                                    }
                                }
                            }
                        }
                    }
                    memset(F_ptr, 0, blocksize);
                    memset(S_ptr, 0, blocksize);

                    //Third INDIRECT
                    if (block_ptr[14] != 0) {
                        pread(file, T_ptr, blocksize, block_ptr[14] * blocksize);
                        for (int t = 0; t < blocksize / 4; t++) {
                            if (T_ptr[t] == 0) {
                                continue;
                            }else {
                                printf("INDIRECT,%d,%d,%d,%d,%d\n", c + 1, 3, 256 * 257 + 12 + t, block_ptr[14], T_ptr[t]);
                                pread(file, S_ptr, blocksize, T_ptr[t] * blocksize);
                                for (int n = 0; n < blocksize / 4; n++) {
                                    if (S_ptr[n] == 0) {
                                        continue;
                                    }else {
                                        printf("INDIRECT,%d,%d,%d,%d,%d\n", c + 1, 2, 256 * 257 + 12 + n, T_ptr[t], S_ptr[n]);
                                        pread(file, F_ptr, blocksize, S_ptr[n] * blocksize);
                                        for (int o = 0; o < blocksize / 4; o++) {
                                            if (F_ptr[o] == 0) {
                                                continue;
                                            }else {
                                                printf("INDIRECT,%d,%d,%d,%d,%d\n", c + 1, 1, 256 * 257 + 12 + o, S_ptr[n], F_ptr[o]);
                                            }
                                        }
                                    }
                                }
                            }
                        }

                    }
                }
            }
        }
    }
    return 0;



}

void read_superblock(int filesystem) {
    void *super_ptr = (void *)&super;
    pread(filesystem, super_ptr, sizeof(super), 1024);
}

void time_converter(__u32 seconds, char *buffer) { //TODO:FIX THE TIME CONVERSION. IMPORT LIBRARY?
    time_t rawtime = seconds;
    struct tm *temp;
    temp = gmtime(&rawtime);
    strftime(buffer, 30, "%m/%d/%g %H:%M:%S", temp);
}

