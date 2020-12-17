#include <glad/glad.h>
#include<GLFW/glfw3.h>


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "solver.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Transformations.h"


// #include <Windows.h>
#include <string>

class Point {
public:
	float x, y, z;
	Point(float _x, float _y, float _z) :
		x(_x), y(_y), z(_z) {};
};


/*Enumeramiento de Colores*/
enum{WHITE, RED, YELLOW, ORANGE, BLUE, GREEN, BLACK};

/*Vectores de los Colores que existen en el cubo*/
static float colorsFaces[7][3] = {
	{1.f,1.f,1.f},
	{1.f,0.f,0.f},
	{1.f,1.f,0.f},
	{1.f,0.5f,0.f},
	{0.f, 0.f, 1.f},
	{0.f, 1.f, 0.f},
	{0.f,0.f,0.f}
};

/*Clase Cara:
Vendría a ser la hoja de nuestro grafo. Aquí es donde se almacenan los
datos de cada cara que nosotros dibujemos*/
class Face {
public:
	float vertices[4][5];
	int color;
	unsigned int indices[14];
	int VAO, VBO, EBO, texture;
	/*Here we do all the binding to a certain VAO, */

	Face() {

	}

	Face(Point a, Point b, Point c, Point d, int _color, int _VAO, int _VBO, int _EBO) {
		vertices[0][0] = a.x; vertices[0][1] = a.y; vertices[0][2] = a.z;
		vertices[1][0] = b.x; vertices[1][1] = b.y; vertices[1][2] = b.z;
		vertices[2][0] = c.x; vertices[2][1] = c.y; vertices[2][2] = c.z;
		vertices[3][0] = d.x; vertices[3][1] = d.y; vertices[3][2] = d.z;

		vertices[0][3] = 0.f; vertices[0][4] = 0.f;
		vertices[1][3] = 0.f; vertices[1][4] = 0.f;
		vertices[2][3] = 0.f; vertices[2][4] = 0.f;
		vertices[3][3] = 0.f; vertices[3][4] = 0.f;

		color = _color;

		indices[0] = 0; indices[1] = 1; indices[2] = 2;
		indices[3] = 1; indices[4] = 2; indices[5] = 3;

		indices[6] = 0; indices[7] = 1;
		indices[8] = 1; indices[9] = 3;
		indices[10] = 3; indices[11] = 1;
		indices[12] = 2; indices[13] = 0;




		VAO = _VAO;
		VBO = _VBO;
		EBO = _EBO;
		texture = -1;
	}

	void init() {
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
		
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

	}
	void drawFace(unsigned int shaderID) {
		unsigned int colorLoc = glGetUniformLocation(shaderID, "ourColor");
		glUniform3f(colorLoc, colorsFaces[color][0], colorsFaces[color][1], colorsFaces[color][2]);
		if(texture != -1) glBindTexture(GL_TEXTURE_2D, texture);
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glUniform3f(colorLoc, 0.f, 0.f, 0.f);
		glLineWidth(5.f);
		glDrawElements(GL_LINES, 4, GL_UNSIGNED_INT, (void*)24);


		glBindVertexArray(0);
	}

	void transform(sco::Matrix* transform) {
		for (int i = 0; i < 4; i++) {
			sco::Vector4 aux = sco::Vector4(vertices[i][0], vertices[i][1], vertices[i][2]);
			aux = (*transform) * aux;
			vertices[i][0] = aux[0]; vertices[i][1] = aux[1]; vertices[i][2] = aux[2];
		}
		init();
	}

	void setTex(int _texture, float texCoord[4][2]) {
		texture = _texture;
		for (int i = 0; i < 4; i++) {
			vertices[i][3] = texCoord[i][0];
			vertices[i][4] = texCoord[i][1];
		}
	}

};

/*Posiciones de las caras del cubo*/
enum { FRONT, BACK, LEFT, RIGHT, UP, DOWN };

/*Clase Cubo:
Aquí, almacenaremos las diferentes caras que componen un cubo.
Requerirá que se manden los 8 vértices que la componen y a partir
de ahí se definirán las caras que deberá tener junto con sus
índices*/
class Cube {
public:
	Face caras[6];
	unsigned int VBO[6];
	unsigned int VAO[6];
	unsigned int EBO[6];
	glm::mat4 trans, trans2;
	glm::vec3 t;
	glm::vec3 center;
	Cube() {		

		center = glm::vec3(0.f, 0.f, 0.f);

		/*We generate the buffers*/
		glGenBuffers(6, VBO);
		glGenVertexArrays(6, VAO);
		glGenBuffers(6, EBO);

		

		caras[FRONT] = Face(Point(-0.5f, 0.5f, 0.f), Point(0.5f, 0.5f, 0.f), Point(-0.5f, -0.5f, 0.f), Point(0.5f, -0.5f, 0.f), 0, VAO[FRONT], VBO[FRONT], EBO[FRONT]);
		caras[BACK] = Face(Point(-0.5f, 0.5f, -1.f), Point(0.5f, 0.5f, -1.f), Point(-0.5f, -0.5f, -1.f), Point(0.5f, -0.5f, -1.f), 1, VAO[BACK], VBO[BACK], EBO[BACK]);
		caras[LEFT] = Face(Point(-0.5f, 0.5f, 0.f), Point(-0.5f, 0.5f, -1.f), Point(-0.5f, -0.5f, 0.f), Point(-0.5f, -0.5f, -1.f), 2, VAO[LEFT], VBO[LEFT], EBO[LEFT]);
		caras[RIGHT] = Face(Point(0.5f, 0.5f, 0.f), Point(0.5f, 0.5f, -1.f), Point(0.5f, -0.5f, 0.f), Point(0.5f, -0.5f, -1.f), 3, VAO[RIGHT], VBO[RIGHT], EBO[RIGHT]);
		caras[UP] = Face(Point(-0.5f, 0.5f, -1.f), Point(0.5f, 0.5f, -1.f), Point(-0.5f, 0.5f, 0.f), Point(0.5f, 0.5f, 0.f), 4, VAO[UP], VBO[UP], EBO[UP]);
		caras[DOWN] = Face(Point(-0.5f, -0.5f, -1.f), Point(0.5f, -0.5f, -1.f), Point(-0.5f, -0.5f, 0.f), Point(0.5f, -0.5f, 0.f), 5, VAO[DOWN], VBO[DOWN], EBO[DOWN]);

		for (int i = 0; i < 6; i++) {
			caras[i].init();
		}
	}

	Cube(Point vertices[8], int colores[6]) {
		/*We generate the buffers*/
		glGenBuffers(6, VBO);
		glGenVertexArrays(6, VAO);
		glGenBuffers(6, EBO);

		trans = glm::mat4(1.f);
		t = glm::vec3(0.f, 0.f, 0.f);
		trans2 = glm::mat4(1.f);

		/*Center*/
		center = glm::vec3((vertices[7].x + vertices[0].x) / 2.f, (vertices[7].y + vertices[0].y) / 2.f, (vertices[7].z + vertices[0].z) / 2.f);

		caras[FRONT] = Face(vertices[0], vertices[1], vertices[2], vertices[3], colores[0], VAO[FRONT], VBO[FRONT], EBO[FRONT]);
		caras[BACK] = Face(vertices[4], vertices[5], vertices[6], vertices[7], colores[1], VAO[BACK], VBO[BACK], EBO[BACK]);
		caras[LEFT] = Face(vertices[4], vertices[0], vertices[6], vertices[2], colores[2], VAO[LEFT], VBO[LEFT], EBO[LEFT]);
		caras[RIGHT] = Face(vertices[1], vertices[5], vertices[3], vertices[7], colores[3], VAO[RIGHT], VBO[RIGHT], EBO[RIGHT]);
		caras[UP] = Face(vertices[4], vertices[5], vertices[0], vertices[1], colores[4], VAO[UP], VBO[UP], EBO[UP]);
		caras[DOWN] = Face(vertices[6], vertices[7], vertices[2], vertices[3], colores[5], VAO[DOWN], VBO[DOWN], EBO[DOWN]);

		for (int i = 0; i < 6; i++) {
			caras[i].init();
		}
	}
	
	
	void drawCube(int shaderID) {
		trans = glm::mat4(1.f);
		trans = glm::translate(trans, t);
		unsigned int transLoc = glGetUniformLocation(shaderID, "trans");
		unsigned int transLoc2 = glGetUniformLocation(shaderID, "trans2");
		glUniformMatrix4fv(transLoc, 1, GL_FALSE, glm::value_ptr(trans));
		glUniformMatrix4fv(transLoc2, 1, GL_FALSE, glm::value_ptr(trans2));
		for (int i = 0; i < 6; i++) {
			caras[i].drawFace(shaderID);
		}
	}

	void setT(glm::vec3 tr) {
		t = tr;
	}

	void setTrans2(glm::mat4 trans2) {
		this->trans2 = trans2;
	}



	glm::vec3 getCenter() {
		return center;
	}

	void transform(sco::Matrix* transform) {
		for (int i = 0; i < 6; i++) {
			caras[i].transform(transform);
		}
		/*Transformar centro*/
		sco::Vector4 aux = sco::Vector4(center.x, center.y, center.z);
		aux = (*transform) * aux;
		center = glm::vec3(aux[0], aux[1], aux[2]);
	}

	void setTexture(int face, int texture, float texCoord[4][2]) {
		caras[face].setTex(texture, texCoord);
		caras[face].init();
	}

};


/*Clase Rubik Cube:
Esta clase almacenará los 27 cubos (posteriormente eliminaremos
los que no son visibles y posiblemente tambíen las caras que no
son visibles). los almacenará en una matriz de 3*3*3, y construirá
cada uno de los cubos tomando como referencia los 8 vértices
que mandaremos a su constructor. */

/*Enum para sentido de la rotación*/
enum {COUNTERCLOCK, CLOCK};

class RubikCube {
	Cube Rubik[3][3][3];
	int shID;
	GLFWwindow* window;
	bool unite;
public:
	RubikCube() {
		Point vertices[8] = {
			Point(-0.5f, 0.5f, 0.5f),
			Point(0.5f, 0.5f, 0.5f),
			Point(-0.5f, -0.5f, 0.5f),
			Point(0.5f, -0.5f, 0.5f),
			Point(-0.5f, 0.5f, -0.5f),
			Point(0.5f, 0.5f, -0.5f),
			Point(-0.5f, -0.5f, -0.5f),
			Point(0.5f, -0.5f, -0.5f)
		};
		/*Guardar tercias de los puntos*/
		float terciaX = (vertices[0].x - vertices[1].x) / 3.f;
		float third1X = vertices[0].x - terciaX;
		float third2X = third1X - terciaX;

		float terciaY = (vertices[0].y - vertices[2].y) / 3.f;
		float third1Y = vertices[0].y - terciaY;
		float third2Y = third1Y - terciaY;

		float terciaZ = (vertices[0].z - vertices[4].z) / 3.f;
		float third1Z = vertices[0].z - terciaY;
		float third2Z = third1Z - terciaZ;
		/*We will generate each Cube individually*/
		/*Corners*/
		/*Corner[0][0][0]*/
		Point Corner0[8] = {
			Point(vertices[0].x, vertices[0].y, vertices[0].z),
			Point(third1X, vertices[0].y, vertices[0].z),
			Point(vertices[0].x, third1Y, vertices[0].z),
			Point(third1X, third1Y, vertices[0].z),
			Point(vertices[0].x, vertices[0].y, third1Z),
			Point(third1X, vertices[0].y, third1Z),
			Point(vertices[0].x, third1Y, third1Z),
			Point(third1X, third1Y, third1Z)
		};

		int colorCorner0[6] = {WHITE, BLACK, ORANGE, BLACK, BLUE, BLACK};

		Rubik[0][0][0] = Cube(Corner0, colorCorner0);
		/*Corner [0][0][2]*/
		Point Corner1[8] = {
			Point(third2X, vertices[1].y, vertices[1].z),
			vertices[1],
			Point(third2X, third1Y, vertices[0].z),
			Point(vertices[1].x, third1Y, vertices[1].z),
			
			Point(third2X, vertices[1].y, third1Z),
			Point(vertices[1].x, vertices[1].y, third1Z),
			Point(third2X, third1Y, third1Z),
			Point(vertices[1].x, third1Y, third1Z)
			
		};

		int colorCorner1[6] = { WHITE, BLACK, BLACK, RED, BLUE, BLACK };

		Rubik[0][0][2] = Cube(Corner1, colorCorner1);


		/*Corner [2][0][0]*/
		Point Corner2[8] = {
			Point(vertices[2].x, third2Y, vertices[2].z),
			Point(third1X, third2Y, vertices[2].z),
			Point(vertices[2].x, vertices[2].y, vertices[2].z),
			Point(third1X, vertices[2].y, vertices[2].z),

			Point(vertices[2].x, third2Y, third1Z),
			Point(third1X, third2Y, third1Z),
			Point(vertices[2].x, vertices[2].y, third1Z),
			Point(third1X, vertices[2].y, third1Z),

		};

		int colorCorner2[6] = { WHITE, BLACK, ORANGE, BLACK, BLACK, GREEN };

		Rubik[2][0][0] = Cube(Corner2, colorCorner2);


		/*Corner [2][0][2]*/
		Point Corner3[8] = {
			Point(third2X, third2Y, vertices[3].z),
			Point(vertices[3].x, third2Y, vertices[3].z),
			Point(third2X, vertices[3].y, vertices[3].z),
			Point(vertices[3].x, vertices[3].y, vertices[3].z),
			
			Point(third2X, third2Y, third1Z),
			Point(vertices[3].x, third2Y, third1Z),
			Point(third2X, vertices[3].y, third1Z),
			Point(vertices[3].x, vertices[3].y, third1Z),

		};

		int colorCorner3[6] = { WHITE, BLACK, BLACK, RED, BLACK, GREEN };

		Rubik[2][0][2] = Cube(Corner3, colorCorner3);



		/*Corner [0][2][0]*/
		Point Corner4[8] = {
			Point(vertices[4].x, vertices[4].y, third2Z),
			Point(third1X, vertices[4].y, third2Z),
			Point(vertices[4].x, third1Y, third2Z),
			Point(third1X, third1Y, third2Z),

			vertices[4],
			Point(third1X, vertices[4].y, vertices[4].z),
			Point(vertices[4].x, third1Y, vertices[4].z),
			Point(third1X, third1Y, vertices[4].z)
		};

		int colorCorner4[6] = { BLACK, YELLOW, ORANGE, BLACK, BLUE, BLACK };

		Rubik[0][2][0] = Cube(Corner4, colorCorner4);



		/*Corner [0][2][2]*/
		Point Corner5[8] = {
			Point(third2X, vertices[5].y, third2Z),
			Point(vertices[5].x, vertices[5].y, third2Z),
			Point(third2X, third1Y, third2Z),
			Point(vertices[5].x, third1Y, third2Z),

			Point(third2X, vertices[5].y, vertices[5].z),
			Point(vertices[5].x, vertices[5].y, vertices[5].z),
			Point(third2X, third1Y, vertices[5].z),
			Point(vertices[5].x, third1Y, vertices[5].z),
		};

		int colorCorner5[6] = { BLACK, YELLOW, BLACK, RED, BLUE, BLACK };

		Rubik[0][2][2] = Cube(Corner5, colorCorner5);



		/*Corner [2][2][0]*/
		Point Corner6[8] = {
			Point(vertices[6].x, third2Y, third2Z),
			Point(third1X, third2Y, third2Z),
			Point(vertices[6].x, vertices[6].y, third2Z),
			Point(third1X, vertices[6].y, third2Z),

			Point(vertices[6].x, third2Y, vertices[6].z),
			Point(third1X, third2Y, vertices[6].z),
			Point(vertices[6].x, vertices[6].y, vertices[6].z),
			Point(third1X, vertices[6].y, vertices[6].z)
		};

		int colorCorner6[6] = { BLACK, YELLOW, ORANGE, BLACK, BLACK, GREEN };

		Rubik[2][2][0] = Cube(Corner6, colorCorner6);



		/*Corner [2][2][2]*/
		Point Corner7[8] = {
			Point(third2X, third2Y, third2Z),
			Point(vertices[7].x, third2Y, third2Z),
			Point(third2X, vertices[7].y, third2Z),
			Point(vertices[7].x, vertices[7].y, third2Z),

			Point(third2X, third2Y, vertices[7].z),
			Point(vertices[7].x, third2Y, vertices[7].z),
			Point(third2X,  vertices[7].y, vertices[7].z),
			Point(vertices[7].x, vertices[7].y, vertices[7].z)
		};

		int colorCorner7[6] = { BLACK, YELLOW, BLACK, RED, BLACK, GREEN };

		Rubik[2][2][2] = Cube(Corner7, colorCorner7);


		/*Edges*/
		/*Rubik [0][0][1]*/
		Point Edge0[8] = {
			Point(third1X, vertices[0].y, vertices[0].z),
			Point(third2X, vertices[0].y, vertices[0].z),
			Point(third1X, third1Y, vertices[0].z),
			Point(third2X, third1Y, vertices[0].z),

			Point(third1X, vertices[0].y, third1Z),
			Point(third2X, vertices[0].y, third1Z),
			Point(third1X, third1Y, third1Z),
			Point(third2X, third1Y, third1Z),
		};

		int edgeColor0[6] = { WHITE, BLACK, BLACK, BLACK, BLUE, BLACK };

		Rubik[0][0][1] = Cube(Edge0, edgeColor0);


		/*Rubik [1][0][0]*/
		Point Edge1[8] = {
			Point(vertices[0].x, third1Y, vertices[0].z),
			Point(third1X, third1Y, vertices[0].z),
			Point(vertices[0].x, third2Y, vertices[0].z),
			Point(third1X, third2Y, vertices[0].z),

			Point(vertices[0].x, third1Y, third1Z),
			Point(third1X, third1Y, third1Z),
			Point(vertices[0].x, third2Y, third1Z),
			Point(third1X, third2Y, third1Z)
		};

		int edgeColor1[6] = { WHITE, BLACK, ORANGE, BLACK, BLACK, BLACK };

		Rubik[1][0][0] = Cube(Edge1, edgeColor1);


		/*Rubik [1][0][2]*/
		Point Edge2[8] = {
			Point(third2X, third1Y, vertices[1].z),
			Point(vertices[1].x, third1Y, vertices[1].z),
			Point(third2X, third2Y, vertices[1].z),
			Point(vertices[1].x, third2Y, vertices[1].z),

			Point(third2X, third1Y, third1Z),
			Point(vertices[1].x, third1Y, third1Z),
			Point(third2X, third2Y, third1Z),
			Point(vertices[1].x, third2Y, third1Z),
		};

		int edgeColor2[6] = { WHITE, BLACK, BLACK, RED, BLACK, BLACK };

		Rubik[1][0][2] = Cube(Edge2, edgeColor2);

		/*Rubik [2][0][1]*/
		Point Edge3[8] = {
			Point(third1X, third2Y, vertices[3].z),
			Point(third2X, third2Y, vertices[3].z),
			Point(third1X, vertices[3].y, vertices[3].z),
			Point(third2X, vertices[3].y, vertices[3].z),

			Point(third1X, third2Y, third1Z),
			Point(third2X, third2Y, third1Z),
			Point(third1X, vertices[3].y, third1Z),
			Point(third2X, vertices[3].y, third1Z),
		};

		int edgeColor3[6] = { WHITE, BLACK, BLACK, BLACK, BLACK, GREEN };

		Rubik[2][0][1] = Cube(Edge3, edgeColor3);

		/*Rubik [0][1][0]*/
		Point Edge4[8] = {
			Point(vertices[0].x, vertices[0].y, third1Z),
			Point(third1X, vertices[0].y, third1Z),
			Point(vertices[0].x, third1Y, third1Z),
			Point(third1X, third1Y, third1Z),

			Point(vertices[0].x, vertices[0].y, third2Z),
			Point(third1X, vertices[0].y, third2Z),
			Point(vertices[0].x, third1Y, third2Z),
			Point(third1X, third1Y, third2Z)
		};

		int edgeColor4[6] = { BLACK, BLACK, ORANGE, BLACK, BLUE, BLACK };

		Rubik[0][1][0] = Cube(Edge4, edgeColor4);

		/*Rubik [0][1][2]*/
		Point Edge5[8] = {
			Point(third2X, vertices[1].y, third1Z),
			Point(vertices[1].x, vertices[1].y, third1Z),
			Point(third2X, third1Y, third1Z),
			Point(vertices[1].x, third1Y, third1Z),

			Point(third2X, vertices[1].y, third2Z),
			Point(vertices[1].x, vertices[1].y, third2Z),
			Point(third2X, third1Y, third2Z),
			Point(vertices[1].x, third1Y, third2Z)
		};

		int edgeColor5[6] = { BLACK, BLACK, BLACK, RED, BLUE, BLACK };

		Rubik[0][1][2] = Cube(Edge5, edgeColor5);


		/*Corner [2][1][0]*/
		Point edge6[8] = {
			Point(vertices[2].x, third2Y, third1Z),
			Point(third1X, third2Y, third1Z),
			Point(vertices[2].x, vertices[2].y, third1Z),
			Point(third1X, vertices[2].y, third1Z),

			Point(vertices[2].x, third2Y, third2Z),
			Point(third1X, third2Y, third2Z),
			Point(vertices[2].x, vertices[2].y, third2Z),
			Point(third1X, vertices[2].y, third2Z),

		};

		int edgeColor6[6] = { BLACK, BLACK, ORANGE, BLACK, BLACK, GREEN };

		Rubik[2][1][0] = Cube(edge6, edgeColor6);


		/*Corner [2][1][2]*/
		Point edge7[8] = {
			Point(third2X, third2Y, third1Z),
			Point(vertices[3].x, third2Y, third1Z),
			Point(third2X, vertices[3].y, third1Z),
			Point(vertices[3].x, vertices[3].y, third1Z),

			Point(third2X, third2Y, third2Z),
			Point(vertices[3].x, third2Y, third2Z),
			Point(third2X, vertices[3].y, third2Z),
			Point(vertices[3].x, vertices[3].y, third2Z),

		};

		int edgeColor7[6] = { BLACK, BLACK, BLACK, RED, BLACK, GREEN };

		Rubik[2][1][2] = Cube(edge7, edgeColor7);


		/*Rubik [0][2][1]*/
		Point Edge8[8] = {
			Point(third1X, vertices[0].y, third2Z),
			Point(third2X, vertices[0].y, third2Z),
			Point(third1X, third1Y, third2Z),
			Point(third2X, third1Y, third2Z),

			Point(third1X, vertices[0].y, vertices[4].z),
			Point(third2X, vertices[0].y, vertices[4].z),
			Point(third1X, third1Y, vertices[4].z),
			Point(third2X, third1Y, vertices[4].z),
		};

		int edgeColor8[6] = { BLACK, YELLOW, BLACK, BLACK, BLUE, BLACK };

		Rubik[0][2][1] = Cube(Edge8, edgeColor8);


		/*Rubik [1][2][0]*/
		Point Edge9[8] = {
			Point(vertices[0].x, third1Y, third2Z),
			Point(third1X, third1Y, third2Z),
			Point(vertices[0].x, third2Y, third2Z),
			Point(third1X, third2Y, third2Z),

			Point(vertices[0].x, third1Y, vertices[4].z),
			Point(third1X, third1Y, vertices[4].z),
			Point(vertices[0].x, third2Y, vertices[4].z),
			Point(third1X, third2Y, vertices[4].z)
		};

		int edgeColor9[6] = { BLACK, YELLOW, ORANGE, BLACK, BLACK, BLACK };

		Rubik[1][2][0] = Cube(Edge9, edgeColor9);


		/*Rubik [1][2][2]*/
		Point Edge10[8] = {
			Point(third2X, third1Y, third2Z),
			Point(vertices[1].x, third1Y, third2Z),
			Point(third2X, third2Y, third2Z),
			Point(vertices[1].x, third2Y, third2Z),

			Point(third2X, third1Y, vertices[4].z),
			Point(vertices[1].x, third1Y, vertices[4].z),
			Point(third2X, third2Y, vertices[4].z),
			Point(vertices[1].x, third2Y, vertices[4].z),
		};

		int edgeColor10[6] = { BLACK, YELLOW, BLACK, RED, BLACK, BLACK };

		Rubik[1][2][2] = Cube(Edge10, edgeColor10);

		/*Rubik [2][2][1]*/
		Point Edge11[8] = {
			Point(third1X, third2Y, third2Z),
			Point(third2X, third2Y, third2Z),
			Point(third1X, vertices[3].y, third2Z),
			Point(third2X, vertices[3].y, third2Z),

			Point(third1X, third2Y, vertices[4].z),
			Point(third2X, third2Y, vertices[4].z),
			Point(third1X, vertices[3].y, vertices[4].z),
			Point(third2X, vertices[3].y, vertices[4].z),
		};

		int edgeColor11[6] = { BLACK, YELLOW, BLACK, BLACK, BLACK, GREEN };

		Rubik[2][2][1] = Cube(Edge11, edgeColor11);

		/*Centers*/
		/*Rubik [1][0][1]*/
		Point Center0[8] = {
			Point(third1X, third1Y, vertices[0].z),
			Point(third2X, third1Y, vertices[0].z),
			Point(third1X, third2Y, vertices[0].z),
			Point(third2X, third2Y, vertices[0].z),
			
			Point(third1X, third1Y, third1Z),
			Point(third2X, third1Y, third1Z),
			Point(third1X, third2Y, third1Z),
			Point(third2X, third2Y, third1Z),
		};

		int centerColor0[6] = { WHITE, BLACK, BLACK, BLACK, BLACK, BLACK };

		Rubik[1][0][1] = Cube(Center0, centerColor0);

		/*Rubik [0][1][1]*/
		Point Center1[8] = {
			Point(third1X, vertices[0].y, third1Z),
			Point(third2X, vertices[0].y, third1Z),
			Point(third1X, third1Y, third1Z),
			Point(third2X, third1Y, third1Z),

			Point(third1X, vertices[0].y, third2Z),
			Point(third2X, vertices[0].y, third2Z),
			Point(third1X, third1Y, third2Z),
			Point(third2X, third1Y, third2Z),
		};

		int centerColor1[6] = { BLACK, BLACK, BLACK, BLACK, BLUE, BLACK };

		Rubik[0][1][1] = Cube(Center1, centerColor1);


		/*Rubik [1][1][0]*/
		Point Center2[8] = {
			Point(vertices[0].x, third1Y, third1Z),
			Point(third1X, third1Y, third1Z),
			Point(vertices[0].x, third2Y, third1Z),
			Point(third1X, third2Y, third1Z),

			Point(vertices[0].x, third1Y, third2Z),
			Point(third1X, third1Y, third2Z),
			Point(vertices[0].x, third2Y, third2Z),
			Point(third1X, third2Y, third2Z)
		};

		int centerColor2[6] = { BLACK, BLACK, ORANGE, BLACK, BLACK, BLACK };

		Rubik[1][1][0] = Cube(Center2, centerColor2);


		/*Rubik [1][1][2]*/
		Point Center3[8] = {
			Point(third2X, third1Y, third1Z),
			Point(vertices[1].x, third1Y, third1Z),
			Point(third2X, third2Y, third1Z),
			Point(vertices[1].x, third2Y, third1Z),

			Point(third2X, third1Y, third2Z),
			Point(vertices[1].x, third1Y, third2Z),
			Point(third2X, third2Y, third2Z),
			Point(vertices[1].x, third2Y, third2Z),
		};

		int centerColor3[6] = { BLACK, BLACK, BLACK, RED, BLACK, BLACK };

		Rubik[1][1][2] = Cube(Center3, centerColor3);

		/*Rubik [2][1][1]*/
		Point Center4[8] = {
			Point(third1X, third2Y, third1Z),
			Point(third2X, third2Y, third1Z),
			Point(third1X, vertices[3].y, third1Z),
			Point(third2X, vertices[3].y, third1Z),

			Point(third1X, third2Y, third2Z),
			Point(third2X, third2Y, third2Z),
			Point(third1X, vertices[3].y, third2Z),
			Point(third2X, vertices[3].y, third2Z),
		};

		int centerColor4[6] = { BLACK, BLACK, BLACK, BLACK, BLACK, GREEN };

		Rubik[2][1][1] = Cube(Center4, centerColor4);

		/*Rubik [1][2][1]*/
		Point Center5[8] = {
			Point(third1X, third1Y, third2Z),
			Point(third2X, third1Y, third2Z),
			Point(third1X, third2Y, third2Z),
			Point(third2X, third2Y, third2Z),

			Point(third1X, third1Y, vertices[4].z),
			Point(third2X, third1Y, vertices[4].z),
			Point(third1X, third2Y, vertices[4].z),
			Point(third2X, third2Y, vertices[4].z),
		};

		int centerColor5[6] = { BLACK, YELLOW, BLACK, BLACK, BLACK, BLACK };

		Rubik[1][2][1] = Cube(Center5, centerColor5);
		
		/*El centro de todo*/
		Point Center6[8] = {
			Point(third1X, third1Y, third1Z),
			Point(third2X, third1Y, third1Z),
			Point(third1X, third2Y, third1Z),
			Point(third2X, third2Y, third1Z),

			Point(third1X, third1Y, third2Z),
			Point(third2X, third1Y, third2Z),
			Point(third1X, third2Y, third2Z),
			Point(third2X, third2Y, third2Z),
		};

		int centerColor6[6] = { BLACK, BLACK, BLACK, BLACK, BLACK, BLACK };

		Rubik[1][1][1] = Cube(Center6, centerColor6);

		/*Load Textures*/
		
		unsigned int texture[6];
		glGenTextures(6, texture);

		glBindTexture(GL_TEXTURE_2D, texture[0]);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		int width, heigth, nrChannels;
		std::string path = "/home/julio/c++/grafica/proyecto/GLFW_GLAD_GLUT_GLEW_cmake_project/src/rubik3/letters/U.jpg";
		unsigned char* data = stbi_load(path.c_str(), &width, &heigth, &nrChannels, 0);

		if (data) {
			std::cout << "Texture read" << std::endl;
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, heigth, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else {
			std::cout << "Failed to load texture" << std::endl;
		}
		

		stbi_image_free(data);

		for (int j = 0; j < 3; j++) {
			for (int k = 0; k < 3; k++) {
				float texCoord[4][2] = { {0.33f*(float)(j+1), 0.33f*(float)k},
										{0.33f * (float)(j+ 1), 0.33f * (float)(k+1)},
										{0.33f * (float)(j), 0.33f * (float)k},
										{0.33f * (float)(j), 0.33f * (float)(k+1)},
				};
				
				Rubik[0][j][k].setTexture(UP, texture[0], texCoord);
					
			}
		}

		/*Cara Frontal*/
		glBindTexture(GL_TEXTURE_2D, texture[1]);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		path = "/home/julio/c++/grafica/proyecto/GLFW_GLAD_GLUT_GLEW_cmake_project/src/rubik3/letters/C.jpg";
		stbi_set_flip_vertically_on_load(true);
		data = stbi_load(path.c_str(), &width, &heigth, &nrChannels, 0);

		if (data) {
			std::cout << "Texture read" << std::endl;
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, heigth, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else {
			std::cout << "Failed to load texture" << std::endl;
		}


		stbi_image_free(data);


		for (int i = 0; i < 3; i++) {
			for (int k = 0; k < 3; k++) {
				float texCoord[4][2] = { 
										{0.33f * (float)(i), 0.33f * (float)k},
										{0.33f * (float)(i), 0.33f * (float)(k + 1)},
										{0.33f * (float)(i + 1), 0.33f * (float)k},
										{0.33f * (float)(i + 1), 0.33f * (float)(k + 1)},
										
											
				};
				Rubik[i][0][k].setTexture(FRONT, texture[1], texCoord);
			}
		}

		/*Cara Baja*/
		glBindTexture(GL_TEXTURE_2D, texture[2]);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		path = "/home/julio/c++/grafica/proyecto/GLFW_GLAD_GLUT_GLEW_cmake_project/src/rubik3/letters/S.jpg";
		stbi_set_flip_vertically_on_load(true);
		data = stbi_load(path.c_str(), &width, &heigth, &nrChannels, 0);

		if (data) {
			std::cout << "Texture read" << std::endl;
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, heigth, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else {
			std::cout << "Failed to load texture" << std::endl;
		}


		stbi_image_free(data);


		for (int j = 0; j < 3; j++) {
			for (int k = 0; k < 3; k++) {
				float texCoord[4][2] = { {0.33f * (float)(j + 1), 0.33f * (float)k},
										{0.33f * (float)(j + 1), 0.33f * (float)(k + 1)},
										{0.33f * (float)(j), 0.33f * (float)k},
										{0.33f * (float)(j), 0.33f * (float)(k + 1)},
				};
				Rubik[2][j][k].setTexture(DOWN, texture[2], texCoord);
			}
		}


		/*Cara Trasera*/
		glBindTexture(GL_TEXTURE_2D, texture[3]);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		path = "/home/julio/c++/grafica/proyecto/GLFW_GLAD_GLUT_GLEW_cmake_project/src/rubik3/letters/P.jpg";
		stbi_set_flip_vertically_on_load(false);
		data = stbi_load(path.c_str(), &width, &heigth, &nrChannels, 0);

		if (data) {
			std::cout << "Texture read" << std::endl;
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, heigth, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else {
			std::cout << "Failed to load texture" << std::endl;
		}


		stbi_image_free(data);


		for (int i = 0; i < 3; i++) {
			for (int k = 0; k < 3; k++) {
				float texCoord[4][2] = {
										{0.33f * (float)(i), 0.33f * (float)k},
										{0.33f * (float)(i), 0.33f * (float)(k + 1)},
										{0.33f * (float)(i + 1), 0.33f * (float)k},
										{0.33f * (float)(i + 1), 0.33f * (float)(k + 1)},


				};
				Rubik[i][2][k].setTexture(BACK, texture[3], texCoord);
			}
		}
		
		/*Cara izquierda*/
		glBindTexture(GL_TEXTURE_2D, texture[4]);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		path = "/home/julio/c++/grafica/proyecto/GLFW_GLAD_GLUT_GLEW_cmake_project/src/rubik3/letters/C.jpg";
		stbi_set_flip_vertically_on_load(false);
		data = stbi_load(path.c_str(), &width, &heigth, &nrChannels, 0);

		if (data) {
			std::cout << "Texture read" << std::endl;
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, heigth, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else {
			std::cout << "Failed to load texture" << std::endl;
		}


		stbi_image_free(data);


		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				float texCoord[4][2] = {
										{0.33f * (float)(i), 0.33f * (float)(2-j)},
										{0.33f * (float)(i), 0.33f * (float)(3-j)},
										{0.33f * (float)(i + 1), 0.33f * (float)(2-j)},
										{0.33f * (float)(i + 1), 0.33f * (float)(3-j)},


				};
				Rubik[i][j][0].setTexture(LEFT, texture[4], texCoord);
			}
		}

		/*Cara izquierda*/
		glBindTexture(GL_TEXTURE_2D, texture[5]);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		path = "/home/julio/c++/grafica/proyecto/GLFW_GLAD_GLUT_GLEW_cmake_project/src/rubik3/letters/G.jpg";
		stbi_set_flip_vertically_on_load(false);
		data = stbi_load(path.c_str(), &width, &heigth, &nrChannels, 0);

		if (data) {
			std::cout << "Texture read" << std::endl;
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, heigth, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else {
			std::cout << "Failed to load texture" << std::endl;
		}


		stbi_image_free(data);


		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				float texCoord[4][2] = {
										{0.33f * (float)(i), 0.33f * (float)(3 - j)},
										{0.33f * (float)(i), 0.33f * (float)(2 - j)},
										{0.33f * (float)(i + 1), 0.33f * (float)(3 - j)},
										{0.33f * (float)(i + 1), 0.33f * (float)(2 - j)},


				};
				Rubik[i][j][2].setTexture(RIGHT, texture[5], texCoord);
			}
		}


		unite = false;

	}
	void drawCorners(int shaderID) {
		glUseProgram(shaderID);
		Rubik[0][0][0].drawCube(shaderID);
		Rubik[0][0][2].drawCube(shaderID);
		Rubik[0][2][0].drawCube(shaderID);
		Rubik[0][2][2].drawCube(shaderID);
		Rubik[2][0][0].drawCube(shaderID);
		Rubik[2][0][2].drawCube(shaderID);
		Rubik[2][2][0].drawCube(shaderID);
		Rubik[2][2][2].drawCube(shaderID);
	}

	void drawEdges(int shaderID) {
		glUseProgram(shaderID);
		Rubik[0][0][1].drawCube(shaderID);
		Rubik[1][0][0].drawCube(shaderID);
		Rubik[1][0][2].drawCube(shaderID);
		Rubik[2][0][1].drawCube(shaderID);
		Rubik[0][1][0].drawCube(shaderID);
		Rubik[0][1][2].drawCube(shaderID);
		Rubik[2][1][0].drawCube(shaderID);
		Rubik[2][1][2].drawCube(shaderID);
		Rubik[0][2][1].drawCube(shaderID);
		Rubik[1][2][0].drawCube(shaderID);
		Rubik[1][2][2].drawCube(shaderID);
		Rubik[2][2][1].drawCube(shaderID);
	}

	void drawCenters(int shaderID) {
		glUseProgram(shaderID);
		Rubik[1][0][1].drawCube(shaderID);
		Rubik[0][1][1].drawCube(shaderID);
		Rubik[1][1][0].drawCube(shaderID);
		Rubik[1][1][2].drawCube(shaderID);
		Rubik[2][1][1].drawCube(shaderID);
		Rubik[1][2][1].drawCube(shaderID);

		Rubik[1][1][1].drawCube(shaderID);
	}

	void drawRubik(int shaderID, bool mode) {
		glUseProgram(shaderID);
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				for (int k = 0; k < 3; k++) {
					mode ? Rubik[i][j][k].setT(glm::vec3((0.5f) * ((float)k - 1.f), -(0.5f) * ((float)i - 1.f), -(0.5f) * ((float)j - 1.f))):
						   Rubik[i][j][k].setT(glm::vec3(0.f,0.f,0.f));
					Rubik[i][j][k].drawCube(shaderID);
				}
			}
		}
	}

	void setShader(int _shID) { shID = _shID; }

	void setWindow(GLFWwindow* _window) { window = _window; }

	void setUnite(bool _unite) { unite = _unite; }

	void rotateYaxis(int layer, bool clockwise) {
		glm::mat4 trans(1.f);
		glm::vec3 layerCenter = Rubik[layer][1][1].getCenter();
	
		/*trans = glm::translate(trans, -1.f*layerCenter);
		trans = glm::rotate(trans, glm::radians(90.f), glm::vec3(0.f, 1.f, 0.f));
		trans = glm::translate(trans, layerCenter);

		for (int j = 0; j < 3; j++) {
			for (int k = 0; k < 3; k++) {
				Rubik[layer][j][k].setTrans2(trans);
			}
		}*/

		sco::Matrix* tr = new sco::Matrix();
		*tr = clockwise? 
			 sco::TraslationMatrix(layerCenter.x, layerCenter.y, layerCenter.z) * sco::rotateYMatrix(-30.f) * sco::TraslationMatrix(-layerCenter.x, -layerCenter.y, -layerCenter.z)
			:sco::TraslationMatrix(layerCenter.x, layerCenter.y, layerCenter.z) * sco::rotateYMatrix(30.f) * sco::TraslationMatrix(-layerCenter.x, -layerCenter.y, -layerCenter.z);

		for (int n=0; n < 3; n++) {
			for (int j = 0; j < 3; j++) {
				for (int k = 0; k < 3; k++) {
					Rubik[layer][j][k].transform(tr);
				}
			}
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			drawRubik(shID, unite);
			glBindVertexArray(0);
			glfwSwapBuffers(window);
			// Sleep(100);			
		}
		

	


		/*Do the real translation*/
		if (!clockwise) {
			for (int c = 0; c < 2; c++) {
				Cube temp = Rubik[layer][0][c];
				Rubik[layer][0][c] = Rubik[layer][2 - c][0];
				Rubik[layer][2 - c][0] = Rubik[layer][2][2 - c];
				Rubik[layer][2][2 - c] = Rubik[layer][c][2];
				Rubik[layer][c][2] = temp;
				/*Cube temp = Rubik[layer][c][0];
				Rubik[layer][c][0] = Rubik[layer][0][2-c];
				Rubik[layer][0][2-c] = Rubik[layer][2-c][2];
				Rubik[layer][2-c][2] = Rubik[layer][2][c];
				Rubik[layer][2][c] = temp;*/

			}
		}
		else {
			for (int c = 0; c < 2; c++) {
				Cube temp = Rubik[layer][0][c];
				Rubik[layer][0][c] = Rubik[layer][c][2];
				Rubik[layer][c][2] = Rubik[layer][2][2 - c];
				Rubik[layer][2][2 - c] = Rubik[layer][2 - c][0];
				Rubik[layer][2 - c][0] = temp;
				/*Cube temp = Rubik[layer][c][0];
				Rubik[layer][c][0] = Rubik[layer][2][c];
				Rubik[layer][2][c] = Rubik[layer][2-c][2];
				Rubik[layer][2-c][2] = Rubik[layer][0][2-c];
				Rubik[layer][0][2-c] = temp;*/
			}
		}
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		drawRubik(shID, unite);
		glBindVertexArray(0);
		glfwSwapBuffers(window);
		// Sleep(100);
	}


	void rotateZaxis(int layer, bool clockwise) {
		glm::mat4 trans(1.f);
		glm::vec3 layerCenter = Rubik[1][layer][1].getCenter();
		
		/*trans = glm::translate(trans, -1.f * layerCenter);
		trans = glm::rotate(trans, glm::radians(90.f), glm::vec3(0.f, 0.f, 1.f));
		trans = glm::translate(trans, layerCenter);

		for (int i = 0; i < 3; i++) {
			for (int k = 0; k < 3; k++) {
				Rubik[i][layer][k].setTrans2(trans);
			}
		}*/
		sco::Matrix* tr = new sco::Matrix();
		*tr = clockwise?
			sco::TraslationMatrix(layerCenter.x, layerCenter.y, layerCenter.z) * sco::rotateZMatrix(-30.f) * sco::TraslationMatrix(-layerCenter.x, -layerCenter.y, -layerCenter.z)
			:sco::TraslationMatrix(layerCenter.x, layerCenter.y, layerCenter.z) * sco::rotateZMatrix(30.f) * sco::TraslationMatrix(-layerCenter.x, -layerCenter.y, -layerCenter.z);

		for (int n = 0; n < 3; n++) {
			for (int i = 0; i < 3; i++) {
				for (int k = 0; k < 3; k++) {
					Rubik[i][layer][k].transform(tr);
				}
			}
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			drawRubik(shID, unite);
			glBindVertexArray(0);
			glfwSwapBuffers(window);
			// Sleep(100);
		}
		


		if (clockwise) {
			for (int c = 0; c < 2; c++) {
				Cube temp = Rubik[0][layer][c];
				Rubik[0][layer][c] = Rubik[2 - c][layer][0];
				Rubik[2 - c][layer][0] = Rubik[2][layer][2 - c];
				Rubik[2][layer][2 - c] = Rubik[c][layer][2];
				Rubik[c][layer][2] = temp;

			}
		}
		else {
			/*Do the real translation*/
			for (int c = 0; c < 2; c++) {
				Cube temp = Rubik[0][layer][c];
				Rubik[0][layer][c] = Rubik[c][layer][2];
				Rubik[c][layer][2] = Rubik[2][layer][2 - c];
				Rubik[2][layer][2 - c] = Rubik[2 - c][layer][0];
				Rubik[2 - c][layer][0] = temp;
			}
		}

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		drawRubik(shID, unite);
		glBindVertexArray(0);
		glfwSwapBuffers(window);
		// Sleep(100);
		
	}

	void rotateXaxis(int layer, bool clockwise) {
		glm::mat4 trans(1.f);
		glm::vec3 layerCenter = Rubik[1][1][layer].getCenter();	
		
		
		
		sco::Matrix* tr = new sco::Matrix();
		*tr = clockwise ?
			sco::TraslationMatrix(layerCenter.x, layerCenter.y, layerCenter.z) * sco::rotateXMatrix(-30.f) * sco::TraslationMatrix(-layerCenter.x, -layerCenter.y, -layerCenter.z)
			: sco::TraslationMatrix(layerCenter.x, layerCenter.y, layerCenter.z) * sco::rotateXMatrix(30.f) * sco::TraslationMatrix(-layerCenter.x, -layerCenter.y, -layerCenter.z);
		for (int n = 0; n < 3; n++) {
			for (int i = 0; i < 3; i++) {
				for (int j = 0; j < 3; j++) {
					Rubik[i][j][layer].transform(tr);

				}
			}
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			drawRubik(shID, unite);
			glBindVertexArray(0);
			glfwSwapBuffers(window);
			// Sleep(100);
		}
		

		




		/*Do the real translation*/
		if (!clockwise) {
			for (int c = 0; c < 2; c++) {
				Cube temp = Rubik[0][c][layer];
				Rubik[0][c][layer] = Rubik[c][2][layer];
				Rubik[c][2][layer] = Rubik[2][2 - c][layer];
				Rubik[2][2 - c][layer] = Rubik[2 - c][0][layer];
				Rubik[2 - c][0][layer] = temp;
			}
		}
		else {
			for (int c = 0; c < 2; c++) {
				Cube temp = Rubik[0][c][layer];
				Rubik[0][c][layer] = Rubik[2 - c][0][layer];
				Rubik[2 - c][0][layer] = Rubik[2][2 - c][layer];
				Rubik[2][2 - c][layer] = Rubik[c][2][layer];
				Rubik[c][2][layer] = temp;
			}
		}
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		drawRubik(shID, unite);
		glBindVertexArray(0);
		glfwSwapBuffers(window);
		// Sleep(100);

	}
	void F(int n, bool inverse){
		bool orientation = true;
		if (inverse){
			orientation = false;
		}
		for(int i = 0; i < n; i++){
			rotateZaxis(0, orientation);
		}
	}
	void R(int n, bool inverse){
		bool orientation = true;
		if (inverse){
			orientation = false;
		}
		for(int i = 0; i < n; i++){
			rotateXaxis(2, orientation);
		}
	}
	void U(int n, bool inverse){
		bool orientation = true;
		if (inverse){
			orientation = false;
		}
		for(int i = 0; i < n; i++){
			rotateYaxis(0, orientation);
		}
	}
	void B(int n, bool inverse){
		bool orientation = false;
		if (inverse){
			orientation = true;
		}
		for(int i = 0; i < n; i++){
			rotateZaxis(2, orientation);
		}
	}
	void L(int n, bool inverse){
		bool orientation = false;
		if (inverse){
			orientation = true;
		}
		for(int i = 0; i < n; i++){
			rotateXaxis(0, orientation);
		}
	}
	void D(int n, bool inverse){
		bool orientation = false;
		if (inverse){
			orientation = true;
		}
		for(int i = 0; i < n; i++){
			rotateYaxis(2, orientation);
		}
	}
	void colors(){
		// cara top
		Rubik[0][2][0].caras[4].color = 0;
		Rubik[0][1][0].caras[4].color = 0;
		Rubik[0][0][0].caras[4].color = 0;
		Rubik[0][2][1].caras[4].color = 0;
		Rubik[0][1][1].caras[4].color = 0;
		Rubik[0][0][1].caras[4].color = 0;
		Rubik[0][2][2].caras[4].color = 0;
		Rubik[0][1][2].caras[4].color = 0;
		Rubik[0][0][2].caras[4].color = 0;
		// cara down
		Rubik[2][0][0].caras[5].color = 1;
		Rubik[2][1][0].caras[5].color = 1;
		Rubik[2][2][0].caras[5].color = 1;
		Rubik[2][0][1].caras[5].color = 1;
		Rubik[2][1][1].caras[5].color = 1;
		Rubik[2][2][1].caras[5].color = 1;
		Rubik[2][0][2].caras[5].color = 1;
		Rubik[2][1][2].caras[5].color = 1;
		Rubik[2][2][2].caras[5].color = 1;
		// cara right
		Rubik[0][0][2].caras[3].color = 5;
		Rubik[1][0][2].caras[3].color = 5;
		Rubik[2][0][2].caras[3].color = 5;
		Rubik[0][1][2].caras[3].color = 5;
		Rubik[1][1][2].caras[3].color = 5;
		Rubik[2][1][2].caras[3].color = 5;
		Rubik[0][2][2].caras[3].color = 5;
		Rubik[1][2][2].caras[3].color = 5;
		Rubik[2][2][2].caras[3].color = 5;
		// cara front
		Rubik[0][0][0].caras[0].color = 4;
		Rubik[1][0][0].caras[0].color = 4;
		Rubik[2][0][0].caras[0].color = 4;
		Rubik[0][0][1].caras[0].color = 4;
		Rubik[1][0][1].caras[0].color = 4;
		Rubik[2][0][1].caras[0].color = 4;
		Rubik[0][0][2].caras[0].color = 4;
		Rubik[1][0][2].caras[0].color = 4;
		Rubik[2][0][2].caras[0].color = 4;
		// cara left
		Rubik[0][2][0].caras[2].color = 3;
		Rubik[1][2][0].caras[2].color = 3;
		Rubik[2][2][0].caras[2].color = 3;
		Rubik[0][1][0].caras[2].color = 3;
		Rubik[1][1][0].caras[2].color = 3;
		Rubik[2][1][0].caras[2].color = 3;
		Rubik[0][0][0].caras[2].color = 3;
		Rubik[1][0][0].caras[2].color = 3;
		Rubik[2][0][0].caras[2].color = 3;
		// cara back
		Rubik[0][2][2].caras[1].color = 2;
		Rubik[1][2][2].caras[1].color = 2;
		Rubik[2][2][2].caras[1].color = 2;
		Rubik[0][2][1].caras[1].color = 2;
		Rubik[1][2][1].caras[1].color = 2;
		Rubik[2][2][1].caras[1].color = 2;
		Rubik[0][2][0].caras[1].color = 2;
		Rubik[1][2][0].caras[1].color = 2;
		Rubik[2][2][0].caras[1].color = 2;
	}
	std::vector<std::string> solve(std::string seq){
		std::vector<std::string> sol = process(seq);
		return sol;
	}

};
