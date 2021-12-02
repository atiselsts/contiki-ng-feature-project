This is the code of the final demonstrator of the [FEATURE project](https://www.edi.lv/en/projects/energy-efficient-health-and-behavior-monitoring-with-wearable-devices-and-the-internet-of-things-feature/).

It shows on-board activity recognition coupled with energy-efficient communication.


## Technical details

Classifier: Random Forest, number of trees: 50, max depth: 9.

Features used in the classifier (selected for the best accuracy / energy tradeoff):

     tTotalAcc-min()
     tTotalAcc-energy()
     tTotalAcc-entropy()
     tTotalAccMagSq-min()
     tTotalAccMagSq-energy()
     tTotalAccMagSq-max()
     tTotalAccJerk-mean()
     tTotalAccJerk-energy()
     tTotalAccJerkMagSq-iqr()

The following activity classes are recognized by the classifier:
   0 WALKING
   1 WALKING_UPSTAIRS
   2 WALKING_DOWNSTAIRS
   3 SITTING
   4 STANDING
   5 LAYING

## Acknowledgements

This work was supported by the ERDF Activity 1.1.1.2 "Post-doctoral Research Aid'' (№1.1.1.2/VIAA/2/18/282).
