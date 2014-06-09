//Brainfuck interpretter
//
#include <iostream>
#include <vector>
#include <stack>
#include <string>

using std::cin;
using std::cout;
using std::endl;

enum cmds {move, change, sLoop, eLoop, input, output, setVal, inc, add, end};

struct inst {
    char cmd;
    int param;
    int param2;
} ;

void printSubset(const std::vector<inst> &program, int start, int end);

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
            case 6: cout<<"setVal"; break;
            case 7: cout<<"inc"; break;
            case 8: cout<<"add"; break;
            case 9: cout<<"end"; break;
            default: cout<<"unknown"; break;
        }
        cout<<",\t"<<program[j].param<<",\t"<<program[j].param2<<endl;
    }
}

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
                program[i].cmd = setVal;
                program[i].param = 0;
                program[i].param2 = 0;
                program.erase(program.begin()+i+1,program.begin()+i+2+1);
            }
        }
    }
    return;
}

void optimizeAdd(std::vector<inst> &program){
    for(int i=0; i<program.size(); i++){
        if(program[i].cmd == sLoop){
            for(int j=1; i+j<program.size(); j++){
                //Stop if this isn't the inner most loop
                if(program[i+j].cmd == sLoop) break;

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
                                if(hasDec) break;
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
                            program[i+k].param2 = truncated[k].param2;
                        }
                        program[i+truncated.size()].cmd = setVal;
                        program[i+truncated.size()].param= 0;
                        program[i+truncated.size()].param2= 0;
                        program.erase(program.begin()+i+truncated.size()+1, program.begin()+i+j+1);
                    }
                }
            }
        }
    }
    return;
}

void printSubset(const std::vector<inst> &program, int start, int end){
    if(start>end || end>program.size() || start<0){
        return;
    }
    std::vector<inst> tmpLoop;
    for(int l=start; l<=end; l++){
        tmpLoop.push_back(program[l]);
    }
    printProgram(tmpLoop);
}

        

void optimizeInc(std::vector<inst> &program){
    for(int i=0; i<program.size(); i++){
        if(program[i].cmd==move & program[i+1].cmd==change & program[i+2].cmd==move){
            //Truncate
            program[i].cmd = inc;
            program[i].param = program[i].param;
            program[i].param2 = program[i+1].param;
            program.erase(program.begin()+i+1,program.begin()+i+2);
            
            //Account for deficit
            if(program[i+1].param+program[i].param != 0){
                program[i+1].param += program[i].param;
            } else {
                //Remove 0 distance moves
                program.erase(program.begin()+i+1,program.begin()+i+2);
            }
        }
    }
    return;
}

void optimizeSetValue(std::vector<inst> &program){
    for(int i=0; i<program.size(); i++){
        if(program[i].cmd==setVal & program[i+1].cmd==change){
            //Truncate
            program[i].cmd = setVal;
            program[i].param += program[i+1].param;
            program[i].param2 = 0;
            program.erase(program.begin()+i+1,program.begin()+i+2);
        }
    }
    for(int i=0; i<program.size(); i++){
        if(program[i].cmd==move & program[i+1].cmd==setVal & program[i+2].cmd==move){
            //Truncate
            program[i].cmd = setVal;
            program[i].param2 = program[i].param;
            program[i].param = program[i+1].param;
            program.erase(program.begin()+i+1,program.begin()+i+2);
            
            //Account for deficit
            if(program[i+1].param+program[i].param2 != 0){
                program[i+1].param += program[i].param2;
            } else {
                //Remove 0 distance moves
                program.erase(program.begin()+i+1,program.begin()+i+2);
            }
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
            if(j==k) cout << "WAT?";
            program[k].param = j;
            program[j].param = k;
        }

    }
}
         
int execute(const std::vector<inst> program, char* ptr, char dat[]){
    static void *jmp[] = { &&iMove, &&iChange , &&iSLoop, &&iELoop, &&iInput, &&iOutput, &&iSetVal, &&iInc, &&iAdd, &&iEnd};
    int i=0;
    //goto iEnd;
    goto *jmp[program[i].cmd];
    
    iMove: 
    ptr = ptr+program[i].param;
    i++;
    goto *jmp[program[i].cmd];
    
    iChange: 
    if(ptr<&dat[0] || ptr>(&dat[30000])){
         return 1;
    }
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

    iSetVal:
    *(ptr+program[i].param2)=program[i].param;
    i++;
    goto *jmp[program[i].cmd];

    iAdd:
    *(ptr+program[i].param) += *ptr * program[i].param2;
    i++;
    goto *jmp[program[i].cmd];
    
    iInc:
    *(ptr+program[i].param) = *(ptr+program[i].param) + program[i].param2;
    i++;
    goto *jmp[program[i].cmd];

    iEnd:
    return 0;
}

int main(int argc, char** argv) {

    /** Arguements 
    * -O1 setVal=0 optimization
    * -O2 O1 + setInv optimization
    * -O3 O2 + setAdd optimization
    * -O4 O3 + setVal optimization
    * -On All optimzations
    * -p Print brainfuck instructions 
    * -h Print help instructions
    * --help Print help instructions
    * -d disable execution of instructions
    **/

    int optimizationLevel = 0;
    bool printInst = false;
    bool executeInst = true;

    for(int i=1; i<argc; i++){
        std::string opt = std::string(argv[i]);
        if(opt == "-p"){
            printInst = true;
        } else if(opt == "-d"){
            executeInst = false;
        } else if(opt.size()>2 & opt.substr(0,2)=="-O"){
            if(optimizationLevel != 0) {
                cout << "Do not select multiple optimization levels !!" << endl;
                return -1;
            }
            if(opt[2] == '1') { optimizationLevel = 1;}
            else if(opt[2] == '2') { optimizationLevel = 2;}
            else if(opt[2] == '3') { optimizationLevel = 3;}
            else if(opt[2] == '4') { optimizationLevel = 4;}
            else if(opt[2] == 'n') { optimizationLevel = 4;}
        }
    }
    
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

    
    if(optimizationLevel>0){
        optimizeSetZero(program);
    }
    if(optimizationLevel>1){
        optimizeInc(program);
    }
    if(optimizationLevel>2){
        optimizeAdd(program);
    }
    if(optimizationLevel>3){
        optimizeSetValue(program);
    }
    linkLoops(program);

    if(printInst) printProgram(program);

    int executionStatus;
    if(executeInst) executionStatus = execute(program, ptr, dat);

    if(executionStatus){
         cout<<endl<<"EXECUTION ERROR!"<<endl;
    }

    return executionStatus;
}
