#include <vector>

#include "glut.h"


GLubyte ColorR = 0, ColorG = 0, ColorB = 0;

GLushort Width = 512, Height = 512;

GLubyte LineWidth = 5;

GLubyte PointSize = 5;

GLushort X_Min = 0, Y_Min = 0;

GLushort X_Max = 1920, Y_Max = 1080;


enum keys      //клавиши
{
   Empty, KeyR, KeyG, KeyB, KeyW, KeyA, KeyS, KeyD, KeyDeletePolygon, KeyDeleteGroup, KeySpace, KeyP, KeyDeleteVertex, KeySavePosition, KeyResetPosition
};

/* Пустая функция отрисовки */
struct Point      //точка
{
   GLushort x;
   GLushort y;
   Point() {}
   Point(GLushort x_, GLushort y_) : x(x_), y(y_) {}
};

Point delta_r(0, 0);
   
struct Polygon    //многоугольник
{
   std::vector<Point> Vertices;
   Polygon() {};
};

struct PolygonGroup  //группа полигонов
{
   std::vector<Polygon> Polygons;
   GLubyte R;
   GLubyte G;
   GLubyte B;
   Point Min_border;
   Point Max_border;
   PolygonGroup()
      : R(ColorR), G(ColorG), B(ColorB), Min_border(Point((X_Max - X_Min)/2,(Y_Max - Y_Min)/2)), Max_border(Point((X_Max - X_Min) / 2, (Y_Max - Y_Min) / 2))
   {
      Polygons.resize(1);
   };
   void MoveAllVertices(GLshort x, GLshort y)
   {
      if (x < 0 && -x > Min_border.x)  //при слишком большом смещении корректируем его
         x = X_Min - Min_border.x;
      else if (x > 0 && Max_border.x > X_Max - x)
         x = X_Max - Max_border.x;
      if (y < 0 && -y > Min_border.y)
         y = Y_Min - Min_border.y;
      else if (y > 0 && Max_border.y > Y_Max - y)
         y = Y_Max - Max_border.y;

      delta_r.x += x;
      delta_r.y += y;

      Min_border.x += x;         //обновляем границы
      Max_border.x += x;
      Min_border.y += y;
      Max_border.y += y;
      for (int i = 0; i < Polygons.size(); i++)       //перемещаем точки
      {
         Polygon *CurPolygon = &Polygons[i];
         for (int j = 0; j < CurPolygon->Vertices.size(); j++)
         {
            Point *CurVertex = &CurPolygon->Vertices[j];
            CurVertex->x += x;
            CurVertex->y += y;
         }
      }
   }
   void UpdateBorders(GLushort x, GLushort y)
   {
      if (x < Min_border.x)
         Min_border.x = x;
      else if(x > Min_border.x)
         Max_border.x = x;
      if (y < Min_border.y)
         Min_border.y = y;
      else if(y > Min_border.y)
         Max_border.y = y;
   }
};

std::vector<PolygonGroup> PolygonGroups;

void Render()     //отрисовка полигонов
{
   glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
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
   }
   glLineWidth(LineWidth);
   int PolygonGroup_last = PolygonGroups.size() - 1;
   PolygonGroup* CurPolygonGroup = &PolygonGroups[PolygonGroup_last];
   int brightness = CurPolygonGroup->R + CurPolygonGroup->G + CurPolygonGroup->B;
   if(brightness < 100)    
      glColor3ub(100, 100, 100);          //если цвет темный - граница светло-серая
   else
      glColor3ub(0, 0, 0);                //если цвет - светлый - граница черная

   glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
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
}

/* Функция вывода на экран */
void Display(void) 
{
   glClearColor(1, 1, 1, 1);
   glClear(GL_COLOR_BUFFER_BIT);
   Render();
   glFinish();
}
/* Функция изменения размеров окна */
void Reshape(GLint w, GLint h) 
{
   Width = w, Height = h;
   glViewport(0, 0, w, h);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluOrtho2D(0, w, 0, h);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
}

void Mouse(int button, int state, int x, int y) 
{
   if (state != GLUT_DOWN) return; //зажатие ЛКМ

   if (button == GLUT_LEFT_BUTTON) //ЛКМ
   {
      int PolygonGroup_last = PolygonGroups.size() -1;
      PolygonGroup* curPolygonGroup = &PolygonGroups[PolygonGroup_last];
      int Polygon_last = curPolygonGroup->Polygons.size() - 1;
      Polygon* curPolygon = &curPolygonGroup->Polygons[Polygon_last];

      GLushort x_ = x;
      GLushort y_ = Height - y;

      curPolygonGroup->UpdateBorders(x_, y_);

      curPolygon->Vertices.push_back(Point(x_,y_)); //добавление точки
   }

   glutPostRedisplay();
}

void Keyboard(unsigned char Key, int x, int y)
{
   int PolygonGroup_last = PolygonGroups.size() - 1;
   PolygonGroup* curPolygonGroup = &PolygonGroups[PolygonGroup_last];
   //int group 
   switch (Key)
   {
   case('r'): curPolygonGroup->R += 5; break;
   case('g'): curPolygonGroup->G += 5; break;
   case('b'): curPolygonGroup->B += 5; break;
      /* Изменение XY-кординат точек */
   case('w'): curPolygonGroup->MoveAllVertices(0, 5); break;
   case('a'): curPolygonGroup->MoveAllVertices(-5, 0); break;
   case('s'): curPolygonGroup->MoveAllVertices(0, -5); break;
   case('d'): curPolygonGroup->MoveAllVertices(5, 0); break;
   case(' '): PolygonGroups.push_back(PolygonGroup()); break;         //создание новой группы
   case('p'): curPolygonGroup->Polygons.push_back(Polygon()); break;      //создание нового многоугольника
   }
   glutPostRedisplay();
}

void DeleteVertex()
{
   int PolygonGroup_last = PolygonGroups.size() - 1;
   PolygonGroup* curPolygonGroup = &PolygonGroups[PolygonGroup_last];
   int Polygon_last = curPolygonGroup->Polygons.size() - 1;
   Polygon* curPolygon = &curPolygonGroup->Polygons[Polygon_last];
   if(!curPolygon->Vertices.empty())
      curPolygon->Vertices.pop_back();
}

void DeleteGroup()
{
   PolygonGroups.pop_back();
   if (PolygonGroups.empty())
      PolygonGroups.push_back(PolygonGroup());
}

void DeletePolygon()
{
   int PolygonGroup_last = PolygonGroups.size() - 1;
   PolygonGroup* curPolygonGroup = &PolygonGroups[PolygonGroup_last];
   curPolygonGroup->Polygons.pop_back();
   if (curPolygonGroup->Polygons.empty())
      curPolygonGroup->Polygons.push_back(Polygon());
}



void Menu(int pos)
{
   int key = (keys)pos;
   switch (key)
   {
      case(KeyR): Keyboard('r', 0, 0); break;
      case(KeyG): Keyboard('g', 0, 0); break;
      case(KeyB): Keyboard('b', 0, 0); break;
      case(KeyW): Keyboard('w', 0, 0); break;
      case(KeyA): Keyboard('a', 0, 0); break;
      case(KeyS): Keyboard('s', 0, 0); break;
      case(KeyD): Keyboard('d', 0, 0); break;
      case(KeySpace): Keyboard(' ', 0, 0); break;
      case(KeyP): Keyboard('p', 0, 0); break;
      case(KeyDeleteVertex): DeleteVertex(); break;
      case(KeyDeleteGroup): DeleteGroup(); delta_r = Point(0, 0); break;  //удаление группы
      case(KeyDeletePolygon): DeletePolygon(); delta_r = Point(0, 0); break;   //удаление многоугольника
      case(KeySavePosition): delta_r = Point(0, 0); break;
      case(KeyResetPosition):
      {
         int PolygonGroup_last = PolygonGroups.size() - 1;
         PolygonGroup* curPolygonGroup = &PolygonGroups[PolygonGroup_last];
         curPolygonGroup->MoveAllVertices(-delta_r.x, -delta_r.y);
         delta_r = Point(0, 0);
         break;
      }
      default:
         int menu_RGB = glutCreateMenu(Menu);   
         glutAddMenuEntry("компонента R++", KeyR);
         glutAddMenuEntry("компонента G++", KeyG);
         glutAddMenuEntry("компонента B++", KeyB);

         int menu_move = glutCreateMenu(Menu);
         glutAddMenuEntry("вверх на 5 px", KeyW);
         glutAddMenuEntry("влево на 5 px", KeyA);
         glutAddMenuEntry("вниз на 5 px", KeyS);
         glutAddMenuEntry("вправо на 5 px", KeyD);

         int menu_delete = glutCreateMenu(Menu);
         glutAddMenuEntry("последнюю вершину", KeyDeleteVertex);
         glutAddMenuEntry("последний полигон", KeyDeletePolygon);
         glutAddMenuEntry("последнюю группу полигонов", KeyDeleteGroup);
         

         int menu = glutCreateMenu(Menu);
         glutAddSubMenu("Смена цвета", menu_RGB);
         glutAddSubMenu("Перемещение", menu_move);
         glutAddSubMenu("Удалить", menu_delete);

         glutAddMenuEntry("Сохранить текущую позицию группы полигонов", KeySavePosition);
         glutAddMenuEntry("Отменить перемещение группы полигонов", KeyResetPosition);

         glutAttachMenu(GLUT_RIGHT_BUTTON);
         Keyboard(Empty, 0, 0);
   }
   glutPostRedisplay();
}

/* Головная программа */
void main(int argc, char* argv[])
{
   glutInit(&argc, argv);
   glutInitDisplayMode(GLUT_RGB);
   glutInitWindowSize(Width, Height);
   glutCreateWindow("Простейшее приложение");

   PolygonGroups.resize(1);

   Menu(Empty);
   glutDisplayFunc(Display);
   glutReshapeFunc(Reshape);
   glutKeyboardFunc(Keyboard);
   glutMouseFunc(Mouse);

   glutMainLoop();
}