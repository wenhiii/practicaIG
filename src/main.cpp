#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Shaders.h"
#include "Model.h"
#include "Texture.h"

void configScene();
void renderScene();
void setLights(glm::mat4 P, glm::mat4 V);
void drawObjectMat(Model &model, Material material, glm::mat4 P, glm::mat4 V, glm::mat4 M);
void drawObjectTex(Model &model, Textures textures, glm::mat4 P, glm::mat4 V, glm::mat4 M);

void drawMO(glm::mat4 P, glm::mat4 V, glm::mat4 M);
void drawCuerpo(glm::mat4 P, glm::mat4 V, glm::mat4 M);
void drawCabeza(glm::mat4 P, glm::mat4 V, glm::mat4 M);
void drawSirena(glm::mat4 P, glm::mat4 V, glm::mat4 M);
void drawBrazoDer(glm::mat4 P, glm::mat4 V, glm::mat4 M);
void drawBrazoIzq(glm::mat4 P, glm::mat4 V, glm::mat4 M);
void drawRueda(glm::mat4 P, glm::mat4 V, glm::mat4 M);
void drawAspiradora(glm::mat4 P, glm::mat4 V, glm::mat4 M);

void funFramebufferSize(GLFWwindow *window, int width, int height);
void funKey(GLFWwindow *window, int key, int scancode, int action, int mods);
void funScroll(GLFWwindow *window, double xoffset, double yoffset);
void funCursorPos(GLFWwindow *window, double xpos, double ypos);

// Shaders
Shaders shaders;

// Modelos
Model sphere;
Model plane;
Model cube;

// M-O
Model aspiradora;
Model brazoDer;
Model brazoIzq;
Model cara;
Model casco;
Model cristalSirena;
Model cubreRueda;
Model cuello;
Model cuerpo;
Model mochila;
Model rueda;
Model tapaAspiradora;
Model tapaSirena;

// Imagenes (texturas)
Texture imgNoEmissive;
Texture imgRuby;
Texture imgGold;
Texture imgChess;
Texture imgCubeDiffuse;
Texture imgCubeSpecular;
Texture imgWallNormal;

// Imagenes TEXTURA ESCENARIO
Texture imgAxiomFloor;
Texture imgAxiomWall;
Texture imgGuideLine;

// FIN Imagenes TEXTURA ESCENARIO

// Luces y materiales
#define NLD 1
#define NLP 1
#define NLF 2
Light lightG;
Light lightD[NLD];
Light lightP[NLP];
Light lightF[NLF];
Material mluz;
Textures texRuby;
Textures texGold;
Textures texChess;
Textures texCube;
Textures texWall;

// TEXTURA ESCENARIO
Textures texAxiomFloor;
Textures texAxiomWall;
Textures texGuideLine;
// FIN TEXTURA ESCENARIO

//  Ancho del pasillo
float anchoPasillo = 10.0f;

// Viewport
int w = 500;
int h = 500;

// Movimiento de camara (Ratón)
float fovy = 60.0;
float alphaX = 0.0;
float alphaY = 0.0;

int main()
{

   // Inicializamos GLFW
   if (!glfwInit())
      return -1;
   glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
   glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
   glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

   // Creamos la ventana
   GLFWwindow *window;
   window = glfwCreateWindow(w, h, "Sesion 7", NULL, NULL);
   if (!window)
   {
      glfwTerminate();
      return -1;
   }
   glfwMakeContextCurrent(window);
   glfwSwapInterval(1);

   // Inicializamos GLEW
   glewExperimental = GL_TRUE;
   GLenum err = glewInit();
   if (GLEW_OK != err)
   {
      std::cout << "Error: " << glewGetErrorString(err) << std::endl;
      return -1;
   }
   std::cout << "Status: Using GLEW " << glewGetString(GLEW_VERSION) << std::endl;
   const GLubyte *oglVersion = glGetString(GL_VERSION);
   std::cout << "This system supports OpenGL Version: " << oglVersion << std::endl;

   // Configuramos los CallBacks
   glfwSetFramebufferSizeCallback(window, funFramebufferSize);
   glfwSetKeyCallback(window, funKey);
   glfwSetScrollCallback(window, funScroll);
   glfwSetCursorPosCallback(window, funCursorPos);

   // Entramos en el bucle de renderizado
   configScene();
   while (!glfwWindowShouldClose(window))
   {
      renderScene();
      glfwSwapBuffers(window);
      glfwPollEvents();
   }
   glfwDestroyWindow(window);
   glfwTerminate();

   return 0;
}

void configScene()
{

   // Test de profundidad
   glEnable(GL_DEPTH_TEST);

   // Transparencias
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

   // Shaders
   shaders.initShaders("resources/shaders/vshader.glsl", "resources/shaders/fshader.glsl");

   // Modelos
   sphere.initModel("resources/models/sphere.obj");
   plane.initModel("resources/models/plane.obj");
   cube.initModel("resources/models/cube.obj");

   // M-O
   aspiradora.initModel("resources/models/aspiradora.obj");
   brazoDer.initModel("resources/models/brazoDerecho.obj");
   brazoIzq.initModel("resources/models/brazoIzquierdo.obj");
   cara.initModel("resources/models/cara.obj");
   casco.initModel("resources/models/casco.obj");
   cristalSirena.initModel("resources/models/cristalSirena.obj");
   cubreRueda.initModel("resources/models/cubreRueda.obj");
   cuello.initModel("resources/models/cuello.obj");
   cuerpo.initModel("resources/models/cuerpo.obj");
   mochila.initModel("resources/models/mochila.obj");
   rueda.initModel("resources/models/rueda.obj");
   tapaAspiradora.initModel("resources/models/tapaAspiradora.obj");
   tapaSirena.initModel("resources/models/tapaSirena.obj");

   // Imagenes (texturas)
   imgNoEmissive.initTexture("resources/textures/imgNoEmissive.png");
   imgRuby.initTexture("resources/textures/imgRuby.png");
   imgGold.initTexture("resources/textures/imgGold.png");
   imgChess.initTexture("resources/textures/imgChess.png");
   imgCubeDiffuse.initTexture("resources/textures/imgCubeDiffuse.png");
   imgCubeSpecular.initTexture("resources/textures/imgCubeSpecular.png");
   imgWallNormal.initTexture("resources/textures/imgWallNormal.png");

   // [NUEVO] Cargamos texturas para el escenario (reutilizamos las existentes si no tienes nuevas)
   imgAxiomFloor.initTexture("resources/textures/texturaXD.jpg!d");
   imgAxiomWall.initTexture("resources/textures/paredXD.jpg");
   imgGuideLine.initTexture("resources/textures/XD.jpg");

   // Luz ambiental global
   lightG.ambient = glm::vec3(0.2, 0.2, 0.3);

   // Luces direccionales
   lightD[0].direction = glm::vec3(-1.0, 0.0, 0.0);
   lightD[0].ambient = glm::vec3(0.1, 0.1, 0.1);
   lightD[0].diffuse = glm::vec3(0.7, 0.7, 0.7);
   lightD[0].specular = glm::vec3(0.7, 0.7, 0.7);

   // Luces posicionales (Luz cenital del pasillo)
   lightP[0].position = glm::vec3(0.0, 4.0, 0.0);
   lightP[0].ambient = glm::vec3(0.2, 0.2, 0.2);
   lightP[0].diffuse = glm::vec3(0.9, 0.95, 1.0);
   lightP[0].specular = glm::vec3(1.0, 1.0, 1.0);
   lightP[0].c0 = 1.00;
   lightP[0].c1 = 0.09;
   lightP[0].c2 = 0.032;

   // Luces focales
   lightF[0].position = glm::vec3(-2.0, 2.0, 5.0);
   lightF[0].direction = glm::vec3(2.0, -2.0, -5.0);
   lightF[0].ambient = glm::vec3(0.2, 0.2, 0.2);
   lightF[0].diffuse = glm::vec3(0.9, 0.9, 0.9);
   lightF[0].specular = glm::vec3(0.9, 0.9, 0.9);
   lightF[0].innerCutOff = 10.0;
   lightF[0].outerCutOff = lightF[0].innerCutOff + 5.0;
   lightF[0].c0 = 1.000;
   lightF[0].c1 = 0.090;
   lightF[0].c2 = 0.032;
   lightF[1].position = glm::vec3(2.0, 2.0, 5.0);
   lightF[1].direction = glm::vec3(-2.0, -2.0, -5.0);
   lightF[1].ambient = glm::vec3(0.2, 0.2, 0.2);
   lightF[1].diffuse = glm::vec3(0.9, 0.9, 0.9);
   lightF[1].specular = glm::vec3(0.9, 0.9, 0.9);
   lightF[1].innerCutOff = 5.0;
   lightF[1].outerCutOff = lightF[1].innerCutOff + 1.0;
   lightF[1].c0 = 1.000;
   lightF[1].c1 = 0.090;
   lightF[1].c2 = 0.032;

   // Materiales existentes
   mluz.ambient = glm::vec4(0.0, 0.0, 0.0, 1.0);
   mluz.diffuse = glm::vec4(0.0, 0.0, 0.0, 1.0);
   mluz.specular = glm::vec4(0.0, 0.0, 0.0, 1.0);
   mluz.emissive = glm::vec4(1.0, 1.0, 1.0, 1.0);
   mluz.shininess = 1.0;

   texRuby.diffuse = imgRuby.getTexture();
   texRuby.specular = imgRuby.getTexture();
   texRuby.emissive = imgNoEmissive.getTexture();
   texRuby.normal = 0;
   texRuby.shininess = 76.8;

   texGold.diffuse = imgGold.getTexture();
   texGold.specular = imgGold.getTexture();
   texGold.emissive = imgNoEmissive.getTexture();
   texGold.normal = 0;
   texGold.shininess = 51.2;

   texChess.diffuse = imgChess.getTexture();
   texChess.specular = imgChess.getTexture();
   texChess.emissive = imgNoEmissive.getTexture();
   texChess.normal = 0;
   texChess.shininess = 10.0;

   texCube.diffuse = imgCubeDiffuse.getTexture();
   texCube.specular = imgCubeSpecular.getTexture();
   texCube.emissive = imgNoEmissive.getTexture();
   texCube.normal = 0;
   texCube.shininess = 10.0;

   // 1. Suelo
   texAxiomFloor.diffuse = imgAxiomFloor.getTexture();
   texAxiomFloor.specular = imgAxiomFloor.getTexture();
   texAxiomFloor.emissive = imgNoEmissive.getTexture();
   texAxiomFloor.normal = 0;
   texAxiomFloor.shininess = 100.0;

   // 2. Paredes
   texAxiomWall.diffuse = imgAxiomWall.getTexture();
   texAxiomWall.specular = imgAxiomWall.getTexture();
   texAxiomWall.emissive = imgNoEmissive.getTexture();
   texAxiomWall.normal = imgWallNormal.getTexture();
   texAxiomWall.shininess = 30.0;

   // 3. Luces
   texGuideLine.diffuse = imgGuideLine.getTexture();
   texGuideLine.specular = imgGuideLine.getTexture();
   texGuideLine.emissive = imgGuideLine.getTexture(); // Brilla
   texGuideLine.normal = 0;
   texGuideLine.shininess = 10.0;
}

void renderScene()
{

   // Borramos el buffer de color
   glClearColor(0.0, 0.0, 0.0, 0.0);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   // Indicamos los shaders a utilizar
   shaders.useShaders();

   // Matriz P
   float nplane = 0.1;
   float fplane = 50.0;
   float aspect = (float)w / (float)h;
   glm::mat4 P = glm::perspective(glm::radians(fovy), aspect, nplane, fplane);

   // Matriz V
   float x = 10.0f * glm::cos(glm::radians(alphaY)) * glm::sin(glm::radians(alphaX));
   float y = 10.0f * glm::sin(glm::radians(alphaY));
   float z = 10.0f * glm::cos(glm::radians(alphaY)) * glm::cos(glm::radians(alphaX));
   glm::vec3 eye(x, y, z);
   glm::vec3 center(0.0, 0.0, 0.0);
   glm::vec3 up(0.0, 1.0, 0.0);
   glm::mat4 V = glm::lookAt(eye, center, up);
   shaders.setVec3("ueye", eye);

   // Fijamos las luces
   setLights(P, V);

   // --------------------------------------------------------------------------
   // EL PASILLO
   // --------------------------------------------------------------------------

   // 1. EL SUELO
   glm::mat4 MatrixSuelo = glm::translate(I, glm::vec3(0.0, -2.0, 0.0)) * glm::scale(I, glm::vec3(8.0, 1.0, 40.0));
   drawObjectTex(plane, texAxiomFloor, P, V, MatrixSuelo);

   // 2. LÍNEA DE GUÍA

   glm::mat4 MatrixLinea = glm::translate(I, glm::vec3(0.0, -1.98, 0.0)) * glm::scale(I, glm::vec3(0.3, 1.0, 40.0));
   drawObjectTex(plane, texGuideLine, P, V, MatrixLinea);

   // 3. PAREDES MODULARES (Bucle)
   for (int i = -3; i <= 3; i++)
   {
      float zPos = i * 5.0f;

      // Pared Izquierda
      glm::mat4 M_Izq = glm::translate(I, glm::vec3(-anchoPasillo, 1.0, zPos)) * glm::scale(I, glm::vec3(0.5, 6.0, 4.0));
      drawObjectTex(cube, texAxiomWall, P, V, M_Izq);

      // Pared Derecha
      /*
      glm::mat4 M_Der = glm::translate(I, glm::vec3( anchoPasillo, 1.0, zPos))
         * glm::scale(I, glm::vec3(0.5, 6.0, 4.0));
      drawObjectTex(cube, texAxiomWall, P, V, M_Der);
      */

      // Luces de zócalo Izquierda
      glm::mat4 Luz_Izq = glm::translate(I, glm::vec3(-anchoPasillo + 0.6, -1.8, zPos)) * glm::scale(I, glm::vec3(0.1, 0.5, 4.0));
      drawObjectTex(cube, texGuideLine, P, V, Luz_Izq);

      /*
      // Luces de zócalo Derecha
      glm::mat4 Luz_Der = glm::translate(I, glm::vec3( anchoPasillo - 0.6, -1.8, zPos))
            * glm::scale(I, glm::vec3(0.1, 0.5, 4.0));
      drawObjectTex(cube, texGuideLine, P, V, Luz_Der);
      */

      // M-O
      glm::mat4 S = glm::scale(I, glm::vec3(0.05, 0.05, 0.05));
      drawMO(P, V, S);
   }

   // 4. SUCIEDAD

   glm::mat4 Suciedad = glm::translate(I, glm::vec3(1.5, -1.95, 2.0)) * glm::scale(I, glm::vec3(0.6, 1.0, 0.6));
   drawObjectTex(plane, texRuby, P, V, Suciedad);
}

void setLights(glm::mat4 P, glm::mat4 V)
{

   shaders.setLight("ulightG", lightG);
   for (int i = 0; i < NLD; i++)
      shaders.setLight("ulightD[" + toString(i) + "]", lightD[i]);
   for (int i = 0; i < NLP; i++)
      shaders.setLight("ulightP[" + toString(i) + "]", lightP[i]);
   for (int i = 0; i < NLF; i++)
      shaders.setLight("ulightF[" + toString(i) + "]", lightF[i]);

   for (int i = 0; i < NLP; i++)
   {
      glm::mat4 M = glm::translate(I, lightP[i].position) * glm::scale(I, glm::vec3(0.1));
      drawObjectMat(sphere, mluz, P, V, M);
   }

   for (int i = 0; i < NLF; i++)
   {
      glm::mat4 M = glm::translate(I, lightF[i].position) * glm::scale(I, glm::vec3(0.025));
      drawObjectMat(sphere, mluz, P, V, M);
   }
}

void drawObjectMat(Model &model, Material material, glm::mat4 P, glm::mat4 V, glm::mat4 M)
{

   shaders.setMat4("uN", glm::transpose(glm::inverse(M)));
   shaders.setMat4("uM", M);
   shaders.setMat4("uPVM", P * V * M);
   shaders.setBool("uWithMaterials", true);
   shaders.setMaterial("umaterial", material);
   model.renderModel(GL_FILL);
}

void drawObjectTex(Model &model, Textures textures, glm::mat4 P, glm::mat4 V, glm::mat4 M)
{

   shaders.setMat4("uN", glm::transpose(glm::inverse(M)));
   shaders.setMat4("uM", M);
   shaders.setMat4("uPVM", P * V * M);
   shaders.setBool("uWithMaterials", false);
   shaders.setTextures("utextures", textures);
   if (textures.normal != 0)
      shaders.setBool("uWithNormals", true);
   else
      shaders.setBool("uWithNormals", false);
   model.renderModel(GL_FILL);
}

void funFramebufferSize(GLFWwindow *window, int width, int height)
{

   // Configuracion del Viewport
   glViewport(0, 0, width, height);

   // Actualizacion de w y h
   w = width;
   h = height;
}

void funKey(GLFWwindow *window, int key, int scancode, int action, int mods)
{
   switch (key)
   {
   
   }
}

void funScroll(GLFWwindow *window, double xoffset, double yoffset)
{

   if (yoffset > 0)
      fovy -= fovy > 10.0f ? 5.0f : 0.0f;
   if (yoffset < 0)
      fovy += fovy < 90.0f ? 5.0f : 0.0f;
}

void funCursorPos(GLFWwindow *window, double xpos, double ypos)
{

   if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
      return;

   float limY = 89.0;
   alphaX = 90.0 * (2.0 * xpos / (float)w - 1.0);
   alphaY = 90.0 * (1.0 - 2.0 * ypos / (float)h);
   if (alphaY < -limY)
      alphaY = -limY;
   if (alphaY > limY)
      alphaY = limY;
}
void drawMO(glm::mat4 P, glm::mat4 V, glm::mat4 M)
{
   drawCuerpo(P, V, M);
   drawCabeza(P, V, M);
   drawBrazoDer(P, V, M);
   drawBrazoIzq(P, V, M);
   drawRueda(P, V, M);
   drawAspiradora(P, V, M);
}

void drawCuerpo(glm::mat4 P, glm::mat4 V, glm::mat4 M)
{
   drawObjectTex(cuerpo, texCube, P, V, M);

   drawObjectTex(cubreRueda, texGold, P, V, M);

   drawObjectTex(mochila, texRuby, P, V, M);
}

void drawCabeza(glm::mat4 P, glm::mat4 V, glm::mat4 M)
{
   drawObjectTex(cara, texGold, P, V, M);

   drawObjectTex(casco, texCube, P, V, M);

   drawObjectTex(cuello, texGold, P, V, M);

   drawSirena(P, V, M);
}

void drawSirena(glm::mat4 P, glm::mat4 V, glm::mat4 M)
{
   drawObjectTex(cristalSirena, texRuby, P, V, M);

   drawObjectTex(tapaSirena, texCube, P, V, M);
}

void drawBrazoDer(glm::mat4 P, glm::mat4 V, glm::mat4 M)
{
   drawObjectTex(brazoDer, texCube, P, V, M);
}

void drawBrazoIzq(glm::mat4 P, glm::mat4 V, glm::mat4 M)
{
   drawObjectTex(brazoIzq, texCube, P, V, M);
}

void drawRueda(glm::mat4 P, glm::mat4 V, glm::mat4 M)
{
   drawObjectTex(rueda, texRuby, P, V, M);
}

void drawAspiradora(glm::mat4 P, glm::mat4 V, glm::mat4 M)
{
   drawObjectTex(aspiradora, texChess, P, V, M);

   drawObjectTex(tapaAspiradora, texGold, P, V, M);
}