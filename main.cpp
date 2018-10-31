
#include <iostream>
#include <bitset>

using namespace std;

const unsigned long long FILTERS[] = {0b111, 0b011};
const unsigned long long NUM_FILTERS = 2;
const unsigned long long FILTER_LEN = 3;



unsigned long long hammingWeight(unsigned long long num)   {
    unsigned long long res = 0;
    while (num) {
        res += (num & 1);
        num >>= 1;
    }
    return res;
}

unsigned long long convEncode(unsigned long long data) {
    unsigned long long res = 0;

    for(unsigned int i = 0; i < 32; i ++)    {

        unsigned long long mask = (data >> i) & 0b111;

        for (unsigned int j = 0; j < NUM_FILTERS; j ++)   {
            res |= ((hammingWeight(mask & FILTERS[j]) & 1) << (NUM_FILTERS * (i + 1) - 1 - j));
        }
    }

    return res;
}

int main(int argc, char* argv[]) {

    if (argc < 2) {
        cerr << "Improper Number of Arguments" << endl;
        return 1;
    }

    string data_string = argv[1];
    string res_string;

    unsigned int original_len = data_string.length();

    unsigned int block_size = 8 * sizeof(unsigned long long) / NUM_FILTERS;
    unsigned int num_blocks = (data_string.size() + block_size - 1) / block_size;

    unsigned long long res;

//    cout << res << endl;

    while (data_string.size() - FILTER_LEN + 2 < block_size * num_blocks)    {
        data_string.insert(0, "0");
    }

    for (unsigned int i = 0; i < num_blocks; i ++)    {
        std::string sub = data_string.substr(i * block_size,
                (i + 1) * block_size + (unsigned int)NUM_FILTERS - 1);

        res = convEncode(std::bitset<64>(sub).to_ullong());

        res_string += std::bitset<64>(res).to_string();
//        cout << res[i] << endl;
//        cout << bitset<64>(res[i]).to_string() << endl;
    }
    if (res_string.length() > 2 * original_len) {
        res_string = res_string.substr(res_string.length() - 2 * original_len);
    }

    cout << res_string << endl;

    return 0;
}




