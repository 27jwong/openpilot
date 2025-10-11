import lightgbm as lgb  
import pickle  
import numpy as np  
  
class LGBMTorqueModel:  
    def __init__(self, model_path):  
        """Load pre-trained LGBM model from file"""  
        with open(model_path, 'rb') as f:  
            self.model = pickle.load(f)  
        self.feature_names = ['speed', 'curvature', 'actual_lateral_accel', 'roll', 'steer_ratio', 'friction', 'error']  
      
    def predict(self, features):  
        """Predict torque from input features"""  
        if len(features) != len(self.feature_names):  
            raise ValueError(f"Expected {len(self.feature_names)} features, got {len(features)}")  
          
        features_array = np.array(features).reshape(1, -1)  
        return float(self.model.predict(features_array)[0])