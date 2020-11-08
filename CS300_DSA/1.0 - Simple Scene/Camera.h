// Camera.h
// -- 3D camera class
// cs250 5/19

#ifndef CS250_CAMERA_H
#define CS250_CAMERA_H


class Camera {
public:
    Camera(void);

    Camera(const glm::vec4 &E, const glm::vec4 &look, const glm::vec4 &rel,
           float fov, float aspect, float N, float F);

    glm::vec4 eye(void) const { return eye_point; }

    glm::vec4 right(void) const { return right_vector; }

    glm::vec4 up(void) const { return up_vector; }

    glm::vec4 back(void) const { return back_vector; }

    glm::vec4 viewport(void) const;

    float near(void) const { return near_distance; }

    float far(void) const { return far_distance; }

    Camera &zoom(float factor);

    Camera &forward(float distance);

    Camera &yaw(float angle);

    Camera &pitch(float angle);

    Camera &roll(float angle);

    // The next three functions will be implemented in a later assignment.
    // DO NOT implement them in assignment #3.
    glm::mat4 static model(const Camera& cam);
    glm::mat4 static view(const Camera& cam);
    glm::mat4 static perspective(const Camera& cam);

    glm::mat4 GetPerspectiveGLM();
    glm::mat4 GetViewMatrixGLM();
private:
    glm::vec4 eye_point;
    glm::vec4 right_vector, up_vector, back_vector;
    float fov;
    float vp_width, vp_height, vp_distance,
            near_distance, far_distance;
};



#endif

