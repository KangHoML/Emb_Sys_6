import dlib
import cv2
import os
import numpy as np
import face_recognition

face_detector = dlib.get_frontal_face_detector()
embedded_faces = np.load('embedded_user_faces.npy')

def get_face_embedding(crop_image):
    if crop_image is None or crop_image.size == 0:
        return None
    rgb_image = cv2.cvtColor(crop_image, cv2.COLOR_BGR2RGB)
    face_locations = face_recognition.face_locations(rgb_image)
    if len(face_locations) == 0:
        return None
    face_encodings = face_recognition.face_encodings(rgb_image, face_locations)
    return face_encodings[0]

def calculate_distance(embedded_faces, new_face_embedding):
    if new_face_embedding is None:
        return None
    distances = face_recognition.face_distance(embedded_faces, new_face_embedding)
    min_distance = np.min(distances)
    min_distance_index = np.argmin(distances)
    return min_distance, min_distance_index
 
def check_identity(rgb_image, embedded_faces, threshold=0.4):
    new_embedding = get_face_embedding(rgb_image)
    
    if new_embedding is not None:
        result = calculate_distance(embedded_faces, new_embedding)
        if result is not None:
            min_distance, min_distance_index = result
                
            if min_distance < threshold:
                if min_distance_index == 0 or min_distance_index == 1 or min_distance_index == 2:
                    print(f"Face matched Choi, distance: {min_distance}")
                elif min_distance_index == 3 or min_distance_index == 4 or min_distance_index == 5:
                    print(f"Face matched Hong, distance: {min_distance}")
                else:
                    print(f"Face matched Kang, distance: {min_distance}")
                return 1
            else:
                print(f"Face not matched.")
                return 0
        else:
            print("Error in calculating distance.")
            return 0
    else:
        print("No valid embedding found for the image.")
        return 0

frameWidth = 320
frameHeight = 240
cap = cv2.VideoCapture(0)
cap.set(3, frameWidth)
cap.set(4, frameHeight)
cap.set(cv2.CAP_PROP_FPS, 15)

while True:    
    success, img = cap.read()
    
    image = cv2.resize(img, dsize=(640,480), interpolation=cv2.INTER_AREA)
    img_gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
    
    key = cv2.waitKey(1)
    if key & 0xFF == ord('q'):
        break

    faces = face_detector(img)
    
    if len(faces) >= 1 :
        state = 1
        
        for f in faces:
            horizon = f.right()-f.left()
            vertical = f.bottom()-f.top()
            size = horizon * vertical
            
            if size > 15000 :
                cv2.rectangle(img, (f.left(), f.top()), (f.right(), f.bottom()), (0,0,255),2)
                
                crop = img[f.top():f.bottom(), f.left():f.right()]
                if crop is None or crop.size  == 0:
                    continue 
                result = check_identity(crop, embedded_faces)

                if result == 1:
                    cv2.rectangle(img, (f.left(), f.top()), (f.right(), f.bottom()), (0,255,0),2)
            
    cv2.imshow("Cam", img)