//Brainfuck Just-in-time compiler
//
#include <iostream>
#include <vector>

using std::cin;
using std::cout;
struct loop {
    int stack;
    char* ptr;
};
 
int main() {
    char dat[30000];
    char *ptr=dat;
    char x;

    std::vector<loop> loops;
    std::vector<char> stack;

    while(cin >> x) {
        if(x=='<' || x=='>' || x=='+' || x=='-' || x=='.' || x==',' || x=='[' || x==']') stack.push_back(x);
    }

    int i=0;
    while(i<stack.size()){
        x = stack[i];

        if(x=='>'){++ptr;}
        else if(x=='<'){--ptr;}
        else if(x=='+'){++*ptr;}
        else if(x=='-'){--*ptr;}
        else if(x=='.'){cout<< *ptr;}
        else if(x==','){cin >> *ptr;}
        else if(x=='['){
	        loop newLoop;
        	newLoop.stack = i;
        	newLoop.ptr = ptr;
        	loops.push_back(newLoop);
        }
        else if(x==']'){
            if(*ptr==0){
                loops.pop_back();
            } else {
                i = loops.back().stack;
            }
        }
        i++;
    }


    cout<<std::endl;
    return 0;
}
