/**
 * @file Transform.h
 * @author Echo 
 * @Date 24-8-3
 * @brief 
 */

#pragma once
#include "Camera.h"
#include "Math/MathTypes.h"

class Transform
{
    friend class Function::GameObject;

public:
    Transform(Function::GameObject* owner = nullptr);

    static Transform Identity();

    Vector3      GetPosition() const;
    ::Transform& SetPosition(Vector3 pos, bool delay);
    ::Transform& SetPosition(Vector3 pos);
    void    Translate(Vector3 pos, bool delay = true);
    Vector3 GetWorldPosition() const { return world_position_; }

    const Rotator& GetRotation() const;
    Transform&     SetRotation(const Rotator& rot, bool delay);
    Transform&     SetRotation(const Rotator& rot);
    void           Rotate(const Rotator& rot, bool delay = true);

    Vector3      GetScale() const;
    ::Transform& SetScale(Vector3 scale, bool delay);
    ::Transform& SetScale(Vector3 scale);

    Vector3 GetForwardVector() const;
    Vector3 GetUpVector() const;
    Vector3 GetRightVector() const;
    String  ToString();

    glm::mat4 GetMat4();

    /**
     * 0->position
     * 其他位置的参数还不知道应该装什么
     * @return
     */
    glm::mat4 ToGPUMat4();

    void ApplyModify(const Vector3& pos, const ::Rotator& rot, const Vector3&scale);

    Transform& operator+=(const Transform& other);
    Transform& operator-=(const Transform& other);
    Transform  operator-(const Transform& other) const;
    Transform  operator+(const Transform& other) const;

protected:
    Matrix4x4 composited_mat_;
    Vector3   position_;
    Vector3   scale_;
    Rotator   rotator_;

    Vector3 world_position_;
    Vector3 world_scale_;
    Rotator world_rotator_;

    bool                  composited_mat_dirty_;
    Function::GameObject* owner_;
};
