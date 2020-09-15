#include <Windows.h>
#include <thread>
#include "3DCubeShow.h"

GLfloat edgeAngle = 0;
GLfloat cubeAngle = 45;

volatile bool moving = false;
volatile Side CurrentR = Side::NO;
volatile bool active = true;
volatile State currentState;
volatile MenuAction currentMenuEvent = MenuAction::NO_ACTION;

GLfloat EDGE_SPEED = 3;

struct RotationAny {
	GLfloat angle;
	GLfloat x;
	GLfloat y;
	GLfloat z;
};
struct Point {
	GLfloat x;
	GLfloat y;
	GLfloat z;
};
struct PlainColors {
	Color up = Color::DEFAULT;
	Color down = Color::DEFAULT;
	Color front = Color::DEFAULT;
	Color back = Color::DEFAULT;
	Color left = Color::DEFAULT;
	Color right = Color::DEFAULT;
};

const Point singleCubeVertexes[6][4] =
{
	{Point{ LITTLE_SIZE,  LITTLE_SIZE,  LITTLE_SIZE}, Point{-LITTLE_SIZE,  LITTLE_SIZE,  LITTLE_SIZE}, Point{-LITTLE_SIZE, -LITTLE_SIZE,  LITTLE_SIZE}, Point{ LITTLE_SIZE, -LITTLE_SIZE,  LITTLE_SIZE} }, //up
	{Point{-LITTLE_SIZE, -LITTLE_SIZE, -LITTLE_SIZE}, Point{ LITTLE_SIZE, -LITTLE_SIZE, -LITTLE_SIZE}, Point{ LITTLE_SIZE,  LITTLE_SIZE, -LITTLE_SIZE}, Point{-LITTLE_SIZE,  LITTLE_SIZE, -LITTLE_SIZE} }, //down
	{Point{ LITTLE_SIZE,  LITTLE_SIZE,  LITTLE_SIZE}, Point{-LITTLE_SIZE,  LITTLE_SIZE,  LITTLE_SIZE}, Point{-LITTLE_SIZE,  LITTLE_SIZE, -LITTLE_SIZE}, Point{ LITTLE_SIZE,  LITTLE_SIZE, -LITTLE_SIZE} }, //front
	{Point{-LITTLE_SIZE, -LITTLE_SIZE, -LITTLE_SIZE}, Point{ LITTLE_SIZE, -LITTLE_SIZE, -LITTLE_SIZE}, Point{ LITTLE_SIZE, -LITTLE_SIZE,  LITTLE_SIZE}, Point{-LITTLE_SIZE, -LITTLE_SIZE,  LITTLE_SIZE} }, //back
	{Point{-LITTLE_SIZE, -LITTLE_SIZE, -LITTLE_SIZE}, Point{-LITTLE_SIZE, -LITTLE_SIZE,  LITTLE_SIZE}, Point{-LITTLE_SIZE,  LITTLE_SIZE,  LITTLE_SIZE}, Point{-LITTLE_SIZE,  LITTLE_SIZE, -LITTLE_SIZE} },
	{Point{ LITTLE_SIZE,  LITTLE_SIZE,  LITTLE_SIZE}, Point{ LITTLE_SIZE,  LITTLE_SIZE, -LITTLE_SIZE}, Point{ LITTLE_SIZE, -LITTLE_SIZE, -LITTLE_SIZE}, Point{ LITTLE_SIZE, -LITTLE_SIZE,  LITTLE_SIZE} }
};


GLvoid DrawAxes() {
	glBegin(GL_LINES);
	glColor3f(1, 0, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(10, 0, 0);

	glColor3f(0, 1, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 10, 0);

	glColor3f(0, 0, 1);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, 10);
	glEnd();
}

void rotateFace(volatile Color(&face)[3][3], const bool clockwise)
{
	Color buffer[3][3];
	if (clockwise)
	{
		for (int i = 0; i < 9; i++)
		{
			buffer[i / 3][i % 3] = face[i % 3][2 - i / 3];
		}
	}
	else
	{
		for (int i = 0; i < 9; i++)
		{
			buffer[i % 3][i / 3] = face[2 - i / 3][i % 3];
		}
	}
	for (int i = 0; i < 9; i++)
		face[i / 3][i % 3] = buffer[i / 3][i % 3];
}

void Rotate()
{
	State bufferState;
	switch (CurrentR)
	{
	case Side::FS:
		rotateFace(currentState.frontFaces, true);
		for (int i = 0; i < 3; i++)
		{
			bufferState.upFaces[2][i] = currentState.rightFaces[i][0];
			bufferState.leftFaces[i][2] = currentState.upFaces[2][2 - i];
			bufferState.downFaces[0][i] = currentState.leftFaces[i][2];
			bufferState.rightFaces[i][0] = currentState.downFaces[0][2 - i];
		}
		for (int i = 0; i < 3; i++)
		{
			currentState.upFaces[2][i] = bufferState.upFaces[2][i];
			currentState.downFaces[0][i] = bufferState.downFaces[0][i];
			currentState.leftFaces[i][2] = bufferState.leftFaces[i][2];
			currentState.rightFaces[i][0] = bufferState.rightFaces[i][0];
		}
		break;
	case Side::F:
		rotateFace(currentState.frontFaces, false);
		for (int i = 0; i < 3; i++)
		{
			bufferState.upFaces[2][i] = currentState.leftFaces[2 - i][2];
			bufferState.leftFaces[i][2] = currentState.downFaces[0][i];
			bufferState.downFaces[0][i] = currentState.rightFaces[2 - i][0];
			bufferState.rightFaces[i][0] = currentState.upFaces[2][i];
		}
		for (int i = 0; i < 3; i++)
		{
			currentState.upFaces[2][i] = bufferState.upFaces[2][i];
			currentState.downFaces[0][i] = bufferState.downFaces[0][i];
			currentState.leftFaces[i][2] = bufferState.leftFaces[i][2];
			currentState.rightFaces[i][0] = bufferState.rightFaces[i][0];
		}
		break;
	case Side::B:
		rotateFace(currentState.backFaces, false);
		for (int i = 0; i < 3; i++)
		{
			bufferState.upFaces[0][i] = currentState.rightFaces[i][2];
			bufferState.leftFaces[i][0] = currentState.upFaces[0][2 - i];
			bufferState.downFaces[2][i] = currentState.leftFaces[i][0];
			bufferState.rightFaces[i][2] = currentState.downFaces[2][2 - i];
		}
		for (int i = 0; i < 3; i++)
		{
			currentState.upFaces[0][i] = bufferState.upFaces[0][i];
			currentState.downFaces[2][i] = bufferState.downFaces[2][i];
			currentState.leftFaces[i][0] = bufferState.leftFaces[i][0];
			currentState.rightFaces[i][2] = bufferState.rightFaces[i][2];
		}
		break;
	case Side::BS:
		rotateFace(currentState.backFaces, true);
		for (int i = 0; i < 3; i++)
		{
			bufferState.upFaces[0][i] = currentState.leftFaces[2 - i][0];
			bufferState.leftFaces[i][0] = currentState.downFaces[2][i];
			bufferState.downFaces[2][i] = currentState.rightFaces[2 - i][2];
			bufferState.rightFaces[i][2] = currentState.upFaces[0][i];
		}
		for (int i = 0; i < 3; i++)
		{
			currentState.upFaces[0][i] = bufferState.upFaces[0][i];
			currentState.downFaces[2][i] = bufferState.downFaces[2][i];
			currentState.leftFaces[i][0] = bufferState.leftFaces[i][0];
			currentState.rightFaces[i][2] = bufferState.rightFaces[i][2];
		}
		break;
	case Side::U:
		rotateFace(currentState.upFaces, false);
		for (int i = 0; i < 3; i++)
		{
			bufferState.frontFaces[0][i] = currentState.rightFaces[0][i];
			bufferState.rightFaces[0][i] = currentState.backFaces[0][i];
			bufferState.backFaces[0][i] = currentState.leftFaces[0][i];
			bufferState.leftFaces[0][i] = currentState.frontFaces[0][i];
		}
		for (int i = 0; i < 3; i++)
		{
			currentState.frontFaces[0][i] = bufferState.frontFaces[0][i];
			currentState.rightFaces[0][i] = bufferState.rightFaces[0][i];
			currentState.backFaces[0][i] = bufferState.backFaces[0][i];
			currentState.leftFaces[0][i] = bufferState.leftFaces[0][i];
		}
		break;
	case Side::US:
		rotateFace(currentState.upFaces, true);
		for (int i = 0; i < 3; i++)
		{
			bufferState.frontFaces[0][i] = currentState.leftFaces[0][i];
			bufferState.rightFaces[0][i] = currentState.frontFaces[0][i];
			bufferState.backFaces[0][i] = currentState.rightFaces[0][i];
			bufferState.leftFaces[0][i] = currentState.backFaces[0][i];
		}
		for (int i = 0; i < 3; i++)
		{
			currentState.frontFaces[0][i] = bufferState.frontFaces[0][i];
			currentState.rightFaces[0][i] = bufferState.rightFaces[0][i];
			currentState.backFaces[0][i] = bufferState.backFaces[0][i];
			currentState.leftFaces[0][i] = bufferState.leftFaces[0][i];
		}
		break;
	case Side::D:
		rotateFace(currentState.downFaces, false);
		for (int i = 0; i < 3; i++)
		{
			bufferState.frontFaces[2][i] = currentState.leftFaces[2][i];
			bufferState.rightFaces[2][i] = currentState.frontFaces[2][i];
			bufferState.backFaces[2][i] = currentState.rightFaces[2][i];
			bufferState.leftFaces[2][i] = currentState.backFaces[2][i];
		}
		for (int i = 0; i < 3; i++)
		{
			currentState.frontFaces[2][i] = bufferState.frontFaces[2][i];
			currentState.rightFaces[2][i] = bufferState.rightFaces[2][i];
			currentState.backFaces[2][i] = bufferState.backFaces[2][i];
			currentState.leftFaces[2][i] = bufferState.leftFaces[2][i];
		}
		break;
	case Side::DS:
		rotateFace(currentState.downFaces, true);
		for (int i = 0; i < 3; i++)
		{
			bufferState.frontFaces[2][i] = currentState.rightFaces[2][i];
			bufferState.rightFaces[2][i] = currentState.backFaces[2][i];
			bufferState.backFaces[2][i] = currentState.leftFaces[2][i];
			bufferState.leftFaces[2][i] = currentState.frontFaces[2][i];
		}
		for (int i = 0; i < 3; i++)
		{
			currentState.frontFaces[2][i] = bufferState.frontFaces[2][i];
			currentState.rightFaces[2][i] = bufferState.rightFaces[2][i];
			currentState.backFaces[2][i] = bufferState.backFaces[2][i];
			currentState.leftFaces[2][i] = bufferState.leftFaces[2][i];
		}
		break;
	case Side::RS:
		rotateFace(currentState.rightFaces, true);
		for (int i = 0; i < 3; i++)
		{
			bufferState.upFaces[i][2] = currentState.backFaces[2 - i][0];
			bufferState.backFaces[i][0] = currentState.downFaces[2 - i][2];
			bufferState.downFaces[i][2] = currentState.frontFaces[i][2];
			bufferState.frontFaces[i][2] = currentState.upFaces[i][2];
		}
		for (int i = 0; i < 3; i++)
		{
			currentState.upFaces[i][2] = bufferState.upFaces[i][2];
			currentState.downFaces[i][2] = bufferState.downFaces[i][2];
			currentState.frontFaces[i][2] = bufferState.frontFaces[i][2];
			currentState.backFaces[i][0] = bufferState.backFaces[i][0];
		}
		break;
	case Side::R:
		rotateFace(currentState.rightFaces, false);
		for (int i = 0; i < 3; i++)
		{
			bufferState.upFaces[i][2] = currentState.frontFaces[i][2];
			bufferState.backFaces[i][0] = currentState.upFaces[2 - i][2];
			bufferState.downFaces[i][2] = currentState.backFaces[2 - i][0];
			bufferState.frontFaces[i][2] = currentState.downFaces[i][2];
		}
		for (int i = 0; i < 3; i++)
		{
			currentState.upFaces[i][2] = bufferState.upFaces[i][2];
			currentState.downFaces[i][2] = bufferState.downFaces[i][2];
			currentState.frontFaces[i][2] = bufferState.frontFaces[i][2];
			currentState.backFaces[i][0] = bufferState.backFaces[i][0];
		}
		break;
	case Side::LS:
		rotateFace(currentState.leftFaces, true);
		for (int i = 0; i < 3; i++)
		{
			bufferState.upFaces[i][0] = currentState.frontFaces[i][0];
			bufferState.backFaces[i][2] = currentState.upFaces[2 - i][0];
			bufferState.downFaces[i][0] = currentState.backFaces[2 - i][2];
			bufferState.frontFaces[i][0] = currentState.downFaces[i][0];
		}
		for (int i = 0; i < 3; i++)
		{
			currentState.upFaces[i][0] = bufferState.upFaces[i][0];
			currentState.downFaces[i][0] = bufferState.downFaces[i][0];
			currentState.frontFaces[i][0] = bufferState.frontFaces[i][0];
			currentState.backFaces[i][2] = bufferState.backFaces[i][2];
		}
		break;
	case Side::L:
		rotateFace(currentState.leftFaces, false);
		for (int i = 0; i < 3; i++)
		{
			bufferState.upFaces[i][0] = currentState.backFaces[2 - i][2];
			bufferState.backFaces[i][2] = currentState.downFaces[2 - i][0];
			bufferState.downFaces[i][0] = currentState.frontFaces[i][0];
			bufferState.frontFaces[i][0] = currentState.upFaces[i][0];
		}
		for (int i = 0; i < 3; i++)
		{
			currentState.upFaces[i][0] = bufferState.upFaces[i][0];
			currentState.downFaces[i][0] = bufferState.downFaces[i][0];
			currentState.frontFaces[i][0] = bufferState.frontFaces[i][0];
			currentState.backFaces[i][2] = bufferState.backFaces[i][2];
		}
		break;
	}
}

GLvoid idle()
{
	Sleep(25); 
	glutPostRedisplay(); 
	if (edgeAngle >= 90) 
	{
		Rotate();
		CurrentR = Side::NO;
		edgeAngle = 0;
		moving = false;
	}
}

GLvoid InitGL()
{
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);
	gluPerspective( /* field of view in degree */ 50.0,
		/* aspect ratio */ 1.0,
		/* Z near */ 1.0, /* Z far */ 10.0);
	glMatrixMode(GL_MODELVIEW);
	glEnable(GLUT_MULTISAMPLE);
	glClearColor((GLfloat)195 / 255, (GLfloat)199 / 255, (GLfloat)219 / 255, 1.0f);
}

GLvoid DrawSingleCube(PlainColors atribute, Point positionShift, RotationAny rotor)
{
	glTranslatef(positionShift.x, positionShift.y, positionShift.z);
	glRotatef(rotor.angle, rotor.x, rotor.y, rotor.z);
	glTranslatef(-positionShift.x, -positionShift.y, -positionShift.z);
	GLfloat colorsT[6][3] = { {1, 0, 0},{1.0, 165.0 / 255.0, 0},{0, 1, 0},{0, 0, 1},{1, 1, 0},{1, 1, 1} };
	Color atributes[6] = { atribute.up, atribute.down, atribute.front, atribute.back, atribute.right, atribute.left };
	for (int i = 0; i < 6; i++)
	{
		switch (atributes[i])
		{
		case Color::RED:
			glColor3f((GLfloat)217 / 255, (GLfloat)11 / 255, (GLfloat)11 / 255);
			break;
		case Color::ORANGE:
			glColor3f((GLfloat)245 / 255, (GLfloat)126 / 255, (GLfloat)7 / 255);
			break;
		case Color::BLUE:
			glColor3f((GLfloat)22 / 255, (GLfloat)22 / 255, (GLfloat)217 / 255);
			break;
		case Color::GREEN:
			glColor3f((GLfloat)23 / 255, (GLfloat)230 / 255, (GLfloat)23 / 255);
			break;
		case Color::YELLOW:
			glColor3f((GLfloat)230 / 255, (GLfloat)230 / 255, (GLfloat)11 / 255);
			break;
		case Color::WHITE:
			glColor3f((GLfloat)230 / 255, (GLfloat)230 / 255, (GLfloat)230 / 255);
			break;
		default:
			glColor3f(0, 0, 0);
			break;
		}
		glBegin(GL_POLYGON);
		for (int j = 0; j < 4; j++)
			glVertex3f(singleCubeVertexes[i][j].x, singleCubeVertexes[i][j].y, singleCubeVertexes[i][j].z);
		glEnd();
	}
	glTranslatef(positionShift.x, positionShift.y, positionShift.z);
	glRotatef(-rotor.angle, rotor.x, rotor.y, rotor.z);
	glTranslatef(-positionShift.x, -positionShift.y, -positionShift.z);
};

GLvoid DrawGLScene(GLvoid)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	glTranslatef(2 * LITTLE_SIZE, 0, -5.0f);
	glRotatef(-55, 1.0f, 0.0f, 0.0f);
	glRotatef(180, 0.0f, 0.0f, 1.0f);
	glTranslatef(2 * LITTLE_SIZE * (1 + GAP), 2 * LITTLE_SIZE * (1 + GAP), 2 * LITTLE_SIZE * (1 + GAP));
	glRotatef(cubeAngle, 0.0f, 0.0f, 1.0f);
	// glRotatef(180, 1.0f, 0.0f, 0.0f);
	glTranslatef(-2 * LITTLE_SIZE * (1 + GAP), -2 * LITTLE_SIZE * (1 + GAP), -2 * LITTLE_SIZE * (1 + GAP));
	for (int currentRow = 0; currentRow < 3; currentRow++)
	{
		for (int currentColum = 0; currentColum < 3; currentColum++)
		{
			for (int currentZ = 0; currentZ < 3; currentZ++)
			{
				PlainColors currentColors = {};
				if (currentZ == 2)
					currentColors.front = currentState.frontFaces[2 - currentRow][2 - currentColum];
				if (currentZ == 0)
					currentColors.back = currentState.backFaces[2 - currentRow][currentColum];
				if (currentRow == 2)
					currentColors.up = currentState.upFaces[currentZ][2 - currentColum];
				if (currentRow == 0)
					currentColors.down = currentState.downFaces[2 - currentZ][2 - currentColum];
				if (currentColum == 2)
					currentColors.left = currentState.leftFaces[2 - currentRow][currentZ];
				if (currentColum == 0)
					currentColors.right = currentState.rightFaces[2 - currentRow][2 - currentZ];
				Point shift = { 0, 0, 0 };
				RotationAny rotation = { 0, 0, 0 };
				switch (CurrentR)
				{
				case Side::F:
					//Pay respects
					if (currentZ == 2)
					{
						shift = { (1 - currentColum) * LITTLE_SIZE * ((1 + GAP) * 2), 0, (1 - currentRow) * LITTLE_SIZE * ((1 + GAP) * 2) };
						rotation = { -edgeAngle, 0.0f, 1.0f, 0.0f };
					}
					break;
				case Side::FS:
					if (currentZ == 2)
					{
						shift = { (1 - currentColum) * LITTLE_SIZE * ((1 + GAP) * 2), 0, (1 - currentRow) * LITTLE_SIZE * ((1 + GAP) * 2) };
						rotation = { edgeAngle, 0.0f, 1.0f, 0.0f };
					}
					break;
				case Side::B:
					if (currentZ == 0)
					{
						shift = { (1 - currentColum) * LITTLE_SIZE * ((1 + GAP) * 2), 0, (1 - currentRow) * LITTLE_SIZE * ((1 + GAP) * 2) };
						rotation = { edgeAngle, 0.0f, 1.0f, 0.0f };
					}
					break;
				case Side::BS:
					if (currentZ == 0)
					{
						shift = { (1 - currentColum) * LITTLE_SIZE * ((1 + GAP) * 2), 0, (1 - currentRow) * LITTLE_SIZE * ((1 + GAP) * 2) };
						rotation = { -edgeAngle, 0.0f, 1.0f, 0.0f };
					}
					break;
				case Side::U:
					if (currentRow == 2)
					{
						shift = { (1 - currentColum) * LITTLE_SIZE * ((1 + GAP) * 2), (1 - currentZ) * LITTLE_SIZE * ((1 + GAP) * 2), 0 };
						rotation = { -edgeAngle, 0.0f, 0.0f, 1.0f };
					}
					break;
				case Side::US:
					if (currentRow == 2)
					{
						shift = { (1 - currentColum) * LITTLE_SIZE * ((1 + GAP) * 2), (1 - currentZ) * LITTLE_SIZE * ((1 + GAP) * 2), 0 };
						rotation = { edgeAngle, 0.0f, 0.0f, 1.0f };
					}
					break;
				case Side::D:
					if (currentRow == 0)
					{
						shift = { (1 - currentColum) * LITTLE_SIZE * ((1 + GAP) * 2), (1 - currentZ) * LITTLE_SIZE * ((1 + GAP) * 2), 0 };
						rotation = { edgeAngle, 0.0f, 0.0f, 1.0f };
					}
					break;
				case Side::DS:
					if (currentRow == 0)
					{
						shift = { (1 - currentColum) * LITTLE_SIZE * ((1 + GAP) * 2), (1 - currentZ) * LITTLE_SIZE * ((1 + GAP) * 2), 0 };
						rotation = { -edgeAngle, 0.0f, 0.0f, 1.0f };
					}
					break;
				case Side::R:
					if (currentColum == 0)
					{
						shift = { 0, (1 - currentZ) * LITTLE_SIZE * ((1 + GAP) * 2), (1 - currentRow) * LITTLE_SIZE * ((1 + GAP) * 2) };
						rotation = { edgeAngle, 1.0f, 0.0f, 0.0f };
					}
					break;
				case Side::RS:
					if (currentColum == 0)
					{
						shift = { 0, (1 - currentZ) * LITTLE_SIZE * ((1 + GAP) * 2), (1 - currentRow) * LITTLE_SIZE * ((1 + GAP) * 2) };
						rotation = { -edgeAngle, 1.0f, 0.0f, 0.0f };
					}
					break;
				case Side::L:
					if (currentColum == 2)
					{
						shift = { 0, (1 - currentZ) * LITTLE_SIZE * ((1 + GAP) * 2), (1 - currentRow) * LITTLE_SIZE * ((1 + GAP) * 2) };
						rotation = { -edgeAngle, 1.0f, 0.0f, 0.0f };
					}
					break;
				case Side::LS:
					if (currentColum == 2)
					{
						shift = { 0, (1 - currentZ) * LITTLE_SIZE * ((1 + GAP) * 2), (1 - currentRow) * LITTLE_SIZE * ((1 + GAP) * 2) };
						rotation = { edgeAngle, 1.0f, 0.0f, 0.0f };
					}
					break;
				}
				DrawSingleCube(currentColors, shift, rotation);
				shift = { 0, 0, 0 };
				rotation = { 0, 0, 0 };
				glTranslatef(0, LITTLE_SIZE * (1 + GAP) * 2, 0);
			}
			glTranslatef(0, -3 * (1 + GAP) * LITTLE_SIZE * 2, 0);
			glTranslatef(LITTLE_SIZE * (1 + GAP) * 2, 0, 0);
		}
		glTranslatef(-3 * (1 + GAP) * LITTLE_SIZE * 2, 0, 0);
		glTranslatef(0, 0, LITTLE_SIZE * (1 + GAP) * 2);
	}
	glutSwapBuffers();
	if (moving)
	{
		edgeAngle += EDGE_SPEED;
	}
	cubeAngle += CUBE_SPEED;
}

void processMenuEvents(int value) {
	currentMenuEvent = static_cast<MenuAction> (value);
}

void createGLUTMenus() {

	int menu;
	menu = glutCreateMenu(processMenuEvents);

	glutAddMenuEntry("Load from file", static_cast<int>(MenuAction::READ));
	glutAddMenuEntry("Save to file", static_cast<int>(MenuAction::SAVE));
	glutAddMenuEntry("Rotate", static_cast<int>(MenuAction::ROTATE));
	glutAddMenuEntry("Solve", static_cast<int>(MenuAction::SOLVE));
	glutAddMenuEntry("Randomize", static_cast<int>(MenuAction::RANDOMIZE));
	glutAddMenuEntry("Reset", static_cast<int>(MenuAction::RESET));
	glutAddMenuEntry("Check", static_cast<int>(MenuAction::CHECK));

	glutAttachMenu(GLUT_RIGHT_BUTTON);
	glutAttachMenu(GLUT_MIDDLE_BUTTON);
	glutAttachMenu(GLUT_LEFT_BUTTON);
}

int MakeNewCube() {
	int argc = 0;
	char arguments[] = " ";
	char* argv;
	argv = arguments;
	glutInit(&argc, &argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_MULTISAMPLE | GLUT_ALPHA);
	glutInitWindowSize(700, 1080);
	glutCreateWindow("3D cube");
	glutDisplayFunc(DrawGLScene);
	glutIdleFunc(idle);
	InitGL();
	createGLUTMenus();
	glutMainLoop();
	return 0;
}


Cube3D::Cube3D() : threadDrawing(MakeNewCube)
{
	setState(defaultState);
}

Cube3D::~Cube3D()
{
	active = false;
}

void Cube3D::setState(const State& newState) noexcept
{
	Cube::setState(newState);
	Update3DState();
}

void Cube3D::setState(volatile const State& newState) noexcept
{
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
		{
			_CurrentState.frontFaces[i][j] = newState.frontFaces[i][j];
			_CurrentState.backFaces[i][j] = newState.backFaces[i][j];
			_CurrentState.upFaces[i][j] = newState.upFaces[i][j];
			_CurrentState.downFaces[i][j] = newState.downFaces[i][j];
			_CurrentState.leftFaces[i][j] = newState.leftFaces[i][j];
			_CurrentState.rightFaces[i][j] = newState.rightFaces[i][j];
		}
	Update3DState();
}

void Cube3D::setEdgeSpeed(const float& speed) noexcept {
	EDGE_SPEED = speed;
}

void Cube3D::UpdateState() noexcept
{
	setState(currentState);
}

void Cube3D::Update3DState() noexcept
{
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
		{
			currentState.frontFaces[i][j] = _CurrentState.frontFaces[i][j];
			currentState.backFaces[i][j] = _CurrentState.backFaces[i][j];
			currentState.upFaces[i][j] = _CurrentState.upFaces[i][j];
			currentState.downFaces[i][j] = _CurrentState.downFaces[i][j];
			currentState.leftFaces[i][j] = _CurrentState.leftFaces[i][j];
			currentState.rightFaces[i][j] = _CurrentState.rightFaces[i][j];
		}

}

void Cube3D::rotateCube(Side rotation) noexcept
{
	CurrentR = rotation;
	moving = true;
	while (moving || (CurrentR != Side::NO))
	{
		
	}
	UpdateState();
}

MenuAction Cube3D::getMenuAction() const noexcept
{
	return currentMenuEvent;
}

void Cube3D::resetMenuAction() noexcept
{
	currentMenuEvent = MenuAction::NO_ACTION;
}

void Cube3D::randomize() noexcept
{
	Cube::randomize();
	Update3DState();
}


