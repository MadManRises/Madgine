#pragma once

#include "scenecomponentcollector.h"

#include "Madgine/app/globalapibase.h"
#include "Madgine/app/globalapicollector.h"
#include "Meta/serialize/container/noparent.h"

#include "Modules/threading/datamutex.h"

#include "scenecomponentbase.h"

#include "Modules/uniquecomponent/uniquecomponentcontainer.h"

#include "entity/entitycomponentcollector.h"

#include "Generic/keyvalue.h"

#include "Generic/container/concatIt.h"

#include "Meta/math/vector3.h"

#include "Generic/intervalclock.h"

#include "Modules/threading/customclock.h"

#include "scenecontainer.h"

namespace Engine {
namespace Scene {
    struct MADGINE_SCENE_EXPORT SceneManager : App::GlobalAPI<SceneManager> {

        using Self = SceneManager;

        SceneManager(App::Application &app);
        SceneManager(const SceneManager &) = delete;
        ~SceneManager();

        virtual std::string_view key() const override;

        void updateFrame(Closure<ByteBufferImpl<Matrix4[]>(Entity::SkeletonPtr)> callback);

        void clear();

        void pause();
        bool unpause();
        bool isPaused() const;
        const Threading::CustomClock &clock() const;

        IntervalClock<Threading::CustomTimepoint> &simulationClock();
        IntervalClock<Threading::CustomTimepoint> &animationClock();

        SceneContainer &container(std::string_view name);

        /* decltype(auto) entities()
        {
            return concatIt(mEntities, mLocalEntities) | std::views::transform(EntityHelper {});
        }*/

        template <typename T>
        T &getComponent()
        {
            return static_cast<T &>(getComponent(UniqueComponent::component_index<T>()));
        }
        SceneComponentBase &getComponent(size_t i);
        size_t getComponentCount();

        struct SceneScope {

            template <typename Rec>
            struct receiver : Execution::algorithm_receiver<Rec> {

                template <typename O>
                friend bool tag_invoke(get_binding_d_t, receiver &rec, std::string_view name, O &out)
                {
                    if (name == "Scene") {
                        out = rec.mScene;
                        return true;
                    } else {
                        return Execution::resolve_var_d(rec.mRec, name, out);
                    }
                }

                SceneManager *mScene;
            };

            template <typename Inner>
            struct sender : Execution::algorithm_sender<Inner> {
                template <typename Rec>
                friend auto tag_invoke(Execution::connect_t, sender &&sender, Rec &&rec)
                {
                    return Execution::algorithm_state<Inner, receiver<Rec>> { std::forward<Inner>(sender.mSender), std::forward<Rec>(rec), sender.mScene };
                }

                SceneManager *mScene;
            };

            template <typename Inner>
            friend auto operator|(Inner &&inner, SceneScope &&scope)
            {
                return sender<Inner> { { {}, std::forward<Inner>(inner) }, scope.mScene };
            }

            SceneManager *mScene;
        };

        template <typename Sender>
        void addBehavior(Sender &&sender)
        {
            Debug::ContextInfo *context = &Debug::Debugger::getSingleton().createContext();
            mLifetime.attach(std::forward<Sender>(sender) | SceneScope { this } | Execution::with_debug_location<Execution::SenderLocation>() | Execution::with_sub_debug_location(context) | Log::log_error());
            mBehaviorContexts.emplace_back(context);
        }

        void addAnimation(Entity::AnimationState *animation);
        bool stopAnimation(Entity::AnimationState *animation);

        Threading::DataMutex &mutex();

        template <typename T>
        Entity::EntityComponentList<T> &entityComponentList()
        {
            return static_cast<Entity::EntityComponentList<T> &>(*mEntityComponentLists.at(UniqueComponent::component_index<T>()));
        }

        Entity::EntityComponentListBase &entityComponentList(size_t index)
        {
            return *mEntityComponentLists.at(index);
        }

        const Entity::EntityComponentListBase &entityComponentList(size_t index) const
        {
            return *mEntityComponentLists.at(index);
        }

    protected:
        virtual Threading::Task<bool> init() final;
        virtual Threading::Task<void> finalize() final;

    private:
        struct Clock : Threading::CustomClock {
            virtual std::chrono::steady_clock::time_point get(std::chrono::steady_clock::time_point timepoint) const override;
            virtual std::chrono::steady_clock::time_point revert(std::chrono::steady_clock::time_point timepoint) const override;

            std::chrono::steady_clock::duration mPauseAcc = std::chrono::steady_clock::duration::zero();
            std::chrono::steady_clock::time_point mPauseStart;
            std::atomic<size_t> mPauseStack = 0;
        } mClock;

        Threading::DataMutex mMutex;
        Execution::Lifetime mLifetime;

        IntervalClock<Threading::CustomTimepoint> mSimulationClock;
        IntervalClock<Threading::CustomTimepoint> mAnimationClock;
        IntervalClock<std::chrono::steady_clock::time_point> mFrameClock;

        std::vector<Debug::ContextInfo *> mBehaviorContexts;
                
        std::mutex mAnimationMutex;
        std::vector<Entity::AnimationState*> mAnimationStates;

        Entity::EntityComponentListContainer<std::vector<Placeholder<0>>> mEntityComponentLists;

    public:
        MEMBER_OFFSET_CONTAINER(mSceneComponents, , SceneComponentContainer<std::set<Placeholder<0>, KeyCompare<Placeholder<0>>>>);

        struct ContainerData {
            ContainerData(SceneManager &manager);
            ContainerData(ContainerData &&) = delete;

            Serialize::NoParent<SceneContainer> mContainer;            
        };
        std::map<std::string, ContainerData> mContainers;
    };

}
}

REGISTER_TYPE(Engine::Scene::SceneManager)
REGISTER_TYPE(Engine::Serialize::NoParent<Engine::Scene::SceneManager>)