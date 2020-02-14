/*
 * main.cpp
 *
 *  Created on: Mar 12, 2019
 *      Author: gilg
 */



#include <fstream>      // std::ifstream
#include <iostream>


#include <math.h>
#include <eigen3/Eigen/Dense>
#include <eigen3/Eigen/Sparse>
#include<eigen3/Eigen/SparseQR>
#include<eigen3/Eigen/IterativeLinearSolvers>


#include <unistd.h>
#include <sys/time.h>


#include <GL/glut.h>


using namespace Eigen;
using namespace std;

typedef Triplet<double> TripD;
typedef Triplet<MatrixXd> TripM;

vector<VectorXd> *ptr_u;


suseconds_t differe(timeval tim1,timeval tim2)
{return (tim2.tv_sec - tim1.tv_sec)*1E6+tim2.tv_usec-tim1.tv_usec;}


inline double f(double x){return 0;}//initial conditions
inline double f1(double x){return 0;}//initial conditions

inline double g(double t){return 0;}//boundary conditions
inline double g1(double t){return 0;}//boundary conditions

double T	=10;
double l	=0.5;
double c	=0.2;

double tay	=0.00025;//0.01;//c
double h	=0.0005;//0.001;//0.1cm


int		N=l/h+1;
int		R=T/tay+1;





void pressNormalKeys(unsigned char key, int xx, int yy)
{

        switch (key)
        {

              case 0x1b:{exit(255);}
        }
}




void display()// �������������� ���������������������� ��������������
{
	glClearColor(1.0, 1.0,1.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	 glLoadIdentity();

	 glColor3ub(0,0,0);

	 static int k=0;

	 GLdouble line[(*ptr_u)[k].size()][2];

	 for(int i=0;i<(*ptr_u)[k].size();i++)
	 {line[i][0]=i*h;line[i][1]=(*ptr_u)[k][i];}


	 k++;
	 usleep(10000);


	 glVertexPointer(2, GL_DOUBLE, 0, line);
	 glEnableClientState(GL_VERTEX_ARRAY);
	 glDrawArrays(GL_LINE_STRIP,0,(*ptr_u)[0].size());



	 glutSwapBuffers();


	 if(k>T/tay)exit(255);
}












void opengl_init(int argc, char** argv)
{
	  glutInit(&argc, argv);
	  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	  glutInitWindowSize(1920, 1080);
	  glutInitWindowPosition(0, 0);
	  glutCreateWindow("graf");
	  glClearColor(0, 0, 0, 0);
	  glMatrixMode(GL_PROJECTION);
	  glLoadIdentity();
	  glOrtho(0, l, -0.5, 0.5, -100, 100);
	  glutDisplayFunc(display);
	  glutIdleFunc(display);
	  glMatrixMode(GL_MODELVIEW);

	  glutKeyboardFunc(pressNormalKeys);


	  glutMainLoop();
}



void solve_problem(int argc, char** argv)
{
	cout<<"N= "<<N<<endl;
	cout<<"r= "<<R<<endl;
	timeval time__;
	timeval time__1;
	gettimeofday(&time__,NULL);

	//inital condition
	VectorXd fi(N);for(int i=0;i<N;i++){fi[i]=f(h*i);}
	VectorXd f1i(N);for(int i=0;i<N;i++){f1i[i]=f1(h*i);}
	// boundary conditions
	VectorXd gk(R);for(int k=0;k<R;k++){gk[k]=g(tay*k);}
	VectorXd g1k(R);for(int k=0;k<R;k++){g1k[k]=g1(tay*k);}

	double a=c*c*tay*tay/h/h;
	double b=2*(h*h-c*c*tay*tay)/h/h;



	SparseMatrix<double> A(N,N);
	vector<TripD> tripletList;
	tripletList.reserve(N*5);

	tripletList.push_back(TripD(0,0,1));
	tripletList.push_back(TripD(N-1,N-1,1));

	for (int i=1;i<(N-1);i++)
	{
		tripletList.push_back(TripD(i,i-1,a));
		tripletList.push_back(TripD(i,i,b));
		tripletList.push_back(TripD(i,i+1,a));
	}
	A.setFromTriplets(tripletList.begin(), tripletList.end());


	vector<VectorXd> u(R);
	ptr_u=&u;


	fi[N/2]=0.1;
	u[0]=fi;
	u[1]=f1i*tay+fi;



	//FILE *fl =fopen("./test.txt","w+");


	for(int k=1;k<R-1;k++)
	{
		u[k+1]=A*u[k]-u[k-1];
		u[k+1][0]=gk[k+1];
		u[k+1][N-1]=g1k[k+1];
	}
	gettimeofday(&time__1,NULL);
	printf("%f \r\n",(double)differe(time__,time__1)/(double)1E6);

	/*
	for(int k=0;k<R-1;k++)
	{
		for(int i=0;i<N;i++){fprintf(fl,"%10.5f ",u[k][i]);}
		fprintf(fl,"\r\n");
	}
	fclose(fl);
	*/

	opengl_init(argc, argv);

}









int main(int argc, char** argv)
{

	solve_problem(argc, argv);



	return 0xff;
}
