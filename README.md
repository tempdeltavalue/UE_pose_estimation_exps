# UE_pose_estimation_exps
UE5, onnx, pose estimation

Here's my try to understand how to use ml models inside of UE using ONNX 

biggest question for now is how to move bones from code ? (for example how to pass the pose from cpp into animinstance)
https://forums.unrealengine.com/t/how-to-load-and-move-skeleton-asset-in-cpp-code/1284585

result of onnx converted model inside Unreal engine (check folder Content/Models)
left pytorch model, right converted to onnx model without quantization 
![imgonline-com-ua-twotoone-hIkCAebGqX25](https://github.com/tempdeltavalue/UE_pose_estimation_exps/assets/36921178/3e54d845-e5e5-4a86-90e3-e2c8a441d723)

colab for model conversion 
https://colab.research.google.com/drive/1oCpRIkPRSYDJXjwHbV2_Ev1Clhbomm6a?usp=sharing
