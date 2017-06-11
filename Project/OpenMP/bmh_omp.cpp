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
	double mid;
	const char* str;
	const char* filename;
	string file_str,  temp_str;
	char* text;
	unsigned char occ_char;
	
	filename = argv[1];
	str = argv[2];
	num =atoi(argv[3]);
	cout<<"Finding string '"<<str<<"' in text file "<<filename<<endl;
	//Create BAD MATCH TABLE

 	size_t str_len = strlen(str);
	tab1 = create_table(reinterpret_cast <const unsigned char*> (str),str_len);
	
	//READ FROM THE FILE AND PERFORM ALGORITHM PARALLELLY

	FILE *ins;
 	ins = fopen(filename,"rb");
 	fseek(ins,0,SEEK_END);
 	const unsigned int size = ftell(ins)/num;

 	cout<<"Length of text to be scanned is "<<size<<" and length of pattern is "<<str_len<<endl;
 	fclose(ins);
 	
 	
 	#pragma omp parallel shared(str,str_len,tab1) num_threads(num)
 	{
	        FILE *in;
	        in = fopen(filename,"r");
	        int tid = omp_get_thread_num();
 		string text;
		char buf[size];  
 		int temp = 0;
 		fseek(in,(size*tid)+str_len-1,SEEK_SET);
 		fgets(buf,size,in);
		text = buf;
		int rem = size - text.length();
		int pos = ftell(in);
		while((pos<=size*(tid+1))&&rem>0)
		  {
		    fgets(buf,size-text.length(),in);
		    text+=buf;
		    rem = size - text.length();
		    pos = ftell(in);
		  }
 		size_t text_len = text.length();
 		fclose(in);
		transform(text.begin(),text.end(),text.begin(), ::tolower);
		temp = boyer_moore_horsepool_sequential(&text[0],text_len,str,str_len,tab1);
		#pragma omp reduce
		count+=temp;
	}

		
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
