#include <iostream>
#include <string>
#include <stdexcept>

#include <cryptopp/osrng.h>
#include <cryptopp/integer.h>
#include <cryptopp/nbtheory.h>
#include <cryptopp/dh.h>


const static size_t P_SIZE{0x1000}; // 4096 is safe

void generate_dh(size_t size)
{
    try
    {
        CryptoPP::PrimeAndGenerator pg;
        CryptoPP::AutoSeededRandomPool rng{};

        pg.Generate(1, rng, size, size-1);

        CryptoPP::DH dh{pg.Prime(), pg.SubPrime(), pg.Generator()};

        if (!dh.GetGroupParameters().ValidateGroup(rng, 3))
            throw std::runtime_error{"Failed to validate prime and generator"};

        size_t count{};

        const CryptoPP::Integer& p = dh.GetGroupParameters().GetModulus();
        count = p.BitCount();
        std::cout << "P (" << std::dec << count << "):" << std::hex << p << std::endl;

        const CryptoPP::Integer& q = dh.GetGroupParameters().GetSubgroupOrder();
        count = q.BitCount();
        std::cout << "Q (" << std::dec << count << "): " << std::hex << q << std::endl;

        const CryptoPP::Integer& g = dh.GetGroupParameters().GetGenerator();
        count = g.BitCount();
        std::cout << "G (" << std::dec << count << "): " << std::dec << g << std::endl;

        // http://groups.google.com/group/sci.crypt/browse_thread/thread/7dc7eeb04a09f0ce
        CryptoPP::Integer v = CryptoPP::ModularExponentiation(g, q, p);
        if (v != CryptoPP::Integer::One())
            throw std::runtime_error("Failed to verify order of the subgroup");
    } catch (const CryptoPP::Exception& e)
    {
        std::cerr << e.what() << std::endl;
    } catch (const std::exception& e) 
    { 
        std::cerr << e.what() << std::endl;
    }
}

int main()
{
    generate_dh(P_SIZE);
    return 0;
}