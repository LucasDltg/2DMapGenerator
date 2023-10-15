#include "Perlin.hpp"

//modulo double
template<typename T, typename U>
inline constexpr double dmod(T x, U mod)
{
    return !mod ? x : x - mod*static_cast<long long>(x/mod);
}

template<typename T, typename U>
inline constexpr double nmod(T x, U mod)
{
    return !mod ? x : dmod(mod + dmod(x, mod), mod);
}


float Perlin::fade(float t)
{
    // t^3(t(6t-15)+10)
    return float(t*t*t*(t*(t*6-15)+10));
}

uint32_t Perlin::inc(uint32_t n)
{
    n++;
    if(this->repeat > 0)
        n = dmod(n, repeat);
    return n%255;
}

double Perlin::grad(uint8_t hash, double x, double y, double z)
{
        switch(hash & 0xF)
        {
        case 0x0:
            return x+y;
        case 0x1:
            return -x+y;
        case 0x2:
            return x-y;
        case 0x3:
            return -x-y;
        case 0x4:
            return x+z;
        case 0x5:
            return -x+z;
        case 0x6:
            return x-z;
        case 0x7:
            return -x-z;
        case 0x8:
            return y+z;
        case 0x9:
            return -y+z;
        case 0xA:
            return y-z;
        case 0xB:
            return -y-z;
        case 0xC:
            return y+x;
        case 0xD:
            return -y+z;
        case 0xE:
            return y-x;
        case 0xF:
            return -y-z;
        default:
            return 0;
        }

}

float Perlin::lerp(float a, float b, float t)
{
    return a+t*(b-a);
}

float Perlin::cerp(float a, float b, float t)
{
    float c = (1 - cos((t*M_PI))*0.5);
    return (1.0 - c)*a+c*b;
}

float Perlin::pNoise(double x, double y, double z)
{
    // fait boucler les coordonnées afin de reproduire le meme motif
    if(repeat > 0)
    {
        x = nmod(x, repeat);
        y = nmod(y, repeat);
        z = nmod(z, repeat);
    }

    //partie entiere de x pour x > 0 puis x < 0
    int32_t xi = (int32_t)x;
    int32_t yi = (int32_t)y;
    int32_t zi = (int32_t)z;
    if(x < 0)
        xi -= 1;
    if(y < 0)
        yi -= 1;
    if(z < 0)
        zi -= 1;

    //partie decimale
    double xf = x - (int32_t)xi;
    double yf = y - (int32_t)yi;
    double zf = z - (int32_t)zi;

    //evite les overflow dans la table de hash p -> impose un repeat, fait boucler a 255
    xi = nmod(xi, 255);
    yi = nmod(yi, 255);
    zi = nmod(zi, 255);

    float u = fade(xf);
    float v = fade(yf);
    float w = fade(zf);

    // table de hash selectionne pseudo aléatoirement des vecteurs gradient pour chaque sommet du cube
    uint8_t aaa = p[p[p[    xi ]+    yi ]+    zi ];
    uint8_t aba = p[p[p[    xi ]+inc(yi)]+    zi ];
    uint8_t aab = p[p[p[    xi ]+    yi ]+inc(zi)];
    uint8_t abb = p[p[p[    xi ]+inc(yi)]+inc(zi)];
    uint8_t baa = p[p[p[inc(xi)]+    yi ]+    zi ];
    uint8_t bba = p[p[p[inc(xi)]+inc(yi)]+    zi ];
    uint8_t bab = p[p[p[inc(xi)]+    yi ]+inc(zi)];
    uint8_t bbb = p[p[p[inc(xi)]+inc(yi)]+inc(zi)];

    
    //Moyenne de moyenne de moyenne a laide de la fonction dinterpolation choisie
    float x1 = (this->*interpolation)(grad(aaa, xf, yf, zf),  grad(baa, xf-1, yf,   zf), u);
    float x2 = (this->*interpolation)(grad(aba, xf, yf-1, zf),grad(bba, xf-1, yf-1, zf), u);
    float y1 = (this->*interpolation)(x1, x2, v);

    x1 = (this->*interpolation)(grad(aab, xf, yf,   zf-1), grad(bab, xf-1, yf,   zf-1), u);
    x2 = (this->*interpolation)(grad(abb, xf, yf-1, zf-1), grad(bbb, xf-1, yf-1, zf-1), u);
    float y2 = (this->*interpolation)(x1, x2, v);

    return ((this->*interpolation)(y1, y2, w)+1.0)/2.0;
}
    
Perlin::Perlin(const Perlin_interpolation interpolation_type, const double repeat, const uint64_t seed)
: repeat(repeat)
{
    uint8_t defaultHash[] = {151, 160, 137, 91, 90, 15, 131, 13, 201, 95, 96, 53, 194, 233, 7, 225, 140, 36, 103, 30, 69, 142, 8, 99, 37, 240, 21, 10, 23,
                             190, 6, 148, 247, 120, 234, 75, 0, 26, 197, 62, 94, 252, 219, 203, 117, 35, 11, 32, 57, 177, 33, 88, 237, 149, 56, 87, 174, 20,
                             125, 136, 171, 168, 68, 175, 74, 165, 71, 134, 139, 48, 27, 166, 77, 146, 158, 231, 83, 111, 229, 122, 60, 211, 133, 230, 220,
                             105, 92, 41, 55, 46, 245, 40, 244, 102, 143, 54, 65, 25, 63, 161, 1, 216, 80, 73, 209, 76, 132, 187, 208, 89, 18, 169, 200, 196,
                             135, 130, 116, 188, 159, 86, 164, 100, 109, 198, 173, 186, 3, 64, 52, 217, 226, 250, 124, 123, 5, 202, 38, 147, 118, 126, 255,
                             82, 85, 212, 207, 206, 59, 227, 47, 16, 58, 17, 182, 189, 28, 42, 223, 183, 170, 213, 119, 248, 152, 2, 44, 154, 163, 70, 221,
                             153, 101, 155, 167, 43, 172, 9, 129, 22, 39, 253, 19, 98, 108, 110, 79, 113, 224, 232, 178, 185, 112, 104, 218, 246, 97, 228,
                             251, 34, 242, 193, 238, 210, 144, 12, 191, 179, 162, 241, 81, 51, 145, 235, 249, 14, 239, 107, 49, 192, 214, 31, 181, 199, 106, 
                             157, 184, 84, 204, 176, 115, 121, 50, 45, 127, 4, 150, 254, 138, 236, 205, 93, 222, 114, 67, 29, 24, 72, 243, 141, 128, 195, 78,
                              66, 215, 61, 156, 180};
    if(seed == 0)
    {
        for(uint32_t i(0);i<2;i++)
        {
            for(uint32_t j(0);j<256;j++)
            {
                p[j+i*256] = defaultHash[j];
            }
        }
    }
    else
    {
        //aleatoire en fonction seed
    }

    //selectionne la methode d'interpolation
    switch (interpolation_type)
    {
    case LINEAR_INTERPOLATION:
        interpolation = &Perlin::lerp;
        break;
    case COSINE_INTERPOLATION:
        interpolation = &Perlin::cerp;
        break;
    }

}
    
float Perlin::perlinNoise(double x, double y, double z, uint32_t octave, float persistence)
{
    float total = 0.0f, amplitude = 1.0f, max = 0.0f;
    uint32_t frequency = 1;
        
    for(uint32_t i(0);i<octave;i++)
    {
        total += this->pNoise(x*frequency, y*frequency, z*frequency)*amplitude;
        max += amplitude;
        amplitude*=persistence;
        frequency*=2;
    }
            
    return total/(float)max;
}
