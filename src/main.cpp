#include<iostream>
#include<windows.h>


int main(int argc, char *argv[]){
    while(true){
        std::string input;
        std::cout<<"vault> ";
        if(!std::getline(std::cin,input)){
            std::cerr << "Error in recieving input from the console" << std::endl;
        }
    }
}