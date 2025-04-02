#ifndef SPLITTER_H
#define SPLITTER_H

#include<string>

class Splitter{
public: 
    Splitter(const std::string& filename, int numOfParts);
    void splitfile();
    Reconstructor(const std::string& metadata);
    void reconstruct();
private: 
    std::string inputfile;
    int numOfParts;
    std::string metadata;
};

#endif
