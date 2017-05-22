/**
 *  \file mandelbrot_susie.cc
 *
 *  \brief Implement your parallel mandelbrot set in this file.
 */


#include <iostream>
#include <cstdlib>
#include <iostream>
#include <cstdlib>
#include "render.hh"
#include <mpi.h>
#include <math.h>


using namespace std;

int
mandelbrot(double x, double y) {
  int maxit = 511;
  double cx = x;
  double cy = y;
  double newx, newy;

  int it = 0;
  for (it = 0; it < maxit && (x*x + y*y) < 4; ++it) {
    newx = x*x - y*y + cx;
    newy = 2*x*y + cy;
    x = newx;
    y = newy;
  }
  return it;
}

int
main(int argc, char* argv[]) {

  char outfilename[80];
  double minX = -2.1;
  double maxX = 0.7;
  double minY = -1.25;
  double maxY = 1.25;
  int height, width;
  if (argc == 3) {
    height = atoi(argv[1]);
    width = atoi(argv[2]);
    assert(height > 0 && width > 0);
  }
  else {
    fprintf(stderr, "usage: %s <height> <width>\n", argv[0]);
    fprintf(stderr, "where <height> and <width> are the dimensions of the image.\n");
    return -1;
  }
  double it = (maxY - minY) / height;
  double jt = (maxX - minX) / width;
  double x, y;

  int rank,size,moveBy;
  double *get;
  int dataSize = height*width;
  double startTime;
  int EachDataSize = width;
  int receiverRank = 0;
  MPI_Init (&argc, &argv);
  //Get Rank and Size
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size (MPI_COMM_WORLD, &size);


  if (rank == 0) {
    get = (double*)malloc(sizeof(double) * height * width );
    startTime = MPI_Wtime();
  }

  double *send = (double*) malloc(width * sizeof(double)) ;

  y = minY+(rank*it);
  for (int i = rank; i < (floor)(height/size) * size; i+=size) {
    x = minX;
    for (int j = 0; j < width; ++j) {
      send[j] = mandelbrot(x, y) / 512.0;
      x += jt;
    }
    y += size*it;
    moveBy = i*width;
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Gather(send,  EachDataSize, MPI_DOUBLE, get+moveBy,  EachDataSize, MPI_DOUBLE, receiverRank , MPI_COMM_WORLD);
  }


  if (rank==0) {
    gil::rgb8_image_t img(height, width);
    auto img_view = gil::view(img);
    int rowID = (floor)(height/size) * size;
    for (int i = 0; i < rowID; ++i) {
      moveBy = i*width;
      for (int j = 0; j < width; ++j)
	img_view(j, i) = render(get[j+moveBy]);
    }

    //Handle Remaining Rows
    y = minY + (rowID * it);
    for( int  i =rowID; i<height; i++)
      {
	x = minX;
	for (int j = 0; j < width; j++) {
	  img_view(j, i) = render(mandelbrot(x, y) / 512.0);
	  x += jt;
	}
	y += it;
      }
    sprintf(outfilename,"mandelbrot_susie_%dx%d_%d.png",width,height,size);
    gil::png_write_view(outfilename, const_view(img));
    printf("\nTime Taken by Susie's Program is %fl \n",MPI_Wtime()-startTime);
    printf(" For Image of %d X %d for %d Nodes\n",width,height,size);

  }
  MPI_Finalize();

  return 0;
}
/* EOF */


