import os
import numpy as np
import tensorflow as tf

from tensorflow import keras
from tensorflow.keras.preprocessing.image import ImageDataGenerator
from tensorflow.keras.optimizers import Adam

os.environ['TF_CPP_MIN_LOG_LEVEL'] = '2'

# Residual Block
class ResidualBlock(keras.layers.Layer):
    def __init__(self, filters, strides=1, activation="relu", dropout_rate = 0.0, **kwargs):
        super().__init__(**kwargs)
        self.activation = keras.activations.get(activation)
        self.conv = [
            keras.layers.Conv2D(filters, 3, strides=strides, padding="same", use_bias=False),
            keras.layers.BatchNormalization(),
            self.activation,
            keras.layers.Dropout(dropout_rate),
            keras.layers.Conv2D(filters, 3, strides=1, padding="same", use_bias=False),
            keras.layers.BatchNormalization()
        ]
        self.skip_layers = []
        if strides > 1:
            self.skip_layers = [
                keras.layers.Conv2D(filters, 1, strides=strides, padding="same", use_bias=False),
                keras.layers.BatchNormalization()
            ]
    
    def call(self, inputs):
        x = inputs
        for layer in self.conv:
            x = layer(x)
        short_cut = inputs
        for layer in self.skip_layers:
            short_cut = layer(short_cut)
        return self.activation(x + short_cut)
    
# ResNet Model
def build_model(input_shape, num_classes, dropout_rate=0.3):
    model = keras.models.Sequential()
    model.add(keras.layers.Conv2D(64, 7, strides=2, input_shape=input_shape,
                                  padding="same", use_bias=False))
    model.add(keras.layers.BatchNormalization())
    model.add(keras.layers.Activation("relu"))
    model.add(keras.layers.MaxPool2D(pool_size=3, strides=2, padding="same"))
    
    prev_filters = 64
    for filters in [64]*3 + [128]*4 + [256]*6 + [512]*3:
        strides = 1 if filters == prev_filters else 2
        model.add(ResidualBlock(filters, strides=strides, dropout_rate=dropout_rate))
        prev_filters = filters
    model.add(keras.layers.GlobalAvgPool2D())
    model.add(keras.layers.Flatten())
    model.add(keras.layers.Dense(num_classes, activation="softmax"))

    return model

# load the dataset
def load_and_preprocess_data(root:str, is_train=True):
    if is_train:
        datagen = ImageDataGenerator(rescale= 1./255,
                                   rotation_range=25,
                                   width_shift_range=0.05,
                                   height_shift_range=0.05,
                                   zoom_range=0.2,
                                   horizontal_flip=True,
                                   vertical_flip=True,
                                   fill_mode='nearest')
        data_root = os.path.join(root, 'train')
    else:
        datagen = ImageDataGenerator(rescale= 1./255)
        data_root = os.path.join(root, 'val')
    
    generator = datagen.flow_from_directory(data_root,
                                      batch_size=16,
                                      color_mode='rgb',
                                      class_mode='sparse',
                                      target_size=(224,224))
    return generator
    
def train(root:str, input_shape, num_classes):
    # data generator
    train_generator = load_and_preprocess_data(root, is_train=True)
    val_generator = load_and_preprocess_data(root, is_train=False) 

    # build the model
    model = build_model(input_shape, num_classes)
    model.compile(optimizer=Adam(), 
                  loss='sparse_categorical_crossentropy', 
                  metrics=['accuracy'])
    
    # fit the model
    model.fit(train_generator,
          validation_data = val_generator,
          epochs=20)
    
    # TFLite Model
    model.save('./model/face_detection.h5')

def convert_tf_lite(model_path):
    model = tf.keras.models.load_model(model_path,
                                       custom_objects={'ResidualBlock': ResidualBlock})
    converter = tf.lite.TFLiteConverter.from_keras_model(model)
    converter.optimizations = [tf.lite.Optimize.DEFAULT]
    converter.target_spec.supported_types = [tf.float16]
    tflite_model = converter.convert()
    
    # save the model
    with open('./model/model.tflite', 'wb') as f:
        f.write(tflite_model)
    
if __name__ == "__main__":
    root = "/home/ho/dataset/face_data/"
    train(root, input_shape=(224, 224, 3), num_classes=2)
    convert_tf_lite('./model/face_detection.h5')

