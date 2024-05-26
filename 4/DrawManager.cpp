#include "DrawManager.h"
#include <utility>
#include <valarray>
#include <fstream>
#include <sstream>
#include <IL/il.h>

DrawManager* instance;
int frameCount = 0;
int previousTime = 0;
int startX, startY, tracking = 0;
int a = 0, b = 45, r = 0;
float camX, camY, camZ;
float moveSpeed = 0.5f;

float m[4][4] = {	{-0.5f,  1.5f, -1.5f,  0.5f},
                     { 1.0f, -2.5f,  2.0f, -0.5f},
                     {-0.5f,  0.0f,  0.5f,  0.0f},
                     { 0.0f,  1.0f,  0.0f,  0.0f}};
float yaxis[3] = {0, 1, 0};

void update(int value) {
    int elapsedTime = glutGet(GLUT_ELAPSED_TIME);
    int deltaTime = elapsedTime - previousTime;
    if (deltaTime > 1000) {
        float fps = (float) frameCount / ((float) deltaTime / 1000.0f);

        ostringstream windowTitle;
        windowTitle << fps;
        glutSetWindowTitle(windowTitle.str().c_str());

        frameCount = 0;
        previousTime = elapsedTime;
    }
    frameCount++;
    glutTimerFunc(1000 / 60, update, 0);
}


DrawManager::DrawManager(int windowWidth, int windowHeight, float px, float py, float pz, float lx, float ly, float lz,
                         float ux, float uy, float uz, float fov, float near, float far, int argc,
                         char **argv, list<Group> rootGroup, list<Light> lights) {
    this->windowWidth = windowWidth;
    this->windowHeight = windowHeight;
    this->px = px;
    this->py = py;
    this->pz = pz;
    this->lx = lx;
    this->ly = ly;
    this->lz = lz;
    this->ux = ux;
    this->uy = uy;
    this->uz = uz;
    this->fov = fov;
    this->near = near;
    this->far = far;
    this->argc = argc;
    this->argv = argv;
    this->rootGroup = std::move(rootGroup);
    this->lights = std::move(lights);
}

void DrawManager::changeSize(int w, int h) {

    // Prevent a divide by zero, when window is too short
    // (you cant make a window with zero width).
    if(instance->windowHeight == 0)
        instance->windowHeight = 1;

    // compute window's aspect ratio
    float ratio = (float) instance->windowWidth * 1.0f / (float) instance->windowHeight;

    // Set the projection matrix as current
    glMatrixMode(GL_PROJECTION);
    // Load Identity Matrix
    glLoadIdentity();

    // Set the viewport to be the entire window
    glViewport(0, 0, instance->windowWidth, instance->windowHeight);

    // Set perspective
    gluPerspective(instance->fov ,ratio, instance->near ,instance->far);

    // return to the model view matrix mode
    glMatrixMode(GL_MODELVIEW);
}

void DrawManager::renderScene() {
    GLenum light_id;
    int i = 0;
    for(const Light& light : instance->lights) {
        light_id = 0x4000 + i;
        if (light.type != "directional")
        {
            float light_position[4] = {light.posX, light.posY, light.posZ, 1.0f};
            glLightfv(light_id, GL_POSITION, light_position);
        }
        else
        {
            float light_direction[4] = {light.dirX, light.dirY, light.dirZ, 0.0f};
            glLightfv(light_id, GL_POSITION, light_direction);
        }
        if (light.type == "spotlight")
        {
            float light_direction[3] = {light.dirX, light.dirY, light.dirZ};
            glLightfv(light_id, GL_SPOT_DIRECTION, light_direction);
            glLightfv(light_id, GL_SPOT_CUTOFF, &light.cutoff);
        }
        i++;
    }

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    // clear buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // set the camera
    glLoadIdentity();
    gluLookAt(camX,camY,camZ,
              instance->lx,instance->ly,instance->lz,
              instance->ux,instance->uy,instance->uz);

    DrawManager::drawMyStuff(instance->rootGroup.front());

    // End of frame
    glutSwapBuffers();
}

void buildRotMatrix(const float *x, const float *y, const float *z, float *m) {

    m[0] = x[0]; m[1] = x[1]; m[2] = x[2]; m[3] = 0;
    m[4] = y[0]; m[5] = y[1]; m[6] = y[2]; m[7] = 0;
    m[8] = z[0]; m[9] = z[1]; m[10] = z[2]; m[11] = 0;
    m[12] = 0; m[13] = 0; m[14] = 0; m[15] = 1;
}

void cross(const float *aa, const float *bb, float *res) {

    res[0] = aa[1]*bb[2] - aa[2]*bb[1];
    res[1] = aa[2]*bb[0] - aa[0]*bb[2];
    res[2] = aa[0]*bb[1] - aa[1]*bb[0];
}


void normalize(float *aa) {

    float l = sqrt(aa[0] * aa[0] + aa[1] * aa[1] + aa[2] * aa[2]);
    aa[0] = aa[0] / l;
    aa[1] = aa[1] / l;
    aa[2] = aa[2] / l;
}

void multMatrixVector(const float *m, const float *v, float *res) {

    for (int j = 0; j < 4; ++j) {
        res[j] = 0;
        for (int k = 0; k < 4; ++k) {
            res[j] += v[k] * m[j * 4 + k];
        }
    }

}

void renderCurve(Group& rootGroup)
{
    map<char, float> values;
    try
    {
        values = rootGroup.transformation.at('t');
    }
    catch (out_of_range e)
    {
        return;
    }
    if (values['n'] == 0)
        return;
    vector<vector<float>> points = rootGroup.points.at((int) values['i']);
    glBegin(GL_LINE_LOOP);
    int nPoints = (int) values['n'];
    float T;
    int index;
    float pos[3];
    for (int t = 0; t <= 100; t++) {
        T = (t / 100.0f) * nPoints;
        index = floor(T);
        T = T - index;

        int indices[4];
        indices[0] = (index + nPoints-1)%nPoints;
        indices[1] = (indices[0]+1)%nPoints;
        indices[2] = (indices[1]+1)%nPoints;
        indices[3] = (indices[2]+1)%nPoints;

        for(int i = 0; i < 3; i++)
        {
            float p[4] = {
                    points[indices[0]][i],
                    points[indices[1]][i],
                    points[indices[2]][i],
                    points[indices[3]][i],
            };
            float A[4];

            multMatrixVector(reinterpret_cast<const float *>(m), p, A);
            pos[i] = ((T * T * T) * A[0]) + ((T * T) * A[1]) + (T * A[2]) + ((1.0f) * A[3]);
        }
        glVertex3f(pos[0], pos[1], pos[2]);
    }
    glEnd();
}

void doTransformations(Group& rootGroup)
{
    char transformations[3] = {'t', 'r', 's'};
    map<char, float> values;
    for(char type: transformations){
        try {
            values = rootGroup.transformation.at(type);
        }
        catch (out_of_range e)
        {
            continue;
        }
        switch (type){
            case 't':
                if(values['t'] == -1)
                {
                    glTranslatef(values['x'], values['y'], values['z']);
                }
                else
                {
                    int nPoints = (int) values['n'];
                    float t = nPoints * (glutGet(GLUT_ELAPSED_TIME) / 1000.0 / values['t']);
                    int index = floor(t);
                    t = t - (float) index;

                    int indices[4];
                    indices[0] = (index + nPoints-1)%nPoints;
                    indices[1] = (indices[0]+1)%nPoints;
                    indices[2] = (indices[1]+1)%nPoints;
                    indices[3] = (indices[2]+1)%nPoints;

                    float pos[3];
                    float deriv[3];
                    vector<vector<float>> points = rootGroup.points.at((int) values['i']);

                    for(int i = 0; i < 3; i++)
                    {
                        float p[4] = {
                                points[indices[0]][i],
                                points[indices[1]][i],
                                points[indices[2]][i],
                                points[indices[3]][i],
                        };
                        float A[4];

                        multMatrixVector(reinterpret_cast<const float *>(m), p, A);
                        pos[i] = ((t * t * t) * A[0]) + ((t * t) * A[1]) + (t * A[2]) + A[3];
                        deriv[i] = 3.0f * t*t * A[0] + 2.0f * t * A[1] + A[2];
                    }
                    glTranslatef(pos[0], pos[1], pos[2]);
                    // Align
                    if (values['a'] == 1)
                    {
                        float zaxis[3];
                        cross(deriv, yaxis, zaxis);
                        cross(zaxis, deriv, yaxis);
                        normalize(deriv);
                        normalize(yaxis);
                        normalize(zaxis);

                        float rotationMatrix[16];
                        buildRotMatrix(deriv, yaxis, zaxis, rotationMatrix);

                        glMultMatrixf(rotationMatrix);
                    }
                }
                break;
            case 'r':
                if(values['t'] == -1)
                {
                    glRotatef(values['a'], values['x'], values['y'], values['z']);
                }
                else
                {
                    if(values['i'] == 0)
                    {
                        values['i'] = (float) glutGet(GLUT_ELAPSED_TIME);
                    }
                    auto now = (float) glutGet(GLUT_ELAPSED_TIME);
                    values['r'] += (now - values['i']) * values['s'];
                    values['i'] = now;
                    glRotatef(values['r'], values['x'], values['y'], values['z']);
                    rootGroup.transformation['r'] = values;
                }
                break;
            case 's':
                glScalef(values['x'], values['y'], values['z']);
                break;
            default:
                break;
        }
    }
}

void DrawManager::drawMyStuff(Group& rootGroup)
{
    glPushMatrix();

    renderCurve(rootGroup);
    // Fazer transformações
    doTransformations(rootGroup);

    // Desenhar modelos
    glColor3f(1,1,1);
    for(const Group::Model& model : rootGroup.models)
    {
        glBindBuffer(GL_ARRAY_BUFFER, model.vboID);
        glVertexPointer(3, GL_FLOAT, 0, nullptr);

        if(model.normalID != -1)
        {
            glBindBuffer(GL_ARRAY_BUFFER, model.normalID);
            glNormalPointer(GL_FLOAT, 0, nullptr);
        }

        if(model.texIDBuffer != -1)
        {
            glEnable(GL_TEXTURE_2D);
            glBindBuffer(GL_ARRAY_BUFFER, model.texIDBuffer);
            glTexCoordPointer(2, GL_FLOAT, 0, nullptr);
            glBindTexture(GL_TEXTURE_2D, model.texIDIL);
        }

        int diff[4] = {model.color.diffuse[0], model.color.diffuse[1], model.color.diffuse[2], 1};
        int spec[4] = {model.color.specular[0], model.color.specular[1], model.color.specular[2], 1};
        int amb[4] = {model.color.ambient[0], model.color.ambient[1], model.color.ambient[2], 1};
        int emis[4] = {model.color.emissive[0], model.color.emissive[1], model.color.emissive[2], 1};
        GLint shininess = model.color.shininess;

        glMaterialiv(GL_BACK, GL_DIFFUSE, diff);
        glMaterialiv(GL_BACK, GL_SPECULAR, spec);
        glMaterialiv(GL_BACK, GL_AMBIENT, amb);
        glMaterialiv(GL_BACK, GL_EMISSION, emis);
        glMateriali(GL_BACK, GL_SHININESS, shininess);

        glDrawArrays(GL_TRIANGLES, 0, model.nPoints);
        if(model.texIDBuffer != -1)
        {
            glDisable(GL_TEXTURE_2D);
        }
    }

    for(Group& g : rootGroup.groups)
    {
        DrawManager::drawMyStuff(g);
    }
    glPopMatrix();
}

void DrawManager::processKeys(unsigned char c, int xx, int yy) {
    // Vetor que vai da câmara ao ponto que estamos a olhar
    float forwardX = instance->lx - camX;
    float forwardY = instance->ly - camY;
    float forwardZ = instance->lz - camZ;

    // Normallização
    float forwardLength = sqrt(forwardX * forwardX + forwardY * forwardY + forwardZ * forwardZ);
    forwardX /= forwardLength;
    forwardY /= forwardLength;
    forwardZ /= forwardLength;

    // Vetor da direita da câmara
    float rightX = forwardY * instance->uy - forwardZ * instance->uz;
    float rightY = forwardZ * instance->ux - forwardX * instance->uz;
    float rightZ = forwardX * instance->uy - forwardY * instance->ux;

    switch(c)
    {
        case 'a':
            camX -= moveSpeed * rightX;
            camY -= moveSpeed * rightY;
            camZ -= moveSpeed * rightZ;

            instance->lx -= moveSpeed * rightX;
            instance->ly -= moveSpeed * rightY;
            instance->lz -= moveSpeed * rightZ;
            break;
        case 'd':
            camX += moveSpeed * rightX;
            camY += moveSpeed * rightY;
            camZ += moveSpeed * rightZ;

            instance->lx += moveSpeed * rightX;
            instance->ly += moveSpeed * rightY;
            instance->lz += moveSpeed * rightZ;
            break;
        default:
            break;
    }
    r = (int) sqrt(camX * camX + camY * camY + camZ * camZ);
    glutPostRedisplay();
}


void DrawManager::processSpecialKeys(int key, int xx, int yy) {
    switch(key)
    {
        case GLUT_KEY_UP:
            moveSpeed += 0.1;
            break;
        case GLUT_KEY_DOWN:
            moveSpeed -= 0.1;
            break;
        default:
            break;
    }
    glutPostRedisplay();
}

void processMouseButtons(int button, int state, int xx, int yy) {
    if (state == GLUT_DOWN)  {
        startX = xx;
        startY = yy;
        if (button == GLUT_LEFT_BUTTON)
            tracking = 1;
        else if (button == GLUT_RIGHT_BUTTON)
            tracking = 2;
        else
            tracking = 0;
    }
    else if (state == GLUT_UP) {
        if (tracking == 1) {
            a += (xx - startX);
            b += (yy - startY);
        }
        else if (tracking == 2) {

            r -= yy - startY;
            if (r < 3)
                r = 3.0;
        }
        tracking = 0;
    }
}


void processMouseMotion(int xx, int yy) {
    int deltaX, deltaY;
    int alphaAux = 0, betaAux = 0, rAux = 0;

    if (!tracking)
        return;

    deltaX = xx - startX;
    deltaY = yy - startY;

    if (tracking == 1) {
        alphaAux = a + deltaX;
        betaAux = b + deltaY;

        if (betaAux > 85.0)
            betaAux = 85.0;
        else if (betaAux < -85.0)
            betaAux = -85.0;

        rAux = r;
    }
    else if (tracking == 2) {

        alphaAux = a;
        betaAux = b;
        rAux = r - deltaY;
        if (rAux < 3)
            rAux = 3;
    }
    camX = (float) rAux * sin((float) alphaAux * 3.14f / 180.0f) * cos((float) betaAux * 3.14f / 180.0f);
    camZ = (float )rAux * cos((float) alphaAux * 3.14f / 180.0f) * cos((float) betaAux * 3.14f / 180.0f);
    camY = (float )rAux * sin((float) betaAux * 3.14f / 180.0f);
    glutPostRedisplay();
}

void initLighting() {
    int i = 0;
    float dark[4] = {0.2, 0.2, 0.2, 1.0};
    float white[4] = {1.0, 1.0, 1.0, 1.0};
    GLenum light_id;

    glShadeModel(GL_SMOOTH);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, new float[4]{ 1.0f, 1.0f, 1.0f, 1.0f });
    for(DrawManager::Light light : instance->lights) {
        // GL_LIGHT0 = 0x4000, GL_LIGHT1 = 0x4001, etc...
        light_id = 0x4000 + i;
        glEnable(light_id);
        glLightfv(light_id, GL_AMBIENT, dark);
        glLightfv(light_id, GL_DIFFUSE, white);
        glLightfv(light_id, GL_SPECULAR, white);
        i++;
    }
}

void DrawManager::Draw() {
    instance = this;
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow("Welcome back, Mr. Stark");


    // Required callback registry
    ::glutDisplayFunc(renderScene);
    ::glutReshapeFunc(changeSize);
    glutIdleFunc(renderScene);

    // Callback registration for keyboard processing
    glutKeyboardFunc(processKeys);
    glutSpecialFunc(processSpecialKeys);
    glutMouseFunc(processMouseButtons);
    glutMotionFunc(processMouseMotion);

    glutTimerFunc(0, update, 0);

    camX = this->px;
    camY = this->py;
    camZ = this->pz;
    r = (int) sqrt(this->px * this->px + this->py * this->py + this->pz * this->pz);
    a = (int) acos(instance->pz / (float) r);
    b = (int) atan2(instance->py, instance->px);

    glewInit();
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glEnable(GL_TEXTURE_2D);
    loadModels(&instance->rootGroup.front());

    // OpenGL settings
    glEnable(GL_DEPTH_TEST);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINES);
    glEnable(GL_LIGHTING);
    initLighting();
    glutMainLoop();
}


unsigned int loadTexture(const string& f) {
    string s = "../textures/" + f;
    unsigned int t, tw, th;
    unsigned char *texData;
    unsigned int texID;

    ilInit();
    ilEnable(IL_ORIGIN_SET);
    ilOriginFunc(IL_ORIGIN_UPPER_LEFT);
    ilGenImages(1, &t);
    ilBindImage(t);
    ilLoadImage((ILstring)s.c_str());
    if (ilGetError() != IL_NO_ERROR) {
        printf("Erro a carregar imagem: %s\n", s.c_str());
        return 0;
    }

    tw = ilGetInteger(IL_IMAGE_WIDTH);
    th = ilGetInteger(IL_IMAGE_HEIGHT);
    ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
    texData = ilGetData();

    glGenTextures(1, &texID);

    glBindTexture(GL_TEXTURE_2D, texID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tw, th, 0, GL_RGBA, GL_UNSIGNED_BYTE, texData);
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);

    if (ilGetError() != IL_NO_ERROR) {
        printf("Erro a carregar imagem: %s\n", s.c_str());
        return 0;
    }

    return texID;

}

void initNormals(Group::Model *model){
    float normal;
    string folder = "../normals/";
    string normals_filename = folder + model->file;
    ifstream file;
    GLuint id;
    vector<GLfloat> normals = {};
    file.open(normals_filename);
    if (!file.is_open()) {
        printf("There was an error reading the normals file: %s", model->file.c_str());
        model->normalID = -1;
        return;
    }
    while(file >> normal) {
        normals.emplace_back(normal);
    }

    glGenBuffers(1, &id);
    glBindBuffer(GL_ARRAY_BUFFER, id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * normals.size(), normals.data(), GL_STATIC_DRAW);
    model->normalID = id;
}

void initTextures(Group::Model* model){
    float texture_point;
    string folder = "../textures/";
    string texture_points_filename = folder + model->file;
    ifstream file;
    GLuint id;
    vector<GLfloat> texture_points = {};
    file.open(texture_points_filename);
    if (!file.is_open()) {
        printf("There was an error reading the textures file: %s", model->file.c_str());
        model->texIDBuffer = -1;
        model->texIDIL = -1;
        return;
    }
    while(file >> texture_point) {
        texture_points.emplace_back(texture_point);
    }

    glGenBuffers(1, &id);
    glBindBuffer(GL_ARRAY_BUFFER, id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * texture_points.size(), texture_points.data(), GL_STATIC_DRAW);
    model->texIDBuffer = id;
    model->texIDIL = loadTexture(model->texture);
}

void initVBO(Group::Model *model) {
    ifstream file(model->file);
    if (!file.is_open()) {
        printf("There was a problem with the file: %s\n", model->file.c_str());
        return;
    }

    float point;
    stringstream p;
    string line;
    getline(file, line);
    vector<GLfloat> vertex;
    while (file >> point)
    {
        vertex.emplace_back(point);
    }

    GLuint id;
    unsigned int size = vertex.size();
    glGenBuffers(1, &id);
    glBindBuffer(GL_ARRAY_BUFFER, id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertex.size(), vertex.data(),
                 GL_STATIC_DRAW);
    model->vboID = id;
    model->nPoints = size;
}

void DrawManager::loadModels(Group *group) {
    for(Group::Model &model : group->models)
    {
        initVBO(&model);
        initNormals(&model);
        initTextures(&model);
    }

    for(Group &g : group->groups)
    {
        loadModels(&g);
    }
}