

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include <windows.h>

#include <string>
#include <vector>

#include "cube_interface.h"


int main(void)
{
  srand(time(0));
  
  // we generate HIGHDIM dimensional uniform distributed means in random locations
  // in hypercube [0,+1]^HIGHDIM. mean = uniform([0.1,0.9]^HIGHDIM)
  
  const unsigned int NUM_DATA = 1000; // was: 10.000
  
  const unsigned int HIGHDIM = 50;
  const unsigned int CLUSTERS = 10; // number of clusters
  const unsigned int N = NUM_DATA/CLUSTERS; // number of samples per cluster (total of 1000 datapoints)
  
  // generate training data
  std::vector< std::vector<float> > data;
  std::vector<float> x, mean;
  
  mean.resize(HIGHDIM);
  x.resize(HIGHDIM);
  
  for(unsigned int c=0;c<CLUSTERS;c++){

    for(unsigned int i=0;i<mean.size();i++){
      mean[i] = 2.0f*( ((float)rand())/((float)RAND_MAX) - 0.50f);
      
      float v1 = 0.8f;
      float v2 = 0.1f;
      mean[i] = v1*mean[i] + v2;
    }
    
    for(unsigned int n=0;n<N;n++){
      std::vector<float> p;
      
      for(unsigned int i=0;i<x.size();i++){
	x[i] = 2.0f*( ((float)rand())/((float)RAND_MAX) - 0.50f);

	float v = 0.10f;

	x[i] *= v;
	x[i] += mean[i];
      }

      p.resize(HIGHDIM);

      for(unsigned int i=0;i<HIGHDIM;i++){
	p[i] = x[i];
	if(p[i] > 1.0) p[i] = 1.0f;
	else if(p[i] < 0.0) p[i] = 0.0f;
      }
      
      data.push_back(p);
    }
  }


  int rval = cube_init_new_cube();

  if(rval < 0){
    printf("ERROR: Initializing cube failed\n.");
    return -1;
  }

  const unsigned int cube = (unsigned int)rval;

  // reduce dimensions to 3 (only 2 and 3 are supported)
  const unsigned int CUBE_DIMENSIONS = 3;

  if(cube_has_model(cube) == 0){
    printf("Cube: Correctly don't have model yet.\n");
    fflush(stdout);
  }

  // allocates C table of memory for preset data [C API uses C tables]
  float** presets_table = NULL;

  presets_table = (float**)malloc(sizeof(float*)*data.size());

  if(presets_table == NULL){
    printf("Out of memory\n");
    return -1;
  }
  
  for(unsigned int i=0;i<data.size();i++){
    presets_table[i]= (float*)malloc(sizeof(float)*data[i].size());
    
    if(presets_table[i] == NULL){
      printf("Out of memory\n");
      return -1;
    }

    for(unsigned int j=0;j<data[i].size();j++){
      presets_table[i][j] = data[i][j];
    }
  }

  // starts parameter reduction thread
  if(cube_start_calculate_parameter_reduction(cube, presets_table, data.size(), data[0].size(),
					      CUBE_METHOD_TSNE, CUBE_DIMENSIONS, 1.0f) == 0){
    printf("Starting parameter reduction FAILED.\n");
    return -1;
  }

  // frees allocated malloced table of presets (presets has been passed to computing thread)
  for(unsigned int i=0;i<data.size();i++){
    free(presets_table[i]);
  }

  free(presets_table);
  presets_table = NULL;


  // wait for computing to finish and reports verbose messages from optimizer thread
  while(cube_is_parameter_reducer_computing(cube)){
    char** messages = NULL;

    const unsigned int MSG_NUM = cube_get_unread_messages(cube, &messages);

    // prints messages are free()s malloc()ed message table
    if(MSG_NUM && messages){
      for(unsigned int i=0;i<MSG_NUM;i++){
	printf("%s\n", messages[i]);
	free(messages[i]);
      }
      
      fflush(stdout);
      free(messages);
    }
    
    Sleep(1000);
  }

  // computing has finished, checks whether cube has a model
  if(cube_has_model(cube) > 0){

    std::vector<float> z;
    std::vector<float> x;
    
    std::vector<float> restored, restored2;

    z.resize(CUBE_DIMENSIONS);
    x.resize(CUBE_DIMENSIONS);
    
    for(unsigned int i=0;i<CUBE_DIMENSIONS;i++){
      x[i] = 2.0f*( ((float)rand())/((float)RAND_MAX) - 0.50f);
      
      z[i] = x[i];
      z[i] *= 0.25; // z is [-0.25,+0.25] valued
    }

    float* rz = NULL;
    float* rv = NULL;

    rz = (float*)malloc(sizeof(float)*z.size());

    if(rz == NULL){
      printf("Out of memory\n");
      return -1;
    }

    for(unsigned int i=0;i<z.size();i++){
      rz[i]= z[i];
    }

    // actually calculates the mapped new preset from rz CUBE_DIMENIONS coordinates
    // rz vector should have values between [-2,+2] (input parameters are approximately Normal(0,I) distributed vectors)
    int rv_dim = cube_restore(cube, rz, z.size(), &rv);

    if(rv_dim <= 0 || rv == NULL){
      printf("WARN: Parameter restoration FAILED.\n");
      return -1;
    }

    restored.resize(rv_dim);

    for(unsigned int i=0;i<restored.size();i++){
      restored[i] = rv[i];
    }

    free(rv); rv = NULL;

    // creates new cube and tests parameter transfer/load/save between cube models
    int rval2 = cube_init_new_cube();
    if(rval < 0){
      printf("ERROR: initializing new cube FAILED.\n");
      return -1;
    }

    const unsigned int cube2 = (unsigned int)rval2;

    double* params = NULL;
    int params_dim = cube_export_model(cube, &params);

    if(params_dim <= 0  || params == NULL){
      printf("WARN: Parameter exporting FAILED.\n");
      return -1;
    }

    if(cube_import_model(cube2, params, (unsigned int)params_dim) == 0){
      printf("WARN: Parameter importing FAILED.\n");
      return -1;
    }

    rv_dim = cube_restore(cube2, rz, z.size(), &rv);

    if(rv_dim <= 0 || rv == NULL){
      printf("WARN: Parameter restoration FAILED.\n");
      return -1;
    }

    restored2.resize(rv_dim);

    for(unsigned int i=0;i<restored2.size();i++){
      restored2[i]= rv[i];
    }

    free(rv); rv = NULL;


    if(restored.size() != restored2.size()){
      printf("WARN: Parameter restoration sizes mismatch.\n");
      return -1;
    }

    // checks that both cube models give same output with the same model parameters
    double e = 0.0;

    for(unsigned int i=0;i<restored.size();i++){
      e += fabs(restored[i] - restored2[i]);
    }

    e /= restored.size();

    if(e > 0.00001){
      printf("WARN: Parameter restoration mismatch.\n");
      return -1;
    }

    printf("RESTORED PARAMETER = ");

    for(unsigned int i=0;i<restored.size();i++){
      printf("%f ", restored[i]);
    }

    printf("\n"); fflush(stdout);

    cube_free_cube(cube2);
       
  }
  else{
    printf("WARN: Parameter reduction don't have a model.\n");
    return -1;
  }

  // free()s cube models memory
  cube_free_cube(cube);

  printf("EVERYTHING OK\n");
  
  return 0;
}
