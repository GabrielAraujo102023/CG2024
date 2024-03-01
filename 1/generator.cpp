#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <string>
#define USE_MATH_DEFINES
#include <cmath>
#include <fstream>
#include <list>
#include <sstream>

void printArgumentError()
{
    printf("Possible arguments:\n\t"
           "\uF0B7Plane, X and Z\n"
           "\uF0B7 Box, dimension, and the number of divisions per edge\n"
           "\uF0B7 Sphere, radius, slices and stacks\n"
           "\uF0B7 Cone, bottom radius, height, slices and stacks\n"
           "And the resulting file");
}

//Guarda os pontos num ficheiro
//Pontos devem vir dentro de uma lista de strings assim --> "x y z"
bool saveFile(char* filename, const std::list<std::string>& points, const std::string& name)
{
    std::ofstream file(filename);
    if (!file.is_open())
    {
        printf("There was a problem with the file.");
        return false;
    }
    if (!name.empty())
        file << name << "\n";
    for(const std::string& s : points)
    {
        file << s << "\n";
    }
    return true;
}

//Após o calculo de todos os quadrados de uma linha num plano, usa esta função para calcular o ponto inicial da próxima linha
void planeNextPoint(int rowsCalculated, double halfLength, double divisionSize, double& nextX, double& nextZ)
{
    // Z = Metade da largura do quadrado menos a altura das linhas já calculadas
    nextZ = halfLength - (divisionSize * rowsCalculated);
    
    nextX = halfLength;
}

//Calcula pontos para um plano
bool plane(double length, int divisions, char* filename)
{
    std::list<std::string> pointList = {};
    int rowsCalculated = 0;
    //Tamanho de cada quadrado que forma o plano
    double divisionSize = length / divisions;
    //Metade da largura do plano, para calcular teorema de pitágoras
    double halfLength = length / 2;
    //Pontos X e Z. Valor inicial é o canto superior direito do plano
    double x = halfLength, z = halfLength;
    while(rowsCalculated < divisions)
    {
        //Aqui calcula os pontos de todos os quadrados de um fila
        for(int i = 0; i < divisions; i++)
        {
            std::stringstream p1, p2, p3, p4;
            double xMin = x - divisionSize;
            double zMin = z - divisionSize;
            p1 << x << " 0 " << z;
            p2 << xMin << " 0 " << z;
            p3 << xMin << " 0 " << zMin;
            p4 << x << " 0 " << zMin;

            pointList.emplace_back(p2.str());
            pointList.emplace_back(p3.str());
            pointList.emplace_back(p1.str());

            pointList.emplace_back(p3.str());
            pointList.emplace_back(p4.str());
            pointList.emplace_back(p1.str());

            x = xMin;
        }
        rowsCalculated++;
        planeNextPoint(rowsCalculated, halfLength, divisionSize, x, z);
    }
    return saveFile(filename, pointList, "plane");
}

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        printArgumentError();
        return 0;
    }
    if (std::strcmp(argv[1], "plane") == 0)
    {
        if(argc != 5)
        {
            printArgumentError();
            return 0;
        }
        plane(std::stod(argv[2]), std::stoi(argv[3]), argv[4]);
    }
}
