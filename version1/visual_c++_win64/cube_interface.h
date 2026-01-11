/**
 * @file cube_interface.h
 * 
 * @brief Cube C API for DLL/shared library
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

  /**
   * Returns allocated cube object number (starting from zero to positive infinity) [returns negative if failure]
   *
   * @return allocated cube number or negative in case of failure
   */
  DLL_FUNCTION int cube_init_new_cube();

  /**
   * Deletes allocated cube and frees memory
   *
   * @return returns 1 in case of success and 0 in case of failure (cube number doesn't exist)
   */
  DLL_FUNCTION int cube_free_cube(unsigned int cube);
  
  
  /**
   * Return number of new messages and stores malloc()ed messages buffer in *messages.
   * User must free messages using free(). ({for(k) free((*messages)[k]);} free(*messages);)
   *
   * @param cube allocated cube number
   * @param messages pointer to char* messages[] buffer which stores messages (printf("%s", (*messages)[i])
   * @return return number of messages allocated or zero for failure
   */
  DLL_FUNCTION int cube_get_unread_messages(unsigned int cube,
					    char*** messages); 
  

  /**
   * Calculates mathematical model/mapping from good preset values to low dimensions,
   * target REDUCED_DIMENSIONS must be 2 or 3.
   *
   * Calculations happens in parallel in a thread and one must check cube_is_paramter_reducer_computing()
   * to know when the computation has been ended.
   *
   * @param cube cube model number allocated using cube_init_new_cube()
   * @param presets pointer to allocated table of good presets ( presets[NUM_PRESETS][DIM_PRESETS] )
   * @param NUM_PRESETS number of presets in presets table
   * @param DIM_PRESETS number of dimension in presets table's presets
   * @param method dimension reduction method, either: CUBE_METHOD_LINEAR or CUBE_METHOD_NONLINEAR (nonlinear is recommended)
   * @param REDUCED_DIMENSIONS number of dimensions in reduced space of good presets, only values 2 or 3
   * @param quality recommended to use value 1.0, not properly used for now (in the future alters model size [0,1])
   * @return 1 for success or 0 for failure
   */
  DLL_FUNCTION int cube_start_calculate_parameter_reduction(unsigned int cube,
							    float** presets,
							    unsigned int NUM_PRESETS,
							    unsigned int DIM_PRESETS,
							    unsigned int method,
							    unsigned int REDUCED_DIMENSIONS,
							    float quality);

  /**
   * @brief number of method uses linear parameter reduction.
   *
   * Parameter for method parameter in cube_start_calculate_parameter_reduction().
   */
  const unsigned int CUBE_METHOD_LINEAR = 0;


  /**
   * @brief number of method uses non-linear parameter reduction (recommended)
   *
   * Parameter for method parameter in cube_start_calculate_parameter_reduction().
   */
  const unsigned int CUBE_METHOD_NONLINEAR = 1; 

  
  /**
   * Returns 1 if parallel computing of model is running and 0 otherwise (not-running (anymore)).
   *
   * @param cube cube number
   * @return returns 1 if parameter reduction is computing or 0 if it is not running (anymore)
   */
  DLL_FUNCTION int cube_is_parameter_reducer_computing(unsigned int cube);

  
  /**
   * Stops parallel computing of model from good presets before it finishes.
   *
   * @param cube cube number
   * @return returns 1 if parameter reduction is stopped or 0 if it is not running (anymore) or in case of failure (no such cube number).
   */
  DLL_FUNCTION int cube_stop_parameter_reduction_calculations(unsigned int cube);

  
  /**
   * Returns 1 if cube has the newest recent model or 0 if computations are still ongoing.
   *
   * @param cube number
   * @return returns 1 if cube has newest recent model or 0 if computations are ongoing (or error, no such cube number)
   */
  DLL_FUNCTION int cube_has_model(unsigned int cube);


  /**
   * Returns number of restored_preset dimensions and stores restored preset in *restored_preset pointer (mallocs).
   * Preset values should be in the range of [-2,+2] (aprox. Normal(0,I) distribution).
   *
   * @param cube cube number
   * @param preset pointer to reduced dimensional table of values (2 or 3)
   * @param DIM_PRESET number of dimension in preset (2 or 3)
   * @param restored_preset pointer to float* which is malloced() and into which restored high-dimensional value is stored
   * @return returns dimensions of *restored_preset or 0 in case of failure
   */
  DLL_FUNCTION int cube_restore(unsigned int cube,
				float* preset, unsigned int DIM_PRESET,
				float** restored_preset);

  /**
   * Returns number of input dimensions (low dimensional reduced space).
   * 
   * @param cube cube number
   * @return number of reduced (low dimensional) space in cube model or zero in case of error
   */
  DLL_FUNCTION int cube_get_input_dimensions(unsigned int cube);

  /**
   * Returns number of restored dimensions (original high dimensions in data)
   *
   * @param cube cube number
   * @return number of restored dimensions (original dimensions in data) or zero in case of error
   */
  DLL_FUNCTION int cube_get_restored_dimensions(unsigned int cube);

  
  /**
   * Exports model's parameters to 1d table, malloc()s memory and stores pointer to *model_params, return model parameters dimensions.
   *
   * @param cube cube number
   * @param model_params pointer to pointer (double*) that will be allocated and keeps model's parameters, must be free()ed with (*model_params).
   * @return returns number of model's parameters or zero in case of error
   */
  DLL_FUNCTION int cube_export_model(unsigned int cube,
				     double** model_params);

  
  /**
   * Imports parameters to model from 1d table [returned by cube_export_model().
   *
   * @param cube cube number
   * @param model_params pointer to 1d table of model parameters (for cube)
   * @param DIM_PARAMS number of dimensions (length) in model_params
   * @return returns 1 in case of success and 0 in case of error
   */
  DLL_FUNCTION int cube_import_model(unsigned int cube,
				     double* model_params, unsigned int DIM_PARAMS);

  /**
   * Saves cube's parameters to a file.
   *
   * @param cube cube number
   * @param filename path+filename in which to save cube's parameters
   * @return 1 in case of success and 0 in case of error
   */
  DLL_FUNCTION int cube_save(unsigned int cube,
			     char* filename);

  /**
   * Loads cube's parameters from a file.
   *
   * @param cube cube number (must be allocated)
   * @param filename path+filename to file from which to load model's parameters
   * @return 1 in case of success and 0 in case of error
   */
  DLL_FUNCTION int cube_load(unsigned int cube,
			     char* filename);

#ifdef __cplusplus
};
#endif

#endif
