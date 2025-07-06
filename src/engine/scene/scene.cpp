//
// Created by andy on 7/4/25.
//

#include "scene.hpp"

namespace engine::scene {
    void scene_object::set_name(const std::string &name) {
        m_name = name;
    }

    void scene_object::internal_attach_to_scene(const std::shared_ptr<scene> &scene) {
        m_scene = scene;
        on_attach_to_scene();
    }

    void scene_object::internal_detach_from_scene() {
        on_detach_from_scene();
        m_scene.reset();
    }

    void scene_object::_add_child(const std::shared_ptr<scene_object> &child) {
        m_children.insert(child);
    }

    void scene_object::_set_parent(const std::shared_ptr<scene_object> &parent) {
        m_parent = parent;
    }

    void scene_object::set_parent(const std::shared_ptr<scene_object> &parent) {
        const auto old_parent = m_parent;

        m_parent = parent;
        m_parent->_add_child(shared_from_this());

        on_parent_changed(old_parent, m_parent);
    }

    void scene_object::update(double delta) {}

    std::shared_ptr<scene_object> scene::get_scene_object(const std::string &name) const {
        if (const auto &it = m_named_objects.find(name); it != m_named_objects.end()) {
            return it->second;
        }
        return nullptr;
    }

    std::shared_ptr<scene_object> scene::get_scene_object(const uint64_t id) const {
        if (const auto &it = m_objects.find(id); it != m_objects.end()) {
            return it->second;
        }
        return nullptr;
    }

    bool scene::has_scene_object(const std::string &name) const {
        return m_named_objects.contains(name);
    }

    bool scene::has_scene_object(const uint64_t id) const {
        return m_objects.contains(id);
    }

    uint64_t scene::add_resource(const std::shared_ptr<void> &resource) {
        return m_resources.emplace(++m_last_resource_id, resource).first->first;
    }

    uint64_t scene::add_resource(const std::string &name, const std::shared_ptr<void> &resource) {
        const auto it           = m_resources.emplace(++m_last_resource_id, resource).first;
        m_named_resources[name] = it->second;
        return it->first;
    }

    std::shared_ptr<void> scene::get_resource(const std::string &name) const {
        if (const auto &it = m_named_resources.find(name); it != m_named_resources.end()) {
            return it->second;
        }
        return nullptr;
    }

    std::shared_ptr<void> scene::get_resource(const uint64_t id) const {
        if (const auto &it = m_resources.find(id); it != m_resources.end()) {
            return it->second;
        }
        return nullptr;
    }

    bool scene::has_resource(const std::string &name) const {
        return m_named_resources.contains(name);
    }

    bool scene::has_resource(const uint64_t id) const {
        return m_resources.contains(id);
    }

    std::shared_ptr<update_group> scene::push_end_new_update_group() {
        const auto it          = m_update_groups.emplace_back(std::make_shared<update_group>());
        m_update_group_map[it] = --m_update_groups.end();
        return it;
    }

    std::shared_ptr<update_group> scene::push_front_new_update_group() {
        const auto it          = m_update_groups.emplace_front(std::make_shared<update_group>());
        m_update_group_map[it] = m_update_groups.begin();
        return it;
    }

    std::shared_ptr<update_group> scene::push_new_update_group_after(const std::shared_ptr<update_group> &group) {
        const auto after = ++m_update_group_map[group];
        const auto it    = m_update_groups.emplace(after, std::make_shared<update_group>());
        return *it;
    }

    std::shared_ptr<update_group> scene::push_new_update_group_before(const std::shared_ptr<update_group> &group) {
        const auto after = m_update_group_map[group];
        const auto it    = m_update_groups.emplace(after, std::make_shared<update_group>());
        return *it;
    }

    void scene::update(const double delta) const {
        for (const auto &it : m_update_groups) {
            it->update(delta);
        }
    }
} // namespace engine::scene
