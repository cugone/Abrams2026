#pragma once

#include "Engine/Services/IService.hpp"

#include "Engine/Physics/Joint.hpp"
#include "Engine/Physics/ForceGenerator.hpp"
#include "Engine/Physics/PhysicsTypes.hpp"
#include "Engine/Physics/SpringJoint.hpp"
#include "Engine/Physics/RodJoint.hpp"
#include "Engine/Physics/CableJoint.hpp"

struct PhysicsSystemDesc;

class IPhysicsService : public IService {
public:
    virtual ~IPhysicsService() noexcept = default;

    virtual void AddObject(RigidBody* body) = 0;
    virtual void AddObjects(std::vector<RigidBody*> bodies) = 0;
    virtual void RemoveObject(RigidBody* body) = 0;
    virtual void RemoveObjects(std::vector<RigidBody*> bodies) = 0;
    virtual void RemoveAllObjects() noexcept = 0;
    virtual void RemoveAllObjectsImmediately() noexcept = 0;

    virtual void Enable(bool enable) = 0;
    virtual void SetGravity(const Vector2& new_gravity) = 0;
    [[nodiscard]] virtual Vector2 GetGravity() const noexcept = 0;
    virtual void SetDragCoefficients(const Vector2& k1k2) = 0;
    virtual void SetDragCoefficients(float linearCoefficient, float squareCoefficient) = 0;
    [[nodiscard]] virtual std::pair<float, float> GetDragCoefficients() const noexcept = 0;
    [[nodiscard]] virtual const PhysicsSystemDesc& GetWorldDescription() const noexcept = 0;
    virtual void SetWorldDescription(const PhysicsSystemDesc& new_desc) = 0;
    virtual void EnableGravity(bool isGravityEnabled) noexcept = 0;
    virtual void EnableDrag(bool isDragEnabled) noexcept = 0;
    virtual void EnablePhysics(bool isPhysicsEnabled) noexcept = 0;


    template<typename JointDefType>
    requires(std::derived_from<JointDefType, JointDef>)
    Joint* CreateJoint(const JointDefType& defType) noexcept {
        std::unique_ptr<Joint> newJoint{};
        if constexpr(std::is_same_v<JointDefType, SpringJointDef>) {
            newJoint.reset(new SpringJoint(defType));
        } else if constexpr(std::is_same_v<JointDefType, RodJointDef>) {
            newJoint.reset(new RodJoint(defType));
        } else if constexpr(std::is_same_v<JointDefType, CableJointDef>) {
            newJoint.reset(new CableJoint(defType));
        } else {
            //Catch-all for user-defined joint definitions (possible vector for malicious types!)
            newJoint.reset(new JointDefType{defType});
        }
        auto* joint_ptr = newJoint.get();
        m_joints.emplace_back(std::move(newJoint));
        return joint_ptr;
    }


    template<typename ForceGeneratorType>
    ForceGeneratorType* CreateForceGenerator() {
        auto newFG = std::make_unique<ForceGeneratorType>();
        auto* new_fg_ptr = newFG.get();
        m_forceGenerators.emplace_back(newFG);
        return new_fg_ptr;
    }

    [[nodiscard]] virtual const std::vector<std::unique_ptr<Joint>>& Debug_GetJoints() const noexcept = 0;
    [[nodiscard]] virtual const std::vector<RigidBody*>& Debug_GetBodies() const noexcept = 0;

    virtual void Debug_ShowCollision(bool show) = 0;
    virtual void Debug_ShowWorldPartition(bool show) = 0;
    virtual void Debug_ShowContacts(bool show) = 0;
    virtual void Debug_ShowJoints(bool show) = 0;

protected:
    std::vector<std::unique_ptr<ForceGenerator>> m_forceGenerators{};
    std::vector<std::unique_ptr<Joint>> m_joints{};
    std::vector<RigidBody*> m_rigidBodies{};
    PhysicsSystemDesc m_desc{};
private:
    
};



class NullPhysicsService : public IPhysicsService {
public:
    virtual ~NullPhysicsService() noexcept { /* DO NOTHING */ }

    void AddObject([[maybe_unused]] RigidBody* body) override{};
    void AddObjects([[maybe_unused]] std::vector<RigidBody*> bodies) override{};
    void RemoveObject([[maybe_unused]] RigidBody* body) override{};
    void RemoveObjects([[maybe_unused]] std::vector<RigidBody*> bodies) override{};
    void RemoveAllObjects() noexcept override{};
    void RemoveAllObjectsImmediately() noexcept override{};

    void Enable([[maybe_unused]] bool enable) override{};
    void SetGravity([[maybe_unused]] const Vector2& new_gravity) override{};
    [[nodiscard]] Vector2 GetGravity() const noexcept override { return Vector2::Zero; };
    void SetDragCoefficients([[maybe_unused]] const Vector2& k1k2) override{};
    void SetDragCoefficients([[maybe_unused]] float linearCoefficient, [[maybe_unused]] float squareCoefficient) override{};
    [[nodiscard]] std::pair<float, float> GetDragCoefficients() const noexcept override { return std::make_pair(0.0f, 0.0f); };
    [[nodiscard]] const PhysicsSystemDesc& GetWorldDescription() const noexcept override { return m_desc; };
    void SetWorldDescription([[maybe_unused]] const PhysicsSystemDesc& new_desc) override { m_desc = new_desc; };
    void EnableGravity([[maybe_unused]] bool isGravityEnabled) noexcept override{};
    void EnableDrag([[maybe_unused]] bool isDragEnabled) noexcept override{};
    void EnablePhysics([[maybe_unused]] bool isPhysicsEnabled) noexcept override{};

    template<typename JointDefType>
    Joint* CreateJoint([[maybe_unused]] const JointDefType& defType) noexcept { return nullptr; }

    template<typename ForceGeneratorType>
    ForceGeneratorType* CreateForceGenerator() { return nullptr; }

    [[nodiscard]] const std::vector<std::unique_ptr<Joint>>& Debug_GetJoints() const noexcept override { return m_joints; };
    [[nodiscard]] const std::vector<RigidBody*>& Debug_GetBodies() const noexcept override { return m_rigidBodies; };

    void Debug_ShowCollision([[maybe_unused]] bool show) override{};
    void Debug_ShowWorldPartition([[maybe_unused]] bool show) override{};
    void Debug_ShowContacts([[maybe_unused]] bool show) override{};
    void Debug_ShowJoints([[maybe_unused]] bool show) override{};

protected:
private:
};
