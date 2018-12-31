#include "ThirdIR.hpp"

void ThirdIR::parse(std::vector<Block> b)
{
    _blocks = b;
    legalize();
}

void ThirdIR::legalize()
{
    removeCopyWithSameArguments();
    searchForTwoTablesWithSameOperation();
    legalizeJumps();


}

void ThirdIR::removeCopyWithSameArguments()
{
    for(auto& b : _blocks)
    {
        for(auto i = b.lines.begin(); i != b.lines.end(); i++)
        {
            if (i->operation == "COPY" && i->one == i->two)
            {
                i = b.lines.erase(i);
                i--;
            }
        }
    }
}

bool ThirdIR::checkVariablesAreSameTable(std::string one, std::string two)
{
    if (one.find("(") == std::string::npos)
    {
        return false;
    }
    if (two.find("(") == std::string::npos)
    {
        return false;
    }
    std::string oneArray = one.substr(0, one.find("("));
    std::string twoArray = two.substr(0, two.find("("));
    return oneArray == twoArray;
}

void ThirdIR::searchForTwoTablesWithSameOperation()
{
    for(auto& b : _blocks)
    {
        for(auto i = b.lines.begin(); i != b.lines.end(); i++)
        {
            if (checkVariablesAreSameTable(i->one, i->two))
            {
                std::string var = getVariable("NAN");
                i = b.lines.insert(i, {"COPY", var, i->two});
                i++;
                i->two = var;
            }
        }
    }
}

void ThirdIR::legalizeJumps()
{
    for (auto& b : _blocks)
    {
        if (!b.blockJump.empty())
        {
            Line l;
            l.operation = "JUMP";
            l.two = "#" + b.blockJump;
            b.lines.push_back(l);
            continue;
        }
        if (b.lines.back().operation == "JLS")
        {
            legalizeJLS(b, false);
        }
        if(b.lines.back().operation == "JMR")
        {
            legalizeJMR(b, false);
        }
        if (b.lines.back().operation == "JLE")
        {
            legalizeJLS(b, true);
        }
        if(b.lines.back().operation == "JME")
        {
            legalizeJMR(b, true);
        }        
    }
}

std::string ThirdIR::calculateDiff(Block& b, std::string one, std::string two, bool inc)
{
    std::string var = _symbolTable->getExtraVariable();
    
    Line copyLine;
    copyLine.operation = "COPY";
    copyLine.one = var;
    copyLine.two = one;
    b.lines.push_back(copyLine);
    
    if (inc)
    {
        Line incLine;
        incLine.operation = "INC";
        incLine.one = var;
        b.lines.push_back(incLine);
    }

    Line subLine;
    subLine.operation = "SUB";
    subLine.one = var;
    subLine.two = two;
    b.lines.push_back(subLine);    

    return var;
}

void ThirdIR::insertJumps(Block&b, std::string var)
{
    Line jzero;
    jzero.operation = "JZERO";
    jzero.one = var;
    jzero.two = "#"+b.blockIfFalse;
    b.lines.push_back(jzero);

    Line jump;
    jump.operation = "JUMP";
    jump.two = "#"+b.blockIfTrue;
    b.lines.push_back(jump);
}

void ThirdIR::legalizeJLS(Block& b, bool inc)
{
    Line lastLine = b.lines.back();
    b.lines.pop_back();

    std::string var = calculateDiff(b, lastLine.two, lastLine.one, inc);
    insertJumps(b, var);
}

void ThirdIR::legalizeJMR(Block& b, bool inc)
{
    Line lastLine = b.lines.back();
    b.lines.pop_back();

    std::string var = calculateDiff(b, lastLine.one, lastLine.two, inc);
    insertJumps(b, var);
}

