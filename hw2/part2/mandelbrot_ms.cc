/**
 *  \file mandelbrot_ms.cc
 *
 *  \brief Implement your parallel mandelbrot set in this file.
 */

#include <iostream>
#include <cstdlib>

#include "render.hh"
#include <mpi.h>

using namespace std;

int mandelbrot(double x, double y) {
  int maxit = 511;
  double cx = x;
  double cy = y;
  double newx, newy;

  int it = 0;
  for (it = 0; it < maxit && (x * x + y * y) < 4; ++it) {
    newx = x * x - y * y + cx;
    newy = 2 * x * y + cy;
    x = newx;
    y = newy;
  }
  return it;
}


void slave()
{
	int tempbuf[width+1];

	double x,y;

	MPI_Status status;

	int row_id;

	while(1)
	{
		MPI_Receive(&row_id,1,MPI_INT,0,MPI_ANY_TAG,MPI_COMM_WORLD,&status);

		y=minY + row_id*it;
		x=minX;
		for(int c=0;c<width;++c)
		{
			tempbuf[c] = mandelbrot(x,y);
			x+=jt;
		}

		tempbuf[width] = row_id;
		MPI_Send(tempbuf,width+1,MPI_INT,0,MPI_ANY_TAG,MPI_COMM_WORLD);
	}

}

void master(int height,int width, int size,double minX,double minY,double jt,double it)

{

	char outfilename[80];

	sprintf(outfilename,"mandelbrot_ms_%dx%d_%d.png",width,height,size);

	int tempimg[height*width], slavebuf[width+1];

	MPI_status status;

	gil::rgb8_image_t img(height, width);
   	auto img_view = gil::view(img);

	int row_id =1;
	int row_count=0;

	for(int i=1;i<size;++i)
	{
		MPI_Send(&rowid,1,MPI_INT,i,MPI_ANY_TAG,MPI_COMM_WORLD);
		++rowid;
	}

	while(rowid<height)
	{
		MPI_Receive(slavebuf,width+1,MPI_INT,MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,&status);
		MPI_Send(&rowid,1,MPI_INT,status.MPI_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD);
		++row_id;
		++row_count;
		memcpy((&tempimg + slavebuf[width]*width),slavebuf,width*sizeof(int));
	}

	while(row_count<height)
	{
		MPI_Receive(slavebuf,width+1,MPI_INT,MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,&status);
		memcpy((&tempimg + slavebuf[width]*width),slavebuf,width*sizeof(int));
		++row_count;	
	}

	for(int r=0;r<height;++r)
		for(int c=0;c<width;++c)
		{
			img_view(c,r) = render(tempimg[r*width+c]/512.0);
		}

		gil::png_write_view(outfilename, const_view(img));
	

}

int
main (int argc, char* argv[])
{
  /* Lucky you, you get to write MPI code */

	double minX = -2.1;
  double maxX = 0.7;
  double minY = -1.25;
  double maxY = 1.25;
  
  int height, width,rank,size;
  if (argc == 3) {
    height = atoi (argv[1]);
    width = atoi (argv[2]);
    assert (height > 0 && width > 0);
  } else {
    fprintf (stderr, "usage: %s <height> <width>\n", argv[0]);
    fprintf (stderr, "where <height> and <width> are the dimensions of the image.\n");
    return -1;
  }

  double it = (maxY - minY)/height;
  double jt = (maxX - minX)/width;
  double x, y;

  MPI_Init(&argc, &argv);

  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  if(rank ==0) //Master Process
  {
  	double start_time = MPI_Wtime();
  	master(height,width,size,minX,minY,jt,it);
  	printf("\nTime Taken by the Master-Slave Program is %fl \n",MPI_Wtime()-start_time);
  }

  else
  {
  	slave(width,rank,minX,minY,jt,it)
  }

  MPI_Finalize();

  return 1;


}

/* eof */
