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
#include <fstream>
#include <list>
#include <sstream>
#include <cmath>

void printArgumentError()
{
    printf("Possible arguments:\n\t"
           "- Plane, X and Z\n\t"
           "- Box, dimension, and the number of divisions per edge\n\t"
           "- Sphere, radius, slices and stacks\n\t"
           "- Cone, bottom radius, height, slices and stacks\n"
           "And the resulting file");
}

//Guarda os pontos num ficheiro
//Pontos devem vir dentro de uma lista de strings assim --> "x y z"
//name é o texto que vai ser escrito na primeira linha
//append é um bool a dizer se o ficheiro é para ser escrito do 0 ou appended
bool saveFile(char* filename, const std::list<std::string>& points, const std::string& name, bool append)
{
    std::ofstream file;
    if (append)
    {
        file.open(filename, std::ios_base::app);
    }
    else
    {
        file.open(filename);
    }
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

double calculatePlaneY(double x, double z, double divisionSize, const std::string& y, std::list<std::string>& pointList)
{
    std::stringstream p1, p2, p3, p4;
    double xMin = x - divisionSize;
    double zMin = z - divisionSize;
    p1 << x << y << z;
    p2 << xMin << y << z;
    p3 << xMin << y << zMin;
    p4 << x << y << zMin;

    pointList.emplace_back(p2.str());
    pointList.emplace_back(p3.str());
    pointList.emplace_back(p1.str());

    pointList.emplace_back(p3.str());
    pointList.emplace_back(p4.str());
    pointList.emplace_back(p1.str());

    return xMin;
}
double calculatePlaneX(double y, double z, double divisionSize, const std::string& x, std::list<std::string>& pointList)
{
    std::stringstream p1, p2, p3, p4;
    double yMin = y - divisionSize;
    double zMin = z - divisionSize;
    p1 << x << y << " " << z;
    p2 << x << y << " " << zMin;
    p3 << x << yMin << " " << zMin;
    p4 << x << yMin << " " << z;

    pointList.emplace_back(p2.str());
    pointList.emplace_back(p3.str());
    pointList.emplace_back(p1.str());

    pointList.emplace_back(p3.str());
    pointList.emplace_back(p4.str());
    pointList.emplace_back(p1.str());

    return yMin;
}
double calculatePlaneZ(double x, double y, double divisionSize, const std::string& z, std::list<std::string>& pointList)
{
    std::stringstream p1, p2, p3, p4;
    double xMin = x - divisionSize;
    double yMin = y - divisionSize;
    p1 << x << " " << y << z;
    p2 << xMin << " " << y << z;
    p3 << xMin << " " << yMin << z;
    p4 << x << " " << yMin << z;

    pointList.emplace_back(p2.str());
    pointList.emplace_back(p3.str());
    pointList.emplace_back(p1.str());

    pointList.emplace_back(p3.str());
    pointList.emplace_back(p4.str());
    pointList.emplace_back(p1.str());

    return xMin;
}

double calculatePlane(double a, double b, double divisionSize, const std::string& constant,
                    std::list<std::string>& pointList, char constAxis)
{
    switch(constAxis)
    {
        case 'x':
            return calculatePlaneX(a, b, divisionSize, constant, pointList);
        case 'y':
            return calculatePlaneY(a, b, divisionSize, constant, pointList);
        case 'z':
            return calculatePlaneZ(a, b, divisionSize, constant, pointList);
        default:
            break;
    }
    return 0;
}

//Calcula pontos para um plano
bool plane(double length, int divisions, const std::string& constant, char* filename, const std::string& firstLine,
           bool append, char axis)
{
    std::list<std::string> pointList = {};
    int rowsCalculated = 0;
    //Tamanho de cada quadrado que forma o plano
    double divisionSize = length / divisions;
    //Metade da largura do plano, para calcular teorema de pitágoras
    double halfLength = length / 2;
    //Pontos nao constantes do plano. Valor inicial é o canto superior direito do plano
    double a = halfLength, b = halfLength;
    while(rowsCalculated < divisions)
    {
        //Aqui calcula os pontos de todos os quadrados de um fila
        for(int i = 0; i < divisions; i++)
        {
            a = calculatePlane(a, b, divisionSize, constant, pointList, axis);
        }
        rowsCalculated++;
        planeNextPoint(rowsCalculated, halfLength, divisionSize, a, b);
    }
    return saveFile(filename, pointList, firstLine, append);
}

//Calcula planos para uma caixa
void box(double length, int divisions, char* filename)
{
    double halfLength = length / 2;
    std::stringstream constant;
    constant << halfLength << " ";
    plane(length, divisions, constant.str(), filename, "box", false, 'x');
    constant.str("");
    constant << "-" << halfLength << " ";
    plane(length, divisions, constant.str(), filename, "", true, 'x');
    constant.str("");

    constant << " " << halfLength << " ";
    plane(length, divisions, constant.str(), filename, "", true, 'y');
    constant.str("");
    constant << " " << "-" << halfLength << " ";
    plane(length, divisions, constant.str(), filename, "", true, 'y');
    constant.str("");

    constant << " " << halfLength;
    plane(length, divisions, constant.str(), filename, "", true, 'z');
    constant.str("");
    constant << " " << "-" << halfLength;
    plane(length, divisions, constant.str(), filename, "", true, 'z');
}

void sphere(double radius, int slices, int stacks, char* filename)
{
    std::list<std::string> pointList = {};
    std::stringstream p;
    double phi1, phi2, theta1, theta2;
    double x1, x2, x3, x4, y1, y2, y3, y4, z1, z2, z3, z4;
    for (int i = 0; i < stacks; ++i) {
        phi1 = (i * M_PI) / stacks;
        phi2 = ((i + 1) * M_PI) / stacks;

        for (int j = 0; j < slices; ++j) {
            theta1 = j * 2 * M_PI / slices;
            theta2 = (j + 1) * 2 * M_PI / slices;

            // Pontos
            x1 = radius * sin(phi1) * cos(theta1);
            y1 = radius * cos(phi1);
            z1 = radius * sin(phi1) * sin(theta1);

            x2 = radius * sin(phi2) * cos(theta1);
            y2 = radius * cos(phi2);
            z2 = radius * sin(phi2) * sin(theta1);

            x3 = radius * sin(phi1) * cos(theta2);
            y3 = radius * cos(phi1);
            z3 = radius * sin(phi1) * sin(theta2);

            x4 = radius * sin(phi2) * cos(theta2);
            y4 = radius * cos(phi2);
            z4 = radius * sin(phi2) * sin(theta2);

            // Triângulos
            p << x1 << " " << y1 << " " << z1;
            pointList.emplace_back(p.str());
            p.str("");

            p << x2 << " " << y2 << " " << z2;
            pointList.emplace_back(p.str());
            p.str("");

            p << x3 << " " << y3 << " " << z3;
            pointList.emplace_back(p.str());
            p.str("");

            p << x2 << " " << y2 << " " << z2;
            pointList.emplace_back(p.str());
            p.str("");

            p << x4 << " " << y4 << " " << z4;
            pointList.emplace_back(p.str());
            p.str("");

            p << x3 << " " << y3 << " " << z3;
            pointList.emplace_back(p.str());
            p.str("");
        }
    }
    saveFile(filename, pointList, "sphere", false);
}

void cone(double radius, double height, int slices, int stacks, char* filename)
{
    stacks++;
    std::list<std::string> pointList;
    std::stringstream p;

    // Generate vertices for the base
    for (int i = 0; i < slices; ++i) {
        double theta = i * 2 * M_PI / slices;
        double x = radius * cos(theta);
        double z = radius * sin(theta);

        p << x << " 0 " << z;
        pointList.emplace_back(p.str());
        p.str("");
    }

    // Generate vertices for the sides
    double delta_height = height / (stacks - 1); // Adjusted for apex
    double delta_theta = 2 * M_PI / slices;

    for (int i = 0; i < stacks; ++i) {
        double y = i * delta_height;
        double r = radius * (1.0 - y / height);

        for (int j = 0; j < slices; ++j) {
            double theta1 = j * delta_theta;
            double theta2 = (j + 1) * delta_theta;

            double x1 = r * cos(theta1);
            double z1 = r * sin(theta1);

            double x2 = r * cos(theta2);
            double z2 = r * sin(theta2);

            // Push vertices for the two triangles forming the side face
            p << x1 << " " << y << " " << z1;
            pointList.emplace_back(p.str());
            p.str("");

            p << x2 << " " << y << " " << z2;
            pointList.emplace_back(p.str());
            p.str("");

            p << "0 " << height << " 0"; // Apex
            pointList.emplace_back(p.str());
            p.str("");
        }
    }

    saveFile(filename, pointList, "cone", false);
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
        plane(std::stod(argv[2]), std::stoi(argv[3]), " 0 ", argv[4],
              "plane", false, 'y');
    }
    else if (std::strcmp(argv[1], "box") == 0)
    {
        if(argc != 5)
        {
            printArgumentError();
            return 0;
        }
        box(std::stod(argv[2]), std::stoi(argv[3]), argv[4]);
    }
    else if (std::strcmp(argv[1], "sphere") == 0)
    {
        if(argc != 6)
        {
            printArgumentError();
            return 0;
        }
        sphere(std::stod(argv[2]), std::stoi(argv[3]), std::stoi(argv[4]), argv[5]);
    }
    else if (std::strcmp(argv[1], "cone") == 0)
    {
        if(argc != 7)
        {
            printArgumentError();
            return 0;
        }
        cone(std::stod(argv[2]), std::stod(argv[3]), std::stoi(argv[4]),
             std::stoi(argv[5]), argv[6]);
    }
    else
    {
        printArgumentError();
        return 0;
    }
}
