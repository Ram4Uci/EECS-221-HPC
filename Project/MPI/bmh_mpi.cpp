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

//Defining Bad_Match_Table
typedef vector<size_t> bad_match_table;

//Creating Bad_Match_Table
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

//Boyer Moore Horspool Algorith Implementation
int boyer_moore_horspool(char* text, size_t txt_len, const char* str, size_t str_len, bad_match_table tab1)
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
  double start_time = MPI_Wtime();
  int count=0,num=8;
  bad_match_table tab1;
  double reduce_time,algo_time;
  const char* str;
  const char* filename;
  string file_str,  temp_str;
  
  unsigned char occ_char;
  
  int size,rank;

  filename = argv[1];
  str = argv[2];
  
  size_t str_len = strlen(str);

  //Create BAD MATCH TABLE

  tab1 = create_table(reinterpret_cast <const unsigned char*> (str),str_len);
  
  MPI_Init(&argc,&argv);

  MPI_File file;
  MPI_Request status;
  MPI_Offset filesize,blocksize,start;

  MPI_Comm_rank(MPI_COMM_WORLD,&rank);
  MPI_Comm_size(MPI_COMM_WORLD,&size);

  //Open MPI File for Self with a separate File handle

  MPI_File_open(MPI_COMM_SELF,filename,MPI_MODE_RDONLY,MPI_INFO_NULL,&file);
  MPI_File_get_size(file, &filesize);

  char* text;
  int temp_count;
  
  //Calculate the Offset position from which the file must read

  blocksize = filesize/size + str_len -1;
  start = (MPI_Offset)rank * (blocksize-str_len+1);
  text = (char*)malloc((blocksize + 1)*sizeof(char)); //Allocate Memory

  MPI_File_iread_at(file,start,text,blocksize,MPI_CHAR,&status); //Non-Blocking Read

  if(rank ==0)
  {
    algo_time = MPI_Wtime();
    cout<<"Read time = "<<algo_time - start_time<<endl;
  }

  temp_count = boyer_moore_horspool(text,blocksize,str,str_len,tab1);
  if(rank ==0)
  {
    reduce_time = MPI_Wtime();
    cout<<"Time for algorithm = "<<reduce_time-algo_time<<endl;
  }

  //Reduce operation to collect all the counts
  MPI_Reduce(&temp_count,&count,1,MPI_INT,MPI_SUM,0,MPI_COMM_WORLD);

  MPI_File_close(&file);

  if(rank == 0)
    {
      cout<<"Finding string '"<<str<<"' in text file "<<filename<<endl;
      cout<<"Total number of occurances of string in text = "<<count<<endl;
      cout<<"Reduce time = "<<MPI_Wtime()-reduce_time<<endl;
      cout<<"Time taken = "<<MPI_Wtime()-start_time<<endl;
    }

  MPI_Finalize();

  return 0;
}




