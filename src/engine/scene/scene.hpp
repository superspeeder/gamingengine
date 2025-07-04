//
// Created by andy on 7/4/25.
//

#pragma once

#include <map>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <utility>

namespace engine::scene {

    class scene;

    /**
     * An object within a scene
     */
    class scene_object : public std::enable_shared_from_this<scene_object> {
      protected:
        inline scene_object(std::shared_ptr<scene> scene, uint64_t id) : m_scene(std::move(scene)), m_id(id) {}

      public:
        virtual ~scene_object() = default;

        scene_object(const scene_object &other)                = delete;
        scene_object(scene_object &&other) noexcept            = default;
        scene_object &operator=(const scene_object &other)     = delete;
        scene_object &operator=(scene_object &&other) noexcept = default;

        template <std::derived_from<scene_object> T, typename... Args>
        [[nodiscard]] static std::shared_ptr<T> create_orphaned(Args &&...args) {
            // ReSharper disable once CppSmartPointerVsMakeFunction
            return create(nullptr, 0, std::forward<Args>(args)...);
        }

        [[nodiscard]] inline const std::shared_ptr<scene> &get_scene() const noexcept { return m_scene; }
        [[nodiscard]] inline uint64_t get_id() const noexcept { return m_id; }

      protected:
        virtual void on_attach_to_scene() {}
        virtual void on_detach_from_scene() {}

      private:
        std::shared_ptr<scene> m_scene;
        uint64_t               m_id;

        friend class scene;

        void internal_attach_to_scene(std::shared_ptr<scene> scene);

        template <std::derived_from<scene_object> T, typename... Args>
        [[nodiscard]] static std::shared_ptr<T> create(std::weak_ptr<scene> scene, Args &&...args) {
            // ReSharper disable once CppSmartPointerVsMakeFunction
            return std::shared_ptr<T>(new T(std::move(scene), std::forward<Args>(args)...));
        }
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
            const auto &[it, s] = m_objects.emplace(
                ++m_last_entity_id, scene_object::create<T>(weak_from_this(), std::forward<Args>(args)...)
            );
            return it->second;
        }

      private:
        uint64_t                                          m_last_entity_id = 0;
        std::map<uint64_t, std::shared_ptr<scene_object>> m_objects;
        std::map<uint64_t, std::shared_ptr<void>>         m_global_resources;
    };

} // namespace engine::scene
