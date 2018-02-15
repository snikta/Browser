/* "readTextFile.cpp" */
 
#include <iostream>
#include <fstream>
#include <string.h>
#include <stdio.h>
 
using namespace std;
 
int getFileLength(string filename, ifstream *is) {
    is->seekg(0, is->end);
    int len = is->tellg();
	is->seekg(0, is->beg);
   
    return len;
}
 
char *readTextFile(string fname, int len, ifstream *is, char *ptrSrc) {
    int i = 0;
	char chr;
    while ((chr = is->get()) != EOF) {
		ptrSrc[i++] = chr;
	};
	ptrSrc[i] = '\0';
	is->close();
   
    return ptrSrc;
}
 
 
void sayMyName(char *ptrName) {
    strcpy(ptrName, "Walt Heisenberg");
}

int main() {
    string fname = "the.txt";
	ifstream *is = new ifstream(fname);
	int len = getFileLength(fname, is);
    char src[len];
    char *ptrSrc = src;
	
	cout << "Length: " << len;
   
    cout << readTextFile(fname, len, is, ptrSrc) << endl;
    sayMyName(ptrSrc);
    cout << ptrSrc;
   
    return 0;
}