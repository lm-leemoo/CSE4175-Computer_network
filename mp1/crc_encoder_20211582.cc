#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>
#include <vector>
#include <bitset>
#include <sstream>

using namespace std;

char XOR(char a, char b){
    if (a != b) return '1';
    else return '0';
}

string modulo(string generator, string divided, int generator_size){
    for (int i = 0; i <= divided.size()- generator_size; i++) {
        if (divided[i] == '1') {
            for (int j = i; j < generator_size + i; j++) {
                 divided[j] = XOR(divided[j], generator[j-i]);
            }
        }
    }
    string remain = "";
    for (int i = divided.size()- generator_size +1; i < divided.size(); i++) {
        remain += divided[i];
    }
    return remain;
}

string make_codeword(vector <char> &array, string generator, int dataword_size){
    string zero = "";
    for(int i = 0; i < generator.size() - 1; i++) zero += "0";
    string codeword = "";
    string cur = "";
    for(int i = 0; i < array.size(); i += dataword_size){
        for(int j = i; j < i + dataword_size; j++){
            cur += array[j];
        }
        cur += modulo(generator, cur + zero, generator.size()); 
        codeword += cur;
        cur = "";
    }
    return codeword;
}

int main(int argc, char* argv[]){
    if (argc != 5) {//인자 수 안맞음
        cout << "usage: ./crc_encoder input_file output_file generator dataword_size\n";
        exit(1);
    }
    //input_file error
    ifstream dataword;
    dataword.open(argv[1], ios::binary);
    if (!dataword.is_open()) {
        cout << "input file open error.\n";
        exit(1);
    }
    //output_file error
    ofstream write;
    write.open(argv[2], ios::binary);
    if (!write.is_open()) {
        cout << "output file open error.\n";
        exit(1);
    }
    //data size error
    int dataword_size = atoi(argv[4]);
    if (!(dataword_size == 4 || dataword_size == 8)){
        cout << "dataword size must be 4 or 8.\n";
        exit(1);
    }

    //dataword크기로 나누기
    int sizenumber = 1;
    for (int m = 0 ; m < dataword_size; m++) {
        sizenumber *= 2;
    }
    vector<char> array;
    while(1){
        char word = dataword.get();
        int number = (int) word; //숫자
        
        if (word == EOF) break;

        int initial = 1 << 7;
        while(initial){
            if(number >= initial){
                number %= initial;
                array.push_back('1');
            }
            else {
                array.push_back('0');
            }
            initial /= 2;
        }
    }
    // //dataword를 codeword로 바꾸기
    string codeword = make_codeword(array, argv[3], dataword_size);

    //패딩비트 추가 8로 나누어떨어지는가?
    int padding_num = 8 - (codeword.size() % 8);
    string padding = "";
    for (int i = 0 ; i < padding_num; i++) {
        padding += "0";
    }
    codeword = padding + codeword;
    
    string paddingsize = "";
    int initial = 1 << 7;
    while(initial){
        if(padding_num >= initial){
            padding_num %= initial;
            paddingsize += "1";
        }
        else paddingsize += "0";
        initial /= 2;
    }
    codeword = paddingsize + codeword;

    vector<string> binaryarray;
    for (int i = 0; i < codeword.size(); i +=8) {
        string a = "";
        for (int j = i; j < i + 8; j++) {
            a += codeword[j];
        }
        binaryarray.push_back(a);
    }
    //outputfile에 적기
    for (int i = 0; i < binaryarray.size(); i++) {
        string ch;
        stringstream ss(binaryarray[i]);
        bitset<8> bs;
        ss >> bs;
        ch = char(bs.to_ulong());
        write << ch;
    }
    write.close();
    return 0;
}
