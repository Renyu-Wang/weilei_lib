/* 
*   Matrix Market I/O example program
*
*   Read a real (non-complex) sparse matrix from a Matrix Market (v. 2.0) file.
*   and copies it to stdout.  This porgram does nothing useful, but
*   illustrates common usage of the Matrix Matrix I/O routines.
*   (See http://math.nist.gov/MatrixMarket for details.)
*
*   Usage:  a.out [filename] > output
*
*       
*   NOTES:
*
*   1) Matrix Market files are always 1-based, i.e. the index of the first
*      element of a matrix is (1,1), not (0,0) as in C.  ADJUST THESE
*      OFFSETS ACCORDINGLY offsets accordingly when reading and writing 
*      to files.
*
*   2) ANSI C requires one to use the "l" format modifier when reading
*      double precision floating point numbers in scanf() and
*      its variants.  For example, use "%lf", "%lg", or "%le"
*      when reading doubles, otherwise errors will occur.
*/
//Weilei: read from the file and return GF2mat
#include <stdio.h>//cout
#include <stdlib.h> //fprintf?
#include "mmio.h" //for MM format
#include <itpp/itbase.h>
//#include <string>
#include "mm_read.h"
#include "my_lib.h"
using namespace std;
using namespace itpp;

GF2mat MM_to_GF2mat(string  file_name){
  char cstr[file_name.size()+1];
  strcpy(cstr,file_name.c_str());
  return MM_to_GF2mat(cstr);
}

mat MM_to_mat(string  file_name){
  char cstr[file_name.size()+1];
  strcpy(cstr,file_name.c_str());
  return MM_to_mat(cstr);
}


GF2mat MM_to_GF2mat(char * file_name)
{
    int ret_code;
    MM_typecode matcode;
    FILE *f;
    int M, N, nz;   
    int i, *I, *J;
    double *val;

    if ((f = fopen(file_name, "r")) == NULL) {
      cout<<"file open fail:"<<file_name<<endl;
      exit(1);}
    if (mm_read_banner(f, &matcode) != 0)
    {
        printf("Could not process Matrix Market banner.\n");
        exit(1);
    }

    /*  This is how one can screen matrix types if their application */
    /*  only supports a subset of the Matrix Market data types.      */

    if (mm_is_complex(matcode) && mm_is_matrix(matcode) && 
            mm_is_sparse(matcode) )
    {
        printf("Sorry, this application does not support ");
        printf("Market Market type: [%s]\n", mm_typecode_to_str(matcode));
        exit(1);
    }

    /* find out size of sparse matrix .... */

    if ((ret_code = mm_read_mtx_crd_size(f, &M, &N, &nz)) !=0)
        exit(1);

    /* reseve memory for matrices */

    I = (int *) malloc(nz * sizeof(int));
    J = (int *) malloc(nz * sizeof(int));
    val = (double *) malloc(nz * sizeof(double));


    /* NOTE: when reading in doubles, ANSI C requires the use of the "l"  */
    /*   specifier as in "%lg", "%lf", "%le", otherwise errors will occur */
    /*  (ANSI C X3.159-1989, Sec. 4.9.6.2, p. 136 lines 13-15)            */

    for (i=0; i<nz; i++)
    {
      if ( fscanf(f, "%d %d %lg\n", &I[i], &J[i], &val[i]) !=3 ){
	fprintf( stderr, "Expected at least three numbers as input\n");
	exit(1);
      }
      I[i]--;  /* adjust from 1-based to 0-based */
      J[i]--;
    }

    if (f !=stdin) fclose(f);

    /************************/
    /* now write out matrix */
    /************************/
    /*
    mm_write_banner(stdout, matcode);
    mm_write_mtx_crd_size(stdout, M, N, nz);
    for (i=0; i<nz; i++)
        fprintf(stdout, "%d %d %20.19g\n", I[i]+1, J[i]+1, val[i]);
    */

    //read into GF2mat
    GF2mat G(M,N);
    for (int i=0;i<nz;i++){
      G.set(I[i],J[i],val[i]);
    }
    //    GF2matPrint(G);

    return G;
}

mat MM_to_mat(char * file_name)
{
  //  cout<<"debug: MM_to_mat"<<endl;
    int ret_code;
    MM_typecode matcode;
    FILE *f;
    int M, N, nz;   
    int i, *I, *J;
    double *val;

    if ((f = fopen(file_name, "r")) == NULL) {
      cout<<"file open fail:"<<file_name<<endl;
      exit(1);}
    if (mm_read_banner(f, &matcode) != 0)
    {
        printf("Could not process Matrix Market banner.\n");
        exit(1);
    }

    /*  This is how one can screen matrix types if their application */
    /*  only supports a subset of the Matrix Market data types.      */

    if (! mm_is_sparse(matcode)){
      if (f !=stdin) fclose(f);
      //a dense matrix is saved in column format insteead of coordinate format.
      return dense_MM_to_mat(file_name);
    }
    
    if (mm_is_complex(matcode) && mm_is_matrix(matcode) && 
            mm_is_sparse(matcode) )
    {
        printf("Sorry, this application does not support ");
        printf("Market Market type: [%s]\n", mm_typecode_to_str(matcode));
        exit(1);
    }

    /* find out size of sparse matrix .... */

    if ((ret_code = mm_read_mtx_crd_size(f, &M, &N, &nz)) !=0)
        exit(1);

    /* reseve memory for matrices */

    I = (int *) malloc(nz * sizeof(int));
    J = (int *) malloc(nz * sizeof(int));
    val = (double *) malloc(nz * sizeof(double));


    /* NOTE: when reading in doubles, ANSI C requires the use of the "l"  */
    /*   specifier as in "%lg", "%lf", "%le", otherwise errors will occur */
    /*  (ANSI C X3.159-1989, Sec. 4.9.6.2, p. 136 lines 13-15)            */

    for (i=0; i<nz; i++)
    {
      if ( fscanf(f, "%d %d %lg\n", &I[i], &J[i], &val[i]) !=3){
	fprintf( stderr, "Expected at least three numbers as input\n");
	exit(1);
      }
        I[i]--;  /* adjust from 1-based to 0-based */
        J[i]--;
    }

    if (f !=stdin) fclose(f);

    /************************/
    /* now write out matrix */
    /************************/
    /*
    mm_write_banner(stdout, matcode);
    mm_write_mtx_crd_size(stdout, M, N, nz);
    for (i=0; i<nz; i++)
        fprintf(stdout, "%d %d %20.19g\n", I[i]+1, J[i]+1, val[i]);
    */

    //read into GF2mat
    mat G(M,N);
    for (int i=0;i<nz;i++){
      G.set(I[i],J[i],val[i]);
    }

    return G;
}

mat dense_MM_to_mat(char * file_name){
  //this is design for dense matrix set in column fashion
  //  cout<<"debug: dense_MM_to_mat"<<endl;

    int ret_code;
    MM_typecode matcode;
    FILE *f;
    int M, N, nz;   
    int i;//, *I, *J;
    double *val;

    if ((f = fopen(file_name, "r")) == NULL) {
      cout<<"file open fail:"<<file_name<<endl;
      exit(1);}
    if (mm_read_banner(f, &matcode) != 0)
    {
        printf("Could not process Matrix Market banner.\n");
        exit(1);
    }

    //    cout<<matcode<<endl;
    /*  This is how one can screen matrix types if their application */
    /*  only supports a subset of the Matrix Market data types.      */

    if (mm_is_complex(matcode) && mm_is_matrix(matcode) && 
            mm_is_sparse(matcode) )
    {
        printf("Sorry, this application does not support ");
        printf("Market Market type: [%s]\n", mm_typecode_to_str(matcode));
        exit(1);
    }

    /* find out size of sparse matrix .... */

    if ((ret_code = mm_read_mtx_array_size(f, &M, &N)	!=0))
        exit(1);
    nz=M*N;
    /* reseve memory for matrices */

    //    I = (int *) malloc(nz * sizeof(int));
    //J = (int *) malloc(nz * sizeof(int));
    
    val = (double *) malloc(nz * sizeof(double));


    /* NOTE: when reading in doubles, ANSI C requires the use of the "l"  */
    /*   specifier as in "%lg", "%lf", "%le", otherwise errors will occur */
    /*  (ANSI C X3.159-1989, Sec. 4.9.6.2, p. 136 lines 13-15)            */

    //    double min_value=100;
    //    int min_index=-1;
    for (i=0; i<nz; i++)
      {

        if ( fscanf(f, "   %lg\n", &val[i]) !=1){
	  fprintf( stderr, "Expected at least one numbers as input\n");
	  exit(1);
	}
	
	/*if ( val[i] < min_value){
	  cout<< val[i]<<endl;
	  min_value = val[i];
	  min_index = i;
	  
	  }*/
        //I[i]--;  /* adjust from 1-based to 0-based */
        //J[i]--;
    }
    // cout<<M<<" M N "<<N<<endl;
    //cout<<min_value <<"value  index " <<min_index<<endl;
    //cout<<"val[2190] = "<<val[2190]<<endl;

    if (f !=stdin) fclose(f);



    //read into mat
    mat G(M,N);
    //    double dd=0,value=100;
    //int index =-1;
    for (int i=0;i<M;i++){
      for ( int j=0;j<N;j++){
	//	dd=val[j*M+i];
	//cout<<j*N+i<<endl;
	  G.set(i,j,val[j*M+i]);
	  /*if ( val[j*N+i] < value){
	    value = val[j*N+i];
	    index = j*N+i;
	    cout<<value <<" value  index," <<index<<endl;
	    //	    cout<< val[j*N+i]<<","<<G(i,j)<<endl;
	  }
	  if ( G(i,j)==0){	    
	    //    cout<<i<<" i,j "<<j<<", val = "<<val[j*N+i]<<endl;
	  }
	  */
	  // cout<<"test "<<endl;;
      }
    }
    //cout<<M<<" M N "<<N<<endl;
    //cout<<G.rows()<<"  row  col "<<G.cols()<<endl;
    return G;
}

