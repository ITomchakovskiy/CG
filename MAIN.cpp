#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>

#include "glut.h"

#define M_PI 3.141592653589793

using std::fstream;
using std::ios;
using std::vector;
/* Разрмер окна */
int Width = 1000;
int Height = 1000;
int MaxCoord = 1000;
float GridStep = 100.;

struct Point3DFloat
{
   float x;
   float y;
   float z;
   Point3DFloat(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {};
   Point3DFloat(float x_, float y_) : x(x_), y(y_), z(0.) {};
   Point3DFloat() : x(0.), y(0.), z(0.) {};
   Point3DFloat operator -(const Point3DFloat& other) const
   {
      return Point3DFloat(x - other.x, y - other.y, z - other.z);
   }
   Point3DFloat operator +(const Point3DFloat& other) const
   {
      return Point3DFloat(x + other.x, y + other.y, z + other.z);
   }
   double operator *(const Point3DFloat& other) const
   {
      return x * other.x + y * other.y + z * other.z;
   }
   Point3DFloat Cross(const Point3DFloat& other)
   {
      return Point3DFloat(y * other.z - z * other.y,
         z * other.x - x * other.z,
         x * other.y - y * other.x);
   }
   double Norm()
   {
      return sqrt(x * x + y * y + z * z);
   }
   void Normalize()
   {
      double Norm = this->Norm();
      x /= Norm;
      y /= Norm;
      z /= Norm;
   }
   Point3DFloat ApplyChangesToPoint(float* Matrix_)
   {
      float coef = Matrix_[3] * x + Matrix_[7] * y + Matrix_[11] * z + Matrix_[15];
      return Point3DFloat(((Matrix_[0] * x + Matrix_[4] * y + Matrix_[8] * z + Matrix_[12]) / coef),
         ((Matrix_[1] * x + Matrix_[5] * y + Matrix_[9] * z + Matrix_[13]) / coef),
         ((Matrix_[2] * x + Matrix_[6] * y + Matrix_[10] * z + Matrix_[14]) / coef));
   }
   Point3DFloat ApplyChangesToVector(float* Matrix_)
   {
      //float coef = Matrix_[3] * x + Matrix_[7] * y + Matrix_[11] * z;
      Point3DFloat result(((Matrix_[0] * x + Matrix_[4] * y + Matrix_[8] * z)),
         ((Matrix_[1] * x + Matrix_[5] * y + Matrix_[9] * z)),
         ((Matrix_[2] * x + Matrix_[6] * y + Matrix_[10] * z)));
      result.Normalize();
      return result;
     /* return Point3DFloat(((Matrix_[0] * x + Matrix_[4] * y + Matrix_[8] * z)),
         ((Matrix_[1] * x + Matrix_[5] * y + Matrix_[9] * z)),
         ((Matrix_[2] * x + Matrix_[6] * y + Matrix_[10] * z)));*/
   }
   float GetAngle(Point3DFloat& other)
   {
      double cos = (other.x*x + other.y*y + other.z*z) / (other.Norm() * this->Norm()) ; //считаем угол для поворота
      double angle_rad = acosf(cos);
      float angle_grad = angle_rad * 180. / M_PI;
      return angle_grad;
   }
};

Point3DFloat SectionVertices[5];

Point3DFloat SectionNormals[5];

vector<Point3DFloat> ReplicationPath;

void SectionNormalsInit()
{
   Point3DFloat VecZ = Point3DFloat(0., 0., 1.);
   for (int i = 0; i < 4; i++)
   {
      Point3DFloat VecSide = SectionVertices[i + 1] - SectionVertices[i];
      SectionNormals[i] = VecZ.Cross(VecSide);
   }
   Point3DFloat VecSide = SectionVertices[0] - SectionVertices[4];
   SectionNormals[4] = VecZ.Cross(VecSide);
}

struct Point3DFloatSet
{
   Point3DFloat Points[5];
   Point3DFloatSet() {};
   Point3DFloatSet(Point3DFloat *points)
   {
      *this = points;
   }
   void ApplyChangesToPointsSet(float* Matrix_)
   {
      for (int i = 0; i < 5; i++)
      {
         Point3DFloat Point = SectionVertices[i];
         float coef = Matrix_[3] * Point.x + Matrix_[7] * Point.y + Matrix_[11] * Point.z + Matrix_[15];
         /*Points[i] = Point3DFloat(((Matrix_[0] * Point.x + Matrix_[4] * Point.y + Matrix_[8] * Point.z + Matrix_[12]) / coef),
                                   ((Matrix_[1] * Point.x + Matrix_[5] * Point.y + Matrix_[9] * Point.z + Matrix_[13]) / coef),
                                   ((Matrix_[2] * Point.x + Matrix_[6] * Point.y + Matrix_[10] * Point.z + Matrix_[14]) / coef));*/
         Points[i] = Point.ApplyChangesToPoint(Matrix_);
      }
   }
   void ApplyChangesToVectorsSet(float* Matrix_)
   {
      for (int i = 0; i < 5; i++)
      {
         Point3DFloat Vector = SectionNormals[i];
         //float coef = Matrix_[3] * Vector.x + Matrix_[7] * Vector.y + Matrix_[11] * Vector.z;
         /*Points[i] = Point3DFloat(((Matrix_[0] * Vector.x + Matrix_[4] * Vector.y + Matrix_[8] * Vector.z)),
                                  ((Matrix_[1] * Vector.x + Matrix_[5] * Vector.y + Matrix_[9] * Vector.z)),
                                  ((Matrix_[2] * Vector.x + Matrix_[6] * Vector.y + Matrix_[10] * Vector.z)));*/
         Points[i] = Vector.ApplyChangesToVector(Matrix_);
      }
   }
};

vector<Point3DFloatSet> Points;

vector<Point3DFloatSet> Normals;

Point3DFloat BorderNormals[2];

void ReadSection()
{
   fstream fin;
   fin.open("Section.txt", ios::in);
   for (int i = 0; i < 5; i++)
   {
      fin >> SectionVertices[i].x;
      fin >> SectionVertices[i].y;
   }
   fin.close();
}

void ReadReplicationPath()
{
   fstream fin;
   fin.open("ReplicationPath.txt", ios::in);
   int n;
   fin >> n;
   ReplicationPath.resize(n);
   for (int i = 0; i < n; i++)
   {
      fin >> ReplicationPath[i].x;
      fin >> ReplicationPath[i].y;
      fin >> ReplicationPath[i].z;
   }
}

float Matrix[16];

void FirstSection()
{
   //glPushMatrix(); //загрузка матрицы в стек
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   glTranslatef(ReplicationPath[0].x, ReplicationPath[0].y, ReplicationPath[0].z); //перемещение в начало тиражирования
   Point3DFloat ReplicationDirection = ReplicationPath[1] - ReplicationPath[0];  //направление тиражирования
   Point3DFloat SectionNormal = Point3DFloat(0, 0, 1.); //нормаль
   float angle = ReplicationDirection.GetAngle(SectionNormal);
   Point3DFloat RotateDirection = SectionNormal.Cross(ReplicationDirection); //вектор,вокруг которого будем совершать поворот
   if (RotateDirection.x > 1.0E-5 || RotateDirection.y > 1.0E-5 || RotateDirection.z > 1.0E-5)
   {
      RotateDirection.Normalize();
      glRotatef(angle, RotateDirection.x, RotateDirection.y, RotateDirection.z);  //поворот, ось z по направлению тиражирования
   }
   glGetFloatv(GL_MODELVIEW_MATRIX, Matrix); //получаем матрицу преобразований
   Point3DFloatSet NewSectionPoints;
   NewSectionPoints.ApplyChangesToPointsSet(Matrix);
   Points[0] = NewSectionPoints;
   //Points.push_back(NewSectionPoints);
   Point3DFloatSet NewSectionNormals;
   NewSectionNormals.ApplyChangesToVectorsSet(Matrix);
   Normals[0] = NewSectionNormals;
   //Normals.push_back(NewSectionNormals);
   BorderNormals[0] = SectionNormal.ApplyChangesToVector(Matrix);
   //glPopMatrix();
    glLoadIdentity();
}

void LastSection()
{
   //glPushMatrix(); //загрузка матрицы в стек
   glTranslatef(ReplicationPath[ReplicationPath.size()-1].x, ReplicationPath[ReplicationPath.size() - 1].y, ReplicationPath[ReplicationPath.size() - 1].z); //перемещение в начало тиражирования
   Point3DFloat ReplicationDirection = ReplicationPath[ReplicationPath.size() - 1] - ReplicationPath[ReplicationPath.size() - 2];  //направление тиражирования
   Point3DFloat SectionNormal = Point3DFloat(0, 0, 1.); //нормаль
   float angle = ReplicationDirection.GetAngle(SectionNormal);
   Point3DFloat RotateDirection = SectionNormal.Cross(ReplicationDirection); //вектор,вокруг которого будем совершать поворот
   if (RotateDirection.x > 1.0E-5 || RotateDirection.y > 1.0E-5 || RotateDirection.z > 1.0E-5)
   {
      RotateDirection.Normalize();
      glRotatef(angle, RotateDirection.x, RotateDirection.y, RotateDirection.z);  //поворот, ось z по направлению тиражирования
   }
   glGetFloatv(GL_MODELVIEW_MATRIX, Matrix); //получаем матрицу преобразований
   Point3DFloatSet NewSectionPoints;
   NewSectionPoints.ApplyChangesToPointsSet(Matrix);
   Points[Points.size() - 1] = NewSectionPoints;
   //Points.push_back(NewSectionPoints);
   /*Point3DFloatSet NewSectionNormals;
   NewSectionNormals.ApplyChangesToVectorsSet(Matrix);
   Normals.push_back(NewSectionNormals);*/
   SectionNormal.z = -SectionNormal.z;
   BorderNormals[1] = SectionNormal.ApplyChangesToVector(Matrix);
   //glPopMatrix();
   glLoadIdentity();
}

void Sections()
{
   for (int i = 1; i < ReplicationPath.size()-1; i++)
   {
      //glPushMatrix(); //загрузка матрицы в стек
      Point3DFloat PreviousReplicationDirection = ReplicationPath[i] - ReplicationPath[i - 1];  //направление тиражирования до узла
      Point3DFloat NextReplicationDirection = ReplicationPath[i + 1] - ReplicationPath[i];  //направление тиражирования после узла
      Point3DFloat SectionNormal = Point3DFloat(0, 0, 1.); //нормаль
      glTranslatef(ReplicationPath[i].x, ReplicationPath[i].y, ReplicationPath[i].z); //перемещение в узел
      glPushMatrix();                                 
      //для сечения
      Point3DFloat AddingDirection = NextReplicationDirection - PreviousReplicationDirection;
      AddingDirection.x /= 2.;
      AddingDirection.y /= 2.;
      AddingDirection.z /= 2.;
      Point3DFloat ReplicationDirection = PreviousReplicationDirection + AddingDirection;
      float angle = ReplicationDirection.GetAngle(SectionNormal);
      Point3DFloat RotateDirection = SectionNormal.Cross(ReplicationDirection); //вектор,вокруг которого будем совершать поворот
      if (RotateDirection.x > 1.0E-5 || RotateDirection.y > 1.0E-5 || RotateDirection.z > 1.0E-5)
      {
         RotateDirection.Normalize();
         glRotatef(angle, RotateDirection.x, RotateDirection.y, RotateDirection.z);  //поворот, ось z по направлению тиражирования
      }
      glGetFloatv(GL_MODELVIEW_MATRIX, Matrix); //получаем матрицу преобразований
      Point3DFloatSet NewSectionPoints;
      NewSectionPoints.ApplyChangesToPointsSet(Matrix);
      Points[i] = NewSectionPoints;
      //Points.push_back(NewSectionPoints);
      /*Point3DFloatSet NewSectionNormals;
      NewSectionNormals.ApplyChangesToVectorsSet(Matrix);
      Normals.push_back(NewSectionNormals);*/
      glPopMatrix();
      //для нормалей
      ReplicationDirection = NextReplicationDirection;
      angle = ReplicationDirection.GetAngle(SectionNormal);
      RotateDirection = SectionNormal.Cross(ReplicationDirection); //вектор,вокруг которого будем совершать поворот
      if (RotateDirection.x > 1.0E-5 || RotateDirection.y > 1.0E-5 || RotateDirection.z > 1.0E-5)
      {
         RotateDirection.Normalize();
         glRotatef(angle, RotateDirection.x, RotateDirection.y, RotateDirection.z);  //поворот, ось z по направлению тиражирования
      }
      glGetFloatv(GL_MODELVIEW_MATRIX, Matrix); //получаем матрицу преобразований
      Point3DFloatSet NewSectionNormals;
      NewSectionNormals.ApplyChangesToVectorsSet(Matrix);
      Normals[i] = NewSectionNormals;
      //Normals.push_back(NewSectionNormals);
      glLoadIdentity();
   }
}

void DisplayGrid()
{
   glLineWidth(10);

   glColor3ub(255, 0, 0);
   glBegin(GL_LINES);

   glVertex2f(-MaxCoord, 0.);
   glVertex2f(MaxCoord, 0.);

   glVertex2f(-MaxCoord, -MaxCoord);
   glVertex2f(MaxCoord, -MaxCoord);

   glVertex2f(-MaxCoord, MaxCoord);
   glVertex2f(MaxCoord, MaxCoord);

   glColor3ub(0, 0, 255);
   glVertex2f(0., -MaxCoord);
   glVertex2f(0., MaxCoord);

   glVertex2f(-MaxCoord, -MaxCoord);
   glVertex2f(-MaxCoord, MaxCoord);

   glVertex2f(MaxCoord, -MaxCoord);
   glVertex2f(MaxCoord, MaxCoord);

   glEnd();

   glLineWidth(3);

   

   glLineStipple(1, 255);
   glEnable(GL_LINE_STIPPLE);

   glBegin(GL_LINES);

   glColor3ub(255, 0, 0);

   for (int i = 1; i < MaxCoord / GridStep; i++)
   {
      glVertex2f(-MaxCoord, i* GridStep);
      glVertex2f(MaxCoord, i * GridStep);

      glVertex2f(-MaxCoord, -i * GridStep);
      glVertex2f(MaxCoord, -i * GridStep);
   }

   glColor3ub(0, 0, 255);

   for (int i = 1; i < MaxCoord / GridStep; i++)
   {
      glVertex2f(i * GridStep, -MaxCoord);
      glVertex2f(i * GridStep, MaxCoord);

      glVertex2f(-i * GridStep, -MaxCoord);
      glVertex2f(-i * GridStep, MaxCoord);
   }

   glEnd();

   glDisable(GL_LINE_STIPPLE);
   //glVertex2f(-MaxCoord, 0.);
   //glVertex2f(MaxCoord, 0.);

   //glColor3ub(0, 0, 255);
   //glVertex2f(0., -MaxCoord);
   //glVertex2f(0., MaxCoord);
   //glEnd;
}

void DisplaySections()
{
  // glColor3f(1., 0., 0.);
   Point3DFloat Normal = BorderNormals[0];
   glNormal3f(Normal.x, Normal.y, Normal.z);
   glBegin(GL_POLYGON);
   for (int i = 0; i < 5; i++)
   {
      Point3DFloat Vertex = Points[0].Points[i];
      glVertex3f(Vertex.x, Vertex.y, Vertex.z);
   }
   glEnd();
   glNormal3f(0, 0, 1.);

   Normal = BorderNormals[1];
   glNormal3f(Normal.x, Normal.y, Normal.z);
   glBegin(GL_POLYGON);
   for (int i = 0; i < 5; i++)
   {
      Point3DFloat Vertex = Points[Points.size()-1].Points[i];
      glVertex3f(Vertex.x, Vertex.y, Vertex.z);
   }
   glEnd();
}

void DisplayQuads()
{
   //glColor3f(.5, .5, .5);
   for (int i = 0; i < Points.size() - 1; i++)
   {
      Point3DFloatSet PreviousSection = Points[i];
      Point3DFloatSet NextSection = Points[i+1];

      for (int j = 0; j < 4; j++)
      {
         Point3DFloat Normal = Normals[i].Points[j];
         Point3DFloat Vertices[4] = { PreviousSection.Points[j], PreviousSection.Points[j + 1], NextSection.Points[j + 1], NextSection.Points[j] };

         glNormal3f(Normal.x, Normal.y, Normal.z);

         glBegin(GL_QUADS);
         for (int p = 0; p < 4; p++)
         {
            glVertex3f(Vertices[p].x, Vertices[p].y, Vertices[p].z);
         }
         glEnd();
      }
      Point3DFloat Normal = Normals[i].Points[4];
      Point3DFloat Vertices[4] = { PreviousSection.Points[4], PreviousSection.Points[0], NextSection.Points[0], NextSection.Points[4] };

      glNormal3f(Normal.x, Normal.y, Normal.z);

      glBegin(GL_QUADS);
      for (int p = 0; p < 4; p++)
      {
         glVertex3f(Vertices[p].x, Vertices[p].y, Vertices[p].z);
      }
      glEnd();
   }
   
}

void Display(void)
{
   glClearColor(1, 1, 1, 1);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   //glColor3ub(20,100);

   
   
   FirstSection();
   Sections();
   LastSection();
   glRotatef(90, 1, 1, 1); // поворот

   DisplayGrid();

   glColor3ub(100, 100, 100);

   DisplaySections();
   DisplayQuads();
   glFinish();
}

float light0_positionT[] = { 500, 500, 500, 1 };

void Reshape(GLint w, GLint h)
{
   Width = w; Height = h;
   glViewport(0, 0, w, h);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glOrtho(-w / 2, w / 2, -h / 2, h / 2, -1000, 1000);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
}

void main(int argc, char* argv[])
{
   ReadSection();
   ReadReplicationPath();
   SectionNormalsInit();
   Points.resize(ReplicationPath.size());
   Normals.resize(Points.size() - 1);
   glutInit(&argc, argv);
   glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH);
   glutInitWindowSize(Width, Height);
   glutCreateWindow("Проекции");
   glutDisplayFunc(Display);
   glutReshapeFunc(Reshape);
   glEnable(GL_DEPTH_TEST);
   //glEnable(GL_LIGHTING);
   //glEnable(GL_LIGHT0);
   //glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
   //glLightfv(GL_LIGHT0, GL_POSITION, light0_positionT);
   glutMainLoop();
}