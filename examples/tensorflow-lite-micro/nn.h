#ifndef NN_H
#define NN_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/* Set up the neural network model for later usage */
int nn_setup(void);
/* Run inference (classification or regression) using the neural network model */
int nn_run_inference(void);

#ifdef __cplusplus
}  /* extern "C" */
#endif  /* __cplusplus */

#endif /* NN_H */
