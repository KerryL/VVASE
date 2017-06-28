/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  componentManager.h
// Date:  6/20/2017
// Auth:  K. Loux
// Licn:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Module for manager-component pattern, where factories can be
//        registered with the manager to create components as required.

#ifndef COMPONENT_MANAGER_H_

// Standard C++ headers
#include <vector>
#include <memory>
#include <string>

namespace VVASE
{

template <typename T>
class ComponentBase
{
public:
    static std::unique_ptr<T> Create() { return T::Create(); }
    static std::string GetName() { return T::GetName(); }
};

class RegisterableComponent : public ComponentBase<RegisterableComponent>
{
    // Derived classes must implement:
    // static std::unique_ptr<T> Create();
    // static std::string GetName();

protected:
    RegisterableComponent() = default;
};

template <typename Base>
class ComponentManager
{
public:
    static_assert(std::is_base_of<RegisterableComponent, Base>::value, "Base must be a descendant of RegisterableComponent");

    template <typename ComponentType>
    void Register();

    template <typename ComponentType>
    void Unregister();

    typedef std::unique_ptr<Base>() Factory;

    struct ComponentInfo
    {
        ComponentInfo(Factory create, const std::string& name) : create(create), name(name) {}

        Factory create;
        std::string name;
    };

    const std::vector<ComponentInfo>& GetInfo() const { return componentInfo; }

private:
    std::vector<ComponentInfo> componentInfo;
};

template <typename ComponentType>
void ComponentManager<Base>::Register()
{
    static_assert(std::is_base_of<Base, ComponentType>::value, "ComponentType must be a descendant of Base");
    componentInfo.push_back(ComponentInfo(ComponentType::Create(), ComponentType::GetName()));
}

template <typename ComponentType>
void ComponentManager<Base>::Unregister()
{
    static_assert(std::is_base_of<Base, ComponentType>::value, "ComponentType must be a descendant of Base");
    // TODO:  Implement
}

}// namespace VVASE

#endif// COMPONENT_MANAGER_H_
