#include <cstddef>
#include <cstdint>
#include <vector>
#include <span>
#include <stdexcept>
#include <iostream>
#include <bitset>
using namespace std;


// state / initVector : s5...s0 (with s6 == s7 == 0)
// weights: p5...p0 (idem)
vector<uint8_t> decodeLFSR6(uint8_t initVector, uint8_t weights, span<uint8_t> toDecode)
{
    vector<uint8_t> res {};
    uint8_t state = initVector & 63; // In case the user gives something on more than 6 bits
    weights &= 63;

    for (uint8_t character : toDecode)
    {
        // Generate 5 bits of keystream
        uint8_t keystreamChk = 0;
        for (int n = 0; n < 5; ++n)
        {
            // Take the rightmost bit in the nth bit (starting on the right)
            keystreamChk |= ((state & 1) << (5-n-1));

            // Get the new leftmost bit which is the XOR of every state bit whose weight is 1.
            uint8_t const newLeft = __builtin_parity(state & weights) << 5; // GCC-specific

            // Shift the state to the right and integrate the leftmost bit
            state = newLeft | (state >> 1);
        }

        // Now just XOR the keystream chunk to the current character
        res.push_back(keystreamChk ^ character);        
    }

    return res;
}

uint8_t toBookCode(char c)
{
    if (c >= 'A' && c <= 'Z') return c - 'A';
    if (c >= 'a' && c <= 'z') return c - 'a';
    if (c >= '0' && c <= '5') return (c - '0') + 26;
    else throw std::runtime_error("invalid book code");
    return 0;
}

char toCharCode(uint8_t c)
{
    if (c <= 25) return c + 'A';
    if (c >= 26 && c <= 31) return c - 26 + '0';
    else throw std::runtime_error("invalid char code");
    return 0;
}

int main()
{
    std::string const toDecode = "j5a0edj2b";
    std::vector<uint8_t> toDecode_num;
    toDecode_num.resize(toDecode.size());
    for (size_t i = 0; i < toDecode.size(); ++i)
    {
        toDecode_num[i] = toBookCode(toDecode[i]);
    }

    // In the assignment the Initialization Vector is 111111 (3F)
    std::vector<uint8_t> result = decodeLFSR6(0x3F, 3, span{toDecode_num});

    std::string resultStr;
    resultStr.resize(result.size());
    for (size_t i = 0; i < result.size(); ++i)
    {
        resultStr[i] = toCharCode(result[i]);
    }

    std::cout << "Key stream is " << resultStr << std::endl;
}
