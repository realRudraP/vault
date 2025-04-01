#ifndef SPLITTER_H
#define SPLITTER_H

#include<string>

class Splitter{
public: 
    Splitter(const std::string& filename, int numOfParts);
    void splitfile();
private: 
    std::string inputfile;
    int numOfParts;
};

#endif