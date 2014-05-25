//Brainfuck interpretter
//
#include <iostream>
#include <vector>
#include <stack>

using std::cin;
using std::cout;
using std::endl;

enum cmds {move, change, sLoop, eLoop, input, output, setZero, inc, add, end};

struct inst {
    char cmd;
    int param;
    int param2;
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
 
void optimizeSetZero(std::vector<inst> &program){
    for(int i=0; i<program.size(); i++){
        if(program[i].cmd == sLoop & program[i+2].cmd == eLoop){
            if(program[i+1].cmd == change & program[i+1].param == -1){
                program[i].cmd = setZero;
                program.erase(program.begin()+i+1,program.begin()+i+2+1);
            }
        }
    }
    return;
}

void optimizeAdd(std::vector<inst> &program){
    for(int i=0; i<program.size(); i++){
        if(program[i].cmd == sLoop){
            for(int j=1; j<program.size(); j++){
                if(program[i+j].cmd == eLoop){
                    
                    std::vector<inst> truncated;

                    //scan root loop for side-effects
                    bool isValid = true;
                    bool hasDec = false;
                    for(int k=1; k<j; k++){
                        if(program[i+k].cmd==change){
                            if(program[i+k].param!=-1){
                                isValid=false;
                            } else {
                                hasDec = true;
                            }
                        } else if (program[i+k].cmd==inc) {
                            if(!(program[i+k].param2==1 || program[i+k].param2==-1)){
                                isValid=false;
                            } else {
                                truncated.push_back(program[i+k]);
                            }
                        } else {
                            isValid=false;
                        }
                    }
                    if(isValid && hasDec){
                        for(int k=0; k<truncated.size(); k++){
                            program[i+k].cmd = add;
                            program[i+k].param = truncated[k].param;
                        }
                        program[i+truncated.size()].cmd = setZero;
                        program.erase(program.begin()+i+truncated.size()+1, program.begin()+i+j+1);
                    }
                } else if(program[i+j].cmd == sLoop)j=program.size();
            }
            //scan:
        }
    }
    return;
}

void optimizeInc(std::vector<inst> &program){
    for(int i=0; i<program.size(); i++){
        if(program[i].cmd==move & program[i+1].cmd==change & program[i+2].cmd==move & program[i].param==program[i+2].param){
            program[i].cmd = inc;
            program[i].param = program[i].param;
            program[i].param2 = program[i+1].param;
            program.erase(program.begin()+i+1,program.begin()+i+2+1);
        }
    }
    return;
}

void linkLoops(std::vector<inst> &program){
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
}
         
void printProgram(const std::vector<inst> program){
    int k = 0;
    for(int j=0; j<program.size(); j++){
        cout<<(j+k)<<": ";
        switch(program[j].cmd){
            case 0: cout<<"move"; break;
            case 1: cout<<"change"; break;
            case 2: cout<<"sLoop"; break;
            case 3: cout<<"eLoop"; break;
            case 4: cout<<"input"; break;
            case 5: cout<<"output"; break;
            case 6: cout<<"setZero"; k+=2; break;
            case 7: cout<<"inc"; break;
            case 8: cout<<"add"; break;
            case 9: cout<<"end"; break;
            default: cout<<"unknown"; break;
        }
        cout<<",\t"<<program[j].param<<",\t"<<program[j].param2<<endl;
    }
}
void execute(const std::vector<inst> program, char* ptr){
    static void *jmp[] = { &&iMove, &&iChange , &&iSLoop, &&iELoop, &&iInput, &&iOutput, &&iSetZero, &&iInc, &&iAdd, &&iEnd};
    int i=0;
    //goto iEnd;
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

    iAdd:
    *(ptr+program[i].param) += *ptr;
    i++;
    goto *jmp[program[i].cmd];
    
    iInc:
    *(ptr+program[i].param) = *(ptr+program[i].param) + program[i].param2;
    i++;
    goto *jmp[program[i].cmd];

    iEnd:
    return;
}

int main() {
    char dat[30000];
    char *ptr=dat;
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

    
    optimizeSetZero(program);
    //optimizeInc(program);
    //optimizeAdd(program);
    linkLoops(program);

    execute(program, ptr);
}
