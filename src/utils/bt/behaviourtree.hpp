#pragma once

#include "node.hpp"

template <typename T>
concept BTNodeType = std::derived_from<T, BTNode>;

struct BehaviourTree {
	using value_type    = std::unique_ptr<BTNode, BTNode::Deleter>;
	using NodeStorage   = std::vector<value_type>;
	using MemoryStorage = std::unique_ptr<std::byte[]>;
	
	explicit BehaviourTree() {}
	
	BTNodeStatus run(EntityContext* ectx) noexcept{
		BTNodeStatus st {BTNodeStatus::fail};

		if( nodes.size() > 0 )
			st = nodes.back()->run(ectx);

		return st;
	}

	template <BTNodeType NodeType, typename... ParamTypes>
	NodeType& createNode(ParamTypes&&... params){
		// Reservar memoria
		ptr_reserved -= sizeof(NodeType);
		if ( ptr_reserved < mem.get() ){
			ptr_reserved += sizeof(NodeType);
			throw std::bad_alloc{};
		}

		auto* pnode = new (ptr_reserved) NodeType{ std::forward<ParamTypes>(params)... };
		nodes.emplace_back( pnode );
	
		// Return node
		return *pnode;
	}

private:
	std::size_t   mem_size 	    { 1024 };
	MemoryStorage mem 	   		{ std::make_unique<std::byte[]>(mem_size) };
	std::byte*    ptr_reserved  { mem.get() + mem_size };
	NodeStorage   nodes 	    {};
};