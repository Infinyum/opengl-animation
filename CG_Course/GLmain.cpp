//#include "stdafx.h"
#include "GLmain.h"
#include "Avatar.h"
#include "Bullet.h"
#include "Decorations.h"
#include "Texture.h"
#include <windows.h>
#include <mmsystem.h> 

#define ANGULAR_SPEED_SCALE  1.5f
#define LINEAR_SPEED_SCALE   1.f

//---------------------------------------------------//
//   COMPUTER GRAPHICS OPENGL PROJECT (ET3 info S5)  //
//              THOMAS VON ASCHEBERG                 //
//                ROBIN MALMASSON                    //
//				 EURYDICE RUGGIERI					 //
//---------------------------------------------------//

//-------------------------------------------------------------------//
//  THIS PROJECT HAS 10 MODULES (AND RELATED HEADERS) + 1 SHADER :	 //
//                                                  -Avatar.cpp      //
//                                                  -Boxes.cpp       //
//                                                  -Bullet.cpp      //
//                                                  -Cylinder.cpp    //
//													-Decorations.cpp //
//													-GLmain.cpp      //
//													-Object.cpp      //
//													-Spheres.cpp     //
//													-stdadx.cpp      //
//													-Texture.cpp     //
//																	 //
//													-avatar.shader   //
//-------------------------------------------------------------------//
// THIS PROJECT HAS 11 IMAGES FILES (.bmp)  + 3 SOUND FILES (.wav) : //
//                                                  -Town.bmp	     //
//                                                  -building.bmp    //
//                                                  -bricks.bmp      //
//                                                  -jean_mod.bmp    //
//													-leather.bmp     //
//													-skin.bmp        //
//													-gold.bmp        //
//													-echo.bmp        //
//													-beton.bmp       //
//													-visage.bmp      //
//                                                  -EnnemyFace.bmp  //
//																	 //
//											     -gun_triple.wav     //
//                                               -airstrike_mod.wav  //
//                                               -hurt_mod.wav       //
//-------------------------------------------------------------------//
//		   /!\ BE SURE TO HAVE EVERYTHING BEFORE COMPILING /!\       //
//-------------------------------------------------------------------//


// initial animation time
double initial_time = 0.f;

bool DODGE = 0;
bool FAIL = 0;
bool BULLET1,BULLET2,BULLET3 = 0;


//Angle of the Camera (for camera rotation)
double CamangleX = 0;
double CamangleZ = 0;

//Animation Speed
double Aspeed = 3.0f;

enum part { LEFT = 0, RIGHT = 1 };

//Neo avatar
Avatar *avatar;

//Decorations
//(Ennemy is considered as a part of the decoration)
Avatar *enemy;
Bullet *b1, *b2, *b3;

BoxeDecoration *roof, *stairs, *building1;
CylinderDecoration *pipe, *gun[2], *town;
Spheres s;

// perspective vs ortho projection
bool perspective = true;

// shader variable pointers
GLint uniform_model;
GLint uniform_inverseModel;
GLint uniform_view;
GLint uniform_proj;
GLint uniform_texture;
GLint uniform_light;

// eye & light
float eye[3] = { 20.0f , 20.0f , 20.0f };
float light[3] = { -200.0f , 600.0f , 500.0f };

int screen_width = 1024;
int screen_height = 768;

GLuint textureID;	//Town
GLuint textureID2;	//Building
GLuint textureID3;	//Bricks
GLuint textureID4;	//Jean
GLuint textureID5;	//Leather
GLuint textureID6;	//Skin
GLuint textureID7;	//Gold
GLuint textureID8;	//Echoes
GLuint textureID9;	//Concrete
GLuint textureID10;	//Face
GLuint textureID11; //Ennemy Face

struct ShaderProgramSources
{
	std::string VertexSource;
	std::string FragmentSource;
};

/**
 * Function that parse the shader source file into a string in our programm
 * @param filepath: the shader text file path
 * @return The shader source code into a string
*/
static ShaderProgramSources ParseShader(const std::string& filepath)
{

	std::ifstream stream(filepath);

	enum class ShaderType {
		NONE = -1, VERTEX = 0, FRAGMENT = 1
	};

	std::string line;
	std::stringstream ss[2];
	ShaderType type = ShaderType::NONE;
	while (getline(stream, line))
	{
		if (line.find("#shader") != std::string::npos)
		{
			if (line.find("vertex") != std::string::npos)
				type = ShaderType::VERTEX;
			else if (line.find("fragment") != std::string::npos)
				type = ShaderType::FRAGMENT;
		}
		else
		{
			ss[(int)type] << line << '\n';
		}
	}

	return{ ss[0].str(), ss[1].str() };
}

/**
* Function that compile the shader string previously parsed
* @param type: an unsigned int, the type of shader (vertex shader or fragment shader)
* @param source: a string that contain the source code of our shader
* @return id: an unsigned int, The newly created shader's id
*/
static unsigned int CompileShader(unsigned int type, const std::string& source)
{
	unsigned int id = glCreateShader(type);
	//pointer to the beginning of our data, source must exist
	const char* src = source.c_str();
	//(id of source code, number of shaders(strings), adress of source code, length assumes that each 
	//source code string is null terminated, can also get an array of length values)
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE) {
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* message = (char*)alloca(length * sizeof(char));
		glGetShaderInfoLog(id, length, &length, message);
		std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader!" << std::endl;
		std::cout << message << std::endl;
		glDeleteShader(id);
		return 0;
	}

	return id;
}

/**
* Function that create the vertex shader and the fragment shader and bind them to our programm
* @param vertexShader: a string that contain the source code of our vertex shader
* @param fragmentShader: a string that contain the source code of our fragment shader
* @return programm: an unsigned int, The newly created programm with our current shaders
*/
static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
	unsigned int program = glCreateProgram();
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

	//attach both shaders to our program
	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glValidateProgram(program);

	//delete intermediate shaders as they are now added to the program
	glDeleteShader(vs);
	glDeleteShader(fs);

	// shader parameeter bindings
	uniform_model = glGetUniformLocation(program, "modelMatrix");
	uniform_inverseModel = glGetUniformLocation(program, "inverseModelMatrix");
	uniform_view = glGetUniformLocation(program, "viewMatrix");
	uniform_proj = glGetUniformLocation(program, "projectionMatrix");
	uniform_texture = glGetUniformLocation(program, "mire");
	uniform_light = glGetUniformLocation(program, "light");

	if (uniform_model == -1)
		fprintf(stderr, "Could not bind uniform uniform_model\n");

	if (uniform_inverseModel == -1)
		fprintf(stderr, "Could not bind uniform uniform_inverseModel\n");

	if (uniform_view == -1)
		fprintf(stderr, "Could not bind uniform uniform_view\n");

	if (uniform_proj == -1)
		fprintf(stderr, "Could not bind uniform uniform_proj\n");

	if (uniform_texture == -1)
		fprintf(stderr, "Could not bind uniform uniform_texture\n");

	if (uniform_light == -1)
		fprintf(stderr, "Could not bind uniform uniform_light\n");

	return program;
}


/**
* Initial Setup Function: Initialise what must be...
* @param : NONE
* @return : NONE
*/
void setup(void) {
	initial_time = glfwGetTime();
	avatar = new Avatar();

	//Bullets
	b1 = new Bullet(-avatar->_torso->_topRadius/2,30,3*avatar->_torso->_height/4);
	b2 = new Bullet(avatar->_torso->_topRadius / 2, 40, 3 * avatar->_torso->_height / 4);
	b3 = new Bullet(-avatar->_torso->_topRadius / 2, 50, 0);

	//Decorations
	enemy = new Avatar();
	enemy->_Yposition = 65;

	gun[0] = new CylinderDecoration(0.5, 0.5, 2.5);
	gun[1] = new CylinderDecoration(0.5, 0.5, 2.5);

	town = new CylinderDecoration(250, 250, 300);

	roof = new BoxeDecoration(75, 150, 50);
	stairs = new BoxeDecoration(30, 20, 35);
	building1 = new BoxeDecoration(75, 50, 150);
	pipe = new CylinderDecoration(2, 2, 45);


	s.buildSphere(1.5, 20, 20);

	
}

/**
* Update Function: Update what must be...
* @param : NONE
* @return : NONE
*/
void update(void) {
	double current_time = glfwGetTime() - initial_time;
	if (DODGE) avatar->updateDodge(current_time);
	else avatar->update(BULLET1,BULLET2,BULLET3);
}


int main(void)
{

	/* initialize random seed: */
	srand(time(NULL));

	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(screen_width, screen_height, "[CG Et3 Polytech] OpenGL Project: Matrix (re)Animation", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	// Set key callback function
	//glfwSetErrorCallback(error_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwSetCharCallback(window, char_callback);

	//initialize glew AFTER bind to context
	if (glewInit() != GLEW_OK)
		std::cout << "Error!" << std::endl;

	std::cout << glGetString(GL_VERSION) << std::endl;

	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"

	//this makes sure specular highlights do not use the texture color
	glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR);

	setup();

	ShaderProgramSources source = ParseShader("avatar.shader");
	unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);

	PlaySound(TEXT("gun_triple.wav"), NULL, SND_ASYNC | SND_FILENAME | SND_NOSTOP | SND_NODEFAULT);

	textureID = loadBMP_custom("town.bmp");			//Town texture
	textureID2 = loadBMP_custom("building.bmp");	//Building Texture
	textureID3 = loadBMP_custom("bricks.bmp");		//Bricks Texture
	textureID4 = loadBMP_custom("jean_mod.bmp");	//Jean Texture
	textureID5 = loadBMP_custom("leather.bmp");		//Leather Texture
	textureID6 = loadBMP_custom("skin.bmp");		//Skin Texture
	textureID7 = loadBMP_custom("gold.bmp");		//Gold Texture
	textureID8 = loadBMP_custom("echo.bmp");		//Echo Texture
	textureID9 = loadBMP_custom("beton.bmp");		//Concrete Texture
	textureID10 = loadBMP_custom("visage.bmp");		//Visage Texture
	textureID11 = loadBMP_custom("ennemyFace.bmp");	//Ennemy Face Texture

	//Makes sure specular highlights do not use the texture color
	glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR);

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{

		update();

		//Camera rotation
		CamangleZ -= 0.0005;

		if (DODGE && !FAIL && (
			(b1->_Yposition<-10 && b1->_Yposition>-15) ||
			(b2->_Yposition<-10 && b2->_Yposition>-15) ||
			(b3->_Yposition<-10 && b3->_Yposition>-15))) {

			PlaySound(TEXT("airstrike_mod.wav"), NULL, SND_ASYNC | SND_FILENAME | SND_NOSTOP | SND_NODEFAULT);

		}

		//Change bullets positions conditions
		if (!DODGE && (b1->_Yposition >=0.5) && (b1->_Yposition <= avatar->_torso->_topRadius / 1.5f)) {

			FAIL = 1;
			BULLET1 = 1;
			b1->_Yposition = 1000;
			PlaySound(TEXT("hurt_mod.wav"), NULL, SND_ASYNC | SND_FILENAME | SND_NOSTOP | SND_NODEFAULT);
		}
		else if (!DODGE && (b2->_Yposition >= 0) && (b2->_Yposition <= avatar->_torso->_topRadius / 2.5f)){
			BULLET2 = 1;
			b2->_Yposition = 1000;
			PlaySound(TEXT("hurt_mod.wav"), NULL, SND_ASYNC | SND_FILENAME | SND_NOSTOP | SND_NODEFAULT);
		}
		else if (!DODGE && (b3->_Yposition >= 0) && (b3->_Yposition <= avatar->_torso->_topRadius / 2.25f)) {
			BULLET3 = 1;
			b3->_Yposition = 1000;
			PlaySound(TEXT("hurt_mod.wav"), NULL, SND_ASYNC | SND_FILENAME | SND_NOSTOP | SND_NODEFAULT);
		}

		// projection matrix
		glm::mat4 projectionMatrix; // Store the projection matrix
		glm::mat4 viewMatrix; // Store the view matrix

		//One ModelMatrix per primitive of our Model

		//glm::mat4 hatModelMatrix;			//Model Matrix of the Hat
		glm::mat4 headModelMatrix;			//Head model matrix
		glm::mat4 neckModelMatrix;			//Neck model Matrix
		glm::mat4 torsoModelMatrix;			//Torso model matrix

		glm::mat4 leftShoulderModelMatrix;
		glm::mat4 rightShoulderModelMatrix;

		glm::mat4 leftArmModelMatrix;
		glm::mat4 rightArmModelMatrix;

		glm::mat4 leftElbowModelMatrix;
		glm::mat4 rightElbowModelMatrix;

		glm::mat4 leftForearmModelMatrix;
		glm::mat4 rightForearmModelMatrix;

		glm::mat4 leftHandModelMatrix;
		glm::mat4 rightHandModelMatrix;

		glm::mat4 leftHipModelMatrix;
		glm::mat4 rightHipModelMatrix;

		glm::mat4 leftThighModelMatrix;
		glm::mat4 rightThighModelMatrix;

		glm::mat4 leftKneeModelMatrix;
		glm::mat4 rightKneeModelMatrix;

		glm::mat4 leftLegModelMatrix;
		glm::mat4 rightLegModelMatrix;

		glm::mat4 leftFootModelMatrix;
		glm::mat4 rightFootModelMatrix;


		glm::mat4 bullet1ModelMatrix;
		glm::mat4 bullet2ModelMatrix;
		glm::mat4 bullet3ModelMatrix;

		glm::mat4 roofModelMatrix;
		glm::mat4 stairsModelMatrix;
		glm::mat4 building1ModelMatrix;
		
		glm::mat4 modelMatrix1;
		//modelMatrix2; // working model matrices

		// Projection: Perspective or Ortho matrix
		if (perspective) {
			projectionMatrix
				= glm::perspective(45.0f, (float)screen_width / (float)screen_height, 1.0f, 400.0f);
		}
		else {
			projectionMatrix
				= glm::ortho(-2.0f*(float)screen_width / (float)screen_height,
					2.0f*(float)screen_width / (float)screen_height,
					-2.0f, 2.0f, -10.0f, 10.0f);
		}

		// Camera matrix
		viewMatrix
			= glm::lookAt(
				glm::vec3(-1.5, -eye[1] * 1.5, eye[2]/1.5),   // defined in eye function, currently -1.5,-3,2
				glm::vec3(0, 0, 0), // and looks at the origin
				glm::vec3(0, 0, 1)  // Head is up (set to 0,1,0)
				);


		//viewMatrix
		//	= glm::lookAt(
		//		glm::vec3(-eye[0]*1.5, eye[1]*1.5, eye[2]*1.5),   // defined in eye function, currently -3,3,3
		//		glm::vec3(0, 0, 0), // and looks at the origin
		//		glm::vec3(0, 0, 1)  // Head is up (set to 0,1,0)
		//		);

		// wipe the drawing surface clear
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		/* Render here */
		//glClear(GL_COLOR_BUFFER_BIT);

		//Camangle += 0.01;
		viewMatrix = glm::rotate(viewMatrix, (float)CamangleZ, glm::vec3(0.0f, 0.0f, 1.0f));
		viewMatrix = glm::rotate(viewMatrix, (float)CamangleX, glm::vec3(1.0f, 0.0f, 0.0f));

		//viewMatrix = glm::rotate(glm::mat4(1), angle, glm::vec3(0, 0, 1));

		glUseProgram(shader);
		glUniformMatrix4fv(uniform_proj, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
		glUniformMatrix4fv(uniform_view, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		glUniform1i(uniform_texture, 0); //Texture unit 0 is for base images.
		glUniform3fv(uniform_light, 1, light);

		glUniformMatrix4fv(uniform_inverseModel, 1, GL_FALSE, glm::value_ptr(glm::inverse(glm::mat4(1.0f))));

/*********************************DECORATIONS**************************************/

//////////////TOWN//////////////

		modelMatrix1 = glm::translate(glm::mat4(1.0f),
			glm::vec3(0, 0, -150));

		modelMatrix1 = glm::rotate(modelMatrix1, 3.14f, glm::vec3(0.0f, 0.0f, 1.0f));

		// model matrix of the component
		glUniformMatrix4fv(uniform_model, 1, GL_FALSE,
			glm::value_ptr(modelMatrix1));

		glEnable(GL_TEXTURE_2D);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glBindTexture(GL_TEXTURE_2D, textureID);

		town->Deco->draw();

		glDisable(GL_TEXTURE_2D);

//////////////HEAD//////////////

// walk translation of the enemy
		headModelMatrix = glm::translate(glm::mat4(1.0f),
			glm::vec3(0, enemy->_Yposition, enemy->_torso->_height + enemy->_neck->_height + enemy->_head->_radius));
		headModelMatrix = glm::rotate(headModelMatrix,
			(float)(-M_PI/2), glm::vec3(0.0f, 0.0f, 1.0f));
		// model matrix of the component
		glUniformMatrix4fv(uniform_model, 1, GL_FALSE,
			glm::value_ptr(headModelMatrix));

		glEnable(GL_TEXTURE_2D);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glBindTexture(GL_TEXTURE_2D, textureID11);

		enemy->_head->draw();

		glDisable(GL_TEXTURE_2D);

		//s.draw();

		//////////////NECK//////////////

		// walk translation of the enemy
		neckModelMatrix = glm::translate(glm::mat4(1.0f),
			glm::vec3(0, enemy->_Yposition, enemy->_torso->_height));
		// model matrix of the component
		glUniformMatrix4fv(uniform_model, 1, GL_FALSE,
			glm::value_ptr(neckModelMatrix));
		
		glEnable(GL_TEXTURE_2D);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glBindTexture(GL_TEXTURE_2D, textureID6);

		enemy->_neck->draw();

		glDisable(GL_TEXTURE_2D);


		//////////////TORSO//////////////

		// walk translation of the enemy
		torsoModelMatrix = glm::translate(glm::mat4(1.0f),
			glm::vec3(0, enemy->_Yposition, 0));
		torsoModelMatrix = glm::rotate(torsoModelMatrix, 3.14f, glm::vec3(0.0f, 0.0f, 1.0f));

		// model matrix of the component
		glUniformMatrix4fv(uniform_model, 1, GL_FALSE,
			glm::value_ptr(torsoModelMatrix));

		glEnable(GL_TEXTURE_2D);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glBindTexture(GL_TEXTURE_2D, textureID4);

		enemy->_torso->draw();

		glDisable(GL_TEXTURE_2D);
		



		//////////////SHOULDERS//////////////

		leftShoulderModelMatrix = glm::translate(glm::mat4(1.0f),
			glm::vec3(0, enemy->_Yposition, enemy->_torso->_height));

		leftShoulderModelMatrix = glm::rotate(leftShoulderModelMatrix, 3.14f / 2, glm::vec3(1.0f, 0.0f, 0.0f));
		leftShoulderModelMatrix = glm::rotate(leftShoulderModelMatrix, -3.14f / 2, glm::vec3(0.0f, 1.0f, 0.0f));
		leftShoulderModelMatrix = glm::rotate(leftShoulderModelMatrix, 3.14f, glm::vec3(0.0f, 0.0f, 1.0f));

		// model matrix of the component
		glUniformMatrix4fv(uniform_model, 1, GL_FALSE,
			glm::value_ptr(leftShoulderModelMatrix));

		glEnable(GL_TEXTURE_2D);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glBindTexture(GL_TEXTURE_2D, textureID4);

		enemy->_shoulder[LEFT]->draw();

		glDisable(GL_TEXTURE_2D);
		
		


		rightShoulderModelMatrix = glm::translate(glm::mat4(1.0f),
			glm::vec3(0, enemy->_Yposition, enemy->_torso->_height));

		rightShoulderModelMatrix = glm::rotate(rightShoulderModelMatrix, 3.14f / 2, glm::vec3(1.0f, 0.0f, 0.0f));
		rightShoulderModelMatrix = glm::rotate(rightShoulderModelMatrix, 3.14f / 2, glm::vec3(0.0f, 1.0f, 0.0f));
		rightShoulderModelMatrix = glm::rotate(rightShoulderModelMatrix, 3.14f, glm::vec3(0.0f, 0.0f, 1.0f));

		// model matrix of the component
		glUniformMatrix4fv(uniform_model, 1, GL_FALSE,
			glm::value_ptr(rightShoulderModelMatrix));


		glEnable(GL_TEXTURE_2D);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glBindTexture(GL_TEXTURE_2D, textureID4);

		enemy->_shoulder[RIGHT]->draw();

		glDisable(GL_TEXTURE_2D);
		
		

		//////////////RIGHT ARM//////////////


		rightArmModelMatrix = glm::translate(glm::mat4(1.0f),
			glm::vec3(enemy->_arm[RIGHT]->_topRadius + enemy->_torso->_topRadius, enemy->_Yposition, enemy->_torso->_height));
		// model matrix of the component

		rightArmModelMatrix = glm::rotate(rightArmModelMatrix, 3.14f, glm::vec3(1.0f, 0.0f, 0.0f));
		rightArmModelMatrix = glm::rotate(rightArmModelMatrix, 3.14f, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniform_model, 1, GL_FALSE,
			glm::value_ptr(rightArmModelMatrix));

		glEnable(GL_TEXTURE_2D);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glBindTexture(GL_TEXTURE_2D, textureID4);

		enemy->_arm[RIGHT]->draw();

		glDisable(GL_TEXTURE_2D);
		


		rightForearmModelMatrix = glm::translate(glm::mat4(1.0f),
			glm::vec3(enemy->_arm[RIGHT]->_topRadius + enemy->_torso->_topRadius, enemy->_Yposition, enemy->_torso->_height - enemy->_arm[RIGHT]->_height));
		

		rightForearmModelMatrix = glm::rotate(rightForearmModelMatrix, 3.14f, glm::vec3(1.0f, 0.0f, 0.0f));
		rightForearmModelMatrix = glm::rotate(rightForearmModelMatrix, 3.14f, glm::vec3(0.0f, 0.0f, 1.0f));

		// model matrix of the component
		glUniformMatrix4fv(uniform_model, 1, GL_FALSE,
			glm::value_ptr(rightForearmModelMatrix));

		glEnable(GL_TEXTURE_2D);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glBindTexture(GL_TEXTURE_2D, textureID4);

		enemy->_forearm[RIGHT]->draw();

		glDisable(GL_TEXTURE_2D);
		

		rightElbowModelMatrix = glm::translate(glm::mat4(1.0f),
			glm::vec3(enemy->_arm[RIGHT]->_topRadius + enemy->_torso->_topRadius,
				enemy->_Yposition,
				enemy->_torso->_height - enemy->_arm[RIGHT]->_height));

		glUniformMatrix4fv(uniform_model, 1, GL_FALSE,
			glm::value_ptr(rightElbowModelMatrix));

		glEnable(GL_TEXTURE_2D);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glBindTexture(GL_TEXTURE_2D, textureID4);

		enemy->_elbow[RIGHT]->draw();

		glDisable(GL_TEXTURE_2D);
		

		rightHandModelMatrix = glm::translate(glm::mat4(1.0f),
			glm::vec3(enemy->_arm[RIGHT]->_topRadius + enemy->_torso->_topRadius,
				enemy->_Yposition,
				enemy->_torso->_height - enemy->_arm[RIGHT]->_height - enemy->_forearm[RIGHT]->_height));

		glUniformMatrix4fv(uniform_model, 1, GL_FALSE,
			glm::value_ptr(rightHandModelMatrix));

		glEnable(GL_TEXTURE_2D);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glBindTexture(GL_TEXTURE_2D, textureID6);

		enemy->_hand[RIGHT]->draw();

		glDisable(GL_TEXTURE_2D);



		//////////////LEFT ARM//////////////

		leftArmModelMatrix = glm::translate(glm::mat4(1.0f),
			glm::vec3(-enemy->_arm[LEFT]->_topRadius - enemy->_torso->_topRadius, enemy->_Yposition, enemy->_torso->_height));
		// model matrix of the component

		leftArmModelMatrix = glm::rotate(leftArmModelMatrix, 3.14f, glm::vec3(1.0f, 0.0f, 0.0f));
		leftArmModelMatrix = glm::rotate(leftArmModelMatrix, 3.14f, glm::vec3(0.0f, 0.0f, 1.0f));

		glUniformMatrix4fv(uniform_model, 1, GL_FALSE,
			glm::value_ptr(leftArmModelMatrix));

		glEnable(GL_TEXTURE_2D);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glBindTexture(GL_TEXTURE_2D, textureID4);

		enemy->_arm[LEFT]->draw();

		glDisable(GL_TEXTURE_2D);
		
		

		leftForearmModelMatrix = glm::translate(glm::mat4(1.0f),
			glm::vec3(-enemy->_arm[LEFT]->_topRadius - enemy->_torso->_topRadius, enemy->_Yposition, enemy->_torso->_height - enemy->_arm[LEFT]->_height));
		
		leftForearmModelMatrix = glm::rotate(leftForearmModelMatrix, 3.14f, glm::vec3(1.0f, 0.0f, 0.0f));
		leftForearmModelMatrix = glm::rotate(leftForearmModelMatrix, -3.14f / 2, glm::vec3(0.0f, 0.0f, 1.0f));
		leftForearmModelMatrix = glm::rotate(leftForearmModelMatrix, 3.14f / 2, glm::vec3(0.0f, -1.0f, 0.0f));
		
		
		// model matrix of the component
		glUniformMatrix4fv(uniform_model, 1, GL_FALSE,
			glm::value_ptr(leftForearmModelMatrix));

		glEnable(GL_TEXTURE_2D);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glBindTexture(GL_TEXTURE_2D, textureID4);

		enemy->_forearm[LEFT]->draw();

		glDisable(GL_TEXTURE_2D);
		

		leftElbowModelMatrix = glm::translate(glm::mat4(1.0f),
			glm::vec3(-enemy->_arm[LEFT]->_topRadius - enemy->_torso->_topRadius,
				enemy->_Yposition,
				enemy->_torso->_height - enemy->_arm[LEFT]->_height));

		glUniformMatrix4fv(uniform_model, 1, GL_FALSE,
			glm::value_ptr(leftElbowModelMatrix));

		glEnable(GL_TEXTURE_2D);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glBindTexture(GL_TEXTURE_2D, textureID4);

		enemy->_elbow[LEFT]->draw();

		glDisable(GL_TEXTURE_2D);
		

		leftHandModelMatrix = glm::translate(glm::mat4(1.0f),
			glm::vec3(-enemy->_arm[LEFT]->_topRadius - enemy->_torso->_topRadius,
				enemy->_Yposition - enemy->_elbow[LEFT]->_radius - enemy->_forearm[LEFT]->_height,
				enemy->_torso->_height - enemy->_arm[LEFT]->_height));

		glUniformMatrix4fv(uniform_model, 1, GL_FALSE,
			glm::value_ptr(leftHandModelMatrix));

		glEnable(GL_TEXTURE_2D);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glBindTexture(GL_TEXTURE_2D, textureID6);

		enemy->_hand[LEFT]->draw();

		glDisable(GL_TEXTURE_2D);
		

		//////////////HIPS//////////////

		leftHipModelMatrix = glm::translate(glm::mat4(1.0f),
			glm::vec3(0, enemy->_Yposition, -enemy->_hip[LEFT]->_topRadius));

		leftHipModelMatrix = glm::rotate(leftHipModelMatrix, 3.14f / 2, glm::vec3(1.0f, 0.0f, 0.0f));
		leftHipModelMatrix = glm::rotate(leftHipModelMatrix, -3.14f / 2, glm::vec3(0.0f, 1.0f, 0.0f));
		leftHipModelMatrix = glm::rotate(leftHipModelMatrix, 3.14f, glm::vec3(0.0f, 0.0f, 1.0f));

		// model matrix of the component
		glUniformMatrix4fv(uniform_model, 1, GL_FALSE,
			glm::value_ptr(leftHipModelMatrix));

		glEnable(GL_TEXTURE_2D);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glBindTexture(GL_TEXTURE_2D, textureID4);

		enemy->_hip[LEFT]->draw();

		glDisable(GL_TEXTURE_2D);
		

		rightHipModelMatrix = glm::translate(glm::mat4(1.0f),
			glm::vec3(0, enemy->_Yposition, -enemy->_hip[RIGHT]->_topRadius));

		rightHipModelMatrix = glm::rotate(rightHipModelMatrix, 3.14f / 2, glm::vec3(1.0f, 0.0f, 0.0f));
		rightHipModelMatrix = glm::rotate(rightHipModelMatrix, 3.14f / 2, glm::vec3(0.0f, 1.0f, 0.0f));
		rightHipModelMatrix = glm::rotate(rightHipModelMatrix, 3.14f, glm::vec3(0.0f, 0.0f, 1.0f));

		// model matrix of the component
		glUniformMatrix4fv(uniform_model, 1, GL_FALSE,
			glm::value_ptr(rightHipModelMatrix));

		glEnable(GL_TEXTURE_2D);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glBindTexture(GL_TEXTURE_2D, textureID4);

		enemy->_hip[RIGHT]->draw();

		glDisable(GL_TEXTURE_2D);
		

		//////////////RIGHT LEG//////////////


		rightThighModelMatrix = glm::translate(glm::mat4(1.0f),
			glm::vec3(enemy->_torso->_baseRadius / 2, enemy->_Yposition, -enemy->_hip[RIGHT]->_topRadius));
		// model matrix of the component

		rightThighModelMatrix = glm::rotate(rightThighModelMatrix, 3.14f, glm::vec3(1.0f, 0.0f, 0.0f));
		rightThighModelMatrix = glm::rotate(rightThighModelMatrix, 3.14f, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniform_model, 1, GL_FALSE,
			glm::value_ptr(rightThighModelMatrix));

		glEnable(GL_TEXTURE_2D);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glBindTexture(GL_TEXTURE_2D, textureID4);

		enemy->_thigh[RIGHT]->draw();

		glDisable(GL_TEXTURE_2D);
		

		rightLegModelMatrix = glm::translate(glm::mat4(1.0f),
			glm::vec3(enemy->_torso->_baseRadius / 2, enemy->_Yposition, -enemy->_thigh[RIGHT]->_height));
		// model matrix of the component
		rightLegModelMatrix = glm::rotate(rightLegModelMatrix, 3.14f, glm::vec3(1.0f, 0.0f, 0.0f));
		rightLegModelMatrix = glm::rotate(rightLegModelMatrix, 3.14f, glm::vec3(0.0f, 0.0f, 1.0f));

		glUniformMatrix4fv(uniform_model, 1, GL_FALSE,
			glm::value_ptr(rightLegModelMatrix));

		glEnable(GL_TEXTURE_2D);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glBindTexture(GL_TEXTURE_2D, textureID4);

		enemy->_leg[RIGHT]->draw();

		glDisable(GL_TEXTURE_2D);
		


		rightKneeModelMatrix = glm::translate(glm::mat4(1.0f),
			glm::vec3(enemy->_torso->_baseRadius / 2,
				enemy->_Yposition,
				-enemy->_thigh[RIGHT]->_height));

		glUniformMatrix4fv(uniform_model, 1, GL_FALSE,
			glm::value_ptr(rightKneeModelMatrix));

		glEnable(GL_TEXTURE_2D);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glBindTexture(GL_TEXTURE_2D, textureID4);


		enemy->_knee[RIGHT]->draw();
		
		glDisable(GL_TEXTURE_2D);
		

		//////////////LEFT LEG//////////////


		leftThighModelMatrix = glm::translate(glm::mat4(1.0f),
			glm::vec3(-enemy->_torso->_baseRadius / 2, enemy->_Yposition, -enemy->_hip[LEFT]->_topRadius));
		

		leftThighModelMatrix = glm::rotate(leftThighModelMatrix, 3.14f, glm::vec3(1.0f, 0.0f, 0.0f));
		leftThighModelMatrix = glm::rotate(leftThighModelMatrix, 3.14f, glm::vec3(0.0f, 0.0f, 1.0f));
		
		// model matrix of the component
		glUniformMatrix4fv(uniform_model, 1, GL_FALSE,
			glm::value_ptr(leftThighModelMatrix));

		glEnable(GL_TEXTURE_2D);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glBindTexture(GL_TEXTURE_2D, textureID4);

		enemy->_thigh[LEFT]->draw();

		glDisable(GL_TEXTURE_2D);
		

		leftLegModelMatrix = glm::translate(glm::mat4(1.0f),
			glm::vec3(-enemy->_torso->_baseRadius / 2, enemy->_Yposition, -enemy->_thigh[LEFT]->_height));
		
		leftLegModelMatrix = glm::rotate(leftLegModelMatrix, 3.14f, glm::vec3(1.0f, 0.0f, 0.0f));
		leftLegModelMatrix = glm::rotate(leftLegModelMatrix, 3.14f, glm::vec3(0.0f, 0.0f, 1.0f));
		// model matrix of the component
		glUniformMatrix4fv(uniform_model, 1, GL_FALSE,
			glm::value_ptr(leftLegModelMatrix));

		glEnable(GL_TEXTURE_2D);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glBindTexture(GL_TEXTURE_2D, textureID4);

		enemy->_leg[LEFT]->draw();
		
		glDisable(GL_TEXTURE_2D);
		

		leftKneeModelMatrix = glm::translate(glm::mat4(1.0f),
			glm::vec3(-enemy->_torso->_baseRadius / 2,
				enemy->_Yposition,
				-enemy->_thigh[LEFT]->_height));

		glUniformMatrix4fv(uniform_model, 1, GL_FALSE,
			glm::value_ptr(leftKneeModelMatrix));

		glEnable(GL_TEXTURE_2D);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glBindTexture(GL_TEXTURE_2D, textureID4);

		enemy->_knee[LEFT]->draw();

		glDisable(GL_TEXTURE_2D);
		

		//////////////FEET//////////////

		///////RIGHTFOOT
		rightFootModelMatrix = glm::translate(glm::mat4(1.0f),
			glm::vec3(enemy->_torso->_baseRadius / 2,
				enemy->_Yposition,
				-enemy->_thigh[RIGHT]->_height - enemy->_leg[RIGHT]->_height));

		rightFootModelMatrix = glm::rotate(rightFootModelMatrix, -3.14f / 2, glm::vec3(1.0f, 0.0f, 0.0f));
		rightFootModelMatrix = glm::rotate(rightFootModelMatrix, 3.14f, glm::vec3(0.0f, 0.0f, 1.0f));
		//rightFootModelMatrix = glm::rotate(rightFootModelMatrix, 3.14f , glm::vec3(0.0f, -1.0f, 0.0f));

		// model matrix of the component
		glUniformMatrix4fv(uniform_model, 1, GL_FALSE,
			glm::value_ptr(rightFootModelMatrix));

		glEnable(GL_TEXTURE_2D);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glBindTexture(GL_TEXTURE_2D, textureID5);

		enemy->_foot[RIGHT]->draw();
		
		glDisable(GL_TEXTURE_2D);
		

		///////LEFTFOOT

		leftFootModelMatrix = glm::translate(glm::mat4(1.0f),
			glm::vec3(-enemy->_torso->_baseRadius / 2,
				enemy->_Yposition,
				-enemy->_thigh[LEFT]->_height - enemy->_leg[LEFT]->_height));

		leftFootModelMatrix = glm::rotate(leftFootModelMatrix, -3.14f / 2, glm::vec3(1.0f, 0.0f, 0.0f));
		leftFootModelMatrix = glm::rotate(leftFootModelMatrix, 3.14f, glm::vec3(0.0f, 0.0f, 1.0f));
		//leftFootModelMatrix = glm::rotate(leftFootModelMatrix, 3.14f / 2, glm::vec3(0.0f, 1.0f, 0.0f));

		// model matrix of the component
		glUniformMatrix4fv(uniform_model, 1, GL_FALSE,
			glm::value_ptr(leftFootModelMatrix));

		glEnable(GL_TEXTURE_2D);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glBindTexture(GL_TEXTURE_2D, textureID5);

		enemy->_foot[LEFT]->draw();

		glDisable(GL_TEXTURE_2D);
		

///END OF enemy DEF 

		leftHandModelMatrix = glm::translate(leftHandModelMatrix, glm::vec3(0.0f, 0.0f, (float)-gun[0]->Deco->_height / 2));

		glUniformMatrix4fv(uniform_model, 1, GL_FALSE,
			glm::value_ptr(leftHandModelMatrix));

		glEnable(GL_TEXTURE_2D);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glBindTexture(GL_TEXTURE_2D, textureID7);

		gun[0]->Deco->draw();

		glDisable(GL_TEXTURE_2D);
		

		leftHandModelMatrix = glm::translate(leftHandModelMatrix,glm::vec3(0.0f, 0.0f, (float)gun[0]->Deco->_height));

		leftHandModelMatrix = glm::rotate(leftHandModelMatrix, 3.14f, glm::vec3(1.0f, 0.0f, 0.0f));
		leftHandModelMatrix = glm::rotate(leftHandModelMatrix, -3.14f / 2, glm::vec3(0.0f, 0.0f, 1.0f));
		leftHandModelMatrix = glm::rotate(leftHandModelMatrix, 3.14f / 2, glm::vec3(0.0f, -1.0f, 0.0f));
		
		glUniformMatrix4fv(uniform_model, 1, GL_FALSE,
			glm::value_ptr(leftHandModelMatrix));

		glEnable(GL_TEXTURE_2D);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glBindTexture(GL_TEXTURE_2D, textureID7);

		gun[1]->Deco->draw();

		glDisable(GL_TEXTURE_2D);
		

		roofModelMatrix = glm::translate(roofModelMatrix,
			glm::vec3(-roof->Deco->_Xlength / 2,
				-roof->Deco->_Ylength / 2,
				-enemy->_thigh[LEFT]->_height-enemy->_knee[LEFT]->_radius-enemy->_leg[LEFT]->_height-roof->Deco->_Zlength));
		//roofModelMatrix = glm::rotate(roofModelMatrix, 3.14f, glm::vec3(0.0f, 1.0f, 0.0f));

		// model matrix of the component
		glUniformMatrix4fv(uniform_model, 1, GL_FALSE,
			glm::value_ptr(roofModelMatrix));

		glEnable(GL_TEXTURE_2D);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glBindTexture(GL_TEXTURE_2D, textureID9);

		roof->Deco->draw();

		glDisable(GL_TEXTURE_2D);
	

		stairsModelMatrix = glm::translate(stairsModelMatrix,
			glm::vec3(roof->Deco->_Xlength/2 - stairs->Deco->_Xlength, -roof->Deco->_Ylength/2 + stairs->Deco->_Ylength, -enemy->_thigh[LEFT]->_height - enemy->_knee[LEFT]->_radius - enemy->_leg[LEFT]->_height));
		// model matrix of the component
		glUniformMatrix4fv(uniform_model, 1, GL_FALSE,
			glm::value_ptr(stairsModelMatrix));
		
		glEnable(GL_TEXTURE_2D);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glBindTexture(GL_TEXTURE_2D, textureID3);

		stairs->Deco->draw();

		glDisable(GL_TEXTURE_2D);

		stairsModelMatrix = glm::translate(stairsModelMatrix,
			glm::vec3(roof->Deco->_Xlength / 2 - stairs->Deco->_Xlength - pipe->Deco->_baseRadius - 7,
				-roof->Deco->_Ylength/2 + stairs->Deco->_Ylength + 60,
				-enemy->_thigh[LEFT]->_height - enemy->_knee[LEFT]->_radius - enemy->_leg[LEFT]->_height));
		// model matrix of the component
		glUniformMatrix4fv(uniform_model, 1, GL_FALSE,
			glm::value_ptr(stairsModelMatrix));
		pipe->Deco->draw();

		building1ModelMatrix = glm::translate(building1ModelMatrix,
			glm::vec3(-roof->Deco->_Xlength/2.1, 120, -50));
		// model matrix of the component
		glUniformMatrix4fv(uniform_model, 1, GL_FALSE,
			glm::value_ptr(building1ModelMatrix));

		glEnable(GL_TEXTURE_2D);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glBindTexture(GL_TEXTURE_2D, textureID2);

		building1->Deco->draw();

		glDisable(GL_TEXTURE_2D);

		


/*********************************BULLET**************************************/

		//First Bullet translation
		if (BULLET1 != 1) {
			//Loop for the Echo Effect
			for (int i = 0; i < 100; i++) {
				if (b1->_Yposition < b1->_Yecho[i]) {
					bullet1ModelMatrix = glm::translate(glm::mat4(1.0f),
						glm::vec3(b1->_Xposition, b1->_Yecho[i], b1->_Zposition));
					bullet1ModelMatrix = glm::rotate(bullet1ModelMatrix, -3.14f / 2, glm::vec3(1.0, 0.0, 0.0));
				
					// model matrix of the component
					glUniformMatrix4fv(uniform_model, 1, GL_FALSE,
						glm::value_ptr(bullet1ModelMatrix));
				
					glEnable(GL_TEXTURE_2D);
					glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
					glBindTexture(GL_TEXTURE_2D, textureID8);

					b1->_Echo->draw();

					glDisable(GL_TEXTURE_2D);
				
				}
			
			}
		

			bullet1ModelMatrix = glm::translate(glm::mat4(1.0f),
				glm::vec3(b1->_Xposition, b1->_Yposition, b1->_Zposition));

			bullet1ModelMatrix = glm::rotate(bullet1ModelMatrix, 3.14f / 2, glm::vec3(1.0, 0.0, 0.0));

		
			b1->update();

			// model matrix of the component
			glUniformMatrix4fv(uniform_model, 1, GL_FALSE,
				glm::value_ptr(bullet1ModelMatrix));

			glEnable(GL_TEXTURE_2D);
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
			glBindTexture(GL_TEXTURE_2D, textureID7);

			b1->_bullet->draw();

			glDisable(GL_TEXTURE_2D);
		}
		
		

		//2nd Bullet translation

		//Loop for the Echo Effect
		for (int i = 0; i < 100; i++) {
			if (b2->_Yposition < b2->_Yecho[i]) {
				bullet2ModelMatrix = glm::translate(glm::mat4(1.0f),
					glm::vec3(b2->_Xposition, b2->_Yecho[i], b2->_Zposition));
				bullet2ModelMatrix = glm::rotate(bullet2ModelMatrix, -3.14f / 2, glm::vec3(1.0, 0.0, 0.0));

				// model matrix of the component
				glUniformMatrix4fv(uniform_model, 1, GL_FALSE,
					glm::value_ptr(bullet2ModelMatrix));

				glEnable(GL_TEXTURE_2D);
				glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
				glBindTexture(GL_TEXTURE_2D, textureID8);

				b2->_Echo->draw();

				glDisable(GL_TEXTURE_2D);
				

			}

		}


		bullet2ModelMatrix = glm::translate(glm::mat4(1.0f),
			glm::vec3(b2->_Xposition, b2->_Yposition, b2->_Zposition));

		bullet2ModelMatrix = glm::rotate(bullet2ModelMatrix, 3.14f / 2, glm::vec3(1.0, 0.0, 0.0));

		b2->update();

		// model matrix of the component
		glUniformMatrix4fv(uniform_model, 1, GL_FALSE,
			glm::value_ptr(bullet2ModelMatrix));

		glEnable(GL_TEXTURE_2D);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glBindTexture(GL_TEXTURE_2D, textureID7);

		b2->_bullet->draw();

		glDisable(GL_TEXTURE_2D);
		

		//2nd Bullet translation

		//Loop for the Echo Effect
		for (int i = 0; i < 100; i++) {
			if (b3->_Yposition < b3->_Yecho[i]) {
				bullet3ModelMatrix = glm::translate(glm::mat4(1.0f),
					glm::vec3(b3->_Xposition, b3->_Yecho[i], b3->_Zposition));
				bullet3ModelMatrix = glm::rotate(bullet3ModelMatrix, -3.14f / 2, glm::vec3(1.0, 0.0, 0.0));

				// model matrix of the component
				glUniformMatrix4fv(uniform_model, 1, GL_FALSE,
					glm::value_ptr(bullet3ModelMatrix));

				glEnable(GL_TEXTURE_2D);
				glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
				glBindTexture(GL_TEXTURE_2D, textureID8);

				b3->_Echo->draw();

				glDisable(GL_TEXTURE_2D);
				
			}

		}


		bullet3ModelMatrix = glm::translate(glm::mat4(1.0f),
			glm::vec3(b3->_Xposition, b3->_Yposition, b3->_Zposition));

		bullet3ModelMatrix = glm::rotate(bullet3ModelMatrix, 3.14f / 2, glm::vec3(1.0, 0.0, 0.0));

		b3->update();

		// model matrix of the component
		glUniformMatrix4fv(uniform_model, 1, GL_FALSE,
			glm::value_ptr(bullet3ModelMatrix));

		glEnable(GL_TEXTURE_2D);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glBindTexture(GL_TEXTURE_2D, textureID7);

		b3->_bullet->draw();

		glDisable(GL_TEXTURE_2D);
		

//////////////////////////////////////////////////////////////////////MODEL DEFINITION//////////////////////////////////////////////////////////////////////

		/*
		//////////////HAT//////////////

		// walk translation of the avatar
		hatModelMatrix = glm::translate(glm::mat4(1.0f),
		glm::vec3(0, avatar->_Yposition, (avatar->_torso->_height+2*avatar->_head->_radius)));

		// model matrix of the component
		glUniformMatrix4fv(uniform_model, 1, GL_FALSE,
		glm::value_ptr(hatModelMatrix));
		avatar->_hat->draw();
		*/

		//////////////HEAD//////////////

		// walk translation of the avatar
		headModelMatrix = glm::translate(glm::mat4(1.0f),
			glm::vec3(0,
				avatar->_Yposition - (avatar->_torso->_height + avatar->_neck->_height / 1.5f +
					avatar->_head->_radius) * glm::sin(avatar->_Tangle + avatar->_LTangle + M_PI)
				+ (avatar->_thigh[LEFT]->_height - avatar->_hip[LEFT]->_topRadius) * glm::sin(avatar->_LTangle),

				(avatar->_torso->_height + avatar->_neck->_height / 1.5f + avatar->_head->_radius) * glm::cos(avatar->_Tangle + avatar->_LTangle + M_PI)
				- avatar->_thigh[LEFT]->_height * (1 + glm::cos(avatar->_LTangle))));

		headModelMatrix = glm::rotate(headModelMatrix, (float)(M_PI/2), glm::vec3(0.0f, 0.0f, 1.0f));
		headModelMatrix = glm::rotate(headModelMatrix,
			(float)(M_PI - avatar->_LTangle - avatar->_Tangle), glm::vec3(0.0f, 1.0f, 0.0f));
		// model matrix of the component
		glUniformMatrix4fv(uniform_model, 1, GL_FALSE,
			glm::value_ptr(headModelMatrix));

		glEnable(GL_TEXTURE_2D);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glBindTexture(GL_TEXTURE_2D, textureID10);

		avatar->_head->draw();

		glDisable(GL_TEXTURE_2D);
		
		//s.draw();

		//////////////NECK//////////////

		// walk translation of the avatar
		neckModelMatrix = glm::translate(glm::mat4(1.0f),
			glm::vec3(0,
				avatar->_Yposition - avatar->_torso->_height * glm::sin(avatar->_Tangle +
				avatar->_LTangle + M_PI) + (avatar->_thigh[LEFT]->_height -
				avatar->_hip[LEFT]->_topRadius) * glm::sin(avatar->_LTangle),

				avatar->_torso->_height * glm::cos(avatar->_Tangle + avatar->_LTangle + M_PI)
				- avatar->_thigh[LEFT]->_height * (1 + glm::cos(avatar->_LTangle))));

		neckModelMatrix = glm::rotate(neckModelMatrix,
			(float)(M_PI + avatar->_LTangle + avatar->_Tangle), glm::vec3(1.0f, 0.0f, 0.0f));
		// model matrix of the component
		glUniformMatrix4fv(uniform_model, 1, GL_FALSE,
			glm::value_ptr(neckModelMatrix));

		glEnable(GL_TEXTURE_2D);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glBindTexture(GL_TEXTURE_2D, textureID6);

		avatar->_neck->draw();

		glDisable(GL_TEXTURE_2D);
		

		//////////////TORSO//////////////

		// walk translation of the avatar
		torsoModelMatrix = glm::translate(glm::mat4(1.0f),
			glm::vec3(0,
				avatar->_Yposition + (avatar->_thigh[LEFT]->_height - avatar->_hip[LEFT]->_topRadius) * glm::sin(avatar->_LTangle),
				-avatar->_thigh[LEFT]->_height * (1 + glm::cos(avatar->_LTangle))));

		torsoModelMatrix = glm::rotate(torsoModelMatrix,
			(float)(avatar->_Tangle + avatar->_LTangle - M_PI),
			glm::vec3(1.0f, 0.0f, 0.0f));
		// model matrix of the component
		glUniformMatrix4fv(uniform_model, 1, GL_FALSE,
			glm::value_ptr(torsoModelMatrix));


		glEnable(GL_TEXTURE_2D);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glBindTexture(GL_TEXTURE_2D, textureID5);

		avatar->_torso->draw();

		glDisable(GL_TEXTURE_2D);

		//////////////SHOULDERS//////////////

		leftShoulderModelMatrix = glm::translate(glm::mat4(1.0f),
			glm::vec3(0,

				avatar->_Yposition - avatar->_torso->_height * glm::sin(avatar->_Tangle + avatar->_LTangle + M_PI)
				+ (avatar->_thigh[LEFT]->_height - avatar->_hip[LEFT]->_topRadius) * glm::sin(avatar->_LTangle),

				avatar->_torso->_height * glm::cos(avatar->_Tangle + avatar->_LTangle + M_PI)
				- avatar->_thigh[LEFT]->_height * (1 + glm::cos(avatar->_LTangle))));

		leftShoulderModelMatrix = glm::rotate(leftShoulderModelMatrix,
			(float)(avatar->_Tangle + avatar->_LTangle - M_PI / 2.f), glm::vec3(1.0f, 0.0f, 0.0f));

		leftShoulderModelMatrix = glm::rotate(leftShoulderModelMatrix,
			(float)-M_PI / 2, glm::vec3(0.0f, 1.0f, 0.0f));

		/*leftShoulderModelMatrix = glm::rotate(leftShoulderModelMatrix,
			(float)(avatar->_LLangle), glm::vec3(0.0f, 1.0f, 0.0f));*/

		// model matrix of the component
		glUniformMatrix4fv(uniform_model, 1, GL_FALSE,
			glm::value_ptr(leftShoulderModelMatrix));


		glEnable(GL_TEXTURE_2D);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glBindTexture(GL_TEXTURE_2D, textureID5);

		avatar->_shoulder[LEFT]->draw();

		glDisable(GL_TEXTURE_2D);
		
		
		rightShoulderModelMatrix = glm::translate(glm::mat4(1.0f),
			glm::vec3(0,

				avatar->_Yposition - avatar->_torso->_height * glm::sin(avatar->_Tangle + avatar->_LTangle + M_PI)
				+ (avatar->_thigh[RIGHT]->_height - avatar->_hip[RIGHT]->_topRadius) * glm::sin(avatar->_LTangle),

				avatar->_torso->_height * glm::cos(avatar->_Tangle + avatar->_LTangle + M_PI)
				- avatar->_thigh[RIGHT]->_height * (1 + glm::cos(avatar->_LTangle))));

		rightShoulderModelMatrix = glm::rotate(rightShoulderModelMatrix,
			(float)(avatar->_LTangle - M_PI / 2 + avatar->_Tangle), glm::vec3(1.0f, 0.0f, 0.0f));

		rightShoulderModelMatrix = glm::rotate(rightShoulderModelMatrix,
			(float)M_PI / 2, glm::vec3(0.0f, 1.0f, 0.0f));

		/*rightShoulderModelMatrix = glm::rotate(rightShoulderModelMatrix,
			(float)(avatar->_LLangle), glm::vec3(0.0f, 1.0f, 0.0f));*/

		// model matrix of the component
		glUniformMatrix4fv(uniform_model, 1, GL_FALSE,
			glm::value_ptr(rightShoulderModelMatrix));

		glEnable(GL_TEXTURE_2D);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glBindTexture(GL_TEXTURE_2D, textureID5);

		avatar->_shoulder[RIGHT]->draw();

		glDisable(GL_TEXTURE_2D);
		

		//////////////LEFT ARM//////////////


		leftArmModelMatrix = glm::translate(glm::mat4(1.0f),
			glm::vec3(avatar->_arm[LEFT]->_topRadius + avatar->_torso->_topRadius,

				avatar->_Yposition - avatar->_torso->_height * glm::sin(avatar->_Tangle + avatar->_LTangle + M_PI)
				+ (avatar->_thigh[LEFT]->_height - avatar->_hip[LEFT]->_topRadius) * glm::sin(avatar->_LTangle),

				avatar->_torso->_height * glm::cos(avatar->_Tangle + avatar->_LTangle + M_PI)
				- avatar->_thigh[LEFT]->_height * (1 + glm::cos(avatar->_LTangle))));
		// model matrix of the component

		leftArmModelMatrix = glm::rotate(leftArmModelMatrix,
			(float)(avatar->_LAangle + avatar->_Tangle + avatar->_LTangle - M_PI),
			glm::vec3(1.0f, 0.0f, 0.0f));

		glUniformMatrix4fv(uniform_model, 1, GL_FALSE,
			glm::value_ptr(leftArmModelMatrix));

		glEnable(GL_TEXTURE_2D);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glBindTexture(GL_TEXTURE_2D, textureID5);

		avatar->_arm[LEFT]->draw();

		glDisable(GL_TEXTURE_2D);



		leftForearmModelMatrix = glm::translate(glm::mat4(1.0f),
			glm::vec3(avatar->_arm[LEFT]->_topRadius + avatar->_torso->_topRadius,

				avatar->_Yposition - glm::sin(avatar->_LAangle + avatar->_Tangle + avatar->_LTangle + M_PI) * avatar->_arm[LEFT]->_height
				- avatar->_torso->_height * glm::sin(avatar->_Tangle + avatar->_LTangle + M_PI)
				+ (avatar->_thigh[LEFT]->_height - avatar->_hip[LEFT]->_topRadius) * glm::sin(avatar->_LTangle),

				avatar->_torso->_height * glm::cos(avatar->_Tangle + avatar->_LTangle + M_PI) +
				glm::cos(avatar->_LAangle + avatar->_Tangle + avatar->_LTangle + M_PI)
				* avatar->_arm[LEFT]->_height - avatar->_thigh[RIGHT]->_height * (1 + glm::cos(avatar->_LTangle))));

		// model matrix of the component
		leftForearmModelMatrix = glm::rotate(leftForearmModelMatrix,
			(float)(avatar->_LAangle + avatar->_LFAangle + avatar->_Tangle + avatar->_LTangle + M_PI),
			glm::vec3(1.0f, 0.0f, 0.0f));

		glUniformMatrix4fv(uniform_model, 1, GL_FALSE,
			glm::value_ptr(leftForearmModelMatrix));

		glEnable(GL_TEXTURE_2D);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glBindTexture(GL_TEXTURE_2D, textureID5);

		avatar->_forearm[LEFT]->draw();

		glDisable(GL_TEXTURE_2D);


		leftElbowModelMatrix = glm::translate(glm::mat4(1.0f),
			glm::vec3(avatar->_arm[LEFT]->_topRadius + avatar->_torso->_topRadius,

				avatar->_Yposition - glm::sin(avatar->_LAangle + avatar->_Tangle + avatar->_LTangle + M_PI) * avatar->_arm[LEFT]->_height
				- avatar->_torso->_height * glm::sin(avatar->_Tangle + avatar->_LTangle + M_PI)
				+ (avatar->_thigh[LEFT]->_height - avatar->_hip[LEFT]->_topRadius) * glm::sin(avatar->_LTangle),

				avatar->_torso->_height * glm::cos(avatar->_Tangle + avatar->_LTangle + M_PI)
				+ glm::cos(avatar->_LAangle + avatar->_Tangle + avatar->_LTangle + M_PI) * avatar->_arm[LEFT]->_height
				- avatar->_thigh[RIGHT]->_height * (1 + glm::cos(avatar->_LTangle))));

		glUniformMatrix4fv(uniform_model, 1, GL_FALSE,
			glm::value_ptr(leftElbowModelMatrix));

		glEnable(GL_TEXTURE_2D);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glBindTexture(GL_TEXTURE_2D, textureID5);

		avatar->_elbow[LEFT]->draw();

		glDisable(GL_TEXTURE_2D);



		leftHandModelMatrix = glm::translate(glm::mat4(1.0f),
			glm::vec3(avatar->_arm[LEFT]->_topRadius + avatar->_torso->_topRadius,

				avatar->_Yposition - glm::sin(avatar->_LAangle + avatar->_Tangle + avatar->_LTangle + M_PI) *
				avatar->_arm[LEFT]->_height - avatar->_torso->_height * glm::sin(avatar->_Tangle + avatar->_LTangle + M_PI)
				- glm::sin(avatar->_LFAangle + avatar->_LAangle + avatar->_Tangle + avatar->_LTangle + M_PI) * avatar->_forearm[LEFT]->_height
				+ (avatar->_thigh[LEFT]->_height - avatar->_hip[LEFT]->_topRadius) * glm::sin(avatar->_LTangle),

				avatar->_torso->_height * glm::cos(avatar->_Tangle + avatar->_LTangle + M_PI) + glm::cos(avatar->_LAangle + avatar->_LTangle + M_PI + avatar->_Tangle)
				* avatar->_arm[LEFT]->_height + glm::cos(avatar->_Tangle + avatar->_LFAangle + avatar->_LAangle + avatar->_LTangle + M_PI) *
				avatar->_forearm[LEFT]->_height
				- avatar->_thigh[RIGHT]->_height * (1 + glm::cos(avatar->_LTangle))));

		glUniformMatrix4fv(uniform_model, 1, GL_FALSE,
			glm::value_ptr(leftHandModelMatrix));

		glEnable(GL_TEXTURE_2D);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glBindTexture(GL_TEXTURE_2D, textureID6);

		avatar->_hand[LEFT]->draw();

		glDisable(GL_TEXTURE_2D);



		//////////////RIGHT ARM//////////////

		rightArmModelMatrix = glm::translate(glm::mat4(1.0f),
			glm::vec3(-avatar->_arm[RIGHT]->_topRadius - avatar->_torso->_topRadius,

				avatar->_Yposition - avatar->_torso->_height * glm::sin(avatar->_Tangle + avatar->_LTangle + M_PI)
				+ (avatar->_thigh[RIGHT]->_height - avatar->_hip[RIGHT]->_topRadius) * glm::sin(avatar->_LTangle),

				avatar->_torso->_height * glm::cos(avatar->_Tangle + avatar->_LTangle + M_PI)
				- avatar->_thigh[RIGHT]->_height * (1 + glm::cos(avatar->_LTangle))));
		// model matrix of the component

		rightArmModelMatrix = glm::rotate(rightArmModelMatrix,
			(float)(avatar->_RAangle + avatar->_Tangle + avatar->_LTangle - M_PI),
			glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniform_model, 1, GL_FALSE,
			glm::value_ptr(rightArmModelMatrix));
		
		glEnable(GL_TEXTURE_2D);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glBindTexture(GL_TEXTURE_2D, textureID5);

		avatar->_arm[RIGHT]->draw();

		glDisable(GL_TEXTURE_2D);



		rightForearmModelMatrix = glm::translate(glm::mat4(1.0f),
			glm::vec3(-avatar->_arm[RIGHT]->_topRadius - avatar->_torso->_topRadius,

				avatar->_Yposition - glm::sin(avatar->_RAangle + avatar->_Tangle + avatar->_LTangle + M_PI) * avatar->_arm[RIGHT]->_height
				- avatar->_torso->_height * glm::sin(avatar->_Tangle + avatar->_LTangle + M_PI)
				+ (avatar->_thigh[RIGHT]->_height - avatar->_hip[RIGHT]->_topRadius) * glm::sin(avatar->_LTangle),

				avatar->_torso->_height * glm::cos(avatar->_Tangle + avatar->_LTangle + M_PI) +
				glm::cos(avatar->_RAangle + avatar->_Tangle + avatar->_LTangle + M_PI) * avatar->_arm[RIGHT]->_height
				- avatar->_thigh[RIGHT]->_height * (1 + glm::cos(avatar->_LTangle))));

		// model matrix of the component
		rightForearmModelMatrix = glm::rotate(rightForearmModelMatrix,
			(float)(avatar->_RAangle + avatar->_RFAangle + avatar->_Tangle + avatar->_LTangle + M_PI),
			glm::vec3(1.0f, 0.0f, 0.0f));

		glUniformMatrix4fv(uniform_model, 1, GL_FALSE,
			glm::value_ptr(rightForearmModelMatrix));

		glEnable(GL_TEXTURE_2D);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glBindTexture(GL_TEXTURE_2D, textureID5);

		avatar->_forearm[RIGHT]->draw();
		
		glDisable(GL_TEXTURE_2D);
		

		rightElbowModelMatrix = glm::translate(glm::mat4(1.0f),
			glm::vec3(-avatar->_arm[RIGHT]->_topRadius - avatar->_torso->_topRadius,

				avatar->_Yposition - glm::sin(avatar->_RAangle + avatar->_Tangle + avatar->_LTangle + M_PI) * avatar->_arm[RIGHT]->_height
				- avatar->_torso->_height * glm::sin(avatar->_Tangle + avatar->_LTangle + M_PI)
				+ (avatar->_thigh[LEFT]->_height - avatar->_hip[LEFT]->_topRadius) * glm::sin(avatar->_LTangle),

				avatar->_torso->_height * glm::cos(avatar->_Tangle + avatar->_LTangle + M_PI)
				+ glm::cos(avatar->_RAangle + avatar->_Tangle + avatar->_LTangle + M_PI) * avatar->_arm[RIGHT]->_height
				- avatar->_thigh[RIGHT]->_height * (1 + glm::cos(avatar->_LTangle))));

		glUniformMatrix4fv(uniform_model, 1, GL_FALSE,
			glm::value_ptr(rightElbowModelMatrix));

		glEnable(GL_TEXTURE_2D);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glBindTexture(GL_TEXTURE_2D, textureID5);

		avatar->_elbow[RIGHT]->draw();

		glDisable(GL_TEXTURE_2D);
		

		rightHandModelMatrix = glm::translate(glm::mat4(1.0f),
			glm::vec3(-avatar->_arm[RIGHT]->_topRadius - avatar->_torso->_topRadius,

				avatar->_Yposition - glm::sin(avatar->_RAangle + avatar->_Tangle + avatar->_LTangle + M_PI) *
				avatar->_arm[RIGHT]->_height - avatar->_torso->_height * glm::sin(avatar->_Tangle + avatar->_LTangle + M_PI)
				- glm::sin(avatar->_RFAangle + avatar->_RAangle + avatar->_Tangle + avatar->_LTangle + M_PI) * avatar->_forearm[RIGHT]->_height
				+ (avatar->_thigh[LEFT]->_height - avatar->_hip[LEFT]->_topRadius) * glm::sin(avatar->_LTangle),

				avatar->_torso->_height * glm::cos(avatar->_Tangle + avatar->_LTangle + M_PI) + glm::cos(avatar->_RAangle + avatar->_LTangle + M_PI + avatar->_Tangle)
				* avatar->_arm[RIGHT]->_height + glm::cos(avatar->_RFAangle + avatar->_RAangle + avatar->_Tangle + avatar->_LTangle + M_PI) *
				avatar->_forearm[RIGHT]->_height
				- avatar->_thigh[RIGHT]->_height * (1 + glm::cos(avatar->_LTangle))));

		glUniformMatrix4fv(uniform_model, 1, GL_FALSE,
			glm::value_ptr(rightHandModelMatrix));

		glEnable(GL_TEXTURE_2D);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glBindTexture(GL_TEXTURE_2D, textureID6);

		avatar->_hand[RIGHT]->draw();

		glDisable(GL_TEXTURE_2D);


		//////////////HIPS//////////////

		leftHipModelMatrix = glm::translate(glm::mat4(1.0f),
			glm::vec3(0,
				avatar->_Yposition + (avatar->_thigh[LEFT]->_height - avatar->_hip[LEFT]->_topRadius) * glm::sin(avatar->_LTangle),
				-avatar->_hip[LEFT]->_topRadius - avatar->_thigh[LEFT]->_height * (1 + glm::cos(avatar->_LTangle))));

		leftHipModelMatrix = glm::rotate(leftHipModelMatrix,
			(float)M_PI / 2.f,
			glm::vec3(1.0f, 0.0f, 0.0f));

		leftHipModelMatrix = glm::rotate(leftHipModelMatrix,
			(float)M_PI / 2.f,
			glm::vec3(0.0f, 1.0f, 0.0f));

		// model matrix of the component
		glUniformMatrix4fv(uniform_model, 1, GL_FALSE,
			glm::value_ptr(leftHipModelMatrix));

		glEnable(GL_TEXTURE_2D);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glBindTexture(GL_TEXTURE_2D, textureID4);

		avatar->_hip[LEFT]->draw();

		glDisable(GL_TEXTURE_2D);
		

		rightHipModelMatrix = glm::translate(glm::mat4(1.0f),
			glm::vec3(0,
				avatar->_Yposition + (avatar->_thigh[RIGHT]->_height - avatar->_hip[RIGHT]->_topRadius)
				* glm::sin(avatar->_RTangle),
				-avatar->_hip[RIGHT]->_topRadius - avatar->_thigh[RIGHT]->_height
				* (1 + glm::cos(avatar->_RTangle))));

		rightHipModelMatrix = glm::rotate(rightHipModelMatrix,
			(float)M_PI / 2.f,
			glm::vec3(1.0f, 0.0f, 0.0f));

		rightHipModelMatrix = glm::rotate(rightHipModelMatrix,
			(float)-M_PI / 2.f,
			glm::vec3(0.0f, 1.0f, 0.0f));

		// model matrix of the component
		glUniformMatrix4fv(uniform_model, 1, GL_FALSE,
			glm::value_ptr(rightHipModelMatrix));

		glEnable(GL_TEXTURE_2D);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glBindTexture(GL_TEXTURE_2D, textureID4);

		avatar->_hip[RIGHT]->draw();

		glDisable(GL_TEXTURE_2D);
		

		//////////////LEFT LEG//////////////


		leftThighModelMatrix = glm::translate(glm::mat4(1.0f),
			glm::vec3(avatar->_torso->_baseRadius / 2,
				avatar->_Yposition,
				-avatar->_thigh[LEFT]->_height));

		leftThighModelMatrix = glm::rotate(leftThighModelMatrix,
			(float)avatar->_LTangle,
			glm::vec3(1.0f, 0.0f, 0.0f));

		leftThighModelMatrix = glm::translate(leftThighModelMatrix,
			glm::vec3(0,
				0,
				avatar->_hip[LEFT]->_topRadius - avatar->_thigh[LEFT]->_height));
		glUniformMatrix4fv(uniform_model, 1, GL_FALSE,
			glm::value_ptr(leftThighModelMatrix));

		glEnable(GL_TEXTURE_2D);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glBindTexture(GL_TEXTURE_2D, textureID4);

		avatar->_thigh[LEFT]->draw();

		glDisable(GL_TEXTURE_2D);
		

		leftLegModelMatrix = glm::translate(glm::mat4(1.0f),
			glm::vec3(avatar->_torso->_baseRadius / 2,
				avatar->_Yposition,
				-avatar->_thigh[LEFT]->_height));
		// model matrix of the component
		leftLegModelMatrix = glm::rotate(leftLegModelMatrix,
			(float)(M_PI),
			glm::vec3(1.0f, 0.0f, 0.0f));

		glUniformMatrix4fv(uniform_model, 1, GL_FALSE,
			glm::value_ptr(leftLegModelMatrix));

		glEnable(GL_TEXTURE_2D);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glBindTexture(GL_TEXTURE_2D, textureID4);

		avatar->_leg[LEFT]->draw();

		glDisable(GL_TEXTURE_2D);
		


		leftKneeModelMatrix = glm::translate(glm::mat4(1.0f),
			glm::vec3(avatar->_torso->_baseRadius / 2,
				avatar->_Yposition,
				-avatar->_thigh[LEFT]->_height - avatar->_knee[LEFT]->_radius));

		glUniformMatrix4fv(uniform_model, 1, GL_FALSE,
			glm::value_ptr(leftKneeModelMatrix));

		glEnable(GL_TEXTURE_2D);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glBindTexture(GL_TEXTURE_2D, textureID4);

		avatar->_knee[LEFT]->draw();

		glDisable(GL_TEXTURE_2D);


		//////////////RIGHT LEG//////////////


		rightThighModelMatrix = glm::translate(glm::mat4(1.0f),
			glm::vec3(-avatar->_torso->_baseRadius / 2,
				avatar->_Yposition,
				-avatar->_thigh[RIGHT]->_height));
		// model matrix of the component

		rightThighModelMatrix = glm::rotate(rightThighModelMatrix,
			(float)avatar->_RTangle,
			glm::vec3(1.0f, 0.0f, 0.0f));

		rightThighModelMatrix = glm::translate(rightThighModelMatrix,
			glm::vec3(0,
				0,
				avatar->_hip[RIGHT]->_topRadius - avatar->_thigh[RIGHT]->_height));
		glUniformMatrix4fv(uniform_model, 1, GL_FALSE,
			glm::value_ptr(rightThighModelMatrix));

		glEnable(GL_TEXTURE_2D);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glBindTexture(GL_TEXTURE_2D, textureID4);

		avatar->_thigh[RIGHT]->draw();

		glDisable(GL_TEXTURE_2D);


		rightLegModelMatrix = glm::translate(glm::mat4(1.0f),
			glm::vec3(-avatar->_torso->_baseRadius / 2,
				avatar->_Yposition,
				-avatar->_thigh[RIGHT]->_height));
		// model matrix of the component
		rightLegModelMatrix = glm::rotate(rightLegModelMatrix, 3.14f, glm::vec3(1.0f, 0.0f, 0.0f));

		glUniformMatrix4fv(uniform_model, 1, GL_FALSE,
			glm::value_ptr(rightLegModelMatrix));

		glEnable(GL_TEXTURE_2D);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glBindTexture(GL_TEXTURE_2D, textureID4);

		avatar->_leg[RIGHT]->draw();

		glDisable(GL_TEXTURE_2D);
		

		rightKneeModelMatrix = glm::translate(glm::mat4(1.0f),
			glm::vec3(-avatar->_torso->_baseRadius / 2,
				avatar->_Yposition,
				-avatar->_thigh[RIGHT]->_height - avatar->_knee[RIGHT]->_radius));

		glUniformMatrix4fv(uniform_model, 1, GL_FALSE,
			glm::value_ptr(rightKneeModelMatrix));

		glEnable(GL_TEXTURE_2D);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glBindTexture(GL_TEXTURE_2D, textureID4);

		avatar->_knee[RIGHT]->draw();

		glDisable(GL_TEXTURE_2D);
		

		//////////////FEET//////////////

		///////RIGHTFOOT
		rightFootModelMatrix = glm::translate(glm::mat4(1.0f),
			glm::vec3(avatar->_torso->_baseRadius / 2,
				avatar->_Yposition,
				-avatar->_thigh[RIGHT]->_height - avatar->_leg[RIGHT]->_height));

		rightFootModelMatrix = glm::rotate(rightFootModelMatrix, -3.14f / 2, glm::vec3(1.0f, 0.0f, 0.0f));
		//rightFootModelMatrix = glm::rotate(rightFootModelMatrix, -3.14f / 2, glm::vec3(0.0f, 1.0f, 0.0f));

		// model matrix of the component
		glUniformMatrix4fv(uniform_model, 1, GL_FALSE,
			glm::value_ptr(rightFootModelMatrix));

		glEnable(GL_TEXTURE_2D);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glBindTexture(GL_TEXTURE_2D, textureID5);

		avatar->_foot[RIGHT]->draw();

		glDisable(GL_TEXTURE_2D);
		

		///////LEFTFOOT

		leftFootModelMatrix = glm::translate(glm::mat4(1.0f),
			glm::vec3(-avatar->_torso->_baseRadius / 2,
				avatar->_Yposition,
				-avatar->_thigh[LEFT]->_height - avatar->_leg[LEFT]->_height));

		leftFootModelMatrix = glm::rotate(leftFootModelMatrix, -3.14f / 2, glm::vec3(1.0f, 0.0f, 0.0f));
		//leftFootModelMatrix = glm::rotate(leftFootModelMatrix, 3.14f / 2, glm::vec3(0.0f, 1.0f, 0.0f));

		// model matrix of the component
		glUniformMatrix4fv(uniform_model, 1, GL_FALSE,
			glm::value_ptr(leftFootModelMatrix));

		glEnable(GL_TEXTURE_2D);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glBindTexture(GL_TEXTURE_2D, textureID5);

		avatar->_foot[LEFT]->draw();

		glDisable(GL_TEXTURE_2D);
		
		
//////////////////////////////////////////////////////////////////////DEFINITION END//////////////////////////////////////////////////////////////////////

		glBindVertexArray(0); // Disable our Vertex Buffer Object

		/* Poll for and process events */
		glfwPollEvents();

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

	}
	//Delete our shader
	glDeleteProgram(shader);

	glfwTerminate();
	return 0;
}

/**
* key_callback Function: get the current call from keybord interaction
* @param window : the OpenGL window
* @param key : the integer OpenGL enum representing the key pressed (or released or anything else...)
* @param scancode : the integer OpenGL enum representing the key scancode
* @param action : the integer OpenGL enum representing the key action (pressed; released, etc.)
* @param mods :
* @return : NONE
*/
void key_callback(GLFWwindow* window, int key, int scancode,
	int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}


/**
* char_callback Function: get the current call from keybord interaction if it is a char
* @param window : the OpenGL window
* @param key : the unsigned integer representing the ASCII key code pressed
* @return : NONE
*/
void char_callback(GLFWwindow* window, unsigned int key)
{
	if (key == 'p' || key == 'P')
		perspective = true;
	if (key == 'o' || key == 'O')
		perspective = false;
	if (key == ' ')
		CamangleZ += 0.05;
	if (key == 'x')
		CamangleX += 0.05;
	if (key == 'e')
		DODGE = !DODGE;
	if (key == '+')
		Aspeed *= 1.5;
	if (key == '-')
		Aspeed /= 1.5;
	if (key == 'a')
		eye[0] -= 0.5;
	if (key == 'd')
		eye[0] += 0.5;
	if (key == 'w')
		eye[1] += 0.5;
	if (key == 's')
		eye[1] -= 0.5;
	if (key == 'n')
		eye[2] += 0.5;
	if (key == 'm')
		eye[2] -= 0.5;
}