#include <memory.hpp>
#include <register.hpp>


#include <snapshot.h>

#include <cstdio>
#include <set>
#include <string>

namespace xldz
{

// checkpoint
std::set<int> checkpoints;
void init_checkpoints(char *checkpoint_file)
{
    FILE *fp = fopen(checkpoint_file, "r");
    int tmp, i = 0;
    while (fscanf(fp, "%d", &tmp) != EOF)
    {
        checkpoints.insert(tmp);
    }
}
void checkpoint_memory(int ins_count)
{
    if (!checkpoints.count(ins_count)) return;
    std::string name = "memory_" + std::to_string(ins_count) + ".bin";
    FILE *fptr = fopen(name.c_str(), "wb");
    fwrite(real_mem, 1, 0x600000, fptr);
    fclose(fptr);
}
void checkpoint_register(int ins_count)
{
    if (!checkpoints.count(ins_count)) return;
    std::string name = "register_" + std::to_string(ins_count) + ".bin";
    FILE *fptr = fopen(name.c_str(), "wb");
    for (u32 i = 0; i < 32; ++i)
    {
        fwrite(reg + i, 4, 1, fptr);
    }
    fwrite(&pc, 4, 1, fptr);
    fwrite(&hi, 4, 1, fptr);
    fwrite(&lo, 4, 1, fptr);
    fclose(fptr);
}

void snapshot(int ins_count)
{
    checkpoint_memory(ins_count);
    checkpoint_register(ins_count);
}


} // namespace xldz