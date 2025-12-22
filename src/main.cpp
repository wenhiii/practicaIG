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

void dibujarEscenario(glm::mat4 P, glm::mat4 V);

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
Model brazoDerecho;
Model brazoIzquierdo;
Model cara;
Model casco;
Model cristalSirena;
Model cubreRueda;
Model cuello;
Model cuerpo;
Model ejeBrazoDerecho;
Model ejeBrazoIzquierdo;
Model mochila;
Model rueda;
Model tapaAspiradora;
Model tapaCasco;
Model tapaSirena;

// Imagenes (texturas)
Texture imgNoEmissive;
Texture imgWhiteMetal;
Texture imgGreyMetal;
Texture imgBlackMetal;
Texture imgBlackRubber;
Texture imgBlueGlass;
Texture imgRedGlass;



// Imagenes TEXTURA ESCENARIO
Texture imgRuby;

Texture imgAxiomFloor;  // Imagen de color (ya la tenías)
Texture imgFloorNormal; // <--- [NUEVO] Para el relieve
Texture imgFloorSpec;   // <--- [NUEVO] Para el brillo (blanco y negro)

Texture imgAxiomWall;
Texture imgGuideLine;
Texture imgWallNormal;

// FIN Imagenes TEXTURA ESCENARIO

// Luces y materiales
#define NLD 1
#define NLP 1
#define NLF 6
Light lightG;
Light lightD[NLD];
Light lightP[NLP];
Light lightF[NLF];
Material mluz;
Material mOjo;

// TEXTURA ESCENARIO
Textures texAxiomFloor;
Textures texAxiomWall;
Textures texGuideLine;

Textures texRuby;
// FIN TEXTURA ESCENARIO

Textures texWhiteMetal;
Textures texGreyMetal;
Textures texBlackMetal;
Textures texBlackRubber;
Textures texBlueGlass;
Textures texRedGlass;

//  Ancho del pasillo

float anchoPasillo = 10.0f;

// Viewport
int w = 500;
int h = 500;

// Movimiento de camara (Ratón)
float fovy = 60.0;
float alphaX = 0.0;
float alphaY = 0.0;

double lastX = 0.0;
double lastY = 0.0;
bool firstClick = true;
float sensitivity = 0.5f;

// Movimiento M-O
float anguloAspiradora = 0.0f;
float anguloBrazos = 0.0f;
float anguloSirena = 0.0f;
float alturaSirena = -7.42f;
float anguloGiro = 0.0f;
float posX = 0.0f;
float posZ = 0.0f;
float inclinacionX = 0.0f;
float inclinacionZ = 0.0f;
glm::mat4 rotRueda = I;

bool animacionActiva = false;
bool sirenaLevantada = false;
bool giroIzq = false;
bool giroDer = false;
bool movW = false;
bool movS = false;
bool movA = false;
bool movD = false;

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

   // Imagenes (texturas)
   imgNoEmissive.initTexture("resources/textures/imgNoEmissive.png");

   imgRuby.initTexture("resources/textures/imgRuby.png");

   imgWhiteMetal.initTexture("resources/textures/whiteMetal.jpg");
   imgGreyMetal.initTexture("resources/textures/greyMetal.jpg");
   imgBlackMetal.initTexture("resources/textures/blackMetal.jpg");
   imgBlackRubber.initTexture("resources/textures/blackRubber.jpg");
   imgBlueGlass.initTexture("resources/textures/blueGlass.png");
   imgRedGlass.initTexture("resources/textures/redGlass.png");

   // [NUEVO] Cargamos texturas para el escenario (reutilizamos las existentes si no tienes nuevas)
   // 1. Albedo (Color) -> imgAxiomFloor
   imgAxiomFloor.initTexture("resources/textures/Scifi_Hex_Wall_Difusse.jpg");

   // 2. Normal (Relieve) -> imgFloorNormal
   imgFloorNormal.initTexture("resources/textures/Scifi_Hex_Wall_normal.jpg");

   // 3. Specular (Mapa de Brillo) -> imgFloorSpec
   imgFloorSpec.initTexture("resources/textures/Scifi_Hex_Wall_specular.jpg");


   imgAxiomWall.initTexture("resources/textures/paredXD.jpg");
   imgGuideLine.initTexture("resources/textures/XD.jpg");

   imgWallNormal.initTexture("resources/textures/imgWallNormal.png");
   imgRuby.initTexture("resources/textures/imgRuby.png");
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

   // Luces Ojos M-O
   lightF[2].ambient = glm::vec3(0.0, 0.0, 0.0);
   lightF[2].diffuse = glm::vec3(0.8, 0.7, 0.2);
   lightF[2].specular = glm::vec3(0.8, 0.7, 0.2);
   lightF[2].innerCutOff = 15.0;
   lightF[2].outerCutOff = lightF[2].innerCutOff + 10.0;
   lightF[2].c0 = 1.000;
   lightF[2].c1 = 0.090;
   lightF[2].c2 = 0.032;

   lightF[3].ambient = glm::vec3(0.0, 0.0, 0.0);
   lightF[3].diffuse = glm::vec3(0.8, 0.7, 0.2);
   lightF[3].specular = glm::vec3(0.8, 0.7, 0.2);
   lightF[3].innerCutOff = 15.0;
   lightF[3].outerCutOff = lightF[3].innerCutOff + 10.0;
   lightF[3].c0 = 1.000;
   lightF[3].c1 = 0.090;
   lightF[3].c2 = 0.032;

   // Luces Sirena M-O
   lightF[4].ambient = glm::vec3(0.0, 0.0, 0.0);
   lightF[4].diffuse = glm::vec3(0.9, 0.9, 0.9);
   lightF[4].specular = glm::vec3(0.9, 0.9, 0.9);
   lightF[4].innerCutOff = 20.0;
   lightF[4].outerCutOff = 30.0;
   lightF[4].c0 = 1.0;
   lightF[4].c1 = 0.09;
   lightF[4].c2 = 0.032;

   lightF[5].ambient = glm::vec3(0.0, 0.0, 0.0);
   lightF[5].diffuse = glm::vec3(0.9, 0.9, 0.9);
   lightF[5].specular = glm::vec3(0.9, 0.9, 0.9);
   lightF[5].innerCutOff = 20.0;
   lightF[5].outerCutOff = 30.0;
   lightF[5].c0 = 1.0;
   lightF[5].c1 = 0.09;
   lightF[5].c2 = 0.032;

   // Materiales existentes
   mluz.ambient = glm::vec4(0.0, 0.0, 0.0, 1.0);
   mluz.diffuse = glm::vec4(0.0, 0.0, 0.0, 1.0);
   mluz.specular = glm::vec4(0.0, 0.0, 0.0, 1.0);
   mluz.emissive = glm::vec4(1.0, 1.0, 1.0, 1.0);
   mluz.shininess = 1.0;

   mOjo.ambient = glm::vec4(0.2, 0.2, 0.0, 1.0);
   mOjo.diffuse = glm::vec4(0.9, 0.8, 0.2, 1.0);
   mOjo.specular = glm::vec4(0.0, 0.0, 0.0, 1.0);
   mOjo.emissive = glm::vec4(0.8, 0.7, 0.1, 1.0);
   mOjo.shininess = 0.75;

   texWhiteMetal.diffuse = imgWhiteMetal.getTexture();
   texWhiteMetal.specular = imgWhiteMetal.getTexture();
   texWhiteMetal.emissive = imgNoEmissive.getTexture();
   texWhiteMetal.normal = 0;
   texWhiteMetal.shininess = 64.0;

   texGreyMetal.diffuse = imgGreyMetal.getTexture();
   texGreyMetal.specular = imgGreyMetal.getTexture();
   texGreyMetal.emissive = imgNoEmissive.getTexture();
   texGreyMetal.normal = 0;
   texGreyMetal.shininess = 100.0;

   texBlackMetal.diffuse = imgBlackMetal.getTexture();
   texBlackMetal.specular = imgBlackMetal.getTexture();
   texBlackMetal.emissive = imgNoEmissive.getTexture();
   texBlackMetal.normal = 0;
   texBlackMetal.shininess = 32.0;

   texBlackRubber.diffuse = imgBlackRubber.getTexture();
   texBlackRubber.specular = imgBlackRubber.getTexture();
   texBlackRubber.emissive = imgNoEmissive.getTexture();
   texBlackRubber.normal = 0;
   texBlackRubber.shininess = 10.0;

   texBlueGlass.diffuse = imgBlueGlass.getTexture();
   texBlueGlass.specular = imgBlueGlass.getTexture();
   texBlueGlass.emissive = imgBlueGlass.getTexture();
   texBlueGlass.normal = 0;
   texBlueGlass.shininess = 128.0;

   texRedGlass.diffuse = imgRedGlass.getTexture();
   texRedGlass.specular = imgRedGlass.getTexture();
   texRedGlass.emissive = imgRedGlass.getTexture();
   texRedGlass.normal = 0;
   texRedGlass.shininess = 128.0;

   // 1. Suelo
   // 1. Suelo
   texAxiomFloor.diffuse = imgAxiomFloor.getTexture();   // Tu imagen Albedo
   texAxiomFloor.specular = imgFloorSpec.getTexture();   // Tu imagen Specular (¡Importante!)
   texAxiomFloor.emissive = imgNoEmissive.getTexture();
   texAxiomFloor.normal = imgFloorNormal.getTexture();   // Tu imagen Normal (¡Importante!)

   // Como tienes un mapa specular, subimos el brillo para que el metal resalte
   texAxiomFloor.shininess = 128.0;

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

   texRuby.diffuse = imgRuby.getTexture();
   texRuby.specular = imgRuby.getTexture();
   texRuby.emissive = imgNoEmissive.getTexture();
   texRuby.normal = 0;
   texRuby.shininess = 128.0;
}

void renderScene()
{
   if (animacionActiva)
   {
      anguloAspiradora += 5.0f;
      if (anguloAspiradora > 360.0f)
         anguloAspiradora -= 360.0f;
   }

   levantarSirena();
   movimientoMO();

   float velocidadGiro = 2.0f;

   if (giroIzq)
      anguloGiro += velocidadGiro; // Izquierda
   if (giroDer)
      anguloGiro -= velocidadGiro; // Derecha

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


   glm::mat4 T = glm::translate(I, glm::vec3(posX, 0.0, posZ));
   glm::mat4 R = glm::rotate(I, glm::radians(anguloGiro), glm::vec3(0.0, 1.0, 0.0));
   glm::mat4 S = glm::scale(I, glm::vec3(0.05, 0.05, 0.05));

   luzOjos(S * T * R);

   // Fijamos las luces
   setLights(P, V);


   dibujarEscenario(P, V);
      drawMO(P, V, S * T * R);

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

   for (int i = 0; i < 2; i++)
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
   case GLFW_KEY_G:
      if (action == GLFW_PRESS)
      {
         animacionActiva = !animacionActiva;
      }
      break;

   case GLFW_KEY_F:
      if (action == GLFW_PRESS || action == GLFW_REPEAT)
      {
         anguloBrazos += 2.0f;

         if (anguloBrazos > 5.0f)
            anguloBrazos = 5.0f;
      }
      break;

   case GLFW_KEY_R:
      if (action == GLFW_PRESS || action == GLFW_REPEAT)
      {
         anguloBrazos -= 2.0f;

         if (anguloBrazos < -10.0f)
            anguloBrazos = -10.0f;
      }
      break;
   case GLFW_KEY_Y:
      if (action == GLFW_PRESS)
      {
         sirenaLevantada = !sirenaLevantada;
      }
      break;
   case GLFW_KEY_Q:
      if (action == GLFW_PRESS)
         giroIzq = true;
      if (action == GLFW_RELEASE)
         giroIzq = false;
      break;

   case GLFW_KEY_E:
      if (action == GLFW_PRESS)
         giroDer = true;
      if (action == GLFW_RELEASE)
         giroDer = false;
      break;
   case GLFW_KEY_W:
      if (action == GLFW_PRESS)
         movW = true;
      if (action == GLFW_RELEASE)
         movW = false;
      break;
   case GLFW_KEY_S:
      if (action == GLFW_PRESS)
         movS = true;
      if (action == GLFW_RELEASE)
         movS = false;
      break;
   case GLFW_KEY_A:
      if (action == GLFW_PRESS)
         movA = true;
      if (action == GLFW_RELEASE)
         movA = false;
      break;
   case GLFW_KEY_D:
      if (action == GLFW_PRESS)
         movD = true;
      if (action == GLFW_RELEASE)
         movD = false;
      break;
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
   if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {
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


   alphaX += xoffset * sensitivity;
   alphaY += yoffset * sensitivity;

   float limY = 89.0f;
   if (alphaY > limY)
      alphaY = limY;
   if (alphaY < -limY)
      alphaY = -limY;
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

   glm::mat4 T_Centro = glm::translate(I, centroCristal); // Ir al centro del cristal
   glm::mat4 T_Radio  = glm::translate(I, glm::vec3(radioGiro, 0.0, 0.0)); // Separarse
   glm::mat4 S        = glm::scale(I, glm::vec3(escala)); // Hacerla pequeña

   glm::mat4 R1 = glm::rotate(I, glm::radians(anguloSirena), glm::vec3(0.0, 1.0, 0.0));
   
   glm::mat4 M_Pos1 = M * Televacion * T_Centro * R1 * T_Radio;
   
   drawObjectMat(sphere, mluz, P, V, M_Pos1 * S);

   lightF[4].position  = glm::vec3(M_Pos1 * glm::vec4(0.0, 0.0, 0.0, 1.0));
   lightF[4].direction = glm::normalize(glm::vec3(M * Televacion * R1 * glm::vec4(1.0, 0.0, 0.0, 0.0)));


   glm::mat4 R2 = glm::rotate(I, glm::radians(anguloSirena + 180.0f), glm::vec3(0.0, 1.0, 0.0));
   
   glm::mat4 M_Pos2 = M * Televacion * T_Centro * R2 * T_Radio;

   drawObjectMat(sphere, mluz, P, V, M_Pos2 * S);
   lightF[5].position  = glm::vec3(M_Pos2 * glm::vec4(0.0, 0.0, 0.0, 1.0));
   lightF[5].direction = glm::normalize(glm::vec3(M * Televacion * R2 * glm::vec4(1.0, 0.0, 0.0, 0.0)));

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

   // 1. Girar el rodillo (Mgiro)
   // 2. Moverse con los brazos (Melevacion)
   // 3. Posición del Robot (M)

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

   drawObjectTex(rueda, texBlackRubber, P, V, M * Tvuelta * rotRueda * Tida);
}

void levantarSirena()
{
   float velocidadSirena = 0.05f;

   if (sirenaLevantada)
   {
      if (alturaSirena < 0.0f)
      {
         alturaSirena += velocidadSirena;
         if (alturaSirena > 0.0f)
            alturaSirena = 0.0f;
      }

      anguloSirena += 5.0f; 
      if (anguloSirena > 360.0f) anguloSirena -= 360.0f;
   }
   else
   {
      if (alturaSirena > -7.42f)
      {
         alturaSirena -= velocidadSirena;

         if (alturaSirena < -7.42f)
            alturaSirena = -7.42f;
      }
   }
}

void movimientoMO()
{
   float velocidadMov = 1.0f;

   float velocidadRot = 5.0f;

   float rad = glm::radians(anguloGiro);

   float maxInclinacionX = 0.0f;
   float maxInclinacionZ = 0.0f;

   if (movW)
   {
      posX += sin(rad) * velocidadMov;
      posZ += cos(rad) * velocidadMov;
      rotRueda = glm::rotate(I, glm::radians(velocidadRot), glm::vec3(1.0, 0.0, 0.0)) * rotRueda;
      if (animacionActiva)
         maxInclinacionX = 40.0f;
      if (!animacionActiva)
         maxInclinacionX = 15.0f;
   }
   if (movS)
   {
      posX -= sin(rad) * velocidadMov;
      posZ -= cos(rad) * velocidadMov;
      rotRueda = glm::rotate(I, glm::radians(-velocidadRot), glm::vec3(1.0, 0.0, 0.0)) * rotRueda;
      maxInclinacionX = -15.0f;
   }
   if (movA)
   {
      posX += cos(rad) * velocidadMov;
      posZ -= sin(rad) * velocidadMov;
      rotRueda = glm::rotate(I, glm::radians(velocidadRot), glm::vec3(0.0, 0.0, 1.0)) * rotRueda;
      maxInclinacionZ = -15.0f;
   }
   if (movD)
   {
      posX -= cos(rad) * velocidadMov;
      posZ += sin(rad) * velocidadMov;
      rotRueda = glm::rotate(I, glm::radians(-velocidadRot), glm::vec3(0.0, 0.0, 1.0)) * rotRueda;
      maxInclinacionZ = 15.0f;
   }

   if (inclinacionX < maxInclinacionX) inclinacionX += 0.75f;
   if (inclinacionX > maxInclinacionX) inclinacionX -= 0.75f;
   if (inclinacionZ < maxInclinacionZ) inclinacionZ += 0.75f;
   if (inclinacionZ > maxInclinacionZ) inclinacionZ -= 0.75f;
}


void luzOjos(glm::mat4 M){
   glm::vec3 centro = glm::vec3(0.0f, 9.80f, -3.58f);

   glm::mat4 Tida = glm::translate(glm::mat4(1.0f), -centro);
   glm::mat4 Rx = glm::rotate(I, glm::radians(inclinacionX), glm::vec3(1.0, 0.0, 0.0));
   glm::mat4 Rz = glm::rotate(I, glm::radians(inclinacionZ), glm::vec3(0.0, 0.0, 1.0));
   glm::mat4 Tvuelta = glm::translate(glm::mat4(1.0f), centro);
   
   glm::mat4 Mcabeza = M * Tvuelta * Rz * Rx * Tida;

   // Ojo Izquierdo: -8.0f, 60.0f, 26.6f
   // Ojo Derecho:   8.0f, 60.0f, 26.6f, 1.0f
   glm::vec4 posOjoIzq = glm::vec4(-8.0f, 60.0f, 26.6f, 1.0f);
   glm::vec4 posOjoDer = glm::vec4(8.0f, 60.0f, 26.6f, 1.0f);

   glm::vec4 direccionOjos   = glm::vec4(0.0f, 0.0f, 1.0f, 0.0f); 

   lightF[2].position  = glm::vec3(Mcabeza * posOjoIzq);
   lightF[2].direction = glm::normalize(glm::vec3(Mcabeza * direccionOjos));

   lightF[3].position  = glm::vec3(Mcabeza * posOjoDer);
   lightF[3].direction = glm::normalize(glm::vec3(Mcabeza * direccionOjos));
}


void dibujarEscenario(glm::mat4 P, glm::mat4 V)
{
   // --------------------------------------------------------------------------
   // CONFIGURACIÓN DE DIMENSIONES
   // --------------------------------------------------------------------------
   float nivelSuelo = -2.0f;
   float escalaParedY = 6.0f;       // Altura visual de la pared
   float posY_Pared = nivelSuelo + escalaParedY;
   float escalaZocaloY = 0.5f;      // Altura visual del zócalo
   float posY_Zocalo = nivelSuelo + escalaZocaloY;

   // --------------------------------------------------------------------------
   // 1. EL SUELO
   // --------------------------------------------------------------------------
   // Tu OBJ mide 2. Multiplicamos por 5 para tener baldosas de 10x10 metros.
   float escala = 5.0f;

   // Bucle Z: 4 filas a lo largo del pasillo
   for (int i = 0; i < 4; i++)
   {
      // Calculamos la posición Z (-15, -5, 5, 15)
      float zPos = -15.0f + (i * 10.0f);

      // Bucle X: 2 columnas a lo ancho (Izquierda y Derecha)
      for (int j = 0; j < 2; j++)
      {
         // Si j=0 (izquierda) -> ponemos en -5.0
         // Si j=1 (derecha)   -> ponemos en +5.0
         float xPos = (j == 0) ? -5.0f : 5.0f;

         glm::mat4 MatrixSuelo = glm::translate(I, glm::vec3(xPos, nivelSuelo, zPos))
                               * glm::scale(I, glm::vec3(escala, 1.0, escala));

         drawObjectTex(plane, texAxiomFloor, P, V, MatrixSuelo);
      }
   }

   // --------------------------------------------------------------------------
   // 2. LÍNEA DE GUÍA CENTRAL
   // --------------------------------------------------------------------------
   glm::mat4 MatrixLinea = glm::translate(I, glm::vec3(0.0, nivelSuelo + 0.02f, 0.0))
                         * glm::scale(I, glm::vec3(0.3, 1.0, 40.0));
   drawObjectTex(plane, texGuideLine, P, V, MatrixLinea);

   // --------------------------------------------------------------------------
   // 3. PAREDES Y ZÓCALOS (LUCES LATERALES)
   // --------------------------------------------------------------------------
   for (int i = -3; i <= 3; i++)
   {
      float zPos = i * 5.0f;

      // Pared Izquierda
      glm::mat4 M_Izq = glm::translate(I, glm::vec3(-anchoPasillo, posY_Pared, zPos))
                      * glm::scale(I, glm::vec3(0.5, escalaParedY, 4.0));
      drawObjectTex(cube, texAxiomWall, P, V, M_Izq);

      // Pared Derecha
      glm::mat4 M_Der = glm::translate(I, glm::vec3( anchoPasillo, posY_Pared, zPos))
                      * glm::scale(I, glm::vec3(0.5, escalaParedY, 4.0));
      drawObjectTex(cube, texAxiomWall, P, V, M_Der);

      // Luz Zócalo Izquierda
      glm::mat4 Luz_Izq = glm::translate(I, glm::vec3(-anchoPasillo + 0.6, posY_Zocalo, zPos))
                        * glm::scale(I, glm::vec3(0.1, escalaZocaloY, 4.0));
      drawObjectTex(cube, texGuideLine, P, V, Luz_Izq);

      // Luz Zócalo Derecha
      glm::mat4 Luz_Der = glm::translate(I, glm::vec3( anchoPasillo - 0.6, posY_Zocalo, zPos))
                        * glm::scale(I, glm::vec3(0.1, escalaZocaloY, 4.0));
      drawObjectTex(cube, texGuideLine, P, V, Luz_Der);
   }

   // --------------------------------------------------------------------------
   // 4. SUCIEDAD
   // --------------------------------------------------------------------------
   glm::mat4 Suciedad = glm::translate(I, glm::vec3(1.5, nivelSuelo + 0.05f, 2.0))
                      * glm::scale(I, glm::vec3(0.6, 1.0, 0.6));
   drawObjectTex(plane, texRuby, P, V, Suciedad);
}