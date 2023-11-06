#pragma once

#include "typelist.hpp"
#include "entitymanager.hpp"
#include "tags.hpp"

using CList = MetaP::Typelist<  BasicComponent, 
                                MovementComponent, 
                                RenderComponent, 
                                InputComponent, 
                                CameraComponent, 
                                AIComponent, 
                                WeaponComponent, 
                                CollisionComponent,
                                SoundComponent,
                                HealthComponent,
                                NavigationComponent,
                                AttackComponent,
                                TriggerComponent,
                                AnimationComponent,
                                AttackStatsComponent>;
using TList  = MetaP::Typelist< PlayerTag,
                                EnemyNormalTag, 
                                EnemyExplosiveTag, 
                                EnemyThrowerTag, 
                                EnemyRugbyTag, 
                                BulletMachineTag,
                                InjectionMachineTag,
                                EffectMachineTag,
                                WeaponMachineTag,
                                WallDoorTag,
                                ExplosiveBarrelTag,
                                TargetBarrelTag,
                                TriggerTag,
                                GoalAreaTag,
                                GoalInteractTag,
                                ReparableTag,
                                MovableObjectiveTag,
                                MarkToDestroyTag,
                                HeadShotMarkTag,
                                MarkToDestroyDelayTag,
                                EnemyTag,
                                BrokenObjectTag,
                                GoalDefendTag,
                                ElectricPanelTag,
                                EnemyExecuteBT,
                                MallNestTag,
                                M1K3Tag,
                                BazookaProjectileTag>;
using EntityMan = EntityManager<CList, TList, 150>;
using Entity = EntityMan::Entity;
