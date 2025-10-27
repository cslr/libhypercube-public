/*
 * Cube C API for DLL/shared library
 *
 * These C API function calls are NOT thread-safe
 * (especially cube creation and removal cannot be used at the same time with other functions.)
 * 
 * Functions return 0 or -1 for failure, positive value for success.
 */


#ifndef __cube_interface_h
#define __cube_interface_h

#ifdef WINOS
#define DLL_FUNCTION __declspec(dllexport)
#else
#define DLL_FUNCTION
#endif

#ifdef __cplusplus
extern "C"
{
#endif

  /* returns allocated cube number (starting from zero to positive infinity) [returns negative if failure] */
  DLL_FUNCTION int cube_init_new_cube();

  /* removes cube and frees memory */
  DLL_FUNCTION int cube_free_cube(unsigned int cube);
  
  
  /* return number of new messages and stores messages in *messages */
  DLL_FUNCTION int cube_get_unread_messages(unsigned int cube,
					    char*** messages); 
  

  /* calculates mathematical model/mapping from good preset values, target REDUCED_DIMENSIONS must be 2 or 3
   *
   * calculation happens in parallel in a thread and one must check cube_is_paramter_reducer_computing() to know when
   * the computation has been ended
   *
   * parameters:
   * cube - cube model number allocated using cube_init_new_cube()
   * presets - allocated table of good presets
   * NUM_PRESETS - number of presets in presets table
   * DIM_PRESETS - number of dimension in presets table's presets
   * method - dimension reduction method, either: CUBE_METHOD_ICA or CUBE_METHOD_TSNE
   * REDUCED_DIMENSIONS - number of dimensions in reduced space of good presets, accepts only values 2 or 3
   * quality - recommended to use value 1.0, not properly used for now (in the future alters model size [0,1])
   */
  DLL_FUNCTION int cube_start_calculate_parameter_reduction(unsigned int cube,
							    float** presets,
							    unsigned int NUM_PRESETS,
							    unsigned int DIM_PRESETS,
							    unsigned int method,
							    unsigned int REDUCED_DIMENSIONS,
							    float quality);
  
  const unsigned int CUBE_METHOD_ICA = 0; /* method uses linear ICA and neural network */
  const unsigned int CUBE_METHOD_TSNE = 1; /* method uses t-SNE and neural network */

  
  /* returns 1 if parallel computing of model is running and 0 otherwise (not-running (anymore)) */
  DLL_FUNCTION int cube_is_parameter_reducer_computing(unsigned int cube);
  
  /* stops parallel computinf of model from good presets before it finishes */
  DLL_FUNCTION int cube_stop_parameter_reduction_calculations(unsigned int cube);

  /* returns 1 if cube has the newest recent model or 0 if computations are still ongoing */
  DLL_FUNCTION int cube_has_model(unsigned int cube);


  /* return number of restored_preset dimensions and stores restored preset in *restored_preset pointer (mallocs)
   * preset values should be in the range of [-2,+2] (aprox. Normal(0,I) distribution)
   */
  DLL_FUNCTION int cube_restore(unsigned int cube,
				float* preset, unsigned int DIM_PRESET,
				float** restored_preset);

  DLL_FUNCTION int cube_get_input_dimensions(unsigned int cube);

  DLL_FUNCTION int cube_get_restored_dimensions(unsigned int cube);

  /* exports model's parameters to 1d table, malloc()s memory and stores pointer to *model_params, return model parameters dimensions */
  DLL_FUNCTION int cube_export_model(unsigned int cube,
				     double** model_params);

  /* imports paramters to model from 1d table [returnd by cube_export_model() */
  DLL_FUNCTION int cube_import_model(unsigned int cube,
				     double* model_params, unsigned int DIM_PARAMS);

  /* saves cube's parameters to a file */
  DLL_FUNCTION int cube_save(unsigned int cube,
			     char* filename);

  /* loads cube's parameters from a file */
  DLL_FUNCTION int cube_load(unsigned int cube,
			     char* filename);

#ifdef __cplusplus
};
#endif

#endif
