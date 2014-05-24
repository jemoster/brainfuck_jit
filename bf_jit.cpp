//Brainfuck interpretter
//
#include <iostream>
#include <vector>
#include <stack>

using std::cin;
using std::cout;

enum cmds {move, change, sLoop, eLoop, input, output};

struct inst {
    char cmd;
    int param;
} ;
 
int main() {
    char dat[30000];
    char *ptr=dat;
    char x;

    std::vector<inst> program;

    while(cin >> x){
         inst tmpInst;
         if(x=='>') {
                tmpInst.cmd = move;
                tmpInst.param = 1;
         } else if (x=='<') {
                tmpInst.cmd = move;
                tmpInst.param = -1;
         } else if (x=='+') {
                tmpInst.cmd = change;
                tmpInst.param = 1;
         } else if (x=='-') {
                tmpInst.cmd = change;
                tmpInst.param = -1;
         } else if (x=='[') {
                tmpInst.cmd = sLoop;
         } else if (x==']') {
                tmpInst.cmd = eLoop;
         } else if (x=='.') {
                tmpInst.cmd = output;
         } else if (x==',') {
                tmpInst.cmd = input;
         }
         program.push_back(tmpInst);
    }

    //Link Loops
    for( int j=0; j<program.size(); j++){
        if(program[j].cmd == sLoop){
            int k=j+1;
            int bCounter = 0;
            while(!(program[k].cmd == eLoop && bCounter==0)){
                if(program[k].cmd == sLoop) bCounter++;
                if(program[k].cmd == eLoop) bCounter--;
                k++;
                if(k>=program.size()){
                    cout << "Loop linking failed";
                }
            }
            program[k].param = j;
            program[j].param = k;
        }

    }


    int i=0;
    while(i<program.size()){
        switch(program[i].cmd) {
            case move : 
                ptr = ptr+program[i].param;
                break;
            case change : 
                *ptr = *ptr+program[i].param;
                break;
            case output : 
                cout << *ptr;
                break;
            case input : 
                cin >> *ptr;
                break;
            case sLoop :
                if(*ptr==0) i = program[i].param;
                break;
            case eLoop :
                if(*ptr!=0) i = program[i].param;
                break;
            default :
                cout << "ALL FUCKED UP!";
                break;
        }
        i++;
    }
    return 0;
}
