#ifndef PERLIN_HPP
#define PERLIN_HPP
#include <iostream>
#include <math.h>

enum Perlin_interpolation
{
    LINEAR_INTERPOLATION,
    COSINE_INTERPOLATION
};


class Perlin
{
    private:
        uint8_t p[512];         //stocke les permutations en 2 fois pour eviter overflow; est utilisé en tant que fonction de hash pour trouver un vecteur gradient pseudo aléatoirement
        uint32_t repeat;        //si > 0, repete un motif
        typedef float (Perlin::*pInterpolation)(float, float, float);    // permet de choisir le type d'interpolation
        Perlin::pInterpolation interpolation;

    private:
        //lisse les coordonées au voisinage de 0 et 1
        float fade(float t);

        //incremente n en tenant compte de la repetition
        uint32_t inc(uint32_t n);       

        //produit scalaire entre le gradient representer par son hash et (x, y, z) le vecteur (sommet cube - point)
        double grad(uint8_t hash, double x, double y, double z);    

        //interpolation lineaire
        float lerp(float a, float b, float c);  

        //interpolation cosinus
        float cerp(float a, float b, float t);

        //calcul du bruit de perlin en (x, y, z)
        float pNoise(double x, double y, double z);                 


    public:
        Perlin(const Perlin_interpolation interpolation=LINEAR_INTERPOLATION, const double repeat=0, const uint64_t seed=0);
        float perlinNoise(double x, double y, double z, uint32_t octave = 1, float persistence = 0);    //bruit de perlin avec parametres

};

#endif