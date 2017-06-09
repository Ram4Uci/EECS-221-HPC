#include <iostream>
#include<omp.h>
#include<string.h>
#include<algorithm>
#include<fstream>
#include<limits.h>
#include<stdlib.h>
#include<vector>

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
	while(text_pos<=text_len)
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

int main(int argc, char *argv[])
{
        
	int count=0,num=8;
	bad_match_table tab1;
	const char* str;
	const char* filename;
	string file_str, temp_str;
	char* text;
	unsigned char occ_char;
	
	filename = argv[1];
	str = argv[2];
	num =atoi(argv[3]);
	cout<<"Finding string '"<<str<<"' in text file "<<filename<<endl;
	//Create BAD MATCH TABLE
	double start = omp_get_wtime();
	tab1 = create_table(reinterpret_cast <const unsigned char*> (str),str_len);
	
	//READ FROM THE FILE AND PERFORM ALGORITHM PARALLELLY

	FILE *in;
 	in = fopen(filename,"rb");
 	fseek(in,0,SEEK_END);
 	const unsigned int size = ftell(in)/num;
 	size_t str_len = strlen(str);
 	cout<<"Length of text to be scanned is "<<(size<<6)<<" and length of pattern is "<<str_len<<endl;
 	fclose(in);
 	in = fopen(filename,"r");
 	
 	#pragma omp parallel shared(in,str,str_len,tab1) num_threads(num)
 	{
 		int tid = omp_get_thread_num();
 		char text[size];
 		int temp = 0;
 		fseek(in,size*tid,SEEK_SET);
 		fgets(text,size,in);
 		size_t text_len = strlen(text);
 		#pragma omp barrier;
 		#pragma omp single
 		{
 		fclose(in);
		double mid = omp_get_wtime();
		cout<<"Pre processing time = "<< mid-start<<"\n";
		}
		temp = boyer_moore_horsepool_sequential(text,text_len,str,str_len,tab1);
		#pragma omp atomic
		count+=temp;
	}
		cout<<"\nAlgorithm TimeTaken = "<<omp_get_wtime()-mid<<"\n";
		
//Calling function to generate the occurrence table . We have to reinterpret_cast in
// order to match the arguments of func. call and func. declaration.

if(count == 0)
cout<< " No match found and error in handling the text" <<endl;
else
{	
  //int match = sizeof(count)/sizeof(int);
	cout<<"Total number of occurances of string in text = "<<count<<endl;
	cout<<"\nTimeTaken = "<<omp_get_wtime()-start<<"\n";
	/*	for(int i=0;i<match;i++)
	{
	  cout<<count[i]<<",";
	  }*/
	cout<<endl;
}
return 0;
}
