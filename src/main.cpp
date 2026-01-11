#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Shaders.h"
#include "Model.h"
#include "Texture.h"
#include <algorithm>

void configScene();
void renderScene();
void setLights(glm::mat4 P, glm::mat4 V);
void drawObjectMat(Model &model, Material material, glm::mat4 P, glm::mat4 V, glm::mat4 M);
void drawObjectTex(Model &model, Textures textures, glm::mat4 P, glm::mat4 V, glm::mat4 M);

void drawEscenario(glm::mat4 P, glm::mat4 V);
void drawContenedorSciFi(glm::mat4 P, glm::mat4 V, glm::vec3 pos, float rotY, float escala);
void drawMO(glm::mat4 P, glm::mat4 V, glm::mat4 M);
void drawCuerpo(glm::mat4 P, glm::mat4 V, glm::mat4 M);
void drawCabeza(glm::mat4 P, glm::mat4 V, glm::mat4 M);
void drawSirena(glm::mat4 P, glm::mat4 V, glm::mat4 M);
void drawBrazos(glm::mat4 P, glm::mat4 V, glm::mat4 M);
void drawBrazoDer(glm::mat4 P, glm::mat4 V, glm::mat4 M);
void drawBrazoIzq(glm::mat4 P, glm::mat4 V, glm::mat4 M);
void drawRueda(glm::mat4 P, glm::mat4 V, glm::mat4 M);
void drawAspiradora(glm::mat4 P, glm::mat4 V, glm::mat4 M);

void levantarSirena();
void movimientoMO();
void luzOjos(glm::mat4 M);
void animacionHyperScanner();
void animacionDiagonalParedes();

void funFramebufferSize(GLFWwindow *window, int width, int height);
void funKey(GLFWwindow *window, int key, int scancode, int action, int mods);
void funScroll(GLFWwindow *window, double xoffset, double yoffset);
void funCursorPos(GLFWwindow *window, double xpos, double ypos);
bool checkColisionCajas(float x, float z);

// --- CONFIGURACIÓN Y MUNDO ---
int w = 500;
int h = 500;

const float SUELO_Y        = -2.0f;
const float TECHO_Y        = 22.0f;
const float ALTO_PARED     = 24.0f;
const float ANCHO_PASILLO  = 20.5f;

#define NLD 1   // Luces Direccionales
#define NLP 45  // Luces Posicionales
#define NLF 5   // Luces Focales

Shaders shaders;

// --- CÁMARA Y CONTROL ---
float fovy          = 60.0f;
float sensitivity   = 0.5f;
bool  firstClick    = true;
bool  modoF5        = false;

double lastX = 0.0;
double lastY = 0.0;

float alphaX = -90.0f;
float alphaY = 0.0f;
glm::vec3 cameraPos   = glm::vec3(0.0f, 2.0f, 15.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);

// --- LUCES Y MATERIALES ---
Light lightG;
Light lightD[NLD];
Light lightP[NLP];
Light lightF[NLF];

Material mluz;
Material mOjo;
Material mNeon;

// --- MODELOS 3D ---
Model sphere, plane, cube, signal, contenedor;
Model officeWindow;

// Jerarquía Robot M-O
Model aspiradora, cuerpo, cabeza, rueda;
Model brazoDerecho, brazoIzquierdo;
Model cara, casco, cuello;
Model cristalSirena, tapaSirena;
Model cubreRueda, mochila;
Model ejeBrazoDerecho, ejeBrazoIzquierdo;
Model tapaAspiradora, tapaCasco;

// --- TEXTURAS Y SETS ---
Texture imgNoEmissive, imgWhiteMetal, imgGreyMetal, imgBlackMetal, imgBlackRubber, imgBlueGlass, imgRedGlass, imgRuby;
Texture imgAxiomFloor, imgFloorNormal, imgFloorSpec;
Texture imgAxiomWall_Albedo, imgAxiomWall_Normal, imgAxiomWall_Roughness;
Texture imgCeiling_Color, imgCeiling_Normal, imgCeiling_Roughness, imgCeiling_AO;
Texture imgOrganic_Albedo, imgOrganic_Normal, imgOrganic_Roughness;
Texture imgCont_Diff, imgCont_Norm, imgCont_Metal, imgCont_Emis;
Texture signal_BaseColor, signal_Roughness, signal_Metallic, signal_Normal, signal_AO;
Texture imgSpace;

Textures texAxiomFloor, texAxiomWall, texZocaloLed, texRuby, texCeiling, texOrganicWall;
Textures texWhiteMetal, texGreyMetal, texBlackMetal, texBlackRubber, texBlueGlass, texRedGlass;
Textures texSignal, texContenedor;
Textures texSpaceBackground;

// --- ESTADO Y ANIMACIÓN ---
float posX          = 0.0f;
float posZ          = 0.0f;
float anguloGiro    = 0.0f;
float inclinacionX  = 0.0f;
float inclinacionZ  = 0.0f;

float anguloRueda      = 0.0f;
float anguloAspiradora = 0.0f;
float anguloBrazos     = 0.0f;
float anguloSirena     = 0.0f;
float alturaSirena     = -7.42f;

// Control de Inputs
bool animacionActiva = false;
bool sirenaLevantada = false;
bool giroIzq = false, giroDer = false;
bool movW = false, movS = false, movA = false, movD = false;

// Luz focal interactiva
float focalPosX = 0.0f;
float focalPosY = 5.0f;
float focalPosZ = 10.0f;
bool  botonDerecho = false;

int main()
{
   // Inicializamos GLFW
   if (!glfwInit()) return -1;
   glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
   glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
   glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

   // Creamos la ventana
   GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
   const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);

   w = mode->width;
   h = mode->height;

   GLFWwindow* window = glfwCreateWindow(w, h, "Práctica IG", NULL, NULL);
   // Forzamos a que la ventana se maximice inmediatamente
   glfwMaximizeWindow(window);
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
   glEnable(GL_DEPTH_TEST);
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

   shaders.initShaders("resources/shaders/vshader.glsl", "resources/shaders/fshader.glsl");

   // --- MODELOS ---
   sphere.initModel("resources/models/sphere.obj");
   plane.initModel("resources/models/plane.obj");
   cube.initModel("resources/models/cube.obj");
   signal.initModel("resources/models/Sign.obj");
   contenedor.initModel("resources/models/Container Free.obj");

   aspiradora.initModel("resources/models/aspiradora.obj");
   brazoDerecho.initModel("resources/models/brazoDerecho.obj");
   brazoIzquierdo.initModel("resources/models/brazoIzquierdo.obj");
   cara.initModel("resources/models/cara.obj");
   casco.initModel("resources/models/casco.obj");
   cristalSirena.initModel("resources/models/cristalSirena.obj");
   cubreRueda.initModel("resources/models/cubreRueda.obj");
   cuello.initModel("resources/models/cuello.obj");
   cuerpo.initModel("resources/models/cuerpo.obj");
   ejeBrazoDerecho.initModel("resources/models/ejeBrazoDerecho.obj");
   ejeBrazoIzquierdo.initModel("resources/models/ejeBrazoIzquierdo.obj");
   mochila.initModel("resources/models/mochila.obj");
   rueda.initModel("resources/models/rueda.obj");
   tapaAspiradora.initModel("resources/models/tapaAspiradora.obj");
   tapaCasco.initModel("resources/models/tapaCasco.obj");
   tapaSirena.initModel("resources/models/tapaSirena.obj");

   // --- TEXTURAS ---
   imgNoEmissive.initTexture("resources/textures/imgNoEmissive.png");
   imgRuby.initTexture("resources/textures/imgRuby.png");
   imgWhiteMetal.initTexture("resources/textures/whiteMetal.jpg");
   imgGreyMetal.initTexture("resources/textures/greyMetal.jpg");
   imgBlackMetal.initTexture("resources/textures/blackMetal.jpg");
   imgBlackRubber.initTexture("resources/textures/blackRubber.jpg");
   imgBlueGlass.initTexture("resources/textures/blueGlass.png");
   imgRedGlass.initTexture("resources/textures/redGlass.png");

   imgAxiomFloor.initTexture("resources/textures/Scifi_Hex_Wall_Difusse.jpg");
   imgFloorNormal.initTexture("resources/textures/Scifi_Hex_Wall_normal.jpg");
   imgFloorSpec.initTexture("resources/textures/Scifi_Hex_Wall_specular.jpg");

   imgAxiomWall_Albedo.initTexture("resources/textures/scifi_panel_1_color_1k.png");
   imgAxiomWall_Normal.initTexture("resources/textures/scifi_panel_1_normal_1k.png");
   imgAxiomWall_Roughness.initTexture("resources/textures/scifi_panel_1_metallic_1k.png");

   imgCeiling_Color.initTexture("resources/textures/floor_tile_1_color.png");
   imgCeiling_Normal.initTexture("resources/textures/floor_tile_1_normal.png");
   imgCeiling_Roughness.initTexture("resources/textures/floor_tile_1_roughness.png");

   imgOrganic_Albedo.initTexture("resources/textures/organic_tech_1_color_1k.png");
   imgOrganic_Normal.initTexture("resources/textures/organic_tech_1_normal_1k.png");
   imgOrganic_Roughness.initTexture("resources/textures/organic_tech_1_roughness_1k.png");

   signal_BaseColor.initTexture("resources/textures/sign_basecolor.png");
   signal_Roughness.initTexture("resources/textures/sign_roughness.png");
   signal_Normal.initTexture("resources/textures/sign_normal.jpg");

   imgCont_Diff.initTexture("resources/textures/Container Free_BaseColor.png");
   imgCont_Norm.initTexture("resources/textures/Container Free_Normal_OpenGL.png");
   imgCont_Metal.initTexture("resources/textures/Container Free_Metallic.png");
   imgCont_Emis.initTexture("resources/textures/TContainer Free_Emissive.png");

   imgSpace.initTexture("resources/textures/space.jpg");

   // --- CONFIGURACIÓN DE MATERIALES ---
   texZocaloLed.diffuse = texZocaloLed.specular = texZocaloLed.emissive = imgWhiteMetal.getTexture();
   texZocaloLed.normal = 0; texZocaloLed.shininess = 23.0f;

   texAxiomWall.diffuse = imgAxiomWall_Albedo.getTexture();
   texAxiomWall.specular = imgAxiomWall_Roughness.getTexture();
   texAxiomWall.emissive = imgNoEmissive.getTexture();
   texAxiomWall.normal = imgAxiomWall_Normal.getTexture();
   texAxiomWall.shininess = 50.0f;

   texWhiteMetal.diffuse = texWhiteMetal.specular = imgWhiteMetal.getTexture();
   texWhiteMetal.emissive = imgNoEmissive.getTexture();
   texWhiteMetal.normal = 0; texWhiteMetal.shininess = 64.0f;

   texGreyMetal.diffuse = texGreyMetal.specular = imgGreyMetal.getTexture();
   texGreyMetal.emissive = imgNoEmissive.getTexture();
   texGreyMetal.normal = 0; texGreyMetal.shininess = 100.0f;

   texBlackMetal.diffuse = texBlackMetal.specular = imgBlackMetal.getTexture();
   texBlackMetal.emissive = imgNoEmissive.getTexture();
   texBlackMetal.normal = 0; texBlackMetal.shininess = 32.0f;

   texBlackRubber.diffuse = texBlackRubber.specular = imgBlackRubber.getTexture();
   texBlackRubber.emissive = imgNoEmissive.getTexture();
   texBlackRubber.normal = 0; texBlackRubber.shininess = 10.0f;

   texBlueGlass.diffuse = texBlueGlass.specular = texBlueGlass.emissive = imgBlueGlass.getTexture();
   texBlueGlass.normal = 0; texBlueGlass.shininess = 128.0f;

   texRedGlass.diffuse = texRedGlass.specular = texRedGlass.emissive = imgRedGlass.getTexture();
   texRedGlass.normal = 0; texRedGlass.shininess = 128.0f;

   texAxiomFloor.diffuse = imgAxiomFloor.getTexture();
   texAxiomFloor.specular = imgFloorSpec.getTexture();
   texAxiomFloor.emissive = imgNoEmissive.getTexture();
   texAxiomFloor.normal = imgFloorNormal.getTexture();
   texAxiomFloor.shininess = 32.0f;

   texCeiling.diffuse = imgCeiling_Color.getTexture();
   texCeiling.specular = imgCeiling_Roughness.getTexture();
   texCeiling.emissive = imgNoEmissive.getTexture();
   texCeiling.normal = imgCeiling_Normal.getTexture();
   texCeiling.shininess = 30.0f;

   texOrganicWall.diffuse = imgOrganic_Albedo.getTexture();
   texOrganicWall.specular = imgOrganic_Roughness.getTexture();
   texOrganicWall.emissive = imgNoEmissive.getTexture();
   texOrganicWall.normal = imgOrganic_Normal.getTexture();
   texOrganicWall.shininess = 64.0f;

   texContenedor.diffuse = imgCont_Diff.getTexture();
   texContenedor.specular = imgCont_Metal.getTexture();
   texContenedor.normal = imgCont_Norm.getTexture();
   texContenedor.emissive = imgCont_Emis.getTexture();
   texContenedor.shininess = 32.0f;

   texRuby.diffuse = texRuby.specular = imgRuby.getTexture();
   texRuby.emissive = imgNoEmissive.getTexture();
   texRuby.normal = 0; texRuby.shininess = 128.0f;

   texSignal.diffuse = signal_BaseColor.getTexture();
   texSignal.specular = signal_Roughness.getTexture();
   texSignal.normal = signal_Normal.getTexture();
   texSignal.shininess = 10.0f;

   texSpaceBackground.diffuse = imgSpace.getTexture();
   texSpaceBackground.emissive = imgSpace.getTexture();
   texSpaceBackground.shininess = 1.0f;
   texSpaceBackground.normal = 0;

   // Materiales puros
   mluz.emissive = glm::vec4(1.0f); mluz.shininess = 1.0f;
   mOjo.ambient = glm::vec4(0.2, 0.2, 0.0, 1.0); mOjo.diffuse = glm::vec4(0.9, 0.8, 0.2, 1.0);
   mOjo.emissive = glm::vec4(0.8, 0.7, 0.1, 1.0); mOjo.shininess = 0.75f;
   mNeon.specular = glm::vec4(1.0f); mNeon.emissive = glm::vec4(0.0, 0.9, 1.0, 1.0); mNeon.shininess = 128.0f;

   // --- CONFIGURACIÓN DE LUCES ---
   lightG.ambient = glm::vec3(0.2, 0.2, 0.3);

   lightD[0].direction = glm::vec3(0.0, -1.0, 0.0);
   lightD[0].ambient = glm::vec3(0.1f);
   lightD[0].diffuse = lightD[0].specular = glm::vec3(0.7f);

   lightP[0].position = glm::vec3(0.0, 22.0, 0.0);
   lightP[0].ambient = glm::vec3(0.2f);
   lightP[0].diffuse = glm::vec3(0.9, 0.95, 1.0);
   lightP[0].specular = glm::vec3(1.0f);
   lightP[0].c0 = 1.00f; lightP[0].c1 = 0.09f; lightP[0].c2 = 0.032f;

   for(int i = 1; i < NLP; i++) {
      lightP[i].diffuse = glm::vec3(0.0, 0.8, 1.0);
      lightP[i].c0 = 1.0f; lightP[i].c1 = 0.5f; lightP[i].c2 = 0.8f;
   }

   // Focales (Ojos y Sirena)
   for(int i = 0; i <= 1; i++){
       lightF[i].diffuse = lightF[i].specular = glm::vec3(0.8, 0.7, 0.2);
       lightF[i].innerCutOff = 15.0f; lightF[i].outerCutOff = 25.0f;
       lightF[i].c0 = 1.0f; lightF[i].c1 = 0.09f; lightF[i].c2 = 0.032f;
   }

   for(int i = 2; i <= 3; i++){
       lightF[i].diffuse = lightF[i].specular = glm::vec3(0.9f);
       lightF[i].innerCutOff = 20.0f; lightF[i].outerCutOff = 30.0f;
       lightF[i].c0 = 1.0f; lightF[i].c1 = 0.09f; lightF[i].c2 = 0.032f;
   }

   lightF[4].ambient = glm::vec3(0.1f, 0.0f, 0.0f);
   lightF[4].diffuse = lightF[4].specular = glm::vec3(1.0f, 0.0f, 0.0f);
   lightF[4].innerCutOff = 10.0f; lightF[4].outerCutOff = 20.0f;
   lightF[4].c0 = 1.0f; lightF[4].c1 = 0.09f; lightF[4].c2 = 0.032f;
   lightF[4].direction = glm::vec3(0.0f, -1.0f, -1.0f);
}

void renderScene()
{
   // --- LÓGICA Y ANIMACIÓN ---
   if (animacionActiva) {
      anguloAspiradora += 5.0f;
      if (anguloAspiradora > 360.0f) anguloAspiradora -= 360.0f;
   }

   levantarSirena();
   movimientoMO();
   animacionHyperScanner();
   animacionDiagonalParedes();

   if (giroIzq) anguloGiro += 2.0f;
   if (giroDer) anguloGiro -= 2.0f;

   // --- CONFIGURACIÓN RENDER ---
   glClearColor(0.0, 0.0, 0.0, 0.0);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   shaders.useShaders();

   lightF[4].position = glm::vec3(focalPosX, focalPosY, focalPosZ);
   lightF[4].direction = glm::vec3(0.0f, -1.0f, 0.0f);

   // --- CÁMARA Y PERSPECTIVA ---
   float aspect = (float)w / (float)h;
   glm::mat4 P = glm::perspective(glm::radians(fovy), aspect, 0.1f, 400.0f);
   glm::mat4 V;

   if (modoF5) {
      // Cámara 3ª Persona
      float dist = 12.5f;
      float rad = glm::radians(anguloGiro);
      glm::vec3 camPosF5;
      camPosF5.x = posX - (sin(rad) * dist);
      camPosF5.y = 5.0f;
      camPosF5.z = posZ - (cos(rad) * dist);

      V = glm::lookAt(camPosF5, glm::vec3(posX, 4.0f, posZ), glm::vec3(0.0, 1.0, 0.0));
      shaders.setVec3("ueye", camPosF5);
   }
   else {
      // Cámara 1ª Persona
      glm::vec3 front;
      front.x = cos(glm::radians(alphaX)) * cos(glm::radians(alphaY));
      front.y = sin(glm::radians(alphaY));
      front.z = sin(glm::radians(alphaX)) * cos(glm::radians(alphaY));
      cameraFront = glm::normalize(front);

      V = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
      shaders.setVec3("ueye", cameraPos);
   }

   // --- MATRICES Y LUCES DEL ROBOT ---
   glm::mat4 T_MO = glm::translate(I, glm::vec3(posX, -2.0, posZ));
   glm::mat4 R_MO = glm::rotate(I, glm::radians(anguloGiro), glm::vec3(0.0, 1.0, 0.0));
   glm::mat4 S_MO = glm::scale(I, glm::vec3(0.05, 0.05, 0.05));
   glm::mat4 matrizMO = T_MO * R_MO * S_MO;

   luzOjos(matrizMO);
   setLights(P, V);

   // --- DIBUJO ---
   glm::mat4 matLuzRoja = glm::translate(I, lightF[4].position) * glm::scale(I, glm::vec3(0.2f));
   Material mRoja = mluz;
   mRoja.emissive = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);

   drawObjectMat(sphere, mRoja, P, V, matLuzRoja);
   drawEscenario(P, V);
   drawMO(P, V, matrizMO);
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

   for (int i = 0; i < 1; i++)
   {
      glm::mat4 M = glm::translate(I, lightP[i].position) * glm::scale(I, glm::vec3(0.1));
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
   glViewport(0, 0, width, height);
   w = width;
   h = height;
}

void funKey(GLFWwindow *window, int key, int scancode, int action, int mods)
{
   float cameraSpeed = 1.0f; // Velocidad

   switch (key)
   {
   case GLFW_KEY_UP:
      if (action == GLFW_PRESS || action == GLFW_REPEAT)
         cameraPos += cameraSpeed * cameraFront;
      break;

   case GLFW_KEY_DOWN:
      if (action == GLFW_PRESS || action == GLFW_REPEAT)
         cameraPos -= cameraSpeed * cameraFront;
      break;

   case GLFW_KEY_LEFT:
      if (action == GLFW_PRESS || action == GLFW_REPEAT)
         cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
      break;

   case GLFW_KEY_RIGHT:
      if (action == GLFW_PRESS || action == GLFW_REPEAT)
         cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
      break;
   case GLFW_KEY_C:
      if (action == GLFW_PRESS) modoF5 = !modoF5;
      break;

   case GLFW_KEY_G:
      if (action == GLFW_PRESS) animacionActiva = !animacionActiva;
      break;

   case GLFW_KEY_F:
      if (action == GLFW_PRESS || action == GLFW_REPEAT) {
         anguloBrazos += 2.0f;
         if (anguloBrazos > 5.0f) anguloBrazos = 5.0f;
      }
      break;

   case GLFW_KEY_R:
      if (action == GLFW_PRESS || action == GLFW_REPEAT) {
         anguloBrazos -= 2.0f;
         if (anguloBrazos < -10.0f) anguloBrazos = -10.0f;
      }
      break;
   case GLFW_KEY_Y:
      if (action == GLFW_PRESS) sirenaLevantada = !sirenaLevantada;
      break;

   // MOVIMIENTO ROBOT (WASD)
   case GLFW_KEY_Q:
      if (action == GLFW_PRESS) giroIzq = true;
      if (action == GLFW_RELEASE) giroIzq = false;
      break;
   case GLFW_KEY_E:
      if (action == GLFW_PRESS) giroDer = true;
      if (action == GLFW_RELEASE) giroDer = false;
      break;
   case GLFW_KEY_W:
      if (action == GLFW_PRESS) movW = true;
      if (action == GLFW_RELEASE) movW = false;
      break;
   case GLFW_KEY_S:
      if (action == GLFW_PRESS) movS = true;
      if (action == GLFW_RELEASE) movS = false;
      break;
   case GLFW_KEY_A:
      if (action == GLFW_PRESS) movA = true;
      if (action == GLFW_RELEASE) movA = false;
      break;
   case GLFW_KEY_D:
      if (action == GLFW_PRESS) movD = true;
      if (action == GLFW_RELEASE) movD = false;
      break;
   }
}

void funScroll(GLFWwindow *window, double xoffset, double yoffset)
{
   if (yoffset > 0) fovy -= fovy > 10.0f ? 5.0f : 0.0f;
   if (yoffset < 0) fovy += fovy < 90.0f ? 5.0f : 0.0f;
}

void funCursorPos(GLFWwindow *window, double xpos, double ypos)
{
   bool lmb = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
   bool rmb = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;
   bool shift = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ||
                glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS;

   if (!lmb && !rmb) {
      firstClick = true;
      return;
   }

   if (firstClick) {
      lastX = xpos;
      lastY = ypos;
      firstClick = false;
   }

   double xoffset = xpos - lastX;
   double yoffset = lastY - ypos;
   lastX = xpos;
   lastY = ypos;

   if (lmb) {
      alphaX += xoffset * sensitivity;
      alphaY += yoffset * sensitivity;

      if (alphaY >  89.0f) alphaY =  89.0f;
      if (alphaY < -89.0f) alphaY = -89.0f;
   }
   else if (rmb) {
      float lightSens = 0.05f;
      focalPosX += xoffset * lightSens;

      if (shift) {
         focalPosZ -= yoffset * lightSens; // profundidad
      } else {
         focalPosY += yoffset * lightSens; // altura
      }
   }
}
void drawMO(glm::mat4 P, glm::mat4 V, glm::mat4 M)
{
   glm::vec3 centro = glm::vec3(0.0f, 9.80f, -3.58f);
   glm::mat4 Tida = glm::translate(glm::mat4(1.0f), -centro);
   glm::mat4 Rx = glm::rotate(I, glm::radians(inclinacionX), glm::vec3(1.0, 0.0, 0.0));
   glm::mat4 Rz = glm::rotate(I, glm::radians(inclinacionZ), glm::vec3(0.0, 0.0, 1.0));
   glm::mat4 Tvuelta = glm::translate(glm::mat4(1.0f), centro);

   drawCuerpo(P, V, M * Tvuelta * Rz * Rx * Tida);
   drawCabeza(P, V, M * Tvuelta * Rz * Rx * Tida);
   drawBrazos(P, V, M * Tvuelta * Rz * Rx * Tida);
   drawRueda(P, V, M);
}

void drawCuerpo(glm::mat4 P, glm::mat4 V, glm::mat4 M)
{
   drawObjectTex(cuerpo, texWhiteMetal, P, V, M);
   drawObjectTex(cubreRueda, texGreyMetal, P, V, M);

   glm::mat4 T = glm::translate(I, glm::vec3(1.0f, 55.24f, -7.86f));
   glm::mat4 S = glm::scale(I, glm::vec3(16.5f, 1.0f, 12.0f));
   drawObjectTex(plane, texWhiteMetal, P, V, M * T * S);

   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   glDepthMask(GL_FALSE);
   drawObjectTex(mochila, texBlueGlass, P, V, M);
   glDepthMask(GL_TRUE);
   glDisable(GL_BLEND);
}

void drawCabeza(glm::mat4 P, glm::mat4 V, glm::mat4 M)
{
   drawObjectTex(cara, texBlackMetal, P, V, M);
   drawObjectTex(casco, texWhiteMetal, P, V, M);
   drawObjectTex(tapaCasco, texWhiteMetal, P, V, M);
   drawObjectTex(cuello, texGreyMetal, P, V, M);
   drawSirena(P, V, M);

   glm::mat4 R = glm::rotate(I, glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));
   glm::mat4 S = glm::scale(I, glm::vec3(6.0f, 1.0f, 1.2f));
   glm::mat4 T = glm::translate(I, glm::vec3(8.0f, 60.0f, 26.6f));
   drawObjectMat(plane, mOjo, P, V, M * T * R * S);

   T = glm::translate(I, glm::vec3(-8.0f, 60.0f, 26.6f));
   drawObjectMat(plane, mOjo, P, V, M * T * R * S);
}

void drawSirena(glm::mat4 P, glm::mat4 V, glm::mat4 M)
{
   glm::mat4 Televacion = glm::translate(I, glm::vec3(0.0, alturaSirena, 0.0));
   drawObjectTex(tapaSirena, texWhiteMetal, P, V, M * Televacion);

   glm::vec3 centroCristal = glm::vec3(-1.13f, 101.94f, -4.40f);
   float radioGiro = 2.5f;
   float escala = 1.0f;

   glm::mat4 T_Centro = glm::translate(I, centroCristal);
   glm::mat4 T_Radio  = glm::translate(I, glm::vec3(radioGiro, 0.0, 0.0));
   glm::mat4 S        = glm::scale(I, glm::vec3(escala));

   glm::mat4 R1 = glm::rotate(I, glm::radians(anguloSirena), glm::vec3(0.0, 1.0, 0.0));
   glm::mat4 M_Pos1 = M * Televacion * T_Centro * R1 * T_Radio;

   drawObjectMat(sphere, mluz, P, V, M_Pos1 * S);
   lightF[2].position  = glm::vec3(M_Pos1 * glm::vec4(0.0, 0.0, 0.0, 1.0));
   lightF[2].direction = glm::normalize(glm::vec3(M * Televacion * R1 * glm::vec4(1.0, 0.0, 0.0, 0.0)));

   glm::mat4 R2 = glm::rotate(I, glm::radians(anguloSirena + 180.0f), glm::vec3(0.0, 1.0, 0.0));
   glm::mat4 M_Pos2 = M * Televacion * T_Centro * R2 * T_Radio;

   drawObjectMat(sphere, mluz, P, V, M_Pos2 * S);
   lightF[3].position  = glm::vec3(M_Pos2 * glm::vec4(0.0, 0.0, 0.0, 1.0));
   lightF[3].direction = glm::normalize(glm::vec3(M * Televacion * R2 * glm::vec4(1.0, 0.0, 0.0, 0.0)));

   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   glDepthMask(GL_FALSE);
   drawObjectTex(cristalSirena, texRedGlass, P, V, M * Televacion);
   glDepthMask(GL_TRUE);
   glDisable(GL_BLEND);
}

void drawBrazos(glm::mat4 P, glm::mat4 V, glm::mat4 M)
{
   drawBrazoDer(P, V, M);
   drawBrazoIzq(P, V, M);
   drawAspiradora(P, V, M);
}

void drawBrazoDer(glm::mat4 P, glm::mat4 V, glm::mat4 M)
{
   drawObjectTex(ejeBrazoDerecho, texGreyMetal, P, V, M);
   glm::vec3 centro = glm::vec3(-21.74f, 45.53f, -8.83f);
   glm::mat4 Torigen = glm::translate(I, -centro);
   glm::mat4 R = glm::rotate(I, glm::radians(anguloBrazos), glm::vec3(1.0, 0.0, 0.0));
   glm::mat4 Tvuelta = glm::translate(I, centro);
   drawObjectTex(brazoDerecho, texWhiteMetal, P, V, M * Tvuelta * R * Torigen);
}

void drawBrazoIzq(glm::mat4 P, glm::mat4 V, glm::mat4 M)
{
   drawObjectTex(ejeBrazoIzquierdo, texGreyMetal, P, V, M);
   glm::vec3 centro = glm::vec3(22.63f, 45.53f, -8.83f);
   glm::mat4 Torigen = glm::translate(I, -centro);
   glm::mat4 R = glm::rotate(I, glm::radians(anguloBrazos), glm::vec3(1.0, 0.0, 0.0));
   glm::mat4 Tvuelta = glm::translate(I, centro);
   drawObjectTex(brazoIzquierdo, texWhiteMetal, P, V, M * Tvuelta * R * Torigen);
}

void drawAspiradora(glm::mat4 P, glm::mat4 V, glm::mat4 M)
{
   // Elevacion del brazo
   glm::vec3 centroBrazo = glm::vec3(0.0f, 45.53f, -8.83f);
   glm::mat4 Tida = glm::translate(I, -centroBrazo);
   glm::mat4 R = glm::rotate(I, glm::radians(anguloBrazos), glm::vec3(1.0, 0.0, 0.0));
   glm::mat4 Tvuelta = glm::translate(I, centroBrazo);
   glm::mat4 Melevacion = Tvuelta * R * Tida;

   // Giro del rodillo
   glm::vec3 centroAspiradora = glm::vec3(0.14f, 34.04f, 24.53f);
   Tida = glm::translate(I, -centroAspiradora);
   R = glm::rotate(I, glm::radians(anguloAspiradora), glm::vec3(1.0, 0.0, 0.0));
   Tvuelta = glm::translate(I, centroAspiradora);
   glm::mat4 Mgiro = Tvuelta * R * Tida;

   drawObjectTex(aspiradora, texBlackRubber, P, V, M * Melevacion * Mgiro);

   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   glDepthMask(GL_FALSE);
   drawObjectTex(tapaAspiradora, texBlueGlass, P, V, M * Melevacion);
   glDepthMask(GL_TRUE);
   glDisable(GL_BLEND);
}

void drawRueda(glm::mat4 P, glm::mat4 V, glm::mat4 M)
{
   glm::vec3 centroRueda = glm::vec3(0.0f, 9.80f, -3.58f);
   glm::mat4 Tida = glm::translate(I, -centroRueda);
   glm::mat4 Tvuelta = glm::translate(I, centroRueda);

   glm::mat4 M_RotacionRueda = glm::rotate(I, glm::radians(anguloRueda), glm::vec3(1.0, 0.0, 0.0));

   drawObjectTex(rueda, texBlackRubber, P, V, M * Tvuelta * M_RotacionRueda * Tida);
}

void levantarSirena()
{
   float velocidadSirena = 0.05f;
   if (sirenaLevantada) {
      if (alturaSirena < 0.0f) {
         alturaSirena += velocidadSirena;
         if (alturaSirena > 0.0f) alturaSirena = 0.0f;
      }
      anguloSirena += 5.0f;
      if (anguloSirena > 360.0f) anguloSirena -= 360.0f;
   } else {
      if (alturaSirena > -7.42f) {
         alturaSirena -= velocidadSirena;
         if (alturaSirena < -7.42f) alturaSirena = -7.42f;
      }
   }
}

void movimientoMO()
{
   float velocidadMov = 0.1f;
   float velocidadRot = 5.0f;
   float rad = glm::radians(anguloGiro);

   float deltaX = 0.0f;
   float deltaZ = 0.0f;

   // Limites
   float minX = -17.3f; float maxX = 17.3f;
   float minZ = -52.3f; float maxZ = 42.3f;

   // Inclinaciones
   float maxInclinacionX = 0.0f;
   float maxInclinacionZ = 0.0f;

   if (movW) {
      deltaX += sin(rad) * velocidadMov;
      deltaZ += cos(rad) * velocidadMov;
      anguloRueda += velocidadRot;
      maxInclinacionX = (animacionActiva) ? 40.0f : 15.0f;
   }
   if (movS) {
      deltaX -= sin(rad) * velocidadMov;
      deltaZ -= cos(rad) * velocidadMov;
      anguloRueda -= velocidadRot;
      maxInclinacionX = -15.0f;
   }
   if (movA) {
      deltaX += cos(rad) * velocidadMov;
      deltaZ -= sin(rad) * velocidadMov;
      maxInclinacionZ = -15.0f;
   }
   if (movD) {
      deltaX -= cos(rad) * velocidadMov;
      deltaZ += sin(rad) * velocidadMov;
      maxInclinacionZ = 15.0f;
   }

   // 2. COLISIÓN (Eje X)
   float nextX = posX + deltaX;
   if (nextX > minX && nextX < maxX && !checkColisionCajas(nextX, posZ)) {
       posX = nextX;
   }

   // 3. COLISIÓN (Eje Z)
   float nextZ = posZ + deltaZ;
   if (nextZ > minZ && nextZ < maxZ && !checkColisionCajas(posX, nextZ)) {
       posZ = nextZ;
   }

   // 4. INCLINACIÓN
   if (inclinacionX < maxInclinacionX) inclinacionX += 0.75f;
   if (inclinacionX > maxInclinacionX) inclinacionX -= 0.75f;
   if (inclinacionZ < maxInclinacionZ) inclinacionZ += 0.75f;
   if (inclinacionZ > maxInclinacionZ) inclinacionZ -= 0.75f;
}

bool checkColisionCajas(float x, float z) {
   // Caja doble
   float size1 = 5.5f;
   if (x > (-8.0f - size1) && x < (-8.0f + size1) &&
       z > (20.0f - size1) && z < (20.0f + size1)) {
      return true;
       }

   // Caja
   float radioX = 6.0f;
   float radioZ = 3.5f;
   if (x > (-8.5f - radioX) && x < (-8.5f + radioX) &&
       z > (27.0f - radioZ) && z < (27.0f + radioZ)) {
      return true;
       }

   return false;
}

void luzOjos(glm::mat4 M){
   glm::vec3 centro = glm::vec3(0.0f, 9.80f, -3.58f);
   glm::mat4 Tida = glm::translate(glm::mat4(1.0f), -centro);
   glm::mat4 Rx = glm::rotate(I, glm::radians(inclinacionX), glm::vec3(1.0, 0.0, 0.0));
   glm::mat4 Rz = glm::rotate(I, glm::radians(inclinacionZ), glm::vec3(0.0, 0.0, 1.0));
   glm::mat4 Tvuelta = glm::translate(glm::mat4(1.0f), centro);
   glm::mat4 Mcabeza = M * Tvuelta * Rz * Rx * Tida;

   glm::vec4 posOjoIzq = glm::vec4(-8.0f, 60.0f, 26.6f, 1.0f);
   glm::vec4 posOjoDer = glm::vec4(8.0f, 60.0f, 26.6f, 1.0f);
   glm::vec4 direccionOjos = glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);

   lightF[0].position  = glm::vec3(Mcabeza * posOjoIzq);
   lightF[0].direction = glm::normalize(glm::vec3(Mcabeza * direccionOjos));

   lightF[1].position  = glm::vec3(Mcabeza * posOjoDer);
   lightF[1].direction = glm::normalize(glm::vec3(Mcabeza * direccionOjos));
}

void animacionHyperScanner()
{
   float t = glfwGetTime();
   float zLuz = -48.0f;
   float amplitud = 19.0f;
   int baseIndex = 1;

   // BARRA 1: CYAN
   float xPos1 = sin(t * 1.5f) * amplitud;

   for(int i = 0; i < 5; i++) {
      int idx = baseIndex + i;
      float yPos = i * 5.0f;

      lightP[idx].position = glm::vec3(xPos1, yPos, zLuz);
      lightP[idx].diffuse  = glm::vec3(0.0f, 2.0f, 5.0f);
      lightP[idx].specular = glm::vec3(1.0f, 1.0f, 1.0f);
      lightP[idx].ambient  = glm::vec3(0.0f, 0.0f, 0.0f);

      lightP[idx].c0 = 1.0f;
      lightP[idx].c1 = 0.1f;
      lightP[idx].c2 = 0.2f;
   }

   // BARRA 2: MAGENTA
   float xPos2 = cos(t * 2.2f) * amplitud;

   for(int i = 0; i < 5; i++) {
      int idx = baseIndex + 5 + i;
      float yPos = 2.5f + (i * 5.0f);

      lightP[idx].position = glm::vec3(xPos2, yPos, zLuz);
      lightP[idx].diffuse  = glm::vec3(5.0f, 0.0f, 4.0f);
      lightP[idx].specular = glm::vec3(1.0f, 1.0f, 1.0f);
      lightP[idx].ambient  = glm::vec3(0.0f, 0.0f, 0.0f);

      lightP[idx].c0 = 1.0f;
      lightP[idx].c1 = 0.1f;
      lightP[idx].c2 = 0.2f;
   }
}

void animacionDiagonalParedes()
{
    float t = glfwGetTime();

    float paredX = 19.0f;
    float velocidad = 25.0f;
    float frecuenciaOnda = 0.5f;
    float amplitudOnda = 1.5f;

    glm::vec3 cFrio = glm::vec3(0.0f, 1.0f, 1.0f);
    glm::vec3 cMedio = glm::vec3(0.5f, 0.0f, 1.0f);
    glm::vec3 cCaliente = glm::vec3(1.0f, 0.2f, 0.0f);

    // PARED IZQUIERDA
    int lucesSerpiente = 15;
    int baseIdx = 11;

    for(int i = 0; i < lucesSerpiente; i++)
    {
        int idx = baseIdx + i;
        float lag = i * 0.5f;

        float ciclo = fmod((t * velocidad) - lag, 130.0f);
        float zActual = -65.0f + ciclo;

        if(zActual > 55.0f || zActual < -55.0f) {
            lightP[idx].diffuse = glm::vec3(0.0f);
            continue;
        }

        // Interpolación de altura
        float factorAltura = glm::clamp((zActual + 50.0f) / 100.0f, 0.0f, 1.0f);
        float yBase = glm::mix(0.0f, 22.0f, factorAltura);

        float ruidoElectrico = sin((zActual * frecuenciaOnda) + (t * 10.0f)) * (amplitudOnda * 1.5f);
        float yFinal = yBase + ruidoElectrico;

        lightP[idx].position = glm::vec3(-paredX, yFinal, zActual);

        // Lógica de Color e Intensidad
        float calor = glm::clamp((zActual + 50.0f) / 100.0f, 0.0f, 1.0f);
        glm::vec3 colorFinal = (calor < 0.5f)
            ? glm::mix(cFrio, cMedio, calor * 2.0f)
            : glm::mix(cMedio, cCaliente, (calor - 0.5f) * 2.0f);

        float estrobo = (sin(t * 60.0f + i) > 0.0f) ? 2.0f : 0.8f;

        if (i < 3) { // Cabeza
             lightP[idx].diffuse = glm::vec3(1.0f, 1.0f, 1.0f) * 5.0f;
             lightP[idx].c2 = 0.5f;
        } else { // Cuerpo
             lightP[idx].diffuse = colorFinal * estrobo;
             lightP[idx].c2 = 0.8f;
        }

        lightP[idx].specular = glm::vec3(1.0f);
        lightP[idx].ambient = glm::vec3(0.0f);
        lightP[idx].c0 = 1.0f;
        lightP[idx].c1 = 0.1f;
    }

    // PARED DERECHA
    int baseIdxDer = 26;
    int lucesLluvia = 15;

    for(int i = 0; i < lucesLluvia; i++)
    {
        int idx = baseIdxDer + i;
        float velocidadGota = velocidad * (1.0f + (i % 3) * 0.5f);
        float ciclo = fmod((t * velocidadGota) + (i * 10.0f), 130.0f);

        float zActual = 65.0f - ciclo;
        float glitchY = (sin(t * 20.0f + i) > 0.9f) ? 1.0f : 0.0f;
        float yActual = 22.0f - (ciclo * 0.2f) + glitchY;

        lightP[idx].position = glm::vec3(paredX, yActual, zActual);

        if (sin(t * 40.0f + i) > 0.95f) { // Efecto corrupto
            lightP[idx].diffuse = glm::vec3(1.0f, 0.0f, 1.0f) * 5.0f;
            lightP[idx].c2 = 0.1f;
        } else {
            lightP[idx].diffuse = glm::vec3(0.0f, 1.5f, 0.2f);
            lightP[idx].c2 = 0.6f;
        }

        lightP[idx].specular = glm::vec3(1.0f);
        lightP[idx].ambient = glm::vec3(0.0f);
        lightP[idx].c0 = 1.0f;
        lightP[idx].c1 = 0.1f;
    }
}
void configurarLuzPuntual(int index, glm::vec3 pos, glm::vec3 colorDiff, float c2) {
    if (index >= NLP) return;

    lightP[index].position = pos;
    lightP[index].diffuse  = colorDiff;
    lightP[index].specular = glm::vec3(1.0f);
    lightP[index].ambient  = glm::vec3(0.0f);
    lightP[index].c0 = 1.0f;
    lightP[index].c1 = 0.1f;
    lightP[index].c2 = c2;
}

void dibujarSuelo(glm::mat4 P, glm::mat4 V) {
   for (int i = 0; i < 10; i++) { // Eje Z
      float zPos = -50.0f + (i * 10.0f);
      for (int j = 0; j < 4; j++) { // Eje X
         float xPos = -15.0f + (j * 10.0f);

         glm::mat4 M = glm::translate(I, glm::vec3(xPos, SUELO_Y, zPos))
                     * glm::scale(I, glm::vec3(5.0f, 1.0f, 5.0f));
         drawObjectTex(plane, texAxiomFloor, P, V, M);
      }
   }
}

void dibujarTecho(glm::mat4 P, glm::mat4 V) {
   for (int i = 0; i < 10; i++) {
      float zPos = -50.0f + (i * 10.0f);
      for (int j = 0; j < 4; j++) {
         float xPos = -15.0f + (j * 10.0f);

         glm::mat4 M = glm::translate(I, glm::vec3(xPos, TECHO_Y, zPos))
                     * glm::rotate(I, glm::radians(180.0f), glm::vec3(1.0, 0.0, 0.0)) // Boca abajo
                     * glm::scale(I, glm::vec3(5.0f, 1.0f, 5.0f));
         drawObjectTex(plane, texCeiling, P, V, M);
      }
   }
}

void dibujarDetalles(glm::mat4 P, glm::mat4 V, float nivelSuelo) {
   // Zócalo LED central
   glm::mat4 MatrixLinea = glm::translate(I, glm::vec3(0.0, nivelSuelo + 0.02f, -5.0))
                         * glm::scale(I, glm::vec3(0.3, 1.0, 50.0f));
   drawObjectTex(plane, texZocaloLed, P, V, MatrixLinea);

   // Detalle de suciedad/suelo
   glm::mat4 Suciedad = glm::translate(I, glm::vec3(1.5, nivelSuelo + 0.05f, 2.0))
                      * glm::scale(I, glm::vec3(0.6, 1.0, 0.6));
   drawObjectTex(plane, texRuby, P, V, Suciedad);
}

void dibujarParedesLaterales(glm::mat4 P, glm::mat4 V) {
   float yCentro = SUELO_Y + (ALTO_PARED / 2.0f);
   float escalaY = ALTO_PARED / 2.0f;

   for (int i = 0; i < 5; i++) {
      float zPos = -45.0f + (i * 20.0f);

      for(int k = -1; k <= 1; k += 2) {
         float xPos = k * ANCHO_PASILLO;

         // Pared principal
         glm::mat4 M = glm::translate(I, glm::vec3(xPos, yCentro, zPos))
                     * glm::scale(I, glm::vec3(0.5f, escalaY, 10.0f));
         drawObjectTex(cube, texAxiomWall, P, V, M);

         // Detalles: Neón y Zócalo
         float xNeon = xPos - (k * 0.6f);
         glm::mat4 M_Neon = glm::translate(I, glm::vec3(xNeon, yCentro, zPos))
                          * glm::scale(I, glm::vec3(0.05f, escalaY * 0.96f, 0.05f));
         drawObjectTex(cube, texZocaloLed, P, V, M_Neon);

         float xZocalo = xPos - (k * 0.55f);
         glm::mat4 M_Zoc = glm::translate(I, glm::vec3(xZocalo, SUELO_Y + 0.5f, zPos))
                         * glm::scale(I, glm::vec3(0.1f, 0.5f, 10.0f));
         drawObjectTex(cube, texZocaloLed, P, V, M_Zoc);
      }
   }
}

void dibujarParedesFondo(glm::mat4 P, glm::mat4 V) {
   float yCentro = SUELO_Y + (ALTO_PARED / 2.0f);
   float anchoTotal = (ANCHO_PASILLO + 0.5f) * 2.0f;
   float escalaX = anchoTotal / 4.0f;

   glDisable(GL_BLEND);
   glDepthMask(GL_TRUE);

   // A) Muros laterales (-Z)
   for(int k = -1; k <= 1; k += 2) {
      float xPos = k * (anchoTotal / 4.0f);
      glm::mat4 M_Muro = glm::translate(I, glm::vec3(xPos, yCentro, -55.0f))
                       * glm::scale(I, glm::vec3(escalaX, ALTO_PARED / 2.0f, 0.5f));
      drawObjectTex(cube, texOrganicWall, P, V, M_Muro);
   }

   // B) Fondo Universo Gigante (+Z)
   glm::mat4 M_FondoEspacio = glm::translate(I, glm::vec3(0.0f, yCentro, 90.0f));
   M_FondoEspacio = glm::rotate(M_FondoEspacio, glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));
   M_FondoEspacio = glm::scale(M_FondoEspacio, glm::vec3(anchoTotal * 2.0f, 1.0f, ALTO_PARED * 2.0f));
   drawObjectTex(plane, texSpaceBackground, P, V, M_FondoEspacio);

   // Cristal ventana
   glEnable(GL_BLEND);
   glDepthMask(GL_FALSE);

   float opacidad = 0.02f;
   glBlendColor(0.0f, 0.05f, 0.1f, opacidad);

   glBlendFunc(GL_CONSTANT_COLOR, GL_ONE_MINUS_SRC_ALPHA);
   glm::mat4 M_Cristal = glm::translate(I, glm::vec3(0.0f, yCentro, 45.0f));
   M_Cristal = glm::rotate(M_Cristal, glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));
   M_Cristal = glm::scale(M_Cristal, glm::vec3(escalaX * 2.0f, 1.0f, ALTO_PARED / 2.0f));

   drawObjectTex(plane, texBlueGlass, P, V, M_Cristal);

   // Restaurar
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   glDepthMask(GL_TRUE);
   glDisable(GL_BLEND);
}

void drawEscenario(glm::mat4 P, glm::mat4 V)
{
   dibujarSuelo(P, V);
   dibujarTecho(P, V);
   dibujarParedesLaterales(P, V);
   dibujarParedesFondo(P, V);

   // Detalles (Zócalo y manchas)
   glm::mat4 M_Linea = glm::translate(I, glm::vec3(0.0, SUELO_Y + 0.02f, -5.0))
                     * glm::scale(I, glm::vec3(0.3, 1.0, 50.0f));
   drawObjectTex(plane, texZocaloLed, P, V, M_Linea);

   glm::mat4 M_Suciedad = glm::translate(I, glm::vec3(1.5, SUELO_Y + 0.05f, 2.0))
                        * glm::scale(I, glm::vec3(0.6, 1.0, 0.6));
   drawObjectTex(plane, texRuby, P, V, M_Suciedad);

   // Props: Contenedores y Señal
   float escala = 5.0f;
   drawContenedorSciFi(P, V, glm::vec3(-8.0f, -2.0f, 20.0f), 65.0f, escala);
   drawContenedorSciFi(P, V, glm::vec3(-8.0f, 1.4f, 20.0f), 65.0f, escala);
   drawContenedorSciFi(P, V, glm::vec3(-8.5f, -2.0f, 27.0f), 25.0f, escala);

   glm::mat4 matrizSenal = glm::translate(I, glm::vec3(0.0f, -2.0f, -2.0f))
                         * glm::scale(I, glm::vec3(0.1f));
   drawObjectTex(signal, texSignal, P, V, matrizSenal);
}

void drawContenedorSciFi(glm::mat4 P, glm::mat4 V, glm::vec3 pos, float rotY, float escala) {
   glm::mat4 M = glm::translate(glm::mat4(1.0f), pos);
   M = glm::rotate(M, glm::radians(rotY), glm::vec3(0.0, 1.0, 0.0));
   M = glm::scale(M, glm::vec3(escala));

   drawObjectTex(contenedor, texContenedor, P, V, M);
}