

#include <GL/glut.h>
#include <math.h>
#include <iostream>
#include <vector>

class myPoint{

public:

  GLdouble x, y;
  myPoint(){
    x = 0.0;
    y = 0.0;
  }
  myPoint(GLdouble X, GLdouble Y){
    x = X;
    y = Y;
  }
  myPoint(const myPoint& a){
    x = a.x;
    y = a.y;
  }

  void set(const myPoint& a){
    x = a.x;
    y = a.y;
  }

private:

};
myPoint operator*(const myPoint& p, GLdouble a){
  myPoint ret(p.x*a,p.y*a);
  return ret;
}
myPoint operator*(GLdouble a, const myPoint& p){
  myPoint ret(p.x*a,p.y*a);
  return ret;
}
myPoint operator+(const myPoint& a, const myPoint& b){
  myPoint ret(a.x+b.x,a.y+b.y);
  return ret;
}
myPoint operator-(const myPoint& a, const myPoint& b){
  myPoint ret(a.x-b.x,a.y-b.y);
  return ret;
}


class myMatrix{
public:
  std::vector<std::vector<GLdouble>> M;
  myMatrix(){
  }
  myMatrix(GLdouble matrix[4][4]){
    M.push_back( std::vector<GLdouble>(&(matrix[0][0]),&(matrix[0][0])+4) );
    M.push_back( std::vector<GLdouble>(&(matrix[1][0]),&(matrix[1][0])+4) );
    M.push_back( std::vector<GLdouble>(&(matrix[2][0]),&(matrix[2][0])+4) );
    M.push_back( std::vector<GLdouble>(&(matrix[3][0]),&(matrix[3][0])+4) );
  }

  std::vector<myPoint> operator*(const std::vector<myPoint> points){
    std::vector<myPoint> ret;
    for (int i = 0; i < 4; i++) {
      myPoint akt;
      for (int j = 0; j < 4; j++) {
        akt = akt + (points[j]*M[i][j]);
      }
      ret.push_back(akt);
    }
    return ret;
  }
private:
};

std::vector<myPoint> operator*(GLdouble M[4][4], const std::vector<myPoint> points){
  std::vector<myPoint> ret;
  for (int i = 0; i < 4; i++) {
    myPoint akt;
    for (int j = 0; j < 4; j++) {
      akt = akt + (points[j]*M[i][j]);
    }
    ret.push_back(akt);
  }
  return ret;
}

myPoint operator*(const std::vector<double> a, const std::vector<myPoint> b){
  myPoint ret;
  for (int i = 0; i < a.size(); i++) {
    ret = ret + (a[i]*b[i]);
  }

  return ret;
}

std::vector<GLdouble> mask;
int depth = 0;

std::vector<myPoint> subdivPoints(std::vector<myPoint> points, int depth){
  std::vector<myPoint> ret;

  for(int i = 0; i< points.size();i++){
    int index[4] = {i-1,i,(int)((i+1) % points.size()),(int)((i+2) % points.size())};
    if (i-1 < 0) index[0]=points.size()-1;

    std::cout << index[0] << ", " << index[1] << ", " << index[2] << ", " << index[3] << " size: " << points.size() << '\n';

    ret.push_back(points[i]);
    myPoint sum;
    for (int j = 0; j< mask.size();j++){
      sum = sum + (mask[j]*points[index[j]]);
    }
    ret.push_back(sum);
  }

  if(depth == 0){
    return ret;
  }
  else{
    return subdivPoints(ret,depth-1);
  }
}

std::vector<myPoint> kontrollPontok;
GLint dragged = -1;
GLdouble rad(GLint i)
{
	return i*(3.1415 / 180.0);
}

GLfloat dist2(myPoint P1, myPoint P2) {
	GLfloat t1 = P1.x - P2.x;
	GLfloat t2 = P1.y - P2.y;
	return t1 * t1 + t2 * t2;
}

void init(void)
{
	glClearColor(1.0, 1.0, 1.0, 0.0);
	glMatrixMode(GL_PROJECTION);
	gluOrtho2D(0.0, 700, 0.0, 600);
	glShadeModel(GL_FLAT);
	glEnable(GL_LINE_STIPPLE);
	glPointSize(10.0);
  glLineWidth(5.0);

  mask.push_back(-3.00/16.00);
  mask.push_back(11.00/16.00);
  mask.push_back(11.00/16.00);
  mask.push_back(-3.00/16.00);


}
void lineSegment(void)
{

	glClear(GL_COLOR_BUFFER_BIT);

	glColor3f(0.5, 0.5, 0.5);

  glBegin(GL_LINE_LOOP);
    for (int i = 0; i < kontrollPontok.size(); i++) {
      glVertex2d(kontrollPontok[i].x, kontrollPontok[i].y);
    }
  glEnd();

  glBegin(GL_POINTS);
    for (int i = 0; i < kontrollPontok.size(); i++) {
      glVertex2d(kontrollPontok[i].x, kontrollPontok[i].y);
    }
  glEnd();

  if(kontrollPontok.size()>=4){
    glColor3f(1.0, 0.0, 0.0);
    glBegin(GL_LINE_LOOP);
      for (myPoint akt : subdivPoints(kontrollPontok,depth)) {
        glVertex2d(akt.x, akt.y);
      }
    glEnd();
  }

	glutSwapBuffers();
}

GLint getActivePoint1(std::vector<myPoint> p, GLint x, GLint y) {
	GLint s = 100;
	myPoint P(x, y);
	for (GLint i = 0; i < p.size(); i++){
		myPoint value(p[i].x, p[i].y );
		if (dist2(value, P) < s)
			return i;
		}
	return -1;
}

void processMouse(GLint button, GLint action, GLint xMouse, GLint yMouse) {
	GLint i=0;
	if (button == GLUT_LEFT_BUTTON && action == GLUT_DOWN){
		if ((i = getActivePoint1(kontrollPontok, xMouse, 600 - yMouse)) != -1){
			dragged = i;
    }
    else{
      kontrollPontok.push_back(myPoint(xMouse, 600 - yMouse));
      glutPostRedisplay();
    }
  }
	if (button == GLUT_LEFT_BUTTON && action == GLUT_UP)
		dragged = -1;
}

void processMouseActiveMotion(GLint xMouse, GLint yMouse) {
	if (dragged >= 0) {
		kontrollPontok[dragged].x = xMouse;
  	kontrollPontok[dragged].y = 600 - yMouse;
    glutPostRedisplay();
	}

}
void keyPressed (unsigned char key, int x, int y) {
     switch (key){
      	case 'w':
          depth +=1;
      	 glutPostRedisplay();
      	break;
      	case 's':
          depth = depth>0?depth-1:depth;
      	  glutPostRedisplay();
      	break;
    }
}


int main (int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(700, 600);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Subdiv");
	init();
	glutKeyboardFunc(keyPressed);
	glutDisplayFunc(lineSegment);
	glutMouseFunc(processMouse);
	glutMotionFunc(processMouseActiveMotion);
	glutMainLoop();
	return 0;
}
