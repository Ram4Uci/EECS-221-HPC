/**
 *  \file mandelbrot_joe.cc
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

int main(int argc, char* argv[]) {

  double minX = -2.1;
  double maxX = 0.7;
  double minY = -1.25;
  double maxY = 1.25;
  int rank, size, moveBy, blkSIZE,dataSIZE;
  int height, width;
  int receiverRank = 0;

  if (argc == 3) {
    height = atoi(argv[1]);
    width = atoi(argv[2]);
    assert(height > 0 && width > 0);
  } else {
    fprintf(stderr, "usage: %s <height> <width>\n", argv[0]);
    fprintf(stderr,
	    "where <height> and <width> are the dimensions of the image.\n");
    return -1;
  }

  double it = (maxY - minY) / height;
  double jt = (maxX - minX) / width;
  double x, y;
  double timeStart;

  double *get;
  MPI_Init(&argc, &argv);
  //Get Rank And Size
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);



  if(rank == 0) {
    get = (double *) malloc(height * width * sizeof(double));
    //Start Of Timer
    timeStart = MPI_Wtime();
  }

  blkSIZE = height / size;
  dataSIZE = blkSIZE * width;
  double *send = (double *) malloc(dataSIZE * sizeof(double));

  y = minY + rank * blkSIZE * it;
  moveBy = 0;

  for (int i = rank * blkSIZE; i < (rank + 1) * blkSIZE; i++) {
    x = minX;
    for (int j = 0; j < width; j++) {
      send[moveBy + j] = mandelbrot(x, y) / 512.0;
      x += jt;
    }
    moveBy += width;
    y += it;
  }
  moveBy = rank*blkSIZE;
  MPI_Barrier (MPI_COMM_WORLD);
  MPI_Gather(send, dataSIZE, MPI_DOUBLE, get + moveBy, dataSIZE, MPI_DOUBLE, receiverRank , MPI_COMM_WORLD);

  if (rank == 0) {
    gil::rgb8_image_t img(height, width);
    auto img_view = gil::view(img);

    for (int i = 0; i < size * blkSIZE; ++i) {
      moveBy = i * width;
      for (int j = 0; j < width; ++j)
	img_view(j, i) = render(get[j + moveBy]);
    }

    y = minY + size * blkSIZE * it;
    //Handle Reamining Rows
    for (int i = size * blkSIZE; i < height; i++) {
      x = minX;
      for (int j = 0; j < width; j++) {
	img_view(j, i) = render(mandelbrot(x, y)/512.0);
	x += jt;
      }
      y += it;
    }

    gil::png_write_view("mandelbrotJoe.png", const_view(img));
    cout<<"program using Joe's Idea is finished. It has run for :"<<(MPI_Wtime()-timeStart)<<endl;
  }

  MPI_Finalize();
  return 0;
}
