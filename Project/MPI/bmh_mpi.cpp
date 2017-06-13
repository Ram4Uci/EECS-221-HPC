
#include <iostream>
#include <vector>
#include <stdio.h>
#include <climits>
#include <cstdlib>
#include <cmath>
#include <algorithm> // for the transform function
#include <fstream>  // for inputting the text file
#include <string.h> // for string functions
#include <mpi.h>
using namespace std;

typedef vector<size_t> bad_match_table;

const bad_match_table create_table(const unsigned char* str, size_t str_len)
{
	bad_match_table tab(UCHAR_MAX+1, str_len);
	
	if(str_len >=1)
	{
		for( size_t i=0; i<str_len-1 ; ++i)
			tab[str[i]] = str_len-1-i;
	
	}
	
	return tab;
}

int boyer_moore_horsepool_sequential(char* text, size_t txt_len, const char* str, size_t str_len, bad_match_table tab1)
{
	size_t text_pos =0;
	unsigned char occ_char;	
	int count=0,temp,tid;
	int offset = str_len-1;
	while(text_pos<=txt_len)
	{
		occ_char = text[text_pos + str_len -1];
		if(occ_char == str[str_len-1] && (memcmp(str, text+text_pos, str_len - 1) == 0) )
		{
		    ++count;
		}
		text_pos+=tab1[occ_char];
	}

	return count;
}

int main(int argc,char *argv[])
{
	double start = omp_get_wtime();
	int count=0,num=8;
	bad_match_table tab1;
	double mid;
	const char* str;
	const char* filename;
	string file_str,  temp_str;
	
	unsigned char occ_char;
	
	int size,rank;

	filename = argv[1];
	str = argv[2];
	num =atoi(argv[3]);
	cout<<"Finding string '"<<str<<"' in text file "<<filename<<endl;
	//Create BAD MATCH TABLE

 	size_t str_len = strlen(str);
	tab1 = create_table(reinterpret_cast <const unsigned char*> (str),str_len);

	
	MPI_Init(&argc,&argv);

	MPI_File file;
	MPI_Status status;
	MPI_Offset filesize,blocksize,start;

	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	MPI_Comm_size(MPI_COMM_WORLD,&size);
	MPI_File_open(MPI_COMM_SELF,filename,MPI_MODE_RDONLY,MPI_INFO_NULL,&file);
	MPI_File_get_size(file, &filesize);

	char* text;
	int temp_count;

    blocksize = filesize/size + str_len -1;
    start = rank * blocksize;
   
    text = (char*)malloc((blocksize + 1)*sizeof(char));

    MPI_File_read_at(file,start,text,blocksize,MPI_CHAR,MPI_STATUS_IGNORE);

    MPI_File_close(&file);

    temp_count = boyer_moore_horsepool_sequential(text,blocksize,str,str_len,tab1);

    MPI_Reduce(&temp_count,&count,1,MPI_INT,MPI_SUM,0,MPI_COMM_WORLD);

    if(rank == 0){

	cout<<"Total number of occurances of string in text = "<<final_count<<endl;

	cout<<"Time taken = "<<MPI_Wtime()-start_time<<endl;

	}

	MPI_Finalize();

	return 0;
}




