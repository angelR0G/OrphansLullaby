#pragma once

#include <cassert>
#include <vector>
#include <tuple>
#include "typelist.hpp"
#include "slotmap.hpp"
#include "tags.hpp"
#include "includeComponents.hpp"

struct PhysicsEngine;
struct SoundEngine;
struct GraphicEngine;

//Check max bytes for the mask and change number type
template <typename LIST>
using smallest_mask_type = 
    MetaP::IFType_t < (LIST::size() <= 8) 
        ,   uint8_t 
        ,   MetaP::IFType_t< (LIST::size() <= 16)
            ,   uint16_t
            ,   MetaP::IFType_t< (LIST::size() <= 32)
                ,   uint32_t
                ,   uint64_t
            >
        >
    >;
    
//Create type traits
/////////////////////////////////////////////////////////////////////////
template <typename List>
struct type_traits{
    static_assert(List::size() <= 64, "ERROR: Maximum 64 types in a list");
    using mask_type = smallest_mask_type<List>;

    //Return list size
    consteval static std::size_t size() noexcept { return List::size(); }

    //Return typetrait id
    template<typename Type>
    consteval static std::uint8_t id() noexcept {
        static_assert(List::template contains<Type>());
        return List:: template pos<Type>();
    }

    //Return typelist mask
    template <typename... Types>
    consteval static mask_type mask() noexcept { return (0| ... | (1<< id<Types>())); }
};

//Component traits
template <typename CompList>
struct comp_traits : type_traits<CompList>{};

//tag traits
template <typename TagList>
struct tag_traits : type_traits<TagList>{};
/////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////
//CREATE COMPONENT STORAGE
////////////////////////////////////////////////////////////////////////////////////
template <typename CompList, typename TagList, std::size_t Capacity = 4>
struct ComponentStorage{
    //Alias type traits
    using compInfo = comp_traits<CompList>;
    using tagInfo  = tag_traits<TagList>;

    //Transform component list into a slotmap tuple
    template <typename List>
    using to_tuple = MetaP::replace_t<std::tuple, List>;
    template <typename T>
    using to_slotmap = Slotmap<T, Capacity>;
    using storage_t  = to_tuple< MetaP::forall_insert_template_t<to_slotmap, CompList>>;
    
    using mask_type = smallest_mask_type<CompList>;
    
    //Get component mask
    template <typename Comp>
    static constexpr mask_type getMask() noexcept {
        return getMask(compInfo::template id<Comp>());
    }

    //Return component storage
    template <typename Comp>
    constexpr auto& getStorage() noexcept {
        constexpr auto id { compInfo:: template id<Comp>() };
        return std::get<id>(componentsStorage_);
    }
    
private:
    static constexpr mask_type getMask(std::uint8_t id) noexcept {return 1 << id;}
    storage_t componentsStorage_;
};


template <typename CompList, typename TagList, std::uint32_t Capacity = 4>
struct EntityManager{
    //Forward declaration 
    struct Entity;

    using storage = ComponentStorage<CompList, TagList, Capacity>;
    template <typename T>
    using to_keytype = typename Slotmap<T, Capacity>::key_type;

    using EntityID_t = std::size_t;

    /////////////////////////////////////////////////////////////
    //Class entity
    /////////////////////////////////////////////////////////////
    struct Entity{
        using keytype_list = MetaP::forall_insert_template_t<to_keytype, CompList>;
        using key_storage_t = MetaP::replace_t<std::tuple, keytype_list>;

        //Add new component to the entity
        template <typename Comp>
        void addComponent(to_keytype<Comp> key){
            cmp_mask |= storage::compInfo::template mask<Comp>();
            std::get< to_keytype<Comp> >(cmp_keys) = key;
        }

        //Remove a component to the entity
        template <typename Comp>
        void removeComponent(){
            to_keytype<Comp> key{0,0};
            cmp_mask ^= storage::compInfo::template mask<Comp>();
            std::get< to_keytype<Comp> >(cmp_keys) = key;
        }

        //Check if an entity has a component
        template <typename Comp>
        bool hasComponent() const noexcept {
            auto c_mask = storage::compInfo::template mask<Comp>();
            return cmp_mask & c_mask;
        }

        //Return a component key
        template <typename Comp>
        to_keytype<Comp> getComponentKey() const {
            assert(hasComponent<Comp>());
            return std::get< to_keytype<Comp> >(cmp_keys);
        }
        
        //Return id
        EntityID_t getId(){return id;}

        //Add a tag to the the entity
        template <typename Tag>
        constexpr void addTag(){
            tag_mask |= storage::tagInfo::template mask<Tag>();
        }

        //Add a tag to the the entity
        template <typename Tag>
        constexpr void removeTag(){
            tag_mask &= ~storage::tagInfo::template mask<Tag>();
        }

        //Check if an entity has the tag
        template <typename Tag>
        constexpr bool hasTag() const noexcept {
            auto t_mask = storage::tagInfo::template mask<Tag>();
            return tag_mask & t_mask;
        }

    private:
        EntityID_t id{nextId++};
        inline static EntityID_t nextId{1};
        typename storage::compInfo::mask_type cmp_mask{};
        typename storage::tagInfo::mask_type  tag_mask{};
        key_storage_t cmp_keys{};
    };

    EntityManager(std::size_t defSize = 100){
        entities_.reserve(defSize);
    }

    template <typename Comp, typename... InitTypes>
    Comp& addComponent(Entity& e, InitTypes&&... initVals){
        //Check if the entity has the component
        if(e.template hasComponent<Comp>()) {
            return getComponent<Comp>(e);
        }
        //static_assert(MetaP::is_sameType_v<Comp, void>);
        //Add the component in to the slotmap
        auto& sto = components_.template getStorage<Comp>();

        auto key = sto.addSlot(Comp{ std::forward<InitTypes>(initVals)... });

        //Save key in the entity
        e.template addComponent<Comp>(key);  
        auto& dataSM  = sto.getDataSlotmap();
        auto& indexSM  = sto.getIndexSlotmap();
        const auto k = indexSM[key.id].id;
        return dataSM[k];
    }

    //Get mask and erase component bit
    //Remove key from cmp_key
    //Remove slot from slotmap
    template <typename Comp, typename Sys>
    bool removeComponent(Entity& e, Sys* system){
        if(system != nullptr) system->deleteData(getComponent<Comp>(e));
        to_keytype<Comp> keyErase = e.template getComponentKey<Comp>();
        auto& sto = components_.template getStorage<Comp>();
        bool exito = sto.eraseSlot(keyErase);
        e.template removeComponent<Comp>();
        return true;
    }
    template <typename Comp>
    bool removeComponent(Entity& e){
        to_keytype<Comp> keyErase = e.template getComponentKey<Comp>();
        auto& sto = components_.template getStorage<Comp>();
        sto.eraseSlot(keyErase);
        e.template removeComponent<Comp>();
        return true;
    }
    
    // template <typename...Comps>
    // bool removeAllComponents(Entity& e){
    //     bool complete = false;
    //     auto comps = (true && ... && removeComponent<Comps>(e));
    //     if(comps) complete = true;
    //     return complete;
    // }

    //TODO: Unify getComponent functions
    template <typename Comp>
    Comp& getComponent(Entity const& e) {
        auto  key     = e.template getComponentKey<Comp>();
        auto& slotmap = components_.template getStorage<Comp>();
        auto& dataSM  = slotmap.getDataSlotmap();
        auto& indexSM  = slotmap.getIndexSlotmap();
        const auto k = indexSM[key.id].id;
        return dataSM[k];
    }
    // template <typename Comp>
    // Comp const& getComponent(Entity const& e) const{
    //     auto  key     = e.template getComponentKey<Comp>();
    //     auto& slotmap = components_.template getStorage<Comp>();
    //     auto& dataSM  = slotmap.getDataSlotmap();
    //     auto& indexSM  = slotmap.getIndexSlotmap();
    //     const auto k = indexSM[key.id].id;
    //     return dataSM[k];
    // }

    //Adds a tag to the entity tag mask
    template <typename Tag>
    void addTag(Entity& e) {
        e.template addTag<Tag>();
    }

    //Removes a tag from the entity tag mask
    template <typename Tag>
    void removeTag(Entity& e) {
        e.template removeTag<Tag>();
    }

    template <typename...InitTypes>
    auto& createEntity(InitTypes&&... initValues) {
        auto& ent = entities_.emplace_back(); 
        addComponent<BasicComponent>(ent, BasicComponent{std::forward<InitTypes>(initValues)...});
        return ent;
    }

    //Delete position, copy last element to the erased position
    void deleteEntity(EntityID_t id) {
        //Search entity
        int position = getEntityPositionById(id);

        //Entity found
        if(position >=0){
            //Delete entity
            if((size_t)position != entities_.size()-1){
                //Not last entity
                //Copy last entity to the erased index
                entities_[position] = entities_[entities_.size()-1];
                if(entities_[position].template hasComponent<CollisionComponent>()){
                    CollisionComponent& coll_cmp = getComponent<CollisionComponent>(entities_[position]);
                    coll_cmp.collision.updateEntity(static_cast<void*>(&entities_[position]));
                }
                if(entities_[position].template hasComponent<TriggerComponent>()){
                    TriggerComponent& trigger_cmp = getComponent<TriggerComponent>(entities_[position]);
                    trigger_cmp.collision.updateEntity(static_cast<void*>(&entities_[position]));
                }
            }
            entities_.pop_back();
        }
    }

    Entity* getEntityById(EntityID_t id){
        Entity* e{nullptr};
        for(size_t i{}; i < entities_.size() && e == nullptr; ++i){
            if(id == entities_[i].getId()){
                //Found entity
                e = &entities_[i];
            }
        }
        return e;
    }

    template <typename COMPLIST, typename TAGLIST>
    void foreach(auto&& funcion){
        foreach_imp(funcion, COMPLIST{}, TAGLIST{} );
    }

    template <typename COMPLIST, typename TAGLIST>
    void foreachOR(auto&& funcion){
        foreachOR_imp(funcion, COMPLIST{}, TAGLIST{} );
    }

    template <typename COMPLIST>
    void foreachORComp(auto&& funcion){
        foreachORComp_imp(funcion, COMPLIST{});
    }

    template <typename COMPLIST, typename TAGLIST>
    std::vector<Entity*> search(){
        std::vector<Entity*> vEntity = search_imp( COMPLIST{}, TAGLIST{} );
        return vEntity;
    }

    template <typename COMPLIST, typename TAGLIST>
    void foreachpairs(auto&& funcion){
        foreachpairs_imp(funcion, COMPLIST{}, TAGLIST{} );
    }

    template <typename COMPLISTFirst, typename COMPLISTSecond, typename TAGLISTFirst, typename TAGLISTSecond>
    void foreachpairs(auto&& funcion){
        foreachpairs_imp(funcion, COMPLISTFirst{}, COMPLISTSecond{}, TAGLISTFirst{}, TAGLISTSecond{} );
    }

    void searchMarkedEntities(PhysicsEngine*, SoundEngine*, GraphicEngine*, const double dt = -1);
private:
    int getEntityPositionById(EntityID_t id){
        int position = -1;
        bool found = false;
        for(size_t i{}; i < entities_.size() && !found; ++i){
            if(id == entities_[i].getId()){
                //Found entity
                found = true;
                position = i;
            }
        }
        return position;
    }
    
    template <typename... COMPS, typename... TAGS>
    void foreach_imp(auto&& funcion, MetaP::Typelist<COMPS...>, MetaP::Typelist<TAGS...>){
        for(auto& e : entities_){
            auto hasComps = (true && ... && e.template hasComponent<COMPS>());
            auto hasTags  = (true && ... && e.template hasTag<TAGS>());
            if(hasComps && hasTags && !e.template hasTag<MarkToDestroyTag>())
                funcion(e);
        }
    }
    
    template <typename... COMPS, typename... TAGS>
    void foreachOR_imp(auto&& funcion, MetaP::Typelist<COMPS...>, MetaP::Typelist<TAGS...>){
        for(auto& e : entities_){
            auto hasComps = (false || ... || e.template hasComponent<COMPS>());
            auto hasTags  = (false || ... || e.template hasTag<TAGS>());
            if(hasComps && hasTags && !e.template hasTag<MarkToDestroyTag>())
                funcion(e);
        }
    }

    template <typename... COMPS>
    void foreachORComp_imp(auto&& funcion, MetaP::Typelist<COMPS...>){
        for(auto& e : entities_){
            auto hasComps = (false || ... || e.template hasComponent<COMPS>());
            if(hasComps && !e.template hasTag<MarkToDestroyTag>())
                funcion(e);
        }
    }

    template <typename... COMPS, typename... TAGS>
    std::vector<Entity*> search_imp(MetaP::Typelist<COMPS...>, MetaP::Typelist<TAGS...>){
        std::vector<Entity*> vEntity;
        for(auto& e : entities_){
            auto hasComps = (true && ... && e.template hasComponent<COMPS>());
            auto hasTags = (true && ... && e.template hasTag<TAGS>());
            if(hasComps && hasTags && !e.template hasTag<MarkToDestroyTag>())
                vEntity.emplace_back(&e);
        }
        return vEntity;
    }
    template <typename... COMPS, typename... TAGS>
    void foreachpairs_imp(auto&& funcion, MetaP::Typelist<COMPS...>, MetaP::Typelist<TAGS...>){
        for(std::size_t i{}; i < entities_.size()-1; ++i){
            Entity& e = entities_[i]; 
            auto hasComps = (true && ... && e.template hasComponent<COMPS>());
            auto hasTags  = (true && ... && e.template hasTag<TAGS>());
            if(hasComps && hasTags && !e.template hasTag<MarkToDestroyTag>()){
                for(std::size_t j{i+1}; j < entities_.size(); ++j){
                    Entity& e1 = entities_[j];
                    auto hasComps1 = (true && ... && e1.template hasComponent<COMPS>());
                    auto hasTags1  = (true && ... && e1.template hasTag<TAGS>());
                    if(hasComps1 && hasTags1 && !e1.template hasTag<MarkToDestroyTag>()){
                        funcion(e, e1);
                    }
                }
            }
        }
    }

    /*template <typename... COMPSFirst, typename... COMPSSecond, typename... TAGSFirst, typename... TAGSSecond>
    void foreachpairs_imp(auto&& funcion, MetaP::Typelist<COMPSFirst...>, MetaP::Typelist<COMPSSecond...>
                                        , MetaP::Typelist<TAGSFirst...> , MetaP::Typelist<TAGSSecond...>){
        for(std::size_t i{}; i < entities_.size()-1; ++i){
            Entity& e = entities_[i]; 
            auto hasCompsF = (true && ... && e.template hasComponent<COMPSFirst>());
            auto hasTagsF  = (true && ... && e.template hasTag<TAGSFirst>());
            if(hasCompsF && hasTagsF){
                for(std::size_t j{}; j < entities_.size(); ++j){
                    if(i!=j){
                        Entity& e1 = entities_[j];
                        auto hasCompsS = (true && ... && e1.template hasComponent<COMPSSecond>());
                        auto hasTagsS  = (true && ... && e1.template hasTag<TAGSSecond>());

                        if(hasCompsS && hasTagsS){
                            funcion(e, e1);
                        }
                    }
                }
            }
        }
    }*/
    storage components_{};
    std::vector<Entity> entities_{};
};  