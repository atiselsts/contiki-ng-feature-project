#ifndef RF_H
#define RF_H

#define RF_NUM_CLASSES 6
#define RF_NUM_FEATURES 19
#define RF_NUM_TREES 50

/* The following activity classes are recognized by the classifier:
   0 WALKING
   1 WALKING_UPSTAIRS
   2 WALKING_DOWNSTAIRS
   3 SITTING
   4 STANDING
   5 LAYING
*/

/*
Features used in the classifier (selected for the best accuracy / energy tradeoff):

     tTotalAcc-min() for x, y, z
     tTotalAcc-energy() for x, y, z
     tTotalAcc-entropy() for x, y, z
     tTotalAccMagSq-min() for combined sample
     tTotalAccMagSq-energy() for combined sample
     tTotalAccMagSq-max() for combined sample
     tTotalAccJerk-mean() for x, y, z
     tTotalAccJerk-energy() for x, y, z
     tTotalAccJerkMagSq-iqr() for combined sample

9 separate features, some of them multi-axial, so in total 19 separate numbers.

*/

/* A note about scaling:
 * the classifier assumes that features are computed on data where 1g == 32.
 */

/* Perform a classification given a number of features */
int rf_classify_single(const float features[]);

#endif /* RF_H */
