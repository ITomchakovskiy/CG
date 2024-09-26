#include <vector>

#include "glut.h"


GLubyte ColorR = 0, ColorG = 0, ColorB = 0;

GLushort Width = 512, Height = 512;

GLubyte LineWidth = 5;

GLubyte PointSize = 5;

enum keys      //�������
{
   Empty, KeyR, KeyG, KeyB, KeyW, KeyA, KeyS, KeyD, KeyDeletePolygon, KeyDeleteGroup, KeySpace, KeyP
};


/* ������ ������� ��������� */
struct Point      //�����
{
   GLushort x;
   GLushort y;
   Point() {}
   Point(GLushort x_, GLushort y_) : x(x_), y(y_) {}
  // GLubyte R;
 //  GLubyte G;
  // GLubyte B;
};
   
struct Polygon    //�������������
{
   std::vector<Point> Vertices;
   Polygon() {};
};

struct PolygonGroup  //������ ���������
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

void Render()     //��������� ���������
{
  // glEnableClientState(GL_VERTEX_ARRAY);
   glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
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
   glLineWidth(LineWidth);
   int i = PolygonGroups.size() - 1;
   int brightness = PolygonGroups[i].R + PolygonGroups[i].G + PolygonGroups[i].B;
   if(brightness < 100)    
      glColor3ub(100, 100, 100);          //���� ���� ������ - ������� ������-�����
   else
      glColor3ub(0, 0, 0);                //���� ���� - ������� - ������� ������
   //else
   //   glColor3ub(255 - PolygonGroups[i].R, 255 - PolygonGroups[i].G, 255 - PolygonGroups[i].B); //����� ������� ���������������� �����
   glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
   for (int j = 0; j < PolygonGroups[i].Polygons.size(); j++)
   {
      glBegin(GL_POLYGON);
      for (int k = 0; k < PolygonGroups[i].Polygons[j].Vertices.size(); k++)
      {
         glVertex2i(PolygonGroups[i].Polygons[j].Vertices[k].x, PolygonGroups[i].Polygons[j].Vertices[k].y);
      }
      glEnd();
   }
   //glDisable(GL_LINE);

}

/* ������� ������ �� ����� */
void Display(void) 
{
   glClearColor(1, 1, 1, 1);
   glClear(GL_COLOR_BUFFER_BIT);
   Render();
   glFinish();
}
/* ������� ��������� �������� ���� */
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
   if (state != GLUT_DOWN) return; //������� ���

   if (button == GLUT_LEFT_BUTTON) //���
   {
      int PolygonGroup_last = PolygonGroups.size() -1;
      int Polygon_last = PolygonGroups[PolygonGroup_last].Polygons.size() - 1;

      PolygonGroups[PolygonGroup_last].Polygons[Polygon_last].Vertices.push_back(Point(x,Height - y)); //���������� �����
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
      /* ��������� XY-�������� ����� */
   case('w'): PolygonGroups[group_num].MoveAllVertices(0, 5); break;
   case('a'): PolygonGroups[group_num].MoveAllVertices(-5, 0); break;
   case('s'): PolygonGroups[group_num].MoveAllVertices(0, -5); break;
   case('d'): PolygonGroups[group_num].MoveAllVertices(5, 0); break;
   case(' '): PolygonGroups.push_back(PolygonGroup());         //�������� ����� ������
   case('p'): PolygonGroups[group_num].Polygons.push_back(Polygon());      //�������� ������ ��������������
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
//   glutAddMenuEntry("�����", KeyW);
//   glutAddMenuEntry("�����", KeyA);
//   glutAddMenuEntry("����", KeyS);
//   glutAddMenuEntry("������", KeyD);
//
//   int menu_delete = glutCreateMenu(Menu);
//   glutAddMenuEntry("�������", KeyDeletePolygon);
//   glutAddMenuEntry("������ ���������", KeyDeleteGroup);
//
//   int menu = glutCreateMenu(Menu);
//   glutAddSubMenu("����� �����", menu_RGB);
//   glutAddSubMenu("�����������", menu_move);
//   glutAddSubMenu("��������", menu_move);
//
//   glutAttachMenu(GLUT_RIGHT_BUTTON);
//   Keyboard(Empty, 0, 0);
//}
void DeleteGroup()
{
   PolygonGroups.pop_back();
   if (PolygonGroups.empty())
      PolygonGroups.push_back(PolygonGroup());
}

void DeletePolygon()
{
   int group_num = PolygonGroups.size() - 1;
   PolygonGroups[group_num].Polygons.pop_back();
   if (PolygonGroups[group_num].Polygons.empty())
      PolygonGroups[group_num].Polygons.push_back(Polygon());
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
      case(KeyDeleteGroup): DeleteGroup(); break;  //�������� ������
      case(KeyDeletePolygon): DeletePolygon(); break;   //�������� ��������������
      default:
         int menu_RGB = glutCreateMenu(Menu);   
         glutAddMenuEntry("R", KeyR);
         glutAddMenuEntry("G", KeyG);
         glutAddMenuEntry("B", KeyB);

         int menu_move = glutCreateMenu(Menu);
         glutAddMenuEntry("�����", KeyW);
         glutAddMenuEntry("�����", KeyA);
         glutAddMenuEntry("����", KeyS);
         glutAddMenuEntry("������", KeyD);

         int menu_delete = glutCreateMenu(Menu);
         glutAddMenuEntry("�������", KeyDeletePolygon);
         glutAddMenuEntry("������ ���������", KeyDeleteGroup);

         int menu = glutCreateMenu(Menu);
         glutAddSubMenu("����� �����", menu_RGB);
         glutAddSubMenu("�����������", menu_move);
         glutAddSubMenu("��������", menu_delete);

         glutAttachMenu(GLUT_RIGHT_BUTTON);
         Keyboard(Empty, 0, 0);
   }
   glutPostRedisplay();
}

/* �������� ��������� */
void main(int argc, char* argv[])
{
   glutInit(&argc, argv);
   glutInitDisplayMode(GLUT_RGB);
   glutInitWindowSize(Width, Height);
   glutCreateWindow("���������� ����������");

   PolygonGroups.resize(1);

   Menu(Empty);
   glutDisplayFunc(Display);
   glutReshapeFunc(Reshape);
   glutKeyboardFunc(Keyboard);
   glutMouseFunc(Mouse);

   glutMainLoop();
}
