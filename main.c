#include <stdio.h>
#include <stdlib.h>
#include "zutil.h"
#include "zindex.h"

#define APP_NAME "gzapp"
#define APP_VER "0.2"
#define MSP_MAX_FILENAME (unsigned long) 4000


void help(void) {
	printf(APP_NAME);
	printf(" ver.");
	printf(APP_VER);
	printf("\n\n");
	printf("This application compresses and uncompresses an input file. The compression method creates an index file, too.\n");
	printf("\n");
	printf("Compression:\n");
	printf("\tThe input file requirements are: \n");
	printf("\t\t- It must be a text file.\n");
	printf("\t\t- It must contain a first column with unique text / number IDs.\n");
	printf("\t\t- The column separator character must be one TAB. (Be careful, some text editors replace the TAB character by some spaces. In this case, an empty output index file would be generated).\n");
	printf("\t\n");
	printf("\tExample:");
	printf("\t\n\t\t");
	printf(APP_NAME);
	printf(" --compress file.tfa\n");
	printf("\t\n");
	printf("\tThe above example creates two files. Both of them can only be used by ngasp tools:\n");
	printf("\t\t - \"file.tfa.gz\" is a binary compressed file.\n");
	printf("\t\t - \"file.tfa.index\" is a binary uncompressed index file.\n");
	printf("\t\n");
	printf("Uncompression:\n");
	printf("\tThe input file requirements are: \n");
	printf("\t\t- It must be '.gz' file compressed with this application.\n");
	printf("\t\n");
	printf("\tExample:");
	printf("\t\n\t\t");
	printf(APP_NAME);
	printf(" --uncompress file.tfa.gz\n");
	printf("\t\n");
	printf("\tThe above example creates one file:\n");
	printf("\t\t - \"file.tfa\" is a text file.\n");
	printf("\t\n");
}

void printLine(FILE *h, SGZip *gz, struct SGZIndex *idx) {
    char ch = ' ';
    while ((!fzeof(h, gz)) && (ch != '\n')) {
        ch = fzgetc(h, gz);
        printf("%c", ch);
    }
}

void test(void) {
	FILE *h = 0;
	SGZip gz;

	h = fzopen("./examples/input.tfa.gz", "r", &gz);

	if (h != NULL) {

		struct SGZIndex idx;
		load_index_from_file(gz.index_file_name, &idx);

        char search_id[10];
        strcpy(search_id, "1:2");
        printf("Positionate at existing %s\n", search_id);

        long int max = 10;
        long int seq_id = 0;

        if (fzseekNearest(h, &gz, &idx, search_id, max, &seq_id) == GZ_OK) {
            printLine(h, &gz, &idx);

            strcpy(search_id, "1:3");
            printf("Searching for %s\n", search_id);

            if (fzseekNearest(h, &gz, &idx, search_id, max, &seq_id) == GZ_OK) {

                printLine(h, &gz, &idx);

                strcpy(search_id, "1:7");
                printf("Searching for %s\n", search_id);

                if (fzseekNearest(h, &gz, &idx, search_id, max, &seq_id) == GZ_OK) {

                    printLine(h, &gz, &idx);

                    strcpy(search_id, "1:9");
                    printf("Searching for %s\n", search_id);

                    if (fzseekNearest(h, &gz, &idx, search_id, max, &seq_id) == GZ_OK) {

                        printLine(h, &gz, &idx);
		            } else {
			            printf ("Not found %s.\n", search_id);
		            }
		        } else {
			        printf ("Not found %s.\n", search_id);
		        }
		    } else {
			    printf ("Not found %s.\n", search_id);
		    }
        }

		unload_all_index_positions(&idx);

		fzclose(h, &gz);
	}
}


int main(int argc, char *argv[]) {
    //test();
    //return 0;

	char compressed_file_name[MSP_MAX_FILENAME];
	char uncompressed_file_name[MSP_MAX_FILENAME];
	char index_file_name[MSP_MAX_FILENAME];

	memset(compressed_file_name, 0, MSP_MAX_FILENAME);
	memset(uncompressed_file_name, 0, MSP_MAX_FILENAME);
	memset(index_file_name, 0, MSP_MAX_FILENAME);

	if (argc == 3) {

		bool input_file_is_gz = ((strlen(argv[1]) > 3) && ((argv[1][strlen(argv[1]) - 3] == '.') && (argv[1][strlen(argv[1]) - 2] == 'g') && (argv[1][strlen(argv[1]) - 1] == 'z')));

		if(strcmp(argv[1], "--compress") == 0) {

			if (input_file_is_gz) {
				printf ("The input file is already compressed. At least, it has the '.gz' extension.");
			}
			else {
				strcpy(uncompressed_file_name, argv[2]);

				strcpy(compressed_file_name, uncompressed_file_name);
				strcat(compressed_file_name, ".gz");

				strcpy(index_file_name, uncompressed_file_name);
				strcat(index_file_name, ".index");

				switch(compress_file_and_create_index(uncompressed_file_name, compressed_file_name, index_file_name)) {
					case GZ_OK:
						printf("Done.\n");
						printf("Note: Remember that the output compressed file is an special GZ file that can only be uncompressed by this application and it can only be accessed by ngasp tools.\n");
						break;
					case GZ_PARAMS_ERROR:
						printf("Parameters error.\n");
						break;
					case GZ_ERROR_OPEN_FILE:
						printf("The input file cannot be open: '%s'.\n", uncompressed_file_name);
						break;
					case GZ_INDEX_FILE_EXISTS:
						printf("The index file already exists: '%s'. Please, remove it first.\n", index_file_name);
						break;
					case GZ_DEFLATED_FILE_EXISTS:
						printf("The compressed file already exists: '%s'. Please, remove it first.\n", compressed_file_name);
						break;
				}
			}
		}

		if(strcmp(argv[1], "--uncompress") == 0) {
			if (input_file_is_gz) {
				printf ("The input file is already uncompressed. At least, it does not have the '.gz' extension.");
			}
			else {
				strcpy(compressed_file_name, argv[2]);

				strncpy(uncompressed_file_name, compressed_file_name, strlen(compressed_file_name) - 3); /* remove last ".gz" */

				switch(uncompress_file(compressed_file_name, uncompressed_file_name)) {
					case GZ_OK:
						printf("Done.\n");
						break;
					case GZ_ERROR_CREATE_FILE:
						printf("The output file cannot be created: '%s'.\n", uncompressed_file_name);
						break;
					case GZ_ERROR_OPEN_FILE:
						printf("The input file cannot be open: '%s'.\n", compressed_file_name);
						break;
				}
			}
		}

	}
	else {
		help();
	}

	return 0;
}


/*
 * TEST - UNCOMPRESSING A FILE BYTE TO BYTE
 */
/*
{
		FILE *h = 0;
		SGZip gz;
 		h = fzopen("/data/gzapp_data/206_tfasta_chr1_1.tfa.gz", "r", &gz);

        FILE *out = fopen("/data/gzapp_data/b.tfa", "w");

		char ch = ' ';
		while (!fzeof(h, &gz)) {
			ch = fzgetc(h, &gz);

			if (ch == '\x0') {
				printf("The input contains '\\x0'.\n");
			}
            fputc(ch, out);
		}

		fzclose(h, &gz);

        fclose(out);
}
*/
/*
 * TEST - COMPRESSING A FILE USING THE fzprintf FUNCTION
 */
/*
{


	char buffer[CHUNK];
	FILE *in = fopen("/data/gzapp_data/206_tfasta_chr1_1.tfa", "r");

	SGZip gz;
	FILE *out = fzopen("/data/gzapp_data/sortida.tfa.gz", "wb+", &gz);

	size_t bytes_read = 0;
	while (!feof(in)) {
		bytes_read = fread(buffer, CHUNK, 1, in);

		if (bytes_read > 0) {
			fzprintf(out, &gz, buffer);
		}
	}

	fclose(in);
	fzclose(out, &gz);

	exit(0);
}
*/

/*
 * TEST - ACCESSING A GZ FILE
 */
/*
{
	FILE *h = 0;
	SGZip gz;

	h = fzopen("/data/gzapp_data/206_tfasta_chr1_1.tfa.gz", "r", &gz);

	if (h != NULL) {

		struct SGZIndex idx;
		load_index_from_file(gz.index_file_name, &idx);

		printf("Items: %ld\n", idx.items);


		//long int row_num = 30427670 - 1347;     // Set to -1 if you want to search by ID.
												// El primero que se ve 30427670 - 1347
												// El primero que no se ve 30427670 - 1348

		//if (fzseek(h, &gz, &idx, NULL, &row_num, false, DO_NOT_GET_NEAREST) == GZ_OK) { // Or set NULL to the ID if you want to seach by position.
														                  				  // After the call to fzseek, the row_num variable has the reached sequence number (0-based).
																		                 // Set the last parameter to true (1) if you want to search from the last position found. It is faster.


		long int row_num = -1;
		if (fzseek(h, &gz, &idx, "30427671", &row_num, false, DO_NOT_GET_NEAREST) == GZ_OK) {

			printf ("Found.\n");

			char ch = ' ';
			while ((!fzeof(h, &gz)) && (ch != '\n')) {
				ch = fzgetc(h, &gz);
				printf("%c", ch);
			}
		} else {
			printf ("Not found.\n");
		}

		unload_all_index_positions(&idx);

		fzclose(h, &gz);
	}

	exit(0);
}
*/


