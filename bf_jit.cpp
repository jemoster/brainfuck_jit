//Brainfuck interpretter
//
#include <iostream>
#include <vector>
#include <stack>

using std::cin;
using std::cout;

enum cmds {move, change, sLoop, eLoop, input, output, setZero, end};

struct inst {
    char cmd;
    int param;
} ;

bool readInstruction(char x, inst &tmpInst){
     if(x=='>') {
            tmpInst.cmd = move;
            tmpInst.param = 1;
            return true;
     } else if (x=='<') {
            tmpInst.cmd = move;
            tmpInst.param = -1;
            return true;
     } else if (x=='+') {
            tmpInst.cmd = change;
            tmpInst.param = 1;
            return true;
     } else if (x=='-') {
            tmpInst.cmd = change;
            tmpInst.param = -1;
            return true;
     } else if (x=='[') {
            tmpInst.cmd = sLoop;
            return true;
     } else if (x==']') {
            tmpInst.cmd = eLoop;
            return true;
     } else if (x=='.') {
            tmpInst.cmd = output;
            return true;
     } else if (x==',') {
            tmpInst.cmd = input;
            return true;
     }
     return false;
}
 
void optimizeSetZero(std::vector<inst> program){
    for(int i=0; i<program.size(); i++){
        if(program[i].cmd == sLoop & program[i+2].cmd == eLoop){
            if(program[i+1].cmd == change & program[i+1].param == -1){
                program[i].cmd = setZero;
                program.erase(program.begin()+i+1,program.begin()+i+2);
            }
        }
    }
    return;
}

int main() {
    char dat[30000];
    char *ptr=dat;
    static void *jmp[] = { &&iMove, &&iChange , &&iSLoop, &&iELoop, &&iInput, &&iOutput, &&iSetZero, &&iEnd};
    std::vector<inst> program;

    char x;
    //Load first valid instruction
    inst firstInst;
    while(cin>>x){
        if(readInstruction(x, firstInst)){
            program.push_back(firstInst);
            break;
        }
    }

    while(cin >> x){
        inst tmpInst;
        if(readInstruction(x, tmpInst)){
            //Merge repeated commands
            if(tmpInst.cmd == program.back().cmd && (tmpInst.cmd==move || tmpInst.cmd==change)){
                program.back().param += tmpInst.param;
            } else {
                program.push_back(tmpInst);
            }
        }
    }

    {
        inst tmpInst;
        tmpInst.cmd = end;
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

    optimizeSetZero(program);

    int i=0;
    goto *jmp[program[i].cmd];
    
    iMove: 
    ptr = ptr+program[i].param;
    i++;
    goto *jmp[program[i].cmd];
    
    iChange: 
    *ptr = *ptr+program[i].param;
    i++;
    goto *jmp[program[i].cmd];
    
    iOutput : 
    cout << *ptr;
    i++;
    goto *jmp[program[i].cmd];
    
    iInput : 
    cin >> *ptr;
    i++;
    goto *jmp[program[i].cmd];
    
    iSLoop :
    if(*ptr==0) i = program[i].param;
    i++;
    goto *jmp[program[i].cmd];
    
    iELoop :
    if(*ptr!=0) i = program[i].param;
    i++;
    goto *jmp[program[i].cmd];

    iSetZero:
    *ptr=0;
    i++;
    goto *jmp[program[i].cmd];
    
    iEnd:
    return 0;
}
