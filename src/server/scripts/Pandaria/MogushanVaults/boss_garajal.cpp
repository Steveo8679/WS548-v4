/*
 * Copyright (C) 2012-2013 JadeCore <http://www.pandashan.com/>
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 * Copyright (C) 2006-2009 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "ScriptMgr.h"
#include "CreatureTextMgr.h"
#include "ScriptedCreature.h"
#include "mogu_shan_vaults.h"

enum eSpells
{
    SPELL_BANISHMENT            = 116272,
    SPELL_SOUL_CUT_SUICIDE      = 116278,
    SPELL_SOUL_CUT_DAMAGE       = 117337,

    SPELL_TAP_THE_SPIRIT_WORLD  = 117002, // Only cosmetic
    SPELL_CHANNEL_SPIRIT_TOTEM  = 116833, // Only cosmetic
    SPELL_STRONG_MOJO           = 115964, // Only cosmetic
    SPELL_VOODOO_DOLL_VISUAL    = 122151,
    SPELL_VOODOO_DOLL_SHARE     = 116000,
    SPELL_SUMMON_SPIRIT_TOTEM   = 116174,
    SPELL_COSMETIC_SUMMON       = 118086, // Only cosmetic - Tiki mask
    SPELL_SOUL_CUTTER_COSMETIC  = 120677, // Only cosmetic - Tiki mask too
    SPELL_SUMMON_MINION         = 118087,

    // Spiritual Innervation
    SPIRIT_INNERVATION_DPS      = 117549,
    SPIRIT_INNERVATION_HEALER   = 117543,
    SPELL_SAVE_SOUL             = 120717,

    // attaques ombreuses
    SPELL_RIGHT_CROSS           = 117215,
    SPELL_LEFT_HOOK             = 117218,
    SPELL_HAMMER_FIST           = 117219,
    SPELL_SWEEPING_KICK         = 117222,

    SPELL_FRENESIE              = 117752,

    // Shadowy Minion
    SPELL_SHADOW_BOLT           = 122118,
    SPELL_SPIRITUAL_GRASP       = 118421,

    // Misc
    SPELL_ANIM_VISUAL           = 121475,
    SPELL_CLONE                 = 119051,
    SPELL_CLONE_VISUAL          = 119053,
    SPELL_LIFE_FRAGILE_THREAD   = 116227,
    SPELL_CROSSED_OVER          = 116161, // Todo : virer le summon

    SPELL_FRAIL_SOUL            = 117723, // Heroic
    SPELL_SOUL_EXPLOSION        = 120639,
    SPELL_ROOT_FOR_EVER         = 31366,

    // Death Event
    SPELL_RELEASE_SPIRIT        = 118580, // Summon Gara'jal Ghost
    SPELL_SPIRIT_HOVER          = 75333,
    SPELL_GHOST_VISUAL          = 91218,
    SPELL_SPIRITUAL_PORTAL      = 118581, // Summon Spiritual Portal
    SPELL_VISUAL_PORTAL         = 118661, // Portal Visual

    // Enrage
    SPELL_FINAL_DESTINATION     = 118469,
    SPELL_INSTANTLY_DEATH       = 118443,
};

enum eEvents
{
    EVENT_SECONDARY_ATTACK      = 1,
    EVENT_SUMMON_TOTEM          = 2,
    EVENT_SUMMON_SHADOWY_MINION = 3,
    EVENT_VOODOO_DOLL           = 4,
    EVENT_BANISHMENT            = 5,

    // Shadowy Minion
    EVENT_SHADOW_BOLT           = 6,
    EVENT_SPIRITUAL_GRASP       = 7,
    EVENT_CHECK_PLAYER          = 8,
    EVENT_CLONE_TAKEOFF         = 9,

    // Gara'Jal Ghost
    EVENT_GROWTH                = 10,
    EVENT_TALK_DEATH            = 11,
    EVENT_SUMMON_PORTAL         = 12,
    EVENT_DISAPPEAR             = 13,

    // Enrage
    EVENT_FINAL_DESTINATION     = 14,
    EVENT_SOUL_EXPLOSION        = 15,
};

enum GarajalTalk
{
    TALK_AGGRO  = 0,
    TALK_SLAY   = 1,
    TALK_FRENZY = 2,
};

enum GarajalGhostTalk
{
    TALK_DEATH  = 0
};

// 60143 - Gara'jal the Spiritbinder
class boss_garajal : public CreatureScript
{
    public:
        boss_garajal() : CreatureScript("boss_garajal") {}

        struct boss_garajalAI : public BossAI
        {
            boss_garajalAI(Creature* creature) : BossAI(creature, DATA_GARAJAL_EVENT)
            {
                pInstance = creature->GetInstanceScript();
            }

            InstanceScript* pInstance;
            uint64 voodooTargets[3];

            void Reset()
            {
                _Reset();

                if (pInstance->GetBossState(DATA_GARAJAL_EVENT) != DONE)
                    pInstance->SetBossState(DATA_GARAJAL_EVENT, NOT_STARTED);

                pInstance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

                pInstance->DoRemoveAurasDueToSpellOnPlayers(SPELL_VOODOO_DOLL_VISUAL);
                pInstance->DoRemoveAurasDueToSpellOnPlayers(SPELL_VOODOO_DOLL_SHARE);
                pInstance->DoRemoveAurasDueToSpellOnPlayers(SPELL_CLONE_VISUAL);
                pInstance->DoRemoveAurasDueToSpellOnPlayers(SPELL_CROSSED_OVER);
                pInstance->DoRemoveAurasDueToSpellOnPlayers(SPELL_LIFE_FRAGILE_THREAD);
                pInstance->DoRemoveAurasDueToSpellOnPlayers(SPELL_SOUL_CUT_DAMAGE);
                pInstance->DoRemoveAurasDueToSpellOnPlayers(SPELL_SOUL_CUT_SUICIDE);
                pInstance->DoRemoveAurasDueToSpellOnPlayers(SPELL_BANISHMENT);

                events.Reset();

                me->AddAura(SPELL_STRONG_MOJO, me);
                me->CastSpell(me, SPELL_TAP_THE_SPIRIT_WORLD, true);
            }

            void JustDied(Unit* attacker)
            {
                pInstance->SetBossState(DATA_GARAJAL_EVENT, DONE);
                _JustDied();
                me->CastSpell(me, SPELL_RELEASE_SPIRIT, false);

                events.Reset();
                pInstance->SetBossState(DATA_GARAJAL_EVENT, DONE);
                pInstance->SaveToDB();

                pInstance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

                pInstance->DoRemoveAurasDueToSpellOnPlayers(SPELL_VOODOO_DOLL_VISUAL);
                pInstance->DoRemoveAurasDueToSpellOnPlayers(SPELL_VOODOO_DOLL_SHARE);
                pInstance->DoRemoveAurasDueToSpellOnPlayers(SPELL_CLONE_VISUAL);
                pInstance->DoRemoveAurasDueToSpellOnPlayers(SPELL_CROSSED_OVER);
                pInstance->DoRemoveAurasDueToSpellOnPlayers(SPELL_LIFE_FRAGILE_THREAD);

                if (Creature* lorewalkerCho = GetClosestCreatureWithEntry(me, NPC_LOREWALKER_CHO, 200.0f, true))
                {
                    if (lorewalkerCho->AI())
                    {
                        {
                            lorewalkerCho->AI()->DoAction(ACTION_CONTINUE_ESCORT);
                            lorewalkerCho->AI()->DoAction(ACTION_RUN);
                        }
                    }
                }
            }

            void EnterCombat(Unit* attacker)
            {
                if (!pInstance->CheckRequiredBosses(DATA_GARAJAL_EVENT))
                {
                    EnterEvadeMode();
                    return;
                }

                events.ScheduleEvent(EVENT_SECONDARY_ATTACK,        urand(5000, 10000));
                events.ScheduleEvent(EVENT_SUMMON_TOTEM,            urand(27500, 32500));
                events.ScheduleEvent(EVENT_SUMMON_SHADOWY_MINION,   urand(10000, 15000));
                events.ScheduleEvent(EVENT_BANISHMENT,              90000);
                events.ScheduleEvent(EVENT_VOODOO_DOLL,             2500);
                events.ScheduleEvent(EVENT_FINAL_DESTINATION,       361000); // 6 min & 10s

                pInstance->SetBossState(DATA_GARAJAL_EVENT, IN_PROGRESS);
                pInstance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);
                Talk(TALK_AGGRO);

                me->RemoveAura(SPELL_STRONG_MOJO);
                me->RemoveAura(SPELL_TAP_THE_SPIRIT_WORLD);

                me->InterruptNonMeleeSpells(false);
            }

            void KilledUnit(Unit* who)
            {
                if (who->GetTypeId() == TYPEID_PLAYER)
                    Talk(TALK_SLAY);
            }

            void JustSummoned(Creature* summon)
            {
                summons.Summon(summon);
            }

            void SummonedCreatureDespawn(Creature* summon)
            {
                summons.Despawn(summon);
            }

            void DamageTaken(Unit* attacker, uint32& damage)
            {
                if (!pInstance)
                    return;

                if (!me->HasAura(SPELL_FRENESIE))
                {
                    if (me->HealthBelowPctDamaged(20, damage))
                    {
                        events.CancelEvent(EVENT_SUMMON_TOTEM);
                        me->CastSpell(me, SPELL_FRENESIE, true);
                        me->MonsterTextEmote("Gara'jal casts |cffba2200|Hspell:117752|h[Frenzy]|h|r !", 0, true);
                    }
                }
            }

            void UpdateAI(const uint32 diff)
            {
                if (!UpdateVictim())
                    return;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_SECONDARY_ATTACK:
                        {
                            if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                            {
                                uint32 spellId = RAND(SPELL_RIGHT_CROSS, SPELL_LEFT_HOOK, SPELL_HAMMER_FIST, SPELL_SWEEPING_KICK);
                                me->CastSpell(target, spellId, true);
                            }
                            events.ScheduleEvent(EVENT_SECONDARY_ATTACK, urand(5000, 10000));
                            break;
                        }
                        case EVENT_SUMMON_TOTEM:
                        {
                            me->MonsterTextEmote("Gara'jal summons a Spirit Totem. Use it to Cross Over to the Spirit World !", 0, true);
                            float x = 0.0f, y = 0.0f;
                            GetRandPosFromCenterInDist(4277.08f, 1341.35f, frand(0.0f, 30.0f), x, y);
                            me->SummonCreature(NPC_SPIRIT_TOTEM, x, y, 454.55f, 0.0f, TEMPSUMMON_CORPSE_DESPAWN);
                            events.ScheduleEvent(EVENT_SUMMON_TOTEM,     urand(47500, 52500));
                            break;
                        }
                        case EVENT_SUMMON_SHADOWY_MINION:
                        {
                            float x = 0.0f, y = 0.0f;
                            GetRandPosFromCenterInDist(4277.08f, 1341.35f, frand(0.0f, 30.0f), x, y);
                            me->SummonCreature(NPC_SHADOWY_MINION_REAL, x, y, 454.55f, 0.0f, TEMPSUMMON_CORPSE_DESPAWN);
                            events.ScheduleEvent(EVENT_SUMMON_SHADOWY_MINION,     urand(10000, 15000));
                            break;
                        }
                        case EVENT_VOODOO_DOLL:
                        {
                            pInstance->DoRemoveAurasDueToSpellOnPlayers(SPELL_VOODOO_DOLL_VISUAL);
                            pInstance->DoRemoveAurasDueToSpellOnPlayers(SPELL_VOODOO_DOLL_SHARE);
                            me->MonsterTextEmote("Gara'jal selects random players to become |cffba2200|Hspell:116000|h[Voodoo Dolls]|h|r !", 0, true);

                            int32 mobCount = Is25ManRaid() ? 4 : 2;

                            for (int32 i = 0; i < mobCount; ++i)
                            {
                                if (Unit* target = SelectTarget(i == 0 ? SELECT_TARGET_TOPAGGRO : SELECT_TARGET_RANDOM, 0, 0, true, -SPELL_VOODOO_DOLL_VISUAL))
                                {
                                    voodooTargets[i] = target->GetGUID();
                                    me->MonsterTextEmote("You are a |cffba2200|Hspell:116000|h[Voodoo Doll]|h|r ! Damage you take is copied to the other Voodoo Dolls in your raid !", target->GetGUID(), true);
                                    target->AddAura(SPELL_VOODOO_DOLL_VISUAL, target);
                                }
                            }

                            if (Player* target = sObjectAccessor->GetPlayer(*me, voodooTargets[0]))
                                me->CastSpell(target, SPELL_VOODOO_DOLL_SHARE, true);

                            break;
                        }
                        case EVENT_BANISHMENT:
                        {
                            if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                            {
                                me->AddAura(SPELL_BANISHMENT,       target);
                                me->AddAura(SPELL_SOUL_CUT_SUICIDE, target);
                                me->AddAura(SPELL_SOUL_CUT_DAMAGE,  target);

                                Difficulty difficulty = me->GetMap()->GetDifficulty();
                                uint64 viewerGuid = target->GetGUID();
                                uint8  mobCount   = IsHeroic() ? 3 : 1;

                                for (uint8 i = 0; i < mobCount; ++i)
                                    if (Creature* soulCutter = me->SummonCreature(NPC_SOUL_CUTTER, target->GetPositionX() + 2.0f, target->GetPositionY() + 2.0f, target->GetPositionZ(), 0.0f, TEMPSUMMON_TIMED_DESPAWN, 30000, i == 0 ? viewerGuid: 0))
                                    {
                                        soulCutter->SetPhaseMask(2, true);
                                        soulCutter->AI()->AttackStart(target);
                                        soulCutter->SetInCombatWith(target);
                                        soulCutter->getThreatManager().addThreat(target, 10000.0f);
                                    }

                                me->getThreatManager().resetAllAggro();
                            }

                            pInstance->DoRemoveAurasDueToSpellOnPlayers(SPELL_VOODOO_DOLL_VISUAL);
                            pInstance->DoRemoveAurasDueToSpellOnPlayers(SPELL_VOODOO_DOLL_SHARE);

                            events.ScheduleEvent(EVENT_VOODOO_DOLL, 5000);
                            events.ScheduleEvent(EVENT_BANISHMENT, 90000);
                            break;
                        }
                        case EVENT_FINAL_DESTINATION:
                        {
                            me->CastSpell(me, SPELL_FINAL_DESTINATION, false);
                            Talk(TALK_FRENZY);
                            break;
                        }
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_garajalAI(creature);
        }
};

// 61140 - Gara'jal the Spiritbinder
class mob_garajal_ghost : public CreatureScript
{
    public:
        mob_garajal_ghost() : CreatureScript("mob_garajal_ghost") {}

        struct mob_garajal_ghostAI : public ScriptedAI
        {
            mob_garajal_ghostAI(Creature* creature) : ScriptedAI(creature)
            {
                pInstance = creature->GetInstanceScript();
            }

            EventMap events;
            InstanceScript* pInstance;
            float ghostScale;

            void Reset()
            {
                me->SetReactState(REACT_PASSIVE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                me->AddAura(SPELL_GHOST_VISUAL, me);
                me->AddAura(SPELL_STRONG_MOJO, me);
                me->AddAura(SPELL_SPIRIT_HOVER, me);
                ghostScale = me->GetFloatValue(OBJECT_FIELD_SCALE_X);
                me->SetObjectScale(ghostScale / 10.0f);

                events.ScheduleEvent(EVENT_GROWTH,          200);
                events.ScheduleEvent(EVENT_TALK_DEATH,      4000);
                events.ScheduleEvent(EVENT_SUMMON_PORTAL,   8000);
                events.ScheduleEvent(EVENT_DISAPPEAR,       13000);
            }

            void JustSummoned(Creature* summon)
            {
                if (summon->GetEntry() == NPC_SPIRITUAL_PORTAL)
                {
                    summon->CastSpell(summon, SPELL_VISUAL_PORTAL, true);
                    summon->SetReactState(REACT_PASSIVE);
                    summon->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                }
            }

            void UpdateAI(const uint32 diff)
            {
                if (pInstance->GetBossState(DATA_GARAJAL_EVENT) != DONE)
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_GROWTH:
                            if (me->GetFloatValue(OBJECT_FIELD_SCALE_X) < ghostScale)
                            {
                                me->SetObjectScale(me->GetFloatValue(OBJECT_FIELD_SCALE_X) + (ghostScale / 10.0f));
                                events.ScheduleEvent(EVENT_GROWTH, 200);
                            }
                            break;
                        case EVENT_TALK_DEATH:
                            Talk(TALK_DEATH);
                            break;
                        case EVENT_SUMMON_PORTAL:
                            me->CastSpell(me, SPELL_SPIRITUAL_PORTAL, true);
                            break;
                        case EVENT_DISAPPEAR:
                            if (Creature* spiritualPortal = me->FindNearestCreature(NPC_SPIRITUAL_PORTAL, 50.0f))
                                spiritualPortal->DespawnOrUnsummon(5000);
                            me->DespawnOrUnsummon(500);
                            break;
                        default:
                            break;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new mob_garajal_ghostAI(creature);
        }
};

// 60240 - Spirit Totem
class mob_spirit_totem : public CreatureScript
{
    public:
        mob_spirit_totem() : CreatureScript("mob_spirit_totem") {}

        struct mob_spirit_totemAI : public ScriptedAI
        {
            mob_spirit_totemAI(Creature* creature) : ScriptedAI(creature)
            {
                pInstance = creature->GetInstanceScript();
            }

            InstanceScript* pInstance;
            
            void IsSummonedBy(Unit* summoner)
            {
                me->AddAura(116827, me);
                me->AddAura(SPELL_ROOT_FOR_EVER, me);
                me->SetReactState(REACT_PASSIVE);
            }

            void JustDied(Unit* attacker)
            {
                std::list<Player*> playerList;
                GetPlayerListInGrid(playerList, me, 6.0f);

                uint8 count = 0;
                for (auto player: playerList)
                {
                    if (player->HasAura(SPELL_VOODOO_DOLL_VISUAL) || player->HasAura(SPELL_FRAIL_SOUL))
                        continue;

                    ++count;
                    if (count > 3)
                        break;

                    if (Creature* clone = player->SummonCreature(56405, player->GetPositionX(), player->GetPositionY(), player->GetPositionZ(), player->GetOrientation(),TEMPSUMMON_TIMED_DESPAWN,30000))
                    {
                        player->SetHealth(player->GetHealth() * 0.3);

                        player->CastSpell(player, SPELL_CLONE_VISUAL, true);

                        player->CastSpell(clone,  SPELL_CLONE, true);
                        player->CastSpell(player, SPELL_LIFE_FRAGILE_THREAD, true);
                        player->CastSpell(player, SPELL_CROSSED_OVER, true);

                        if (player->GetRoleForGroup(player->GetSpecializationId(player->GetActiveSpec())) == ROLES_HEALER)
                            player->AddAura(SPIRIT_INNERVATION_HEALER, player);
                        else
                            player->AddAura(SPIRIT_INNERVATION_DPS, player);
                    }
                }
            }

            void UpdateAI(const uint32 diff)
            {
                if (pInstance)
                    if (pInstance->GetBossState(DATA_GARAJAL_EVENT) != IN_PROGRESS)
                        me->DespawnOrUnsummon();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new mob_spirit_totemAI(creature);
        }
};

// 60240 - Clone player
class mob_clone_player : public CreatureScript
{
    public:
        mob_clone_player() : CreatureScript("mob_clone_player") {}

        struct mob_clone_playerAI : public ScriptedAI
        {
            mob_clone_playerAI(Creature* creature) : ScriptedAI(creature)
            {
            }
            
            void IsSummonedBy(Unit* summoner)
            {
               me->AddAura(SPELL_ANIM_VISUAL,me);
               me->SetCanFly(true);
               me->SetDisableGravity(true);
               me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE|UNIT_FLAG_NOT_SELECTABLE);
               events.ScheduleEvent(EVENT_CHECK_PLAYER, 5000);
               events.ScheduleEvent(EVENT_CLONE_TAKEOFF, 3000);
            }

            void UpdateAI(const uint32 diff)
            {
                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        // Spirit World
                        case EVENT_CHECK_PLAYER:
                            if (me->ToTempSummon())
                                if (Unit * Summoner = me->ToTempSummon()->GetSummoner())
                                    if (!Summoner->HasAura(SPELL_CROSSED_OVER))
                                        me->DespawnOrUnsummon();
                            events.ScheduleEvent(EVENT_CHECK_PLAYER, 1000);
                            break;
                        case EVENT_CLONE_TAKEOFF:
                            me->GetMotionMaster()->MoveTakeoff(1, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ() + 9.0f);
                            break;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new mob_clone_playerAI(creature);
        }
};

// 60184 - Shadowy Minion
// 60940 - Shadowy Minion
class mob_shadowy_minion : public CreatureScript
{
    public:
        mob_shadowy_minion() : CreatureScript("mob_shadowy_minion") {}

        struct mob_shadowy_minionAI : public Scripted_NoMovementAI
        {
            mob_shadowy_minionAI(Creature* creature) : Scripted_NoMovementAI(creature)
            {
                pInstance = creature->GetInstanceScript();
            }

            InstanceScript* pInstance;

            uint64 spiritGuid;
            EventMap events;

            void IsSummonedBy(Unit* summoner)
            {
                events.Reset();
                spiritGuid = 0;

                me->setFaction(summoner->getFaction());
                if (me->GetEntry() == NPC_SHADOWY_MINION_REAL)
                {
                    me->CastSpell(me,SPELL_CLONE_VISUAL,true);

                    if (Creature* spirit = me->SummonCreature(NPC_SHADOWY_MINION_SPIRIT, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_DEAD_DESPAWN))
                    {
                        spiritGuid = spirit->GetGUID();
                        spirit->SetPhaseMask(2, true);
                    }

                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE|UNIT_FLAG_NOT_SELECTABLE);
                    events.ScheduleEvent(EVENT_SPIRITUAL_GRASP, urand(2000, 5000));
                }
                else
                    events.ScheduleEvent(EVENT_SHADOW_BOLT, urand(2000, 5000));

                me->AddAura(SPELL_COSMETIC_SUMMON, me);

                DoZoneInCombat();
            }

            void SummonedCreatureDespawn(Creature* summon)
            {
                if (summon->GetEntry() == NPC_SHADOWY_MINION_SPIRIT)
                    me->DespawnOrUnsummon();
            }

            void JustDied(Unit* attacker)
            {
                if (me->GetEntry() == NPC_SHADOWY_MINION_SPIRIT)
                    if (me->ToTempSummon())
                        if (Unit* summoner = me->ToTempSummon()->GetSummoner())
                            if (summoner->ToCreature())
                                summoner->ToCreature()->DespawnOrUnsummon();
            }

            void UpdateAI(const uint32 diff)
            {
                if (pInstance)
                    if (pInstance->GetBossState(DATA_GARAJAL_EVENT) != IN_PROGRESS)
                        me->DespawnOrUnsummon();

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        // Spirit World
                        case EVENT_SHADOW_BOLT:
                        {
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100.0f, true, SPELL_CROSSED_OVER))
                                me->CastSpell(target, SPELL_SHADOW_BOLT, true);
                            else if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100.0f, true, SPELL_SOUL_CUT_SUICIDE))
                                me->CastSpell(target, SPELL_SHADOW_BOLT, true);

                            events.ScheduleEvent(EVENT_SHADOW_BOLT, urand(2000, 3000));
                            break;
                        }
                        // Real World
                        case EVENT_SPIRITUAL_GRASP:
                        {
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100.0f, true, -SPELL_SOUL_CUT_SUICIDE))
                                me->CastSpell(target, SPELL_SPIRITUAL_GRASP, true);
                            else if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100.0f, true, -SPELL_CROSSED_OVER))
                                me->CastSpell(target, SPELL_SPIRITUAL_GRASP, true);

                            events.ScheduleEvent(EVENT_SPIRITUAL_GRASP, urand(5000, 8000));
                            break;
                        }
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new mob_shadowy_minionAI(creature);
        }
};

// 62003 - Severer of Souls
class mob_soul_cutter : public CreatureScript
{
    public:
        mob_soul_cutter() : CreatureScript("mob_soul_cutter") {}

        struct mob_soul_cutterAI : public ScriptedAI
        {
            mob_soul_cutterAI(Creature* creature) : ScriptedAI(creature)
            {
                pInstance = creature->GetInstanceScript();
            }

            InstanceScript* pInstance;

            void Reset()
            {
                if (IsHeroic())
                    events.ScheduleEvent(EVENT_SOUL_EXPLOSION, 30000);
            }

            void JustDied(Unit* attacker)
            {
                std::list<uint64> playerList;
                me->GetMustBeVisibleForPlayersList(playerList);

                for (auto guid: playerList)
                {
                    if (Player* player = ObjectAccessor::FindPlayer(guid))
                    {
                        player->RemoveAurasDueToSpell(SPELL_BANISHMENT);
                        player->RemoveAurasDueToSpell(SPELL_SOUL_CUT_SUICIDE);
                        player->RemoveAurasDueToSpell(SPELL_SOUL_CUT_DAMAGE);
                    }
                }
            }

            void UpdateAI(const uint32 diff)
            {
                if (pInstance)
                    if (pInstance->GetBossState(DATA_GARAJAL_EVENT) != IN_PROGRESS)
                        me->DespawnOrUnsummon();

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch(eventId)
                    {
                        case EVENT_SOUL_EXPLOSION:
                        {
                            std::list<Player*> playerList;
                            GetPlayerListInGrid(playerList, me, 300.0f);
                            bool hasCast = false;

                            while (!hasCast)
                            {
                                if (playerList.empty())
                                    break;

                                for (auto player : playerList)
                                {
                                    if (urand(0, 1))
                                    {
                                        me->CastSpell(player, SPELL_SOUL_EXPLOSION, false);
                                        hasCast = true;
                                    }
                                }
                            }

                            if (IsHeroic())
                                events.ScheduleEvent(EVENT_SOUL_EXPLOSION, 30000);

                            break;
                        }
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new mob_soul_cutterAI(creature);
        }
};

// 60512 - Spirit Totem
// 60513 - Spirit Totem
class mob_spirit_totem_intro : public CreatureScript
{
    public:
        mob_spirit_totem_intro() : CreatureScript("mob_spirit_totem_intro") {}

        struct mob_spirit_totem_introAI : public ScriptedAI
        {
            mob_spirit_totem_introAI(Creature* creature) : ScriptedAI(creature)
            {
                pInstance = creature->GetInstanceScript();
                creature->AddUnitMovementFlag(MOVEMENTFLAG_DISABLE_GRAVITY);
            }

            InstanceScript* pInstance;

            void Reset()
            {
                if (me->GetEntry() == NPC_SPIRIT_TOTEM_SKULL_INTRO)
                    me->SetDisplayId(11686);
            }

            void UpdateAI(const uint32 diff)
            {
                if (Unit* skull = me->FindNearestCreature(NPC_SPIRIT_TOTEM_SKULL_INTRO, 15))
                    if (!skull->HasAura(SPELL_CHANNEL_SPIRIT_TOTEM))
                        me->CastSpell(skull, SPELL_CHANNEL_SPIRIT_TOTEM, true);
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new mob_spirit_totem_introAI(creature);
        }
};

// Soul Back - 120715
class spell_soul_back : public SpellScriptLoader
{
    public:
        spell_soul_back() : SpellScriptLoader("spell_soul_back") { }

        class spell_soul_back_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_soul_back_SpellScript);

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                if (Unit* target = GetHitUnit())
                {
                    // SPELL_LIFE_FRAGILE_THREAD removed by default effect
                    target->RemoveAurasDueToSpell(SPELL_CLONE_VISUAL);
                    target->RemoveAurasDueToSpell(SPELL_CROSSED_OVER);
                    if (target->GetMap()->IsHeroic())
                        target->AddAura(SPELL_FRAIL_SOUL, target);
                    target->SetHealth(target->CountPctFromCurHealth(30));

                    // Todo : Jump le joueur la ou etait son corps
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_soul_back_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_REMOVE_AURA);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_soul_back_SpellScript();
        }
};

// Final Destination - 118469
class spell_final_destination : public SpellScriptLoader
{
    public:
        spell_final_destination() : SpellScriptLoader("spell_final_destination") { }

        class spell_final_destination_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_final_destination_AuraScript);

            void OnTick(constAuraEffectPtr aurEff)
            {
                if (Unit* Garajal = GetCaster())
                {
                    if (InstanceScript* pInstance = Garajal->GetInstanceScript())
                    {
                        Map::PlayerList const &PlayerList = pInstance->instance->GetPlayers();

                        if (!PlayerList.isEmpty())
                            for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                                if (Player* player = i->getSource())
                                    if (player->isAlive())
                                        Garajal->CastSpell(player, SPELL_INSTANTLY_DEATH, true);
                    }
                }
            }

            void Register()
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_final_destination_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_final_destination_AuraScript();
        }
};

// Soul suicide - 116278
class spell_soul_death : public SpellScriptLoader
{
    public:
        spell_soul_death() : SpellScriptLoader("spell_soul_death") { }

        class spell_soul_death_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_soul_death_AuraScript);

            void OnRemove(constAuraEffectPtr aurEff, AuraEffectHandleModes /*mode*/)
            {
                Unit* caster = GetCaster();
                if (caster && caster->isAlive() && GetTargetApplication()->GetRemoveMode() == AURA_REMOVE_BY_EXPIRE)
                    caster->CastSpell(caster,116325,true); // suicide

            }

            void Register()
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_soul_death_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_soul_death_AuraScript();
        }
};

// Life fragile - 116227
class spell_spirit_intervation : public SpellScriptLoader
{
    public:
        spell_spirit_intervation() : SpellScriptLoader("spell_spirit_intervation") { }

        class spell_spirit_intervation_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_spirit_intervation_AuraScript);

            void OnProc(constAuraEffectPtr aurEff, ProcEventInfo& eventInfo)
            {
                PreventDefaultAction();

                if (!GetCaster())
                    return;

                Player* _player = GetCaster()->ToPlayer();
                if (!_player)
                    return;

                if (!eventInfo.GetHealInfo() || !eventInfo.GetHealInfo()->GetHeal())
                    return;

                if (AuraPtr Intervation = _player->GetAura(_player->GetRoleForGroup(_player->GetSpecializationId(_player->GetActiveSpec())) == ROLES_HEALER ? SPIRIT_INNERVATION_HEALER : SPIRIT_INNERVATION_DPS))
                {
                    uint32 gotHeal = eventInfo.GetHealInfo()->GetHeal();
                    int32 amount = ((gotHeal / _player->GetMaxHealth()) * 100) / 5; // every 5% healed by spells is 1% of intervation buff
                    if (Intervation->GetEffect(EFFECT_1)->GetAmount() < 25)
                    {
                        Intervation->GetEffect(EFFECT_0)->ChangeAmount(Intervation->GetEffect(EFFECT_0)->GetAmount()+(amount/100.0f)*_player->GetMaxPower(POWER_MANA));
                        Intervation->GetEffect(EFFECT_1)->ChangeAmount(Intervation->GetEffect(EFFECT_1)->GetAmount()+amount);
                    
                        if (Intervation->GetId() == SPIRIT_INNERVATION_DPS)
                        {
                            Intervation->GetEffect(EFFECT_2)->ChangeAmount(Intervation->GetEffect(EFFECT_2)->GetAmount()+amount);
                            Intervation->GetEffect(EFFECT_3)->ChangeAmount(Intervation->GetEffect(EFFECT_3)->GetAmount()+amount);
                            Intervation->GetEffect(EFFECT_4)->ChangeAmount(Intervation->GetEffect(EFFECT_4)->GetAmount()+amount);
                            Intervation->GetEffect(EFFECT_5)->ChangeAmount(Intervation->GetEffect(EFFECT_5)->GetAmount()+amount);
                        }
                    }

                    if (_player->GetHealth() == _player->GetMaxHealth())
                        if (!_player->HasAura(SPELL_SAVE_SOUL))
                            _player->CastSpell(_player, SPELL_SAVE_SOUL, true);
                }
            }

            void Register()
            {
                OnEffectProc += AuraEffectProcFn(spell_spirit_intervation_AuraScript::OnProc, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_spirit_intervation_AuraScript();
        }
};

void AddSC_boss_garajal()
{
    new boss_garajal();
    new mob_garajal_ghost();
    new mob_spirit_totem();
    new mob_clone_player();
    new mob_shadowy_minion();
    new mob_soul_cutter();
    new mob_spirit_totem_intro();
    new spell_soul_back();
    new spell_final_destination();
    new spell_soul_death();
    new spell_spirit_intervation();
}