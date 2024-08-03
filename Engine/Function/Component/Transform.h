/**
 * @file Transform.h
 * @author Echo 
 * @Date 24-8-3
 * @brief 
 */

#pragma once
#include "Component.h"
#include "Math/MathTypes.h"

class Transform
{
    friend class GameObject;
public:
    Transform(Function::GameObject* owner);

    Vector3 GetPosition();
    void    SetPosition(Vector3 pos);

    const Rotator& GetRotation();
    void           SetRotation(const Rotator& rot);
    void           Rotate(const Rotator& rot);

    Vector3 GetScale();
    void    SetScale(Vector3 scale);

    Quaternion GetRotationQuaternion();

    /**
     * 将mat旋转四元数q
     * @param q
     */
    void SetRotationQuaternion(Quaternion q);

    Vector3 GetForwardVector() const;
    Vector3 GetUpVector() const;
    Vector3 GetRightVector() const;
    String  ToString();

    glm::mat4 GetMat4() const;

    /**
     * 0->position
     * 其他位置的参数还不知道应该装什么
     * @return
     */
    glm::mat4 ToGPUMat4();

    void ApplyModify(const Matrix4x4& modify);

protected:
    /**
     * 将变换矩阵mat_分解成 position, rotation, scale
     */
    void DecomposeMat();

    Matrix4x4             mat_;
    Vector3               position_;
    Vector3               scale_;
    Quaternion            rotation_;
    Rotator               rotator_;
    bool                  decompose_dirty_ = true;
    Function::GameObject* owner_;
};
