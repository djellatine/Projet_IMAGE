/********************************************************/
/*                     cube.cpp                                                 */
/********************************************************/
/*                Affiche a l'ecran un cube en 3D                      */
/********************************************************/

/* inclusion des fichiers d'en-tete freeglut */

#ifdef __APPLE__
#include <GLUT/glut.h> /* Pour Mac OS X */
#else
#include <GL/glut.h>   /* Pour les autres systemes */
#endif
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <math.h>
#include "Point.h"

#define PI 3.141592653589793


/*
* Declaration des caracteristique de l'algorithme de casteljau
*/
const int m=2;//degré sur la longueur
const int n=2; //degre de la courbe de bezier sur la largeur
const int nbRec=50; //nombre de recursion avant de s'arreter

/** config courbe de bezier **/
Point pc[]={Point(),Point(0.3,1,0),Point(2,0.5,0),Point(2.5,-1.5,0)};//point de controle
Point tab[nbRec+1]; //tableau de point de la courbe de bezier
Point fractal[10*nbRec+1];

/** config surface de bezier **/
Point polyC[n+1][m+1]; //polygone de controle
Point surface[nbRec+1][nbRec+1];//tableau de point constituant la surface de bezier
Point a,b;//point de controls pour le fractal

/*prototype de fonction pour le calcul de la surface de bezier*/
void casteljau();
unsigned int miniFacto(int k,int i);
unsigned int facto(int k);
double bernstein(int i,int n,double t);
void initTab();
void dessinBezierSurface();
double weierstrass(double x);
void bezierFractal();
void fractalDeWeierstrass();
void dessinFractalDeWeierstrass();
void dessinBezierFractalSurface();

/*gestion affichage*/
double zoom=0.0;/* valeur de zoom */

char presse;
int anglex,angley,x,y,xold,yold;

/* Prototype des fonctions */
void affichage();
void clavier(unsigned char touche,int x,int y);
void reshape(int x,int y);
void idle();
void mouse(int bouton,int etat,int x,int y);
void mousemotion(int x,int y);
void bezierCourbe();

int main(int argc,char **argv)
{
  /* initialisation de glut et creation
     de la fenetre */
    srand (time(NULL));
  glutInit(&argc,argv);
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
  glutInitWindowPosition(250,250);
  glutInitWindowSize(1000,1000);
  glutCreateWindow("projet mgsi");

  /* Initialisation d'OpenGL */
  glClearColor(0.0,0.0,0.0,0.0);
  glColor3f(1.0,1.0,1.0);
  glPointSize(2.0);
  glEnable(GL_DEPTH_TEST);

    /* Mise en place de la projection perspective */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(65,1,1,30.0);
    glMatrixMode(GL_MODELVIEW);


  /* enregistrement des fonctions de rappel */
  glutDisplayFunc(affichage);
  glutKeyboardFunc(clavier);
  glutReshapeFunc(reshape);
  glutMouseFunc(mouse);
  glutMotionFunc(mousemotion);




  glMatrixMode( GL_PROJECTION );
     glLoadIdentity();
   gluPerspective(60 ,1,.1,30.);



  /* Entree dans la boucle principale glut */
  glutMainLoop();
  return 0;
}


int mod(int a, int b)
{
    int res=(a%b);

    if(res>=00)
        return res;
    else
        return b+res;
}

void affichage()
{
  /* effacement de l'image avec la couleur de fond */
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glShadeModel(GL_SMOOTH);
  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity();

  glTranslatef(0.,0.,-5.);
  glRotatef(angley,1.0,0.0,0.0);
  glRotatef(anglex,0.0,1.0,0.0);


    //bezierCourbe();
    //dessinBezierSurface();
    dessinFractalDeWeierstrass();
    //dessinBezierFractalSurface();


    //Repère
    //axe x en rouge
    glBegin(GL_LINES);
        glColor3f(1.0,0.0,0.0);
    	glVertex3f(0, 0,0.0);
    	glVertex3f(1, 0,0.0);
    glEnd();
    //axe des y en vert
    glBegin(GL_LINES);
    	glColor3f(0.0,1.0,0.0);
    	glVertex3f(0, 0,0.0);
    	glVertex3f(0, 1,0.0);
    glEnd();
    //axe des z en bleu
    glBegin(GL_LINES);
    	glColor3f(0.0,0.0,1.0);
    	glVertex3f(0, 0,0.0);
    	glVertex3f(0, 0,1.0);
    glEnd();

  glFlush();

  //On echange les buffers
  glutSwapBuffers();
}

void clavier(unsigned char touche,int x,int y)
{
  switch (touche)
    {
    case 'p': /* affichage du carre plein */
      glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
      glutPostRedisplay();
      break;
    case 'f': /* affichage en mode fil de fer */
      glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
      glutPostRedisplay();
      break;
    case 's' : /* Affichage en mode sommets seuls */
      glPolygonMode(GL_FRONT_AND_BACK,GL_POINT);
      glutPostRedisplay();
      break;
    case 'd':
      glEnable(GL_DEPTH_TEST);
      glutPostRedisplay();
      break;
    case 'D':
      glDisable(GL_DEPTH_TEST);
      glutPostRedisplay();
      break;
    case 'Z':
        zoom+=0.1;
        glutPostRedisplay();
        break;
    case 'z':
        zoom-=0.1;
        glutPostRedisplay();
        break;
    case 'q' : /*la touche 'q' permet de quitter le programme */
      exit(0);
    }
}

void reshape(int x,int y)
{
  if (x<y)
    glViewport(0,(y-x)/2,x,x);
  else
    glViewport((x-y)/2,0,y,y);
}

void mouse(int button, int state,int x,int y)
{
  /* si on appuie sur le bouton gauche */
  if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
  {
    presse = 1; /* le booleen presse passe a 1 (vrai) */
    xold = x; /* on sauvegarde la position de la souris */
    yold=y;
  }
  /* si on relache le bouton gauche */
  if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
    presse=0; /* le booleen presse passe a 0 (faux) */
}

void mousemotion(int x,int y)
  {
    if (presse) /* si le bouton gauche est presse */
    {
      /* on modifie les angles de rotation de l'objet
	 en fonction de la position actuelle de la souris et de la derniere
	 position sauvegardee */
      anglex=anglex+(x-xold);
      angley=angley+(y-yold);
      glutPostRedisplay(); /* on demande un rafraichissement de l'affichage */
    }

    xold=x; /* sauvegarde des valeurs courante de le position de la souris */
    yold=y;
  }



/**
*return le factoriel de k
*/
unsigned int facto(int k)
{
    if(k==0||k==1)
        return 1;
    else
        return k*facto(k-1);
}

/**
* return le resultat de la calcul de k!/(k-i)!
*/
unsigned int miniFacto(int k, int i)
{
    if(k==i)
        return 1;
    else
        return k*miniFacto(k-1,i);
}

/**
*return le polynomde de bernsetein Bi,n(t)
*/
double bernstein(int i,int n,double t)
{
    unsigned int c=miniFacto(n,n-i)/facto(i);//combinaison nCi
    double b=c*pow(1-t,n-i)*pow(t,i);
    return b;
}

/**
    produit tensoriel en utilisant le polynome de beirnstein
    ceci calcul une courbe de bezier
**/
void casteljau()
{
    for(int u=0;u<=nbRec;u++)
    {
        double t=double(u)/double(nbRec); //si u=0 =>t=0; si u=nbrec => t=1; on a t € [0,1]

        double x=0,y=0,z=0;
        double b=0;

        for(int i=0;i<=n;i++)
        {
            b=bernstein(i,n,t);
            //printf("%.2f %.2f %.2f \n",pc[i].x,pc[i].y,pc[i].z);
            x+=b*pc[i].x;
            y+=b*pc[i].y;
            z+=b*pc[i].z;
        }

        //on met le point dans la position u du tableau de la courbe
        tab[u]=Point(x,y,z);
    }
}


/** affiche la courbe de bezier calculer par la fonction casteljau()**/
void bezierCourbe()
{
    casteljau();

    for(int i=0;i<=n;i++)
    {
        glPointSize(5);
        glBegin(GL_POINTS);
            glColor3f(1,0,0);
            glVertex3d(pc[i].x,pc[i].y,pc[i].z);
        glEnd();
    }
    for(int i=0;i<nbRec;i++)
    {
        glBegin(GL_LINES);
            glColor3f(tab[i].r,tab[i].g,tab[i].b);
            glVertex3f(tab[i].x,tab[i].y,tab[i].z);
            glVertex3f(tab[i+1].x,tab[i+1].y,tab[i+1].z);
        glEnd();
    }
}

/** produit tensoriel entre 2 courbe de bezier **/
void bezierSurface()
{
    initTab();
    //on fixe d'abord j et on fait varier i

    for(int l=0;l<=nbRec;l++)
    {
        double v=double(l)/double(nbRec);

        for(int k=0;k<=nbRec;k++)
        {
            double u=double(k)/double(nbRec);
            double xij=0, yij=0, zij=0;
            double bj;
            for(int j=0;j<=n;j++)
            {
                bj= bernstein(j,n,v);

                double xi=0, yi=0, zi=0;
                double bi;

                for(int i=0;i<=m;i++)
                {
                    bi=bernstein(i,m,u);

                    xi+=bi*polyC[i][j].x;
                    yi+=bi*polyC[i][j].y;
                    zi+=bi*polyC[i][j].z;
                }
                xij+=bj*xi;
                yij+=bj*yi;
                zij+=bj*zi;
            }
            surface[k][l]=Point(xij,yij,zij);
        }
    }
}

/** initialise le tableau des points de controls  **/
void initTab()
{
    a= Point(-10,0,0);//points de controle fractal de weierstrass
    b= Point(-1,0,0);//points de controle fractal de weierstrass
    polyC[0][0]=Point(0,0,3);
    polyC[0][1]=Point(0,1,5);
    polyC[0][2]=Point(0,2,2.5);
    //polyC[0][3]=Point(0,3,3);

    polyC[1][0]=Point(1,0,2);
    polyC[1][1]=Point(1,1,1.5);
    polyC[1][2]=Point(1,2,2.5);
    //polyC[1][3]=Point(1,3,1);

    polyC[2][0]=Point(2,0,2);
    polyC[2][1]=Point(2,1,3.5);
    polyC[2][2]=Point(2,2,2.5);
    //polyC[2][3]=Point(2,3,1.5);


    /*polyC[3][0]=Point(3,0,3.5);
    polyC[3][1]=Point(3,1,1.5);
    polyC[3][2]=Point(3,2,2);
    polyC[3][3]=Point(3,3,4);*/
}

/** affiche la surface de bezier **/
void dessinBezierSurface()
{
    bezierSurface();

    /*affichage polygone de controle*/
    for(int i=0;i<=n;i++)
    {
        for(int j=0;j<=m;j++)
        {
            glPointSize(8);
            glBegin(GL_POINTS);
                glColor3f(1,0,0);
                glVertex3f(polyC[i][j].x,polyC[i][j].y,polyC[i][j].z);
            glEnd();

            glLineWidth(3);
            if(i!=n)
            {
                glBegin(GL_LINES);
                glColor3f(0,1,0);
                glVertex3f(polyC[i][j].x,polyC[i][j].y,polyC[i][j].z);
                glVertex3f(polyC[i+1][j].x,polyC[i+1][j].y,polyC[i+1][j].z);
                glEnd();
            }

            if(j!=m)
            {
                glBegin(GL_LINES);
                glColor3f(0,1,0);
                glVertex3f(polyC[i][j].x,polyC[i][j].y,polyC[i][j].z);
                glVertex3f(polyC[i][j+1].x,polyC[i][j+1].y,polyC[i][j+1].z);
                glEnd();
            }
        }
    }

    /*affichage surface de bezier*/
    for(int i=0;i<=nbRec;i++)
    {
        for(int j=0;j<=nbRec;j++)
        {
            glLineWidth(1);
            if(j!=nbRec)
            {
                glBegin(GL_LINES);
                    glColor3f(surface[i][j].r,surface[i][j].g,surface[i][j].b);
                    glVertex3d(surface[i][j].x,surface[i][j].y,surface[i][j].z);
                    glVertex3d(surface[i][j+1].x,surface[i][j+1].y,surface[i][j+1].z);
                glEnd();
            }

            if(i!=nbRec)
            {
                glBegin(GL_LINES);
                    glColor3f(surface[i][j].r,surface[i][j].g,surface[i][j].b);
                    glVertex3d(surface[i][j].x,surface[i][j].y,surface[i][j].z);
                    glVertex3d(surface[i+1][j].x,surface[i+1][j].y,surface[i+1][j].z);
                glEnd();
            }
        }
    }
}

/*############### PARTIE FRACTAL#######################*/

/** return la valeur de la fonction fractal de weierstrass **/
double weierstrass(double x)
{
    double y=0;
    double a,b;
    for(int k=0;k<=nbRec;k++)
    {
        b=pow(1.5,k);
        a=pow(0.75,k);

        y+=a*sin(PI*b*x);
    }
    return y;
}


void fractalDeWeierstrass()
{
    fractal[0]=a;
    fractal[10*nbRec]=b;
    double x,dt=(b.x-a.x)/(10*nbRec);//distance entre chaque pas
    for(int i=1;i<10*nbRec;i++)
    {
        x=fractal[i-1].x+dt;//on ajoute le nombre de pas a l'ancienne valeur de x;
        fractal[i]=Point(x,weierstrass(x),0);
    }
}

void dessinFractalDeWeierstrass()
{
    initTab();
    fractalDeWeierstrass();
    for(int i=0;i<10*nbRec;i++)
    {
        glBegin(GL_LINES);
            glColor3f(fractal[i].r,fractal[i].g,fractal[i].b);
            glVertex3d(fractal[i].x,fractal[i].y,fractal[i].z);
            glColor3f(fractal[i+1].r,fractal[i+1].g,fractal[i+1].b);
            glVertex3d(fractal[i+1].x,fractal[i+1].y,fractal[i+1].z);
        glEnd();
    }
}
/*############### PARTIE BEZIER X FRACTAL#######################*/

/** produit tensoriel entre une courbe de bezier et la fonction de weierstrass **/
void bezierFractal()
{
    initTab();
    //on fixe d'abord j et on fait varier i

    for(int l=0;l<=nbRec;l++)
    {
        /**pour la colonne**/
        double v=double(l)/double(nbRec);

        for(int k=0;k<=nbRec;k++)
        {
            double u=double(k)/double(nbRec);//pour la ligne
            double xij=0, yij=0, zij=0;
            double bj;
            for(int j=0;j<=n;j++)
            {
                bj= bernstein(j,n,v);
                double xi=0, yi=0, zi=0;
                double bi;

                for(int i=0;i<=m;i++)
                {
                    bi=bernstein(i,m,u);

                    xi+=bi*polyC[i][j].x;
                    yi+=bi*polyC[i][j].y;
                    zi+=bi*polyC[i][j].z;
                }
                if(j%2==1)
                {
                    xij+=bj*xi;
                    yij+=bj*weierstrass(xi);
                    zij+=bj*weierstrass(xi);
                }
                else
                {
                    xij+=bj*xi;
                    yij+=bj*yi;
                    zij+=bj*zi;
                }
            }
            surface[k][l]=Point(xij,yij,zij);
        }
    }
}

/** affiche la surface de bezier **/
void dessinBezierFractalSurface()
{
    //bezierSurface();
    bezierFractal();

    /*affichage polygone de controle*/
    for(int i=0;i<=n;i++)
    {
        for(int j=0;j<=m;j++)
        {
            glPointSize(8);
            glBegin(GL_POINTS);
                glColor3f(1,0,0);
                glVertex3f(polyC[i][j].x,polyC[i][j].y,polyC[i][j].z);
            glEnd();

            glLineWidth(3);
            if(i!=n)
            {
                glBegin(GL_LINES);
                glColor3f(0,1,0);
                glVertex3f(polyC[i][j].x,polyC[i][j].y,polyC[i][j].z);
                glVertex3f(polyC[i+1][j].x,polyC[i+1][j].y,polyC[i+1][j].z);
                glEnd();
            }

            if(j!=m)
            {
                glBegin(GL_LINES);
                glColor3f(0,1,0);
                glVertex3f(polyC[i][j].x,polyC[i][j].y,polyC[i][j].z);
                glVertex3f(polyC[i][j+1].x,polyC[i][j+1].y,polyC[i][j+1].z);
                glEnd();
            }
        }
    }

    /*affichage surface de bezier*/
    for(int i=0;i<=nbRec;i++)
    {
        for(int j=0;j<=nbRec;j++)
        {
            glLineWidth(1);
            if(j!=nbRec)
            {
                glBegin(GL_LINES);
                    glColor3f(surface[i][j].r,surface[i][j].g,surface[i][j].b);
                    glVertex3d(surface[i][j].x,surface[i][j].y,surface[i][j].z);
                    glVertex3d(surface[i][j+1].x,surface[i][j+1].y,surface[i][j+1].z);
                glEnd();
            }

            if(i!=nbRec)
            {
                glBegin(GL_LINES);
                    glColor3f(surface[i][j].r,surface[i][j].g,surface[i][j].b);
                    glVertex3d(surface[i][j].x,surface[i][j].y,surface[i][j].z);
                    glVertex3d(surface[i+1][j].x,surface[i+1][j].y,surface[i+1][j].z);
                glEnd();
            }
        }
    }
}



