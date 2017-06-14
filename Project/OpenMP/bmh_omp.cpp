#include <iostream>
#include<omp.h>
#include<string.h>
#include<algorithm>
#include<fstream>
#include<limits.h>
#include<stdlib.h>
#include<vector>

using namespace std;

//Defining the Bad_Match_Table

typedef vector<size_t> bad_match_table;

// Create Bad_Match_Table 

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

//Boyer Moore Horspool Algorithm

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

int main(int argc, char *argv[])
{
       	double start = omp_get_wtime();
	int count=0,num=8;
	bad_match_table tab1;
	
	const char* str;
	const char* filename;
	string file_str,  temp_str;
	char* text;
	unsigned char occ_char;
	
	filename = argv[1];
	str = argv[2];
	num =atoi(argv[3]);
	cout<<"Finding string '"<<str<<"' in text file "<<filename<<endl;
 	size_t str_len = strlen(str);
	
	//Create BAD MATCH TABLE
	tab1 = create_table(reinterpret_cast <const unsigned char*> (str),str_len);
	
	//READ FROM THE FILE AND PERFORM ALGORITHM PARALLELLY

	FILE *ins;
 	ins = fopen(filename,"rb");
 	fseek(ins,0,SEEK_END);
 	const unsigned int blocksize = ftell(ins)/num ;
	const unsigned int size = blocksize + str_len -1;

 	cout<<"Length of text to be scanned is "<<size<<" and length of pattern is "<<str_len<<" Threads "<<num<<endl;
 	fclose(ins);
 	 	
        #pragma omp parallel shared(str,str_len,tab1) num_threads(num)
 	{
	        FILE *in;
	        in = fopen(filename,"r");
	        int tid = omp_get_thread_num();
 		string text;
		char *buf;
		buf = (char*)malloc((size+1)*sizeof(char));
 		int temp = 0;
 		fseek(in,(blocksize*tid),SEEK_SET);
 		fgets(buf,size,in);
		
		//Fgets() returns on a '\n'. So repeat Fgets() till size of text is blocksize
		
		text = buf;
		int rem = size - text.length();
		int pos = ftell(in);
		while((pos<blocksize*(tid+1))&&rem>0)
		{
		    fgets(buf,size-text.length(),in);
		    text+=buf;
		    rem = size - text.length();
		    pos = ftell(in);
		}
 		size_t text_len = text.length();
 		fclose(in);
		transform(text.begin(),text.end(),text.begin(), ::tolower);
		temp = boyer_moore_horspool(&text[0],text_len,str,str_len,tab1);
                #pragma omp reduce // Sum all the temp count's
		count+=temp;
	}

	if(count == 0)
	    cout<< " No match found " <<endl;
	else
	  {	
	    cout<<"Total number of occurances of string in text = "<<count<<endl;
	  }
	cout<<"\nTimeTaken = "<<omp_get_wtime()-start<<"\n";
	return 0;
}
