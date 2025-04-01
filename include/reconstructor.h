#ifndef RECONSTRUCTOR_H 
#define RECONSTRUCTOR_H

#include<string>

class Reconstructor{
public:
    Reconstructor(const std::string& metadata);
    void reconstructor();
     
private:
    std::string metadata; 


};

#endif