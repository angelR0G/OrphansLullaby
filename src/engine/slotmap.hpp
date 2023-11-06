#pragma once

//#include<array>
#include<cstdint>
#include<stdexcept>
//#include<cassert>

template<typename DataType, std::uint32_t Capacity = 4, typename IndexT = std::uint64_t>
struct Slotmap{
    using value_type = DataType;
    using index_type = IndexT;
    using gen_type = index_type;
    using key_type = struct{ index_type id; index_type gen;};
    using iterator = value_type*;
    using const_iterator = value_type const*;

    //Constructor
    constexpr Slotmap(){ clear(); }

    constexpr std::size_t get_capacity() const noexcept { return Capacity; }
    constexpr std::size_t get_size()     const noexcept { return size_; }
    
    //Adds a slot into the slotmap with a temporal value
    constexpr key_type addSlot(value_type&& vt) {
        auto slotID = allocate();
        auto& slot = index_[slotID];
        //move data
        data_[slot.id] = std::move(vt);
        erase_[slot.id] = slotID;
        key_type key {slot};
        key.id = slotID;

        return key;
    }
    //Adds a new slot into the slotmap
    constexpr key_type addSlot(value_type const& vt) {
        auto keyAdded = addSlot( value_type{ vt } );
        return keyAdded;
    }

    constexpr auto& getDataSlotmap(){
        return data_;
    }
    constexpr auto& getIndexSlotmap(){
        return index_;
    }
    // constexpr DataType& operator[](key_type& key){
    //     assert(key_valid(key));
    //     auto dataId = index_[key.id].id;
    //     return data_[dataId];
    // }

    // constexpr DataType const& operator[](key_type const& key){
    //     assert(key_valid(key));
    //     auto dataId = index_[key.id].id;
    //     return data_[dataId];
    // }

    constexpr DataType const& at(key_type const& key){
        if(!key_valid(key)) throw std::invalid_argument("Invalid key");
        auto dataId = index_.at(key.id).id;
        return data_.at(dataId);
    }
    //Initialize slotmap
    constexpr void clear(){
        freelist_ = 0;
        for(std::size_t i = 0; i < index_.size(); i++){
            index_[i].id = i+1;
        }
    }

    constexpr bool eraseSlot(key_type key) noexcept {
        if(!key_valid(key)) return false; //key invalid - cant erase the slot
        //Erase
        free(key);
        return true;
    }
    //Checks if the key is valid
    //Use exceptions or boolean verification
    constexpr bool key_valid (key_type key) const noexcept{
        bool valid = true;
        if(key.gen > 0){
            if(key.id >= Capacity || index_[key.id].gen != key.gen) 
                valid = false;
        }
        return valid;
    }

    constexpr iterator  begin() noexcept{ return data_.begin(); }
    constexpr iterator    end() noexcept{ return data_.begin() + size_; }
    constexpr const_iterator cbegin() const noexcept{ return data_.cbegin(); }
    constexpr const_iterator   cend() const noexcept{ return data_.cbegin() + size_; }
    
private:
    //Allocate memory in the slotmap for the new element
    constexpr index_type allocate() {
        //Check if the slotmap has space left for the new insertion
        if(size_ >= Capacity) throw std::runtime_error("No space left in the slotmap");
        assert(freelist_ < Capacity);
        
        //Reserve
        auto slotid = freelist_;
        freelist_ = index_[slotid].id;  //Update freelist to the new free space

        //Init slot
        auto& slot = index_[slotid];
        slot.id  = size_;
        slot.gen = generation_;
        //Increase pointers
        ++size_;
        ++generation_;

        return slotid;
    }

    //Free the memory
    constexpr void free(key_type key) noexcept {
        assert(key_valid(key));

        auto& slot = index_[key.id];
        auto slotid = slot.id;           //Save slot id to check if its last
        //Update freelist
        slot.id    = freelist_;
        slot.gen   = generation_;
        freelist_  = key.id;
        
        //Copy data
        if(slotid != size_-1){
            //not last, copy
            data_[slotid]  = data_[size_-1];
            erase_[slotid] = erase_[size_-1];
            index_[erase_[slotid]].id = slotid;
        }
        //Update slotmap size and gen
        --size_;
        ++generation_;
    }

    //char const st1[8] = "#FREEL#";
    index_type freelist_{};
    //char const st2[8] = "#SIZE##";
    index_type size_{};
    //char const st3[8] = "#GENE##";
    gen_type generation_{1};
    //char const st4[16] = "#INDEX#########";
    std::array<key_type, Capacity>   index_{};
    //char const st5[16] = "#DATA##########";
    std::array<value_type, Capacity> data_{};
    //char const st6[16] = "#ERASE#########";
    std::array<index_type, Capacity> erase_{};

};
