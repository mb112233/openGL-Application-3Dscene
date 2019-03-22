//
//  Camera.cpp
//  Lab5
//
//  Created by CGIS on 28/10/2016.
//  Copyright © 2016 CGIS. All rights reserved.
//

#include "Camera.hpp"

namespace gps {
    
    Camera::Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget)
    {
        this->cameraPosition = cameraPosition;
        this->cameraTarget = cameraTarget;
        this->cameraDirection = glm::normalize(cameraTarget - cameraPosition);
        this->cameraRightDirection = glm::normalize(glm::cross(this->cameraDirection, glm::vec3(0.0f, 1.0f, 0.0f)));
    }
    
    glm::mat4 Camera::getViewMatrix()
    {
        return glm::lookAt(cameraPosition, cameraPosition + cameraDirection , glm::vec3(0.0f, 1.0f, 0.0f));
    }

	glm::vec3 Camera::getCameraTarget()
	{
		return cameraTarget;
	}
	glm::vec3 Camera::getCameraDir()
	{
		return cameraDirection;
	}
    
    void Camera::move(MOVE_DIRECTION direction, float speed)
    {
        switch (direction) {
            case MOVE_FORWARD:
                cameraPosition += cameraDirection * speed;
                break;
                
            case MOVE_BACKWARD:
                cameraPosition -= cameraDirection * speed;
                break;
                
            case MOVE_RIGHT:
                cameraPosition += cameraRightDirection * speed;
                break;
                
            case MOVE_LEFT:
                cameraPosition -= cameraRightDirection * speed;
                break;
        }
    }
    
    void Camera::rotate(float verticalAngle, float horizontalAngle)
    {
		glm::vec3 right = glm::vec3(
			sin(glm::radians(horizontalAngle) - 3.14f / 2.0f),
			0,
			cos(glm::radians(horizontalAngle) - 3.14f / 2.0f)
		);
		glm::vec3 direction(
			cos(glm::radians(verticalAngle)) * sin(glm::radians(horizontalAngle)),
			sin(glm::radians(verticalAngle)),
			cos(glm::radians(verticalAngle)) * cos(glm::radians(horizontalAngle))
		);
		cameraDirection = direction;
		cameraRightDirection = right;
		//glm::vec3 up = glm::cross(right, direction);
    }
    
}
