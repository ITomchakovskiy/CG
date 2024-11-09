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
int Width = 1000; //Ширина окна
int Height = 1000; //Высота окна
int MaxCoord = 1000; //максимальная координата для прямых сетки
float GridStep = 100.; //шаг сетки

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
   Point3DFloat Cross(const Point3DFloat& other)  //декартово произведение
   {
      return Point3DFloat(y * other.z - z * other.y,
         z * other.x - x * other.z,
         x * other.y - y * other.x);
   }
   double Norm() //расчет нормы вектора
   {
      return sqrt(x * x + y * y + z * z);
   }
   void Normalize() //Нормирование вектора
   {
      double Norm = this->Norm();
      x /= Norm;
      y /= Norm;
      z /= Norm;
   }
   Point3DFloat ApplyChangesToPoint(float* Matrix_) //умножение модельно-видовой матрицы на вектор 
                                                    //для получения координат точки в исходной системе координат по координатам в измененной системе координат
   {
      float coef = Matrix_[3] * x + Matrix_[7] * y + Matrix_[11] * z + Matrix_[15];
      return Point3DFloat(((Matrix_[0] * x + Matrix_[4] * y + Matrix_[8] * z + Matrix_[12]) / coef),
         ((Matrix_[1] * x + Matrix_[5] * y + Matrix_[9] * z + Matrix_[13]) / coef),
         ((Matrix_[2] * x + Matrix_[6] * y + Matrix_[10] * z + Matrix_[14]) / coef));
   }
   Point3DFloat ApplyChangesToVector(float* Matrix_)//умножение модельно-видовой матрицы на вектор 
                                                    //для получения координат вектора в исходной системе координат по координатам в измененной системе координат
   {
      Point3DFloat result(((Matrix_[0] * x + Matrix_[4] * y + Matrix_[8] * z)),
         ((Matrix_[1] * x + Matrix_[5] * y + Matrix_[9] * z)),
         ((Matrix_[2] * x + Matrix_[6] * y + Matrix_[10] * z)));
      result.Normalize();
      return result;
   }
   float GetAngle(Point3DFloat& other) //Расчет угла между векторами
   {
      double cos = (other.x * x + other.y * y + other.z * z) / (other.Norm() * this->Norm()); //считаем угол для поворота
      double angle_rad = acosf(cos);
      float angle_grad = angle_rad * 180. / M_PI;
      return angle_grad;
   }
};

vector<Point3DFloat> ReplicationPath; //траектория тиражирования

struct Point3DFloatSet 
{
   Point3DFloat Points[5];
   Point3DFloatSet() {};
 
   void ApplyChangesToPointsSet(float* Matrix_) //применение модельно-видовых преобразований ко всем точкам набора
   {
      for (int i = 0; i < 5; i++)
      {
         Points[i] = Points[i].ApplyChangesToPoint(Matrix_);
      }
   }
   void ApplyChangesToVectorsSet(float* Matrix_) //применение модельно-видовых преобразований ко всем векторам набора
   {
      for (int i = 0; i < 5; i++)
      {
         Points[i] = Points[i].ApplyChangesToVector(Matrix_); 
      }
   }
};

Point3DFloatSet SectionVertices; //вершины исходного сечения

Point3DFloatSet SectionNormalVectors; //векторы нормали для каждой стороны исходного сечения

vector<Point3DFloatSet> Points; //массив сечений в узлах траектории тиражирования

vector<Point3DFloatSet> NormalVectors; //массив боковых нормалей относительно направления тиражирования

Point3DFloat BorderNormalVectors[2]; //вектор нормали для граней соответствующих первому и последнему сечению

void SectionNormalVectorsInit() //инициализация исходных нормалей
{
   Point3DFloat VecZ = Point3DFloat(0., 0., 1.);
   for (int i = 0; i < 4; i++)
   {
      Point3DFloat VecSide = SectionVertices.Points[i + 1] - SectionVertices.Points[i];
      SectionNormalVectors.Points[i] = VecZ.Cross(VecSide);
   }
   Point3DFloat VecSide = SectionVertices.Points[0] - SectionVertices.Points[4];
   SectionNormalVectors.Points[4] = VecZ.Cross(VecSide);
}

void ReadSection() //чтение сечения
{
   fstream fin;
   fin.open("Section.txt", ios::in);
   for (int i = 0; i < 5; i++)
   {
      fin >> SectionVertices.Points[i].x;
      fin >> SectionVertices.Points[i].y;
   }
   fin.close();
}

void ReadReplicationPath() //чтение траектории тиражирования
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
   fin.close();
}

vector<std::pair<float,float>> ScaleModifiers; //параметры масштабирования сечения по x и y

void ReadScaleModifiers() //чтение параметров масштабирования
{
   fstream fin;
   fin.open("ScaleModifiers.txt", ios::in);
   int n = ReplicationPath.size();
   ScaleModifiers.resize(n);
   for (int i = 0; i < n; i++)
   {
      fin >> ScaleModifiers[i].first;
      fin >> ScaleModifiers[i].second;
   }
   fin.close();
}

void RotateVectorToVector(Point3DFloat& A, Point3DFloat& B) //поворачивает систему координат таким образом чтобы вектора были сонапрвлены
{
   float angle = A.GetAngle(B);
   Point3DFloat RotateDirection = B.Cross(A); //вектор,вокруг которого будем совершать поворот
   if (RotateDirection.x > 1.0E-5 || RotateDirection.y > 1.0E-5 || RotateDirection.z > 1.0E-5)
   {
      RotateDirection.Normalize();
      glRotatef(angle, RotateDirection.x, RotateDirection.y, RotateDirection.z);  //поворот, ось z по направлению тиражирования
   }
}

float Matrix[16];

void ModifyScale(Point3DFloatSet& SectionPoints_, Point3DFloatSet& SectionNormalVectors_, float* Matrix_, int num) //применяет масштабирование к сечению и нормалям для сторон сечения
{
   glPushMatrix(); //загрузка матрицы в стек

   glScalef(ScaleModifiers[num].first, ScaleModifiers[num].second, 1.);

   glGetFloatv(GL_MODELVIEW_MATRIX, Matrix_); //получаем матрицу преобразований

   glPopMatrix();

   SectionPoints_.ApplyChangesToPointsSet(Matrix_);
   SectionNormalVectors_.ApplyChangesToVectorsSet(Matrix_);
}

void FirstSection() // обработка первого сечения
{
   glPushMatrix(); //сохраняем модельно-видовую матрицу

   Point3DFloatSet SectionPointsWithModifiedScale = SectionVertices;
   Point3DFloatSet SectionNormalVectorsWithModifiedScale = SectionNormalVectors;
   ModifyScale(SectionPointsWithModifiedScale, SectionNormalVectorsWithModifiedScale, Matrix, 0); //масштабируем сечение и нормали

   glTranslatef(ReplicationPath[0].x, ReplicationPath[0].y, ReplicationPath[0].z); //перемещение в начало тиражирования

   Point3DFloat ReplicationDirection = ReplicationPath[1] - ReplicationPath[0];  //направление тиражирования
   Point3DFloat SectionNormal = Point3DFloat(0, 0, 1.); //нормаль грани сечения
   RotateVectorToVector(ReplicationDirection, SectionNormal); //поворачиваем сечение перпендикулярно к направлению тиражирования

   glGetFloatv(GL_MODELVIEW_MATRIX, Matrix); //получаем матрицу преобразований

   SectionPointsWithModifiedScale.ApplyChangesToPointsSet(Matrix); 
   Points[0] = SectionPointsWithModifiedScale;  //заносим вершины сечения в исходной системе координат

   SectionNormalVectorsWithModifiedScale.ApplyChangesToVectorsSet(Matrix);
   NormalVectors[0] = SectionNormalVectorsWithModifiedScale; //заносим нормали сторон сечения в исходной системе координат

   BorderNormalVectors[0] = SectionNormal.ApplyChangesToVector(Matrix); //то же для нормали самого сечения

   glPopMatrix();
}

void LastSection()
{
   glPushMatrix(); //сохраняем модельно-видовую матрицу

   int num = Points.size() - 1;

   Point3DFloatSet SectionPointsWithModifiedScale = SectionVertices;
   Point3DFloatSet SectionNormalVectorsWithModifiedScale = SectionNormalVectors;
   ModifyScale(SectionPointsWithModifiedScale, SectionNormalVectorsWithModifiedScale, Matrix, num); //масштабируем сечение и нормали

   glTranslatef(ReplicationPath[num].x, ReplicationPath[num].y, ReplicationPath[num].z); //перемещение в начало тиражирования

   Point3DFloat ReplicationDirection = ReplicationPath[num] - ReplicationPath[num - 1];  //направление тиражирования
   Point3DFloat SectionNormal = Point3DFloat(0, 0, 1.); //нормаль сечения
   RotateVectorToVector(ReplicationDirection, SectionNormal); //поворачиваем сечение перпендикулярно к направлению тиражирования

   glGetFloatv(GL_MODELVIEW_MATRIX, Matrix); //получаем матрицу преобразований

   SectionPointsWithModifiedScale.ApplyChangesToPointsSet(Matrix); 
   Points[num] = SectionPointsWithModifiedScale; //заносим вершины сечения в исходной системе координат

   SectionNormal.z = -SectionNormal.z;
   BorderNormalVectors[1] = SectionNormal.ApplyChangesToVector(Matrix);

   glPopMatrix();
}

void Sections()
{
   for (int i = 1; i < ReplicationPath.size() - 1; i++)
   {
      glPushMatrix(); //сохраняем модельно-видовую матрицу

      Point3DFloatSet SectionPointsWithModifiedScale = SectionVertices;
      Point3DFloatSet SectionNormalVectorsWithModifiedScale = SectionNormalVectors;
      ModifyScale(SectionPointsWithModifiedScale, SectionNormalVectorsWithModifiedScale, Matrix, i);  //масштабируем сечение и нормали

      Point3DFloat PreviousReplicationDirection = ReplicationPath[i] - ReplicationPath[i - 1];  //направление тиражирования до узла
      Point3DFloat NextReplicationDirection = ReplicationPath[i + 1] - ReplicationPath[i];  //направление тиражирования после узла
      Point3DFloat SectionNormal = Point3DFloat(0, 0, 1.); //нормаль сечения
      glTranslatef(ReplicationPath[i].x, ReplicationPath[i].y, ReplicationPath[i].z); //перемещение в узел
      glPushMatrix();
      //для вершин сечения
      Point3DFloat AddingDirection = NextReplicationDirection - PreviousReplicationDirection;
      AddingDirection.x /= 2.;
      AddingDirection.y /= 2.;
      AddingDirection.z /= 2.;
      Point3DFloat ReplicationDirection = PreviousReplicationDirection + AddingDirection;

      RotateVectorToVector(ReplicationDirection, SectionNormal); //поворачиваем сечение на половину угла между направлением тиражирования до узла и после

      glGetFloatv(GL_MODELVIEW_MATRIX, Matrix); //получаем матрицу преобразований

      SectionPointsWithModifiedScale.ApplyChangesToPointsSet(Matrix);
      Points[i] = SectionPointsWithModifiedScale;

      glPopMatrix();
      //для нормалей
      ReplicationDirection = NextReplicationDirection;

      RotateVectorToVector(ReplicationDirection, SectionNormal); //поворачиваем сечение перпендикулярно к направлению тиражирования

      glGetFloatv(GL_MODELVIEW_MATRIX, Matrix); //получаем матрицу преобразований

      SectionNormalVectorsWithModifiedScale.ApplyChangesToVectorsSet(Matrix);
      NormalVectors[i] = SectionNormalVectorsWithModifiedScale;

      glPopMatrix();
   }
}

void DisplayGrid()
{
   glLineWidth(10);

   glColor3ub(255, 0, 0);
   glBegin(GL_LINES); 
                        //рисуем толстые красные отрезки для оси х и для границ сетки по х
   glVertex2f(-MaxCoord, 0.); 
   glVertex2f(MaxCoord, 0.);

   glVertex2f(-MaxCoord, -MaxCoord);
   glVertex2f(MaxCoord, -MaxCoord);

   glVertex2f(-MaxCoord, MaxCoord);
   glVertex2f(MaxCoord, MaxCoord);
                        //рисуем толстые синие отрезки для оси y и для границ сетки по y
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

   glColor3ub(255, 0, 0); //рисуем тонкие отрезки пунктиром красным цветом по х

   for (int i = 1; i < MaxCoord / GridStep; i++)
   {
      glVertex2f(-MaxCoord, i * GridStep);
      glVertex2f(MaxCoord, i * GridStep);

      glVertex2f(-MaxCoord, -i * GridStep);
      glVertex2f(MaxCoord, -i * GridStep);
   }

   glColor3ub(0, 0, 255); //рисуем тонкие отрезки пунктиром синим цветом по у

   for (int i = 1; i < MaxCoord / GridStep; i++)
   {
      glVertex2f(i * GridStep, -MaxCoord);
      glVertex2f(i * GridStep, MaxCoord);

      glVertex2f(-i * GridStep, -MaxCoord);
      glVertex2f(-i * GridStep, MaxCoord);
   }

   glEnd();

   glDisable(GL_LINE_STIPPLE);
}

void DisplaySections() //отрисовка сечений
{
   glColor3ub(255., 255., 0.); //цвет сечения
   Point3DFloat Normal = BorderNormalVectors[0];
   glNormal3f(Normal.x, Normal.y, Normal.z);
   glBegin(GL_POLYGON);
   for (int i = 0; i < 5; i++)
   {
      Point3DFloat Vertex = Points[0].Points[i];
      glVertex3f(Vertex.x, Vertex.y, Vertex.z);
   }
   glEnd();
   glNormal3f(0, 0, 1.);

   Normal = BorderNormalVectors[1];
   glNormal3f(Normal.x, Normal.y, Normal.z);
   glBegin(GL_POLYGON);
   for (int i = 0; i < 5; i++)
   {
      Point3DFloat Vertex = Points[Points.size() - 1].Points[i];
      glVertex3f(Vertex.x, Vertex.y, Vertex.z);
   }
   glEnd();
}

void DisplayQuads() //отрисовка боковых граней
{
   glColor3ub(100, 100, 100); //цвет боковых граней
   for (int i = 0; i < Points.size() - 1; i++)
   {
      Point3DFloatSet PreviousSection = Points[i];
      Point3DFloatSet NextSection = Points[i + 1];

      for (int j = 0; j < 4; j++)
      {
         Point3DFloat Normal = NormalVectors[i].Points[j];
         Point3DFloat Vertices[4] = { PreviousSection.Points[j], PreviousSection.Points[j + 1], NextSection.Points[j + 1], NextSection.Points[j] };

         glNormal3f(Normal.x, Normal.y, Normal.z);

         glBegin(GL_QUADS);
         for (int p = 0; p < 4; p++)
         {
            glVertex3f(Vertices[p].x, Vertices[p].y, Vertices[p].z);
         }
         glEnd();
      }
      Point3DFloat Normal = NormalVectors[i].Points[4];
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

//float light0_positionT[] = { 500, 500, 500, 1 };

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
   ReadScaleModifiers();
   SectionNormalVectorsInit();
   Points.resize(ReplicationPath.size());
   NormalVectors.resize(Points.size() - 1);
   glutInit(&argc, argv);
   glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH);
   glutInitWindowSize(Width, Height);
   glutCreateWindow("Тиражирование сечений");
   glutDisplayFunc(Display);
   glutReshapeFunc(Reshape);
   glEnable(GL_DEPTH_TEST);
 
   glutMainLoop();
}