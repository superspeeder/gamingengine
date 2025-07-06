//
// Created by andy on 7/4/25.
//

#pragma once

#include <functional>
#include <list>
#include <map>
#include <memory>
#include <set>
#include <unordered_set>
#include <utility>

namespace engine::scene {

    class scene;
    class scene_object;

    template <typename... Args>
    class update_phase;

    /**
     * An object within a scene
     */
    class scene_object : public std::enable_shared_from_this<scene_object> {
      protected:
        inline scene_object(std::weak_ptr<scene> scene, const uint64_t id) : m_scene(std::move(scene)), m_id(id) {}

      public:
        virtual ~scene_object() = default;

        scene_object(const scene_object &other)                = delete;
        scene_object(scene_object &&other) noexcept            = default;
        scene_object &operator=(const scene_object &other)     = delete;
        scene_object &operator=(scene_object &&other) noexcept = default;

        template <std::derived_from<scene_object> T, typename... Args>
        [[nodiscard]] static std::shared_ptr<T> create_orphaned(Args &&...args) {
            // ReSharper disable once CppSmartPointerVsMakeFunction
            return create(std::weak_ptr<scene>(), 0, std::forward<Args>(args)...);
        }

        [[nodiscard]] inline const std::weak_ptr<scene> &get_scene() const noexcept { return m_scene; }
        [[nodiscard]] inline uint64_t                    get_id() const noexcept { return m_id; }
        [[nodiscard]] std::string_view                   get_name() const { return m_name; }

        void set_parent(const std::shared_ptr<scene_object> &parent);

        virtual void update(double delta);

        // TODO: draw system

      protected:
        virtual void on_attach_to_scene() {}
        virtual void on_detach_from_scene() {}
        virtual void on_child_added(const std::shared_ptr<scene_object> &child) {}
        virtual void on_child_removed(const std::shared_ptr<scene_object> &child) {}
        virtual void on_parent_changed(
            const std::shared_ptr<scene_object> &old_parent, const std::shared_ptr<scene_object> &new_parent
        ) {}

      private:
        std::weak_ptr<scene> m_scene;
        uint64_t             m_id;

        std::shared_ptr<scene_object>           m_parent;
        std::set<std::shared_ptr<scene_object>> m_children;

        std::string m_name = std::string();

        void set_name(const std::string &name);

        friend class scene;

        void internal_attach_to_scene(const std::shared_ptr<scene> &scene);
        void internal_detach_from_scene();

        void _add_child(const std::shared_ptr<scene_object> &child);
        void _set_parent(const std::shared_ptr<scene_object> &parent);

        template <std::derived_from<scene_object> T, typename... Args>
        [[nodiscard]] static std::shared_ptr<T> create(std::weak_ptr<scene> &&scene, const uint64_t id, Args &&...args) {
            // ReSharper disable once CppSmartPointerVsMakeFunction
            return std::shared_ptr<T>(new T(scene, id, std::forward<Args>(args)...));
        }
    };

    template <typename... Args>
    class update_phase {
      public:
        using update_func = void (scene_object::*)(Args...);
        explicit update_phase(const update_func f, std::unordered_set<std::shared_ptr<scene_object>> &set)
            : m_objects(set), m_update_function(f) {}

        void run_updates(Args... args) {
            for (const auto &object : m_objects) {
                std::invoke(m_update_function, object, std::forward<Args>(args)...);
            }
        }

      private:
        std::unordered_set<std::shared_ptr<scene_object>> &m_objects;
        update_func                                        m_update_function;
    };

    struct update_group {
        std::unordered_set<std::shared_ptr<scene_object>> objects;

        update_phase<double> on_update{&scene_object::update, objects};

        inline void insert(const std::shared_ptr<scene_object> &object) { objects.insert(object); }

        inline void update(const double delta) { on_update.run_updates(delta); }
    };

    class scene : std::enable_shared_from_this<scene> {
      public:
        /**
         * @tparam T The scene object type
         * @tparam Args
         * @param args
         * @return A pair [id, object]
         */
        template <std::derived_from<scene_object> T, typename... Args>
        std::pair<uint64_t, std::shared_ptr<scene_object>> emplace_object(Args &&...args) {
            const auto id = ++m_last_object_id;
            const auto &[it, s] = m_objects.emplace(
                id, scene_object::create<T>(weak_from_this(), id, std::forward<Args>(args)...)
            );
            it->second->on_attach_to_scene();
            return {it->first, it->second};
        }

        /**
         * @tparam T The scene object type
         * @tparam Args
         * @param name
         * @param args
         * @return A pair [id, object]
         */
        template <std::derived_from<scene_object> T, typename... Args>
        std::pair<uint64_t, std::shared_ptr<scene_object>> emplace_object_named(const std::string &name, Args &&...args) {
            const auto id = ++m_last_object_id;
            const auto &[it, s] = m_objects.emplace(
                id, scene_object::create<T>(weak_from_this(), id, std::forward<Args>(args)...)
            );

            it->second->set_name(name);

            m_named_objects[name] = it->second;

            it->second->on_attach_to_scene();
            return {it->first, it->second};
        }

        template <std::derived_from<scene_object> T, typename... Args>
        std::pair<uint64_t, std::shared_ptr<scene_object>>
        emplace_object_ug(const std::shared_ptr<update_group> &update_group, Args &&...args) {
            const auto pair = emplace_object<T>(std::forward<Args>(args)...);
            update_group->insert(pair.second);
            return pair;
        }

        template <std::derived_from<scene_object> T, typename... Args>
        std::pair<uint64_t, std::shared_ptr<scene_object>>
        emplace_object_named_ug(const std::string &name, const std::shared_ptr<update_group> &update_group, Args &&...args) {
            const auto pair = emplace_object_named<T>(name, std::forward<Args>(args)...);
            update_group->insert(pair.second);
            return pair;
        }

        std::shared_ptr<scene_object> get_scene_object(const std::string &name) const;
        std::shared_ptr<scene_object> get_scene_object(uint64_t id) const;

        bool has_scene_object(const std::string &name) const;
        bool has_scene_object(uint64_t id) const;

        uint64_t add_resource(const std::shared_ptr<void> &resource);
        uint64_t add_resource(const std::string &name, const std::shared_ptr<void> &resource);

        std::shared_ptr<void> get_resource(const std::string &name) const;
        std::shared_ptr<void> get_resource(uint64_t id) const;

        bool has_resource(const std::string &name) const;
        bool has_resource(uint64_t id) const;

        std::shared_ptr<update_group> push_end_new_update_group();
        std::shared_ptr<update_group> push_front_new_update_group();
        std::shared_ptr<update_group> push_new_update_group_after(const std::shared_ptr<update_group> &group);
        std::shared_ptr<update_group> push_new_update_group_before(const std::shared_ptr<update_group> &group);

        void update(double delta) const;

      private:
        uint64_t m_last_object_id   = 0;
        uint64_t m_last_resource_id = 0;

        std::vector<std::shared_ptr<update_group>> m_update_phases;

        std::list<std::shared_ptr<update_group>> m_update_groups;

        std::map<std::shared_ptr<update_group>, typename decltype(m_update_groups)::iterator> m_update_group_map;

        std::map<uint64_t, std::shared_ptr<scene_object>> m_objects;
        std::map<uint64_t, std::shared_ptr<void>>         m_resources;

        std::map<std::string, std::shared_ptr<scene_object>> m_named_objects;
        std::map<std::string, std::shared_ptr<void>>         m_named_resources;
    };
} // namespace engine::scene
