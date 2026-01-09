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

// Shaders
Shaders shaders;

// Modelos
Model sphere;
Model plane;
Model cube;

Model signal;

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
Texture imgAxiomFloor;
Texture imgFloorNormal;
Texture imgFloorSpec;

Texture imgAxiomWall_Albedo;    // Reemplaza a la antigua difusa
Texture imgAxiomWall_Normal;    // Reemplaza a la antigua normal
Texture imgAxiomWall_Roughness; // Se usará para el slot 'specular'

// [NUEVO - COMENTADO] El resto de texturas del set, disponibles para el futuro
// Texture imgAxiomWall_AO;       // Ambient Occlusion
// Texture imgAxiomWall_Metallic; // Metallic
// Texture imgAxiomWall_Height;   // Height / Displacement

Texture imgCeiling_Color;
Texture imgCeiling_Normal;
Texture imgCeiling_Roughness;
Texture imgCeiling_AO;

Texture imgOrganic_Albedo;
Texture imgOrganic_Normal;
Texture imgOrganic_Roughness;


Texture signal_BaseColor;
Texture signal_Roughness;
Texture signal_Metallic;
Texture signal_Normal;
Texture signal_AO;


// Luces y materiales
#define NLD 1
#define NLP 50
#define NLF 6
Light lightG;
Light lightD[NLD];
Light lightP[NLP];
Light lightF[NLF];
Material mluz;
Material mOjo;
Material mNeon;

// TEXTURA ESCENARIO
Textures texAxiomFloor;
Textures texAxiomWall;
Textures texZocaloLed;
Textures texRuby;
Textures texCeiling;
Textures texOrganicWall;

Textures texWhiteMetal;
Textures texGreyMetal;
Textures texBlackMetal;
Textures texBlackRubber;
Textures texBlueGlass;
Textures texRedGlass;

Textures texSignal;


//  Ancho del pasillo
float anchoPasillo = 20.0f;

// Viewport
int w = 500;
int h = 500;

// --------------------------------------------------------
// VARIABLES DE CAMARA (FPS / LIBRE)
// --------------------------------------------------------
float fovy = 60.0;

// [CAMBIO] Iniciamos alphaX en -90 para mirar hacia -Z
float alphaX = -90.0f;
float alphaY = 0.0f;

// [NUEVO] Vectores para cámara libre
glm::vec3 cameraPos   = glm::vec3(0.0f, 2.0f, 15.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);

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

bool modoF5 = false;

int main()
{
   // Inicializamos GLFW
   if (!glfwInit()) return -1;
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
   signal.initModel("resources/models/Sign.obj");

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

   // Texturas escenario
   imgAxiomFloor.initTexture("resources/textures/Scifi_Hex_Wall_Difusse.jpg");
   imgFloorNormal.initTexture("resources/textures/Scifi_Hex_Wall_normal.jpg");
   imgFloorSpec.initTexture("resources/textures/Scifi_Hex_Wall_specular.jpg");
   imgAxiomWall_Albedo.initTexture("resources/textures/scifi_panel_1_color_1k.png");
   imgAxiomWall_Normal.initTexture("resources/textures/scifi_panel_1_normal_1k.png");
   //imgAxiomWall_Roughness.initTexture("resources/textures/scifi_panel_1_roughness_1k.png");
   imgAxiomWall_Roughness.initTexture("resources/textures/scifi_panel_1_metallic_1k.png");
   // imgAxiomWall_AO.initTexture("resources/textures/scifi_panel_1_ao_4k.png");
   // imgAxiomWall_Metallic.initTexture("resources/textures/scifi_panel_1_metallic_4k.png");
   // imgAxiomWall_Height.initTexture("resources/textures/scifi_panel_1_height_4k.png");

   imgCeiling_Color.initTexture("resources/textures/floor_tile_1_color.png");
   imgCeiling_Normal.initTexture("resources/textures/floor_tile_1_normal.png");
   imgCeiling_Roughness.initTexture("resources/textures/floor_tile_1_roughness.png");
   // imgCeiling_AO.initTexture("resources/textures/floor_tile_1_ao.png"); // Opcional

   imgOrganic_Albedo.initTexture("resources/textures/organic_tech_1_color_1k.png");
   imgOrganic_Normal.initTexture("resources/textures/organic_tech_1_normal_1k.png");
   imgOrganic_Roughness.initTexture("resources/textures/organic_tech_1_roughness_1k.png");

   signal_BaseColor.initTexture("resources/textures/sign_basecolor.png");
   signal_Roughness.initTexture("resources/textures/sign_roughness.png");
   signal_Normal.initTexture("resources/textures/sign_normal.png");

   // Configuracion texturas materiales
   texZocaloLed.diffuse  = imgWhiteMetal.getTexture();
   texZocaloLed.specular = imgWhiteMetal.getTexture();
   texZocaloLed.emissive = imgWhiteMetal.getTexture();
   texZocaloLed.normal   = 0;
   texZocaloLed.shininess = 23.0;

   texAxiomWall.diffuse  = imgAxiomWall_Albedo.getTexture();
   texAxiomWall.specular = imgAxiomWall_Roughness.getTexture(); // Usamos roughness aquí
   texAxiomWall.emissive = imgNoEmissive.getTexture();
   texAxiomWall.normal   = imgAxiomWall_Normal.getTexture();
   // Puedes necesitar ajustar el shininess dependiendo de cómo interprete tu shader el mapa de roughness.
   // Si se ve muy brillante, prueba a bajarlo (ej. 10.0). Si se ve muy mate, súbelo.
   texAxiomWall.shininess = 30.0;

   // Luz ambiental global
   lightG.ambient = glm::vec3(0.2, 0.2, 0.3);

   // Luces direccionales
   lightD[0].direction = glm::vec3(0.0, -1.0, 0.0);
   lightD[0].ambient = glm::vec3(0.1, 0.1, 0.1);
   lightD[0].diffuse = glm::vec3(0.7, 0.7, 0.7);
   lightD[0].specular = glm::vec3(0.7, 0.7, 0.7);

   // Luces posicionales
   lightP[0].position = glm::vec3(0.0, 4.0, 0.0);
   lightP[0].ambient = glm::vec3(0.2, 0.2, 0.2);
   lightP[0].diffuse = glm::vec3(0.9, 0.95, 1.0);
   lightP[0].specular = glm::vec3(1.0, 1.0, 1.0);
   lightP[0].c0 = 1.00; lightP[0].c1 = 0.09; lightP[0].c2 = 0.032;

   for(int i = 1; i < NLP; i++) {
      lightP[i].ambient = glm::vec3(0.0, 0.0, 0.0);
      lightP[i].diffuse = glm::vec3(0.0, 0.8, 1.0);
      lightP[i].specular = glm::vec3(0.0, 0.0, 0.0);
      lightP[i].c0 = 1.0; lightP[i].c1 = 0.5; lightP[i].c2 = 0.8;
   }

   // Luces focales (Axiom + Ojos + Sirena)
   lightF[0].position = glm::vec3(-2.0, 2.0, 5.0);
   lightF[0].direction = glm::vec3(2.0, -2.0, -5.0);
   lightF[0].ambient = glm::vec3(0.2, 0.2, 0.2);
   lightF[0].diffuse = glm::vec3(0.9, 0.9, 0.9);
   lightF[0].specular = glm::vec3(0.9, 0.9, 0.9);
   lightF[0].innerCutOff = 10.0; lightF[0].outerCutOff = 15.0;
   lightF[0].c0 = 1.0; lightF[0].c1 = 0.09; lightF[0].c2 = 0.032;

   lightF[1].position = glm::vec3(2.0, 2.0, 5.0);
   lightF[1].direction = glm::vec3(-2.0, -2.0, -5.0);
   lightF[1].ambient = glm::vec3(0.2, 0.2, 0.2);
   lightF[1].diffuse = glm::vec3(0.9, 0.9, 0.9);
   lightF[1].specular = glm::vec3(0.9, 0.9, 0.9);
   lightF[1].innerCutOff = 5.0; lightF[1].outerCutOff = 6.0;
   lightF[1].c0 = 1.0; lightF[1].c1 = 0.09; lightF[1].c2 = 0.032;

   // Ojos
   for(int i=2; i<=3; i++){
       lightF[i].ambient = glm::vec3(0.0, 0.0, 0.0);
       lightF[i].diffuse = glm::vec3(0.8, 0.7, 0.2);
       lightF[i].specular = glm::vec3(0.8, 0.7, 0.2);
       lightF[i].innerCutOff = 15.0; lightF[i].outerCutOff = 25.0;
       lightF[i].c0 = 1.0; lightF[i].c1 = 0.09; lightF[i].c2 = 0.032;
   }
   // Sirena
   for(int i=4; i<=5; i++){
       lightF[i].ambient = glm::vec3(0.0, 0.0, 0.0);
       lightF[i].diffuse = glm::vec3(0.9, 0.9, 0.9);
       lightF[i].specular = glm::vec3(0.9, 0.9, 0.9);
       lightF[i].innerCutOff = 20.0; lightF[i].outerCutOff = 30.0;
       lightF[i].c0 = 1.0; lightF[i].c1 = 0.09; lightF[i].c2 = 0.032;
   }

   // Materiales
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

   mNeon.ambient   = glm::vec4(0.0, 0.0, 0.0, 1.0);
   mNeon.diffuse   = glm::vec4(0.0, 0.0, 0.0, 1.0);
   mNeon.specular  = glm::vec4(1.0, 1.0, 1.0, 1.0);
   mNeon.emissive  = glm::vec4(0.0, 0.9, 1.0, 1.0);
   mNeon.shininess = 128.0;

   // Asignar texturas a structs Textures
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

   texAxiomFloor.diffuse = imgAxiomFloor.getTexture();
   texAxiomFloor.specular = imgFloorSpec.getTexture();
   texAxiomFloor.emissive = imgNoEmissive.getTexture();
   texAxiomFloor.normal = imgFloorNormal.getTexture();
   texAxiomFloor.shininess = 32.0;

   texAxiomWall.diffuse  = imgAxiomWall_Albedo.getTexture();
   texAxiomWall.specular = imgAxiomWall_Roughness.getTexture(); // Usamos roughness aquí
   texAxiomWall.emissive = imgNoEmissive.getTexture();
   texAxiomWall.normal   = imgAxiomWall_Normal.getTexture();
   texAxiomWall.shininess = 50.0;

   texCeiling.diffuse  = imgCeiling_Color.getTexture();
   texCeiling.specular = imgCeiling_Roughness.getTexture();
   texCeiling.emissive = imgNoEmissive.getTexture();
   texCeiling.normal   = imgCeiling_Normal.getTexture();
   texCeiling.shininess = 30.0;

   texOrganicWall.diffuse  = imgOrganic_Albedo.getTexture();
   texOrganicWall.specular = imgOrganic_Roughness.getTexture();
   texOrganicWall.emissive = imgNoEmissive.getTexture();
   texOrganicWall.normal   = imgOrganic_Normal.getTexture();
   texOrganicWall.shininess = 64.0;



   texRuby.diffuse = imgRuby.getTexture();
   texRuby.specular = imgRuby.getTexture();
   texRuby.emissive = imgNoEmissive.getTexture();
   texRuby.normal = 0;
   texRuby.shininess = 128.0;


   texSignal.diffuse  = signal_BaseColor.getTexture();
   texSignal.specular = signal_Roughness.getTexture();
   texSignal.normal   = signal_Normal.getTexture();
   texSignal.shininess = 10.0f;
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
   animacionHyperScanner();
   animacionDiagonalParedes();

   float velocidadGiro = 2.0f;
   if (giroIzq) anguloGiro += velocidadGiro;
   if (giroDer) anguloGiro -= velocidadGiro;

   glClearColor(0.0, 0.0, 0.0, 0.0);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   shaders.useShaders();

   // Matriz P
   float nplane = 0.1;
   float fplane = 400.0;
   float aspect = (float)w / (float)h;
   glm::mat4 P = glm::perspective(glm::radians(fovy), aspect, nplane, fplane);

   // -----------------------------------------------------------------------
   // CÁLCULO DE CÁMARA (LIBRE / F5)
   // -----------------------------------------------------------------------
   glm::mat4 V;

   if (modoF5)
   {
      // Modo F5: Cámara detrás del robot
      float distancia = 12.5f;
      float altura = 5.0f;
      float rad = glm::radians(anguloGiro);

      // Posición de la cámara calculada relativa al ángulo del robot
      glm::vec3 camPosF5;
      camPosF5.x = posX - (sin(rad) * distancia);
      camPosF5.y = altura;
      camPosF5.z = posZ - (cos(rad) * distancia);

      // Miramos hacia el robot (un poco por encima del suelo)
      glm::vec3 target = glm::vec3(posX, 4.0f, posZ);

      V = glm::lookAt(camPosF5, target, glm::vec3(0.0f, 1.0f, 0.0f));
      shaders.setVec3("ueye", camPosF5);
   }
   else
   {
      // Modo FPS: Cámara Libre
      glm::vec3 front;
      front.x = cos(glm::radians(alphaX)) * cos(glm::radians(alphaY));
      front.y = sin(glm::radians(alphaY));
      front.z = sin(glm::radians(alphaX)) * cos(glm::radians(alphaY));
      cameraFront = glm::normalize(front);

      V = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
      shaders.setVec3("ueye", cameraPos);
   }
   // -----------------------------------------------------------------------

   glm::mat4 T = glm::translate(I, glm::vec3(posX, -2.0, posZ));
   glm::mat4 R = glm::rotate(I, glm::radians(anguloGiro), glm::vec3(0.0, 1.0, 0.0));
   glm::mat4 S = glm::scale(I, glm::vec3(0.05, 0.05, 0.05));

   luzOjos(T * R * S);

   // Fijamos las luces
   setLights(P, V);

   drawEscenario(P, V);
   drawMO(P, V, T * R * S);
   drawObjectTex(signal, texSignal, P, V,
    glm::scale(
        glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -2.0f, -2.0f)),
        glm::vec3(0.1f)
    )
);
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
   glViewport(0, 0, width, height);
   w = width;
   h = height;
}

void funKey(GLFWwindow *window, int key, int scancode, int action, int mods)
{
   float cameraSpeed = 1.0f; // Velocidad de movimiento de la cámara

   switch (key)
   {
   // --- CONTROLES CÁMARA LIBRE (FLECHAS) ---
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
         // Producto cruz normalizado para moverse lateralmente (Strafe)
         cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
      break;

   case GLFW_KEY_RIGHT:
      if (action == GLFW_PRESS || action == GLFW_REPEAT)
         cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
      break;
   // -----------------------------------------
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

   // Limitamos la vista arriba/abajo para no dar la vuelta (Gimbal Lock)
   float limY = 89.0f;
   if (alphaY > limY) alphaY = limY;
   if (alphaY < -limY) alphaY = -limY;
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
   float maxInclinacionX = 0.0f;
   float maxInclinacionZ = 0.0f;


   float limiteX = 17.3f;
   float limiteZ = 47.3f;

   // 1. CÁLCULO DE MOVIMIENTO
   if (movW) {
      posX += sin(rad) * velocidadMov;
      posZ += cos(rad) * velocidadMov;
      rotRueda = glm::rotate(I, glm::radians(velocidadRot), glm::vec3(1.0, 0.0, 0.0)) * rotRueda;
      if (animacionActiva) maxInclinacionX = 40.0f;
      if (!animacionActiva) maxInclinacionX = 15.0f;
   }
   if (movS) {
      posX -= sin(rad) * velocidadMov;
      posZ -= cos(rad) * velocidadMov;
      rotRueda = glm::rotate(I, glm::radians(-velocidadRot), glm::vec3(1.0, 0.0, 0.0)) * rotRueda;
      maxInclinacionX = -15.0f;
   }
   if (movA) {
      posX += cos(rad) * velocidadMov;
      posZ -= sin(rad) * velocidadMov;
      rotRueda = glm::rotate(I, glm::radians(velocidadRot), glm::vec3(0.0, 0.0, 1.0)) * rotRueda;
      maxInclinacionZ = -15.0f;
   }
   if (movD) {
      posX -= cos(rad) * velocidadMov;
      posZ += sin(rad) * velocidadMov;
      rotRueda = glm::rotate(I, glm::radians(-velocidadRot), glm::vec3(0.0, 0.0, 1.0)) * rotRueda;
      maxInclinacionZ = 15.0f;
   }

   // 2. RESTRICCIÓN DE LÍMITES (LO NUEVO)
   // Si el cálculo anterior te sacó del mapa, esto te "empuja" suavemente al borde.

   // Eje X (Paredes laterales)
   if (posX > limiteX) posX = limiteX;
   if (posX < -limiteX) posX = -limiteX;

   // Eje Z (Fondo y Frente)
   if (posZ > limiteZ) posZ = limiteZ;
   if (posZ < -limiteZ) posZ = -limiteZ;

   // 3. INCLINACIÓN
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

   glm::vec4 posOjoIzq = glm::vec4(-8.0f, 60.0f, 26.6f, 1.0f);
   glm::vec4 posOjoDer = glm::vec4(8.0f, 60.0f, 26.6f, 1.0f);
   glm::vec4 direccionOjos = glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);

   lightF[2].position  = glm::vec3(Mcabeza * posOjoIzq);
   lightF[2].direction = glm::normalize(glm::vec3(Mcabeza * direccionOjos));

   lightF[3].position  = glm::vec3(Mcabeza * posOjoDer);
   lightF[3].direction = glm::normalize(glm::vec3(Mcabeza * direccionOjos));
}

void animacionHyperScanner()
{
    float t = glfwGetTime();

    // --- CORRECCIÓN DE POSICIÓN ---
    // Si tu pared del fondo la pusiste en Z = -26.0 (según corregimos antes),
    // ponemos la luz en -24.0. (2 metros de distancia es seguro).
    float zLuz = -48.0f;

    // Límites de movimiento en X
    float amplitud = 19.0f;

    // --- CORRECCIÓN DE ÍNDICES ---
    // Usamos luces bajas (1 al 10) para evitar limites del shader.
    // La luz 0 es la del techo/ambiente, esa no la tocamos.
    int baseIndex = 1;

    // ---------------------------------------------------------
    // BARRA 1: CYAN (Izquierda -> Derecha)
    // ---------------------------------------------------------
    float xPos1 = sin(t * 1.5f) * amplitud;

    for(int i = 0; i < 5; i++) {
        int idx = baseIndex + i; // Luces 1, 2, 3, 4, 5

        // Estiramos más la barra verticalmente (de -2 a +10)
        float yPos = -2.0f + (i * 3.0f);

        lightP[idx].position = glm::vec3(xPos1, yPos, zLuz);

        // POTENCIA AUMENTADA (Valores > 1.0 hacen que brille más)
        lightP[idx].diffuse  = glm::vec3(0.0f, 2.0f, 5.0f); // Cyan muy fuerte
        lightP[idx].specular = glm::vec3(1.0f, 1.0f, 1.0f);
        lightP[idx].ambient  = glm::vec3(0.0f, 0.0f, 0.0f);

        // --- CORRECCIÓN DE ATENUACIÓN ---
        // c2 bajo (0.1) = la luz llega muy lejos.
        // c2 alto (4.0) = la luz muere enseguida.
        // Lo bajamos a 0.2 para asegurar que se vea la mancha en la pared.
        lightP[idx].c0 = 1.0f;
        lightP[idx].c1 = 0.1f;
        lightP[idx].c2 = 0.2f;
    }

    // ---------------------------------------------------------
    // BARRA 2: MAGENTA (Derecha -> Izquierda)
    // ---------------------------------------------------------
    float xPos2 = cos(t * 2.2f) * amplitud;

    for(int i = 0; i < 5; i++) {
        int idx = baseIndex + 5 + i; // Luces 6, 7, 8, 9, 10

        float yPos = -1.0f + (i * 3.0f);

        lightP[idx].position = glm::vec3(xPos2, yPos, zLuz);

        // Magenta Potente
        lightP[idx].diffuse  = glm::vec3(5.0f, 0.0f, 4.0f);
        lightP[idx].specular = glm::vec3(1.0f, 1.0f, 1.0f);
        lightP[idx].ambient  = glm::vec3(0.0f, 0.0f, 0.0f);

        // Misma atenuación segura
        lightP[idx].c0 = 1.0f;
        lightP[idx].c1 = 0.1f;
        lightP[idx].c2 = 0.2f;
    }
}

void animacionDiagonalParedes()
{float t = glfwGetTime();

    // 1. AJUSTE DE ANCHO
    // Antes 9.2f -> Ahora 19.0f (Para pegar con la pared de ancho 20)
    float paredX = 19.0f;

    // 2. CONFIGURACIÓN DEL CAOS
    float velocidad = 25.0f;
    float frecuenciaOnda = 0.5f;
    float amplitudOnda = 1.5f;

    glm::vec3 cFrio = glm::vec3(0.0f, 1.0f, 1.0f);
    glm::vec3 cMedio = glm::vec3(0.5f, 0.0f, 1.0f);
    glm::vec3 cCaliente = glm::vec3(1.0f, 0.2f, 0.0f);

    // -------------------------------------------------------------------------
    // GRUPO 1: PARED IZQUIERDA (El Rayo Ascendente)
    // -------------------------------------------------------------------------
    // Usamos las mismas luces (10 a 24)
    int lucesSerpiente = 15;
    int baseIdx = 10;

    for(int i = 0; i < lucesSerpiente; i++)
    {
        int idx = baseIdx + i;
        float lag = i * 0.5f; // Un poco más de separación entre bolas

        // 3. AJUSTE DE LARGO (CICLO)
        // El pasillo mide 100m (-50 a 50). El ciclo debe ser mayor (130m) para dar margen.
        float ciclo = fmod((t * velocidad) - lag, 130.0f);

        // Empezamos en -65 para asegurarnos de que cruza todo el pasillo
        float zActual = -65.0f + ciclo;

        // Apagar si sale del rango visible (-55 a +55)
        if(zActual > 55.0f || zActual < -55.0f) {
            lightP[idx].diffuse = glm::vec3(0.0f);
            continue;
        }

        // 4. AJUSTE DE PENDIENTE (Math)
        // "zActual / 10.0f" -> Pendiente más suave para que no se vaya al techo muy rápido
        // Sumamos 4.0f para que empiece a media altura
        float yBase = 4.0f + (zActual * 0.12f);

        float ruidoElectrico = sin((zActual * frecuenciaOnda) + (t * 10.0f)) * amplitudOnda;
        float yFinal = yBase + ruidoElectrico;

        // Posicionar en la pared IZQUIERDA (-paredX)
        lightP[idx].position = glm::vec3(-paredX, yFinal, zActual);

        // --- COLOR (Ajustado al nuevo rango de Z) ---
        // Normalizamos Z de -50..50 a 0..1
        float calor = (zActual + 50.0f) / 100.0f;
        calor = glm::clamp(calor, 0.0f, 1.0f);

        glm::vec3 colorFinal;
        if(calor < 0.5f) {
            float mezcla = calor * 2.0f;
            colorFinal = glm::mix(cFrio, cMedio, mezcla);
        } else {
            float mezcla = (calor - 0.5f) * 2.0f;
            colorFinal = glm::mix(cMedio, cCaliente, mezcla);
        }

        float estrobo = (sin(t * 60.0f + i) > 0.0f) ? 2.0f : 0.8f;

        if (i < 3) {
             lightP[idx].diffuse = glm::vec3(1.0f, 1.0f, 1.0f) * 5.0f;
             lightP[idx].c2 = 0.5f;
        } else {
             lightP[idx].diffuse = colorFinal * estrobo;
             lightP[idx].c2 = 1.0f; // Atenuación ajustada para pasillo ancho
        }

        lightP[idx].specular = glm::vec3(1.0f);
        lightP[idx].ambient = glm::vec3(0.0f);
        lightP[idx].c0 = 1.0f; lightP[idx].c1 = 0.1f;
    }

    // -------------------------------------------------------------------------
    // GRUPO 2: PARED DERECHA (Lluvia de Datos Matrix)
    // -------------------------------------------------------------------------
    int baseIdxDer = 25;
    int lucesLluvia = 15;

    for(int i = 0; i < lucesLluvia; i++)
    {
        int idx = baseIdxDer + i;

        float velocidadGota = velocidad * (1.0f + (i % 3) * 0.5f);

        // Ciclo largo invertido
        float ciclo = fmod((t * velocidadGota) + (i * 10.0f), 130.0f);
        float zActual = 65.0f - ciclo; // De +65 a -65

        // Glitch vertical
        float glitchY = (sin(t * 20.0f + i) > 0.9f) ? 1.0f : 0.0f;

        // Altura constante con variación leve
        float yActual = 10.0f - (ciclo * 0.1f) + glitchY; // Cae suavemente

        // Posicionar en la pared DERECHA (+paredX)
        lightP[idx].position = glm::vec3(paredX, yActual, zActual);

        bool corrupto = (sin(t * 40.0f + i) > 0.95f);

        if (corrupto) {
            lightP[idx].diffuse = glm::vec3(1.0f, 0.0f, 1.0f) * 5.0f;
            lightP[idx].c2 = 0.1f; // Explosión de luz más grande para llenar el ancho
        } else {
            lightP[idx].diffuse = glm::vec3(0.0f, 1.5f, 0.2f);
            lightP[idx].c2 = 0.6f; // Atenuación suave para que se vea desde lejos
        }

        lightP[idx].specular = glm::vec3(1.0f);
        lightP[idx].ambient = glm::vec3(0.0f);
        lightP[idx].c0 = 1.0f; lightP[idx].c1 = 0.1f;
    }
}

void configurarLuzPuntual(int index, glm::vec3 pos, glm::vec3 colorDiff, float c2) {
    if (index >= NLP) return; // Protección contra desbordamiento del array

    lightP[index].position = pos;
    lightP[index].diffuse  = colorDiff;
    lightP[index].specular = glm::vec3(1.0f);
    lightP[index].ambient  = glm::vec3(0.0f);
    lightP[index].c0 = 1.0f;
    lightP[index].c1 = 0.1f;
    lightP[index].c2 = c2; // Atenuación
}

void dibujarSuelo(glm::mat4 P, glm::mat4 V, float nivelSuelo) {
   float escalaSuelo = 5.0f;
   float tamanoLosa = 10.0f;
   for (int i = 0; i < 10; i++) {
      float zPos = -50.0f + (i * tamanoLosa);
      for (int j = 0; j < 4; j++) {
         float xPos = -15.0f + (j * tamanoLosa);

         glm::mat4 MatrixSuelo = glm::translate(I, glm::vec3(xPos, nivelSuelo, zPos))
                               * glm::scale(I, glm::vec3(escalaSuelo, 1.0, escalaSuelo));
         drawObjectTex(plane, texAxiomFloor, P, V, MatrixSuelo);
      }
   }
}

void dibujarTecho(glm::mat4 P, glm::mat4 V, float nivelTecho) {
   float escalaSuelo = 5.0f;
   float tamanoLosa = 10.0f;
   for (int i = 0; i < 10; i++) {
      float zPos = -50.0f + (i * tamanoLosa);
      for (int j = 0; j < 4; j++) {
         float xPos = -15.0f + (j * tamanoLosa);

         glm::mat4 MatrixTecho = glm::translate(I, glm::vec3(xPos, nivelTecho, zPos))
                               * glm::rotate(I, glm::radians(180.0f), glm::vec3(1.0, 0.0, 0.0))
                               * glm::scale(I, glm::vec3(escalaSuelo, 1.0, escalaSuelo));
         drawObjectTex(plane, texCeiling, P, V, MatrixTecho);
      }
   }
}

void dibujarDetalles(glm::mat4 P, glm::mat4 V, float nivelSuelo) {
   // CAMBIO: Escala Z de 25 a 50 (Total 100 de largo)
   glm::mat4 MatrixLinea = glm::translate(I, glm::vec3(0.0, nivelSuelo + 0.02f, -5.0))
                         * glm::scale(I, glm::vec3(0.3, 1.0, 50.0f));
   drawObjectTex(plane, texZocaloLed, P, V, MatrixLinea);

   // Suciedad (puedes moverla si quieres, o dejarla ahí)
   glm::mat4 Suciedad = glm::translate(I, glm::vec3(1.5, nivelSuelo + 0.05f, 2.0))
                      * glm::scale(I, glm::vec3(0.6, 1.0, 0.6));
   drawObjectTex(plane, texRuby, P, V, Suciedad);
}

void dibujarParedesLaterales(glm::mat4 P, glm::mat4 V, float nivelSuelo, float escalaGiganteY, int &luzIndex, float anchoPasillo) {

    // Altura total y centro
    float alturaReal = escalaGiganteY * 2.0f;
    float yCentro = nivelSuelo + escalaGiganteY;

    // Configuración Z (Longitudinal)
    float escalaGiganteZ = 10.0f;
    float pasoZ = 20.0f;
    float zInicio = -45.0f;
    int numBloques = 5;

    // Detalles
    float offsetNeon = 0.6f;
    float offsetLuz  = 1.2f; // Que tan adentro de la pared está la luz
    glm::vec3 colorNeon = glm::vec3(0.0f, 1.0f, 1.0f);
    glm::vec3 escalaNeon = glm::vec3(0.05f, alturaReal * 0.48f, 0.05f);

    float offsetsY[3] = { -8.0f, 0.0f, 8.0f };

    for (int i = 0; i < numBloques; i++) {
        float zPos = zInicio + (i * pasoZ);
        float zCentroDetalle = zPos;

        // 1. PAREDES GIGANTES (Usando anchoPasillo calculado)
        // Nota: anchoPasillo es la distancia del centro (0,0) al CENTRO de la pared.
        glm::mat4 M_Izq = glm::translate(I, glm::vec3(-anchoPasillo, yCentro, zPos))
                        * glm::scale(I, glm::vec3(0.5f, escalaGiganteY, escalaGiganteZ));
        drawObjectTex(cube, texAxiomWall, P, V, M_Izq);

        glm::mat4 M_Der = glm::translate(I, glm::vec3(anchoPasillo, yCentro, zPos))
                        * glm::scale(I, glm::vec3(0.5f, escalaGiganteY, escalaGiganteZ));
        drawObjectTex(cube, texAxiomWall, P, V, M_Der);

        // 2. NEONES
        // Ajustamos la posición X para que sobresalgan un poco hacia el pasillo
        // anchoPasillo es el centro de la pared. Si restamos offsetNeon vamos hacia el centro (0) en el lado derecho.
        glm::vec3 posNeonIzq = glm::vec3(-anchoPasillo + offsetNeon, yCentro, zCentroDetalle);
        glm::vec3 posNeonDer = glm::vec3( anchoPasillo - offsetNeon, yCentro, zCentroDetalle);

        drawObjectTex(cube, texZocaloLed, P, V, glm::translate(I, posNeonIzq) * glm::scale(I, escalaNeon));
        drawObjectTex(cube, texZocaloLed, P, V, glm::translate(I, posNeonDer) * glm::scale(I, escalaNeon));

        // 3. LUCES
        for(int k = 0; k < 3; k++) {
            float alturaLuz = yCentro + offsetsY[k];
            if (luzIndex < NLP)
                configurarLuzPuntual(luzIndex++, glm::vec3(-anchoPasillo + offsetLuz, alturaLuz, zCentroDetalle), colorNeon, 0.4f);
            if (luzIndex < NLP)
                configurarLuzPuntual(luzIndex++, glm::vec3(anchoPasillo - offsetLuz, alturaLuz, zCentroDetalle), colorNeon, 0.4f);
        }

        // 4. ZÓCALO LED (Base)
        // Lo ponemos ligeramente más adentro que la pared para que tape la unión con el suelo
        float posY_Zocalo = nivelSuelo + 0.5f;
        // anchoPasillo - 0.55 pone el zocalo justo pegado a la cara interna de la pared
        float xZocaloDist = anchoPasillo - 0.55f;

        glm::vec3 posZocaloIzq = glm::vec3(-xZocaloDist, posY_Zocalo, zPos);
        glm::vec3 posZocaloDer = glm::vec3( xZocaloDist, posY_Zocalo, zPos);

        drawObjectTex(cube, texZocaloLed, P, V, glm::translate(I, posZocaloIzq) * glm::scale(I, glm::vec3(0.1f, 0.5f, escalaGiganteZ)));
        drawObjectTex(cube, texZocaloLed, P, V, glm::translate(I, posZocaloDer) * glm::scale(I, glm::vec3(0.1f, 0.5f, escalaGiganteZ)));
    }
}

void dibujarParedesCierre(glm::mat4 P, glm::mat4 V, float nivelSuelo, float escalaGiganteY, float anchoPasillo) {

   float yCentro = nivelSuelo + escalaGiganteY;

   float anchoTotalRequerido = (anchoPasillo + 0.5f) * 2.0f;

   float anchoRealPanel = anchoTotalRequerido / 2.0f;

   float escalaPanelX = anchoRealPanel / 2.0f;

   float xOffset = anchoRealPanel / 2.0f;

   for(int i = 0; i < 2; i++) {
      // i=0 -> Izquierda (-xOffset), i=1 -> Derecha (+xOffset)
      float xPos = (i == 0) ? -xOffset : xOffset;

      // PARED TRASERA
      glm::vec3 posFondo = glm::vec3(xPos, yCentro, -55.0f);
      glm::mat4 M_Fondo = glm::translate(I, posFondo)
                        * glm::scale(I, glm::vec3(escalaPanelX, escalaGiganteY, 0.5f));
      drawObjectTex(cube, texOrganicWall, P, V, M_Fondo);

      // PARED DELANTERA
      glm::vec3 posFrente = glm::vec3(xPos, yCentro, 45.0f);
      glm::mat4 M_Frente = glm::translate(I, posFrente)
                         * glm::scale(I, glm::vec3(-escalaPanelX, escalaGiganteY, -0.5f));
      drawObjectTex(cube, texOrganicWall, P, V, M_Frente);
   }
}

void drawEscenario(glm::mat4 P, glm::mat4 V)
{
   float nivelSuelo = -2.0f;
   float escalaGiganteY = 12.0f; // Altura paredes = 24.0
   float nivelTecho = 22.0f;

   float anchoPasillo = 20.5f;

   int luzIndex = 1;

   dibujarSuelo(P, V, nivelSuelo);
   dibujarTecho(P, V, nivelTecho);
   dibujarDetalles(P, V, nivelSuelo);

   // Pasamos anchoPasillo a las funciones para que todo cuadre
   dibujarParedesLaterales(P, V, nivelSuelo, escalaGiganteY, luzIndex, anchoPasillo);
   dibujarParedesCierre(P, V, nivelSuelo, escalaGiganteY, anchoPasillo);
}