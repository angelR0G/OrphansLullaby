#include "animation.hpp"
#include "../engine/graphic/resource/animationresource.hpp"
#include "../engine/graphic/resource/shaderresource.hpp"
#include "../engine/graphic/resource/resourceManager.hpp"
#include "../engine/graphic/engine.hpp"

#define	ANIMATION_DISTANCE 350.0
#define ANIMATION_UPDATE_TIME 0.15

AnimationSystem::AnimationSystem(){
}

AnimationSystem::~AnimationSystem(){}

void AnimationSystem::update(EntityMan& EM, float deltaTime) {

	//Search player
	using CListP = MetaP::Typelist<>;
	using TListP = MetaP::Typelist<PlayerTag>;
	Entity* playerEntity = EM.template search<CListP, TListP>()[0];


    using CList = MetaP::Typelist<AnimationComponent>;
    using TList = MetaP::Typelist<>;
    EM.foreach<CList, TList>([&](Entity& e){
		BasicComponent*		basicComp   = &EM.getComponent<BasicComponent>(e);
		BasicComponent*		pbasicComp  = &EM.getComponent<BasicComponent>(*playerEntity);
        AnimationComponent* animComp 	= &EM.getComponent<AnimationComponent>(e);
		RenderComponent* 	renComp 	= &EM.getComponent<RenderComponent>(e);

		WeaponComponent* wComp		{nullptr};
		if(e.hasComponent<WeaponComponent>())
			wComp 			= &EM.getComponent<WeaponComponent>(e);

		auto currentAnimation = animComp->animations[animComp->rAnimation];
		bool animationEnd{false};
		
		// If the animation has changed restart the animation timer
		if(animComp->needChange && currentAnimation.m_CurrentTime >= currentAnimation.maxDuration) {
			currentAnimation.m_CurrentTime = 0.0f; 
			animComp->needChange = false;
		}
		
		// Change to the next frame
		currentAnimation.m_CurrentTime += currentAnimation.framesPerSecond * deltaTime;
		
		// The animation ends
		if(currentAnimation.m_CurrentTime >= currentAnimation.maxDuration) animationEnd = true;

		//Clamp the time
		currentAnimation.m_CurrentTime = fmod(currentAnimation.m_CurrentTime, currentAnimation.maxDuration);
		animComp->animations[animComp->rAnimation] = currentAnimation; 
		
		float distX = basicComp->x - pbasicComp->x;
		float distZ = basicComp->z - pbasicComp->z;

		float distHor {std::sqrt(distX*distX + distZ*distZ)};

		if(distHor <= ANIMATION_DISTANCE){
			CalculateBoneTransform(&currentAnimation.animation->getRootNode(), glm::mat4(1.0f), animComp);
		}else {
			if(animComp->nextUpdate < ANIMATION_UPDATE_TIME){
				animComp->nextUpdate += deltaTime;
			}else{
				animComp->animations[animComp->rAnimation].m_CurrentTime += animComp->nextUpdate;
				CalculateBoneTransform(&currentAnimation.animation->getRootNode(), glm::mat4(1.0f), animComp);
				
				animComp->nextUpdate = 0;
			}
		}

		currentAnimation = animComp->animations[animComp->rAnimation];

		//Send bone transform to the vertex shader
		if(e.hasTag<PlayerTag>()) {
			wComp->activeWeapon->weaponNode->getMesh()->setFinalBoneMatrices(currentAnimation.m_FinalBoneMatrices, animComp->rAnimation);
			activateAnimation(animComp, wComp->activeWeapon->weaponNode->getMesh());
		}
		else{
			renComp->node.getMesh()->setFinalBoneMatrices(currentAnimation.m_FinalBoneMatrices, animComp->rAnimation);
			activateAnimation(animComp, renComp->node.getMesh());
		}

		//Check tactical shotgun reload
		if(e.hasComponent<WeaponComponent>()){
			WeaponComponent* wComp = &EM.getComponent<WeaponComponent>(e);
			if(wComp->activeWeapon->weaponID == 8){
				if(animComp->rAnimation == static_cast<int>(RunningAnimationTactShootgun::FIRST_RELOAD_ANIMATION)){
					animComp->rAnimation = static_cast<int>(RunningAnimationTactShootgun::RELOAD_ANIMATION);
					currentAnimation = animComp->animations[animComp->rAnimation];
				}else if(animComp->rAnimation == static_cast<int>(RunningAnimationTactShootgun::RELOAD_ANIMATION)){
					if(animationEnd)
						++currentAnimation.auxData;
					if(currentAnimation.auxData >= wComp->activeWeapon->magSize){
						animComp->rAnimation = static_cast<int>(RunningAnimationTactShootgun::LAST_RELOAD_ANIMATION);
						currentAnimation = animComp->animations[animComp->rAnimation];
					}
				}
			}
		}

		// The animation has changed
		if(animComp->rAnimation != animComp->prevAnimation) animComp->needChange = true;


		// If a one shoot animation ends return to the prev animation
		if(!currentAnimation.loop && animationEnd){
			currentAnimation.m_CurrentTime = 0.0f;
			animComp->rAnimation = animComp->prevAnimation;
			currentAnimation = animComp->animations[animComp->rAnimation];
			if(e.hasTag<EnemyTag>() && animComp->rAnimation == static_cast<int>(RunningAnimationEnemy::DEATH_ANIMATION)){
				RenderComponent* rComp = &EM.getComponent<RenderComponent>(e);
				rComp->node.setVisible(false);
			}

		}

		//Update animation component
		animComp->animations[animComp->rAnimation] = currentAnimation;
    });

}


void AnimationSystem::CalculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform, AnimationComponent* animCmp) {
		std::string nodeName = node->name;
		glm::mat4 nodeTransform = node->transformation;
		auto currentAnimation = animCmp->animations[animCmp->rAnimation];
		Bone* Bone = currentAnimation.animation->findBone(nodeName);

		if (Bone)
		{
			Bone->update(currentAnimation.m_CurrentTime);
			nodeTransform = Bone->getLocalTransform();
		}

		glm::mat4 globalTransformation = parentTransform * nodeTransform;

		auto boneInfoMap = currentAnimation.animation->getBoneIDMap();
		if (boneInfoMap.find(nodeName) != boneInfoMap.end())
		{
			int index = boneInfoMap[nodeName].id;
			glm::mat4 offset = boneInfoMap[nodeName].offset;
			currentAnimation.m_FinalBoneMatrices[index] = globalTransformation * offset;
		}

		//Update current animation data
		animCmp->animations[animCmp->rAnimation] = currentAnimation;

		for (int i = 0; i < node->childrenCount; i++)
			CalculateBoneTransform(&node->children[i], globalTransformation, animCmp);
}


void AnimationSystem::activateAnimation(AnimationComponent* animCmp, GE::SceneMesh* sMesh){
	auto animations = animCmp->animations;
	for(unsigned int i {0}; i < animations.size(); ++i){
		if((int)i == animCmp->rAnimation){
			//animCmp->animations[i].animation->activateAnimation(true);
			sMesh->setUseAnimation(true, i);
			
		}else{
			//animCmp->animations[i].animation->activateAnimation(false);
			sMesh->setUseAnimation(false, i);
		}
	}
}
