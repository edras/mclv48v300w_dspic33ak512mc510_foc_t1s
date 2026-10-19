#ifndef __MODEL_JSON_H__
#define __MODEL_JSON_H__

const char recognition_model_string_json[] = {"{\"NumModels\":1,\"ModelIndexes\":{\"0\":\"SMART_MOTOR_PRED_MAINT_FOLD_0\"},\"ModelDescriptions\":[{\"Name\":\"SMART_MOTOR_PRED_MAINT_FOLD_0\",\"ClassMaps\":{\"1\":\"fingertap\",\"2\":\"normal\",\"3\":\"notch_long\",\"4\":\"notch_short\",\"0\":\"Unknown\"},\"ModelType\":\"DecisionTreeEnsemble\",\"FeatureFunctions\":[\"Mean\",\"StandardDeviation\",\"StandardDeviation\",\"Minimum\",\"Minimum\",\"Maximum\",\"Maximum\",\"GlobalPeaktoPeak\",\"GlobalPeaktoPeak\",\"25thPercentile\",\"25thPercentile\",\"75thPercentile\",\"Kurtosis\",\"MeanCrossingRate\",\"MeanCrossingRate\",\"AverageDemeanedEnergy\"]}]}"};

int32_t recognition_model_string_json_len = sizeof(recognition_model_string_json);

#endif /* __MODEL_JSON_H__ */
