
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

typedef vector<size_t> occtable_type; // table type for occurrence table

const occtable_type create_table(const unsigned char* str , size_t str_length )
{
	occtable_type occ(UCHAR_MAX+1,str_length);

	if(str_length >= 1)
	{
		for( size_t i=0; i<(str_length-1); ++i)
			occ[str[i]] = (str_length -1)-i;
	}
	
	return occ;
}


int main(int argc,char *argv[])
{
double start_time = MPI_Wtime();
double mid_time=0;
occtable_type occ1;
const char* str;
char* filename;



unsigned char occ_char;	
size_t text_pos =0;

int temp_count =0,rank, size, type=99 ,final_count =0;
char *block;
char ***lines;
int *nlines;
int overlap;


 filename = argv[1];
 str = argv[2];
 size_t str_len = strlen(str);

 FILE *ins;

 ins = fopen(filename,"rb");

 fseek(ins,0,SEEK_END);



 fclose(ins);

// We add an overlap to ensure that splitting the string does not cause incorrect results. 
 overlap = atoi(argv[3]);
 
 	//Calling function to generate the occurrence table . We have to reinterpret_cast in
	// order to match the arguments of func. call and func. declaration.
	occ1 = create_table(reinterpret_cast <const unsigned char*> (str),str_len);

//cout<<"Finding string '"<<str<<"' in text file "<<filename<<endl;

	MPI_Status status;	

	
	//Initialize MPI environment.
	MPI_Init(&argc, &argv);
	
	//Getting number of processes.
	MPI_Comm_size(MPI_COMM_WORLD,&size);
	//cout<<"Size = "<<size;
	//Getting rank of process.
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);

	FILE *in;

	in = fopen(filename,"r");

	const unsigned int filesize = ftell(ins)/size;
	MPI_Offset blocksize;
	MPI_Offset start;
	MPI_Offset end;
	MPI_Request request;

    
    // Calculating length of text and determining how much text should be given to each
    // node.
    
  
	blocksize = filesize/size + str_len-1;
	
    // Creating pointers to start and end of block
	start = rank * blocksize ;
	string itext;
	char *text;
	char buf[blocksize];  
	int temp = 0;
	fseek(in,start,SEEK_SET);
	fgets(buf,blocksize,in);
	itext = buf;
	int rem = size - itext.length();
	int pos = ftell(in);
	while((pos<=blocksize*(rank+1))&&rem>0)
	  {
	    fgets(buf,blocksize-itext.length(),in);
	    itext+=buf;
	    rem = blocksize - itext.length();
	    pos = ftell(in);
	  }
	size_t text_len = itext.length();
	fclose(in);
	transform(itext.begin(),itext.end(),itext.begin(), ::tolower);
	text = &itext[0];
 	if(rank == 0){  
	  mid_time = MPI_Wtime();
	  cout<<"String '"<<str<<"' is being searched in file '"<<filename<<"'"<<endl<<"Mid time = \n"<<mid_time-start_time;
    }else
	  {
	    cout<<"Process "<<rank<<" takes time "<<MPI_Wtime()-start_time<<endl;
	  }
    
	//cout<<"Length of file "<<filesize<<" is divided into blocksize "<<blocksize<<" for node "<<rank<<endl;
  
	// Making pointer point to start of block
	while(text_pos <= (blocksize - str_len ))	
	  {
		// Selecting charachter at position equal to pattern length . -1 is to nullify
		// the 0th element.
		occ_char = text[text_pos + str_len -1];
		
		// If last charachter of pattern matches curent character in text and 
		// if charachters ahead of current in text contains pattern  , we have a match.
		// memcmp compares charachters ahead of current with the pattern in blocks of
		// str_len -1.
		 
		if (occ_char == str[str_len-1] && (memcmp(str, text+text_pos, str_len - 1) == 0))
		{
		  temp_count++;
		}
		
		// Look at occurrence table and decide how to increment text pointer
		text_pos += occ1[occ_char];
		
}

// Adding the tempsum of each thread to the total sum using mpi_reduce

MPI_Reduce(&temp_count,&final_count,1,MPI_INT,MPI_SUM,0,MPI_COMM_WORLD);

if(rank == 0){
	cout<<"Total number of occurances of string in text = "<<final_count<<endl;
	cout<<"Time taken = "<<MPI_Wtime()-mid_time<<endl;
}



MPI_Finalize();

return 0;

}




