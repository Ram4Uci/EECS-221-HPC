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
	int count=0;
	while(text_pos<=(txt_len-str_len))
	{
		occ_char = text[text_pos + str_len -1];
		
		if(occ_char == str[str_len-1] && (memcmp(str, text+text_pos, str_len - 1) == 0) )
		{
		  ++count;
		  //  cout<<text_pos<<"\n";
		}

		text_pos+=tab1[occ_char];
	}
	return count;
}

int main(int argc, char *argv[])
{
  double start = omp_get_wtime();
	int count;
	bad_match_table tab1;
	const char* str;
	const char* filename;
	string file_str, temp_str;
	char* text;
	unsigned char occ_char;
	size_t text_pos = 0;
	
	filename = argv[1];
	str = argv[2];
	cout<<"Finding string '"<<str<<"' in text file "<<filename<<endl;

 ifstream in;
 in.open(filename);
 while (!in.eof())
 {
    //getting one line in the file 
 	getline(in,temp_str);
 	
 	//Converting the line into lower case to avoid case sensitive search
 	transform(temp_str.begin(),temp_str.end(),temp_str.begin(), ::tolower);
 	
 	//Appending line to final string of txt file
 	file_str.append(temp_str);
 }
 in.close();

// Making char pointer point to first element in string. 
text = &file_str[0];

size_t str_len = strlen(str);
size_t text_len = strlen(text);
cout<<"Length of text to be scanned is "<<text_len<<" and length of pattern is "<<str_len<<endl;

//Calling function to generate the occurrence table . We have to reinterpret_cast in
// order to match the arguments of func. call and func. declaration.
tab1 = create_table(reinterpret_cast <const unsigned char*> (str),str_len);

count = boyer_moore_horsepool_sequential(text,text_len,str,str_len,tab1);

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
