#include <typeinfo>
#include <math.h>
#include <cmath>

#include "Punish.h"
#include "../Util/Constants.h"
#include "../Util/Logger.h"
#include "../Chains/SmashAttack.h"
#include "../Chains/Nothing.h"
#include "../Chains/Run.h"
#include "../Chains/Walk.h"
#include "../Chains/Wavedash.h"
#include "../Chains/EdgeAction.h"

Punish::Punish()
{
    m_roll_position = 0;
    m_chain = NULL;
}

Punish::~Punish()
{
    delete m_chain;
}

void Punish::DetermineChain()
{
    //If we're not in a state to interupt, just continue with what we've got going
    if((m_chain != NULL) && (!m_chain->IsInterruptible()))
    {
        m_chain->PressButtons();
        return;
    }

    //If we're hanging on the egde, and they are falling above the stage, stand up
    if(m_state->m_memory->player_one_action == DEAD_FALL &&
        m_state->m_memory->player_two_action == EDGE_HANGING &&
        std::abs(m_state->m_memory->player_one_x) < m_state->getStageEdgeGroundPosition() + .001)
    {
        CreateChain2(EdgeAction, STAND_UP);
        m_chain->PressButtons();
        return;
    }

    bool player_two_is_to_the_left = (m_state->m_memory->player_one_x > m_state->m_memory->player_two_x);

    //If they're rolling, go punish it where they will stop
    if(m_state->isRollingState((ACTION)m_state->m_memory->player_one_action))
    {
        //Figure out where they will stop rolling, only on the first frame
        if(m_roll_position == 0)
        {
            switch(m_state->m_memory->player_one_action)
            {
                case ROLL_FORWARD:
                {
                    if(m_state->m_memory->player_one_facing)
                    {
                        m_roll_position = m_state->m_rollStartPosition + MARTH_ROLL_DISTANCE;
                    }
                    else
                    {
                        m_roll_position = m_state->m_rollStartPosition - MARTH_ROLL_DISTANCE;
                    }
                    break;
                }
                case ROLL_BACKWARD:
                {
                    if(m_state->m_memory->player_one_facing)
                    {
                        m_roll_position = m_state->m_rollStartPosition - MARTH_ROLL_DISTANCE;
                    }
                    else
                    {
                        m_roll_position = m_state->m_rollStartPosition + MARTH_ROLL_DISTANCE;
                    }
                    break;
                }
                case EDGE_ROLL_SLOW:
                {
                    if(m_state->m_memory->player_one_facing)
                    {
                        m_roll_position = m_state->m_rollStartPosition + MARTH_EDGE_ROLL_DISTANCE ;
                    }
                    else
                    {
                        m_roll_position = m_state->m_rollStartPosition - MARTH_EDGE_ROLL_DISTANCE;
                    }
                    break;
                }
                case EDGE_ROLL_QUICK:
                {
                    if(m_state->m_memory->player_one_facing)
                    {
                        m_roll_position = m_state->m_rollStartPosition + MARTH_EDGE_ROLL_DISTANCE;
                    }
                    else
                    {
                        m_roll_position = m_state->m_rollStartPosition - MARTH_EDGE_ROLL_DISTANCE;
                    }
                    break;
                }
                case EDGE_GETUP_QUICK:
                {
                    if(m_state->m_memory->player_one_facing)
                    {
                        m_roll_position = m_state->m_rollStartPosition + MARTH_GETUP_DISTANCE;
                    }
                    else
                    {
                        m_roll_position = m_state->m_rollStartPosition - MARTH_GETUP_DISTANCE;
                    }
                    break;
                }
                case EDGE_GETUP_SLOW:
                {
                    if(m_state->m_memory->player_one_facing)
                    {
                        m_roll_position = m_state->m_rollStartPosition + MARTH_GETUP_DISTANCE;
                    }
                    else
                    {
                        m_roll_position = m_state->m_rollStartPosition - MARTH_GETUP_DISTANCE;
                    }
                    break;
                }
                case FORWARD_TECH:
                {
                    if(m_state->m_memory->player_one_facing)
                    {
                        m_roll_position = m_state->m_rollStartPosition + MARTH_TECHROLL_DISTANCE;
                    }
                    else
                    {
                        m_roll_position = m_state->m_rollStartPosition - MARTH_TECHROLL_DISTANCE;
                    }
                    break;
                }
                case BACKWARD_TECH:
                {
                    if(m_state->m_memory->player_one_facing)
                    {
                        m_roll_position = m_state->m_rollStartPosition - MARTH_TECHROLL_DISTANCE;
                    }
                    else
                    {
                        m_roll_position = m_state->m_rollStartPosition + MARTH_TECHROLL_DISTANCE;
                    }
                    break;
                }
                case GROUND_ROLL_FORWARD:
                case GROUND_ROLL_FORWARD_OTHER:
                {
                    if(m_state->m_memory->player_one_facing)
                    {
                        m_roll_position = m_state->m_rollStartPosition + MARTH_GROUND_FORWARD_ROLL_DISTANCE;
                    }
                    else
                    {
                        m_roll_position = m_state->m_rollStartPosition - MARTH_GROUND_FORWARD_ROLL_DISTANCE;
                    }
                    break;
                }
                case GROUND_ROLL_BACKWARD:
                case GROUND_ROLL_BACKWARD_OTHER:
                {
                    if(m_state->m_memory->player_one_facing)
                    {
                        m_roll_position = m_state->m_rollStartPosition - MARTH_GROUND_BACK_ROLL_DISTANCE;
                    }
                    else
                    {
                        m_roll_position = m_state->m_rollStartPosition + MARTH_GROUND_BACK_ROLL_DISTANCE;
                    }
                    break;
                }
                default:
                {
                    m_roll_position = m_state->m_rollStartPosition;
                    break;
                }
            }

            if(m_roll_position > m_state->getStageEdgeGroundPosition())
            {
                m_roll_position = m_state->getStageEdgeGroundPosition();
            }
            else if (m_roll_position < (-1) * m_state->getStageEdgeGroundPosition())
            {
                m_roll_position = (-1) * m_state->getStageEdgeGroundPosition();
            }
        }

        int frames_left = m_state->totalActionFrames((CHARACTER)m_state->m_memory->player_one_character,
            (ACTION)m_state->m_memory->player_one_action) - m_state->m_memory->player_one_action_frame;

        //If we can't get there in time, just run in
        if(frames_left <= 7)
        {
            CreateChain2(Run, player_two_is_to_the_left);
            m_chain->PressButtons();
            return;
        }

        //Upsmash if we're in range and facing the right way
        //  Factor in sliding during the smash animation
        double distance;
        int frameDelay = 7;

        if(m_state->m_memory->player_two_action == DASHING ||
            m_state->m_memory->player_two_action == RUNNING)
        {
            //We have to jump cancel the grab. So that takes an extra frame
            frameDelay++;
        }
        double slidingAdjustment = m_state->calculateSlideDistance(std::abs(m_state->m_memory->player_two_speed_ground_x_self), frames_left - 1);
        distance = std::abs(std::abs(m_roll_position - m_state->m_memory->player_two_x) - slidingAdjustment);

        Logger::Instance()->Log(INFO, "Trying to punish a roll at position: " + std::to_string(m_roll_position) +
            " with: " + std::to_string(frames_left) + " frames left");

        bool to_the_left = m_roll_position > m_state->m_memory->player_two_x;
        if(frames_left - frameDelay >= 0 &&
            distance < FOX_UPSMASH_RANGE_NEAR &&
            to_the_left == m_state->m_memory->player_two_facing)
        {
            CreateChain3(SmashAttack, SmashAttack::UP, std::max(0, frames_left - frameDelay - 1));
            m_chain->PressButtons();
            return;
        }
        else
        {
            //If the target location is right behind us, just turn around, don't run
            if(distance < FOX_UPSMASH_RANGE_NEAR &&
                to_the_left != m_state->m_memory->player_two_facing)
            {
                CreateChain2(Walk, to_the_left);
                m_chain->PressButtons();
                return;
            }
            else
            {
                CreateChain2(Run, to_the_left);
                m_chain->PressButtons();
                return;
            }
        }
    }

    //Calculate distance between players
    double distance = pow(m_state->m_memory->player_one_x - m_state->m_memory->player_two_x, 2);
    distance += pow(m_state->m_memory->player_one_y - m_state->m_memory->player_two_y, 2);
    distance = sqrt(distance);

    //How many frames do we have until we need to do something?
    int frames_left;
    //Are we before the attack or after?
    if(m_state->m_memory->player_one_action_frame < m_state->lastHitboxFrame((CHARACTER)m_state->m_memory->player_one_character,
        (ACTION)m_state->m_memory->player_one_action))
    {
        //Before
        frames_left = m_state->firstHitboxFrame((CHARACTER)m_state->m_memory->player_one_character,
            (ACTION)m_state->m_memory->player_one_action) - m_state->m_memory->player_one_action_frame - 1;
        Logger::Instance()->Log(INFO, "Frames until first hitbox of the attack: " + std::to_string(frames_left));
    }
    else
    {
        //After
        frames_left = m_state->totalActionFrames((CHARACTER)m_state->m_memory->player_one_character,
           (ACTION)m_state->m_memory->player_one_action) - m_state->m_memory->player_one_action_frame - 1;
       Logger::Instance()->Log(INFO, "Frames left until end of the attack: " + std::to_string(frames_left));
    }

    //If we're in upsmash range, then prepare for attack
    if(m_state->m_memory->player_two_facing == player_two_is_to_the_left && //Facing the right way?
        (distance < FOX_UPSMASH_RANGE ||
        (distance < FOX_UPSMASH_RANGE - 25.5 && (m_state->m_memory->player_two_action == DASHING ||
            m_state->m_memory->player_two_action == RUNNING))))
    {

        int frameDelay = 9; //Frames until the first smash hitbox, plus one for strage startup latency and another for charge lag
        if(m_state->m_memory->player_two_action == DASHING ||
            m_state->m_memory->player_two_action == RUNNING)
        {
            frameDelay += 4;
        }

        //Do we have time to upsmash? Do that.
        if(frames_left > frameDelay)
        {
            //Do two less frames of charging than we could, just to be safe
            CreateChain3(SmashAttack, SmashAttack::UP, std::max(0, frames_left - frameDelay - 1));
            m_chain->PressButtons();
            return;
        }
    }

    //Is it safe to wavedash in after shielding the attack?
    //  Don't wavedash off the edge of the stage
    if(frames_left > 15 &&
        m_state->m_memory->player_two_action == SHIELD_RELEASE &&
        (m_state->getStageEdgeGroundPosition() > std::abs(m_state->m_memory->player_two_x) + 10))
    {
        CreateChain2(Wavedash, player_two_is_to_the_left);
        m_chain->PressButtons();
        return;
    }

    //Default to running in towards the player
    CreateChain2(Run, player_two_is_to_the_left);
    m_chain->PressButtons();
    return;
}
