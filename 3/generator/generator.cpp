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
#include <vector>
#include <map>
#include <algorithm>
using namespace std;
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
bool saveFile(char* filename, const list<string>& points, const string& name, bool append)
{
    ofstream file;
    if (append)
    {
        file.open(filename, ios_base::app);
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
    for(const string& s : points)
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

float calculatePlaneY(float x, float z, float divisionSize, const string& y, list<string>& pointList,
                      bool flipped)
{
    stringstream p1, p2, p3, p4;
    float xMin = x - divisionSize;
    float zMin = z - divisionSize;
    p1 << x << y << z;
    p2 << xMin << y << z;
    p3 << xMin << y << zMin;
    p4 << x << y << zMin;

    if (!flipped)
    {
        pointList.emplace_back(p2.str());
        pointList.emplace_back(p3.str());
        pointList.emplace_back(p1.str());

        pointList.emplace_back(p3.str());
        pointList.emplace_back(p4.str());
        pointList.emplace_back(p1.str());
    }
    else
    {
        pointList.emplace_back(p1.str());
        pointList.emplace_back(p3.str());
        pointList.emplace_back(p2.str());

        pointList.emplace_back(p1.str());
        pointList.emplace_back(p4.str());
        pointList.emplace_back(p3.str());
    }

    return xMin;
}
float calculatePlaneX(float y, float z, float divisionSize, const string& x, list<string>& pointList,
                      bool flipped)
{
    stringstream p1, p2, p3, p4;
    float yMin = y - divisionSize;
    float zMin = z - divisionSize;
    p1 << x << y << " " << z;
    p2 << x << y << " " << zMin;
    p3 << x << yMin << " " << zMin;
    p4 << x << yMin << " " << z;

    if (!flipped)
    {
        pointList.emplace_back(p2.str());
        pointList.emplace_back(p3.str());
        pointList.emplace_back(p1.str());

        pointList.emplace_back(p3.str());
        pointList.emplace_back(p4.str());
        pointList.emplace_back(p1.str());
    }
    else
    {
        pointList.emplace_back(p1.str());
        pointList.emplace_back(p3.str());
        pointList.emplace_back(p2.str());

        pointList.emplace_back(p1.str());
        pointList.emplace_back(p4.str());
        pointList.emplace_back(p3.str());
    }

    return yMin;
}
float calculatePlaneZ(float x, float y, float divisionSize, const string& z, list<string>& pointList,
                      bool flipped)
{
    stringstream p1, p2, p3, p4;
    float xMin = x - divisionSize;
    float yMin = y - divisionSize;
    p1 << x << " " << y << z;
    p2 << xMin << " " << y << z;
    p3 << xMin << " " << yMin << z;
    p4 << x << " " << yMin << z;

    if (!flipped)
    {
        pointList.emplace_back(p2.str());
        pointList.emplace_back(p3.str());
        pointList.emplace_back(p1.str());

        pointList.emplace_back(p3.str());
        pointList.emplace_back(p4.str());
        pointList.emplace_back(p1.str());
    }
    else
    {
        pointList.emplace_back(p1.str());
        pointList.emplace_back(p3.str());
        pointList.emplace_back(p2.str());

        pointList.emplace_back(p1.str());
        pointList.emplace_back(p4.str());
        pointList.emplace_back(p3.str());
    }

    return xMin;
}

float calculatePlane(float a, float b, float divisionSize, const string& constant,
                     list<string>& pointList, char constAxis, bool flipped)
{
    switch(constAxis)
    {
        case 'x':
            return calculatePlaneX(a, b, divisionSize, constant, pointList, flipped);
        case 'y':
            return calculatePlaneY(a, b, divisionSize, constant, pointList, flipped);
        case 'z':
            return calculatePlaneZ(a, b, divisionSize, constant, pointList, flipped);
        default:
            break;
    }
    return 0;
}

//Calcula pontos para um plano
bool plane(float length, int divisions, const string& constant, char* filename, const string& firstLine,
           bool append, char axis, bool flipped)
{
    list<string> pointList = {};
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
            a = calculatePlane(a, b, divisionSize, constant, pointList, axis, flipped);
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
    stringstream constant;
    constant << halfLength << " ";
    plane(length, divisions, constant.str(), filename, "box", false, 'x', false);
    constant.str("");
    constant << "-" << halfLength << " ";
    plane(length, divisions, constant.str(), filename, "", true, 'x', true);
    constant.str("");

    constant << " " << halfLength << " ";
    plane(length, divisions, constant.str(), filename, "", true, 'y', false);
    constant.str("");
    constant << " " << "-" << halfLength << " ";
    plane(length, divisions, constant.str(), filename, "", true, 'y', true);
    constant.str("");

    constant << " " << halfLength;
    plane(length, divisions, constant.str(), filename, "", true, 'z', true);
    constant.str("");
    constant << " " << "-" << halfLength;
    plane(length, divisions, constant.str(), filename, "", true, 'z', false);
}

// Calcular esfera
void sphere(float radius, int slices, int stacks, char* filename)
{
    list<string> pointList = {};
    stringstream p;
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
    list<string> pointList;
    stringstream p;
    pointList.emplace_back("0 0 0");

    for (int i = 0; i <= slices; ++i) {
        float angle = (i / (float)slices) * 2.0f * M_PI;
        p << radius * cos(angle) << " 0 " << radius * sin(angle);
        pointList.emplace_back(p.str());
        p.str("");
    }

    pointList.emplace_back("triang");
    stacks--;
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

void fixLine(ifstream &patch, string line, stringstream &ss)
{
    getline(patch, line);
    //Remove as vírgulas
    line.erase(std::remove_if(line.begin(), line.end(),[](unsigned char x){return x == ',';}), line.end());
    ss.str(line);
}

int nCr(int n, int r) {
    if (r == 0 || r == n) {
        return 1;
    } else {
        return nCr(n - 1, r - 1) + nCr(n - 1, r);
    }
}

double blend(double u, double v, const vector<vector<vector<float>>>& controlPoints, int i, int j) {
    int n = controlPoints.size() - 1;
    int m = controlPoints[0].size() - 1;

    double bi = 1.0;
    double bj = 1.0;

    // Calculate Bernstein polynomials
    for (int k = 0; k <= n; ++k) {
        bi *= (double)std::pow(u, k) * (double)std::pow(1 - u, n - k) * (double)nCr(n, k);
    }
    for (int l = 0; l <= m; ++l) {
        bj *= (double)std::pow(v, l) * (double)std::pow(1 - v, m - l) * (double)nCr(m, l);
    }

    return bi * bj;
}

list<string> bezierSurfacePoint(double u, double v, const vector<vector<vector<float>>> controlPoints) {
    int n = controlPoints.size();
    int m = controlPoints[0].size();
    list<string> point; // Assuming 3D points
    stringstream p;
    float x,y,z;
    double b;

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            b = blend(u, v, controlPoints, i, j);
            x = b * controlPoints[i][j][0];
            y = b * controlPoints[i][j][1];
            z = b * controlPoints[i][j][2];
            p << x << " " << y << " " << z;
            point.emplace_back(p.str());
            p.str("");
        }
    }

    return point;
}

void patches(char* patchFile, int tessellation ,char* filename)
{
    ifstream patch;
    patch.open(patchFile);
    int nControl, nPoints;
    patch >> nControl;
    vector<vector<vector<float>>> controlPoints;
    map<int, vector<int>> pointIndex;
    string line;
    stringstream ss;
    int c1, c2, c3, c4, c5, c6, c7, c8, c9, c10, c11, c12, c13, c14, c15, c16;
    //Preparar matriz
    for(int i = 0; i < nControl; i++)
    {
        fixLine(patch, line, ss);
        ss >> c1 >> c2 >> c3 >> c4 >> c5 >> c6 >> c7 >> c8 >> c9 >> c10 >> c11 >> c12 >> c13 >> c14 >> c15 >> c16;
        vector<int> aux = {c1, c2, c3, c4, c5, c6, c7, c8, c9, c10, c11};
        for(int j = 0; j < 16; j++)
        {
            pointIndex[aux[j]] = {i, j};
        }
    }
    getline(patch, line);
    nPoints = stoi(line);
    float x,y,z;
    vector<int> index;
    //Colocar pontos na matriz
    for(int i = 0; i < nPoints; i++)
    {
        fixLine(patch, line, ss);
        ss >> x >> y >> z;
        index = pointIndex[i];
        controlPoints[index[0]][index[1]][0] = x;
        controlPoints[index[0]][index[1]][1] = y;
        controlPoints[index[0]][index[1]][2] = z;
    }

    double u, v;
    for(int i = 0; i < nControl; i++)
    {
        for(int j = 0; j < 16; j++)
        {
            u = i / (tessellation - 1);
            v = j / (tessellation - 1);
        }
    }
}

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        printArgumentError();
        return 0;
    }
    if (strcmp(argv[1], "plane") == 0)
    {
        if(argc != 5)
        {
            printArgumentError();
            return 0;
        }
        plane(stod(argv[2]), stoi(argv[3]), " 0 ", argv[4],
              "plane", false, 'y', false);
    }
    else if (strcmp(argv[1], "box") == 0)
    {
        if(argc != 5)
        {
            printArgumentError();
            return 0;
        }
        box(stod(argv[2]), stoi(argv[3]), argv[4]);
    }
    else if (strcmp(argv[1], "sphere") == 0)
    {
        if(argc != 6)
        {
            printArgumentError();
            return 0;
        }
        sphere(stod(argv[2]), stoi(argv[3]), stoi(argv[4]), argv[5]);
    }
    else if (strcmp(argv[1], "cone") == 0)
    {
        if(argc != 7)
        {
            printArgumentError();
            return 0;
        }
        cone(stod(argv[2]), stod(argv[3]), stoi(argv[4]),
             stoi(argv[5]) + 1, argv[6]);
    }
    else if (strcmp(argv[1], "patch") == 0)
    {
        if(argc != 5)
        {
            printArgumentError();
            return 0;
        }
        patches(argv[2], stoi(argv[3]), argv[4]);
    }
    else
    {
        printArgumentError();
        return 0;
    }
}