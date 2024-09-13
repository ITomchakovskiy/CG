#include <vector>

#include "glut.h"


GLubyte ColorR = 0, ColorG = 0, ColorB = 0;

GLushort Width = 512, Height = 512;


GLubyte PointSize = 5;

enum keys      //клавиши
{
   Empty, KeyR, KeyG, KeyB, KeyW, KeyA, KeyS, KeyD, KeyDeletePolygon, KeyDeleteGroup, KeySpace, KeyP
};


/* Пустая функция отрисовки */
struct Point      //точка
{
   GLushort x;
   GLushort y;
   Point() {}
   Point(GLushort x_, GLushort y_) : x(x_), y(y_) {}
  // GLubyte R;
 //  GLubyte G;
  // GLubyte B;
};
   
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
   PolygonGroup()
      : R(ColorR), G(ColorG), B(ColorB)
   {
      Polygons.resize(1);
      
   };
   void MoveAllVertices(short x, short y) 
   {
      for (int i = 0; i < Polygons.size(); i++)
      {
         for (int j = 0; j < Polygons[i].Vertices.size(); j++)
         {
            Polygons[i].Vertices[j].x += x;
            Polygons[i].Vertices[j].y += y;
         }
      }
   }
};

std::vector<PolygonGroup> PolygonGroups;

void Render()     //отрисовка полигонов
{
   glEnableClientState(GL_VERTEX_ARRAY);
   for (int i = 0; i < PolygonGroups.size(); i++)
   {
      glColor3ub(PolygonGroups[i].R, PolygonGroups[i].G, PolygonGroups[i].B);
      for (int j = 0; j < PolygonGroups[i].Polygons.size(); j++)
      {
         glBegin(GL_POLYGON);
            //glVertexPointer(2, GL_UNSIGNED_SHORT, sizeof(Point), &PolygonGroups[i].Polygons[j].Vertices[0].x);
            //glDrawArrays(GL_POLYGON, 0, PolygonGroups[i].Polygons[j].Vertices.size());
         for (int k = 0; k < PolygonGroups[i].Polygons[j].Vertices.size(); k++)
         {
            glVertex2i(PolygonGroups[i].Polygons[j].Vertices[k].x, PolygonGroups[i].Polygons[j].Vertices[k].y);
         }
         glEnd();
      }
   }
   glDisableClientState(GL_VERTEX_ARRAY);
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
      int Polygon_last = PolygonGroups[PolygonGroup_last].Polygons.size() - 1;

      PolygonGroups[PolygonGroup_last].Polygons[Polygon_last].Vertices.push_back(Point(x,Height - y)); //добавление точки
   }

   glutPostRedisplay();
}

void Keyboard(unsigned char Key, int x, int y)    
{
   int group_num = PolygonGroups.size() - 1; 
   //int group 
   switch (Key)
   {
   case('r'): PolygonGroups[group_num].R += 5; break; 
   case('g'): PolygonGroups[group_num].G += 5; break;
   case('b'): PolygonGroups[group_num].B += 5; break;
      /* Изменение XY-кординат точек */
   case('w'): PolygonGroups[group_num].MoveAllVertices(0, 5); break;
   case('a'): PolygonGroups[group_num].MoveAllVertices(-5, 0); break;
   case('s'): PolygonGroups[group_num].MoveAllVertices(0, -5); break;
   case('d'): PolygonGroups[group_num].MoveAllVertices(5, 0); break;
   case(' '): PolygonGroups.push_back(PolygonGroup());         //создание новой группы
   case('p'): PolygonGroups[group_num].Polygons.push_back(Polygon());      //создание нового многоугольника
   }
   glutPostRedisplay();
}

//void CreateMenu()
//{
//   int menu_RGB = glutCreateMenu(Menu);
//   glutAddMenuEntry("R", KeyR);
//   glutAddMenuEntry("G", KeyG);
//   glutAddMenuEntry("B", KeyB);
//
//   int menu_move = glutCreateMenu(Menu);
//   glutAddMenuEntry("вверх", KeyW);
//   glutAddMenuEntry("влево", KeyA);
//   glutAddMenuEntry("вниз", KeyS);
//   glutAddMenuEntry("вправо", KeyD);
//
//   int menu_delete = glutCreateMenu(Menu);
//   glutAddMenuEntry("полигон", KeyDeletePolygon);
//   glutAddMenuEntry("группа полигонов", KeyDeleteGroup);
//
//   int menu = glutCreateMenu(Menu);
//   glutAddSubMenu("Смена цвета", menu_RGB);
//   glutAddSubMenu("Перемещение", menu_move);
//   glutAddSubMenu("Удаление", menu_move);
//
//   glutAttachMenu(GLUT_RIGHT_BUTTON);
//   Keyboard(Empty, 0, 0);
//}

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
      case(KeyDeleteGroup): break;  //удаление группы
      case(KeyDeletePolygon): break;   //удаление многоугольника
      default:
         int menu_RGB = glutCreateMenu(Menu);   
         glutAddMenuEntry("R", KeyR);
         glutAddMenuEntry("G", KeyG);
         glutAddMenuEntry("B", KeyB);

         int menu_move = glutCreateMenu(Menu);
         glutAddMenuEntry("вверх", KeyW);
         glutAddMenuEntry("влево", KeyA);
         glutAddMenuEntry("вниз", KeyS);
         glutAddMenuEntry("вправо", KeyD);

         int menu_delete = glutCreateMenu(Menu);
         glutAddMenuEntry("полигон", KeyDeletePolygon);
         glutAddMenuEntry("группа полигонов", KeyDeleteGroup);

         int menu = glutCreateMenu(Menu);
         glutAddSubMenu("Смена цвета", menu_RGB);
         glutAddSubMenu("Перемещение", menu_move);
         glutAddSubMenu("Удаление", menu_delete);

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
