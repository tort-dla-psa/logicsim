#include "sim/sim.h"
#include "sim/file_ops.h"
#include <filesystem>

void print_diff(std::vector<uint8_t>::const_iterator begin,
    std::vector<uint8_t>::const_iterator end, 
    std::vector<uint8_t>::const_iterator begin2)
{
    auto bak = begin;
    while(begin != end){
        if(*begin != *begin2){
            std::cout<<"diff at:"<<std::distance(bak, begin)<<
                "\t"<<std::to_string(*begin)<<
                "\t"<<std::to_string(*begin2)<<"\n";
        }
        begin++;
        begin2++;
    }
}

int main(){
    auto root1 = std::make_unique<elem_meta>("root");
    auto and_elem = std::make_unique<elem_and>("and1");
    auto not_elem = std::make_unique<elem_not>("not1");
    and_elem->get_out(0)->tie_input(not_elem->get_in(0));
    root1->emplace_back(std::move(and_elem));
    root1->emplace_back(std::move(not_elem));

    auto bin_save = elem_file_saver::to_bin(root1.get());
    auto path = std::filesystem::path("/tmp/sim_save.sim");
    if(std::filesystem::exists(path)){
        std::filesystem::remove(path);
    }
    elem_file_saver::save_bin(bin_save.cbegin(), bin_save.cend(), path);

    auto bin_load = elem_file_saver::load_bin(path);
    if(!std::equal(bin_save.cbegin(), bin_save.cend(), bin_load.cbegin())){
        std::filesystem::remove(path);
        print_diff(bin_save.begin(), bin_save.end(), bin_load.begin());
        throw std::runtime_error("saved and loaded binaries are not the same");
    }

    std::vector<std::unique_ptr<element>> elems_load;
    auto it = bin_load.cbegin();
    elem_file_saver::from_bin(it, elems_load);
    auto cast = dynamic_cast<elem_meta*>(elems_load.front().get());
    if(*cast != *root1.get()){
        std::filesystem::remove(path);
        throw std::runtime_error("saved and loaded elements are not the same");
    }
    std::filesystem::remove(path);
};