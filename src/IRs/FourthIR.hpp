#include "IRBase.hpp"
#include "Register/RegisterBlock.hpp"

#include "Calculators/NumberGenerator.hpp"

class FourthIR : public IRBase
{
    public:
    FourthIR() : IRBase("FourthIR") {};

    void parse(std::vector<Block> b);

    private:
    std::vector<Block> _notYetConvertedBlocks;
    void convertToAssembler();
    void convertBlockToAssembler(Block& block, RegisterBlock registerBlock);

    void handleConst(RegisterBlock& rb, Block& b, Line& l)
    {
        Register& r = rb.getRegisterForVariable(l.one);
        prepareRegisterWithoutLoading(rb, r, b, l);

        std::cerr<<__func__<< " "<<r.name<< " is ";
        if (_symbolTable->checkVariableIsVariable(l.one).empty())
        {
            r.state = RegisterState::CONSTVARIABLE;
            std::cerr<<"constvariable"<<std::endl;
        }
        else if (_symbolTable->checkVariableIsTable(l.one).empty())
        {
            //TODO: add instant save
            r.state = RegisterState::TABLE;
            std::cerr<<"table"<<std::endl;
        }
        else
        {
            r.state = RegisterState::CONST;
            std::cerr<<"const"<<std::endl;
        }      

        auto lines = NumberGenerator::generateConstFrom(std::stoull(l.two), {{r.name, 0}});
        b.lines.insert(b.lines.end(), lines.begin(), lines.end());
    }

    void handleWrite(RegisterBlock& rb, Block& b, Line& l)
    {
        Register& r = rb.getRegisterForVariable(l.one);
        
        std::cout<<"TEST->"<<r<<std::endl;
        std::cout<<"TEST->"<<l.one<<std::endl;

        prepareRegisterWithLoading(rb, r, b, l);
        
        b.lines.push_back({"PUT", r.name});      
    }

    void prepareRegisterWithLoading(RegisterBlock& rb, Register& r, Block& b, Line l)
    {
        if (r.variableName != l.one)
        {
            //save current content
            if (r.state == RegisterState::CONSTVARIABLE || r.state == RegisterState::VARIABLE)
            {
                auto lines = rb.performMemoryOperation("STORE", r, _symbolTable->getMemoryCell(r.variableName));
                b.lines.insert(b.lines.end(), lines.begin(), lines.end());
            }
            //load new content
            auto lines = rb.loadVariableToRegister(r, l.one);
            b.lines.insert(b.lines.end(), lines.begin(), lines.end());
        }
        r.variableName = l.one;  
    }


    void prepareRegisterWithoutLoading(RegisterBlock& rb, Register& r, Block& b, Line l)
    {
        if (r.variableName != l.one)
        {
            //save current content
            if (r.state == RegisterState::CONSTVARIABLE || r.state == RegisterState::VARIABLE)
            {
                auto lines = rb.performMemoryOperation("STORE", r, _symbolTable->getMemoryCell(r.name));
                b.lines.insert(b.lines.end(), lines.begin(), lines.end());
            }
        }
        r.variableName = l.one;
    }
};