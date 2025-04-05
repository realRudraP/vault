#include "../include/core.h"


bool Core::splitFile(const fs::path& filePath,int numChunks){
    ImplManager iManager;
    iManager.splitFile(filePath,numChunks);
    return true;
}