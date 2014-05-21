//Brainfuck Just-in-time compiler
//
#include <iostream>
#include <vector>
#include <stack>

using std::cin;
using std::cout;
 
int main() {
    char dat[40000];
    char *ptr=dat;
    char x;

    std::stack<int> loops;
    std::vector<char> stack;

    while(cin >> x) stack.push_back(x);

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
            if(*ptr!=0){
                loops.push(i);
            } else {
                int bCounter = 0;
                i++;
                while(!(stack[i]==']' && bCounter==0)){
                    if(stack[i]=='[') bCounter++;
                    if(stack[i]==']') bCounter--;
                    i++;
                }
            }
        }
        else if(x==']'){
            if((*ptr)==0){
                loops.pop();
            } else {
                i = loops.top();
            }
        }
        i++;
    }
    return 0;
}
