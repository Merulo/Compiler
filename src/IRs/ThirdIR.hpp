#include "IRBase.hpp"

class ThirdIR : public IRBase
{
    public:
    ThirdIR() : IRBase("ThirdIR") {};

    void parse(std::vector<Block> b);

    private:
    void legalize();
    void legalizeJumps();
    void legalizeJLS(Block& b);
};