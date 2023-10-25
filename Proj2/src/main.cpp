#include <fstream>
#include <cstdio>

#include <loader.h>

#include <memory.hpp>
#include <register.hpp>


int main(int argc, char** argv)
{
    std::ifstream asm_file(argv[1]);
    xldz::load(asm_file);
    printf("data_pos at 0x%08x\n",xldz::data_pos-xldz::mem);
    for(unsigned char *p=xldz::mem+xldz::DATA_BEGIN;p!=xldz::data_pos;++p)
    {
        printf("%02x",*p);
        if((p-xldz::mem)%4==3)puts("");
    }
}
