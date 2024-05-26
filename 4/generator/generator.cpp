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
#include <iostream>
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

// Calcula normais de um triangulo
float *normal(float p1[3], float p2[3], float p3[3])
{
    float *normal = NULL;
    normal = (float*)malloc(3 * sizeof(float));

    float v2[3] = { p3[0] - p1[0], p3[1] - p1[1], p3[2] - p1[2] };
    float v1[3] = { p2[0] - p1[0], p2[1] - p1[1], p2[2] - p1[2] };

    float v[3];
    v[0] = v1[1] * v2[2] - v1[2] * v2[1];
    v[1] = v1[2] * v2[0] - v1[0] * v2[2];
    v[2] = v1[0] * v2[1] - v1[1] * v2[0];

    float vn = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);

    normal[0] = v[0] / vn;
    normal[1] = v[1] / vn;
    normal[2] = v[2] / vn;

    return normal;
}

bool saveNormals(char *filename, bool append, const list<float> &normals) {
    if (normals.empty()) {
        printf("Normals empty.\n");
        return true;
    }

    ofstream file;
    string folder = "../normals/";
    string normals_filename = folder + filename;
    if (append)
    {
        file.open(normals_filename, ios_base::app);
    }
    else
    {
        file.open(normals_filename);
    }
    if (!file.is_open())
    {
        printf("There was a problem with the normals file.");
        return false;
    }
    for(float f : normals)
    {
        file << f << " ";
    }
    file.close();
    return true;
}

void saveTextures(char *filename, const list<float> &textures, bool append) {
    if (textures.empty()) {
        printf("Textures empty.\n");
        return;
    }

    ofstream file;
    string folder = "../textures/";
    string textures_filename = folder + filename;
    if (append)
    {
        file.open(textures_filename, ios_base::app);
    }
    else
    {
        file.open(textures_filename);
    }
    if (!file.is_open())
    {
        printf("There was a problem with the textures file.");
        return;
    }
    for(float f : textures)
    {
        file << f << " ";
    }
    file.close();
}

//Guarda os pontos num ficheiro
//Pontos devem vir dentro de uma lista de strings assim --> "x y z"
//name é o texto que vai ser escrito na primeira linha
//append é um bool a dizer se o ficheiro é para ser escrito do 0 ou appended
bool saveFile(char* filename, const list<string>& points, const string& name, bool append, const list<float>& normals)
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
    file.close();
    saveNormals(filename, append, normals);
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
                      bool flipped, list<float>& normals)
{
    float dir = 1.0f;
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

        dir = -1;
    }

    for(int i = 0; i < 6; i++)
    {
        normals.emplace_back(0.0f);
        normals.emplace_back(dir);
        normals.emplace_back(0.0f);
    }

    return xMin;
}
float calculatePlaneX(float y, float z, float divisionSize, const string& x, list<string>& pointList,
                      bool flipped, list<float>& normals)
{
    float dir = 1.0f;
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

        dir = -1;
    }

    for(int i = 0; i < 6; i++)
    {
        normals.emplace_back(dir);
        normals.emplace_back(0.0f);
        normals.emplace_back(0.0f);
    }

    return yMin;
}
float calculatePlaneZ(float x, float y, float divisionSize, const string& z, list<string>& pointList,
                      bool flipped, list<float>& normals)
{
    float dir = 1.0f;
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

        dir = -1;
    }
    for(int i = 0; i < 6; i++)
    {
        normals.emplace_back(0.0f);
        normals.emplace_back(0.0f);
        normals.emplace_back(dir);
    }


    return xMin;
}

float calculatePlane(float a, float b, float divisionSize, const string& constant,
                     list<string>& pointList, char constAxis, bool flipped, list<float>& normals)
{
    switch(constAxis)
    {
        case 'x':
            return calculatePlaneX(a, b, divisionSize, constant, pointList, flipped, normals);
        case 'y':
            return calculatePlaneY(a, b, divisionSize, constant, pointList, flipped, normals);
        case 'z':
            return calculatePlaneZ(a, b, divisionSize, constant, pointList, flipped, normals);
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
    list<float> normals = {};
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
            a = calculatePlane(a, b, divisionSize, constant, pointList, axis, flipped, normals);
        }
        rowsCalculated++;
        planeNextPoint(rowsCalculated, halfLength, divisionSize, a, b);
    }
    return saveFile(filename, pointList, firstLine, append, normals);
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
    list<float> normals = {};
    list<float> textures = {};
    stringstream p;
    float alpha, beta;
    float p1x, p1y, p1z, p2x, p2y, p2z, p3x, p3y, p3z, p4x, p4y, p4z;
    float n1x, n1y, n1z, n2x, n2y, n2z, n3x, n3y, n3z, n4x, n4y, n4z;
    float t1x, t1z, t2x, t2z, t3x, t3z, t4x, t4z;
    float pi2 = M_PI * 2;
    float dalpha = pi2 / slices;
    float dbeta = (float) M_PI / stacks;
    for (int i = 0; i < stacks; ++i) {
        beta = i * dbeta;

        for (int j = 0; j < slices; ++j) {
            alpha = j * dalpha;

            // Pontos
            p1x = radius*sin(beta)*cos(alpha);
            p1y = radius*sin(beta)*sin(alpha);
            p1z = radius*cos(beta);

            n1x = sin(beta)*cos(alpha);
            n1y = sin(beta)*sin(alpha);
            n1z = cos(beta);

            t1x =  alpha / (2 * M_PI);
            t1z =  beta / ( M_PI);

            p2x = radius*sin((beta + dbeta))*cos(alpha);
            p2y = radius*sin((beta + dbeta))*sin(alpha);
            p2z = radius*cos((beta + dbeta));

            n2x = sin((beta + dbeta))*cos(alpha);
            n2y = sin((beta + dbeta))*sin(alpha);
            n2z = cos((beta + dbeta));

            t2x = alpha  / (2 * M_PI);
            t2z = (beta+dbeta) / (M_PI);

            p3x = radius*sin(beta)*cos((alpha + dalpha));
            p3y = radius*sin(beta)*sin((alpha + dalpha));
            p3z = radius*cos(beta);

            n3x = sin(beta)*cos((alpha + dalpha));
            n3y = sin(beta)*sin((alpha + dalpha));
            n3z = cos(beta);

            t3x = (alpha + dalpha ) / (2 * M_PI);
            t3z = beta / (M_PI);

            p4x = radius*sin((beta + dbeta))*cos((alpha + dalpha));
            p4y = radius*sin((beta + dbeta))*sin((alpha + dalpha));
            p4z = radius*cos((beta + dbeta));

            n4x = sin((beta + dbeta))*cos((alpha + dalpha));
            n4y = sin((beta + dbeta))*sin((alpha + dalpha));
            n4z = cos((beta + dbeta));

            t4x = (alpha + dalpha) / (2 * M_PI);
            t4z = (beta + dbeta) / ( M_PI);

            // Triângulos
            p << p1x << " " << p1y << " " << p1z;
            pointList.emplace_back(p.str());
            p.str("");
            normals.emplace_back(n1x);
            normals.emplace_back(n1y);
            normals.emplace_back(n1z);
            textures.emplace_back(t1x);
            textures.emplace_back(t1z);

            p << p2x << " " << p2y << " " << p2z;
            pointList.emplace_back(p.str());
            p.str("");
            normals.emplace_back(n2x);
            normals.emplace_back(n2y);
            normals.emplace_back(n2z);
            textures.emplace_back(t2x);
            textures.emplace_back(t2z);

            p << p3x << " " << p3y << " " << p3z;
            pointList.emplace_back(p.str());
            p.str("");
            normals.emplace_back(n3x);
            normals.emplace_back(n3y);
            normals.emplace_back(n3z);
            textures.emplace_back(t3x);
            textures.emplace_back(t3z);

            // Second triangle
            p << p3x << " " << p3y << " " << p3z;
            pointList.emplace_back(p.str());
            p.str("");
            normals.emplace_back(n3x);
            normals.emplace_back(n3y);
            normals.emplace_back(n3z);
            textures.emplace_back(t3x);
            textures.emplace_back(t3z);

            p << p2x << " " << p2y << " " << p2z;
            pointList.emplace_back(p.str());
            p.str("");
            normals.emplace_back(n2x);
            normals.emplace_back(n2y);
            normals.emplace_back(n2z);
            textures.emplace_back(t2x);
            textures.emplace_back(t2z);

            p << p4x << " " << p4y << " " << p4z;
            pointList.emplace_back(p.str());
            p.str("");
            normals.emplace_back(n4x);
            normals.emplace_back(n4y);
            normals.emplace_back(n4z);
            textures.emplace_back(t4x);
            textures.emplace_back(t4z);
        }
    }
    saveFile(filename, pointList, "sphere", false, normals);
    saveTextures(filename, textures, false);
}


// Calcular cone
void cone(float radius, float height, int slices, int stacks, char* filename)
{
    list<string> pointList;
    list<float> normals = {};
    stringstream p;

    for (int i = 0; i < slices; ++i) {
        float angle1 = 2.0f * M_PI * i / slices;
        float angle2 = 2.0f * M_PI * (i + 1) / slices;

        // Add center vertex (0, 0, 0)
        pointList.emplace_back("0 0 0");

        // First triangle vertex
        p << radius * cos(angle2) << " 0 " << radius * sin(angle2);
        pointList.emplace_back(p.str());
        p.str("");

        // Second triangle vertex
        p << radius * cos(angle1) << " 0 " << radius * sin(angle1);
        pointList.emplace_back(p.str());
        p.str("");

        // Third triangle vertex (repeated for next triangle)
        p << radius * cos(angle1) << " 0 " << radius * sin(angle1);
        pointList.emplace_back(p.str());
        p.str("");

        // Fourth triangle vertex (repeated for next triangle)
        p << radius * cos(angle2) << " 0 " << radius * sin(angle2);
        pointList.emplace_back(p.str());
        p.str("");

        normals.emplace_back(0.0f);
        normals.emplace_back(-1.0f);
        normals.emplace_back(0.0f);
        normals.emplace_back(0.0f);
        normals.emplace_back(-1.0f);
        normals.emplace_back(0.0f);
        normals.emplace_back(0.0f);
        normals.emplace_back(-1.0f);
        normals.emplace_back(0.0f);
        normals.emplace_back(0.0f);
        normals.emplace_back(-1.0f);
        normals.emplace_back(0.0f);
        normals.emplace_back(0.0f);
        normals.emplace_back(-1.0f);
        normals.emplace_back(0.0f);
    }

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
    saveFile(filename, pointList, "cone", false, normals);
}

float getBezierPoint(float u, float v, int coord, float ** vertices, int * indices) {
    float pointValue = 0;

    float bu[4][1] = { { powf(1 - u, 3) },{ 3 * u * powf(1 - u, 2) },{ 3 * powf(u, 2) * (1 - u) },{ powf(u, 3) } };
    float bv[4][1] = { { powf(1 - v, 3) },{ 3 * v * powf(1 - v, 2) },{ 3 * powf(v, 2) * (1 - v) },{ powf(v, 3) } };

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            pointValue += vertices[indices[j + 4 * i]][coord] * bu[i][0] * bv[j][0];
        }
    }
    return pointValue;
}

list<string> renderPatch(float ** controlPoints, int patches, int ** indices, float tessel) {
    float x, y, z;
    float v, u;
    stringstream ret;
    list<string> pointList;
    float tesselation = 1.0 / tessel;
    for (int patch = 0; patch < patches; patch++) {
        int * indicesPatch = indices[patch];
        for (v = 0; v < 1; v += tesselation) {
            for (u = 0; u < 1; u += tesselation) {
                x = getBezierPoint(u, v, 0, controlPoints, indicesPatch);
                y = getBezierPoint(u, v, 1, controlPoints, indicesPatch);
                z = getBezierPoint(u, v, 2, controlPoints, indicesPatch);
                ret << x << " " << y << " " << z;
                pointList.emplace_back(ret.str());
                ret.str("");
                x = getBezierPoint(u + tesselation, v, 0, controlPoints, indicesPatch);
                y = getBezierPoint(u + tesselation, v, 1, controlPoints, indicesPatch);
                z = getBezierPoint(u + tesselation, v, 2, controlPoints, indicesPatch);
                ret << x << " " << y << " " << z ;
                pointList.emplace_back(ret.str());
                ret.str("");
                x = getBezierPoint(u, v + tesselation, 0, controlPoints, indicesPatch);
                y = getBezierPoint(u, v + tesselation, 1, controlPoints, indicesPatch);
                z = getBezierPoint(u, v + tesselation, 2, controlPoints, indicesPatch);
                ret << x << " " << y << " " << z ;
                pointList.emplace_back(ret.str());
                ret.str("");
                x = getBezierPoint(u, v + tesselation, 0, controlPoints, indicesPatch);
                y = getBezierPoint(u, v + tesselation, 1, controlPoints, indicesPatch);
                z = getBezierPoint(u, v + tesselation, 2, controlPoints, indicesPatch);
                ret << x << " " << y << " " << z ;
                pointList.emplace_back(ret.str());
                ret.str("");
                x = getBezierPoint(u + tesselation, v, 0, controlPoints, indicesPatch);
                y = getBezierPoint(u + tesselation, v, 1, controlPoints, indicesPatch);
                z = getBezierPoint(u + tesselation, v, 2, controlPoints, indicesPatch);
                ret << x << " " << y << " " << z ;
                pointList.emplace_back(ret.str());
                ret.str("");
                x = getBezierPoint(u + tesselation, v + tesselation, 0, controlPoints, indicesPatch);
                y = getBezierPoint(u + tesselation, v + tesselation, 1, controlPoints, indicesPatch);
                z = getBezierPoint(u + tesselation, v + tesselation, 2, controlPoints, indicesPatch);
                ret << x << " " << y << " " << z ;
                pointList.emplace_back(ret.str());
                ret.str("");
            }
        }
    }
    return pointList;
}

void patches(string patchFile, float tessellation, char* filename) {
    string line;
    ifstream infile;
    int i, j, patches, points, **indices;
    float **controlPoints;
    infile.open(patchFile);
    getline(infile, line);
    patches = stoi(line);
    indices = (int **)malloc(sizeof(int*)*patches);

    for (i = 0; i < patches; i++) {
        indices[i] = (int *)malloc(sizeof(int) * 16);
    }

    for (i = 0; i < patches; i++) {
        getline(infile, line);
        std::istringstream ss(line);
        std::string token;
        j = 0;
        while (std::getline(ss, token, ',')) {
            indices[i][j] = stoi(token);
            j++;
        }
    }
    getline(infile, line);
    points = stoi(line);
    controlPoints = (float **)malloc(sizeof(float*)*points);
    for (i = 0; i < points; i++) {
        controlPoints[i] = (float *)malloc(sizeof(float) * 3);
    }
    for (i = 0; i < points; i++) {
        getline(infile, line);
        std::istringstream ss(line);
        std::string token;
        j = 0;
        while (std::getline(ss, token, ',')) {
            controlPoints[i][j] = stof(token);
            j++;
        }
    }
    list<string> pointList = renderPatch(controlPoints, patches, indices, tessellation);
    saveFile(filename, pointList, "patches", false, {});
    free(indices);
    free(controlPoints);
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