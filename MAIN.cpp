#include <vector>

#include <iostream>

#include <string>

#include <fstream>

#include <limits>

#include "glut.h"


using std::string;
using std::vector;
using std::fstream;
using std::ios;

int Width = 1000; //Ширина окна
int Height = 1000; //Высота окна
int MaxCoord = 1000; //максимальная координата для прямых сетки
float GridStep = 100.; //шаг сетки
float CameraStepX = 0.;
float CameraStepY = 0.;
float CameraStepZ = 0.;

float CameraAngleX = 0.;
float CameraAngleY = 0.;
//float cameraAngleX = 0; // Угол поворота камеры по X
//float cameraAngleY = 0; // Угол поворота камеры по Y
//float cameraDistance = 50; // Расстояние от камеры
float normal_vectors_scale = 100.;

bool smoothNormals = false; // Флаг для сглаженных нормалей
bool displaynormals = false; // Флаг для отображения нормалей
bool need_texture = false; // Флаг для текстуры
bool display_frame = true; //флаг для отображения каркаса
int currentProjection = 1; // 0 - Перспективная, 1 - Ортографическая
bool need_ligtning = false; // Флаг для освещения
bool display_grid = true;

GLuint textureID;

enum keys
{
   Empty,
   KeyDisplayFigure, KeyDisplayFrame, //режим каркаса/фигуры
   KeyDisplayNormalVectors, KeyNotDisplayNormalVectors, //отображение нормалей
   KeySmoothNormals, KeyPlainNormals,  //тип нормалей
   KeyOrtho, KeyPerspective,
   KeyDisplayTexture, KeyNotDisplayTexture,
   KeyEnableLighting, KeyDisableLighting, KeyDisableLightningSource, KeyEnableLightingSource,
   KeyDisplayGrid, KeyNotDisplayGrid
};

GLfloat light_position[] = { 0, 0, 1, 1 }; // Позиция источника света
GLfloat light_ambient[] = { 0.5, 0.5, 0.5, 1 }; // Уровень окружающего света
GLfloat light_diffuse[] = { 1, 1, 1, 1 }; // Рассеянный свет
GLfloat light_specular[] = { 1, 1, 1, 1 }; // Спекулятивный свет

//
//GLfloat ColorR = 0, ColorG = 0, ColorB = 0;
//
/////GLushort Width = 512, Height = 512;
//
//GLubyte LineWidth = 5;
//
//GLubyte PointSize = 5;
//
//GLushort X_Min = 0, Y_Min = 0;
//
//GLushort X_Max = 1920, Y_Max = 1080;


//enum keys      //клавиши
//{
//   Empty, KeyR, KeyG, KeyB, KeyW, KeyA, KeyS, KeyD, KeyDeletePolygon, KeyDeleteGroup, KeySpace, KeyP, KeyDeleteVertex, KeySavePosition, KeyResetPosition
//};

/* Пустая функция отрисовки */
struct Point      //точка
{
   GLushort x;
   GLushort y;
   Point() {}
   Point(GLushort x_, GLushort y_) : x(x_), y(y_) {}
};

//struct Point3D      //точка
//{
//   double X;
//   double X;
//   double X;
//   GLushort y;
//   //Point() {}
//   Point3D()
//   //Point(GLushort x_, GLushort y_) : x(x_), y(y_) {}
//};

Point delta_r(0, 0);
   
struct Polygon    //многоугольник
{
   vector<Point> Vertices;
   Polygon() {};
};

struct Vector3D
{
   double X;
   double Y;
   double Z;
   Vector3D(double X_, double Y_, double Z_)
   {
      X = X_;
      Y = Y_;
      Z = Z_;
   }
   Vector3D() {};
   /*Point ToPoint()
   {
      GLushort newX = Vector2DCoordToPointCoord(X);
      GLushort newY = Vector2DCoordToPointCoord(Y);
      return Point(newX, newY);
   }*/
};

struct Polygon3D    //многоугольник
{
   vector<Vector3D> Vertices;
   Polygon3D() {};
};

//struct PolygonGroup  //группа полигонов
//{
//   vector<Polygon> Polygons;
//   GLubyte R;
//   GLubyte G;
//   GLubyte B;
//   Point Min_border;
//   Point Max_border;
//   PolygonGroup()
//      : R(ColorR), G(ColorG), B(ColorB), Min_border(Point((X_Max - X_Min)/2,(Y_Max - Y_Min)/2)), Max_border(Point((X_Max - X_Min) / 2, (Y_Max - Y_Min) / 2))
//   {
//      Polygons.resize(1);
//   };
//   void MoveAllVertices(GLshort x, GLshort y)
//   {
//      if (x < 0 && -x > Min_border.x)  //при слишком большом смещении корректируем его
//         x = X_Min - Min_border.x;
//      else if (x > 0 && Max_border.x > X_Max - x)
//         x = X_Max - Max_border.x;
//      if (y < 0 && -y > Min_border.y)
//         y = Y_Min - Min_border.y;
//      else if (y > 0 && Max_border.y > Y_Max - y)
//         y = Y_Max - Max_border.y;
//
//      delta_r.x += x;
//      delta_r.y += y;
//
//      Min_border.x += x;         //обновляем границы
//      Max_border.x += x;
//      Min_border.y += y;
//      Max_border.y += y;
//      for (int i = 0; i < Polygons.size(); i++)       //перемещаем точки
//      {
//         Polygon *CurPolygon = &Polygons[i];
//         for (int j = 0; j < CurPolygon->Vertices.size(); j++)
//         {
//            Point *CurVertex = &CurPolygon->Vertices[j];
//            CurVertex->x += x;
//            CurVertex->y += y;
//         }
//      }
//   }
//   void UpdateBorders(GLushort x, GLushort y)
//   {
//      if (x < Min_border.x)
//         Min_border.x = x;
//      else if(x > Min_border.x)
//         Max_border.x = x;
//      if (y < Min_border.y)
//         Min_border.y = y;
//      else if(y > Min_border.y)
//         Max_border.y = y;
//   }
//};

struct PolygonGroup3D  //группа полигонов
{
   vector<Polygon3D> Polygons;
   //float R;
   //float G;
   //float B;
   //Point Min_border;
   //Point Max_border;
   PolygonGroup3D()
      //: R(ColorR), G(ColorG), B(ColorB)
   {
      Polygons.resize(1);
   };
   //void MoveAllVertices(GLshort x, GLshort y)
   //{
   //   if (x < 0 && -x > Min_border.x)  //при слишком большом смещении корректируем его
   //      x = X_Min - Min_border.x;
   //   else if (x > 0 && Max_border.x > X_Max - x)
   //      x = X_Max - Max_border.x;
   //   if (y < 0 && -y > Min_border.y)
   //      y = Y_Min - Min_border.y;
   //   else if (y > 0 && Max_border.y > Y_Max - y)
   //      y = Y_Max - Max_border.y;

   //   delta_r.x += x;
   //   delta_r.y += y;

   //   Min_border.x += x;         //обновляем границы
   //   Max_border.x += x;
   //   Min_border.y += y;
   //   Max_border.y += y;
   //   for (int i = 0; i < Polygons.size(); i++)       //перемещаем точки
   //   {
   //      Polygon* CurPolygon = &Polygons[i];
   //      for (int j = 0; j < CurPolygon->Vertices.size(); j++)
   //      {
   //         Point* CurVertex = &CurPolygon->Vertices[j];
   //         CurVertex->x += x;
   //         CurVertex->y += y;
   //      }
   //   }
   //}
   /*void UpdateBorders(GLushort x, GLushort y)
   {
      if (x < Min_border.x)
         Min_border.x = x;
      else if (x > Min_border.x)
         Max_border.x = x;
      if (y < Min_border.y)
         Min_border.y = y;
      else if (y > Min_border.y)
         Max_border.y = y;
   }*/
};

GLushort Vector2DCoordToPointCoord(double X)
{
   GLushort x_new = 0;
   if (X >= 0 && X <= std::numeric_limits<unsigned short>::max() / 10)
      x_new = static_cast<unsigned short>(X*100);
   return x_new;
}

struct Vector2D
{
   double X;
   double Y;
   Vector2D(double X_, double Y_)
   {
      X = X_;
      Y = Y_;
   }
   Vector2D() {};
   Point ToPoint()
   {
      GLushort newX = Vector2DCoordToPointCoord(X);
      GLushort newY = Vector2DCoordToPointCoord(Y);
      return Point(newX, newY);
   }
};





struct Element
{
   vector<int> VertexNumbers;
};



//struct FiniteElementMesh
//{
//   vector<Vector2D> Vertices;
//   vector<Element> Elements;
//
//   FiniteElementMesh() {};
//
//   FiniteElementMesh(string VertexFileName, string ElementsFileName)
//   {
//      ReadVertices(VertexFileName);
//      ReadElements(ElementsFileName);
//   }
//
//   Polygon ElementToPolygon(int num)
//   {
//      Polygon polygon;
//      Element element = Elements[num];
//      int n = element.VertexNumbers.size();
//      polygon.Vertices.resize(n);
//      for (int i = 0; i < n; i++)
//      {
//         int point_num = element.VertexNumbers[i];
//         Vector2D vector_ = Vertices[point_num];
//         polygon.Vertices[i] = vector_.ToPoint();
//      }
//      return polygon;
//   }
//
//   PolygonGroup MeshToPolygonGroup()
//   {
//      PolygonGroup group;
//      int elements_num = Elements.size();
//      group.Polygons.resize(elements_num);
//      for (int i = 0; i < elements_num; i++)
//      {
//         //Element element = Elements[i];
//         group.Polygons[i] = ElementToPolygon(i);
//         
//      }
//      return group;
//   }
//
//
//
//private:
//   void ReadVertices(string VertexFileName)
//   {
//      fstream f;
//      f.open(VertexFileName, ios::in);
//      int n = 0;
//      f >> n;
//      Vertices.resize(n);
//      for (int i = 0; i < n; i++)
//      {
//         double X, Y;
//         f >> X;
//         f >> Y;
//         //X *= 10;
//         //Y *= 10;
//         Vertices[i] = Vector2D(X, Y);
//      }
//      f.close();
//   }
//   void ReadElements(string ElementsFileName)
//   {
//      fstream f;
//      f.open(ElementsFileName, ios::in);
//      int n = 0;
//      f >> n;
//      Elements.resize(n);
//      for (int i = 0; i < n; i++)
//      {
//         int n_vertices;
//         f >> n_vertices;
//         Elements[i].VertexNumbers.resize(n_vertices);
//         vector<int>& vertices_num = Elements[i].VertexNumbers;
//         for (int j = 0; j < n_vertices; j++)
//            f >> vertices_num[j];
//      }
//      f.close();
//   }
//};

struct FiniteElementMesh3D
{
   vector<Vector3D> Vertices;
   vector<Element> Elements;

   FiniteElementMesh3D() {};

   FiniteElementMesh3D(string VertexFileName, string ElementsFileName)
   {
      ReadVertices(VertexFileName);
      ReadElements(ElementsFileName);
   }

   Polygon3D ElementToPolygon(int num)
   {
      Polygon3D polygon;
      Element element = Elements[num];
      int n = element.VertexNumbers.size();
      polygon.Vertices.resize(n);
      for (int i = 0; i < n; i++)
      {
         int point_num = element.VertexNumbers[i];
         Vector3D vector_ = Vertices[point_num];
         polygon.Vertices[i] = vector_;
      }
      return polygon;
   }

   PolygonGroup3D MeshToPolygonGroup()
   {
      PolygonGroup3D group;
      int elements_num = Elements.size();
      group.Polygons.resize(elements_num);
      for (int i = 0; i < elements_num; i++)
      {
         //Element element = Elements[i];
         group.Polygons[i] = ElementToPolygon(i);

      }
      return group;
   }

private:
   void ReadVertices(string VertexFileName)
   {
      fstream f;
      f.open(VertexFileName, ios::in);
      int n = 0;
      f >> n;
      Vertices.resize(n);
      for (int i = 0; i < n; i++)
      {
         double X, Y, Z;
         f >> X;
         f >> Y;
         f >> Z;
         //X *= 10;
         //Y *= 10;
         Vertices[i] = Vector3D(X, Y,Z);
      }
      f.close();
   }
   void ReadElements(string ElementsFileName)
   {
      fstream f;
      f.open(ElementsFileName, ios::in);
      int n = 0;
      f >> n;
      Elements.resize(n);
      for (int i = 0; i < n; i++)
      {
         int n_vertices;
         f >> n_vertices;
         Elements[i].VertexNumbers.resize(n_vertices);
         vector<int>& vertices_num = Elements[i].VertexNumbers;
         for (int j = 0; j < n_vertices; j++)
            f >> vertices_num[j];
      }
      f.close();
   }
};

vector<PolygonGroup3D> PolygonGroups3D;

//void ElementsToPolygons(FiniteElementMesh& mesh, vector<PolygonGroup>& polygonGroups)
//{
//   polygonGroups.resize(3);
//
//}

//void ReadVertices(string fileName )

//void Render()     //отрисовка полигонов
//{
   /*glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
   for (int i = 0; i < PolygonGroups.size(); i++)
   {
      PolygonGroup* CurPolygonGroup = &PolygonGroups[i];
      glColor3ub(CurPolygonGroup->R, CurPolygonGroup->G, CurPolygonGroup->B);
      for (int j = 0; j < CurPolygonGroup->Polygons.size(); j++)
      {
         Polygon* CurPolygon = &CurPolygonGroup->Polygons[j];
         glBegin(GL_POLYGON);
         for (int k = 0; k < CurPolygon->Vertices.size(); k++)
         {
            Point* curVertex = &CurPolygon->Vertices[k];
            glVertex2i(curVertex->x, curVertex->y);
         }
         glEnd();
      }
   }*/
   //glLineWidth(LineWidth);
   //int PolygonGroup_last = PolygonGroups.size() - 1;
   //PolygonGroup* CurPolygonGroup = &PolygonGroups[PolygonGroup_last];
   //int brightness = CurPolygonGroup->R + CurPolygonGroup->G + CurPolygonGroup->B;
   //if(brightness < 100)    
   //   glColor3ub(100, 100, 100);          //если цвет темный - граница светло-серая
   //else
   //   glColor3ub(0, 0, 0);                //если цвет - светлый - граница черная

   //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
   //for (int j = 0; j < CurPolygonGroup->Polygons.size(); j++)
   //{
   //   Polygon* CurPolygon = &CurPolygonGroup->Polygons[j];
   //   glBegin(GL_POLYGON);
   //   for (int k = 0; k < CurPolygon->Vertices.size(); k++)
   //   {
   //      Point* curVertex = &CurPolygon->Vertices[k];
   //      glVertex2i(curVertex->x, curVertex->y);
   //   }
   //   glEnd();
   //}
//}

/* Функция вывода на экран */
//void Display(void) 
//{
//   glClearColor(1, 1, 1, 1);
//   glClear(GL_COLOR_BUFFER_BIT);
//   Render();
//   glFinish();
//}
/* Функция изменения размеров окна */
//void Reshape(GLint w, GLint h) 
//{
//   Width = w, Height = h;
//   glViewport(0, 0, w, h);
//   glMatrixMode(GL_PROJECTION);
//   glLoadIdentity();
//   gluOrtho2D(0, w, 0, h);
//   glMatrixMode(GL_MODELVIEW);
//   glLoadIdentity();
//}

//void Mouse(int button, int state, int x, int y) 
//{
//   if (state != GLUT_DOWN) return; //зажатие ЛКМ
//
//   if (button == GLUT_LEFT_BUTTON) //ЛКМ
//   {
//      int PolygonGroup_last = PolygonGroups.size() -1;
//      PolygonGroup* curPolygonGroup = &PolygonGroups[PolygonGroup_last];
//      int Polygon_last = curPolygonGroup->Polygons.size() - 1;
//      Polygon* curPolygon = &curPolygonGroup->Polygons[Polygon_last];
//
//      GLushort x_ = x;
//      GLushort y_ = Height - y;
//
//      curPolygonGroup->UpdateBorders(x_, y_);
//
//      curPolygon->Vertices.push_back(Point(x_,y_)); //добавление точки
//   }
//
//   glutPostRedisplay();
//}

//void Keyboard(unsigned char Key, int x, int y)
//{
//   int PolygonGroup_last = PolygonGroups.size() - 1;
//   PolygonGroup* curPolygonGroup = &PolygonGroups[PolygonGroup_last];
//   //int group 
//   switch (Key)
//   {
//   case('r'): curPolygonGroup->R += 5; break;
//   case('g'): curPolygonGroup->G += 5; break;
//   case('b'): curPolygonGroup->B += 5; break;
//      /* Изменение XY-кординат точек */
//   case('w'): curPolygonGroup->MoveAllVertices(0, 5); break;
//   case('a'): curPolygonGroup->MoveAllVertices(-5, 0); break;
//   case('s'): curPolygonGroup->MoveAllVertices(0, -5); break;
//   case('d'): curPolygonGroup->MoveAllVertices(5, 0); break;
//   case(' '): PolygonGroups.push_back(PolygonGroup()); break;         //создание новой группы
//   case('p'): curPolygonGroup->Polygons.push_back(Polygon()); break;      //создание нового многоугольника
//   }
//   glutPostRedisplay();
//}

//void DeleteVertex()
//{
//   int PolygonGroup_last = PolygonGroups.size() - 1;
//   PolygonGroup* curPolygonGroup = &PolygonGroups[PolygonGroup_last];
//   int Polygon_last = curPolygonGroup->Polygons.size() - 1;
//   Polygon* curPolygon = &curPolygonGroup->Polygons[Polygon_last];
//   if(!curPolygon->Vertices.empty())
//      curPolygon->Vertices.pop_back();
//}
//
//void DeleteGroup()
//{
//   PolygonGroups.pop_back();
//   if (PolygonGroups.empty())
//      PolygonGroups.push_back(PolygonGroup());
//}
//
//void DeletePolygon()
//{
//   int PolygonGroup_last = PolygonGroups.size() - 1;
//   PolygonGroup* curPolygonGroup = &PolygonGroups[PolygonGroup_last];
//   curPolygonGroup->Polygons.pop_back();
//   if (curPolygonGroup->Polygons.empty())
//      curPolygonGroup->Polygons.push_back(Polygon());
//}


void EnableLighting() {
   glEnable(GL_LIGHTING);
   int par[] = { 0,0,0,0 };
   glLightModeliv(GL_LIGHT_MODEL_AMBIENT, par);
   //glLightModelfv(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
} // Включаем освещение}

void EnableLightingSource1() {
   glEnable(GL_LIGHT0); // Включаем первый источник света

   // Устанавливаем параметры света
   //GLfloat light_position[] = { 0, 10, 10, 1 }; // Позиция источника света
   //GLfloat light_ambient[] = { 0.2, 0.2, 0.2, 1 }; // Уровень окружающего света
   //GLfloat light_diffuse[] = { 1, 1, 1, 1 }; // Рассеянный свет
   //GLfloat light_specular[] = { 1, 1, 1, 1 }; // Спекулятивный свет

   glLightfv(GL_LIGHT0, GL_POSITION, light_position);
   glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
   glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
   glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
   glEnable(GL_COLOR_MATERIAL); // Включаем использование материала
   GLfloat material_diffuse[] = { 1, 1, 1, 1 }; // Цвет материала
   //glEnable(GL_COLOR_MATERIAL); // Включаем использование материала
   glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, material_diffuse);

}

void DisableLightingSource1()
{
   glDisable(GL_LIGHT0);
   //glDisable(GL_AMBIENT);
   glDisable(GL_COLOR_MATERIAL);
   //glColor3ub(0, 0, 0);
}

void DisableLighting()
{
   glDisable(GL_LIGHTING);
}

void DisplayGrid()
{
   glNormal3f(0, 0, 0);
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

FiniteElementMesh3D mesh("vertex3D1.txt", "elements3D1.txt");

void DisplayFrame()
{
   //glNormal3f(0, 0, 0);
   //glColor3ub(0, 255, 0); //цвет отображения каркаса
   //for (int i = 0; i < ReplicationPath.size() - 1; i++)
   //{
   //   glBegin(GL_LINE_LOOP);
   //   for (int j = 0; j < 5; j++) //рисуем каркас сечения
   //   {
   //      Point3DFloat Vertex = Points[i].Points[j];
   //      glVertex3f(Vertex.x, Vertex.y, Vertex.z);
   //   }
   //   glEnd();
   //   glBegin(GL_LINES);
   //   for (int j = 0; j < 5; j++) //рисуем каркас сечения
   //   {
   //      Point3DFloat Vertex1 = Points[i].Points[j];
   //      Point3DFloat Vertex2 = Points[i + 1].Points[j];
   //      glVertex3f(Vertex1.x, Vertex1.y, Vertex1.z);
   //      glVertex3f(Vertex2.x, Vertex2.y, Vertex2.z);
   //   }
   //   glEnd();
   //}
   //glBegin(GL_LINE_LOOP);
   //int NumLastSection = Points.size() - 1;
   //for (int i = 0; i < 5; i++) //рисуем каркас последнего сечения
   //{
   //   Point3DFloat Vertex = Points[NumLastSection].Points[i];
   //   glVertex3f(Vertex.x, Vertex.y, Vertex.z);
   //}
   //glEnd();

   //glNormal3f(0, 0, 0);
   glColor3ub(0, 255, 0); //цвет отображения каркаса
   glLineWidth(10);
   int PolygonGroup_last = PolygonGroups3D.size() - 1;
   PolygonGroup3D* CurPolygonGroup = &PolygonGroups3D[PolygonGroup_last];
   //int brightness = CurPolygonGroup->R + CurPolygonGroup->G + CurPolygonGroup->B;
   //if (brightness < 100)
   //   glColor3ub(100, 100, 100);          //если цвет темный - граница светло-серая
   //else
      //glColor3ub(0, 0, 0);                //если цвет - светлый - граница черная

   //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
   for (int j = 0; j < CurPolygonGroup->Polygons.size(); j++)
   {
      glColor3ub(0, 255, 0);
      Polygon3D* CurPolygon = &CurPolygonGroup->Polygons[j];      //отрисовка тетраэдра
      glBegin(GL_LINE_LOOP);
      
      glVertex3d(CurPolygon->Vertices[0].X, CurPolygon->Vertices[0].Y, CurPolygon->Vertices[0].Z);
      glVertex3d(CurPolygon->Vertices[1].X, CurPolygon->Vertices[1].Y, CurPolygon->Vertices[1].Z);
      glVertex3d(CurPolygon->Vertices[2].X, CurPolygon->Vertices[2].Y, CurPolygon->Vertices[2].Z);

      glEnd();

      glBegin(GL_LINES);

      glVertex3d(CurPolygon->Vertices[0].X, CurPolygon->Vertices[0].Y, CurPolygon->Vertices[0].Z);
      glVertex3d(CurPolygon->Vertices[3].X, CurPolygon->Vertices[3].Y, CurPolygon->Vertices[3].Z);

      glVertex3d(CurPolygon->Vertices[1].X, CurPolygon->Vertices[1].Y, CurPolygon->Vertices[1].Z);
      glVertex3d(CurPolygon->Vertices[3].X, CurPolygon->Vertices[3].Y, CurPolygon->Vertices[3].Z);

      glVertex3d(CurPolygon->Vertices[2].X, CurPolygon->Vertices[2].Y, CurPolygon->Vertices[2].Z);
      glVertex3d(CurPolygon->Vertices[3].X, CurPolygon->Vertices[3].Y, CurPolygon->Vertices[3].Z);

      glEnd();

      
      
      //glEnd();
   }

   glPointSize(15.0f);
   glColor3ub(255, 0, 255); //цвет отображения точек
   glBegin(GL_POINTS);

   for (int i = 0; i < mesh.Vertices.size(); i++)
   {
      glVertex3d(mesh.Vertices[i].X, mesh.Vertices[i].Y, mesh.Vertices[i].Z);
   }

   glEnd();

   //glPointSize(15.0f);
   //glColor3ub(255, 0, 255); //цвет отображения точек
   //glBegin(GL_POINTS);

   //for (int i = 0; i < CurPolygon->Vertices.size(); i++)
   //   glVertex3d(CurPolygon->Vertices[0].X, CurPolygon->Vertices[0].Y, CurPolygon->Vertices[0].Z);
}

void Display(void)
{
   glClearColor(0, 0, 0, 1);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   //glLoadIdentity();

   //glLoadIdentity();
   gluLookAt(0., 0., 0., 0., 1., 0., 0., 0., 1.);
   glTranslatef(CameraStepX, CameraStepY, CameraStepZ);
   glRotatef(-CameraAngleX, 0, 0, 1);
   glRotatef(CameraAngleY, 1, 0, 0);
   /*glRotatef(cameraAngleX, 1, 0, 0);
   glRotatef(cameraAngleY, 0, 1, 0);*/

   if (display_grid)
      DisplayGrid();

   if (display_frame)
      DisplayFrame();
   else
   {
      //DisplaySections();
      //DisplayQuads();
   }
   if (displaynormals)
      //DisplayNormalVectors();
   //glLoadIdentity();
   glFinish();
}

void Reshape(GLint w, GLint h)
{
   Width = w; Height = h;
   glViewport(0, 0, w, h);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   if (currentProjection == 0) {
      // Перспективная проекция
      // 45, w / h, 0.1, 1000000
      gluPerspective(45, w / h, 0.1, 1000000);
      //glFrustum(-w / 2, w / 2, -h / 2, h / 2, -1000, 1000);
   }
   else {
      // Ортографическая проекция
      glOrtho(-w / 2, w / 2, -h / 2, h / 2, -1000, 1000);
   }
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
}

void HandleKeyboard(unsigned char key, int x, int y) {
   switch (key) {
   case 'p':
      currentProjection = 1 - currentProjection; // Переключение проекции
      Reshape(Width, Height); // Вызов перерисовки окна
      break;
   case 'n':
      smoothNormals = !smoothNormals; // Переключение режима нормалей
      break;
   case 'w':
      CameraStepY -= 5; // вперед
      break;
   case 's':
      CameraStepY += 5; // назад
      break;
   case 'a':
      CameraStepX += 5; // влево
      break;
   case 'd':
      CameraStepX -= 5; // вправо
      break;
   case 'q':
      CameraStepZ += 5; //вниз
      break;
   case 'e':
      CameraStepZ -= 5; //вверх
      break;
      //case 'l':
      //   cameraDistance -= 100; // передвижение камеры
      //   break;
      //case 'k':
      //   cameraDistance += 100; // передвижение камеры
      //   break;
   }
   glutPostRedisplay(); // принудительный (досрочный) вызов функции обновления окна
}

float LKM_PositionX = 0.;
float LKM_PositionY = 0.;

void Mouse(int button, int state, int x, int y)
{
   ///* клавиша была нажата, но не отпущена */
   if (state == GLUT_LEFT_BUTTON)
   {
      CameraAngleX -= (int)(CameraAngleX / 360) * 360.;
      CameraAngleY -= (int)(CameraAngleY / 360) * 360.;
      LKM_PositionX = x;
      LKM_PositionY = Height - y;
   }
   //else if (state == GLUT_UP)
   //{
   //   LKM_PositionX = 0.;
   //   LKM_PositionY = 0.;
   //}
   ///* новая точка по левому клику */
   ////glutPostRedisplay();
}

void MouseMotion(int x, int y)
{
   y = Height - y;
   CameraAngleX += (x - LKM_PositionX);
   CameraAngleY += (y - LKM_PositionY);
   LKM_PositionX = x;
   LKM_PositionY = y;
   glutPostRedisplay();
}

void Menu(int pos)
{
   int key = (keys)pos;

   switch (key)
   {
   case(KeyDisplayFigure): display_frame = false; glutChangeToMenuEntry(1, "отображать каркас", KeyDisplayFrame); break;
   case(KeyDisplayFrame): display_frame = true; glutChangeToMenuEntry(1, "отображать фигуру", KeyDisplayFigure); break;
   case(KeyNotDisplayNormalVectors): displaynormals = false; glutChangeToMenuEntry(2, "включить отображение нормалей", KeyDisplayNormalVectors); break;
   case(KeyDisplayNormalVectors): displaynormals = true; glutChangeToMenuEntry(2, "отключить отображение нормалей", KeyNotDisplayNormalVectors); break;
   case(KeyPlainNormals): smoothNormals = false; glutChangeToMenuEntry(3, "включить сглаживание нормалей", KeySmoothNormals); break;
   case(KeySmoothNormals): smoothNormals = true; glutChangeToMenuEntry(3, "отключить сглаживание нормалей", KeyPlainNormals); break;
   case(KeyPerspective): HandleKeyboard('p', 0, 0); glutChangeToMenuEntry(4, "включить режим ортографической проекции", KeyOrtho); break;
   case(KeyOrtho): HandleKeyboard('p', 0, 0); glutChangeToMenuEntry(4, "включить режим перспективной проекции", KeyPerspective); break;
   case(KeyNotDisplayTexture): need_texture = false; glutChangeToMenuEntry(5, "включить отображение текстуры", KeyDisplayTexture); break;
   case(KeyDisplayTexture): need_texture = true; glutChangeToMenuEntry(5, "отключить отображение текстуры", KeyNotDisplayTexture); break;
   case(KeyNotDisplayGrid): display_grid = false; glutChangeToMenuEntry(6, "включить отображение сетки", KeyDisplayGrid); break;
   case(KeyDisplayGrid): display_grid = true; glutChangeToMenuEntry(6, "отключить отображение сетки", KeyNotDisplayGrid); break;
      //case(KeyEnableLighting): need_ligtning = true; glutChangeToMenuEntry(6, "включить освещение", KeyDisableLighting); break;
   case(KeyEnableLighting): EnableLighting(); break;
   case(KeyEnableLightingSource): EnableLightingSource1(); break;
   case(KeyDisableLighting): DisableLighting(); break;
   case(KeyDisableLightningSource): DisableLightingSource1(); break;
   default:
      int menu_Light = glutCreateMenu(Menu); // создание меню
      glutAddMenuEntry("Выключить весь свет", KeyDisableLighting); // добавление в меню пунктов 
      glutAddMenuEntry("Включить расчет света", KeyEnableLighting);
      glutAddMenuEntry("Выключить один источник света", KeyDisableLightningSource);
      glutAddMenuEntry("Включить один источник света", KeyEnableLightingSource);

      int menu = glutCreateMenu(Menu);
      if (display_frame)   //пункт меню для отображения фигуры/каркаса
         glutAddMenuEntry("отображать фигуру", KeyDisplayFigure);
      else
         glutAddMenuEntry("отображать каркас", KeyDisplayFrame);
      if (displaynormals)  //пункт меню для отображения нормалей
         glutAddMenuEntry("отключить отображение нормалей", KeyNotDisplayNormalVectors);
      else
         glutAddMenuEntry("включить отображение нормалей", KeyDisplayNormalVectors);
      if (smoothNormals)   //пункт меню для режима нормалей
         glutAddMenuEntry("отключить сглаживание нормалей", KeyPlainNormals);
      else
         glutAddMenuEntry("включить сглаживание нормалей", KeySmoothNormals);
      if (currentProjection == 1) //пункт для выбора режима проекции
         glutAddMenuEntry("включить режим перспективной проекции", KeyPerspective);
      else
         glutAddMenuEntry("включить режим ортографической проекции", KeyOrtho);
      if (need_texture)
         glutAddMenuEntry("отключить отображение текстуры", KeyNotDisplayTexture);
      else
         glutAddMenuEntry("включить отображение текстуры", KeyDisplayTexture);
      if (display_grid)
         glutAddMenuEntry("отключить отображение сетки", KeyNotDisplayGrid);
      else
         glutAddMenuEntry("включить отображение сетки", KeyDisplayGrid);

      glutAddSubMenu("Включение/отключение света", menu_Light); // добавление в меню подменю

      glutAttachMenu(GLUT_RIGHT_BUTTON);
   }
   glutPostRedisplay();
}



void main(int argc, char* argv[])
{
   PolygonGroups3D.resize(1);

   

   //Points.resize(ReplicationPath.size());
   glutInit(&argc, argv);
   glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH);
   glutInitWindowSize(Width, Height);
   glutCreateWindow("Тиражирование сечений");

   //InitTextures();
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();

   //FirstSection();
   ////Sections();
  // LastSection();
  // NormalVectorsInitialize();
   //SmoothNormalVectorsInitialize();

   //FiniteElementMesh3D mesh("vertex3D1.txt", "elements3D1.txt");

   PolygonGroups3D[0] = mesh.MeshToPolygonGroup();

   Menu(Empty);
   glLoadIdentity();

   glutDisplayFunc(Display);
   glutReshapeFunc(Reshape);
   glutKeyboardFunc(HandleKeyboard);
   glutMouseFunc(Mouse);
   glutMotionFunc(MouseMotion);
   glEnable(GL_DEPTH_TEST);

   // Включение освещения


   glutMainLoop();
}



//void Menu(int pos)
//{
//   int key = (keys)pos;
//   switch (key)
//   {
//      case(KeyR): Keyboard('r', 0, 0); break;
//      case(KeyG): Keyboard('g', 0, 0); break;
//      case(KeyB): Keyboard('b', 0, 0); break;
//      case(KeyW): Keyboard('w', 0, 0); break;
//      case(KeyA): Keyboard('a', 0, 0); break;
//      case(KeyS): Keyboard('s', 0, 0); break;
//      case(KeyD): Keyboard('d', 0, 0); break;
//      case(KeySpace): Keyboard(' ', 0, 0); break;
//      case(KeyP): Keyboard('p', 0, 0); break;
//      case(KeyDeleteVertex): DeleteVertex(); break;
//      case(KeyDeleteGroup): DeleteGroup(); delta_r = Point(0, 0); break;  //удаление группы
//      case(KeyDeletePolygon): DeletePolygon(); delta_r = Point(0, 0); break;   //удаление многоугольника
//      case(KeySavePosition): delta_r = Point(0, 0); break;
//      case(KeyResetPosition):
//      {
//         int PolygonGroup_last = PolygonGroups.size() - 1;
//         PolygonGroup* curPolygonGroup = &PolygonGroups[PolygonGroup_last];
//         curPolygonGroup->MoveAllVertices(-delta_r.x, -delta_r.y);
//         delta_r = Point(0, 0);
//         break;
//      }
//      default:
//         int menu_RGB = glutCreateMenu(Menu);   
//         glutAddMenuEntry("компонента R++", KeyR);
//         glutAddMenuEntry("компонента G++", KeyG);
//         glutAddMenuEntry("компонента B++", KeyB);
//
//         int menu_move = glutCreateMenu(Menu);
//         glutAddMenuEntry("вверх на 5 px", KeyW);
//         glutAddMenuEntry("влево на 5 px", KeyA);
//         glutAddMenuEntry("вниз на 5 px", KeyS);
//         glutAddMenuEntry("вправо на 5 px", KeyD);
//
//         int menu_delete = glutCreateMenu(Menu);
//         glutAddMenuEntry("последнюю вершину", KeyDeleteVertex);
//         glutAddMenuEntry("последний полигон", KeyDeletePolygon);
//         glutAddMenuEntry("последнюю группу полигонов", KeyDeleteGroup);
//         
//
//         int menu = glutCreateMenu(Menu);
//         glutAddSubMenu("Смена цвета", menu_RGB);
//         glutAddSubMenu("Перемещение", menu_move);
//         glutAddSubMenu("Удалить", menu_delete);
//
//         glutAddMenuEntry("Сохранить текущую позицию группы полигонов", KeySavePosition);
//         glutAddMenuEntry("Отменить перемещение группы полигонов", KeyResetPosition);
//
//         glutAttachMenu(GLUT_RIGHT_BUTTON);
//         Keyboard(Empty, 0, 0);
//   }
//   glutPostRedisplay();
//}

//void Start(int )

/* Головная программа */
//void main(int argc, char* argv[])
//{
//   glutInit(&argc, argv);
//   glutInitDisplayMode(GLUT_RGB);
//   glutInitWindowSize(Width, Height);
//   glutCreateWindow("Простейшее приложение");
//
//   PolygonGroups.resize(1);
//
//   FiniteElementMesh mesh("vertex.txt", "elements.txt");
//
//   PolygonGroups[0] = mesh.MeshToPolygonGroup();
//   
//   Menu(Empty);
//   glutDisplayFunc(Display);
//   glutReshapeFunc(Reshape);
//   glutKeyboardFunc(Keyboard);
//   glutMouseFunc(Mouse);
//
//   glutMainLoop();
//}