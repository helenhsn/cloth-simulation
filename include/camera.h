#ifndef CAMERA_H
#define CAMERA_H

#include "glad.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
};

// Default camera values
const float YAW         = -90.0f;
const float PITCH       = 0.0f;
const float SPEED       =  10.0f;
const float SENSITIVITY =  0.10f;
const float ZOOM        =  30.0f;


// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:

    // constructor with vectors
    Camera(glm::vec3 pos = glm::vec3(0.0, 0.0, 0.0), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : m_front(glm::vec3(0.0f, 0.0f, -1.0f)), m_transSpeed(SPEED), m_rotSpeed(SENSITIVITY), m_zoom(ZOOM)
    {
        m_pos = pos;
        m_worldUp = up;
        m_yaw = yaw;
        m_pitch = pitch;
        updateCameraVectors();
    }

    glm::vec3 pos() {return m_pos;};
    
    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 viewMatrix()
    {
        return glm::lookAt(m_pos, m_pos + m_front, m_up);
    }

    glm::mat4 projectionMatrix(float &windowWidth, float &windowHeight)
    {
        return glm::perspective(glm::radians(m_zoom), windowWidth / windowHeight, 0.1f, 10000.0f);
    }

    // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void processTranslation(Camera_Movement direction, float deltaTime)
    {
        float velocity = m_transSpeed * deltaTime;
        switch(direction)
        {
            case Camera_Movement::FORWARD:
                m_pos += m_front * velocity;
                break;
            case Camera_Movement::BACKWARD:
                m_pos -= m_front * velocity;
                break;
            case Camera_Movement::LEFT:
                m_pos -= m_right * velocity;
                break;
            case Camera_Movement::RIGHT:
                m_pos += m_right * velocity;
                break;
            case Camera_Movement::UP:
                m_pos += m_up * velocity;
                break;
        }

    }

    // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void processRotation(float xoffset, float yoffset, GLboolean constrainPitch = true)
    {
        xoffset *= m_rotSpeed;
        yoffset *= m_rotSpeed;

        m_yaw   += xoffset;
        m_pitch += yoffset;

        // make sure that when pitch is out of bounds, screen doesn't get flipped
        if (constrainPitch)
        {
            if (m_pitch > 89.0f)
                m_pitch = 89.0f;
            if (m_pitch < -89.0f)
                m_pitch = -89.0f;
        }

        // update m_front, m_right and m_up Vectors using the updated Euler angles
        updateCameraVectors();
    }

private:
    // camera Attributes
    glm::vec3 m_pos;
    glm::vec3 m_front;
    glm::vec3 m_up;
    glm::vec3 m_right;
    glm::vec3 m_worldUp;
    // euler Angles
    float m_yaw;
    float m_pitch;
    // camera options
    float m_transSpeed;
    float m_rotSpeed;
    float m_zoom;
    // calculates the m_front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors()
    {
        // calculate the new m_front vector
        glm::vec3 front;
        float cosPitch = cos(glm::radians(m_pitch));
        front.x = cos(glm::radians(m_yaw)) * cosPitch;
        front.y = sin(glm::radians(m_pitch));
        front.z = sin(glm::radians(m_yaw)) * cosPitch;
        m_front = glm::normalize(front);
        // also re-calculate the m_right and m_up vector
        m_right = glm::normalize(glm::cross(m_front, m_worldUp));  // normalize the vectors, because their length gets closer to 0 the more you look m_up or down which results in slower movement.
        m_up    = glm::normalize(glm::cross(m_right, m_front));
    }
};
#endif