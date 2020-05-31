#include <iostream>
#include <cassert>
#include "sim/sim.h"

class half_adder:public element{
public:
    half_adder()
        :element("half_adder"),
        nameable("half_adder")
    {
        ins.emplace_back(new gate_in("in1", 1, get_id()));
        ins.emplace_back(new gate_in("in2", 1, get_id()));
        outs.emplace_back(new gate_out("out", 1, get_id()));
        outs.emplace_back(new gate_out("c_out", 1, get_id()));
    }

    void process()override{
        auto in1 = ins.at(0)->get_value()[0];
        auto in2 = ins.at(1)->get_value()[0];
        auto tmp1 = in1 != in2; //summ
        auto tmp2 = in1 && in2; //c_out
        std::cout<<"in1:"<<in1<<"\n";
        std::cout<<"in2:"<<in2<<"\n";
        std::cout<<"sum:"<<tmp1<<"\n";
        std::cout<<"cout:"<<tmp2<<"\n";
        outs.at(0)->set_value({tmp1});
        outs.at(1)->set_value({tmp2});
    }

    auto& get_a(){ return ins.at(0); }
    auto& get_b(){ return ins.at(1); }
    auto& get_sum(){ return outs.at(0); }
    auto& get_cout(){ return outs.at(1); }
};

class full_adder:public element{
    std::vector<half_adder> adders;
    std::vector<elem_or> ors;
public:
    full_adder(const size_t &size)
        :element("full_adder"),
        nameable("full_adder")
    {
        ins.clear();
        outs.clear();
        adders.reserve(size*2);
        ors.reserve(size);
        std::shared_ptr<gate_out> prev_cout;
        for(size_t i=0; i<size; i++){
            auto &hf_1 = adders.emplace_back();
            auto &hf_2 = adders.emplace_back();
            auto &or_1 = ors.emplace_back("or");
            auto &a = hf_1.get_a();
            auto &b = hf_1.get_b();
            auto &c_in = hf_2.get_b();
            hf_1.get_sum()->tie_input(hf_2.get_a());
            hf_1.get_cout()->tie_input(or_1.get_in(0));
            hf_2.get_cout()->tie_input(or_1.get_in(1));
            auto &sum =  hf_2.get_sum();
            auto c_out = or_1.get_out(0);
            if(i==0){
                ins.emplace_back(c_in);
            }else{
                prev_cout->tie_input(c_in);
            }
            ins.emplace_back(a);
            ins.emplace_back(b);
            outs.emplace_back(sum);
            prev_cout = c_out;
        }
        outs.emplace_back(prev_cout);
    }

    void process()override{
        for(size_t i=0; i<ors.size(); i++){
            adders.at(i*2+0).process();
            adders.at(i*2+1).process();
            ors.at(i).process();
        }
    }

    void set_c_in(bool c_in){
        ins.at(0)->set_value({c_in});
    }

    bool get_c_out()const{
        return outs.back()->get_value(0);
    }

    template<class Val>
    void set_ins(Val a, Val b){
        for(size_t i=0; i<sizeof(Val)*8; i++){
            bool bit_a = (a>>i)&1;
            bool bit_b = (b>>i)&1;
            std::cout<<bit_a<<" "<<bit_b<<"\n";
            ins.at(1+i*2)->set_value({bit_a});
            ins.at(1+i*2+1)->set_value({bit_b});
        }
    }

    std::vector<bool> get_value()const{
        std::vector<bool> result;
        result.reserve(outs.size()-1);
        for(size_t i=0; i<outs.size()-1; i++){
            result.emplace_back(outs.at(i)->get_value()[0]);
        }
        return result;
    }
};

int main(int argc, char* argv[]){
    sim s;
    auto root = s.begin();
    auto fa_ptr = std::make_unique<full_adder>(8);
    auto &el = *s.emplace(root, std::move(fa_ptr));
    auto fa = dynamic_cast<full_adder*>(el.get());
    uint8_t in1 = 11;
    uint8_t in2 = 13;
    fa->set_ins(in1, in2);
    fa->set_c_in(0);
    s.tick();
    auto out_bits = fa->get_value();
    int out = 0;
    for(size_t i=0; i<out_bits.size(); i++){
        out |= out_bits.at(i)<<i;
    }
    std::cout<<"in1 = "<<(int)in1<<"\n";
    std::cout<<"in2 = "<<(int)in2<<"\n";
    std::cout<<"out = "<<out<<"\n";
    assert(in1+in2 == out);
}