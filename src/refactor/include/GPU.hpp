#include <array>

class GPU {
    private:
        uint32_t display[2048];

    public:
        GPU();
        ~GPU();

        uint32_t getPixel(uint16_t position);
        void setPixel(uint16_t position, uint32_t value);
};