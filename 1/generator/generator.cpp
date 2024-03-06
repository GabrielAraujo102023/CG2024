#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#endif
#include <cstring>
#include <cstdio>
#include <string>
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
void planeNextPoint(int rowsCalculated, float halfLength, float divisionSize, float& nextX, float& nextZ)
{
    // Z = Metade da largura do quadrado menos a altura das linhas já calculadas
    nextZ = halfLength - (divisionSize * rowsCalculated);

    nextX = halfLength;
}

float calculatePlaneY(float x, float z, float divisionSize, const std::string& y, std::list<std::string>& pointList)
{
    std::stringstream p1, p2, p3, p4;
    float xMin = x - divisionSize;
    float zMin = z - divisionSize;
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
float calculatePlaneX(float y, float z, float divisionSize, const std::string& x, std::list<std::string>& pointList)
{
    std::stringstream p1, p2, p3, p4;
    float yMin = y - divisionSize;
    float zMin = z - divisionSize;
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
float calculatePlaneZ(float x, float y, float divisionSize, const std::string& z, std::list<std::string>& pointList)
{
    std::stringstream p1, p2, p3, p4;
    float xMin = x - divisionSize;
    float yMin = y - divisionSize;
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

float calculatePlane(float a, float b, float divisionSize, const std::string& constant,
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
bool plane(float length, int divisions, const std::string& constant, char* filename, const std::string& firstLine,
           bool append, char axis)
{
    std::list<std::string> pointList = {};
    int rowsCalculated = 0;
    //Tamanho de cada quadrado que forma o plano
    float divisionSize = length / divisions;
    //Metade da largura do plano, para calcular teorema de pitágoras
    float halfLength = length / 2;
    //Pontos nao constantes do plano. Valor inicial é o canto superior direito do plano
    float a = halfLength, b = halfLength;
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
void box(float length, int divisions, char* filename)
{
    float halfLength = length / 2;
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

// Calcular esfera
void sphere(float radius, int slices, int stacks, char* filename)
{
    std::list<std::string> pointList = {};
    std::stringstream p;
    float phi1, phi2, theta1, theta2;
    float x1, x2, x3, x4, y1, y2, y3, y4, z1, z2, z3, z4;
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

// Calcular cone
void cone(float radius, float height, int slices, int stacks, char* filename)
{
    std::list<std::string> pointList;
    std::stringstream p;
    pointList.emplace_back("0 0 0");

    /*
    // Base
    for (int i = 0; i < slices; ++i) {
        float theta = i * 2 * M_PI / slices;
        float x = radius * cos(theta);
        float z = radius * sin(theta);

        p << x << " 0 " << z;
        pointList.emplace_back(p.str());
        p.str("");
    }
     */

    for (int i = 0; i <= slices; ++i) {
        float angle = (i / (float)slices) * 2.0f * M_PI;
        p << radius * cos(angle) << " 0 " << radius * sin(angle);
        pointList.emplace_back(p.str());
        p.str("");
    }

    /*
    // Lado
    float delta_height = height / (stacks - 1);
    float delta_theta = 2 * M_PI / slices;

    for (int i = 0; i < stacks; ++i) {
        float y = i * delta_height;
        float r = radius * (1.0 - y / height);

        for (int j = 0; j < slices; ++j) {
            float theta1 = j * delta_theta;
            float theta2 = (j + 1) * delta_theta;

            float x1 = r * cos(theta1);
            float z1 = r * sin(theta1);

            float x2 = r * cos(theta2);
            float z2 = r * sin(theta2);

            p << x1 << " " << y << " " << z1;
            pointList.emplace_back(p.str());
            p.str("");

            p << x2 << " " << y << " " << z2;
            pointList.emplace_back(p.str());
            p.str("");

            p << "0 " << height << " 0";
            pointList.emplace_back(p.str());
            p.str("");
        }
    }
    */
    pointList.emplace_back("triang");

    float stackHeight = height / stacks;
    float stackRadius = radius / stacks;
    for (int i = 0; i < stacks; ++i) {
        float z0 = i * stackHeight;
        float z1 = (i + 1) * stackHeight;
        float r0 = radius - i * stackRadius;
        float r1 = radius - (i + 1) * stackRadius;

        for (int j = 0; j < slices; ++j) {
            float angle0 = (j / (float)slices) * 2.0f * M_PI;
            float angle1 = ((j + 1) / (float)slices) * 2.0f * M_PI;

            // Triangle 1
            p << r0 * cos(angle0) << " " << z0 << " " << r0 * sin(angle0);
            pointList.emplace_back(p.str());
            p.str("");
            p << r0 * cos(angle1) << " " << z0 << " " << r0 * sin(angle1);
            pointList.emplace_back(p.str());
            p.str("");
            p << r1 * cos(angle1) << " " << z1 << " " << r1 * sin(angle1);
            pointList.emplace_back(p.str());
            p.str("");

            // Triangle 2
            p << r0 * cos(angle0) << " " << z0 << " " << r0 * sin(angle0);
            pointList.emplace_back(p.str());
            p.str("");
            p << r1 * cos(angle1) << " " << z1 << " " << r1 * sin(angle1);
            pointList.emplace_back(p.str());
            p.str("");
            p << r1 * cos(angle0) << " " << z1 << " " << r1 * sin(angle0);
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
             std::stoi(argv[5]) + 1, argv[6]);
    }
    else
    {
        printArgumentError();
        return 0;
    }
}
