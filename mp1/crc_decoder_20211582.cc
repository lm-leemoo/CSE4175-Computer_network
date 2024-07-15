#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>
#include <vector>
#include <bitset>
#include <sstream>
#include <cstring>

using namespace std;
int total =0;
int error =0;
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

string make_dataword(string array, string generator, int codeword_size){
    string cur = "";
    string remain = "";
    string dataword = "";
    string test = "";
    for (int i = 0 ; i < generator.size()-1; i++) test += "0";

    for(int i = 0; i < array.size(); i += codeword_size){
        for(int j = i; j < i + codeword_size; j++){
            cur += array[j];
        }
        remain = modulo(generator, cur, generator.size());
        total++;
        if (remain != test) error++; 
        for (int j = i; j <= i + codeword_size - generator.size(); j++) {
            dataword += array[j];
        }
        cur = "";
    }
    return dataword;
}

int main(int argc, char* argv[]){
    if (argc != 6) {//인자 수 안맞음
        cout << "usage: ./crc_decoder input_file output_file result_file generator dataword_size\n";
        exit(1);
    }
    //input_file error
    ifstream input;
    input.open(argv[1], ios::binary);
    if (!input.is_open()) {
        cout << "input file open error.\n";
        exit(1);
    }
    //output_file error
    ofstream output;
    output.open(argv[2], ios::binary);
    if (!output.is_open()) {
        cout << "output file open error.\n";
        exit(1);
    }
    //result_file error
    ofstream result;
    result.open(argv[3]);
    if (!result.is_open()) {
        cout << "result file open error.\n";
        exit(1);
    }
    //data size error
    int dataword_size = atoi(argv[5]);
    if (!(dataword_size == 4 || dataword_size == 8)){
        cout << "dataword size must be 4 or 8.\n";
        exit(1);
    }
    //binarydata 배열에 저장
    vector<string> binaryarray;
    char c;
    while(1){
        c = input.get();
        if (input.fail()) break;
        bitset<8> bit(static_cast<unsigned char>(c));
        binaryarray.push_back(bit.to_string());
    }
    input.close();

    //padding제거
    string padding = binaryarray[0];
    int initial = 1 << 7;
    int padding_num = 0;
    for (int i = 0 ; i < padding.size(); i++) {
        padding_num += (padding[i] -'0') * initial;
        initial /= 2;
    }

    int generator_size = strlen(argv[4]);
    int codeword_size = generator_size + dataword_size -1;

    string binary;
    for (int i = 1 ; i < binaryarray.size(); i++) {
        for (int j = 0; j < binaryarray[i].size(); j++){
            if ((i != 1) || (j >= padding_num))
                binary += binaryarray[i][j];
        }
    }

    string dataword = make_dataword(binary, argv[4], codeword_size);

    //result파일에 쓰기
    result << total << ' ' << error << "\n";

    //output파일에 쓰기
    for (int i = 0; i < dataword.size(); i+=8) {
        string ch;
        stringstream ss(dataword.substr(i,8));
        bitset<8> bs;
        ss >> bs;
        ch = char(bs.to_ulong());
        output << ch;
    }

    result.close();
    output.close();
    return 0;
}
